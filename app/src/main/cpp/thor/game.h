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
 
#ifndef OTHELLO_SENSEI_GAME_H
#define OTHELLO_SENSEI_GAME_H

#include <cstring>
#include <sstream>
#include <stdint.h>
#include <string>

#include "../board/board.h"

struct ThorSquareToSquare {
  Square square[256];

  constexpr ThorSquareToSquare() : square() {
    for (int i = 0; i < 256; ++i) {
      if (i % 10 >= 1 && i % 10 <= 8 && i / 10 >= 1 && i / 10 <= 8) {
        square[i] = (8 - i % 10) + 8 * (8 - i / 10);
      } else {
        square[i] = kNoSquare;
      }
    }
  }
};

constexpr ThorSquareToSquare kThorSquareToSquare;

class Game {
 public:
  Game(char* buffer, int offset, short year, const std::vector<std::string>& players, const std::vector<std::string>& tournaments) :
      black_(&players[(buffer[offset + 2] & 0xff) | ((buffer[offset + 3] & 0xff) << 8)]),
      white_(&players[(buffer[offset + 4] & 0xff) | ((buffer[offset + 5] & 0xff) << 8)]),
      year_(year),
      tournament_(&tournaments[(buffer[offset] & 0xff) | ((buffer[offset + 1] & 0xff) << 8)]),
      score_(buffer[offset + 6]) {
    for (int i = 0; i < 60; ++i) {
      moves_[i] = kThorSquareToSquare.square[buffer[offset + 8 + i]];
    }
  }

  std::string Black() const { return *black_; }
  std::string White() const { return *white_; }
  std::string Tournament() const { return *tournament_; }
  short Year() const { return year_; }
  Eval Score() const { return score_; }

  const std::array<uint8_t, 60>& MovesByte() const { return moves_; }

  std::string Moves() const;

  std::string ToString() const;

 private:
  std::array<uint8_t, 60> moves_;
  const std::string* black_;
  const std::string* white_;
  const std::string* tournament_;
  short year_;
  Eval score_;
};



#endif // OTHELLO_SENSEI_GAME_H
