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
 
#ifndef OTHELLO_SENSEI_LOAD_THOR_H
#define OTHELLO_SENSEI_LOAD_THOR_H

#include "../board/bitpattern.h"
#include "files.h"

std::vector<std::vector<Square>> GetGames(std::string filename) {
  std::fstream file = std::fstream(filename, std::ios::in | std::ios::binary);
  auto length = FileLength(file);
  std::vector<std::vector<Square>> result;
  file.seekg(16);
  while (file.tellg() < length) {
    std::vector<Square> moves(60);
    file.seekg(8, std::ios::cur);
    file.read((char*) &moves[0], 60);
    for (int i = 0; i < 60; ++i) {
      if (moves[i] != 0) {
        assert(moves[i] >= 11 && moves[i] <= 88);
        moves[i] = (moves[i] % 10) - 1 + (moves[i] / 10 - 1) * 8;
      } else {
        moves[i] = kNoSquare;
      }
    }
    result.push_back(moves);
  }
  return result;
}

#endif //OTHELLO_SENSEI_LOAD_THOR_H
