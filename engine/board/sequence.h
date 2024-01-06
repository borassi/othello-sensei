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

#ifndef BOARD_SEQUENCE_H
#define BOARD_SEQUENCE_H

#include "bitpattern.h"

class Sequence {
 public:
  Sequence(std::string moves) : moves_(moves.size() / 2) {
    assert(moves.size() % 2 == 0);
    assert(moves.size() < 120);

    for (int i = 0; i < moves.length(); i += 2) {
      moves_[i / 2] = MoveToSquare(moves.substr(i, 2));
    }
  }
  Sequence(Square* moves, int length) : moves_(length) {
    memcpy(moves_.data(), moves, moves_.size() * sizeof(Square));
  }
  Sequence(std::array<Square, 60> moves, int length) : Sequence(moves.data(), length) {}

  void ToCanonicalSequence() {
    if (moves_[0] == 19) {
      return;
    } else if (moves_[0] == 26) {
      for (int i = 0; i < moves_.size(); ++i) {
        moves_[i] = 8 * (moves_[i] % 8) + (moves_[i] / 8);
      }
    } else if (moves_[0] == 37) {
      for (int i = 0; i < moves_.size(); ++i) {
        moves_[i] = 8 * (7 - moves_[i] % 8) + (7 - moves_[i] / 8);
      }
    } else if (moves_[0] == 44) {
      for (int i = 0; i < moves_.size(); ++i) {
        moves_[i] = 8 * (7 - moves_[i] / 8) + (7 - moves_[i] % 8);
      }
    } else {
      assert(false);
    }
  }

  inline bool operator< (const Sequence& rhs) const {
    if (moves_.size() < rhs.moves_.size()) {
      return true;
    } else if (moves_.size() > rhs.moves_.size()) {
      return false;
    }
    for (int i = 0; i < moves_.size(); ++i) {
      if (moves_[i] < rhs.moves_[i]) {
        return true;
      } else if (moves_[i] > rhs.moves_[i]) {
        return false;
      }
    }
    return false;
  }
  inline bool operator>(const Sequence& rhs) const { return rhs < *this; }
  inline bool operator<=(const Sequence& rhs) const { return !(*this > rhs); }
  inline bool operator>=(const Sequence& rhs) const { return !(*this < rhs); }

  inline bool operator==(const Sequence& rhs) const { return !(rhs < *this || *this < rhs); }
  inline bool operator!=(const Sequence& rhs) const { return !(*this == rhs); }

  const std::vector<Square>& Moves() const { return moves_; }

 private:
  std::vector<Square> moves_;
};
#endif  // BOARD_SEQUENCE_H