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

#ifndef OTHELLOSENSEI_EVALUATEDERIVATIVE_TREE_NODE_H
#define OTHELLOSENSEI_EVALUATEDERIVATIVE_TREE_NODE_H

#include <algorithm>
#include <atomic>
#include <float.h>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <mutex>
#include <optional>
#include <queue>
#include <string.h>
#include <unordered_set>
#include <set>
#include "endgame_time_estimator.h"
#include "evaluation.h"
#include "../board/bitpattern.h"
#include "../board/board.h"
#include "../board/get_moves.h"
#include "../utils/misc.h"

constexpr float kErrors[][60] = {
    {},
    {2.00, 2.00, 2.00, 2.00, 6.64, 6.87, 7.64, 7.77, 8.18, 8.30, 8.72, 8.73,
     8.98, 8.71, 8.65, 8.35, 8.29, 8.05, 8.20, 7.64, 7.55, 7.05, 6.82, 6.00,
     6.36, 5.61, 5.86, 5.19, 5.76, 5.13, 5.58, 4.91, 5.19, 4.39, 4.89, 4.14,
     4.82, 4.03, 4.38, 3.83, 4.16, 3.52, 3.82, 3.20, 3.28, 2.79, 2.96, 2.41,
     2.84, 2.57, 2.51, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00},
    {2.00, 2.00, 2.00, 2.00, 5.57, 5.86, 6.40, 7.08, 7.23, 7.63, 7.77, 8.06,
     8.03, 8.19, 7.91, 7.81, 7.57, 7.51, 7.41, 7.29, 6.92, 6.56, 6.18, 5.41,
     5.67, 5.14, 5.13, 4.66, 4.98, 4.56, 4.78, 4.37, 4.21, 3.90, 4.03, 3.73,
     3.97, 3.63, 3.54, 3.46, 3.51, 3.15, 3.17, 2.94, 2.67, 2.52, 2.56, 2.21,
     2.10, 2.48, 2.09, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00},
    {2.00, 2.00, 2.00, 2.00, 5.17, 4.96, 5.63, 5.93, 6.68, 6.74, 7.21, 7.25,
     7.47, 7.38, 7.51, 7.21, 7.10, 6.97, 6.89, 6.66, 6.57, 6.03, 5.75, 5.01,
     5.30, 4.71, 5.16, 4.30, 4.73, 4.09, 4.59, 3.88, 4.40, 3.50, 3.97, 3.29,
     4.03, 3.17, 3.81, 3.09, 3.50, 2.74, 3.25, 2.59, 3.00, 2.28, 2.61, 2.05,
     2.25, 2.00, 2.37, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00},
    {2.00, 2.00, 2.00, 2.00, 2.00, 4.72, 4.71, 5.25, 5.53, 6.34, 6.37, 6.77,
     6.71, 6.87, 6.81, 7.03, 6.56, 6.54, 6.37, 6.27, 6.05, 5.93, 5.32, 4.77,
     4.93, 4.39, 4.64, 3.98, 4.05, 3.66, 3.96, 3.53, 3.67, 3.24, 3.15, 2.96,
     3.34, 2.93, 3.06, 2.86, 2.80, 2.57, 2.72, 2.40, 2.50, 2.13, 2.13, 2.00,
     2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00}
};

class ChildError: public std::exception {
 public:
  ChildError() : ChildError("Generic child error") {}
  ChildError(std::string msg) : msg_(msg) {}
 private:
  std::string msg_;
};

enum ChildValueType {
  DEFAULT = 0,
  WINNING = 1,
  LOSING = 2
};

class TreeNode {
 public:
  TreeNode() :
      evaluations_(nullptr),
      leaf_eval_(kLessThenMinEvalLarge),
      children_(nullptr),
      n_children_(0),
      fathers_(nullptr),
      n_fathers_(0),
      evaluator_(255) {}
  TreeNode(const TreeNode&) = delete;
  ~TreeNode() {
    Reset(0, 0, 0, 0);
  }

  Square NEmpties() const { return n_empties_; }
  BitPattern Player() const { return player_; }
  BitPattern Opponent() const { return opponent_; }
  Eval Lower() const { return lower_; }
  Eval Upper() const { return upper_; }
  Eval WeakLower() const { return weak_lower_; };
  Eval WeakUpper() const { return weak_upper_; };
  NVisited GetNVisited() const { return descendants_; }

  Board ToBoard() const {
    return Board(player_, opponent_);
  }

  float ProofNumber(Eval eval_goal) const {
    assert(eval_goal >= WeakLower() && eval_goal <= WeakUpper());
    if (eval_goal < lower_) {
      return 0;
    } else if (eval_goal > upper_) {
      return std::numeric_limits<float>::infinity();
    }
    return GetEvaluation(eval_goal).ProofNumber();
  }

  float DisproofNumber(Eval eval_goal) const {
    assert(eval_goal >= WeakLower() && eval_goal <= WeakUpper());
    if (eval_goal < lower_) {
      return std::numeric_limits<float>::infinity();
    } else if (eval_goal > upper_) {
      return 0;
    }
    return GetEvaluation(eval_goal).DisproofNumber();
  }

  float ProbGreaterEqual(Eval eval_goal) const {
    assert(eval_goal >= WeakLower() && eval_goal <= WeakUpper());
    return GetEvaluation(eval_goal).ProbGreaterEqual();
  }

