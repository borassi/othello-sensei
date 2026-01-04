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
  Game(char* buffer, int offset, short year, const std::vector<std::string>& players, const std::vector<std::string>& tournaments, double priority) :
      moves_(Sequence::FromThor((Square*) buffer + offset + 8)),
      black_(&players[(buffer[offset + 2] & 0xff) | ((buffer[offset + 3] & 0xff) << 8)]),
      white_(&players[(buffer[offset + 4] & 0xff) | ((buffer[offset + 5] & 0xff) << 8)]),
      tournament_(&tournaments[(buffer[offset] & 0xff) | ((buffer[offset + 1] & 0xff) << 8)]),
      priority_(priority),
      year_(year),
      score_(buffer[offset + 6]) {
    assert(priority_ >= 0 && priority_ <= 1);
    moves_.ToCanonicalGameInplace();
  }

  Game(Sequence moves, const std::string* black, const std::string* white,
       const std::string* tournament, short year, Eval score, double priority, bool canonicalize = true) :
      moves_(moves),
      black_(black),
      white_(white),
      tournament_(tournament),
      priority_(priority),
      year_(year),
      score_(score) {
    assert(priority_ >= 0 && priority_ <= 1);
    if (canonicalize) {
      moves_.ToCanonicalGameInplace();
    }
  }

  std::string Black() const { return *black_; }
  std::string White() const { return *white_; }
  std::string Tournament() const { return *tournament_; }
  const char* BlackC() const { return black_->c_str(); }
  const char* WhiteC() const { return white_->c_str(); }
  const char* TournamentC() const { return tournament_->c_str(); }
  short Year() const { return year_; }
  Eval Score() const { return score_; }
  double Priority() const { return priority_; }

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
  double priority_;
  short year_;
  Eval score_;
};

struct GamesList {
  int num_games = 0;
  std::vector<Game> examples;
  std::unordered_map<Square, int> next_moves;
  int max_games = 0;

  void Merge(const GamesList& other);

  void Rotate(const Sequence& target);
};

inline bool CompareGamesByHash(const Game& g1, const Game& g2) {
  return
      std::forward_as_tuple(-g1.Year(), 1 - g1.Priority(), g1) <
      std::forward_as_tuple(-g2.Year(), 1 - g2.Priority(), g2);
}

void SortAndCutExamples(int max_games, std::vector<Game>& examples);

std::ostream& operator<<(std::ostream& stream, const Game& s);

// Compare by:
// - The value of f(g);
// - The sequence of f(g) (if f(g1) is a subsequence of f(g2), g1 > g2).
// - The game itself (just to break ties).
//
// When comparing a game and a sequence, they are equal if the sequence is a
// substring of the game (so that lower_bound is the first game with this
// sequence, upper_bound is the last).
template<class T, T f(const Game&), class GameGetter>
class CmpGameAndSequence {
 public:
  explicit CmpGameAndSequence(const GameGetter& game_getter) : game_getter_(game_getter) {}

  bool operator()(int game_index1, int game_index2) {
    Game g1 = game_getter_.GetGame(game_index1);
    Game g2 = game_getter_.GetGame(game_index2);
    auto size = std::min(g1.Moves().Size(), g2.Moves().Size());
    return
        std::forward_as_tuple(f(g1), g1.Moves().Subsequence(size), g2.Moves().Size(), g1) <
        std::forward_as_tuple(f(g2), g2.Moves().Subsequence(size), g1.Moves().Size(), g2);
  }
  bool operator()(int game_index, const std::pair<T, Sequence>& s) {
    Game g = game_getter_.GetGame(game_index);
    // NOTE: We take the minimum so that lower_bound is before everything with
    // this sequence, upper_bound is after.
    auto size = std::min(g.Moves().Size(), s.second.Size());
    return
        std::forward_as_tuple(f(g), g.Moves().Subsequence(size), s.second.Size()) <
        std::forward_as_tuple(s.first, s.second.Subsequence(size), size);
  }
  bool operator()(const std::pair<T, Sequence>& s, int game_index) {
    Game g = game_getter_.GetGame(game_index);
    // NOTE: We take the minimum so that lower_bound is before everything with
    // this sequence, upper_bound is after.
    auto size = std::min(g.Moves().Size(), s.second.Size());
    return
        std::forward_as_tuple(f(g), g.Moves().Subsequence(size), s.second.Size()) >
        std::forward_as_tuple(s.first, s.second.Subsequence(size), size);
  }

 private:
  const GameGetter& game_getter_;
};

inline bool True(const Game& g) { return true; }
inline std::string Black(const Game& g) { return g.Black(); }
inline std::string White(const Game& g) { return g.White(); }
inline std::string Tournament(const Game& g) { return g.Tournament(); }
inline short Year(const Game& g) { return g.Year(); }

template<class GameGetter>
class CmpByBlack : public CmpGameAndSequence<std::string, Black, GameGetter> { using CmpGameAndSequence<std::string, Black, GameGetter>::CmpGameAndSequence; };
template<class GameGetter>
class CmpByWhite : public CmpGameAndSequence<std::string, White, GameGetter> { using CmpGameAndSequence<std::string, White, GameGetter>::CmpGameAndSequence; };
template<class GameGetter>
class CmpByTournament : public CmpGameAndSequence<std::string, Tournament, GameGetter> { using CmpGameAndSequence<std::string, Tournament, GameGetter>::CmpGameAndSequence; };
template<class GameGetter>
class CmpByYear : public CmpGameAndSequence<short, Year, GameGetter> { using CmpGameAndSequence<short, Year, GameGetter>::CmpGameAndSequence; };

#endif // OTHELLO_SENSEI_GAME_H
