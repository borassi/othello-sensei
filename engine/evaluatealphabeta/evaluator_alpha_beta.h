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
#ifndef EVALUATOR_ALPHA_BETA_H
#define EVALUATOR_ALPHA_BETA_H

#include <functional>
#include <climits>

#include "../board/bitpattern.h"
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

enum StatsType {
  LAST_5 = 0,
  VERY_QUICK = 1,
  QUICK1 = 2,
  QUICK2 = 3,
  MOVES_OR_DISPROOF = 4,
  PASS = 5,
  TREE_NODE = 6,
  LAST_NODE_TYPE = 6,
  NEXT_POSITION_FAIL = 7,
  NEXT_POSITION_SUCCESS = 8,
  SOLVED_TOO_EARLY = 9,
  NO_TYPE = 10
};

class Stats {
 public:
  Stats() : n_visited_(), time_deepen_(0), time_next_position_(0) { Reset(); }

  void Reset() {
    std::fill(std::begin(n_visited_), std::end(n_visited_), 0);
    time_deepen_ = 0;
    time_next_position_ = 0;
  }

  void Add(NVisited n, StatsType type) { n_visited_[type] += n; }
  void AddTimeDeepen(double t) { time_deepen_ += t; }
  void AddTimeNextPosition(double t) { time_next_position_ += t; }

  void Merge(const Stats& other) {
    for (int i = 0; i < NO_TYPE; ++i) {
      n_visited_[i] += other.Get(static_cast<StatsType>(i));
    }
    time_deepen_ += other.TimeDeepen();
    time_next_position_ += other.TimeNextPosition();
  }

  NVisited GetAll() const {
    NVisited total = 0;
    for (int i = 0; i <= LAST_NODE_TYPE; ++i) {
      total += n_visited_[i];
    }
    return total;
  }

  NVisited Get(StatsType type) const {
    return n_visited_[type];
  }

  double TimeDeepen() const { return time_deepen_; }
  double TimeNextPosition() const { return time_next_position_; }

 private:
  std::array<NVisited, NO_TYPE> n_visited_;
  double time_deepen_;
  double time_next_position_;
};

class MoveIteratorBase {
 public:
  explicit MoveIteratorBase(Stats* stats) : stats_(stats) {}
  virtual void Setup(
      BitPattern player, BitPattern opponent, BitPattern last_flip, int upper,
      HashMapEntry* entry,
      EvaluatorDepthOneBase* evaluator_depth_one) = 0;
  virtual BitPattern NextFlip() = 0;

 protected:
  Stats* stats_;
};

class MoveIteratorVeryQuick : public MoveIteratorBase {
 public:
  explicit MoveIteratorVeryQuick(Stats* stats) : MoveIteratorBase(stats), player_(0), opponent_(0), candidate_moves_() {}
  virtual ~MoveIteratorVeryQuick() {}
  void Setup(BitPattern player, BitPattern opponent, BitPattern last_flip,
             int upper, HashMapEntry* entry,
             EvaluatorDepthOneBase* evaluator_depth_one) override;
  BitPattern NextFlip() override;

 private:
  BitPattern player_;
  BitPattern opponent_;
  BitPattern candidate_moves_;
};

template<bool very_quick>
class MoveIteratorQuick : public MoveIteratorBase {
 public:
  virtual ~MoveIteratorQuick() {}
  explicit MoveIteratorQuick(Stats* stats);
  void Setup(BitPattern player, BitPattern opponent, BitPattern last_flip,
             int upper, HashMapEntry* entry,
             EvaluatorDepthOneBase* evaluator_depth_one) override;
  BitPattern NextFlip() override;

 private:
  BitPattern player_;
  BitPattern opponent_;
  BitPattern candidate_moves_;
  // NOTE: could be 6 when very_quick = true, but probably not worth it.
  BitPattern masks_[9];
  int current_mask_;
};

class MoveIteratorEval : public MoveIteratorBase {
 public:
  explicit MoveIteratorEval(Stats* stats) : MoveIteratorBase(stats), moves_(), remaining_moves_(), empties_(), depth_one_evaluator_() {};

