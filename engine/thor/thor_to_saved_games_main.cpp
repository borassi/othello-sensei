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

#include <string>

#include "thor.h"
#include "../utils/parse_flags.h"
#include "../savegame/game_to_save.h"

int main(int argc, char* argv[]) {
  ParseFlags parse_flags(argc, argv);
  std::string thor_path = parse_flags.GetFlag("thor_path");
  std::string output_folder = parse_flags.GetFlag("output_folder");

  fs::create_directories(fs::path(output_folder));
  Thor thor(thor_path, "/tmp/saved_files.txt");

  auto games = thor.GetGamesFromAllSources(Sequence("e6"), 10000);

  ElapsedTime t;
  int i = 0;
  std::cout << games.num_games << " " << games.examples.size() << "\n";
  for (const Game& game : games.examples) {
    GameToSave game_to_save = GameToSave::FromGame(game);
    std::ofstream output(output_folder + "/" + std::to_string(i++) + " - " + game_to_save.Black() + " - " + game_to_save.White() + ".sensei.txt");
    output << game_to_save.ToString();
    output.close();
  }
  std::cout << "Saved in " << t.Get() << "\n";

  SavedGameList source("/home/michele/Desktop/sensei/saved_games", false);
  std::cout << "Loaded in " << t.Get() << "\n";
  std::cout << "Num games: " << source.GetGames(Sequence("e6")).num_games << "\n";
  std::cout << "Get games in " << t.Get();
}