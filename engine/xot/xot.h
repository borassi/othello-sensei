/*
 * Copyright 2025-2026 Michele Borassi
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

class Xot {
 public:
  // Delete copy and move constructors/assignments (board_to_sequence_ uses pointers).
  // It's relatively easy to recreate these, but not worth it as of now.
  Xot(const Xot&) = delete;
  Xot& operator=(const Xot&) = delete;
  Xot(Xot&&) = delete;
  Xot& operator=(Xot&&) = delete;

  Xot(const std::string& sequences, const std::string& name = "") : name_(name) {
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

  const std::string& Name() const { return name_; }

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
  std::string name_;
  std::vector<SequenceWithMetadata> sequences_;
  std::unordered_map<Sequence, const SequenceWithMetadata*> board_to_sequence_;
  int sequence_size_;
};

// Handles multiple Xot datasets loaded from a directory.
class XotHandler {
 public:
  // Initializes the handler by loading all Xot files from the given folder.
  // Files are loaded in alphabetical order. It assumes all filenames have a
  // prefix of the same length (used for sorting), followed by a " - " delimiter,
  // and ending with a file extension.
  // The resulting Xot name will have the prefix, the delimiter, and the extension
  // removed (e.g., "000 - file2.txt" becomes "file2").
  explicit XotHandler(const std::string& folder);

  // Returns the length of the sequence prefix found in the lists, or -1 if not found.
  int GetPrefixLength(const Sequence& sequence) const;

  // Returns true if the sequence is fully present in any of the loaded Xot lists.
  bool IsInList(const Sequence& sequence) const;

  // Returns a random sequence from the specific Xot file.
  Sequence RandomSequence(const std::string& filename) const;

  // Returns a list of all loaded Xot filenames.
  std::vector<std::string> Filenames() const;

 private:
  std::vector<std::unique_ptr<Xot>> xots_;
};

#endif  // OTHELLO_SENSEI_XOT_XOT_H