  int MaxLogDerivative(Eval eval_goal) const {
    assert(eval_goal >= WeakLower() && eval_goal <= WeakUpper());
    return GetEvaluation(eval_goal).MaxLogDerivative();
  }

  float GetEval() const {
    int lower = std::max(lower_ + 1, (int) WeakLower());
    int upper = std::min(upper_ - 1, (int) WeakUpper());
    float eval = lower - 1;
    for (int i = lower; i <= upper; i += 2) {
      float prob = GetEvaluation(i).ProbGreaterEqual();
      if (prob >= 1 - kProbIncreaseWeakEval) {
        prob = 1;
      } else if (prob <= kProbIncreaseWeakEval) {
        prob = 0;
      }
      eval += 2 * prob;
    }
    return eval;
  }

  virtual void Reset(Board b, int depth, u_int8_t evaluator) {
    Reset(b.Player(), b.Opponent(), depth, evaluator);
  }

  virtual void Reset(BitPattern player, BitPattern opponent, int depth,
                     u_int8_t evaluator) {
    auto guard = Lock();
    if (evaluations_ != nullptr) {
      free(evaluations_);
      evaluations_ = nullptr;
    }
    player_ = player;
    opponent_ = opponent;
    n_empties_ = ::NEmpties(player, opponent);
    lower_ = kMinEval;
    upper_ = kMaxEval;
    evaluator_ = evaluator;
    if (n_fathers_ != 0) {
      free(fathers_);
      n_fathers_ = 0;
    }
    if (n_children_ != 0) {
      delete[] children_;
      n_children_ = 0;
    }
    depth_ = depth;
    weak_lower_ = kLessThenMinEval;
    weak_upper_ = kLessThenMinEval;
    min_evaluation_ = weak_lower_;
    leaf_eval_ = kLessThenMinEvalLarge;
    eval_depth_ = 0;
    n_threads_working_ = 0;
    descendants_ = 0;
  }

  const Evaluation& GetEvaluation(int eval_goal) const {
    assert((eval_goal - kMinEval) % 2 == 1);
    assert(eval_goal >= WeakLower() && eval_goal <= WeakUpper());
    assert(min_evaluation_ <= WeakLower());
    int index = ToEvaluationIndex(eval_goal);
    assert(index >= 0 && index <= (WeakUpper() - min_evaluation_) / 2);
    assert(evaluations_ != nullptr);
    return evaluations_[index];
  }

  inline bool IsSolvedIn(Eval lower, Eval upper) const {
    assert((lower - kMinEval) % 2 == 1);
    return upper_ <= lower || lower_ >= upper || IsSolved();
  }

  inline bool IsSolved() const {
    return lower_ == upper_;
  }

  bool IsSolvedAtProb(float prob, Eval lower, Eval upper) {
    if (WeakLower() <= lower && lower <= WeakUpper() &&
        GetEvaluation(lower).ProbGreaterEqual() <= prob) {
      return true;
    }
    if (WeakLower() <= upper && upper <= WeakUpper() &&
        GetEvaluation(upper).ProbGreaterEqual() >= 1 - prob) {
      return true;
    }
    for (int i = WeakLower(); i <= WeakUpper() - 2; i += 2) {
      if (GetEvaluation(i).ProbGreaterEqual() >= 1 - prob &&
          GetEvaluation(i + 2).ProbGreaterEqual() <= prob) {
        return true;
      }
    }
    return false;
  }

  bool IsPartiallySolved(Eval lower, Eval upper) {
    return IsSolvedAtProb(kZeroPercForWeak, lower, upper);
  }

  Eval GetPercentileUpper(float p) {
//    assert(p >= kProbIncreaseWeakEval && p <= 1 - kProbIncreaseWeakEval);
    int lower = std::max(lower_ + 1, (int) WeakLower());
    int upper = std::min(upper_ - 1, (int) WeakUpper());
    for (int i = lower; i <= upper; i += 2) {
      const Evaluation& eval = GetEvaluation(i);
      if (eval.IsValid() && eval.ProbGreaterEqual() <= p) {
        return i - 1;
      }
    }
    return upper_;
  }

  Eval GetPercentileLower(float p) {
//    assert(p >= kProbIncreaseWeakEval && p <= 1 - kProbIncreaseWeakEval);
    int lower = std::max(lower_ + 1, (int) WeakLower());
    int upper = std::min(upper_ - 1, (int) WeakUpper());
    for (int i = upper; i >= lower; i -= 2) {
      const Evaluation& eval = GetEvaluation(i);
      if (eval.IsValid() && eval.ProbGreaterEqual() >= 1 - p) {
        return i + 1;
      }
    }
    return lower_;
  }

  int ChildLogDerivative(TreeNode* child, int eval_goal) const {
//    assert(Contains(GetChildren(), (T*) child));
    Evaluation eval = GetEvaluation(eval_goal);
    Evaluation child_eval = child->GetEvaluation(-eval_goal);
    return child_eval.LogDerivative(eval);
  }

  virtual int ToEvaluationIndex(Eval eval) const final {
    assert((eval - kMinEval) % 2 == 1);
    assert(eval >= kMinEval);
    assert(eval <= kMaxEval);
    return (eval - min_evaluation_) >> 1;
  }

  virtual void EnlargeEvaluations() final {
    int desired_size = ToEvaluationIndex(WeakUpper()) + 1;
    if (evaluations_ != nullptr) {
      evaluations_ = (Evaluation*) realloc(evaluations_, desired_size * sizeof(Evaluation));
    } else {
      evaluations_ = (Evaluation*) malloc(desired_size * sizeof(Evaluation));
    }
  }

