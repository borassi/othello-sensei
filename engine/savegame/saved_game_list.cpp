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

#include "saved_game_list.h"

GamesList SavedGameList::GetGames(
    const Sequence& sequence,
    int max_games,
    const std::vector<std::string>& blacks,
    const std::vector<std::string>& whites,
    const std::vector<std::string>& tournaments,
    short start_year,
    short end_year) const {
  Sequence canonical = sequence.ToCanonicalGame();
  GamesList result;
  result.num_games = 0;
  result.max_games = max_games;
  int sequence_size = (int) sequence.Size();
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
    if ((int) result.examples.size() < max_games) {
      result.examples.push_back(game);
    }
  }
  return result;
}

void SavedGameList::LoadFolder(
    const std::string& folder,
    std::unordered_map<std::string, int>& players_to_index,
    std::unordered_map<std::string, int>& tournaments_to_index,
    std::vector<std::pair<fs::file_time_type, GameToSave>>& games_to_save,
    int* remaining_steps) {
  if (*remaining_steps <= 0) {
    return;
  }
  for (const std::string& path : GetAllFilesMostRecentFirst(folder, true, true)) {
    if ((*remaining_steps)-- <= 0) {
      return;
    }
    if (fs::is_directory(path)) {
      LoadFolder(path, players_to_index, tournaments_to_index, games_to_save, remaining_steps);
      continue;
    }
    if (!EndsWith(ToLower(path), ".sensei.txt")) {
      continue;
    }
    auto time = fs::last_write_time(path);
    auto game = GameToSave::FromString(LoadTextFile(path));
    for (const auto& player : {game.Black(), game.White()}) {
      auto [iterator, inserted] = players_to_index.insert({player, (int) players_.size()});
      if (inserted) {
        players_.push_back(player);
      }
    }
    auto tournament = game.Tournament();
    auto [iterator, inserted] = tournaments_to_index.insert({tournament, (int) tournaments_.size()});
    if (inserted) {
      tournaments_.push_back(tournament);
    }
    games_to_save.push_back({time, std::move(game)});
  }
}