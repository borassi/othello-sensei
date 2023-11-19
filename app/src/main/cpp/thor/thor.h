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
 
#ifndef OTHELLO_SENSEI_THOR_H
#define OTHELLO_SENSEI_THOR_H

// Copyright 2023 Michele Borassi
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <fstream>
#include <map>
#include <unordered_set>

#include "game.h"
#include "../board/board.h"
#include "../board/get_flip.h"
#include "../board/get_moves.h"
#include "../utils/files.h"
#include "../utils/misc.h"

const std::vector<std::string> sources = {"WTH", "PLAYOK", "QUEST"};

class Thor {
 public:
  Thor(std::string folder);

  std::vector<std::string> Tournaments() const;

  std::vector<std::string> Players() const;

  void LookupPositions(std::unordered_set<std::string> black, std::unordered_set<std::string> white, std::unordered_set<std::string> tournaments);

  std::vector<Game> GetGames(const Board& child) const;

  int GetNumGames(const Board& child) {
    return GetGames(child).size();
  }

 private:
  std::vector<Game> games_;
  std::unordered_map<Board, std::vector<Game>> games_for_position_;
  std::unordered_map<std::string, std::vector<std::string>> players_for_source_;
  std::unordered_map<std::string, std::vector<std::string>> tournaments_for_source_;

  void AddGameToPosition(Board b, Game game) {
    if (!Contains(games_for_position_, b)) {
      games_for_position_[b] = std::vector<Game>();
    }
    games_for_position_[b].push_back(game);
  }

  std::vector<std::string> LoadListFromFile(const std::string& filepath, int size);

  void LoadGames(const std::string& filepath, const std::vector<std::string>& players, const std::vector<std::string>& tournaments);
};

#endif //OTHELLO_SENSEI_THOR_H