  void AddDescendants(NVisited n) {
    descendants_ += n;
  }

  void IncreaseNThreadsWorking(int amount) {
    n_threads_working_ += amount;
  }

  void IncreaseNThreadsWorking() {
    n_threads_working_++;
  }

  void DecreaseNThreadsWorking() {
    n_threads_working_--;
  }

  int NThreadsWorking() const {
    return n_threads_working_;
  }

  Eval NextPositionEvalGoal(float prob_min, float prob_max) const {
    static Eval last_eval_goal = kLessThenMinEval;
    Eval best_eval = kLessThenMinEval;
    double best_value = -DBL_MAX;
    for (int i = weak_lower_; i <= weak_upper_; i += 2) {
      const Evaluation& eval = GetEvaluation(i);
      if (eval.ProbGreaterEqual() < prob_min || eval.ProbGreaterEqual() > prob_max ||
              eval.ProofNumber() == 0 || eval.DisproofNumber() == 0 ||
              (i < weak_upper_ && GetEvaluation(i + 2).ProbGreaterEqual() > 1 - kMinProbEvalGoal) ||
              (i > weak_lower_ && GetEvaluation(i - 2).ProbGreaterEqual() < kMinProbEvalGoal)) {
        continue;
      }
      double cur_value = 0;
      // cur_value += (eval.ProbGreaterEqual() > kMinProbEvalGoal && eval.ProbGreaterEqual() < 1 - kMinProbEvalGoal) ?
      //                0 : kLogDerivativeMinusInf * 1000.0;
      cur_value += eval.MaxLogDerivative() * 10;
      cur_value += i == last_eval_goal ? 0 : 1;

      if (cur_value > best_value) {
        best_value = cur_value;
        best_eval = i;
      }
    }
    last_eval_goal = best_eval;
    return best_eval;
  }

  bool operator==(const TreeNode& other) const {
    return Equals(other, false);
  }

  virtual bool IsLeaf() const { return n_children_ == 0; }
  Square Depth() const { return depth_; }

  std::lock_guard<std::mutex> Lock() const {
    return std::lock_guard<std::mutex>{mutex_};
  }

  inline bool IsValid() const {
    return leaf_eval_ != kLessThenMinEvalLarge;
  }

  virtual void GetFathersFromBook() {}

  void UpdateFathers() {
    if (NThreadsWorking() > 0) {
      return;
    }
    GetFathersFromBook();
    // Use an index to avoid co-modification (if some other thread adds fathers in the meantime).
    for (int i = 0; i < n_fathers_; ++i) {
      TreeNode* father = fathers_[i];
      assert(!father->IsLeaf());
      if (father->UpdateFather()) {
        father->UpdateFathers();
      }
    }
  }

  bool UpdateFather() {
    auto guard = Lock();
    assert((WeakLower() - kMinEval) % 2 == 1);
    assert((WeakUpper() - kMinEval) % 2 == 1);
    assert((lower_ - kMinEval) % 2 == 0);
    assert((upper_ - kMinEval) % 2 == 0);
    assert(!IsLeaf());
    lower_ = kLessThenMinEval;
    upper_ = kLessThenMinEval;
    for (int i = WeakLower(); i <= WeakUpper(); i += 2) {
      MutableEvaluation(i)->Initialize();
    }
    auto start = ChildrenStart();
    auto end = ChildrenEnd();
    for (auto child = start; child != end; ++child) {
      if (!UpdateWithChild(**child)) {
        return false;
      }
    }
    // Mark as valid.
    leaf_eval_ = 0;
    for (int i = WeakLower(); i <= WeakUpper(); i += 2) {
      MutableEvaluation(i)->Finalize();
    }
    return true;
  }

  std::vector<TreeNode*> GetChildren() {
    std::vector<TreeNode*> children;
    auto start = ChildrenStart();
    auto end = ChildrenEnd();
    for (auto child = start; child != end; ++child) {
      children.push_back(*child);
    }
    return children;
  }

  void SetChildren(std::vector<TreeNode*> children) {
    {
      auto guard = Lock();
      n_children_ = (Square) children.size();
      children_ = new TreeNode*[n_children_];
      for (int i = 0; i < n_children_; ++i) {
        TreeNode* child = children[i];
        assert(!child->IsValid() || (child->WeakLower() - kMinEval) % 2 == 1);
        child->AddFather(this);
        children_[i] = child;
      }
    }
    UpdateFather();
    assert(AreChildrenCorrect());
  }

  void SetSolved(EvalLarge lower, EvalLarge upper) {
    auto guard = Lock();
    assert(IsValid());
    assert(lower % 16 == 0);
    assert(upper % 16 == 0);
    assert(IsLeaf());
    assert(kMinEvalLarge <= leaf_eval_ && leaf_eval_ <= kMaxEvalLarge);

    Eval lower_small = EvalLargeToEvalRound(lower);
    Eval upper_small = EvalLargeToEvalRound(upper);
    lower_ = MaxEval(lower_, lower_small);
    upper_ = MinEval(upper_, upper_small);
    assert(lower_ <= 64);
    assert(upper_ >= -64);
  }


  virtual void SetLeaf(Eval weak_lower, Eval weak_upper, EvalLarge leaf_eval,
                       Square depth) {
    auto guard = Lock();
    SetLeafNoLock(weak_lower, weak_upper, leaf_eval, depth);
  }

