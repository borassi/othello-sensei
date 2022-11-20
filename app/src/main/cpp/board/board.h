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

#include <array>
#include "bitpattern.h"
#include "get_flip.h"

typedef std::array<u_int8_t, 13> SerializedBoard;

class Board {

public:
  Board(BitPattern player, BitPattern opponent) : player_(player), opponent_(opponent) {}
  Board(const char* board, bool player_black);
  Board(const std::string& sequence);
  Board();

  BitPattern Player() const {
    return player_;
  }
  BitPattern Opponent() const {
    return opponent_;
  }
  BitPattern Empties() const {
    return ~(player_ | opponent_);
  }
  int NEmpties() const {
    return __builtin_popcountll(Empties());
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
  void PlayMove(BitPattern flip) {
    BitPattern tmp = opponent_;
    opponent_ = NewOpponent(flip, player_);
    player_ = NewPlayer(flip, tmp);
  }
  bool operator==(const Board& rhs) const {
    return Player() == rhs.Player()
           && Opponent() == rhs.Opponent();
  }
  std::vector<Board> AllTranspositions() {
    auto players = AllBitPatternTranspositions(player_);
    auto opponents = AllBitPatternTranspositions(opponent_);
    assert(players.size() == opponents.size());
    std::vector<Board> result;
    result.reserve(players.size());

    for (int i = 0; i < players.size(); ++i) {
      result.push_back(Board(players[i], opponents[i]));
    }
    return result;
  }

  Board Unique() {
    auto boards = AllTranspositions();
    Board best_board = boards[0];

    for (const Board& board : boards) {
      if (board.Player() < best_board.Player() ||
          (board.Player() == best_board.Player() &&
           board.Opponent() < best_board.Opponent())) {
        best_board = board;
      }
    }
    return best_board;
  }

  SerializedBoard Serialize() {
    SerializedBoard serialized;
    BitPattern current_square = 1ULL << 63;
    Board unique = Board(player_, opponent_).Unique();
    BitPattern player = unique.Player();
    BitPattern opponent = unique.Opponent();
    for (int i = 0; i < 13; ++i) {
      uint8_t five_squares_serialized = 0;
      int current_multiplier = 1;
      for (int j = 0; j < 5; ++j) {
        if (player & current_square) {
          five_squares_serialized += current_multiplier;
        } else if (opponent & current_square) {
          five_squares_serialized += 2 * current_multiplier;
        }
        current_multiplier *= 3;
        current_square = current_square >> 1;
      }
      serialized[i] = five_squares_serialized;
    }
    return serialized;
  }

  static Board Deserialize(SerializedBoard serialized) {
    BitPattern player = 0;
    BitPattern opponent = 0;
    BitPattern current_square = 1ULL << 63;

    for (int i = 0; i < 13; ++i) {
      uint8_t five_squares_serialized = serialized[i];
      for (int j = 0; j < 5; ++j) {
        int current_value = five_squares_serialized % 3;
        if (current_value == 1) {
          player |= current_square;
        } else if (current_value == 2) {
          opponent |= current_square;
        }
        current_square = current_square >> 1;
        five_squares_serialized /= 3;
      }
    }
    return Board(player, opponent);
  }

private:
  BitPattern player_;
  BitPattern opponent_;
  
};

namespace std {
  template <>
  struct hash<Board> {
    std::size_t operator()(const Board& b) const {
      return HashFull(b.Player(), b.Opponent());
    }
  };
}

std::ostream& operator<<(std::ostream& stream, const Board& b);
Board RandomBoard(double percentage_player, double percentage_opponent);
Board RandomBoard();

#endif /* BOARD_H */

