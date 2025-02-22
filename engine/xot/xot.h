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
#ifndef OTHELLO_SENSEI_XOT_XOT_H
#define OTHELLO_SENSEI_XOT_XOT_H

#include <limits.h>
#include <optional>

#include "../board/board.h"
#include "../board/sequence.h"
#include "../utils/misc.h"

class XOT {
 public:

  XOT(const std::string& sequences) {
    for (const std::string& sequence_string : Split(sequences, '\n')) {
      sequences_.emplace_back(Sequence(sequence_string));
    }
    for (const SequenceWithMetadata& sequence : sequences_) {
      board_to_sequence_[sequence.unique] = &sequence;
    }
  }

  bool IsInList(const Board& board) const {
    return board_to_sequence_.find(board.Unique()) != board_to_sequence_.end();
  }

  Sequence RandomSequence() const {
    return sequences_[rand() % sequences_.size()].sequence;
  }

  std::vector<Sequence> FindStart(const Sequence& suffix, int max = INT_MAX) const;

 private:
  struct SequenceWithMetadata {
    Sequence sequence;
    Board board;
    Board unique;
    std::vector<Board> board_transpositions;
    std::vector<Sequence> sequence_transpositions;

    SequenceWithMetadata(const Sequence& sequence) : sequence(sequence), board(sequence.ToBoard()) {
      unique = board.Unique();
      board_transpositions = board.AllTranspositions();
      sequence_transpositions = sequence.AllTranspositions();
    }
  };
  std::vector<SequenceWithMetadata> sequences_;
  std::unordered_map<Board, const SequenceWithMetadata*> board_to_sequence_;
};

#endif  // OTHELLO_SENSEI_XOT_XOT_H
