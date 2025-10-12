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

#include <algorithm> // Required for std::remove
#include <array>
#include <iomanip>
#include <iostream>
#include <string>

#include "../book/book.h"
#include "../thor/thor.h"
#include "../utils/misc.h"
#include "../utils/parse_flags.h"

struct BoardMetadata {
  std::string example_sequence;
  double eval;
  std::vector<const Game*> games;
  bool black_turn;

  BoardMetadata() : example_sequence(""), eval(-100), games(), black_turn(false) {}

  bool ToBeSaved(int min_players) const {
    std::vector<std::string> blacks;
    std::vector<std::string> whites;
    for (const Game* game : games) {
      if (!Contains(blacks, game->Black())) {
        blacks.push_back(game->Black());
      }
      if (!Contains(whites, game->White())) {
        whites.push_back(game->White());
      }
      if (blacks.size() >= min_players && whites.size() >= min_players) {
        return true;
      }
    }
    return false;
  }
};

typedef std::unordered_map<std::string, std::unordered_set<std::string>> PlayerGraph;

PlayerGraph GetPlayerGraph(const std::vector<Game>& games) {
  PlayerGraph player_graph;

  for (const auto& game : games) {
    if (game.Black().find('(') != std::string::npos || game.White().find('(') != std::string::npos) {
      continue;
    }
    player_graph[game.Black()].insert(game.White());
    player_graph[game.White()].insert(game.Black());
  }
  return player_graph;
}

void CleanPlayerNeighbors(PlayerGraph& player_graph, int min_num_games) {
  std::vector<std::string> players_to_remove;
  for (auto& [player, opponents] : player_graph) {
    if (opponents.size() < min_num_games) {
      players_to_remove.push_back(player);
    }
  }
  while (!players_to_remove.empty()) {
    std::string player_to_remove = players_to_remove.back();
    players_to_remove.pop_back();

    if (player_graph.find(player_to_remove) == player_graph.end()) {
      continue;
    }
    // Get a copy of the opponents before erasing the player.
    auto opponents_to_update = player_graph[player_to_remove];
    player_graph.erase(player_to_remove);

    // 3. Update neighbors: For each former opponent...
    for (const auto& opponent : opponents_to_update) {
      auto& opponent_neighbors = player_graph[opponent];
      opponent_neighbors.erase(player_to_remove);
      if (opponent_neighbors.size() < min_num_games) {
        players_to_remove.push_back(opponent);
      }
    }
  }
}

std::unordered_map<std::string, std::string> TournamentToFile(const Thor<GameGetterOnDisk>& thor) {
  std::unordered_map<std::string, std::string> result;
  for (const auto& [source, tournaments] : thor.Tournaments()) {
    for (const auto& tournament : tournaments) {
      result[tournament] = (tournaments.size() > 30 ? source : tournament) + ".csv";
    }
  }
  return result;
}

class FileSaver {
 public:
  FileSaver(const std::string& folder) : folder_(folder), file_cache_() {}

  void Save(const std::string& file, const std::stringstream& content, const std::string& header) {
    if (file_cache_.find(file) == file_cache_.end()) {
      file_cache_[file] = std::make_shared<std::ofstream>(folder_ + "/" + file);
      (*file_cache_[file]) << header;
    }
    (*file_cache_[file]) << content.str();
  }

 private:
  std::string folder_;
  std::unordered_map<std::string, std::shared_ptr<std::ofstream>> file_cache_;
};

std::unordered_map<Board, int> GetBoardToNumGames(std::vector<Game> games, PlayerGraph player_graph) {
  std::unordered_map<Board, int> board_to_num_games;
  int i = 0;
  for (const auto& game : games) {
    if (player_graph.find(game.Black()) == player_graph.end() ||
        player_graph.find(game.White()) == player_graph.end()) {
      continue;
    }
    for (const Board& board : game.Moves().ToBoards()) {
      board_to_num_games[board]++;
    }
    if (++i % 100000 == 0) {
      std::cout << "  Done " << i << " of " << games.size() << "\n";
    }
  }
  return board_to_num_games;
}

