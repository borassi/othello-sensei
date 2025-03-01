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
#include "../board/sequence.h"

class Game {
 public:
  Game(char* buffer, int offset, short year, const std::vector<std::string>& players, const std::vector<std::string>& tournaments, float priority) :
      black_(&players[(buffer[offset + 2] & 0xff) | ((buffer[offset + 3] & 0xff) << 8)]),
      white_(&players[(buffer[offset + 4] & 0xff) | ((buffer[offset + 5] & 0xff) << 8)]),
      year_(year),
      tournament_(&tournaments[(buffer[offset] & 0xff) | ((buffer[offset + 1] & 0xff) << 8)]),
      score_(buffer[offset + 6]),
      moves_(Sequence::FromThor((Square*) buffer + offset + 8)),
      priority_(priority) {
    assert(priority_ >= 0 && priority_ <= 1);
    moves_.ToCanonicalGameInplace();
  }

  Game(Sequence moves, const std::string* black, const std::string* white,
       const std::string* tournament, short year, Eval score, float priority) :
      moves_(moves),
      black_(black),
      white_(white),
      tournament_(tournament),
      year_(year),
      score_(score),
      priority_(priority) {
    assert(priority_ >= 0 && priority_ <= 1);
  }

  std::string Black() const { return *black_; }
  std::string White() const { return *white_; }
  std::string Tournament() const { return *tournament_; }
  const char* BlackC() const { return black_->c_str(); }
  const char* WhiteC() const { return white_->c_str(); }
  const char* TournamentC() const { return tournament_->c_str(); }
  short Year() const { return year_; }
  Eval Score() const { return score_; }
  float Priority() const { return priority_; }

  const Sequence& Moves() const { return moves_; }
  Square Move(Square i) const { return moves_.Move(i); }

  std::string ToString() const;

  inline bool operator<(const Game& rhs) const {
    return ToTuple() < rhs.ToTuple();
  }
  inline bool operator>(const Game& rhs) const { return rhs < *this; }
  inline bool operator<=(const Game& rhs) const { return !(*this > rhs); }
  inline bool operator>=(const Game& rhs) const { return !(*this < rhs); }
  inline bool operator==(const Game& rhs) const {
    return !(*this > rhs) && !(*this < rhs);
  }
  inline bool operator!=(const Game& rhs) const { return !(*this == rhs); }

  std::tuple<Sequence, std::string, std::string, std::string, short, Eval> ToTuple() const {
    return std::tie(moves_, *black_, *white_, *tournament_, year_, score_);
  }

 private:
  Sequence moves_;
  const std::string* black_;
  const std::string* white_;
  const std::string* tournament_;
  short year_;
  float priority_;
  Eval score_;
};

namespace std {
  template<>
  struct hash<Game> {
    std::size_t operator()(const Game& g) const {
      std::size_t hash = std::hash<Sequence>()(g.Moves());
      hash = CombineHashes(hash, HashString(g.Black()));
      hash = CombineHashes(hash, HashString(g.White()));
      hash = CombineHashes(hash, HashString(g.Tournament()));
      return hash;
    }
  };
}

std::ostream& operator<<(std::ostream& stream, const Game& s);

#endif // OTHELLO_SENSEI_GAME_H
