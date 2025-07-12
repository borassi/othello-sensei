/*
 * Copyright 2025 Michele Borassi
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

#ifndef OTHELLO_SENSEI_SAVED_GAME_LIST_H
#define OTHELLO_SENSEI_SAVED_GAME_LIST_H

#include <climits>
#include <string>
#include "../board/sequence.h"
#include "../thor/game.h"
#include "../utils/files.h"
#include "game_to_save.h"

class SavedGameList {
 public:
  SavedGameList(const std::string& folder, bool recursive) {
    std::unordered_map<std::string, int> players_to_index;
    std::unordered_map<std::string, int> tournaments_to_index;
    std::vector<GameToSave> games_to_save;
    LoadFolder(folder, players_to_index, tournaments_to_index, games_to_save, recursive);

    for (const auto& game_to_save : games_to_save) {
      games_.push_back(
          game_to_save.ToGame(
              &players_[players_to_index[game_to_save.Black()]],
              &players_[players_to_index[game_to_save.White()]],
              &tournaments_[tournaments_to_index[game_to_save.Tournament()]]
          )
      );
    }
    // TODO: SORT GAMES!!!
  }

  void LoadFolder(
      const std::string& folder,
      std::unordered_map<std::string, int>& players_to_index,
      std::unordered_map<std::string, int>& tournaments_to_index,
      std::vector<GameToSave>& games_to_save,
      bool recursive) {
    for (const std::string& path : GetAllFiles(folder, true, recursive)) {
      if (fs::is_directory(path)) {
        assert(recursive);
        LoadFolder(path, players_to_index, tournaments_to_index, games_to_save, true);
        continue;
      }
      if (!EndsWith(ToLower(path), ".stxt")) {
        continue;
      }
      auto game = GameToSave::FromString(LoadTextFile(path));
      for (const auto& player : {game.Black(), game.White()}) {
        auto [iterator, inserted] = players_to_index.insert({player, players_.size()});
        if (inserted) {
          players_.push_back(player);
        }
      }
      auto tournament = game.Tournament();
      auto [iterator, inserted] = tournaments_to_index.insert({tournament, tournaments_.size()});
      if (inserted) {
        tournaments_.push_back(tournament);
      }
      games_to_save.push_back(std::move(game));
    }
  }

  const std::vector<std::string>& Players() { return players_; }
  const std::vector<std::string>& Tournaments() { return tournaments_; }

  template<bool transpositions = true>
  GamesList GetGames(
      const Sequence& sequence,
      int max_games = 1,
      const std::vector<std::string>& blacks = {},
      const std::vector<std::string>& whites = {},
      const std::vector<std::string>& tournaments = {},
      short start_year = SHRT_MIN,
      short end_year = SHRT_MAX) const {
    Sequence canonical = sequence.ToCanonicalGame();
    GamesList result {.num_games = 0, .max_games = max_games};
    int sequence_size = sequence.Size();
    for (auto& game : games_) {
      if ((!blacks.empty() && !Contains(blacks, game.Black())) ||
          (!whites.empty() && !Contains(whites, game.White())) ||
          (!tournaments.empty() && !Contains(tournaments, game.Tournament())) ||
          game.Year() < start_year || game.Year() > end_year ||
          !game.Moves().StartsWith(canonical)) {
        continue;
      }
      result.next_moves[game.Moves().Move(sequence_size)]++;
      ++result.num_games;
      if (result.examples.size() < max_games) {
        result.examples.push_back(game);
      }
    }
    return result;
  }

 private:
  std::vector<Game> games_;
  std::vector<std::string> players_;
  std::vector<std::string> tournaments_;
};

class SavedGameLists {
 public:
  SavedGameLists(
      const std::vector<std::pair<std::string, bool>>& folders,
      const SequenceCanonicalizer& canonicalizer) : lists_(), canonicalizer_(canonicalizer) {
    for (auto& [folder, recursive] : folders) {
      lists_.emplace_back(folder, recursive);
    }
  }

 private:
  std::vector<SavedGameList> lists_;
  const SequenceCanonicalizer& canonicalizer_;
};


#endif //OTHELLO_SENSEI_SAVED_GAME_LIST_H
