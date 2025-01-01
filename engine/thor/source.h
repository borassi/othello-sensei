/*
 * Copyright 2024 Michele Borassi
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

#ifndef OTHELLO_SENSEI_THOR_SOURCE_H
#define OTHELLO_SENSEI_THOR_SOURCE_H

#include <algorithm>
#include <climits>
#include <cstring>
#include <gtest/gtest_prod.h>
#include <random>

#include "../board/sequence.h"
#include "../main/bindings.h"
#include "../utils/files.h"
#include "game.h"

constexpr int kPlayerLength = 20;
constexpr int kTournamentLength = 26;

class GameGetter {
 public:
  GameGetter() : games_(), min_year_(SHRT_MAX), max_year_(SHRT_MIN) {}

  void LoadGameFiles(const std::set<std::string>& game_files, const std::vector<std::string>& players, const std::vector<std::string>& tournaments) {
    for (const auto& game_file : game_files) {
      LoadGames(game_file, players, tournaments);
    }
  }

  const Game* GetGame(int index) const {
    return &games_[index];
  }

  const std::vector<Game>& GetAllGames() const { return games_; }
  int NumGames() const { return games_.size(); }
  short MinYear() const { return min_year_; }
  short MaxYear() const { return max_year_; }

 private:
  std::vector<Game> games_;
  short min_year_;
  short max_year_;

  void LoadGames(const std::string& filepath, const std::vector<std::string>& players, const std::vector<std::string>& tournaments) {
    ElapsedTime t;
    std::vector<char> games = ReadFile<char>(filepath);
    if (games.size() == 0) {
      return;
    }
    short year = *((short*) &games[10]);
    min_year_ = std::min(min_year_, year);
    max_year_ = std::max(max_year_, year);
    for (int i = 16; i < games.size(); i += 68) {
      auto game = games_.emplace_back(games.data(), i, year, players, tournaments);
    }
  }
};

// Compare by:
// - The value of f(g);
// - The sequence of f(g) (if f(g1) is a subsequence of f(g2), g1 > g2).
// - The game itself (just to break ties).
//
// When comparing a game and a sequence, they are equal if the sequence is a
// substring of the game (so that lower_bound is the first game with this
// sequence, upper_bound is the last).
template<class T, T f(const Game*)>
class CmpGameAndSequence {
 public:
  CmpGameAndSequence(const GameGetter& game_getter) : game_getter_(game_getter) {}

  bool operator()(int game_index1, int game_index2) {
    const Game* g1 = game_getter_.GetGame(game_index1);
    const Game* g2 = game_getter_.GetGame(game_index2);
    auto size = std::min(g1->Moves().Size(), g2->Moves().Size());
    return
        std::forward_as_tuple(f(g1), g1->Moves().Subsequence(size), g2->Moves().Size(), *g1) <
        std::forward_as_tuple(f(g2), g2->Moves().Subsequence(size), g1->Moves().Size(), *g2);
  }
  bool operator()(int game_index, const std::pair<T, Sequence>& s) {
    const Game* g = game_getter_.GetGame(game_index);
    // NOTE: We take the minimum so that lower_bound is before everything with
    // this sequence, upper_bound is after.
    auto size = std::min(g->Moves().Size(), s.second.Size());
    return
        std::forward_as_tuple(f(g), g->Moves().Subsequence(size), s.second.Size()) <
        std::forward_as_tuple(s.first, s.second.Subsequence(size), size);
  }
  bool operator()(const std::pair<T, Sequence>& s, int game_index) {
    const Game* g = game_getter_.GetGame(game_index);
    // NOTE: We take the minimum so that lower_bound is before everything with
    // this sequence, upper_bound is after.
    auto size = std::min(g->Moves().Size(), s.second.Size());
    return
        std::forward_as_tuple(f(g), g->Moves().Subsequence(size), s.second.Size()) >
        std::forward_as_tuple(s.first, s.second.Subsequence(size), size);
  }

 private:
  const GameGetter& game_getter_;
};

inline bool True(const Game* g) { return true; }
inline std::string Black(const Game* g) { return g->Black(); }
inline std::string White(const Game* g) { return g->White(); }
inline std::string Tournament(const Game* g) { return g->Tournament(); }
inline short Year(const Game* g) { return g->Year(); }

class CmpBySequence : public CmpGameAndSequence<bool, True> { using CmpGameAndSequence::CmpGameAndSequence; };
class CmpByBlack : public CmpGameAndSequence<std::string, Black> { using CmpGameAndSequence::CmpGameAndSequence; };
class CmpByWhite : public CmpGameAndSequence<std::string, White> { using CmpGameAndSequence::CmpGameAndSequence; };
class CmpByTournament : public CmpGameAndSequence<std::string, Tournament> { using CmpGameAndSequence::CmpGameAndSequence; };
class CmpByYear : public CmpGameAndSequence<short, Year> { using CmpGameAndSequence::CmpGameAndSequence; };

inline bool CompareGamesByHash(const Game* g1, const Game* g2) {
  return
      std::forward_as_tuple(-g1->Year(), std::hash<Game>()(*g1), *g1) <
      std::forward_as_tuple(-g2->Year(), std::hash<Game>()(*g2), *g2);
}

struct GamesList {
  int num_games = 0;
  std::vector<const Game*> examples;
  std::unordered_map<Square, int> next_moves;
  int max_games = 0;

  void Merge(const GamesList& other) {
    num_games += other.num_games;
    examples.insert(examples.end(), other.examples.begin(), other.examples.end());
    std::sort(examples.begin(), examples.end(), CompareGamesByHash);
    if (examples.size() > max_games) {
      examples.resize(max_games);
    }
    for (const auto& [key, value] : other.next_moves) {
      next_moves[key] += value;
    }
  }

  void Rotate(const Sequence& target) {
    if (num_games == 0) {
      return;
    }
    assert(examples[0]->Moves().Size() >= target.Size());
    int transposition = target.GetTransposition(examples[0]->Moves().Subsequence(target.Size()));
    if (transposition == 0) {
      return;
    }
    std::unordered_map<Square, int> old_next_moves(next_moves.begin(), next_moves.end());
    next_moves.clear();
    for (const auto& [move, games] : old_next_moves) {
      next_moves[TransposeMove(move, transposition)] = games;
    }
  }
};

struct GamesInterval {
  std::vector<uint32_t>::const_iterator start;
  std::vector<uint32_t>::const_iterator end;

  GamesInterval() {}
  GamesInterval(std::vector<uint32_t>::const_iterator start,
                std::vector<uint32_t>::const_iterator end) : start(start), end(end) {}
};

class Source {
 public:
  Source(const std::string& folder, bool rebuild_games_order = false, bool rebuild_games_with_small_hash = false) :
      folder_(folder), players_(), tournaments_(), game_getter_() {
    ElapsedTime t;
    game_indices_ = {&game_index_by_black_, &game_index_by_white_, &game_index_by_tournament_, &game_index_by_year_};
    std::set<std::string> game_files;
    auto all_files = GetAllFiles(folder, /*include_files=*/true, /*include_directories=*/false);
    // We can't use directory_iterator because it's available only from MacOS 10.15 and we want to
    // run on MacOS 10.14.
    for (const std::string& entry : all_files) {
      if (EndsWith(ToLower(entry), ".jou")) {
        assert(players_.empty());
        players_ = LoadListFromFile(entry, kPlayerLength);
      } else if (EndsWith(ToLower(entry), ".trn")) {
        assert(tournaments_.empty());
        tournaments_ = LoadListFromFile(entry, kTournamentLength);
      } else if (EndsWith(ToLower(entry), ".wtb")) {
        game_files.insert(entry);
      }
    }
    game_getter_.LoadGameFiles(game_files, players_, tournaments_);
    if (FileExists(SortedGamesPath()) && !rebuild_games_order) {
      LoadSortedGames();
    } else {
      ComputeSortedGames();
    }
    if (FileExists(GamesSmallHashPath()) && !rebuild_games_with_small_hash) {
      LoadGamesSmallHash();
    } else {
      ComputeGamesSmallHash();
    }
  }

  const std::vector<std::string>& Tournaments() const { return tournaments_; }
  const std::vector<std::string>& Players() const { return players_; }

  GamesList GetGames(
      const Sequence& sequence,
      int max_games = INT_MAX,
      std::vector<std::string> blacks = {},
      std::vector<std::string> whites = {},
      std::vector<std::string> tournaments = {},
      short start_year = SHRT_MIN,
      short end_year = SHRT_MAX) {
    assert(max_games > 0);
    start_year = std::max(start_year, game_getter_.MinYear());
    end_year = std::min(end_year, game_getter_.MaxYear());
    std::vector<GamesInterval> intervals;
    bool must_iterate_over_games = true;

    GamesInterval games_interval;
    if (!blacks.empty()) {
      for (const std::string& black : blacks) {
        games_interval = GetGameIntervals<std::pair<std::string, Sequence>>(
            game_index_by_black_, {black, sequence}, CmpByBlack(game_getter_));
        intervals.push_back(games_interval);
      }
    } else if (!whites.empty()) {
      for (const std::string& white : whites) {
        games_interval = GetGameIntervals<std::pair<std::string, Sequence>>(
            game_index_by_white_, {white, sequence}, CmpByWhite(game_getter_));
        intervals.push_back(games_interval);
      }
    } else if (!tournaments.empty()) {
      for (const std::string& tournament : tournaments) {
        games_interval = GetGameIntervals<std::pair<std::string, Sequence>>(
            game_index_by_tournament_, {tournament, sequence}, CmpByTournament(game_getter_));
        intervals.push_back(games_interval);
      }
    } else {
      for (int year = end_year; year >= start_year; --year) {
        games_interval = GetGameIntervals<std::pair<int, Sequence>>(
            game_index_by_year_, {year, sequence}, CmpByYear(game_getter_));
        intervals.push_back(games_interval);
      }
      must_iterate_over_games = false;
    }
    GamesList result;
    result.max_games = max_games;
    int sequence_size = sequence.Size();
    if (must_iterate_over_games) {
      for (const GamesInterval interval : intervals) {
        for (auto it = interval.start; it != interval.end; ++it) {
          const Game* game = game_getter_.GetGame(*it);
          if ((blacks.empty() || Contains(blacks, game->Black())) &&
              (whites.empty() || Contains(whites, game->White())) &&
              (tournaments.empty() || Contains(tournaments, game->Tournament())) &&
              game->Year() >= start_year && game->Year() <= end_year) {
            result.next_moves[game->Move(sequence_size)]++;
            result.examples.push_back(game);
          }
        }
      }
      result.num_games = (int) result.examples.size();
    } else {
      for (const GamesInterval& interval : intervals) {
        int games_in_interval = (int) (interval.end - interval.start);
        result.num_games += games_in_interval;
        FillNextMoves(interval, sequence_size, result.next_moves);

        int games_to_add = max_games - result.examples.size();
        if (games_to_add <= 0 || games_in_interval == 0) {
          continue;
        }
        ElapsedTime t;
        double include_probability = games_to_add / (double) games_in_interval;
        int hash_index = IncludeProbabilityToHashIndex(include_probability);
        assert(hash_index < (int) games_with_small_hash_.size());
        for (; hash_index >= 0; --hash_index) {
          auto& games_with_small_hash = games_with_small_hash_[hash_index];
          auto start = std::lower_bound(games_with_small_hash.begin(), games_with_small_hash.end(), interval.start - game_index_by_year_.begin());
          auto end = std::lower_bound(games_with_small_hash.begin(), games_with_small_hash.end(), interval.end - game_index_by_year_.begin());
          if (end - start < games_to_add) {
            continue;
          }
          for (auto it = start; it < end; ++it) {
            result.examples.push_back(game_getter_.GetGame(game_index_by_year_[*it]));
          }
          break;
        }
        if (hash_index < 0) {
          for (auto it = interval.start; it < interval.end; ++it) {
            result.examples.push_back(game_getter_.GetGame(*it));
          }
        }
      }
    }
    ElapsedTime t;
    SortAndCutExamples(max_games, result.examples);
    return result;
  }

  const std::vector<Game>& AllGames() const { return game_getter_.GetAllGames(); }

  std::string SortedGamesPath() const {
    return folder_ + "/games_order.sen";
  }

  std::string GamesSmallHashPath() const {
    return folder_ + "/games_with_small_hash.sen";
  }

  void SaveSortedGames() const {
    std::ofstream file(SortedGamesPath());
    uint32_t index;
    for (const std::vector<uint32_t>* v : game_indices_) {
      file.write((char*) v->data(), v->size() * sizeof(uint32_t));
    }
  }

  virtual void LoadSortedGames() {
    std::vector<uint32_t> file = ReadFile<uint32_t>(SortedGamesPath());
    int position = 0;
    int num_games = file.size() / game_indices_.size();
    assert(num_games == game_getter_.NumGames());

    for (int i = 0; i < game_indices_.size(); ++i) {
      std::copy(file.begin() + i * num_games,
                file.begin() + (i+1) * num_games,
                std::back_inserter(*game_indices_[i]));
    }
  }

  void SaveGamesSmallHash() const {
    std::ofstream file(GamesSmallHashPath());
    uint32_t index;
    for (const std::vector<uint32_t> v : games_with_small_hash_) {
      uint32_t size = (uint32_t) v.size();
      file.write((char*) &size, sizeof(uint32_t));
      file.write((char*) v.data(), v.size() * sizeof(uint32_t));
    }
  }

 private:
  FRIEND_TEST(ThorSourceTest, SaveLoadGamesSmallHash);
  std::string folder_;
  GameGetter game_getter_;
  std::vector<std::string> players_;
  std::vector<std::string> tournaments_;
  SequenceCanonicalizer* canonicalizer_;

  std::vector<uint32_t> game_index_by_black_;
  std::vector<uint32_t> game_index_by_white_;
  std::vector<uint32_t> game_index_by_tournament_;
  std::vector<uint32_t> game_index_by_year_;
  std::vector<std::vector<uint32_t>> games_with_small_hash_;

  std::vector<std::vector<uint32_t>*> game_indices_;

  int IncludeProbabilityToHashIndex(double include_probability) {
    // 1: -2
    // 0.5: -1
    // 0.49: 0 (here we start to filter)
    // 0.25: 0
    // 0.24: 1
    return log(0.25 / include_probability) / log(2);
  }

  void LoadGamesSmallHash() {
    std::vector<uint32_t> file = ReadFile<uint32_t>(GamesSmallHashPath());
    uint32_t index = 0;
    while (index < file.size()) {
      uint32_t size = file[index++];
      std::vector<uint32_t>& v = games_with_small_hash_.emplace_back();
      std::copy(file.begin() + index, file.begin() + index + size, std::back_inserter(v));
      index += size;
    }
  }

  void ComputeGamesSmallHash() {
    ElapsedTime t1;
    games_with_small_hash_.resize(IncludeProbabilityToHashIndex(0.5 / game_index_by_year_.size()));
    std::size_t max_hash = std::numeric_limits<std::size_t>::max();

    for (uint32_t i = 0; i < game_index_by_year_.size(); ++i) {
      auto* game = game_getter_.GetGame(game_index_by_year_[i]);
      auto probability = std::hash<Game>()(*game) / (double) max_hash;
      for (int j = std::min(IncludeProbabilityToHashIndex(probability), (int) games_with_small_hash_.size() - 1);
           j >= 0; --j) {
        games_with_small_hash_[j].push_back(i);
      }
    }
  }

  void FillNextMoves(const GamesInterval& interval, int sequence_size, std::unordered_map<Square, int>& next_moves) {
    for (auto it = interval.start; it < interval.end; ) {
      const Game* game = game_getter_.GetGame(*it);
      Square next_move = sequence_size < game->Moves().Size() + 1 ? game->Move(sequence_size) : kNoSquare;
      // If it=e6f4c3, and sequence_size=2, next_sequence will be e6f4c4.
      // If it=e6f4 and sequence_size=2, next_sequence will be e6f4, so that
      // next_it becomes equal to interval.end.
      Sequence next_sequence = game->Moves().Subsequence(sequence_size + (next_move == kNoSquare ? 0 : 1));
      auto next_it = std::upper_bound(
          it, interval.end,
          std::pair<short, Sequence>{game->Year(), next_sequence},
          CmpByYear(game_getter_));
      next_moves[next_move] += next_it - it;
      it = next_it;
    }
  }

  void SortAndCutExamples(int max_games, std::vector<const Game*>& examples) {
    std::sort(examples.begin(), examples.end(), CompareGamesByHash);
    if (examples.size() > max_games) {
      examples.resize(max_games);
    }
  }

  void ComputeSortedGames() {
    for (int i = 0; i < game_getter_.NumGames(); ++i) {
      game_index_by_black_.push_back(i);
      game_index_by_white_.push_back(i);
      game_index_by_tournament_.push_back(i);
      game_index_by_year_.push_back(i);
    }
    std::sort(game_index_by_black_.begin(), game_index_by_black_.end(), CmpByBlack(game_getter_));
    std::sort(game_index_by_white_.begin(), game_index_by_white_.end(), CmpByWhite(game_getter_));
    std::sort(game_index_by_tournament_.begin(), game_index_by_tournament_.end(), CmpByTournament(game_getter_));
    std::sort(game_index_by_year_.begin(), game_index_by_year_.end(), CmpByYear(game_getter_));
  }

  template<class T, class Compare>
  GamesInterval GetGameIntervals(const std::vector<uint32_t>& games, const T& search_key, Compare c) {
    return GamesInterval(
        std::lower_bound(games.begin(), games.end(), search_key, c),
        std::upper_bound(games.begin(), games.end(), search_key, c)
    );
  }

  std::vector<std::string> LoadListFromFile(const std::string& filepath, int size) {
    std::vector<std::string> result;
    std::ifstream file(filepath, std::ios::binary);
    std::vector<char> content = ReadFile<char>(filepath);
    if (content.empty()) {
      return {};
    }
    for (int i = 16; i < content.size(); i += size) {
      std::string value(content.data() + i, size);
      value.erase(value.find_last_not_of('\0') + 1, std::string::npos);
      result.push_back(value);
    }
    return result;
  }
};

#endif // OTHELLO_SENSEI_THOR_SOURCE_H