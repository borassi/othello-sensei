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

#include <algorithm>
#include <climits>
#include <cstring>
#include <random>

#include "../board/sequence.h"
#include "../main/bindings.h"
#include "../utils/files.h"
#include "game.h"

constexpr int kPlayerLength = 20;
constexpr int kTournamentLength = 26;

// Compare by:
// - The value of f(g);
// - The sequence of f(g) (if f(g1) is a subsequence of f(g2), g1 > g2).
// - The game itself (just to break ties).
//
// When comparing a game and a sequence, they are equal if the sequence is a
// substring of the game (so that lower_bound is the first game with this
// sequence, upper_bound is the last).
template<class T, T f(const Game*)>
struct cmpGameAndSequence {
  bool operator()(const Game* g1, const Game* g2) {
    auto size = std::min(g1->Moves().Size(), g2->Moves().Size());
    return
        std::forward_as_tuple(f(g1), g1->Moves().Subsequence(size), g2->Moves().Size(), *g1) <
        std::forward_as_tuple(f(g2), g2->Moves().Subsequence(size), g1->Moves().Size(), *g2);
  }
  bool operator()(const Game* g, const std::pair<T, Sequence>& s) {
    // NOTE: We take the minimum so that lower_bound is before everything with
    // this sequence, upper_bound is after.
    auto size = std::min(g->Moves().Size(), s.second.Size());
    return
        std::forward_as_tuple(f(g), g->Moves().Subsequence(size), s.second.Size()) <
        std::forward_as_tuple(s.first, s.second.Subsequence(size), size);
  }
  bool operator()(const std::pair<T, Sequence>& s, const Game* g) {
    // NOTE: We take the minimum so that lower_bound is before everything with
    // this sequence, upper_bound is after.
    auto size = std::min(g->Moves().Size(), s.second.Size());
    return
        std::forward_as_tuple(f(g), g->Moves().Subsequence(size), s.second.Size()) >
        std::forward_as_tuple(s.first, s.second.Subsequence(size), size);
  }
};

inline bool True(const Game* g) { return true; }
inline std::string Black(const Game* g) { return g->Black(); }
inline std::string White(const Game* g) { return g->White(); }
inline std::string Tournament(const Game* g) { return g->Tournament(); }
inline short Year(const Game* g) { return g->Year(); }

struct cmpBySequence : cmpGameAndSequence<bool, True> {};
struct cmpByBlack : cmpGameAndSequence<std::string, Black> {};
struct cmpByWhite : cmpGameAndSequence<std::string, White> {};
struct cmpByTournament : cmpGameAndSequence<std::string, Tournament> {};
struct cmpByYear : cmpGameAndSequence<short, Year> {};

struct GamesList {
  int num_games = 0;
  std::vector<const Game*> examples;
  std::unordered_map<Square, int> next_moves;
  int max_games = 0;

