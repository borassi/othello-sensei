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
#ifndef EVALUATOR_ALPHA_BETA_H
#define EVALUATOR_ALPHA_BETA_H

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
  void Set(BitPattern flip, int value) {
    flip_ = flip;
    value_ = value;
  }
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

class MoveIteratorBase {
 public:
  virtual void Setup(BitPattern player, BitPattern opponent, BitPattern last_flip, int upper, HashMapEntry* entry,
                     EvaluatorDepthOneBase* evaluator_depth_one) = 0;
  virtual BitPattern NextFlip() = 0;
};

class MoveIteratorVeryQuick : public MoveIteratorBase {
 public:
  MoveIteratorVeryQuick() {}
  void Setup(BitPattern player, BitPattern opponent, BitPattern last_flip,
             int upper, HashMapEntry* entry,
             EvaluatorDepthOneBase* evaluator_depth_one) override;
  BitPattern NextFlip() override;

 private:
  BitPattern player_;
  BitPattern opponent_;
  BitPattern candidate_moves_;
};

class MoveIteratorQuick : public MoveIteratorBase {
 public:
  MoveIteratorQuick();
  void Setup(BitPattern player, BitPattern opponent, BitPattern last_flip,
             int upper, HashMapEntry* entry,
             EvaluatorDepthOneBase* evaluator_depth_one) override;
  BitPattern NextFlip() override;

 private:
  BitPattern player_;
  BitPattern opponent_;
  BitPattern candidate_moves_;
  BitPattern masks_[9];
  int current_mask_;
};

class MoveIteratorEval : public MoveIteratorBase {
 public:
  MoveIteratorEval() : moves_() {};

  void Setup(BitPattern player, BitPattern opponent, BitPattern last_flip,
             int upper, HashMapEntry* entry,
             EvaluatorDepthOneBase* evaluator_depth_one_base) override;

  virtual int Eval(BitPattern player, BitPattern opponent, BitPattern flip, int upper, Square square) = 0;

  BitPattern NextFlip() override;

 private:
  BitPattern player_;
  BitPattern opponent_;
  BitPattern candidate_moves_;
  Move moves_[64];
  int remaining_moves_;

 protected:
  EvaluatorDepthOneBase* depth_one_evaluator_;
};

class MoveIteratorMinimizeOpponentMoves : public MoveIteratorEval {
 public:
  int Eval(BitPattern player, BitPattern opponent, BitPattern flip, int upper, Square square) final;
};

class MoveIteratorDisproofNumber : public MoveIteratorEval {
 public:
  int Eval(BitPattern player, BitPattern opponent, BitPattern flip, int upper, Square square) final;
};

class EvaluatorAlphaBeta {
 public:
  EvaluatorAlphaBeta(
      HashMap* hash_map,
      EvaluatorFactory evaluator_depth_one_factory);

  NVisited GetNVisited() const { return n_visited_; }

  EvalLarge Evaluate(BitPattern player, BitPattern opponent, int depth) {
    return Evaluate(player, opponent, depth, kMinEvalLarge, kMaxEvalLarge);
  }
  EvalLarge Evaluate(BitPattern player, BitPattern opponent, int depth, EvalLarge lower, EvalLarge upper) {
    n_visited_ = 0;
    int n_empties = __builtin_popcountll(~(player | opponent));
    depth = std::min(depth, n_empties);
    evaluator_depth_one_->Setup(player, opponent);
    if (depth == n_empties) {
      return (this->*solvers_[depth])(player, opponent, lower, upper, 0, 0);
    } else {
      return (this->*evaluators_[depth])(player, opponent, lower, upper, 0, 0);
    }
  }

  static constexpr int kMaxDepth = 64;
 private:

  typedef EvalLarge(EvaluatorAlphaBeta::*EvaluateInternalFunction)(
      const BitPattern, const BitPattern, const EvalLarge, const EvalLarge,
      const BitPattern, const BitPattern);

  template<int depth, bool passed, bool solve>
  EvalLarge EvaluateInternal(
      const BitPattern player, const BitPattern opponent,
      const EvalLarge lower, const EvalLarge upper,
      const BitPattern last_flip, const BitPattern stable);

  static EvaluatorAlphaBeta::EvaluateInternalFunction solvers_[kMaxDepth];
  static EvaluatorAlphaBeta::EvaluateInternalFunction evaluators_[kMaxDepth];
  HashMap* hash_map_;
  EpochValue epoch_;
  NVisited n_visited_;
  std::shared_ptr<MoveIteratorBase> move_iterators_[4 * kMaxDepth];
  std::unique_ptr<EvaluatorDepthOneBase> evaluator_depth_one_;
};
#endif  // EVALUATOR_ALPHA_BETA_H