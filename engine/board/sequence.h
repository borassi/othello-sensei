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

#ifndef BOARD_SEQUENCE_H
#define BOARD_SEQUENCE_H

#include <cassert>
#include <cstring>
#include <map>
#include <optional>
#include <ostream>
#include <regex>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include "bitpattern.h"
#include "board.h"
#include "get_flip.h"
#include "get_moves.h"
#include "../utils/misc.h"

struct ThorSquareToSquare {
  Square square[256];

  constexpr ThorSquareToSquare() : square() {
    for (int i = 0; i < 256; ++i) {
      if (i % 10 >= 1 && i % 10 <= 8 && i / 10 >= 1 && i / 10 <= 8) {
        square[i] = (8 - i % 10) + 8 * (8 - i / 10);
      } else {
        square[i] = kNoSquare;
      }
    }
  }
};

constexpr ThorSquareToSquare kThorSquareToSquare;
const std::string kWrongSequence = "Wrong sequence";

class InvalidSequenceException : public std::exception {};

constexpr Square VerticalMirrorMove(Square move) { return 8 * (7 - move / 8) + (move % 8); }
constexpr Square Diag9MirrorMove(Square move) { return 8 * (move % 8) + (move / 8); }
constexpr Square Diag7MirrorMove(Square move) { return 8 * (7 - move % 8) + (7 - move / 8); }
constexpr Square Rotate180Move(Square move) { return 8 * (7 - move / 8) + (7 - move % 8); }

constexpr Square TransposeMove(Square move, int k) {
  if (move == kNoSquare) { return kNoSquare; }
  for (int i = 0; i < k; i += 2) {
    move = VerticalMirrorMove(move);
    if (i + 1 == k) {
      return move;
    }
    move = Diag9MirrorMove(move);
  }
  return move;
}

class Sequence {
 public:
  Sequence() : Sequence(0) {}
  Sequence(const std::string& moves);

  Sequence(const Square* moves, size_t size) : Sequence(size) {
    assert(size <= 64);
    memcpy(moves_, moves, size_ * sizeof(Square));
  }

  Sequence(const std::vector<Square>& moves) : Sequence(moves.data(), moves.size()) {}
  Sequence(const Sequence& other) : Sequence(other.moves_, other.size_) {}

  template <typename Iterator>
  Sequence(Iterator begin, Iterator end) : Sequence(end - begin) {
    int i = 0;
    for (Iterator it = begin; it != end; ++it) {
      moves_[i++] = *it;
    }
  }

  ~Sequence() {
    if (size_ > 0) {
      free(moves_);
    }
  }

  Sequence& operator=(const Sequence& src) {
    if (this != &src) {
      UpdateSize(src.Size());
      memcpy(moves_, src.moves_, size_ * sizeof(Square));
    }
    return *this;
  }

  static Sequence ParseFromString(std::string string) {
    string = ToLower(string);
    std::vector<Square> result;
    Board b;
    std::regex move_regex ("[a-h][1-8]");

    for (std::sregex_iterator iter(string.begin(), string.end(), move_regex);
         iter != std::sregex_iterator();
         ++iter) {
      Square move = MoveToSquare((*iter)[0]);
      assert(move >= 0 && move < 64);
      if (((1ULL << move) & b.Empties()) == 0) {
        continue;
      }
      auto flip = GetFlip(move, b.Player(), b.Opponent());
      if (flip == 0) {
        continue;
      }
      b.PlayMove(flip);
      if (HaveToPass(b.Player(), b.Opponent())) {
        b.PlayMove(0);
      }
      result.push_back(move);
    }
    return Sequence(result);
  }

  static Sequence FromThor(Square* moves) {
    Sequence result(60);
    for (int i = 0; i < 60; ++i) {
      Square move = moves[i];
      if (move == 0) {
        result.UpdateSize(i);
        break;
      }
      result.moves_[i] = kThorSquareToSquare.square[move];
    }
    return result;
  }

  Sequence Subsequence(int size) const {
    assert(size <= Size());
    return Sequence(moves_, size);
  }

  Square LastMove() const { return moves_[size_ - 1]; }
  Square Move(Square i) const {
    return i < Size() ? moves_[i] : kNoSquare;
  }

  void ToCanonicalFirstMoveInplace();

  Sequence ToCanonicalFirstMove() const {
    Sequence copy(*this);
    copy.ToCanonicalFirstMoveInplace();
    return copy;
  };

  void ToCanonicalDiagonalInplace();

  Sequence ToCanonicalDiagonal() const {
    Sequence copy(*this);
    copy.ToCanonicalDiagonalInplace();
    return copy;
  };

