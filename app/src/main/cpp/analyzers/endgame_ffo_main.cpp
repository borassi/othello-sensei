/*
 * Copyright 2021 Google LLC
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
  using std::setw;
  HashMap hash_map;
  auto evals = LoadEvals();
  TreeNodeSupplier tree_node_supplier;
  EvaluatorDerivative evaluator(&tree_node_supplier, &hash_map, PatternEvaluator::Factory(evals.data()), n_threads);
  std::cout << " num empties        t       nVisPos   nVisPos/sec   nStored n/nodes   n/mid     avgbatch  eval       last5  vquick  quick1  quick2   moves    pass   nodes \n";
  for (int i = 41; i <= 60; i++) {
    Board b = GetIthBoard(i);
    ElapsedTime t;
    tree_node_supplier.Reset();
    evaluator.Evaluate(b.GetPlayer(), b.GetOpponent(), -63, 63, 1000000000000L, 1200, approx);
    double time = t.Get();
    auto first_position = evaluator.GetFirstPosition();
    const Stats& stats = evaluator.GetStats();
    auto n_visited = stats.GetAll();
    std::cout
        << setw(4) << i << setw(8) << b.NEmpties()
        << std::fixed
        << setw(9) << std::setprecision(4) << time << " "
        << setw(13) << std::setprecision(0) << n_visited
        << setw(14) << static_cast<double>(n_visited / time)
        << setw(10) << tree_node_supplier.NumTreeNodes()
        << setw(8) << n_visited / tree_node_supplier.NumTreeNodes()
        << setw(8) << 0 // n_visited / evaluator.GetStats()
        << setw(13) << std::setprecision(2) << evaluator.AverageBatchSize()
        << setw(6) << std::setprecision(0) << first_position->GetEval()
        << "    ";

    for (int i = 0; i < NO_TYPE; ++i) {
        std::cout << std::setw(7) << std::setprecision(2) << stats.Get((StatsType) i) / (double) n_visited * 100 << "%";
    }
    std::cout << "\n";
  }
}