  virtual void UpdateLeafEval() {
    auto guard = Lock();
    assert(IsValid());
    assert(IsLeaf());
    auto lower_large = EvalToEvalLarge(lower_);
    auto upper_large = EvalToEvalLarge(upper_);
    if (leaf_eval_ < lower_large) {
      leaf_eval_ = lower_large;
      assert(kMinEvalLarge <= leaf_eval_ && leaf_eval_ <= kMaxEvalLarge);
      SetLeafNoLock(weak_lower_, weak_upper_, leaf_eval_, eval_depth_);
    } else if (leaf_eval_ > upper_large) {
      leaf_eval_ = upper_large;
      assert(kMinEvalLarge <= leaf_eval_ && leaf_eval_ <= kMaxEvalLarge);
      SetLeafNoLock(weak_lower_, weak_upper_, leaf_eval_, eval_depth_);
    }
    assert(IsValid());
  }

  float RemainingWork() const {
    assert(n_empties_ >= 0 && n_empties_ <= 60);
    float result = std::numeric_limits<float>::infinity();

    for (int i = weak_lower_ + 2; i <= weak_upper_; i += 2) {
      result = std::min(result, GetEvaluation(i-2).ProofNumber() + GetEvaluation(i).DisproofNumber());
    }
    return result;
  }

  bool ToBeSolved(Eval alpha, Eval beta, NVisited max_proof) const {
    assert(alpha <= beta);
    assert(alpha >= weak_lower_ && beta <= weak_upper_);
    assert(IsLeaf());
    assert(n_empties_ >= 0 && n_empties_ <= 60);

    for (Eval i = alpha; i <= beta; i += 2) {
      assert(i >= weak_lower_ && i <= weak_upper_);
      Evaluation eval = GetEvaluation(i);
      assert(IsLeaf());
      if (eval.RemainingWork() > max_proof) {
        return false;
      }
    }
    return true;
  }

  bool UpdateWeakLowerUpper(Eval lower, Eval upper) {
    assert((lower - kMinEval) % 2 == 1);
    assert((upper - kMinEval) % 2 == 1);
    assert(kMinEval <= lower && lower <= kMaxEval);
    assert(kMinEval <= upper && upper <= kMaxEval);
    assert(weak_lower_ >= lower && weak_upper_ <= upper);
    bool extend_lower = false;
    bool extend_upper = false;
    bool reduced = false;
    float prob_lower = ProbGreaterEqual(weak_lower_);
    float prob_upper = ProbGreaterEqual(weak_upper_);
    float prob_lower_next = weak_lower_ + 4 <= weak_upper_ ?
        ProbGreaterEqual(weak_lower_ + 4) : 0;
    float prob_upper_prev = weak_upper_ - 4 >= weak_lower_ ?
        ProbGreaterEqual(weak_upper_ - 4) : 1;
    if (prob_lower < 1 - kProbIncreaseWeakEval && weak_lower_ - 2 >= lower_
        && weak_lower_ - 2 >= lower) {
      extend_lower = true;
    }
    if (prob_upper > kProbIncreaseWeakEval && weak_upper_ + 2 <= upper_
        && weak_upper_ + 2 <= upper) {
      extend_upper = true;
    }
    if (!extend_lower && !extend_upper && prob_lower_next >= 1) {
      weak_lower_ += 2;
      assert(weak_lower_ <= weak_upper_);
      reduced = true;
    }
    if (!extend_lower && !extend_upper && prob_upper_prev <= 0) {
      weak_upper_ -= 2;
      assert(weak_lower_ <= weak_upper_);
      reduced = true;
    }
    {
      if (extend_lower) {
        ExtendEval(weak_lower_ - 2);
      }
      if (extend_upper) {
        ExtendEval(weak_upper_ + 2);
      }
    }
    return reduced || extend_upper || extend_lower;
  }

  std::vector<TreeNode*> Fathers() const {
    std::vector<TreeNode*> result;
    for (int i = 0; i < n_fathers_; ++i) {
      result.push_back(fathers_[i]);
    }
    return result;
  }

  u_int8_t Evaluator() { return evaluator_; }

  void ExtendToAllEvals() {
    for (int i = weak_lower_ - 2; i >= kMinEval; i -= 2) {
      ExtendEval(i);
    }
    for (int i = weak_upper_ + 2; i <= kMaxEval; i += 2) {
      ExtendEval(i);
    }
  }

  bool ContainsFather(TreeNode* father) {
    for (auto possible_father : Fathers()) {
      if (possible_father == father) {
        return true;
      }
    }
    return false;
  }

  bool Equals(const TreeNode& other, bool approx) const {
    assert(IsValid());
    assert(other.IsValid());
    if (!EqualsExceptDescendants(other)) {
      return false;
    }
    if (approx) {
      if (abs((double) GetNVisited() / other.GetNVisited() - 1) > 1E-6) {
        return false;
      }
    } else {
      if (GetNVisited() != other.GetNVisited()) {
        return false;
      }
    }
    return true;
  }

  virtual TreeNode** ChildrenStart() {
    return children_;
  }

  virtual TreeNode** ChildrenEnd() {
    return children_ + n_children_;
  }

