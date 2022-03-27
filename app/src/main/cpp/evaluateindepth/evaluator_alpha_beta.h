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
  Move() : flip_(0), move_(kNoSquare), value_(0) {}
  Move(BitPattern flip, Square move, int value) : flip_(flip), move_(move), value_(value) {}

  bool operator==(const Move& other) const {
    return value_ == other.value_ && move_ == other.move_;
  }

  bool operator!=(const Move& other) const {
    return !(operator==(other));
  }

  bool operator<(const Move& other) const {
    return value_ < other.value_ || (value_ == other.value_ && move_ < other.move_);
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
  Square move_;
  int value_;
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
    depth = std::min(depth, __builtin_popcountll(~(player | opponent)));
    return (this->*evaluators_[depth])(player, opponent, lower, upper, 0, 0);
  }

 private:
  template<int depth, bool passed>
  EvalLarge EvaluateInternal(
      const BitPattern player, const BitPattern opponent,
      const EvalLarge lower, const EvalLarge upper,
      const BitPattern last_flip, const BitPattern stable);

  typedef EvalLarge(EvaluatorAlphaBeta::*EvaluateInternalFunction)(
      const BitPattern, const BitPattern, const EvalLarge, const EvalLarge,
      const BitPattern, const BitPattern);

  static EvaluateInternalFunction evaluators_[65];
  HashMap* hash_map_;
  EpochValue epoch_;
  NVisited n_visited_;
  Move moves_[64][64];
  std::unique_ptr<EvaluatorDepthOneBase> evaluator_depth_one_;
};