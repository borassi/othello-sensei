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

#include "endgame_ffo.h"

#include <array>
#include <iomanip>
#include <iostream>
#include "../hashmap/hash_map.h"
#include "../evaluatedepthone/pattern_evaluator.h"
#include "../evaluatederivative/evaluator_derivative.h"
#include "../evaluatealphabeta/evaluator_alpha_beta.h"
#include "../evaluatealphabeta/evaluator_last_moves.h"
#include "../utils/misc.h"
#include "../utils/parse_flags.h"

int main(int argc, char* argv[]) {
  ParseFlags parse_flags(argc, argv);
  bool approx = parse_flags.GetBoolFlagOrDefault("approx", false);
  int n_threads = parse_flags.GetIntFlagOrDefault("n_threads", 1);
  int start = parse_flags.GetIntFlagOrDefault("start", 41);
  int end = parse_flags.GetIntFlagOrDefault("end", 60);
  PrintSupportedFeatures();
  using std::setw;
  HashMap<kBitHashMap> hash_map;
  auto evals = LoadEvals();
  TreeNodeSupplier tree_node_supplier;
  EvaluatorDerivative evaluator(&tree_node_supplier, &hash_map, PatternEvaluator::Factory(evals.data()));
  std::cout << " num empties        t       nVisPos   nVisPos/sec   nStored n/nodes   eval too_early nextgood nextbad\n";
  srand(42);
  //  std::cout << " num empties        t       nVisPos   nVisPos/sec   nStored n/nodes   n/mid     avgbatch  eval       last5  vquick  quick1  quick2   moves    pass   nodes \n";
  for (int step = 0; step < 10000; ++step) {
  if (rand() % 10 == 0) {
    tree_node_supplier.Reset();
    hash_map.Reset();
  }
  for (int i = start; i <= end; ++i) {
    Board b = GetIthBoard(i);
    ElapsedTime t;
    tree_node_supplier.Reset();
    evaluator.Evaluate(b.Player(), b.Opponent(), -63, 63, 1000000000000L, 1200, n_threads, approx);
    double time = t.Get();
    auto first_position_ptr = evaluator.GetFirstPosition();
    assert(first_position_ptr);
    auto first_position = *first_position_ptr;
    if (parse_flags.GetBoolFlagOrDefault("show_evals", false)) {
      for (int k = -63; k <= 63; k += 2) {
        std::cout << std::setprecision(0) << k << " " << first_position.ProofNumber(k) << " " <<
        first_position.DisproofNumber(k) << "\n";
      }
    }
    const Stats& stats = evaluator.GetStats();
    auto n_visited = stats.GetAll();
    assert(n_visited == first_position.GetNVisited());
    std::cout
        << setw(4) << i << setw(8) << b.NEmpties()
        << std::fixed
        << setw(9) << std::setprecision(4) << time << " "
        << setw(13) << std::setprecision(0) << n_visited
        << setw(14) << static_cast<double>(n_visited / time)
        << setw(10) << tree_node_supplier.NumTreeNodes()
        << setw(8) << n_visited / tree_node_supplier.NumTreeNodes()
//        << setw(13) << std::setprecision(2) << evaluator.AverageBatchSize()
        << setw(6) << std::setprecision(0) << first_position.GetEval()
        << setw(11) << stats.Get(SOLVED_TOO_EARLY)
        << "  ";

    std::cout << std::setprecision(2) << std::setw(7) << stats.Get(NEXT_POSITION_SUCCESS) << std::setw(8) << stats.Get(NEXT_POSITION_FAIL);
    //for (int i = 0; i < NO_TYPE; ++i) {
    //  std::cout << std::setw(7) << std::setprecision(2) << stats.Get((StatsType) i) / (double) n_visited * 100 << "%";
    //}
    std::cout << "\n";
  }
  }
}
