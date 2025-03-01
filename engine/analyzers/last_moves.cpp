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

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

#include "../board/board.h"
#include "../board/get_flip.h"
#include "../evaluatedepthone/pattern_evaluator.h"
#include "../evaluatealphabeta/evaluator_alpha_beta.h"
#include "../evaluatealphabeta/evaluator_last_moves.h"

using namespace std;

typedef struct TestCase {
  BitPattern player;
  BitPattern opponent;
  int alpha;
  int beta;
} TestCase;

int main() {
  HashMap<kBitHashMap> hash_map;
  auto evals = LoadEvals();
  EvaluatorAlphaBeta evaluator(&hash_map, PatternEvaluator::Factory(evals.data()));
//  EvaluatorLastMoves evaluator(&hash_map);
  TestCase tests[10000];
// TODO: Fix.
//  ifstream tests_file("testdata/evaluator_last_moves_profile_examples.txt");
//  std::string line;
//  for (int i = 0; i < 10000; ++i) {
//    std::getline(tests_file, line);
//    TestCase test;
//    sscanf(line.c_str(), "%llu %llu %d %d", &test.player, &test.opponent, &test.alpha, &test.beta);
//    tests[i] = test;
//  }
//  tests_file.close();

  int N = 1;
  long long n_positions = 0;
  NVisited tot_n_visited = 0;
  unsigned long long tmp = 12;
  auto start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < N; ++i) {
    for (const auto& test : tests) {
      n_positions++;
//      n_visited = 0;
//      std::cout << Board(test.player, test.opponent).NEmpties() << " " << EvalToEvalLarge(
//          (Eval) test.alpha) << " " << EvalToEvalLarge((Eval) test.beta) << "\n";
      tmp ^= evaluator.Evaluate(test.player, test.opponent, 64, EvalToEvalLarge((Eval) test.alpha),
                                EvalToEvalLarge((Eval) test.beta));
      tot_n_visited += evaluator.GetNVisited();
    }
  }
  auto end = std::chrono::high_resolution_clock::now();
  auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  std::cout << tmp << "\n";


  std::cout << "Visited/sec: " << (int) (1000.0 / (double) millis * (double) tot_n_visited) << "\n";
  std::cout << "Visited/test: " << (double) tot_n_visited / (double) n_positions << "\n";
  std::cout << "Total time: " << (double) millis / 1000.0 << "\n";

  return (EXIT_SUCCESS);
}