  double GetValueLosing(const Evaluation& father, int eval_goal) const {
    const Evaluation& child_eval = GetEvaluation(eval_goal);
    NVisited n_visited = GetNVisited();
    assert(child_eval.ProbGreaterEqual() == 1);
//    return 0.0000000000001 * n_visited;
//    auto proof_number = child_eval.ProofNumber();
//    auto stage = n_visited / (n_visited + proof_number);
    auto proof_number_father = father.ProofNumber();
    auto result =
        GetNVisited() + 10000 < proof_number_father / 100 ?
        1E-16 * n_visited :
        -1E16 * (n_visited / child_eval.ProofNumber());
    return result;
//    return -1E16 * (n_visited / child_eval.ProofNumber());
//    return 1E-12 * (stage > 0.01 ? log(n_visited) : -stage);
  }

  double GetValueDefault(const Evaluation& father, int eval_goal) const {
    return GetEvaluation(eval_goal).LogDerivative(father);
  }

  double GetValueWinning(const Evaluation& father, int eval_goal) const {
    const Evaluation& child_eval = GetEvaluation(eval_goal);
    auto proof_number_using_this =
        ByteToProofNumber(kCombineProb.disproof_to_proof_number[child_eval.DisproofNumberSmall()][child_eval.ProbGreaterEqualSmall()]);
    auto ratio = GetNVisited() / father.ProofNumber();
    float multiplier = -1;
    if (ratio < 1E-5) {
      multiplier = -0.3;
    } else if (ratio < 1E-4) {
      multiplier = -0.6;
    } else if (ratio < 1E-3) {
      multiplier = -0.8;
    } else if (ratio < 1E-2) {
      multiplier = -0.9;
    }
    return proof_number_using_this * multiplier;
  }

  template<ChildValueType type>
  double GetValue(const Evaluation& father, int eval_goal) const {
    if (type == LOSING) {
      return GetValueLosing(father, eval_goal);
    } else if (type == WINNING) {
      return GetValueWinning(father, eval_goal);
    } else {
      return GetValueDefault(father, eval_goal);
    }
  }


  template<ChildValueType type>
  TreeNode* BestChild(int eval_goal) {
    assert(!leaf_->IsLeaf());
    assert(eval_goal_ >= weak_lower_ && eval_goal_ <= weak_upper_);
    const Evaluation& father_eval = GetEvaluation(eval_goal);
    assert(!father_eval.IsSolved());
    double best_child_value = -INFINITY;
    TreeNode* best_child = nullptr;
    auto start = ChildrenStart();
    auto end = ChildrenEnd();
    int child_eval_goal = -eval_goal;
    for (auto child = start; child != end; ++child) {
      if (child_eval_goal <= (*child)->Lower() || child_eval_goal >= (*child)->Upper()) {
        continue;
      }
      assert(!(*child)->GetEvaluation(child_eval_goal).IsSolved());
      assert(father_eval.ProbGreaterEqual() >= 1 - (*child)->GetEvaluation(child_eval_goal).ProbGreaterEqual() - 0.001);
      double cur_loss = (*child)->template GetValue<type>(father_eval, child_eval_goal);
      if (cur_loss > best_child_value) {
        best_child = *child;
        best_child_value = cur_loss;
      }
    }
    return best_child;
  }

 protected:
  mutable std::mutex mutex_;
  BitPattern player_;
  BitPattern opponent_;
  std::atomic_uint64_t descendants_;
  TreeNode** children_;
  TreeNode** fathers_;
  Evaluation* evaluations_;
  EvalLarge leaf_eval_;
  Square n_empties_;
  Eval lower_;
  Eval upper_;
  Eval weak_lower_;
  Eval weak_upper_;
  Eval min_evaluation_;
  std::atomic_uint8_t n_threads_working_;
  Square n_children_;
  Square n_fathers_;
  Square depth_;
  Square eval_depth_;
  u_int8_t evaluator_;

  template<class U>
  void Copy(const U& other) {
    Reset(other.Player(), other.Opponent());
    n_empties_ = other.NEmpties();
    lower_ = other.Lower();
    upper_ = other.Upper();
    descendants_ = other.GetNVisited();
    EnlargeEvaluations();

    auto other_proof_number = other.GetEvaluation(other.WeakLower()).ProofNumber();
    auto other_disproof_number = other.GetEvaluation(other.WeakUpper()).DisproofNumber();
    for (int i = kMinEval + 1; i <= kMaxEval - 1; i += 2) {
      if (i < other.WeakLower()) {
        MutableEvaluation(i)->SetProving(ProofNumberToByte(ConvertProofNumber(other_proof_number, other.WeakLower(), i)));
      } else if (i > other.WeakUpper()) {
        MutableEvaluation(i)->SetDisproving(ProofNumberToByte(ConvertDisproofNumber(other_disproof_number, other.WeakUpper(), i)));
      } else {
        *MutableEvaluation(i) = Evaluation(other.GetEvaluation(i));
      }
    }
  }

  virtual Evaluation* MutableEvaluation(Eval eval_goal) final {
    assert((eval_goal - kMinEval) % 2 == 1);
    assert(eval_goal >= WeakLower() && eval_goal <= WeakUpper());
    assert(min_evaluation_ <= WeakLower());
    int index = ToEvaluationIndex(eval_goal);
    assert(index >= 0 && index <= (WeakUpper() - min_evaluation_) / 2);
    return &evaluations_[index];
  }