  void Merge(const GamesList& other) {
    num_games += other.num_games;
    examples.insert(examples.end(), other.examples.begin(), other.examples.end());
    std::stable_sort(examples.begin(), examples.end(), [](const Game* g1, const Game* g2) { return g1->Year() > g2->Year(); });
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
  std::vector<const Game*>::const_iterator start;
  std::vector<const Game*>::const_iterator end;

  GamesInterval() {}
  GamesInterval(
      std::vector<const Game*>::const_iterator start,
      std::vector<const Game*>::const_iterator end) : start(start), end(end) {}
};

class Source {
 public:
  Source(const std::string& folder, bool rebuild_games_order = false) : folder_(folder), players_(), tournaments_(), min_year_(SHRT_MAX), max_year_(SHRT_MIN) {
    ElapsedTime t;
    std::set<std::string> game_files;
    for (const auto& entry : fs::directory_iterator(folder)) {
      if (!fs::is_regular_file(entry)) {
        continue;
      }
      if (ToLower(entry.path().extension().string()) == ".jou") {
        assert(players_.empty());
        players_ = LoadListFromFile(entry.path().string(), kPlayerLength);
      } else if (ToLower(entry.path().extension().string()) == ".trn") {
        assert(tournaments_.empty());
        tournaments_ = LoadListFromFile(entry.path().string(), kTournamentLength);
      } else if (ToLower(entry.path().extension().string()) == ".wtb") {
        game_files.insert(entry.path().string());
      }
    }
    for (const auto& game_file : game_files) {
      LoadGames(game_file);
    }
    if (fs::exists(SortedGamesPath()) && !rebuild_games_order) {
      LoadSortedGames();
    } else {
      ComputeSortedGames();
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
    start_year = std::max(start_year, min_year_);
    end_year = std::min(end_year, max_year_);
    std::vector<GamesInterval> intervals;
    bool must_iterate_over_games = true;

    GamesInterval games_interval;
    if (!blacks.empty()) {
      for (const std::string& black : blacks) {
        games_interval = GetGameIntervals<std::pair<std::string, Sequence>>(
            games_by_black_, {black, sequence}, cmpByBlack());
        intervals.push_back(games_interval);
      }
    } else if (!whites.empty()) {
      for (const std::string& white : whites) {
        games_interval = GetGameIntervals<std::pair<std::string, Sequence>>(
            games_by_white_, {white, sequence}, cmpByWhite());
        intervals.push_back(games_interval);
      }
    } else if (!tournaments.empty()) {
      for (const std::string& tournament : tournaments) {
        games_interval = GetGameIntervals<std::pair<std::string, Sequence>>(
            games_by_tournament_, {tournament, sequence}, cmpByTournament());
        intervals.push_back(games_interval);
      }
    } else {
      for (int year = end_year; year >= start_year; --year) {
        games_interval = GetGameIntervals<std::pair<int, Sequence>>(
            games_by_year_, {year, sequence}, cmpByYear());
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
          const Game* game = *it;
          if ((blacks.empty() || Contains(blacks, game->Black())) &&
              (whites.empty() || Contains(whites, game->White())) &&
              (tournaments.empty() || Contains(tournaments, game->Tournament())) &&
              game->Year() >= start_year && game->Year() <= end_year) {
            result.examples.push_back(game);
            result.next_moves[game->Move(sequence_size)]++;
          }
        }
      }
      result.num_games = (int) result.examples.size();
      std::sort(result.examples.begin(), result.examples.end(), [](const Game* g1, const Game* g2) {
        return
            std::forward_as_tuple(-g1->Year(), std::hash<Game>()(*g1), *g1) <
            std::forward_as_tuple(-g2->Year(), std::hash<Game>()(*g2), *g2);
        }
      );
      if (result.examples.size() > max_games) {
        result.examples.resize(max_games);
      }
    } else {
      for (const GamesInterval& interval : intervals) {
        auto cur_games = interval.end - interval.start;
        result.num_games += (int) cur_games;
        if (result.examples.size() + cur_games <= max_games) {
          result.examples.insert(result.examples.end(), interval.start, interval.end);
        } else if (result.examples.size() < max_games) {
          std::sample(interval.start, interval.end, std::back_inserter(result.examples), max_games - result.examples.size(), std::mt19937 {42});
        } else {
          assert(result.examples.size() == max_games);
        }
        for (auto it = interval.start; it != interval.end; ) {
          Square next_move = sequence_size < (*it)->Moves().Size() + 1 ? (*it)->Move(sequence_size) : kNoSquare;
          // If it=e6f4c3, and sequence_size=2, next_sequence will be e6f4c4.
          // If it=e6f4 and sequence_size=2, next_sequence will be e6f4, so that
          // next_it becomes equal to interval.end.
          Sequence next_sequence = (*it)->Moves().Subsequence(sequence_size + (next_move == kNoSquare ? 0 : 1));
          auto next_it = std::upper_bound(
              it, interval.end,
              std::pair<short, Sequence>{(*it)->Year(), next_sequence},
              cmpByYear());
          result.next_moves[next_move] += (int) (next_it - it);
          it = next_it;
        }
      }
    }
    return result;
  }

  const std::vector<Game>& AllGames() const { return games_; }

  std::string SortedGamesPath() const {
    return folder_ + "/games_order.sen";
  }

  void SaveSortedGames() const {
    std::ofstream file(SortedGamesPath());
    uint32_t index;
    for (const std::vector<const Game*>* v : {&games_by_black_, &games_by_white_, &games_by_tournament_, &games_by_year_}) {
      for (const Game* game_pointer : *v) {
        index = (int) (game_pointer - games_.data());
        assert(index >= 0 && index < games_.size());
        file.write((char*) &index, sizeof(uint32_t));
      }
    }
  }

  virtual void LoadSortedGames() {
    std::vector<uint32_t> file = ReadFile<uint32_t>(SortedGamesPath());
    int position = 0;

    for (std::vector<const Game*>* v : {&games_by_black_, &games_by_white_, &games_by_tournament_, &games_by_year_}) {
      for (int i = 0; i < games_.size(); ++i) {
        assert(file[position] >= 0 && file[position] < games_.size());
        v->push_back(games_.data() + file[position]);
        ++position;
      }
    }
  }

 private:
  std::string folder_;
  std::vector<Game> games_;
  std::vector<std::string> players_;
  std::vector<std::string> tournaments_;
  SequenceCanonicalizer* canonicalizer_;
  short min_year_;
  short max_year_;

  std::vector<const Game*> games_by_black_;
  std::vector<const Game*> games_by_white_;
  std::vector<const Game*> games_by_tournament_;
  std::vector<const Game*> games_by_year_;

  void ComputeSortedGames() {
    for (const Game& game : games_) {
      games_by_black_.push_back(&game);
      games_by_white_.push_back(&game);
      games_by_tournament_.push_back(&game);
      games_by_year_.push_back(&game);
    }
    std::sort(games_by_black_.begin(), games_by_black_.end(), cmpByBlack());
    std::sort(games_by_white_.begin(), games_by_white_.end(), cmpByWhite());
    std::sort(games_by_tournament_.begin(), games_by_tournament_.end(), cmpByTournament());
    std::sort(games_by_year_.begin(), games_by_year_.end(), cmpByYear());
  }

  template<class T, class Compare>
  GamesInterval GetGameIntervals(const std::vector<const Game*>& games, const T& search_key, Compare c) {
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

  void LoadGames(const std::string& filepath) {
    ElapsedTime t;
    std::vector<char> games = ReadFile<char>(filepath);
    if (games.size() == 0) {
      return;
    }
    short year = *((short*) &games[10]);
    min_year_ = std::min(min_year_, year);
    max_year_ = std::max(max_year_, year);
    for (int i = 16; i < games.size(); i += 68) {
      auto game = games_.emplace_back(games.data(), i, year, players_, tournaments_);
    }
  }
};
