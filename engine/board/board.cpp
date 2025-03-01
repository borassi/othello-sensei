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

#include <cassert>
#include <iostream>
#include <sstream>
#include "board.h"
#include "get_flip.h"
#include "get_moves.h"

const char kInitialBoard[] =
    "--------"
    "--------"
    "--------"
    "---OX---"
    "---XO---"
    "--------"
    "--------"
    "--------";

std::ostream& operator<<(std::ostream& stream, const Board& b) {
  for (int i = 63; i >= 0; --i) {
    BitPattern mask = (1ULL << i);
    if ((b.Player() & mask) != 0) {
      stream << 'X';
    } else if ((b.Opponent() & mask) != 0) {
      stream << 'O';
    } else {
      stream << '-';
    }
    if (i % 8 == 0) {
      stream << '\n';
    }
  }
  return stream;
}

Board::Board(const std::string& board, bool player_x)
    : Board(ParsePattern(board.c_str(), player_x ? 'X' : 'O'),
            ParsePattern(board.c_str(), player_x ? 'O' : 'X')) {}

Board::Board(const std::string& sequence, std::vector<Board>* previous) : Board() {
  assert(sequence.length() % 2 == 0);
  for (int i = 0; i < sequence.length(); i += 2) {
    Square move = MoveToSquare(sequence.substr(i, 2));
    BitPattern flip = GetFlip(move, player_, opponent_);
    assert(flip != 0);
    if (previous) {
      previous->push_back(Board(*this));
    }
    PlayMove(flip);
    if (HaveToPass(player_, opponent_)) {
      if (previous) {
        previous->push_back(Board(*this));
      }
      PlayMove(0);
    }
  }
}

Board::Board() : Board(kInitialBoard, true) {}

Board RandomBoard() {
  double percentage_player = (double) std::rand() / RAND_MAX;
  double percentage_opponent = (double) std::rand() / RAND_MAX;
  double tot = percentage_player + percentage_opponent + (double) std::rand() / RAND_MAX;
  percentage_player /= tot;
  percentage_opponent /= tot;
  if (percentage_player < 0.01 && percentage_opponent < 0.01) {
    return RandomBoard();
  }
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
  Board b(board_string.c_str(), true);
  if (b.Player() == 0 && b.Opponent() == 0) {
    return RandomBoard(percentage_player, percentage_opponent);
  }
  return {board_string.c_str(), true};
}

std::string Board::ToString() const {
  std::stringstream stream;
  stream << *this;
  return stream.str();
}