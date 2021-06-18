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

int main(int argc, char** argv) {
  std::cout << "%SUITE_STARTING% evaluator_last_moves_profile" << std::endl;
  std::cout << "%SUITE_STARTED%" << std::endl;

  std::cout << "%TEST_STARTED% test1 (evaluator_last_moves_profile)" << std::endl;
  
  std::vector<Board> boards;
  while (boards.size() < 10000) {
    double perc_player = (double) rand() / RAND_MAX * 0.9;
    double perc_opponent = 0.9 - perc_player;
    Board b = RandomBoard(perc_player, perc_opponent);
    if (b.NEmpties() == 8) {
      boards.push_back(RandomBoard(perc_player, perc_opponent));
    }
  }
  long totNVisited = 0;
  
  std::cout << "Starting\n";
  int N = 10;
  auto start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < N; ++i) {
    for (const auto& board : boards) {
      int nVisited = 0;
      Evaluate(board.GetPlayer(), board.GetOpponent(), -1, 1, &nVisited);
      totNVisited += nVisited;
    }
  }
  auto end = std::chrono::high_resolution_clock::now();
  double millis = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  
  
  std::cout << "\n\nTime: " << (int) (1000.0 / millis * totNVisited) << "\n\n\n";
  std::cout << "\nTests: " << (int) totNVisited << "\n\n\n";
  std::cout << "%TEST_FINISHED% time=" << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " test1 (evaluator_last_moves_profile)" << std::endl;

  std::cout << "%SUITE_FINISHED% time=0" << std::endl;

  return (EXIT_SUCCESS);
}