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

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

#include "evaluator_alpha_beta_optimized.h"
#include "../utils/load_training_set.h"

using namespace std;

constexpr int kNumIterations = 1;


int main(int argc, char** argv) {
  int N = 200;
  unsigned long long tmp = 12;

  std::vector<EvaluatedBoard> boards = load_train_set();
  std::vector<Board> tests;
  while (tests.size() < N) {
    EvaluatedBoard& b = boards[rand() % boards.size()];
    if (b.GetBoard().NEmpties() != 10) {
      continue;
    }
    tests.push_back(b.GetBoard());
  }

  uint64_t visited = 0;
  auto start = std::chrono::high_resolution_clock::now();
  for (const Board& test : tests) {
    tmp += Evaluate(test.Player(), test.Opponent());
  }

  auto end = std::chrono::high_resolution_clock::now();
  double millis = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

  std::cout << tmp << "\n";
  std::cout << "Flips / sec: " << (int) (n_visited * 1000.0 / millis) << "\n";
  std::cout << "Total time: " << (double) millis / 1000 << " sec\n";

  return (EXIT_SUCCESS);
}
