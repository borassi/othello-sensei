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

#include "game_to_save.h"
#include "../board/board.h"
#include "../board/sequence.h"

int EstimatedScore(const Sequence& sequence) {
  auto [board, black_turn] = sequence.ToBoardAndBlackTurn();
  if (IsGameOver(board)) {
    return (GetEvaluationGameOver(board.Player(), board.Opponent()) * (black_turn ? 1 : -1) + 64) / 2;
  }
  return (black_turn ? 0 : 64);
}

// static
GameToSave GameToSave::FromString(const std::string& game) {
  std::regex word_regex("\\s*(.*):\\s*(.*)\\s*");

  auto game_begin =
      std::sregex_iterator(game.begin(), game.end(), word_regex);
  auto game_end = std::sregex_iterator();
  std::unordered_map<std::string, std::string> game_map;

  int first_subfield = (int) game.size();
  for (std::sregex_iterator i = game_begin; i != game_end; ++i) {
    std::smatch match = *i;
    if (!Contains(std::vector<std::string>({"Black", "White", "Tournament", "Year", "Notes", "Black disks"}), match[1].str())) {
      continue;
    }
    first_subfield = std::min(first_subfield, (int) i->position());
    game_map[match[1].str()] = match[2].str();
  }
  Sequence sequence = Sequence::ParseFromString(first_subfield == -1 ? game : game.substr(0, first_subfield));

  int black_disks;
  try {
    black_disks = stoi(GetOrDefault(game_map, std::string("Black disks"), std::string("")));
  } catch (std::invalid_argument) {
    black_disks = EstimatedScore(sequence);
  }
  int year;
  try {
    year = stoi(GetOrDefault(game_map, std::string("Year"), std::string("")));
  } catch (std::invalid_argument) {
    year = GetCurrentYear();
  }

  return GameToSave(
      sequence,
      GetOrDefault(game_map, std::string("Black"), std::string("")),
      GetOrDefault(game_map, std::string("White"), std::string("")),
      GetOrDefault(game_map, std::string("Tournament"), std::string("")),
      GetOrDefault(game_map, std::string("Notes"), std::string("")),
      year,
      black_disks
  );
}

std::ostream& operator<<(std::ostream& stream, const GameToSave& s) {
  stream << s.ToGame().ToString();
  return stream;
}