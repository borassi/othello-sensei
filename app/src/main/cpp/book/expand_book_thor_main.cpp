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

#include <filesystem>
#include <limits>

#include "book.h"
#include "../board/board.h"
#include "../board/get_flip.h"
#include "../hashmap/hash_map.h"
#include "../evaluatedepthone/pattern_evaluator.h"
#include "../evaluatederivative/evaluator_derivative.h"
#include "../evaluatealphabeta/evaluator_alpha_beta.h"
#include "../utils/assets.h"
#include "../utils/misc.h"
#include "../utils/load_thor.h"
#include "../utils/parse_flags.h"

namespace fs = std::filesystem;

class ExpandBookThorMain {
 public:
  ExpandBookThorMain(const std::string& filepath) :
      book_(filepath),
      evals_(LoadEvals()),
      hash_map_(),
      tree_node_supplier_(),
      evaluators_() {
    for (int i = 0; i < evaluators_.size(); ++i) {
      evaluators_[i] = std::make_unique<EvaluatorDerivative>(
          &tree_node_supplier_, &hash_map_,
          PatternEvaluator::Factory(evals_.data()),
          std::thread::hardware_concurrency(),
          static_cast<u_int8_t>(i));
    }
  }

  void AddChildrenToBook(const std::vector<Board>& ancestors, const Board& father, NVisited n_descendants_children, NVisited n_descendants_solve) {
    std::vector<Book<>::BookNode*> ancestors_in_book;

    for (const Board& ancestor : ancestors) {
      auto ancestor_in_book = book_.Mutable(ancestor);
      assert(ancestor_in_book);
      ancestors_in_book.push_back(ancestor_in_book.value());
    }
    Book<>::BookNode* father_in_book = book_.Mutable(father).value();
    ancestors_in_book.push_back(father_in_book);
    if (father_in_book->Node::IsSolved()) {
      std::cout << "Solved the position!\n";
      return;
    }
    tree_node_supplier_.Reset();
    NVisited n_visited = 0;

    auto leaf = LeafToUpdate<Book<>::BookNode>::Leaf(ancestors_in_book);
    std::cout << "Expanding board:\n" << leaf.Leaf()->ToBoard();
    bool solved = false;

    if (father_in_book->RemainingWork(-63, 63) < n_descendants_solve) {
      std::cout << "Solving\n";
      auto evaluator = evaluators_[0].get();
      evaluator->Evaluate(father_in_book->Player(), father_in_book->Opponent(), -63, 63, 5 * n_descendants_solve, 600, false);
      auto result = evaluator->GetFirstPosition();
      auto lower = result.Lower();
      auto upper = result.Upper();
      father_in_book->SetLower(lower);
      father_in_book->SetUpper(upper);
      n_visited += result.GetNVisited();
      solved = lower != -64 || upper != 64;
      if (solved) {
        std::cout << "Solved: " << (int) result.Lower() << " <= eval <= " << (int) result.Upper() << "\n";
      } else {
        std::cout << "Did not solve it in time\n";
      }
    }
    if (!solved) {
      std::cout << "Adding children\n";
      std::vector<Node> children;
      int i = 0;
      for (auto [child, unused_flip] : GetUniqueNextBoardsWithPass(father_in_book->ToBoard())) {
        auto evaluator = evaluators_[++i].get();
        evaluator->Evaluate(
            child.Player(), child.Opponent(), -63, 63, n_descendants_children, 600);
        children.push_back(evaluator->GetFirstPosition());
        n_visited += evaluator->GetFirstPosition().GetNVisited();
      }
      book_.AddChildren(father_in_book->ToBoard(), children);
    }
    leaf.Finalize(n_visited);
  }

  void AddGame(std::vector<Square> game, NVisited n_descendants_children, NVisited n_descendants_solve, double max_error) {
    Board b;
    Board old_b;
    std::vector<Board> old_boards;
    double error = 0;
    for (Square move : game) {
      std::cout << "\n";
      if (move == kNoSquare) {
        return;
      }
      old_boards.push_back(b);
      b.PlayMove(GetFlip(move, b.Player(), b.Opponent()));
      auto old_b_in_book = book_.Get(old_b).value();
      auto b_in_book = book_.Get(b);
      if (old_b_in_book.IsSolved() || b_in_book.value().IsSolved()) {
        std::cout << "Solved!\n";
        break;
      }
      error += old_b_in_book.GetEval() - (-b_in_book.value().GetEval());
      old_b = b;
      std::cout << "Error = " << error << "\n";
      if (error > max_error) {
        std::cout << "Stopping, error too large\n";
        break;
      }
      if (b_in_book.value().IsLeaf()) {
        AddChildrenToBook(old_boards, b, n_descendants_children, n_descendants_solve);
      }
      if (HaveToPass(b.Player(), b.Opponent())) {
        old_boards.push_back(b);
        b.PlayMove(0);
        auto old_b_in_book = book_.Get(old_b).value();
        auto b_in_book = book_.Get(b);
        if (b_in_book.value().IsLeaf()) {
          AddChildrenToBook(old_boards, b, n_descendants_children, n_descendants_solve);
        }
        old_b = b;
      }
    }
    book_.Commit();
  }

 private:
  Book<> book_;
  const std::vector<int8_t> evals_;
  HashMap<kBitHashMap> hash_map_;
  TreeNodeSupplier tree_node_supplier_;
  std::array<std::unique_ptr<EvaluatorDerivative>, 64> evaluators_;
};

int main(int argc, char* argv[]) {
  ParseFlags parse_flags(argc, argv);
  std::string filepath = parse_flags.GetFlagOrDefault("folder", kBookFilepath);
  NVisited n_descendants_children = parse_flags.GetIntFlagOrDefault("n_descendants_children", 50 * 1000 * 1000UL);
  NVisited n_descendants_solve = parse_flags.GetIntFlagOrDefault("n_descendants_solve",  2 * 1000 * 1000 * 1000UL);
  int max_error = parse_flags.GetIntFlagOrDefault("max_error",  20);

  ExpandBookThorMain expander(filepath);
  for (int year = 2023; year >= 1977; --year) {
    auto games = GetGames(kAssetFilepath + std::string("thor/WTH_" + std::to_string(year) + ".wtb"));
    for (int i = 0; i < games.size(); ++i) {
      const auto& game = games[i];
      expander.AddGame(game, n_descendants_children, n_descendants_solve, max_error);
      std::cout << "\n\n\nDONE GAME " << i << "/" << games.size() << " OF YEAR " << year << ".\n\n\n";
    }
  }
}
