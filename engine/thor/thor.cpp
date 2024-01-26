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
//
//#include "thor.h"
//
//Thor::Thor(std::string folder) {
//  for (std::string source : sources) {
//    players_for_source_[source] = LoadListFromFile(folder + "/" + source + ".JOU", kPlayerLength);
//    tournaments_for_source_[source] = LoadListFromFile(folder + "/" + source + ".TRN", kTournamentLength);
//  }
//
//  for (int i = GetCurrentYear(); i >= 1977; --i) {
//    std::vector<std::string> files;
//    for (std::string source : sources) {
//      LoadGames(
//          folder + "/" + source + "_" + std::to_string(i) + ".wtb",
//          players_for_source_[source],
//          tournaments_for_source_[source]
//      );
//    }
//  }
//}
//
//std::vector<std::string> Thor::Tournaments() const {
//  std::vector<std::string> result;
//  std::unordered_set<std::string> seen;
//  for (const Game& game : games_) {
//    auto tournament = game.Tournament();
//    if (seen.insert(tournament).second) {
//      result.push_back(tournament);
//    }
//  }
//  std::sort(result.begin(), result.end());
//  return result;
//}
//
//std::vector<std::string> Thor::Players() const {
//  std::vector<std::string> result;
//  std::unordered_set<std::string> seen;
//  for (const Game& game : games_) {
//    for (auto& player : {game.Black(), game.White()}) {
//      if (seen.insert(player).second) {
//        result.push_back(player);
//      }
//    }
//  }
//  std::sort(result.begin(), result.end());
//  return result;
//}
//
//void Thor::LookupPositions(std::unordered_set<std::string> black, std::unordered_set<std::string> white, std::unordered_set<std::string> tournaments) {
//  games_for_position_.clear();
//  int k = 0;
//  std::cout << games_.size() << "\n";
//  for (Game& game : games_) {
//    if (k++ % 10000 == 0)
//      std::cout << k << "\n";
//    if ((!black.empty() && !Contains(black, game.Black())) ||
//        (!white.empty() && !Contains(white, game.White())) ||
//        (!tournaments.empty() && !Contains(tournaments, game.Tournament()))) {
//      continue;
//    }
//    Board b;
//    const auto& moves = game.Moves().Moves();
//    AddGameToPosition(b, &game);
//    for (auto move : moves) {
//      if (move == kNoSquare) {
//        break;
//      }
//      b.PlayMove(GetFlip(move, b.Player(), b.Opponent()));
//      if (HaveToPass(b.Player(), b.Opponent())) {
//        b.PlayMove(0);
//      }
//      AddGameToPosition(b, &game);
//    }
//  }
//}
//
//std::vector<const Game*> Thor::GetGames(const Board& child) const {
//  std::vector<const Game*> result;
//  std::unordered_set<Board> seen_boards;
//  for (const Board& b : child.AllTranspositions()) {
//    if (Contains(seen_boards, b)) {
//      continue;
//    }
//    seen_boards.insert(b);
//    auto games = GetOrDefault(games_for_position_, b, {});
////    result.insert(result.end(), games.begin(), games.end());
//  }
//  return result;
//}
//
//std::vector<std::string> Thor::LoadListFromFile(const std::string& filepath, int size) {
//  std::vector<std::string> result;
//  std::ifstream file(filepath, std::ios::binary);
//  std::vector<char> content = ReadFile<char>(filepath);
//  if (content.empty()) {
//    return {};
//  }
//  for (int i = 16; i < content.size(); i += size) {
//    std::string value(content.data() + i, size);
//    value.erase(value.find_last_not_of('\0') + 1, std::string::npos);
//    result.push_back(value);
//  }
//  return result;
//}
//
//void Thor::LoadGames(const std::string& filepath, const std::vector<std::string>& players, const std::vector<std::string>& tournaments) {
//  std::vector<char> games = ReadFile<char>(filepath);
//  if (games.size() == 0) {
//    return;
//  }
//  short year = *((short*) &games[10]);
//  for (int i = 16; i < games.size(); i += 68) {
//    games_.emplace_back(games.data(), i, year, players, tournaments);
//  }
//}