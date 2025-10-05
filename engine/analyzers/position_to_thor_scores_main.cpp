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
  Sequence example_sequence;
  double eval;
  int occurrences;
  std::vector<std::string*> blacks;
  std::vector<std::string*> whites;

  BoardMetadata() : example_sequence(""), eval(-100), occurrences(0), blacks(), whites() {}
};

int main(int argc, char* argv[]) {
  ParseFlags parse_flags(argc, argv);
  std::string output_positions = parse_flags.GetFlag("output_positions");
  std::string output_games = parse_flags.GetFlag("output_games");
  std::string archive_path = parse_flags.GetFlag("archive_path");
  std::string book_path = parse_flags.GetFlag("book_path");

  Thor thor(archive_path, "/tmp/saved_games.txt");
  Book book(book_path);

  auto games = thor.GetAllGames();
  std::cout << "Got " << games.size() << " games\n";
  std::ofstream output_positions_file(output_positions);
  std::ofstream output_games_file(output_games);
  output_positions_file << "sequence, occurrences, black, white, tournament, black_score, book_eval\n";
  output_games_file << "black, white, tournament, black_score\n";
  std::unordered_map<Board, BoardMetadata> board_to_metadata;
  int i = 0;
  for (auto& game : games) {
    output_games_file
        << game.Black() << ", " << game.White() << ", " << game.Tournament() << ", "
        << 2 * game.Score() - 64 << "\n";
    std::vector<Board> boards = game.Moves().ToBoards();
    for (int move = 0; move < boards.size(); ++move) {
      const Board& board = boards[move];
      BoardMetadata& metadata = board_to_metadata[board];
      if (metadata.eval < -64) {
        std::unique_ptr<Node> book_node = book.Get(board);
        if (book_node) {
          metadata.eval = book_node->GetEval();
        } else {
          metadata.eval = 100;
          break;
        }
        metadata.example_sequence = game.Moves().Subsequence(move);
      }
      ++metadata.occurrences;
    }
    if (++i % 10000 == 0) {
      std::cout << "Done " << i << " of " << games.size() << "\n";
    }
  }
  i = 0;
  for (auto& game : games) {
    std::vector<Board> boards = game.Moves().ToBoards();
    for (int move = 0; move < boards.size(); ++move) {
      const Board& board = boards[move];
      const BoardMetadata& metadata = board_to_metadata[board];
      if (metadata.occurrences < 5) {
        break;
      }
      std::string board_string = board.ToString();
      board_string.erase(std::remove(board_string.begin(), board_string.end(), '\n'), board_string.end());
      output_positions_file
          << metadata.example_sequence << ", " << metadata.occurrences << ", "
          << game.Black() << ", " << game.White() << ", " << game.Tournament() << ", "
          << 2 * game.Score() - 64 << ", " << metadata.eval << "\n";
    }
    if (++i % 10000 == 0) {
      std::cout << "Done " << i << " of " << games.size() << "\n";
    }
  }
}
