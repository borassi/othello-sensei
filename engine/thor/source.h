/*
 * Copyright 2024-2025 Michele Borassi
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
#include <random>

#include "game.h"
#include "generic_source.h"
#include "../board/sequence.h"
#include "../main/bindings.h"
#include "../utils/files.h"

constexpr int kPlayerLength = 20;
constexpr int kTournamentLength = 26;

struct GameFile {
  GameFile(const std::string& path, uint32_t offset) :
      path(path), offset(offset), num_games(0), year(0) {
    std::fstream file = File();
    FileOffset length = FileLength(file);
    if (length % 68 != 16) {
      std::cout << "WARNING: Wrong length for Thor file " << path << ". Expected 16+68k. Got " << length << ".\n" << std::flush;
      return;
    }
    num_games = (uint32_t) ((length - 16) / 68);
    file.seekg(10);
    file.read((char*) &year, sizeof(year));
  }

  std::fstream File() {
    return std::fstream(path, std::ios::in | std::ios::binary);
  }
  std::string path;
  uint32_t offset;
  uint32_t num_games;
  short year;
};

class GameGetterOnDisk {
 public:
  explicit GameGetterOnDisk(const std::string& folder) :
      game_files_(), index_to_file_(), games_cache_(), min_year_(SHRT_MAX), max_year_(SHRT_MIN) {
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
    if (game_files.empty() || players_.empty() || tournaments_.empty()) {
      min_year_ = 0;
      max_year_ = 0;
      return;
    }
    for (const auto& game_file : game_files) {
      LoadGames(game_file);
    }
    char buffer[68];
    auto file = game_files_[0].File();
    file.seekg(16);
    file.read(buffer, 68 * sizeof(char));
    Game random_game(buffer, 0, game_files_[0].year, players_, tournaments_, 1);
    games_cache_.reserve(kHashSize);
    for (int i = 0; i < kHashSize; ++i) {
      games_cache_.emplace_back(-1, random_game);
    }
  }

  virtual Game GetGame(int index) const {
    auto hash = std::hash<int>()(index) % kHashSize;
    std::pair<int, Game>* cached = &games_cache_[hash];
    if (cached->first == index) {
      return cached->second;
    }
    GameFile& game_file = game_files_[index_to_file_[index]];
    auto file = game_file.File();
    char game[68];
    file.seekg(16 + 68 * (index - game_file.offset));
    file.read(game, 68 * sizeof(char));
    Game result(game, 0, game_file.year, players_, tournaments_,
         (index - game_file.offset) / (double) game_file.num_games);
    *cached = {index, result};
    return result;
  }

  virtual std::vector<Game> GetAllGames() const {
    std::vector<Game> result;
    for (const GameFile& game_file : game_files_) {
      std::vector<char> games = ReadFile<char>(game_file.path);
      for (int i = 16; i < games.size(); i += 68) {
        result.emplace_back(games.data(), i, game_file.year, players_, tournaments_, (i - 16.0) / (games.size() - 16.0));
      }
    }
    return result;
  }
  virtual int NumGames() const { return (int) index_to_file_.size(); }
  virtual short MinYear() const { return min_year_; }
  virtual short MaxYear() const { return max_year_; }
  virtual const std::vector<std::string>& Players() const { return players_; }
  virtual const std::vector<std::string>& Tournaments() const { return tournaments_; }

 private:
  mutable std::vector<GameFile> game_files_;
  std::vector<int> index_to_file_;
  std::vector<std::string> players_;
  std::vector<std::string> tournaments_;
  mutable std::vector<std::pair<int, Game>> games_cache_;
  short min_year_;
  short max_year_;
  static constexpr int kHashSize = 5000;

  void LoadGames(const std::string& filepath) {
    GameFile& game_file = game_files_.emplace_back(filepath, (uint32_t) index_to_file_.size());
    min_year_ = std::min(min_year_, game_file.year);
    max_year_ = std::max(max_year_, game_file.year);
    index_to_file_.insert(index_to_file_.end(), game_file.num_games, (int) game_files_.size() - 1);
  }

  static std::vector<std::string> LoadListFromFile(const std::string& filepath, int size) {
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

class GameGetterInMemory : public GameGetterOnDisk {
 public:
  explicit GameGetterInMemory(const std::string& folder) : GameGetterOnDisk(folder) {
    all_games_ = GetAllGames();
  }
  Game GetGame(int index) const override {
    return all_games_[index];
  }

 private:
  std::vector<Game> all_games_;
};

struct GamesInterval {
  std::vector<uint32_t>::const_iterator start;
  std::vector<uint32_t>::const_iterator end;

  GamesInterval() = default;
  GamesInterval(std::vector<uint32_t>::const_iterator start,
                std::vector<uint32_t>::const_iterator end) : start(start), end(end) {}
};

template<class GameGetter = GameGetterOnDisk>
class Source : public GenericSource {
 public:
  explicit Source(const std::string& folder, bool rebuild_games_order = false, bool rebuild_games_with_small_hash = false) :
      folder_(folder), game_getter_(folder) {
    if (game_getter_.NumGames() == 0) {
      return;
    }
    ElapsedTime t;
    game_indices_ = {&game_index_by_black_, &game_index_by_white_, &game_index_by_tournament_, &game_index_by_year_};
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

  const std::vector<std::string>& Tournaments() const override { return game_getter_.Tournaments(); }
  const std::vector<std::string>& Players() const override { return game_getter_.Players(); }

  GamesList GetGames(
      const Sequence& sequence,
      int max_games,
      const std::vector<std::string>& blacks = {},
      const std::vector<std::string>& whites = {},
      const std::vector<std::string>& tournaments = {},
      short start_year = SHRT_MIN,
      short end_year = SHRT_MAX) const override;

  int NumGames() const override { return game_getter_.NumGames(); }

  std::vector<Game> AllGames() const override { return game_getter_.GetAllGames(); }

  std::string SortedGamesPath() const { return folder_ + "/games_order.sen"; }

  std::string GamesSmallHashPath() const { return folder_ + "/games_with_small_hash.sen"; }

  GenericSourceType GetType() const override { return SOURCE_TYPE_GAME_ARCHIVE; }

  std::string GetFolder() const override { return folder_; }

  void Save() const override {
    SaveSortedGames();
    SaveGamesSmallHash();
  }

  void SaveSortedGames() const {
    std::ofstream file(SortedGamesPath(), std::ios::binary);
    for (const std::vector<uint32_t>* v : game_indices_) {
      file.write((char*) v->data(), v->size() * sizeof(uint32_t));
    }
  }

  void SaveGamesSmallHash() const {
    std::ofstream file(GamesSmallHashPath(), std::ios::binary);
    for (const std::vector<uint32_t> v : games_with_small_hash_) {
      uint32_t size = (uint32_t) v.size();
      file.write((char*) &size, sizeof(uint32_t));
      file.write((char*) v.data(), v.size() * sizeof(uint32_t));
    }
  }

 private:
  std::string folder_;
  GameGetter game_getter_;

  std::vector<uint32_t> game_index_by_black_;
  std::vector<uint32_t> game_index_by_white_;
  std::vector<uint32_t> game_index_by_tournament_;
  std::vector<uint32_t> game_index_by_year_;
  std::vector<std::vector<uint32_t>> games_with_small_hash_;

  std::vector<std::vector<uint32_t>*> game_indices_;

  int IncludeProbabilityToHashIndex(double include_probability) const {
    // 0: -2
    // 0.5: -1
    // 0.51: 0 (here we start to filter)
    // 0.75: 0
    // 0.76: 1
    return (int) std::max(-2.0, log(0.25 / (1-include_probability)) / log(2));
  }

  void LoadGamesSmallHash() {
    std::vector<uint32_t> file = ReadFile<uint32_t>(GamesSmallHashPath());
    uint32_t index = 0;
    while (index < file.size()) {
      uint32_t size = file[index++];
      std::vector<uint32_t>& v = games_with_small_hash_.emplace_back();
      v.resize(size);
      memcpy(v.data(), file.data() + index, size * sizeof(uint32_t));
      index += size;
    }
  }

  void ComputeGamesSmallHash() {
    ElapsedTime t1;
    games_with_small_hash_.resize(IncludeProbabilityToHashIndex(1 - 0.5 / game_index_by_year_.size()));
    GameGetterInMemory game_getter(folder_);

    for (uint32_t i = 0; i < game_index_by_year_.size(); ++i) {
      Game game = game_getter.GetGame(game_index_by_year_[i]);
      auto probability = game.Priority();
      for (int j = std::min(IncludeProbabilityToHashIndex(probability), (int) games_with_small_hash_.size() - 1);
           j >= 0; --j) {
        games_with_small_hash_[j].push_back(i);
      }
    }
  }

  void LoadSortedGames() {
    std::fstream file(SortedGamesPath(), std::ios::in | std::ios::binary);
    int num_games = game_getter_.NumGames();
    assert(num_games == FileLength(file) / sizeof(uint32_t) / game_indices_.size());

    for (auto& game_indices : game_indices_) {
      game_indices->resize(num_games);
      file.read((char*) game_indices->data(), static_cast<int>(num_games * sizeof(uint32_t)));
    }
  }

  void ComputeSortedGames() {
    for (int i = 0; i < game_getter_.NumGames(); ++i) {
      game_index_by_black_.push_back(i);
      game_index_by_white_.push_back(i);
      game_index_by_tournament_.push_back(i);
      game_index_by_year_.push_back(i);
    }
    std::sort(game_index_by_black_.begin(), game_index_by_black_.end(), CmpByBlack<GameGetter>(game_getter_));
    std::sort(game_index_by_white_.begin(), game_index_by_white_.end(), CmpByWhite<GameGetter>(game_getter_));
    std::sort(game_index_by_tournament_.begin(), game_index_by_tournament_.end(), CmpByTournament<GameGetter>(game_getter_));
    std::sort(game_index_by_year_.begin(), game_index_by_year_.end(), CmpByYear<GameGetter>(game_getter_));
  }

  void FillNextMoves(const GamesInterval& interval, int sequence_size, std::unordered_map<Square, int>& next_moves) const {
    for (auto it = interval.start; it < interval.end; ) {
      Game game = game_getter_.GetGame(*it);
      Square next_move = sequence_size < game.Moves().Size() + 1 ? game.Move(sequence_size) : kNoSquare;
      // If it=e6f4c3, and sequence_size=2, next_sequence will be e6f4c4.
      // If it=e6f4 and sequence_size=2, next_sequence will be e6f4, so that
      // next_it becomes equal to interval.end.
      Sequence next_sequence = game.Moves().Subsequence(sequence_size + (next_move == kNoSquare ? 0 : 1));
      auto next_it = std::upper_bound(
          it, interval.end,
          std::pair<short, Sequence>{game.Year(), next_sequence},
          CmpByYear<GameGetter>(game_getter_));
      next_moves[next_move] += (int) (next_it - it);
      it = next_it;
    }
  }

  template<class T, class Compare>
  GamesInterval GetGameIntervals(const std::vector<uint32_t>& games, const T& search_key, Compare c) const {
    return GamesInterval(
        std::lower_bound(games.begin(), games.end(), search_key, c),
        std::upper_bound(games.begin(), games.end(), search_key, c)
    );
  }
};

#endif // OTHELLO_SENSEI_THOR_SOURCE_H