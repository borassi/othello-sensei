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

/** Conversion array: neighbour bits */
const unsigned long long NEIGHBOUR[] = {
	0x0000000000000302ULL, 0x0000000000000705ULL, 0x0000000000000e0aULL, 0x0000000000001c14ULL,
	0x0000000000003828ULL, 0x0000000000007050ULL, 0x000000000000e0a0ULL, 0x000000000000c040ULL,
	0x0000000000030203ULL, 0x0000000000070507ULL, 0x00000000000e0a0eULL, 0x00000000001c141cULL,
	0x0000000000382838ULL, 0x0000000000705070ULL, 0x0000000000e0a0e0ULL, 0x0000000000c040c0ULL,
	0x0000000003020300ULL, 0x0000000007050700ULL, 0x000000000e0a0e00ULL, 0x000000001c141c00ULL,
	0x0000000038283800ULL, 0x0000000070507000ULL, 0x00000000e0a0e000ULL, 0x00000000c040c000ULL,
	0x0000000302030000ULL, 0x0000000705070000ULL, 0x0000000e0a0e0000ULL, 0x0000001c141c0000ULL,
	0x0000003828380000ULL, 0x0000007050700000ULL, 0x000000e0a0e00000ULL, 0x000000c040c00000ULL,
	0x0000030203000000ULL, 0x0000070507000000ULL, 0x00000e0a0e000000ULL, 0x00001c141c000000ULL,
	0x0000382838000000ULL, 0x0000705070000000ULL, 0x0000e0a0e0000000ULL, 0x0000c040c0000000ULL,
	0x0003020300000000ULL, 0x0007050700000000ULL, 0x000e0a0e00000000ULL, 0x001c141c00000000ULL,
	0x0038283800000000ULL, 0x0070507000000000ULL, 0x00e0a0e000000000ULL, 0x00c040c000000000ULL,
	0x0302030000000000ULL, 0x0705070000000000ULL, 0x0e0a0e0000000000ULL, 0x1c141c0000000000ULL,
	0x3828380000000000ULL, 0x7050700000000000ULL, 0xe0a0e00000000000ULL, 0xc040c00000000000ULL,
	0x0203000000000000ULL, 0x0507000000000000ULL, 0x0a0e000000000000ULL, 0x141c000000000000ULL,
	0x2838000000000000ULL, 0x5070000000000000ULL, 0xa0e0000000000000ULL, 0x40c0000000000000ULL,
	0, 0 // <- hack for passing move & nomove
};
  
class TestCase {
  public:
  Move move;
  BitPattern player;
  BitPattern opponent;
};

int main(int argc, char** argv) {
  std::cout << "%SUITE_STARTING% get_flip_profile" << std::endl;
  std::cout << "%SUITE_STARTED%" << std::endl;

  std::cout << "%TEST_STARTED% test1 (get_flip_profile)" << std::endl;
  
  std::vector<TestCase> tests;
  ifstream tests_file("../../othello-sensei/coefficients/moves.txt");
  std::string line;
  int i = 0;
  while (std::getline(tests_file, line)) {
    TestCase test;
    sscanf(line.c_str(), "%hhd %lld %lld", &test.move, &test.player, &test.opponent);
    tests.push_back(test);
  }
  tests_file.close();
  int tot = 0;
  
  int N = 100000;
  unsigned long long tmp = 12;
  auto start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < N; ++i) {
    for (const auto& test : tests) {
//      tmp ^= GetFlip(test.move, test.player, test.opponent);
//      if ((NEIGHBOUR[test.move] & test.opponent) != 0) {
        tmp ^= GetFlip(test.move, test.player, test.opponent);
//        tmp ^= flip[test.move](test.player, test.opponent);      
//      }
    }
  }
  auto end = std::chrono::high_resolution_clock::now();
  std::cout << tmp << "\n";
  double millis = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  
  
  std::cout << "\n\nTime: " << (int) (1000 * N / millis * tests.size()) << "\n\n\n";
  std::cout << "\nTests: " << (int) tests.size() << "\n\n\n";
  std::cout << "%TEST_FINISHED% time=" << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " test1 (get_flip_profile)" << std::endl;

  std::cout << "%SUITE_FINISHED% time=0" << std::endl;

  return (EXIT_SUCCESS);
}