int main(int argc, char* argv[]) {
  ParseFlags parse_flags(argc, argv);
  std::string output_folder = parse_flags.GetFlag("output_folder");
  std::string archive_path = parse_flags.GetFlag("archive_path");
  std::string book_path = parse_flags.GetFlag("book_path");

  Thor<GameGetterOnDisk> thor(archive_path, "/tmp/saved_games.txt");
  std::unordered_map<std::string, std::string> tournament_to_file = TournamentToFile(thor);
  Book book(book_path);
  FileSaver saver(output_folder);

  std::vector<Game> games = thor.GetAllGames();
  std::cout << "Got " << games.size() << " games\n";
  PlayerGraph player_graph = GetPlayerGraph(games);
  std::cout << "Got " << player_graph.size() << " players\n";
  CleanPlayerNeighbors(player_graph, 20);
  std::cout << "Filtered " << player_graph.size() << " players with enough games\n";
  std::unordered_map<Board, BoardMetadata> board_to_metadata;

  int kept_games = 0;
  int i = 0;

  ElapsedTime t;
  std::string games_header = "black, white, tournament, black_score\n";
  std::string positions_header = "position, num_games, black, white, tournament, black_score, book_black_eval, turn, xot\n";

  std::cout << "First pass: get board to num games...\n";
  std::unordered_map<Board, int> board_to_num_games = GetBoardToNumGames(games, player_graph);

  std::cout << "Second pass: save games and fetch boards...\n";
  for (const auto& game : games) {
    if (++i % 100000 == 0) {
      std::cout << "  Done " << i << " of " << games.size() << " after " << t.Get() << " seconds\n";
    }
    if (player_graph.find(game.Black()) == player_graph.end() ||
        player_graph.find(game.White()) == player_graph.end()) {
      continue;
    }
    std::stringstream content;
    content
        << game.Black() << ", " << game.White() << ", " << game.Tournament() << ", "
        << 2 * game.Score() - 64 << "\n";
    saver.Save("Games - " + tournament_to_file[game.Tournament()], content, games_header);
    ++kept_games;
    auto boards = game.Moves().ToBoardsAndBlackTurns();
    for (int move = 0; move < boards.size(); ++move) {
      const Board& board = boards[move].first;
      if (board_to_num_games[board] < 10) {
        break;
      }
      BoardMetadata& metadata = board_to_metadata[board];
      metadata.games.push_back(&game);
      metadata.black_turn = boards[move].second;
      metadata.example_sequence = std::max(metadata.example_sequence, game.Moves().Subsequence(move).ToString());
    }
  }
  std::cout << "Kept " << kept_games << " games after filtering players\n";

  std::cout << "Third pass: save boards ...\n";
  i = 0;
  for (const auto& [board, metadata] : board_to_metadata) {
    std::unique_ptr<Node> book_node = book.Get(board);
    if (++i % 10000 == 0) {
      std::cout << "  Done " << i << " of " << board_to_metadata.size()  << " after " << t.Get() << " seconds\n";
    }
    bool xot = ToLower(metadata.games[0]->Tournament()).find("xot") != std::string::npos;
    if (!metadata.ToBeSaved(10) || (board.NEmpties() < 8 && xot)) {
      continue;
    }
    double eval = 100;
    if (book_node) {
      eval = book_node->GetEval();
    }
    std::string board_string = board.ToString();
    board_string.erase(std::remove(board_string.begin(), board_string.end(), '\n'), board_string.end());
    for (const Game* game : metadata.games) {
      std::stringstream content;
      content
          << metadata.example_sequence << ", " << metadata.games.size() << ", "
          << game->Black() << ", " << game->White() << ", " << game->Tournament() << ", "
          << 2 * game->Score() - 64 << ", " << (metadata.black_turn ? 1 : -1) * eval
          << ", " << (metadata.black_turn ? "B" : "W") << ", " << xot << "\n";
      saver.Save("Positions - " + tournament_to_file[game->Tournament()], content, positions_header);
    }
  }
}
