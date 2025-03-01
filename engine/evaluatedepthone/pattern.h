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

#include <array>
#include <sstream>
#include "../board/bitpattern.h"
#include "../board/board.h"

constexpr int kMaxPatternSize = 13;
constexpr int kMaxPatternTranspositions = 8;

typedef int FeatureValue;

constexpr int Pow(FeatureValue x, int y) {
  int result = 1;
  for (; y > 0; y--) {
    result *= x;
  }
  return result;
}

class Pattern {
 public:
  constexpr Pattern() : size_(0), squares_() {}

  constexpr Pattern(int size, Square squares[]) : size_(size), squares_() {
    for (int i = 0; i < size; ++i) {
      squares_[i] = squares[i];
    }
  }
  constexpr Pattern(std::initializer_list<Square> squares) :
      size_((Square) squares.size()), squares_() {
    int i = 0;
    for (Square square : squares) {
      squares_[i++] = square;
    }
  }

  constexpr Square Size() const { return size_; }
  constexpr const Square* const Squares() const { return squares_; }

  constexpr Pattern(BitPattern pattern) : size_(), squares_() {
    while (pattern != 0) {
      Square square = (Square) __builtin_ctzll(pattern);
      pattern &= ~(1ULL << square);
      squares_[size_++] = square;
    }
  }

  constexpr bool operator==(const Pattern& other) const {
    if (size_ != other.size_) {
      return false;
    }
    for (int i = 0; i < size_; ++i) {
      if (squares_[i] != other.squares_[i]) {
        return false;
      }
    }
    return true;
  }

  constexpr BitPattern ToBitPattern() const {
    BitPattern result = 0;
    for (int i = 0; i < size_; ++i) {
      result |= 1ULL << squares_[i];
    }
    return result;
  }

  constexpr Pattern Rotate() const {
    Square new_squares[kMaxPatternSize] = {};

    for (int i = 0; i < size_; ++i) {
      Square s = squares_[i];
      int row = s / 8;
      int column = s % 8;
      int newRow = column;
      int newColumn = 7 - row;
      new_squares[i] = newRow * 8 + newColumn;
    }
    return Pattern(size_, new_squares);
  }

  constexpr Pattern HorizMirror() const {
    Square new_squares[kMaxPatternSize] = {};

    for (int i = 0; i < size_; ++i) {
      Square s = squares_[i];
      int row = s / 8;
      int column = s % 8;
      int newRow = row;
      int newColumn = 7 - column;
      new_squares[i] = newRow * 8 + newColumn;
    }
    return Pattern(size_, new_squares);
  }

  static constexpr void AllTranspositions(BitPattern p, Pattern result[8]) {
    Pattern f = Pattern(p);
    for (int i = 0; i < 2; ++i) {
      for (int j = 0; j < 4; ++j) {
        result[4 * i + j] = f;
        f = f.Rotate();
      }
      f = f.HorizMirror();
    }
  }

  static std::array<Pattern, kMaxPatternTranspositions> AllTranspositions(BitPattern p);

  constexpr FeatureValue MaxValue() const {
    return Pow(3, Size()) - 1;
  }

  constexpr FeatureValue GetWeight(int position) const {
    assert(position < size_);
    return Pow(3, position);
  }

  constexpr FeatureValue GetValue(BitPattern player, BitPattern opponent) const {
    BitPattern square_pattern = 0;
    FeatureValue result = 0;
    for (int j = 0; j < Size(); ++j) {
      square_pattern = 1ULL << Squares()[j];
      if ((square_pattern & opponent) != 0) {
        // Opponent: add weight * 2.
        result += GetWeight(j) << 1;
      } else if ((square_pattern & player) == 0) {
        // Empty: add weight.
        result += GetWeight(j);
      }
      // Nothing if player.
    }
    return result;
  }

  friend std::ostream& operator<<(std::ostream& s, const Pattern& f);

 private:
  Square size_;
  Square squares_[kMaxPatternSize];
};

template<>
struct std::hash<Pattern>
{
  std::size_t operator()(const Pattern& f) const noexcept;
};