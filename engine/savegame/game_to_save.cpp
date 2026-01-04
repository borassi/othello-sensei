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
#include "../utils/misc.h"

int EstimatedScore(const Sequence& sequence) {
  auto [board, black_turn] = sequence.ToBoardAndBlackTurn();
  if (IsGameOver(board)) {
    return (GetEvaluationGameOver(board.Player(), board.Opponent()) * (black_turn ? 1 : -1) + 64) / 2;
  }
  return (black_turn ? 0 : 64);
}

// static
GameToSave GameToSave::FromString(const std::string& game) {
  std::unordered_map<std::string, std::string> game_map;

  std::string moves;
  for (const std::string& line : Split(game, '\n')) {
    auto colon_position = line.find(':');
    if (colon_position == -1) {
      moves += line;
      continue;
    }
    std::string key = LeftStrip(RightStrip(line.substr(0, colon_position)));
    if (!Contains(std::vector<std::string>({"Black", "White", "Tournament", "Year", "Notes", "Black disks", "Round"}), key)) {
      continue;
    }
    std::string value = LeftStrip(RightStrip(line.substr(colon_position + 1)));
    game_map[key] = value;
  }
  Sequence sequence = Sequence::ParseFromString(moves);

  int black_disks;
  try {
    black_disks = stoi(GetOrDefault(game_map, std::string("Black disks"), std::string("")));
  } catch (const std::invalid_argument&) {
    black_disks = EstimatedScore(sequence);
  }
  int year;
  try {
    year = stoi(GetOrDefault(game_map, std::string("Year"), std::string("")));
  } catch (const std::invalid_argument&) {
    year = GetCurrentYear();
  }

  return {
      sequence,
      GetOrDefault(game_map, std::string("Black"), std::string("")),
      GetOrDefault(game_map, std::string("White"), std::string("")),
      GetOrDefault(game_map, std::string("Tournament"), std::string("")),
      GetOrDefault(game_map, std::string("Notes"), std::string("")),
      static_cast<short>(year),
      static_cast<Eval>(black_disks),
      GetOrDefault(game_map, std::string("Round"), std::string(""))
  };
}

std::ostream& operator<<(std::ostream& stream, const GameToSave& s) {
  stream << s.ToGame(/*canonicalize=*/false).ToString() << " (round " << s.Round() << ")";
  return stream;
}