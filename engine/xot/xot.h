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
    sequence_size_ = -1;
    for (const SequenceWithMetadata& sequence : sequences_) {
      if (sequence_size_ == -1) {
        sequence_size_ = sequence.sequence.Size();
      }
      assert(sequence_size_ == sequence.sequence.Size());
      board_to_sequence_[sequence.sequence_unique] = &sequence;
    }
  }

  int SequenceSize() const { return sequence_size_; }

  bool IsInListPrefix(const Sequence& sequence) const {
    return sequence.Size() >= sequence_size_ && IsInList(sequence.Subsequence(sequence_size_));
  }

  bool IsInList(const Sequence& sequence) const {
    return board_to_sequence_.find(sequence.Unique()) != board_to_sequence_.end();
  }

  Sequence RandomSequence() const {
    return sequences_[rand() % sequences_.size()].sequence;
  }

  std::vector<Sequence> FindStart(const Sequence& suffix, int max = INT_MAX) const;

 private:
  struct SequenceWithMetadata {
    Sequence sequence;
    Sequence sequence_unique;
    Board board;
    std::vector<Board> board_transpositions;
    std::vector<Sequence> sequence_transpositions;

    SequenceWithMetadata(const Sequence& sequence) : sequence(sequence), board(sequence.ToBoard()) {
      sequence_unique = sequence.Unique();
      board_transpositions = board.AllTranspositions();
      sequence_transpositions = sequence.AllTranspositions();
    }
  };
  std::vector<SequenceWithMetadata> sequences_;
  std::unordered_map<Sequence, const SequenceWithMetadata*> board_to_sequence_;
  int sequence_size_;
};

#endif  // OTHELLO_SENSEI_XOT_XOT_H
