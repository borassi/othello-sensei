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

using namespace std;

class TestCase {
 public:
  Square move;
  BitPattern player;
  BitPattern opponent;
};

int main(int argc, char** argv) {
  std::vector<TestCase> tests;
//  ifstream tests_file("testdata/get_flip_profile_examples.txt");
//  std::string line;
//  int i = 0;
//  while (std::getline(tests_file, line)) {
//    TestCase test;
//    sscanf(line.c_str(), "%hhd %llu %llu", &test.move, &test.player, &test.opponent);
//    tests.push_back(test);
//  }
//  tests_file.close();
  int tot = 0;

  int N = 100000;
  unsigned long long tmp = 12;
  auto start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < N; ++i) {
    for (const auto& test : tests) {
      tmp ^= GetFlip(test.move, test.player, test.opponent);
    }
  }
  auto end = std::chrono::high_resolution_clock::now();
  double millis = (double) std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

  std::cout << tmp << "\n";
  std::cout << "Flips / sec: " << (int) (1000 * N / millis * tests.size()) << "\n";
  std::cout << "Total time: " << (double) millis / 1000 << " sec\n";

  return (EXIT_SUCCESS);
}