  void ToCanonicalGameInplace() {
    ToCanonicalFirstMoveInplace();
    ToCanonicalDiagonalInplace();
  }

  Sequence ToCanonicalGame() const {
    return ToCanonicalFirstMove().ToCanonicalDiagonal();
  }

  void ReplaceStart(const Sequence& sequence) {
    assert(sequence.Size() <= Size());
    memcpy(moves_, sequence.moves_, sequence.Size() * sizeof(Square));
  }

  inline bool operator< (const Sequence& rhs) const {
    for (int i = 0; i < std::min(Size(), rhs.Size()); ++i) {
      if (moves_[i] < rhs.moves_[i]) {
        return true;
      } else if (moves_[i] > rhs.moves_[i]) {
        return false;
      }
    }
    return Size() > rhs.Size();
  }
  inline bool operator>(const Sequence& rhs) const { return rhs < *this; }
  inline bool operator<=(const Sequence& rhs) const { return !(*this > rhs); }
  inline bool operator>=(const Sequence& rhs) const { return !(*this < rhs); }
  inline bool operator==(const Sequence& rhs) const {
    return
        Size() == rhs.Size() &&
        memcmp(moves_, rhs.moves_, Size() * sizeof(Square)) == 0;
  }
  inline bool operator!=(const Sequence& rhs) const { return !(*this == rhs); }

  Square* Moves() const { return moves_; }
  const Square Size() const { return size_; }

  std::vector<Board> ToBoards() const {
    Board b;
    std::vector<Board> result = {b};
    for (int i = 0; i < size_; ++i) {
      Square move = moves_[i];
      assert(move >= 0 && move < 64);
      auto flip = GetFlip(move, b.Player(), b.Opponent());
      if (flip == 0) {
        throw InvalidSequenceException();
      }
      b.PlayMove(flip);
      if (HaveToPass(b.Player(), b.Opponent())) {
        b.PlayMove(0);
      }
      result.push_back(b);
    }
    return result;
  }

  Board ToBoard() const {
    return ToBoards().back();
  }

  Sequence VerticalMirror() const {
    Sequence result(Size());
    for (int i = 0; i < Size(); ++i) {
      result.moves_[i] = VerticalMirrorMove(moves_[i]);
    }
    return result;
  }

  Sequence Diag9Mirror() const {
    Sequence result(Size());
    for (int i = 0; i < Size(); ++i) {
      result.moves_[i] = Diag9MirrorMove(moves_[i]);
    }
    return result;
  }

  std::vector<Sequence> AllTranspositions() {
    std::vector<Sequence> result;
    Sequence sequence(*this);
    for (int i = 0; i < 4; ++i) {
      result.push_back(sequence);
      sequence = sequence.VerticalMirror();
      result.push_back(sequence);
      sequence = sequence.Diag9Mirror();
    }
    assert(result.size() == 8);
    return result;
  }

  Sequence Transpose(int k) {
    Sequence result(Size());
    for (int i = 0; i < Size(); ++i) {
      result.moves_[i] = TransposeMove(moves_[i], k);
    }
    return result;
  }

  std::string ToString() const {
    std::stringstream stream;
    for (int i = 0; i < Size(); ++i) {
      Square s = moves_[i];
      stream << SquareToMove(s);
    }
    return stream.str();
  }

  uint8_t GetTransposition(const Sequence& goal) const {
    Board goal_board = goal.ToBoard();
    Board board = ToBoard();
    if (goal_board == board) {
      return 0;
    }
    int i = 0;
    for (const Board& actual : board.AllTranspositions()) {
      if (actual == goal_board) {
        return i;
      }
      ++i;
    }
    throw std::invalid_argument("This should never happen. Wrong rotation.");
  }

  void AddMove(Square move) {
    UpdateSize(Size() + 1);
    moves_[Size() - 1] = move;
  }

  void RemoveLastMove() { UpdateSize(size_ - 1); }

 private:
  Square* moves_;
  Square size_;

  Sequence(int size) : size_(size) {
    assert(size_ <= 64);
    moves_ = size_ == 0 ? nullptr : (Square*) malloc(size_ * sizeof(Square));
  }

  void UpdateSize(Square new_size) {
    size_ = new_size;
    moves_ = (Square*) realloc(moves_, size_);
  }
};

namespace std {
  template<>
  struct hash<Sequence> {
    std::size_t operator()(const Sequence& s) const {
      std::size_t hash = murmur64(42);
      int last_quick = s.Size() - s.Size() % 8;
      int i;
      for (int64_t* position = (int64_t*) s.Moves(); position < (int64_t*) (s.Moves() + last_quick); ++position) {
        hash ^= murmur64(*position) + 0x9e3779b9 + (hash<<6) + (hash>>2);
      }
      for (Square* position = s.Moves() + last_quick; position < s.Moves() + s.Size(); ++position) {
        hash ^= murmur64(*position) + 0x9e3779b9 + (hash<<6) + (hash>>2);
      }
      return hash;
    }
  };
}

