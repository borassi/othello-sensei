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

#include "board_optimized.h"
#include "../utils/load_training_set.h"

using namespace std;

constexpr int kNumIterations = 1;

class TestCase {
 public:
  Square move = 255;
  short patterns[kNumPatterns];
  Square empties[64];
  int n_empties;
};

//int VisitUntilDepth(short patterns[kNumPatterns], short* empties, int n_empties, int depth) {
//  if (depth == 0) {
//    return patterns[12];
//  }
//  int result = 0;
//  for (int i = 0; i < n_empties; ++i) {
//    kConstants.play_move_function[empties[i]](patterns);
//    Invert(patterns);
//  }
//  return result;
//}

int main(int argc, char** argv) {
  int N = 100000;
  unsigned long long tmp = 12;

  std::vector<EvaluatedBoard> boards = load_train_set();
  std::vector<TestCase> tests;
  while (tests.size() < N) {
    EvaluatedBoard& b = boards[rand() % boards.size()];
    if (b.GetBoard().NEmpties() < 40 || b.GetBoard().NEmpties() > 50) {
      continue;
    }
    TestCase& t = tests.emplace_back();
    PatternsInit(t.patterns, b.GetPlayer(), b.GetOpponent());
    t.n_empties = 0;
    FOR_EACH_SET_BIT(~(b.GetPlayer() | b.GetOpponent()), pattern) {
      t.empties[t.n_empties++] = __builtin_ctzll(pattern);
    }
    t.move = t.empties[rand() % t.n_empties];
  }
  for (auto& test : tests) {
    if (test.move >= 64) {
      std::cout << "BIG MISTAKE\n";
      while(true) {}
    }
  }

  int64_t tot_moves = 0;
  auto start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < kNumIterations; ++i) {
    for (auto& test : tests) {
      for (int j = 0; j < 1000; ++j) {
        kConstants.play_move_function[test.empties[j & 31]](test.patterns);
//        Invert(test.patterns);
        ++tot_moves;
      }
      tmp ^= test.patterns[12];
    }
  }
  auto end = std::chrono::high_resolution_clock::now();
  double millis = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

  std::cout << tmp << "\n";
  std::cout << "Flips / sec: " << (int) (tot_moves * 1000.0 / millis) << "\n";
  std::cout << "Total time: " << (double) millis / 1000 << " sec\n";

  return (EXIT_SUCCESS);
}
