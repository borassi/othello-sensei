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
#include "../board/get_moves.h"
#include "../evaluatedepthone/evaluator_depth_one_base.h"
#include "../hashmap/hash_map.h"


constexpr int kSquareValue[] = {
  18,  4,  16, 12, 12, 16,  4, 18,
   4,  2,   6,  8,  8,  6,  2,  4,
  16,  6,  14, 10, 10, 14,  6, 16,
  12,  8,  10,  0,  0, 10,  8, 12,
  12,  8,  10,  0,  0, 10,  8, 12,
  16,  6,  14, 10, 10, 14,  6, 16,
   4,  2,   6,  8,  8,  6,  2,  4,
  18,  4,  16, 12, 12, 16,  4, 18
};

class Move {
 public:
  Move() : flip_(0), value_(0) {}
  Move(BitPattern flip, int value) : flip_(flip), value_(value) {}

  BitPattern GetFlip() const { return flip_; }
  int GetValue() const { return value_; }

  bool operator==(const Move& other) const {
    return flip_ == other.flip_ && value_ == other.value_;
  }

  bool operator!=(const Move& other) const {
    return !(operator==(other));
  }

  bool operator<(const Move& other) const {
    return value_ < other.value_ || (value_ == other.value_ && flip_ < other.flip_);
  }

  bool operator>(const Move& other) const {
    return other.operator<(*this);
  }

  bool operator<=(const Move& other) const {
    return !(operator>(other));
  }

  bool operator>=(const Move& other) const {
    return !(operator<(other));
  }
 private:
  BitPattern flip_;
  int value_;
};
//
//class MoveIteratorBase {
//  using iterator_category = std::input_iterator_tag;
//  using difference_type = std::ptrdiff_t;
//  using value_type = Move;
//  using pointer = Move*;
//  using reference = Move&;
//
//  virtual Move& operator*() const = 0;
//  virtual MoveIteratorBase operator++(int) = 0;
//  bool operator==(const MoveIteratorBase& other) = 0;
//
//  virtual Move* operator->() {
//    return &(*this);
//  }
//  virtual MoveIteratorBase& operator++() {
//    Move move = *this;
//    ++(*this);
//    return move;
//  };
//  bool operator!=(const MoveIteratorBase& other) {
//    return !(*this == other);
//  }
//};

class MoveIteratorQuick {

 public:
  MoveIteratorQuick(BitPattern player, BitPattern opponent, BitPattern last_flip);

  BitPattern NextFlip() {
    BitPattern mask = masks_[current_mask_];
    BitPattern flip = 0;
    while (flip == 0 && candidate_moves_ != 0) {
      while ((mask & candidate_moves_) == 0) {
        ++current_mask_;
        mask = masks_[current_mask_];
      }
      int move = __builtin_ctzll(mask & candidate_moves_);
      candidate_moves_ &= (~(1ULL << move));
      flip = GetFlip(move, player_, opponent_);
    }
    return flip;
  }

 private:
  BitPattern player_;
  BitPattern opponent_;
  BitPattern candidate_moves_;
  BitPattern masks_[9];
  int current_mask_ = 0;
};

class EvaluatorAlphaBeta {
 public:
  EvaluatorAlphaBeta(
      HashMap* hash_map,
      std::unique_ptr<EvaluatorDepthOneBase> evaluator_depth_one_factory()) :
      n_visited_(0),
      hash_map_(hash_map),
      evaluator_depth_one_(evaluator_depth_one_factory()) {}

  NVisited GetNVisited() const { return n_visited_; }

  EvalLarge Evaluate(BitPattern player, BitPattern opponent, int depth) {
    return Evaluate(player, opponent, depth, kMinEvalLarge, kMaxEvalLarge);
  }
  EvalLarge Evaluate(BitPattern player, BitPattern opponent, int depth, EvalLarge lower, EvalLarge upper) {
    n_visited_ = 0;
    int n_empties = __builtin_popcountll(~(player | opponent));
    depth = std::min(depth, n_empties);
    if (depth == n_empties) {
      return (this->*solvers_[depth])(player, opponent, lower, upper, 0, 0);
    } else {
      evaluator_depth_one_->Setup(player, opponent);
      return (this->*evaluators_[depth])(player, opponent, lower, upper, 0, 0);
    }
  }

 private:
  typedef EvalLarge(EvaluatorAlphaBeta::*EvaluateInternalFunction)(
      const BitPattern, const BitPattern, const EvalLarge, const EvalLarge,
      const BitPattern, const BitPattern);

  template<int depth, bool passed, bool solve>
  EvalLarge EvaluateInternal(
      const BitPattern player, const BitPattern opponent,
      const EvalLarge lower, const EvalLarge upper,
      const BitPattern last_flip, const BitPattern stable);

  static EvaluatorAlphaBeta::EvaluateInternalFunction solvers_[65];
  static EvaluatorAlphaBeta::EvaluateInternalFunction evaluators_[65];
  HashMap* hash_map_;
  EpochValue epoch_;
  NVisited n_visited_;
  Move moves_[64][64];
  std::unique_ptr<EvaluatorDepthOneBase> evaluator_depth_one_;
};