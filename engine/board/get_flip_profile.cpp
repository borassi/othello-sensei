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

#include "bitpattern.h"
#include "get_flip.h"
#include "../utils/load_training_set.h"

using namespace std;

class TestCase {
 public:
  Square move;
  BitPattern player;
  BitPattern opponent;
};

int main(int argc, char** argv) {
  int N = 1000000;
  unsigned long long tmp = 12;
  int iterations = 101;

  std::vector<EvaluatedBoard> boards = load_train_set();
  std::vector<TestCase> tests;
  for (int i = 0; i < N; ++i) {
    EvaluatedBoard& b = boards[rand() % boards.size()];
    TestCase t;
    t.player = b.GetPlayer();
    t.opponent = b.GetOpponent();
    t.move = __builtin_ctzll(~(t.player | t.opponent));
    tests.push_back(t);
  }

  auto start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < iterations; ++i) {
    for (const auto& test : tests) {
      tmp ^= GetFlip(test.move, test.player, test.opponent);
    }
  }
  auto end = std::chrono::high_resolution_clock::now();
  double millis = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

  std::cout << tmp << "\n";
  std::cout << "Flips / sec: " << (int) (iterations * N * 1000.0 / millis) << "\n";
  std::cout << "Total time: " << (double) millis / 1000 << " sec\n";

  return (EXIT_SUCCESS);
}
