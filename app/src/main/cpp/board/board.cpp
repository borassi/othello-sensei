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

#include <iostream>
#include "board.h"

const char kInitialBoard[] =
    "--------"
    "--------"
    "--------"
    "---OX---"
    "---XO---"
    "--------"
    "--------"
    "--------";

std::string Board::ToString() const {
  std::string result;
  for (int i = 63; i >= 0; --i) {
    BitPattern mask = (1L << i);
    if ((player_ & mask) != 0) {
      result += 'X';
    } else if ((opponent_ & mask) != 0) {
      result += 'O';
    } else {
      result += '-';
    }
    if (i % 8 == 0) {
      result += '\n';
    }
  }
  return result;
}

Board::Board (const char* board)
    : Board(ParsePattern(board, 'X'), ParsePattern(board, 'O')) {}

Board::Board() : Board(kInitialBoard) {}

Board RandomBoard() {
  double percentage_player = (double) std::rand() / RAND_MAX;
  double percentage_opponent = (double) std::rand() / RAND_MAX;
  double tot = percentage_player + percentage_opponent + (double) std::rand() / RAND_MAX;
  percentage_player /= tot;
  percentage_opponent /= tot;
  return RandomBoard(percentage_player, percentage_opponent);
}

Board RandomBoard(double percentage_player, double percentage_opponent) {
  std::string board_string;
  for (int j = 0; j < 64; j++) {
    double value = (double) std::rand() / RAND_MAX;
    if (value < percentage_player) {
      board_string += "X";
    } else if (value < percentage_opponent + percentage_player) {
      board_string += "O";
    } else {
      board_string += "-";
    }
  }
  return {board_string.c_str()};
}