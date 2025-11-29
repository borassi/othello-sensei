/*
 * Copyright 2025 Michele Borassi
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <iomanip>
#include <iostream>
#include <limits>
#include <queue>

#include "../book/book.h"
#include "../hashmap/hash_map.h"
#include "../evaluatedepthone/pattern_evaluator.h"
#include "../evaluatederivative/evaluator_derivative.h"
#include "../evaluatederivative/tree_node.h"
#include "../evaluatealphabeta/evaluator_alpha_beta.h"
#include "../thor/thor.h"
#include "../utils/files.h"
#include "../utils/misc.h"
#include "../utils/parse_flags.h"
#include "visitor.h"

struct BoardWithStats {
  std::string line;
  Board board;
  double error;
  double uncertainty;
  int depth;
  int num_games;

  double GetValue(double new_uncertainty) const {
    // We add - because the priority queue is a max-heap.
    // Old: return -pow(2, -error / 2.0 - depth / 4.0) * new_uncertainty;
    return -pow(2, -error / 2.0 - depth / 8.0) * std::min(0.3, new_uncertainty);
  }

  double GetValue() const {
    return GetValue(uncertainty);
  }

  bool operator<(const BoardWithStats& other) const {
    return
        std::forward_as_tuple(GetValue(), board.Player(), board.Opponent()) <
        std::forward_as_tuple(other.GetValue(), other.board.Player(), other.board.Opponent());
  }
};

typedef std::priority_queue<BoardWithStats> BestPositionsPriorityQueue;

class BookVisitorBestLines : public BookVisitorWithProgress<kBookVersion> {
 public:
  using typename BookVisitorWithProgress::Book;
  using typename BookVisitorWithProgress::BookNode;
  using BookVisitorWithProgress::book_;

  BookVisitorBestLines(const Book& book, const Thor<GameGetterInMemory>& archive, int num_positions, int min_games) :
      BookVisitorWithProgress(book),
      archive_(archive),
      num_positions_(num_positions),
      best_positions_(),
      min_games_(min_games) {}

  BestPositionsPriorityQueue Get() {
    return best_positions_;
  }

 protected:
  using BookVisitor::sequence_;
  using BookVisitor::depth_;

  bool VisitNode(Node& node) {
    int num_thor_games = archive_.GetGames<false>("OthelloQuest", sequence_).num_games;
    if (num_thor_games < min_games_) {
      return false;
    }
    auto [error_black, error_white] = GetErrors();

    BoardWithStats board {
      sequence_.ToString(),
      node.ToBoard(),
      error_black + error_white,
      node.Uncertainty(),
      depth_,
      num_thor_games};
    if (best_positions_.size() >= num_positions_) {
      // Highest value = the worst one in terms of priority, as it's a minus.
      if (board.GetValue() < best_positions_.top().GetValue()) {
        best_positions_.push(std::move(board));
        best_positions_.pop();
      }
    } else {
      best_positions_.push(std::move(board));
    }
    return true;
  }

  void VisitLeaf(Node& node) override {
    VisitNode(node);
  }

  bool PreVisitInternalNode(Node& node) override {
    return VisitNode(node);
  }

 private:
  const Thor<GameGetterInMemory>& archive_;
  const int num_positions_;
  BestPositionsPriorityQueue best_positions_;
  const int min_games_;
};

std::string GetBestBoard(Book<>& book, const std::vector<BoardWithStats>& boards) {
  BoardWithStats best_board = boards[0];
  double best = DBL_MAX;
  std::cout << "Choosing line\n";
  int max_line_length = 0;
  for (const auto& board : boards) {
    max_line_length = std::max(max_line_length, (int) board.line.size());
  }

  for (const auto& board : boards) {
    auto board_in_book = book.Mutable(board.board);
    auto uncertainty = board_in_book->Uncertainty();
    auto value = board.GetValue(uncertainty);
    char format_value_eval[39];
    snprintf(format_value_eval, 39, "%6.2f  %5d  %6.2f  %8.6f  %+2.2f", board.error, board.depth, uncertainty, value, board_in_book->GetEval());
    std::cout << "  "
        << std::setw(max_line_length) << board.line << "  " << format_value_eval << "\n";
    if (value < best) {
      best = value;
      best_board = board;
    }
  }
  return best_board.line;
}

int main(int argc, char* argv[]) {
  ParseFlags parse_flags(argc, argv);
  std::string start_line = parse_flags.GetFlagOrDefault("start", "");
  std::string archive_path = parse_flags.GetFlag("archive_path");
  std::string book_path = parse_flags.GetFlag("book_path");
  int min_games = parse_flags.GetIntFlagOrDefault("min_games", 0);
  NVisited n_descendants_children = parse_flags.GetLongLongFlagOrDefault("n_descendants_children", 50 * 1000 * 1000LL);
  NVisited n_descendants_solve = parse_flags.GetLongLongFlagOrDefault("n_descendants_solve",  4 * 1000 * 1000 * 1000LL);
  int n_threads = parse_flags.GetIntFlagOrDefault("n_threads", (int)std::thread::hardware_concurrency());
  bool force_first_position = parse_flags.GetBoolFlagOrDefault("force_first_position", false);

  fs::create_directories(book_path);
  Book<> book(book_path);
  HashMap<kBitHashMap> hash_map;
  auto evals = LoadEvals();
  TreeNodeSupplier tree_node_supplier;
  std::array<std::unique_ptr<EvaluatorDerivative>, 64> evaluators;
  for (int i = 0; i < evaluators.size(); ++i) {
    evaluators[i] = std::make_unique<EvaluatorDerivative>(
        &tree_node_supplier, &hash_map,
        PatternEvaluator::Factory(evals.data()),
        static_cast<uint8_t>(i));
  }
  if (!book.Get(Board(start_line))) {
    if (!force_first_position) {
      std::cout << "The node '" << start_line
                << "' is not in the library. Run with --force_first_position, or choose a different start.\n";
      return 1;
    }
    std::unique_ptr<TreeNode> t(new TreeNode());
    t->Reset(Board(start_line), 1, 0);
    t->SetLeafEval(0, 1);
    t->UpdateLeafWeakLowerUpper(-63, 63);
    t->AddDescendants(1);
    book.Add(*t);
  }
  Thor<GameGetterInMemory> archive(archive_path, "");
  std::vector<BoardWithStats> best_boards;

  for (int step = 0; true; ++step) {
    std::cout << "Step "<< step << "\n";
    if (step % 300 == 0) {
      BookVisitorBestLines visitor(book, archive, 90, min_games);
      visitor.VisitString(start_line);
      BestPositionsPriorityQueue best_boards_queue = visitor.Get();
      best_boards.clear();
      while (!best_boards_queue.empty()) {
        best_boards.emplace_back(best_boards_queue.top());
        best_boards_queue.pop();
      }
    }
    std::string current_line = GetBestBoard(book, best_boards);
    Eval last_eval_goal = kLessThenMinEval;
    std::vector<Board> current_line_boards;
    Board start_board(current_line, &current_line_boards);
    ElapsedTime t;
    std::vector<Book<>::BookNode*> current_line_in_book;
    for (const Board& current_line_board : current_line_boards) {
      auto board_in_book = book.Mutable(current_line_board);
      if (board_in_book) {
        current_line_in_book.push_back(board_in_book);
      } else {
        current_line_in_book.clear();
      }
    }
    Book<>::BookNode* start = book.Mutable(start_board);
    assert(start);
    if (start->Node::IsSolved()) {
      std::cout << "Solved the position!\n";
      break;
    }
    tree_node_supplier.Reset();
    NVisited n_visited = 0;
    // TODO: Avoid duplication with BestDescendant.
    Eval eval_goal = start->NextPositionEvalGoal(0, 1, start->SolveProbability(-63, 63) > 0.05 ? kLessThenMinEval : last_eval_goal);
    std::cout
        << "Expanding line:        " << current_line << "\n"
        << "Positions:             " << PrettyPrintDouble((double) book.Size()) << "\n"
        << "Descendants of start:  " << PrettyPrintDouble((double) start->GetNVisited()) << "\n"
        << "Evaluation of start:   " << std::setprecision(3) << start->GetEval() << "\n"
        << "Advancement            " << (int) start->Advancement(-64, 64) << "\n"
        << "Missing:               " << PrettyPrintDouble(start->RemainingWork(-63, 63)) << "\n"
        << "Eval goal:             " << (int) eval_goal << "\n";

    auto leaf_ptr = LeafToUpdate<Book<>::BookNode>::BestDescendant(start, 0, last_eval_goal, current_line_in_book);
    assert(leaf_ptr);
    auto leaf = *leaf_ptr;
    last_eval_goal = eval_goal;
    bool solved = false;
    int alpha = (int) leaf.Alpha();
    int beta = (int) leaf.Beta();
    auto node = leaf.Leaf();
    auto remaining_work = node->RemainingWork((Eval) alpha, (Eval)beta);
    assert(leaf.Alpha() <= leaf.EvalGoal() && leaf.EvalGoal() <= leaf.Beta());
    std::cout
        << "Remaining work:        " << PrettyPrintDouble(remaining_work) << "\n"
        << "Board:\n" << Indent(leaf.Leaf()->ToBoard().ToString(), "                       ");
    if (remaining_work < (double) n_descendants_solve) {
      std::cout << "Solving with alpha=" << alpha << " beta=" << beta << "\n";
      auto evaluator = evaluators[0].get();
      evaluator->Evaluate(node->Player(), node->Opponent(), (Eval)alpha, (Eval) beta, 5 * n_descendants_solve, 240, n_threads, false);
      auto result_ptr = evaluator->GetFirstPosition();
      assert(result_ptr);
      const auto& result = *result_ptr;
      auto lower = result.Lower();
      auto upper = result.Upper();
      node->SetLower(lower);
      node->SetUpper(upper);
      n_visited += result.GetNVisited();
      solved = lower != -64 || upper != 64;
      if (solved) {
        std::cout << "Solved: " << (int) result.Lower() << " <= eval <= " << (int) result.Upper() << "\n";
      } else {
        std::cout << "Did not solve it in time\n";
      }
    } else {
      std::cout << "Too early to solve\n";
    }
    if (!solved) {
      std::cout << "Adding children\n";
      std::vector<Node> children;
      int i = 0;
      for (auto child_flip : GetUniqueNextBoardsWithPass(node->ToBoard())) {
        auto child = child_flip.first;
        auto evaluator = evaluators[++i].get();
        evaluator->Evaluate(
            child.Player(), child.Opponent(), -63, 63, n_descendants_children / 100, 300, n_threads);
        auto early_result = evaluator->GetFirstPosition();
        assert(early_result);
        auto remaining_work = std::max((NVisited) 1000, (NVisited) early_result->RemainingWork((Eval)alpha, (Eval) beta));
        evaluator->ContinueEvaluate(
            std::min(n_descendants_children, (NVisited) remaining_work / 30), 300, n_threads);
        auto result = evaluator->GetFirstPosition();
        assert(result);
        children.push_back(*result);
        n_visited += result->GetNVisited();
      }
      book.AddChildren(node->ToBoard(), children);
    }
    leaf.Finalize(n_visited);
    book.Commit();
    double time = t.Get();
    std::cout << "Position:              " << PrettyPrintDouble((double) n_visited) << "\n";
    std::cout << "Time:                  " << PrettyPrintDouble(time) << " sec\n";
    std::cout << "Positions / sec:       " << PrettyPrintDouble((double)n_visited / time) << "\n";
    std::cout << "\n";
  }
}
