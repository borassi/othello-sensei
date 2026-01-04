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
#include "../thor/generic_source.h"
#include "../utils/files.h"
#include "game_to_save.h"

class SavedGameList : public GenericSource {
 public:
  static constexpr int kDefaultMaxGames = 2000;

  SavedGameList(const std::string& folder, int max_games = kDefaultMaxGames) : folder_(folder) {
    std::unordered_map<std::string, int> players_to_index;
    std::unordered_map<std::string, int> tournaments_to_index;
    std::vector<std::pair<fs::file_time_type, GameToSave>> games_to_save;
    LoadFolder(folder, players_to_index, tournaments_to_index, games_to_save, &max_games);

    std::sort(
        games_to_save.begin(),
        games_to_save.end(),
        [](const std::pair<fs::file_time_type, GameToSave>& lhs, const std::pair<fs::file_time_type, GameToSave>& rhs) {
          return lhs.first > rhs.first;
        });
    for (const auto& [unused_time, game_to_save] : games_to_save) {
      games_.push_back(
          game_to_save.ToGame(
              /*canonicalize=*/true,
              &players_[players_to_index[game_to_save.Black()]],
              &players_[players_to_index[game_to_save.White()]],
              &tournaments_[tournaments_to_index[game_to_save.Tournament()]]
          )
      );
    }
  }

  const std::vector<std::string>& Players() const override { return players_; }
  const std::vector<std::string>& Tournaments() const override { return tournaments_; }
  GenericSourceType GetType() const override { return SOURCE_TYPE_SAVED_GAMES; }
  std::string GetFolder() const override { return folder_; }

  GamesList GetGames(
      const Sequence& sequence,
      int max_games = 1,
      const std::vector<std::string>& blacks = {},
      const std::vector<std::string>& whites = {},
      const std::vector<std::string>& tournaments = {},
      short start_year = SHRT_MIN,
      short end_year = SHRT_MAX) const override;

  int NumGames() const override { return (int) games_.size(); }

  std::vector<Game> AllGames() const override { return games_; }

 private:
  std::string folder_;
  std::vector<Game> games_;
  std::vector<std::string> players_;
  std::vector<std::string> tournaments_;

  void LoadFolder(
      const std::string& folder,
      std::unordered_map<std::string, int>& players_to_index,
      std::unordered_map<std::string, int>& tournaments_to_index,
      std::vector<std::pair<fs::file_time_type, GameToSave>>& games_to_save,
      int* remaining_steps);
};

#endif //OTHELLO_SENSEI_SAVED_GAME_LIST_H