  virtual bool UpdateWithChild(const TreeNode& child) {
    auto child_guard = child.Lock();
    if (!child.IsValid()) {
      // Another thread is still working on this node or a child. This father
      // will be updated by this other thread.
      leaf_eval_ = kLessThenMinEvalLarge;
      return false;
    }
    assert(min_evaluation_ <= WeakLower());
    lower_ = MaxEval(lower_, (Eval) -child.Upper());
    upper_ = MaxEval(upper_, (Eval) -child.Lower());
    assert(AllValidEvals());
    // TODO: Make weak_lower_ >= lower_ to avoid extra computations.
    weak_lower_ = MaxEval(weak_lower_, (Eval) -child.weak_upper_);
    weak_upper_ = MinEval(weak_upper_, (Eval) -child.weak_lower_);
    for (int i = WeakLower(); i <= WeakUpper(); i += 2) {
      Evaluation* father_eval = MutableEvaluation(i);
//      assert(-i >= child.WeakLower() && -i <= child.WeakUpper());
      if (child.Upper() <= -i) {
        father_eval->SetSolved();
      } else if (-i > child.Lower()) {
        assert(-i >= child.WeakLower() && -i <= child.WeakUpper());
        const Evaluation& eval = child.GetEvaluation(-i);
        assert(eval.IsValid());
        assert(!eval.IsSolved());
        father_eval->UpdateFatherWithThisChild(eval);
      } // Else do nothing (child eval is solved).
    }
    return true;
  }

  virtual void SetLeafNoLock(
      Eval weak_lower, Eval weak_upper, EvalLarge leaf_eval, Square depth) {
    assert(IsLeaf());
    assert(kMinEvalLarge <= leaf_eval && leaf_eval <= kMaxEvalLarge);

    SetWeakLowerUpper(weak_lower, weak_upper);

    eval_depth_ = depth;
    leaf_eval_ = leaf_eval;

    for (int i = weak_lower_; i <= weak_upper_; i += 2) {
      UpdateLeafEvaluation(i);
    }
    assert(min_evaluation_ <= WeakLower());
    assert(AllValidEvals());
  }

  bool AllValidEvals() const {
    for (int i = weak_lower_; i <= weak_upper_; i += 2) {
      const Evaluation& eval = GetEvaluation(i);
      if (!eval.IsValid()) {
        return false;
      }
    }
    return true;
  }

  void SetWeakLowerUpper(Eval weak_lower, Eval weak_upper) {
    assert(IsLeaf());
    assert(weak_lower >= kMinEval && weak_lower <= kMaxEval && (weak_lower - kMinEval) % 2 == 1);
    assert(weak_upper >= kMinEval && weak_upper <= kMaxEval && (weak_upper - kMinEval) % 2 == 1);
    assert(weak_lower <= weak_upper);
    assert(weak_lower_ == kLessThenMinEval || weak_lower >= weak_lower_);
    assert(weak_upper_ == kLessThenMinEval || weak_upper <= weak_upper_);
    assert((weak_lower_ == kLessThenMinEval) == (weak_upper_ == kLessThenMinEval));
    min_evaluation_ = weak_lower;
    weak_lower_ = weak_lower;
    weak_upper_ = weak_upper;
    EnlargeEvaluations();
  }

  void UpdateLeafEvaluation(int i) {
    assert(i >= weak_lower_ && i <= weak_upper_);
    Evaluation* evaluation = MutableEvaluation(i);
    assert(evaluation != nullptr);
    if (i <= lower_) {
      evaluation->SetProved();
    } else if (i >= upper_) {
      evaluation->SetDisproved();
    } else {
      assert(eval_depth_ >= 0 && eval_depth_ <= 4);
      assert(n_empties_ >= 0 && n_empties_ <= 63);
      float prob = 1 - (float) GaussianCDF(EvalToEvalLarge(i), leaf_eval_, 0.95F * 8 * std::max(3.0F, kErrors[eval_depth_][n_empties_]));
      float proof_number = ::ProofNumber(player_, opponent_, EvalToEvalLarge(i), leaf_eval_);
      assert(isfinite(proof_number) && proof_number > 0);
      float disproof_number = ::DisproofNumber(player_, opponent_, EvalToEvalLarge(i), leaf_eval_);
      assert(isfinite(disproof_number) && disproof_number > 0);
      evaluation->SetLeaf(prob, proof_number, disproof_number);
    }
  }

  void AddFather(TreeNode* father) {
    auto guard = Lock();
    if (n_fathers_ == 0) {
      fathers_ = (TreeNode**) malloc(sizeof(TreeNode*));
    } else {
      fathers_ = (TreeNode**) realloc(fathers_, (n_fathers_ + 1) * sizeof(TreeNode*));
    }
    fathers_[n_fathers_] = father;
    depth_ = father->depth_ + 1;
    n_fathers_++;
  }

  virtual bool AreChildrenCorrect() {
    auto next_boards = GetNextBoardsWithPass(ToBoard());
    if (n_children_ != next_boards.size()) {
      throw ChildError("Wrong children size");
    }

    for (int i = 0; i < n_children_; ++i) {
      TreeNode* child = children_[i];
      if (child->depth_ != depth_ + 1) {
        throw ChildError("Wrong child depth");
      }
      bool found = false;
      for (Board& next_board : next_boards) {
        if (child->ToBoard() == next_board) {
          assert(!found);  // If it's true, there is a duplicate in the flips.
          found = true;
          next_board = Board(0ULL, 0ULL);
        }
      }
      if (!found) {
        throw ChildError("Missing child");
      }
    }
    return true;
  }

