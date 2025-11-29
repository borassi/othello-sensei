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

#include <string>

#include "move_selector.h"
#include "../book/book.h"
#include "../evaluatedepthone/pattern_evaluator.h"
#include "../evaluatederivative/evaluator_derivative.h"
#include "../hashmap/hash_map.h"
#include "../utils/parse_flags.h"
#include "../xot/xot.h"

int main(int argc, char* argv[]) {
  ParseFlags parse_flags(argc, argv);
  std::string book_path = parse_flags.GetFlag("book_path");

  HashMap<kBitHashMap> hash_map;
  auto evals = LoadEvals();
  TreeNodeSupplier tree_node_supplier;
  EvaluatorDerivative evaluator(&tree_node_supplier, &hash_map, PatternEvaluator::Factory(evals.data()));
  Book book(book_path);

  for (double error = 0; error < 100; error += 5) {
    std::cout << error;
    for (int i = 0; i < 5; ++i) {
      double error_black = 0;
      double error_white = 0;
      double expected_error_other = rand() % 70;
      bool black_turn = true;
      Board board;
      while (!IsGameOver(board)) {
        std::vector<Node> children;
        if (HaveToPass(board)) {
          board.PlayMove(0);
          black_turn = !black_turn;
        }
        for (const auto& [child, _]: GetUniqueNextBoardsWithPass(board)) {
          auto node = book.Get(child);
          if (node) {
            children.push_back(*node);
          } else {
            tree_node_supplier.Reset();
            evaluator.Evaluate(child.Player(), child.Opponent(), -63, 63, 10000000, 0.1, 12);
            children.push_back(*evaluator.GetFirstPosition());
          }
        }
        std::vector<const Node*> children_pointer;
        double best_eval = DBL_MAX;
        for (const Node& child : children) {
          children_pointer.push_back(&child);
          best_eval = std::min(best_eval, child.GetEval());
        }
        const Node* best_node = FindNextMoveTotalError(children_pointer, (black_turn == (i % 2 == 0)) ? error : expected_error_other, 999999999);
        board = best_node->ToBoard();
        if (black_turn) {
          error_black += best_node->GetEval() - best_eval;
        } else {
          error_white += best_node->GetEval() - best_eval;
        }
        black_turn = !black_turn;
      }
      std::cout << ", " << ((i % 2 == 0) ? error_black : error_white) << std::flush;
    }
    std::cout << "\n" << std::flush;
  }
}