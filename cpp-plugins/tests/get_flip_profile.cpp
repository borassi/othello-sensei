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

#include "bitpattern.h"
#include "get_flip.h"

using namespace std;

  
class TestCase {
  public:
  Move move;
  BitPattern player;
  BitPattern opponent;
  TestCase(int move, BitPattern player, BitPattern opponent) : move(move), player(player), opponent(opponent) {}
};

int main(int argc, char** argv) {
  std::cout << "%SUITE_STARTING% newsimpletest1" << std::endl;
  std::cout << "%SUITE_STARTED%" << std::endl;

  std::cout << "%TEST_STARTED% test1 (newsimpletest1)" << std::endl;
  
  std::vector<TestCase> tests;
  ifstream tests_file("../../othello-sensei/coefficients/moves.txt");
  std::string line;
  int i = 0;
  while (std::getline(tests_file, line)) {
    // Output the text from the file
    Move move = stoi(line.substr(0, line.find(' ')));
    line = line.substr(line.find(' ') + 1, line.length());
    BitPattern player = stol(line.substr(0, line.find(' ')));
    line = line.substr(line.find(' ') + 1, line.length());
    BitPattern opponent = stol(line.substr(0, line.find(' ')));
    TestCase test(move, player, opponent);
//    std::cout << test.move << "\n" << Board(test.player, test.opponent).toString();
    
    line.substr(0, line.find(' '));
    tests.push_back(test);
  }
  tests_file.close();
  int tot = 0;
  
  int N = 10000;
  auto start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < N; ++i) {
    for (const auto& test : tests) {
      GetFlip(test.move, test.player, test.opponent);
    }
  }
  auto end = std::chrono::high_resolution_clock::now();
  double millis = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  
  
  std::cout << "\n\nTime: " << (int) (1000 * N / millis * tests.size()) << "\n\n\n";
  std::cout << "\nTests: " << (int) tests.size() << "\n\n\n";
  std::cout << "%TEST_FINISHED% time=" << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " test1 (newsimpletest1)" << std::endl;

  std::cout << "%SUITE_FINISHED% time=0" << std::endl;

  return (EXIT_SUCCESS);
}