  void ExtendEval(Eval value) {
    if (value < weak_lower_) {
      assert(value == weak_lower_ - 2);
      assert((value - kMinEval) % 2 == 1);
      if (value < min_evaluation_) {
        int shift = (min_evaluation_ - value) / 2;
        int old_size = ToEvaluationIndex(weak_upper_) + 1;
        min_evaluation_ = value;
        EnlargeEvaluations();
        memmove(evaluations_ + shift, evaluations_, sizeof(Evaluation) * old_size);
      }
      weak_lower_ = value;
    } else if (value > weak_upper_) {
      assert(value == weak_upper_ + 2);
      weak_upper_ = value;
      EnlargeEvaluations();
    }
    if (IsLeaf()) {
      UpdateLeafEvaluation(value);
      return;
    }
    for (int i = 0; i < n_children_; ++i) {
      children_[i]->ExtendEval(-value);
    }
    UpdateFather();
    assert(AllValidEvals());
  }

  bool EqualsExceptDescendants(const TreeNode& other) const {
    if (ToBoard() != other.ToBoard() ||
        lower_ != other.lower_ ||
        upper_ != other.upper_ ||
        weak_lower_ != other.weak_lower_ ||
        weak_upper_ != other.weak_upper_) {
      return false;
    }

    for (int i = weak_lower_; i <= weak_upper_; i += 2) {
      if (GetEvaluation(i) != other.GetEvaluation(i)) {
        return false;
      }
    }
    return true;
  }
};

std::ostream& operator<<(std::ostream& stream, const TreeNode& b);

template<class Node>
class LeafToUpdate {
 public:
  static std::optional<LeafToUpdate> BestDescendant(Node* node) {
    return BestDescendant(node, node->NextPositionEvalGoal(0, 1));
  }
  static std::optional<LeafToUpdate> BestDescendant(Node* node, Eval goal) {
    LeafToUpdate result(node, goal);
    return BestDescendant(result);
  }

  static LeafToUpdate Leaf(std::vector<Node*> sequence) {
    LeafToUpdate leaf(sequence[0], 1);
    sequence[0]->IncreaseNThreadsWorking();
    for (int i = 1; i < sequence.size(); ++i) {
      Node* descendant = sequence[i];
      leaf.ToChild(descendant, 0);
      descendant->IncreaseNThreadsWorking();
    }
    return leaf;
  }

  static std::optional<LeafToUpdate> BestDescendant(LeafToUpdate& result) {
    while (!result.leaf_->IsLeaf()) {
      Node* best_child = result.BestChild();
      if (best_child == nullptr) {
        return std::nullopt;
      }
      result.ToChild(best_child, 0);
    }
    for (Node* node : result.parents_) {
      node->IncreaseNThreadsWorking();
    }
    assert(result.Leaf()->NThreadsWorking() == 1);
    return result;
  }

  static std::vector<LeafToUpdate> BestDescendants(Node* node) {
    std::vector<LeafToUpdate> result;

    int eval_goal = node->NextPositionEvalGoal(0, 1);
    LeafToUpdate best_descendant(node, eval_goal);
    BestDescendants(node, eval_goal, &result);
    int secondary_eval_goal = kLessThenMinEval;
    if (node->ProbGreaterEqual(eval_goal) == 0) {
      secondary_eval_goal = node->NextPositionEvalGoal(0.5, 1);
    } else if (node->ProbGreaterEqual(eval_goal) == 1) {
      secondary_eval_goal = node->NextPositionEvalGoal(0, 0.5);
    }
    if (secondary_eval_goal != kLessThenMinEval) {
      BestDescendants(node, secondary_eval_goal, &result);
    }
    return result;
  }

  LeafToUpdate Copy() const {
    LeafToUpdate result(*this);
    result.parents_ = {leaf_};
    return result;
  }

  Eval EvalGoal() const { return eval_goal_; }
  Eval Alpha() const { return alpha_; }
  Eval Beta() const { return beta_; }
  Eval WeakLower() const { return weak_lower_; }
  Eval WeakUpper() const { return weak_upper_; }
  double Loss() const { return loss_; }
  const std::vector<Node*>& Parents() const { return parents_; }
  Node* Leaf() const { return leaf_; }

  bool operator==(const LeafToUpdate& other) const = default;

  void Finalize(NVisited n_visited) {
    assert(leaf_ == parents_[parents_.size() - 1]);
    for (auto parent : Parents()) {
      parent->AddDescendants(n_visited);
      parent->DecreaseNThreadsWorking();
    }
    leaf_->UpdateFathers();
  }

 protected:
  Node* leaf_;
  std::vector<Node*> parents_;
  Eval eval_goal_;
  Eval alpha_;
  Eval beta_;
  Eval weak_lower_;
  Eval weak_upper_;
  double loss_;

  LeafToUpdate(Node* leaf, int eval_goal) :
      leaf_(leaf),
      parents_({leaf}),
      eval_goal_(static_cast<Eval>(eval_goal)),
      weak_lower_(leaf->WeakLower()),
      weak_upper_(leaf->WeakUpper()),
      loss_(0) {
    alpha_ = weak_lower_;
    beta_ = weak_upper_;
    UpdateAlphaBeta();
    assert(weak_lower_ <= alpha_ && alpha_ <= eval_goal_ && eval_goal_ <= beta_
           && beta_ <= weak_upper_);
  }

  static bool leaf_less(const LeafToUpdate& left, const LeafToUpdate& right) {
    return left.Loss() < right.Loss();
  };

