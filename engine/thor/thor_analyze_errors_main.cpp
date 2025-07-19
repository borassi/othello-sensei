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

#include "fix_xot_source.h"
#include "thor.h"
#include "../book/book.h"
#include "../evaluatedepthone/pattern_evaluator.h"
#include "../evaluatederivative/evaluator_derivative.h"
#include "../hashmap/hash_map.h"
#include "../utils/parse_flags.h"
#include "../xot/xot.h"

bool IsAfterPass(const Board& last_board, const Board& board) {
  for (const Board& child : GetNextBoardsWithPass(last_board)) {
    if (board == child) {
      return false;
    } else if (board.Opponent() == child.Player() && board.Player() == child.Opponent()) {
      return true;
    }
  }
  assert(false);
  return true;
}

// Usage:
// $ cmake -S engine -B build -DANDROID=FALSE -DCMAKE_BUILD_TYPE=RelWithDebInfo \
// -DLIBRARY_OUTPUT_DIRECTORY=build && \
// cmake --build build --parallel=12 --target=thor_create_extra_files && \
// ./build/thor/thor_create_extra_files --path=assets/archive
int main(int argc, char* argv[]) {
  ParseFlags parse_flags(argc, argv);
  std::string input_path = parse_flags.GetFlag("input_path");
  std::string xot_path = parse_flags.GetFlag("xot_path");
  std::string book_path = parse_flags.GetFlag("book_path");
  std::string output_path = parse_flags.GetFlag("output_path");

  std::ofstream output_file;
  output_file.open(output_path);

  HashMap<kBitHashMap> hash_map;
  Thor<GameGetterInMemory> thor(input_path, "/tmp/saved_files.txt");
  auto evals = LoadEvals();
  TreeNodeSupplier tree_node_supplier;
  EvaluatorDerivative evaluator(&tree_node_supplier, &hash_map, PatternEvaluator::Factory(evals.data()));
  Book book(book_path);
  XOT xot(LoadTextFile(xot_path));

  auto rng = std::default_random_engine {};
  rng.seed(42);

  std::vector<Game> games = thor.GetAllGames();
  std::shuffle(games.begin(), games.end(), rng);
  int done = 0;
  output_file << "Sequence";
  for (int i = 0; i < 61; ++i) {
    output_file << ", " << i;
  }
  for (const auto& game : games) {
    Sequence moves = game.Moves();
    // NOTE: We might skip some non-XOT openings, no big deal.
    if (moves.Size() < 60 || xot.IsInListPrefix(moves)) {
      continue;
    }
    double last_eval = 0;
    output_file << "\n" << moves;

    Board last_board;
    for (const Board& board : moves.ToBoards()) {
      double eval;
      auto node = book.Get(board.Player(), board.Opponent());
      if (node) {
        eval = node->GetEval();
      } else {
        tree_node_supplier.Reset();
        evaluator.Evaluate(board.Player(), board.Opponent(), -63, 63, 10000000, 0.1, 12);
        eval = evaluator.GetFirstPosition()->GetEval();
      }
      bool is_after_pass = last_board != Board() && IsAfterPass(last_board, board);
      output_file << ", " << last_eval + eval * (is_after_pass ? -1 : 1);
      last_eval = eval;
      last_board = board;
    }
    output_file.flush();
    std::cout << "Done " << ++done << "\n";
  }
}