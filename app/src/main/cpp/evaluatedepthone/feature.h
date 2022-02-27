/*
 * Copyright 2022 Google LLC
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

#include "../board/bitpattern.h"

constexpr int kMaxFeatureSize = 13;
constexpr int kMaxFeatureTranspositions = 8;
class Feature {
 public:
  constexpr Square size() { return size_; }
  constexpr Square* squares() { return squares_; }

  constexpr Feature(BitPattern pattern) : size_(), squares_() {
    while (pattern != 0) {
      Square square = __builtin_ctzll(pattern);
      pattern &= ~(1ULL << square);
      squares_[size_++] = square;
    }
  }

  constexpr bool operator==(const Feature& other) const {
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

  constexpr Feature Rotate() const {
    Square new_squares[kMaxFeatureSize] = {};

    for (int i = 0; i < size_; ++i) {
      Square s = squares_[i];
      int row = s / 8;
      int column = s % 8;
      int newRow = column;
      int newColumn = 7 - row;
      new_squares[i] = newRow * 8 + newColumn;
    }
    return Feature(size_, new_squares);
  }

  constexpr Feature HorizMirror() {
    Square new_squares[kMaxFeatureSize] = {};

    for (int i = 0; i < size_; ++i) {
      Square s = squares_[i];
      int row = s / 8;
      int column = s % 8;
      int newRow = row;
      int newColumn = 7 - column;
      new_squares[i] = newRow * 8 + newColumn;
    }
    return Feature(size_, new_squares);
  }

  static constexpr void AllFeatures(BitPattern p, Feature* output) {
    int result_size = 0;
    Feature f = Feature(p);
    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 2; ++j) {
        bool duplicate = false;
        for (int k = 0; k < result_size; ++k) {
          if (output[k].ToBitPattern() == f.ToBitPattern()) {
            duplicate = true;
            break;
          }
        }
        if (!duplicate) {
          output[result_size++] = f;
        }
        f = f.HorizMirror();
      }
      f = f.Rotate();
    }
    for (; result_size < kMaxFeatureTranspositions; result_size++) {
      output[result_size].size_ = 0;
    }
  }

  friend std::ostream& operator<<(std::ostream& s, Feature const& f) {
    s << "(";
    for (int i = 0; i < f.size_; ++i) {
      s << (int) f.squares_[i] << " ";
    }
    s << ")";
    return s;
  }

  constexpr Feature() : size_(0), squares_() {}

  constexpr Feature(int size, Square squares[]) : size_(size), squares_() {
    for (int i = 0; i < size; ++i) {
      squares_[i] = squares[i];
    }
  }

 private:
  Square size_;
  Square squares_[kMaxFeatureSize];
};