  typedef std::priority_queue<
      LeafToUpdate, std::vector<LeafToUpdate>, decltype(&leaf_less)>
      NextNodesPriorityQueue;

  Node* BestChild() const {
    auto prob = leaf_->GetEvaluation(eval_goal_).ProbGreaterEqualSmall();
    if (prob == 0) {
      return leaf_->template BestChild<LOSING>(eval_goal_);
    } else if (prob == kProbStep) {
      return leaf_->template BestChild<WINNING>(eval_goal_);
    } else {
      return leaf_->template BestChild<DEFAULT>(eval_goal_);
    }
  }

  Node* NextChildren(NextNodesPriorityQueue* next_nodes) const {
    auto prob = leaf_->GetEvaluation(eval_goal_).ProbGreaterEqualSmall();
    if (prob == 0) {
      return NextChildren<LOSING>(next_nodes);
    } else if (prob == kProbStep) {
      return NextChildren<WINNING>(next_nodes);
    } else {
      return NextChildren<DEFAULT>(next_nodes);
    }
  }
  template<ChildValueType type>
  Node* NextChildren(NextNodesPriorityQueue* next_nodes) const {
    Node* best_child = leaf_->template BestChild<type>(EvalGoal());
    const Evaluation& father_eval = leaf_->GetEvaluation(EvalGoal());
    const Evaluation& best_child_eval = best_child->GetEvaluation(-EvalGoal());
    float best_child_prob = best_child_eval.ProbGreaterEqual();
    assert(best_child->GetNVisited() > 0);
    float prob = father_eval.ProbGreaterEqual();

    auto start = leaf_->ChildrenStart();
    auto end = leaf_->ChildrenEnd();
    auto child_eval_goal = -EvalGoal();

    double best_child_value = best_child->template GetValue<type>(father_eval, child_eval_goal);
    for (auto iter = start; iter != end; ++iter) {
      Node* child = (Node*) *iter;
      if (child_eval_goal <= child->Lower() ||
          child_eval_goal >= child->Upper()) {
        continue;
      }
      Evaluation child_eval = child->GetEvaluation(child_eval_goal);
      assert(child->GetNVisited() > 0);
      double extra_loss =
          child->template GetValue<type>(father_eval, child_eval_goal) -
          best_child_value
          + log(best_child_prob) * kLogDerivativeMultiplier;
      assert(extra_loss <= 0);
      if (!child_eval.IsSolved() &&
          child != best_child &&
          child->NThreadsWorking() == 0 &&
          prob < 1 &&
          Loss() + extra_loss > -4 * kLogDerivativeMultiplier) {
        LeafToUpdate child_leaf = CopyToChild(child, extra_loss);
        next_nodes->push(child_leaf);
      }
    }
    assert(!best_child_eval.IsSolved());
    return best_child;
  };

  static void BestDescendants(
      Node* root, int eval_goal, std::vector<LeafToUpdate>* result) {
    assert(!root->IsSolved());
    assert(!root->GetEvaluation(eval_goal).IsSolved());
    NextNodesPriorityQueue next_nodes(&leaf_less);

    next_nodes.push(LeafToUpdate(root, eval_goal));

    Node* current_leaf;
    while (!next_nodes.empty() && result->size() < 200) {
      LeafToUpdate current = next_nodes.top();
      next_nodes.pop();
      current_leaf = current.Leaf();
      while (!current_leaf->IsLeaf()) {
        Board current_board = current_leaf->ToBoard();
        auto best_child = current.NextChildren(&next_nodes);
        current = current.CopyToChild(best_child, 0);
        current_leaf = current.Leaf();
      }
      if (current_leaf->IsLeaf() &&
          current_leaf->NThreadsWorking() == 0) {
        for (auto parent : current.Parents()) {
          parent->IncreaseNThreadsWorking();
        }
        result->push_back(current);
        assert(current_leaf->NThreadsWorking() == 1);
      }
    }
  }

  void ToChild(Node* child, double extra_loss) {
    int tmp;

    leaf_ = child;
    eval_goal_ = -eval_goal_;
    tmp = alpha_;
    alpha_ = -beta_;
    beta_ = -tmp;
    tmp = weak_lower_;
    weak_lower_ = -weak_upper_;
    weak_upper_ = -tmp;
    loss_ += extra_loss;
    UpdateAlphaBeta();
    parents_.push_back(leaf_);
  }

  LeafToUpdate CopyToChild(Node* child, double extra_loss) const {
    LeafToUpdate leaf(*this);
    leaf.ToChild(child, extra_loss);
    assert(leaf.weak_lower_ <= leaf.alpha_ && leaf.alpha_ <= leaf.eval_goal_ &&
           leaf.eval_goal_ <= leaf.beta_ && leaf.beta_ <= leaf.weak_upper_);
    return leaf;
  }

  void UpdateAlphaBeta() {
    alpha_ = MinEval(eval_goal_, MaxEval(
        alpha_, leaf_->GetPercentileLower(kProbForEndgameAlphaBeta) + 1));
    beta_ = MaxEval(eval_goal_, MinEval(
        beta_, leaf_->GetPercentileUpper(kProbForEndgameAlphaBeta) - 1));
  }
};

typedef LeafToUpdate<TreeNode> TreeNodeLeafToUpdate;
#endif  // OTHELLOSENSEI_EVALUATEDERIVATIVE_TREE_NODE_H