std::ostream& operator<<(std::ostream& stream, const Sequence& s);

struct cmpBoard {
  bool operator()(const Board& b1, const Board& b2) const {
    if (b1.NEmpties() > b2.NEmpties()) {
      return true;
    } else if (b1.NEmpties() < b2.NEmpties()) {
      return false;
    }
    if (b1.Player() < b2.Player()) {
      return true;
    } else if (b1.Player() > b2.Player()) {
      return false;
    }
    return b1.Opponent() < b2.Opponent();
  }
};

class SequenceCanonicalizer {
 public:
  SequenceCanonicalizer() {}

  SequenceCanonicalizer(const std::vector<Sequence>& sequences) : SequenceCanonicalizer() {
    AddAll(sequences);
  }

  void Load(const std::vector<char>& serialized) {
    const char* it = serialized.data();
    while (it < serialized.data() + serialized.size()) {
      Board& board = *((Board*) it);
      it += sizeof(Board);
      int n_sequences = *((int*) it);
      it += sizeof(int);
      uint8_t size = *((uint8_t*) it);
      it += sizeof(uint8_t);
      std::vector<Sequence>& sequences = board_to_sequences_[board];
      sequences.reserve(n_sequences);
      for (int i = 0; i < n_sequences; ++i) {
        sequences.push_back(Sequence((Square*) it, size));
        it += size;
      }
    }
  }

  void AddAll(const std::vector<Sequence>& sequences) {
    ElapsedTime t;
    int k = 0;
    std::unordered_map<Board, int> boards;
    for (const Sequence& sequence : sequences) {
      if (++k % 10000 == 0) {
        std::cout << "Preprocessing sequence " << k << " of " << sequences.size() << " after " << t.Get() << " seconds.\n" << std::flush;
      }
      for (const Board& b : sequence.ToBoards()) {
        boards[b.Unique()]++;
      }
    }
    k = 0;
    for (const Sequence& sequence : sequences) {
      if (++k % 10000 == 0) {
        std::cout << "Preprocessing sequence " << k << " of " << sequences.size() << " after " << t.Get() << " seconds.\n" << std::flush;
      }
      for (const Board& b : sequence.ToBoards()) {
        Board unique = b.Unique();
        if (boards[unique] > 1) {
          std::vector<Sequence>& sequences = board_to_sequences_.insert({unique, std::vector<Sequence>()}).first->second;
          Sequence subsequence = sequence.Subsequence(60 - b.NEmpties());
          if (!Contains(sequences, subsequence)) {
            sequences.push_back(subsequence);
          }
        }
      }
    }
    for (auto it = board_to_sequences_.begin(); it != board_to_sequences_.end(); ) {
      if (it->second.size() == 1) {
        it = board_to_sequences_.erase(it);
      } else {
        ++it;
      }
    }
  }

  std::vector<Sequence> AllEquivalentSequences(const Sequence& sequence) const {
    auto boards = sequence.ToBoards();
    for (auto it = boards.rbegin(); it != boards.rend(); ++it) {
      Board unique = it->Unique();
      auto sequences_it = board_to_sequences_.find(unique);
      if (sequences_it == board_to_sequences_.end()) {
        continue;
      }
      std::vector<Sequence> result;
      for (const Sequence& replacement : sequences_it->second) {
        Sequence copy = sequence;
        auto transposition = copy.Subsequence(replacement.Size()).GetTransposition(replacement);
        if (transposition != 0) {
          copy = copy.AllTranspositions()[transposition];
        }
        copy.ReplaceStart(replacement);
        result.push_back(copy);
      }
      return result;
    }
    return {sequence};
  }

  std::vector<char> Serialize() {
    std::vector<char> result;
    for (const auto& [board, sequences] : board_to_sequences_) {
      result.insert(result.end(), (char*) &board, (char*) &board + sizeof(Board));
      int n_sequences = (int) sequences.size();
      result.insert(result.end(), (char*) &n_sequences, (char*) &n_sequences + sizeof(int));
      result.push_back(sequences.begin()->Size());
      for (const Sequence& sequence : sequences) {
        result.insert(result.end(), sequence.Moves(), sequence.Moves() + sequence.Size());
      }
    }
    return result;
  }

 private:
  std::unordered_map<Board, std::vector<Sequence>> board_to_sequences_;
};

#endif  // BOARD_SEQUENCE_H