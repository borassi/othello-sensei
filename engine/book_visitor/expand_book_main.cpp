/*
 * Copyright 2023 Michele Borassi
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

#include "../book/book.h"
#include "../hashmap/hash_map.h"
#include "../evaluatedepthone/pattern_evaluator.h"
#include "../evaluatederivative/evaluator_derivative.h"
#include "../evaluatealphabeta/evaluator_alpha_beta.h"
#include "../utils/files.h"
#include "../utils/misc.h"
#include "../utils/parse_flags.h"

// TODO: Fix proof / disproof number at end.
// TODO: Test Google cloud.

std::string GetBestLine(Book<>& book, const std::vector<std::string>& lines) {
  std::string best_line = lines[0];
  double best = -DBL_MAX;
  std::cout << "Choosing line\n";
  int max_line_length = 0;
  for (const auto& line : lines) {
    max_line_length = std::max(max_line_length, (int) line.size());
  }

  for (const auto& line : lines) {
    auto board_in_book = book.Mutable(Board(line));
    auto value = board_in_book->Uncertainty();
    char format_value_eval[13];
    snprintf(format_value_eval, 13, "%.2f  %+.2f", value, board_in_book->GetEval());
    std::cout << "  "
        << std::setw(max_line_length) << line << "  " << format_value_eval << "\n";
    if (value > best) {
      best = value;
      best_line = line;
    }
  }
  return best_line;
}

int main(int argc, char* argv[]) {
  ParseFlags parse_flags(argc, argv);
  std::string start_lines_str = parse_flags.GetFlagOrDefault("start", "");
//  std::string start_line = parse_flags.GetFlagOrDefault("start", "e6f4c3c4d3d6e3c2b3d2c5f5f3f6e1d1e2f1g4g3g5h5f2h4c7g6e7a4a3a2");
  std::string filepath = parse_flags.GetFlagOrDefault("folder", kBookFilepath);
  NVisited n_descendants_children = parse_flags.GetLongLongFlagOrDefault("n_descendants_children", 50 * 1000 * 1000LL);
  NVisited n_descendants_solve = parse_flags.GetLongLongFlagOrDefault("n_descendants_solve",  4 * 1000 * 1000 * 1000LL);
  int n_threads = parse_flags.GetIntFlagOrDefault("n_threads", std::thread::hardware_concurrency());
  bool force_first_position = parse_flags.GetBoolFlagOrDefault("force_first_position", false);

  std::vector<std::string> start_lines = Split(start_lines_str, ',');
  if (start_lines.empty()) {
    start_lines.push_back("");
  }
  fs::create_directories(filepath);
  Book<> book(filepath);
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
  for (const auto& start_line : start_lines) {
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
  }

  Eval last_eval_goal = kLessThenMinEval;
  std::string last_start_line = "";
  while (true) {
    std::string start_line = GetBestLine(book, start_lines);
    std::vector<Board> start_line_boards;
    Board start_board(start_line, &start_line_boards);
    ElapsedTime t;
    std::vector<Book<>::BookNode*> start_line_in_book;
    for (const Board& start_line_board : start_line_boards) {
      auto board_in_book = book.Mutable(start_line_board);
      if (board_in_book) {
        start_line_in_book.push_back(board_in_book);
      } else {
        start_line_in_book.clear();
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
    Eval eval_goal = start->NextPositionEvalGoal(0, 1, start->SolveProbability(-63, 63) > 0.05 || start_line != last_start_line ? kLessThenMinEval : last_eval_goal);
    std::cout
        << "Expanding line:        " << start_line << "\n"
        << "Positions:             " << PrettyPrintDouble((double) book.Size()) << "\n"
        << "Descendants of start:  " << PrettyPrintDouble((double) start->GetNVisited()) << "\n"
        << "Evaluation of start:   " << std::setprecision(3) << start->GetEval() << "\n"
        << "Advancement            " << (int) start->Advancement(-64, 64) << "\n"
        << "Missing:               " << PrettyPrintDouble(start->RemainingWork(-63, 63)) << "\n"
        << "Eval goal:             " << (int) eval_goal << "\n";

    auto leaf_ptr = LeafToUpdate<Book<>::BookNode>::BestDescendant(start, 0, last_eval_goal, start_line_in_book);
    assert(leaf_ptr);
    auto leaf = *leaf_ptr;
    last_eval_goal = eval_goal;
    last_start_line = start_line;
    bool solved = false;
    int alpha = leaf.Alpha();
    int beta = leaf.Beta();
    auto node = leaf.Leaf();
    auto remaining_work = node->RemainingWork(alpha, beta);
    assert(leaf.Alpha() <= leaf.EvalGoal() && leaf.EvalGoal() <= leaf.Beta());
    std::cout
        << "Remaining work:        " << PrettyPrintDouble(remaining_work) << "\n"
        << "Board:\n" << Indent(leaf.Leaf()->ToBoard().ToString(), "                       ");
    if (remaining_work < n_descendants_solve) {
      std::cout << "Solving with alpha=" << alpha << " beta=" << beta << "\n";
      auto evaluator = evaluators[0].get();
      evaluator->Evaluate(node->Player(), node->Opponent(), alpha, beta, 5 * n_descendants_solve, 240, n_threads, false);
      auto result_ptr = evaluator->GetFirstPosition();
      assert(result_ptr);
      auto result = *result_ptr;
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
        auto remaining_work = std::max((NVisited) 1000, (NVisited) early_result->RemainingWork(alpha, beta));
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
    std::cout << "Positions / sec:       " << PrettyPrintDouble(n_visited / time) << "\n";
    std::cout << "\n";
  }
}
