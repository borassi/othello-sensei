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

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

#include "board.h"
#include "evaluator_last_moves.h"
#include "get_flip.h"

using namespace std;

typedef struct TestCase {
  BitPattern player;
  BitPattern opponent;
  int alpha;
  int beta;
} TestCase;

int main(int argc, char** argv) {
  TestCase tests[10000];
  ifstream tests_file("testdata/evaluator_last_moves_profile_examples.txt");
  std::string line;
  for (int i = 0; i < 10000; ++i) {
    std::getline(tests_file, line);
    TestCase test;
    sscanf(line.c_str(), "%lld %lld %d %d", &test.player, &test.opponent, &test.alpha, &test.beta);
    tests[i] = test;
  }
  tests_file.close();

  int N = 100;
  long long n_positions = 0;
  int n_visited = 0;
  long long tot_n_visited = 0;
  unsigned long long tmp = 12;
  auto start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < N; ++i) {
    for (int i = 0; i < 10000; ++i) {
      const auto& test = tests[i];
      n_positions++;
      n_visited = 0;
//      std::cout << Board(test.player, test.opponent).GetEmpties() << "\n";
      tmp ^= Evaluate(test.player, test.opponent, test.alpha, test.beta, &n_visited);
      tot_n_visited += n_visited;
    }
  }
  auto end = std::chrono::high_resolution_clock::now();
  double millis = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();


  std::cout << "Visited/sec: " << (int) (1000.0 / millis * tot_n_visited) << "\n";
  std::cout << "Visited/test: " << tot_n_visited / (double) n_positions << "\n";
  std::cout << "Total time: " << millis / 1000.0 << "\n";

  return (EXIT_SUCCESS);
}