  void Setup(BitPattern player, BitPattern opponent, BitPattern last_flip,
             int upper, HashMapEntry* entry,
             EvaluatorDepthOneBase* evaluator_depth_one_base) override;

  virtual int Eval(BitPattern player, BitPattern opponent, BitPattern flip, int upper, Square square, Square empties) = 0;

  BitPattern NextFlip() override;

 private:
  Move moves_[64];
  int remaining_moves_;
  Square empties_;

 protected:
  EvaluatorDepthOneBase* depth_one_evaluator_;
};

class MoveIteratorMinimizeOpponentMoves : public MoveIteratorEval {
 public:
  virtual ~MoveIteratorMinimizeOpponentMoves() {}
  explicit MoveIteratorMinimizeOpponentMoves(Stats* stats) : MoveIteratorEval(stats) {};
  int Eval(BitPattern player, BitPattern opponent, BitPattern flip, int upper, Square square, Square empties) final;
};

class MoveIteratorDisproofNumber : public MoveIteratorEval {
 public:
  virtual ~MoveIteratorDisproofNumber() {}
  explicit MoveIteratorDisproofNumber(Stats* stats) : MoveIteratorEval(stats) {};
  int Eval(BitPattern player, BitPattern opponent, BitPattern flip, int upper, Square square, Square empties) final;
};

class EvaluatorAlphaBeta {
 public:
  EvaluatorAlphaBeta(
      HashMap<kBitHashMap>* hash_map,
      const EvaluatorFactory& evaluator_depth_one_factory);
  EvaluatorAlphaBeta(const EvaluatorAlphaBeta&) = delete;

  NVisited GetNVisited() const { return stats_.GetAll(); }

  EvalLarge Evaluate(BitPattern player, BitPattern opponent, int depth) {
    return Evaluate(player, opponent, depth, kMinEvalLarge, kMaxEvalLarge);
  }
  EvalLarge Evaluate(BitPattern player, BitPattern opponent, int depth, EvalLarge lower, EvalLarge upper, int max_visited = INT_MAX) {
    stats_.Reset();
    int n_empties = (int) __builtin_popcountll(~(player | opponent));
    depth = std::min(depth, n_empties);
    evaluator_depth_one_->Setup(player, opponent);
    stats_.Add(1, LAST_5);
    if (depth == n_empties) {
      return (this->*solvers_[depth])(player, opponent, lower, upper, 0, 0, max_visited);
    } else {
      return (this->*evaluators_[depth])(player, opponent, lower, upper, 0, 0, max_visited);
    }
  }

  const Stats& GetStats() const { return stats_; }
  static constexpr int kMaxDepth = 64;

 private:
  typedef EvalLarge(EvaluatorAlphaBeta::*EvaluateInternalFunction)(
      const BitPattern, const BitPattern, const EvalLarge, const EvalLarge,
      const BitPattern, const BitPattern, int);

  template<int depth, bool passed, bool solve>
  EvalLarge EvaluateInternal(
      BitPattern player, BitPattern opponent,
      EvalLarge lower, EvalLarge upper,
      BitPattern last_flip, BitPattern stable, int max_visited);

  int VisitedToDisprove(BitPattern player, BitPattern opponent, EvalLarge upper);
  int VisitedToProve(BitPattern player, BitPattern opponent, EvalLarge lower);

  static const EvaluatorAlphaBeta::EvaluateInternalFunction solvers_[kMaxDepth];
  static const EvaluatorAlphaBeta::EvaluateInternalFunction evaluators_[kMaxDepth];
  HashMap<kBitHashMap>* hash_map_;
  std::shared_ptr<MoveIteratorBase> move_iterators_[4 * kMaxDepth];
  std::unique_ptr<EvaluatorDepthOneBase> evaluator_depth_one_;
  Stats stats_;
};

typedef std::function<EvaluatorAlphaBeta> EvaluatorAlphaBetaFactory;
#endif  // EVALUATOR_ALPHA_BETA_H