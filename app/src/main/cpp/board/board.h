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

#ifndef BOARD_H
#define BOARD_H

#include "bitpattern.h"
#include "get_flip.h"

class Board {

public:
  Board(BitPattern player, BitPattern opponent) : player_(player), opponent_(opponent) {}
  Board(const char* board);
  Board();

  long GetPlayer() const {
    return player_;
  }
  long GetOpponent() const {
    return opponent_;
  }
  long GetEmpties() const {
    return ~(player_ | opponent_);
  }
  long NEmpties() const {
    return __builtin_popcountll(GetEmpties());
  }
  bool IsEmpty(Square move) const {
    return !IsFull(player_, move) && !IsFull(opponent_, move);
  }
  bool IsPlayer(Square move) const {
    return IsFull(player_, move);
  }
  bool IsOpponent(Square move) const {
    return IsFull(opponent_, move);
  }
  std::string ToString() const;
  void PlayMove(BitPattern flip) {
    BitPattern tmp = opponent_;
    opponent_ = NewOpponent(flip, player_);
    player_ = NewPlayer(flip, tmp);
  }
  bool operator==(const Board& rhs) const {
    return GetPlayer() == rhs.GetPlayer()
           && GetOpponent() == rhs.GetOpponent();
  }
private:
  long player_;
  long opponent_;
  
};

Board RandomBoard(double percentage_player, double percentage_opponent);
Board RandomBoard();
#endif /* BOARD_H */

