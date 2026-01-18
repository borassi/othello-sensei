/*
 * Copyright 2026 Michele Borassi
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

// Usage:
//
// cmake -S engine -B build -DANDROID=FALSE -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-mbmi2 -mpopcnt" \
//   && cmake --build build --parallel=12 --target=evaluate_cli_main \
//   && ./build/analyzers/evaluate_cli_main --board=F5D6C3D3C4F4F6G5E6D7C7G6E3C6C5B6F7F3G4B5E7F2B4H5E2F8G3H4F1E8 --n_threads=16
//
// cmake -S engine -B build -DANDROID=FALSE -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-mbmi2 -mpopcnt" \
//   && cmake --build build --parallel=12 --target=evaluate_cli_main \
//   && ./build/analyzers/evaluate_cli_main --board="..................OOOX....XOXO...OOOXXOO.OOXXXXO..XOOX.....XXXX." --n_threads=16

#include "endgame_ffo.h"

#include <array>
#include <iomanip>
#include <iostream>
#include "../board/board.h"
#include "../board/sequence.h"
#include "../hashmap/hash_map.h"
#include "../evaluatedepthone/pattern_evaluator.h"
#include "../evaluatederivative/evaluator_derivative.h"
#include "../evaluatealphabeta/evaluator_alpha_beta.h"
#include "../utils/misc.h"
#include "../utils/parse_flags.h"

int main(int argc, char* argv[]) {
  ElapsedTime t_setup;
  std::cout << "\nStarting setup...\n";
  ParseFlags parse_flags(argc, argv);
  int n_threads = parse_flags.GetIntFlagOrDefault("n_threads", 1);
  std::string board = parse_flags.GetFlag("board");
  PrintSupportedFeatures();
  using std::setw;
  HashMap<kBitHashMap> hash_map;
  auto evals = LoadEvals();
  TreeNodeSupplier tree_node_supplier;
  EvaluatorDerivative evaluator(&tree_node_supplier, &hash_map, PatternEvaluator::Factory(evals.data()));
  Board b;
  Sequence sequence = Sequence::ParseFromString(board);
  if (sequence.Size() != 0) {
    b = sequence.ToBoard();
  } else {
    auto b_optional = Board::FromString(board);
    if (b_optional) {
      b = b_optional->first;
    } else {
      std::cout << "\nFAILED: Not a move list or board " << board << "\n";
      return 1;
    }
  }
  std::cout << "\nEvaluating\n" << b << "Empties: " << b.NEmpties() << "\n";
  std::cout << "Finished setup after " << std::setprecision(2) << t_setup.Get() << " seconds..." << "\n\n" << std::flush;
  ElapsedTime t;
  evaluator.Evaluate(b.Player(), b.Opponent(), -63, 63, 1000000000000L, 1200, n_threads, false);
  double time = t.Get();
  auto first_position_ptr = evaluator.GetFirstPosition();
  assert(first_position_ptr);
  auto first_position = *first_position_ptr;
  const Stats& stats = evaluator.GetStats();
  auto n_visited = stats.GetAll();
  std::cout
      << std::fixed
      << "Time (sec):     " << setw(9) << std::setprecision(4) << time << "\n"
      << "Positions:  " << setw(13) << std::setprecision(0) << n_visited << "\n"
      << "Pos/sec:   " << setw(14) << static_cast<double>(n_visited / time) << "\n"
      << "Eval:              " << setw(6) << std::setprecision(0) << first_position.GetEval() << "\n";
}
