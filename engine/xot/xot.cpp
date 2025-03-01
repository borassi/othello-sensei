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

#include <vector>

#include "xot.h"
#include "../board/sequence.h"


std::vector<Sequence> XOT::FindStart(const Sequence& suffix, int max) const {
  BitPattern full = 0;
  std::vector<Sequence> result;
  for (int i = 0; i < suffix.Size(); ++i) {
    Square move = suffix.Moves()[i];
    assert(move >= 0 && move < 64);
    full |= 1ULL << suffix.Moves()[i];
  }
  for (const auto& sequence_with_metadata : sequences_) {
    const Sequence& sequence = sequence_with_metadata.sequence;
    const Board& board = sequence_with_metadata.board;
    const auto& sequence_transpositions = sequence_with_metadata.sequence_transpositions;
    const auto& board_transpositions = sequence_with_metadata.board_transpositions;
    for (int i = 0; i < board_transpositions.size(); ++i) {
      auto& start = sequence_transpositions[i];
      Square first_move = start.Moves()[0];
      if (first_move != 19 && first_move != 26 && first_move != 37 && first_move != 44) {
        continue;
      }
      auto& board = board_transpositions[i];
      if ((full & (board.Player() | board.Opponent())) != 0) {
        continue;
      }
      Sequence full = start;
      if (!full.Extend(suffix)) {
        continue;
      }
      try {
        full.ToBoard();
      } catch (InvalidSequenceException e) {
        continue;
      }
      result.push_back(start);
      if (result.size() >= max) {
        return result;
      }
    }
  }
  return result;
}