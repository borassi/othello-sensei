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
 
 #include "game.h"

void SortAndCutExamples(int max_games, std::vector<Game>& examples) {
  std::sort(examples.begin(), examples.end(), CompareGamesByHash);
  if (examples.size() > max_games) {
    examples.erase(examples.begin() + max_games, examples.end());
  }
}

void GamesList::Merge(const GamesList& other) {
  num_games += other.num_games;
  examples.insert(examples.end(), other.examples.begin(), other.examples.end());
  SortAndCutExamples(max_games, examples);
  for (const auto& [key, value] : other.next_moves) {
    next_moves[key] += value;
  }
}

void GamesList::Rotate(const Sequence& target) {
  if (num_games == 0) {
    return;
  }
  assert(examples[0].Moves().Size() >= target.Size());
  int transposition = target.GetTransposition(examples[0].Moves().Subsequence(target.Size()));
  if (transposition == 0) {
    return;
  }
  std::unordered_map<Square, int> old_next_moves(next_moves.begin(), next_moves.end());
  next_moves.clear();
  for (const auto& [move, games] : old_next_moves) {
    next_moves[TransposeMove(move, transposition)] = games;
  }
}

std::string Game::ToString() const {
  std::stringstream result;
  result << *black_ << " " << (int) score_ << " - " << (64-score_) << " " << *white_ << " (" << *tournament_ << " " << year_ << "): " << Moves() << " " << Priority() << "\n";
  return result.str();
}

std::ostream& operator<<(std::ostream& stream, const Game& game) {
  stream << game.ToString();
  return stream;
}