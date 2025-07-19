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

#ifndef OTHELLO_SENSEI_GENERIC_SOURCE_H
#define OTHELLO_SENSEI_GENERIC_SOURCE_H

#include <climits>
#include <string>
#include "game.h"
#include "../board/sequence.h"

enum GenericSourceType {
  SOURCE_TYPE_GAME_ARCHIVE,
  SOURCE_TYPE_SAVED_GAMES
};

class GenericSource {
 public:
  virtual ~GenericSource() = default;
  virtual const std::vector<std::string>& Tournaments() const = 0;
  virtual const std::vector<std::string>& Players() const = 0;
  virtual GamesList GetGames(
      const Sequence& sequence,
      int max_games,
      const std::vector<std::string>& blacks = {},
      const std::vector<std::string>& whites = {},
      const std::vector<std::string>& tournaments = {},
      short start_year = SHRT_MIN,
      short end_year = SHRT_MAX) const = 0;
  virtual int NumGames() const = 0;
  virtual void Save() const {};
  virtual std::vector<Game> AllGames() const = 0;
  virtual GenericSourceType GetType() const = 0;
  virtual std::string GetFolder() const = 0;
};


#endif //OTHELLO_SENSEI_GENERIC_SOURCE_H
