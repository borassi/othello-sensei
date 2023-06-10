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
#include "evaluation.h"
#include "../board/bitpattern.h"
#include "../board/board.h"
#include "../board/get_moves.h"
#include "../utils/misc.h"

constexpr float kProbIncreaseWeakEval = 0.05;

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

template<class Node>
class LeafToUpdate;

class EvaluatorDerivative;

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
    if (evaluations_ != nullptr) {
      free(evaluations_);
    }
    if (n_fathers_ != 0) {
      free(fathers_);
    }
    if (n_children_ != 0) {
      delete[] children_;
    }
  }

  Square NEmpties() const { return n_empties_; }
  BitPattern Player() const { return player_; }
  BitPattern Opponent() const { return opponent_; }
  Eval Lower() const { return lower_; }
  Eval Upper() const { return upper_; }
  Eval WeakLower() const { return weak_lower_; }
  Eval WeakUpper() const { return weak_upper_; }
  NVisited GetNVisited() const { return descendants_; }

  Board ToBoard() const {
    return Board(player_, opponent_);
  }

  float ProofNumber(Eval eval_goal) const {
    assert((eval_goal - kMinEval) % 2 == 1);
    assert(eval_goal >= weak_lower_ && eval_goal <= weak_upper_);
    if (eval_goal < lower_) {
      return 0;
    } else if (eval_goal > upper_) {
      return std::numeric_limits<float>::infinity();
    }
    return GetEvaluation(eval_goal).ProofNumber();
  }

  float DisproofNumber(Eval eval_goal) const {
    assert((eval_goal - kMinEval) % 2 == 1);
    assert(eval_goal >= weak_lower_ && eval_goal <= weak_upper_);
    if (eval_goal < lower_) {
      return std::numeric_limits<float>::infinity();
    } else if (eval_goal > upper_) {
      return 0;
    }
    return GetEvaluation(eval_goal).DisproofNumber();
  }

  float ProbGreaterEqual(Eval eval_goal) const {
    assert((eval_goal - kMinEval) % 2 == 1);
    assert(eval_goal >= weak_lower_ && eval_goal <= weak_upper_);
    return GetEvaluation(eval_goal).ProbGreaterEqual();
  }

  int MaxLogDerivative(Eval eval_goal) const {
    assert((eval_goal - kMinEval) % 2 == 1);
    assert(eval_goal >= weak_lower_ && eval_goal <= weak_upper_);
    return GetEvaluation(eval_goal).MaxLogDerivative();
  }

  float GetEval() const {
    int lower = std::max(lower_ + 1, (int) WeakLower());
    int upper = std::min(upper_ - 1, (int) WeakUpper());
    double eval = lower - 1;
    for (int i = lower; i <= upper; i += 2) {
      double prob = GetEvaluation(i).ProbGreaterEqual();
      eval += 2 * prob;
    }
    return (float) eval;
  }

  virtual void Reset(BitPattern player, BitPattern opponent, int depth,
                     EvalLarge leaf_eval, Square eval_depth,
                     EvaluatorDerivative* evaluator);

  virtual void Reset(Board b, int depth,
                     u_int8_t evaluator, EvalLarge leaf_eval, Square eval_depth,
                     Eval weak_lower, Eval weak_upper) {
    Reset(b.Player(), b.Opponent(), depth, evaluator, leaf_eval, eval_depth, weak_lower, weak_upper);
  }

  virtual void Reset(BitPattern player, BitPattern opponent, int depth,
                     u_int8_t evaluator, EvalLarge leaf_eval, Square eval_depth,
                     Eval weak_lower, Eval weak_upper);

  const Evaluation& GetEvaluation(int eval_goal) const {
    assert((eval_goal - kMinEval) % 2 == 1);
    assert(eval_goal >= weak_lower_ && eval_goal <= weak_upper_);
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

  float SolveProbability(Eval lower, Eval upper) const {
    auto guard = ReadLock();
    return SolveProbabilityNoLock(lower, upper);
  }

  float SolveProbabilityLower(Eval lower) const {
    auto guard = ReadLock();
    return SolveProbabilityLowerNoLock(lower);
  }

  float SolveProbabilityUpper(Eval upper) const {
    auto guard = ReadLock();
    return SolveProbabilityUpperNoLock(upper);
  }

  float SolveProbabilityNoLock(Eval lower, Eval upper) const {
    return std::max(SolveProbabilityUpperNoLock(upper), SolveProbabilityLowerNoLock(lower));
  }

  double SolveProbabilityUpperNoLock(Eval upper) const {
    auto goal = MinEval(GetPercentileLower(0.5F), upper);
    assert(goal >= weak_lower_);
    // This can rarely happen when we are in dire need of updating the eval.
    // Only happens with multiple threads.
    if (goal > weak_upper_) {
      return 0.5;
    }
    return GetEvaluation(goal).ProbGreaterEqual();
  }

  double SolveProbabilityLowerNoLock(Eval lower) const {
    auto goal = MaxEval(GetPercentileUpper(0.5F), lower);
    assert(goal <= weak_upper_);
    // This can rarely happen when we are in dire need of updating the eval.
    // Only happens with multiple threads.
    if (goal < weak_lower_) {
      return 0.5;
    }
    return 1.0 - GetEvaluation(goal).ProbGreaterEqual();
  }

  Eval GetPercentileUpper(double p) const {
    int lower = std::max(lower_ - 1, (int) WeakLower());
    int upper = std::min(upper_ + 1, (int) WeakUpper());
    for (int i = upper; i >= lower; i -= 2) {
      const Evaluation& eval = GetEvaluation(i);
      if (eval.ProbGreaterEqual() > p) {
        return i;
      }
    }
    return lower - 2;
  }

  // Returns the first score with probability <= 1-p.
  Eval GetPercentileLower(double p) const {
    int lower = std::max(lower_ - 1, (int) WeakLower());
    int upper = std::min(upper_ + 1, (int) WeakUpper());
    for (int i = lower; i <= upper; i += 2) {
      const Evaluation& eval = GetEvaluation(i);
      if (eval.ProbGreaterEqual() < 1 - p) {
        return i;
      }
    }
    return upper + 2;
  }

  int ChildLogDerivative(TreeNode* child, int eval_goal) const {
    assert((eval_goal - kMinEval) % 2 == 1);
    assert(eval_goal >= weak_lower_ && eval_goal <= weak_upper_);
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

  void EnlargeEvaluationsInternal() {
    min_evaluation_ = weak_lower_;
    int desired_size = ToEvaluationIndex(WeakUpper()) + 1;
    if (evaluations_ != nullptr) {
      evaluations_ = (Evaluation*) realloc(evaluations_, desired_size * sizeof(Evaluation));
    } else {
      evaluations_ = (Evaluation*) malloc(desired_size * sizeof(Evaluation));
    }
  }

  void EnlargeEvaluations() {
    EnlargeEvaluationsInternal();
    if (!IsLeaf()) {
      UpdateFatherNoLock();
    } else {
      UpdateLeafEvaluations();
    }
  }

  void AddDescendants(NVisited n) {
    descendants_ += n;
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
      cur_value += (eval.ProbGreaterEqual() > 0.001 && eval.ProbGreaterEqual() < 0.999) ?
          eval.MaxLogDerivative() * 10 : kLogDerivativeMinusInf;
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

  std::lock_guard<std::mutex> ReadLock() const {
    return std::lock_guard<std::mutex>{mutex_};
  }

  std::lock_guard<std::mutex> WriteLock() const {
    return std::lock_guard<std::mutex>{mutex_};
  }

  virtual void GetFathersFromBook() {}

  void UpdateFathers() {
    GetFathersFromBook();
    // Use an index to avoid co-modification (if some other thread adds fathers in the meantime).
    for (int i = 0; i < n_fathers_; ++i) {
      TreeNode* father = fathers_[i];
      assert(!father->IsLeaf());
      father->UpdateFather();
      father->UpdateFathers();
    }
  }

  void UpdateFather() {
    auto guard = WriteLock();
    UpdateFatherNoLock();
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
    auto guard = WriteLock();
    SetChildrenNoLock(children);
  }

  void SetSolved(EvalLarge lower, EvalLarge upper, const EvaluatorDerivative& evaluator_derivative);

  void SetLeaf(
      EvalLarge leaf_eval, Square depth,
      Eval weak_lower, Eval weak_upper) {
    SetLeafNoLock(leaf_eval, depth, weak_lower, weak_upper);
  }

  std::tuple<Eval, Eval, Eval, Eval> ExpectedWeakLowerUpper() const {
    auto guard = ReadLock();

    Eval expected_weak_lower = weak_lower_;
    Eval expected_weak_upper = weak_upper_;
    if (ProbGreaterEqual(weak_lower_) < 1 - kZeroPercForWeak) {
      expected_weak_lower = weak_lower_ - 2;
    } else {
      for (int i = weak_lower_ + 2; i <= weak_upper_; i += 2) {
        if (ProbGreaterEqual(i) < 0.99) {
          expected_weak_lower = i - 2;
          break;
        }
      }
    }
    if (ProbGreaterEqual(weak_upper_) > kZeroPercForWeak) {
      expected_weak_upper = weak_upper_ + 2;
    } else {
      for (int i = weak_upper_ - 2; i >= weak_lower_; i -= 2) {
        if (ProbGreaterEqual(i) > 0.01) {
          expected_weak_upper = i + 2;
          break;
        }
      }
    }
    return std::make_tuple(weak_lower_, weak_upper_, expected_weak_lower, expected_weak_upper);
  }

  virtual void UpdateLeafEvalNoLock() {
    assert(IsLeaf());
    auto lower_large = EvalToEvalLarge(lower_);
    auto upper_large = EvalToEvalLarge(upper_);
    if (leaf_eval_ < lower_large) {
      leaf_eval_ = lower_large;
      assert(kMinEvalLarge <= leaf_eval_ && leaf_eval_ <= kMaxEvalLarge);
    } else if (leaf_eval_ > upper_large) {
      leaf_eval_ = upper_large;
      assert(kMinEvalLarge <= leaf_eval_ && leaf_eval_ <= kMaxEvalLarge);
    }
  }

  // Measures the progress towards solving the position (lower is better).
  // Starts from 0, decreases until kLogDerivativeMinusInf until partially
  // solved, then decreases more until becoming -inf.
  double Advancement(Eval lower, Eval upper) const {
    double result = -std::numeric_limits<double>::infinity();
    {
      auto guard = ReadLock();
      for (int i = weak_lower_; i <= weak_upper_; i += 2) {
        result = std::max(result, (double) GetEvaluation(i).MaxLogDerivative());
      }
    }
    if (result == kLogDerivativeMinusInf) {
      result += log(RemainingWork(lower, upper)) - 1E5;
    }
    return result;
  }

  double RemainingWork(Eval lower, Eval upper) const {
    assert((lower - kMinEval) % 2 == 1);
    assert((upper - kMinEval) % 2 == 1);
    auto guard = ReadLock();
    assert(n_empties_ >= 0 && n_empties_ <= 60);
    double result = std::numeric_limits<double>::infinity();

    if (lower >= weak_lower_) {
      result = std::min(
          result,
          (double) GetEvaluation(MinEval(lower, weak_upper_)).DisproofNumber());
    }
    if (upper <= weak_upper_) {
      result = std::min(
          result,
          (double) GetEvaluation(MaxEval(upper, weak_lower_)).ProofNumber());
    }

    for (int i = weak_lower_ + 2; i <= weak_upper_; i += 2) {
      result = std::min(result, (double) GetEvaluation(i-2).ProofNumber() + GetEvaluation(i).DisproofNumber());
    }
    assert(result < std::numeric_limits<double>::infinity());
    return result;
  }

  bool ToBeSolved(Eval alpha, Eval beta, NVisited max_proof) const {
    assert(alpha <= beta);
    assert(alpha >= weak_lower_ && beta <= weak_upper_);
    assert(IsLeaf());
    assert(n_empties_ >= 0 && n_empties_ <= 60);
    assert ((alpha - kMinEval) % 2 == 1);
    assert ((beta - kMinEval) % 2 == 1);

    for (Eval i = alpha; i <= beta; i += 2) {
      assert(i >= weak_lower_ && i <= weak_upper_);
      Evaluation eval = GetEvaluation(i);
      if (eval.RemainingWork() > max_proof) {
        return false;
      }
    }
    return true;
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
    ExtendEval(weak_lower_, weak_upper_);
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

  template<ChildValueType type>
  TreeNode* BestChild(int eval_goal, float n_thread_multiplier) {
    assert(!IsLeaf());
    assert(eval_goal >= weak_lower_ && eval_goal <= weak_upper_);
    const Evaluation& father_eval = GetEvaluation(eval_goal);
//    assert(!father_eval.IsSolved());
    double best_child_value = -INFINITY;
    TreeNode* best_child = nullptr;
    auto start = ChildrenStart();
    auto end = ChildrenEnd();
    int child_eval_goal = -eval_goal;
    for (auto iter = start; iter != end; ++iter) {
      TreeNode* child = *iter;
      auto guard = child->ReadLock();
      if (child_eval_goal <= child->Lower() || child_eval_goal >= child->Upper() ||
          child_eval_goal < child->WeakLower() || child_eval_goal > child->WeakUpper()) {
        continue;
      }
      assert(!child->GetEvaluation(child_eval_goal).IsSolved());
//      assert(father_eval.ProbGreaterEqual() >= 1 - child->GetEvaluation(child_eval_goal).ProbGreaterEqual() - 0.001);
      double cur_loss = child->template GetValue<type>(father_eval, child_eval_goal, n_thread_multiplier, GetNVisited());
      if (cur_loss > best_child_value) {
        best_child = child;
        best_child_value = cur_loss;
      }
    }
    if (best_child != nullptr && !best_child->IsLeaf()) {
      best_child->IncreaseNThreadsWorking();
    }
    return best_child;
  }

  bool TryLockLeaf() {
    auto guard = ReadLock();
    // Some other thread has added this node's children when I was not looking.
    if (!IsLeaf()) {
      return false;
    }
    uint8_t expected = 0;
    bool locked = n_threads_working_.compare_exchange_strong(expected, (uint8_t) 1);
    assert((locked && expected == 0) || (!locked && expected == 1));
    return locked;
  }

  bool IsSolved(Eval lower, Eval upper, bool approx) {
    auto guard = ReadLock();
    return IsSolvedIn(lower, upper) || (SolveProbabilityNoLock(lower, upper) < 0.05 && approx);
  }

  template<class T>
  std::optional<LeafToUpdate<T>> AsLeaf() {
    auto guard = ReadLock();
    auto eval_goal = NextPositionEvalGoal(0, 1);
    if (eval_goal == kLessThenMinEval) {
      return std::nullopt;
    }
    return AsLeafNoLock<T>(eval_goal);
  }

  template<class T>
  LeafToUpdate<T> AsLeafNoLock(Eval eval_goal) {
    LeafToUpdate<T> leaf;
    leaf.leaf_ = (T*) this;
    leaf.parents_ = {(T*) this};
    leaf.eval_goal_ = eval_goal;
    leaf.alpha_ = weak_lower_;
    leaf.beta_ = weak_upper_;

    UpdateAlphaBetaNoLock(&leaf);

    assert(leaf.alpha_ <= leaf.eval_goal_ && leaf.eval_goal_ <= leaf.beta_);
    return leaf;
  }

  template<class T>
  void UpdateAlphaBetaNoLock(LeafToUpdate<T>* leaf) {
    leaf->alpha_ = MinEval(leaf->eval_goal_, MaxEval(
        leaf->alpha_, GetPercentileLower(kProbForEndgameAlphaBeta)));
    leaf->beta_ = MaxEval(leaf->eval_goal_, MinEval(
        leaf->beta_, GetPercentileUpper(kProbForEndgameAlphaBeta)));
  }

  template<class T>
  void UpdateAlphaBeta(LeafToUpdate<T>* leaf) {
    auto guard = ReadLock();
    UpdateAlphaBetaNoLock(leaf);
  }

  LeafToUpdate<TreeNode> AsLeaf(Eval eval_goal);
  std::optional<LeafToUpdate<TreeNode>> AsLeaf();

  void ExtendEval(Eval weak_lower, Eval weak_upper) {
    {
      auto guard = WriteLock();
      if (weak_lower >= weak_lower_ && weak_upper <= weak_upper_) {
        return;
      }
      if (IsLeaf()) {
        SetLeafNoLock(leaf_eval_, eval_depth_, weak_lower, weak_upper);
        return;
      }
    }
    for (int i = 0; i < n_children_; ++i) {
      children_[i]->ExtendEval(-weak_upper, -weak_lower);
    }
    {
      auto guard = WriteLock();
      weak_lower_ = weak_lower;
      weak_upper_ = weak_upper;
      EnlargeEvaluations();
    }
  }

  template<ChildValueType type>
  double GetValue(const Evaluation& father, int eval_goal, float n_thread_multiplier, NVisited father_visited) const {
    if (type == WINNING) {
      return GetValueWinning(father, eval_goal);
    } else {
      return GetValueDefault(father, eval_goal, n_thread_multiplier, father_visited);
    }
  }

  TreeNode* BestChild(int eval_goal, float n_thread_multiplier) {
    auto guard = ReadLock();
    if (eval_goal < weak_lower_ || eval_goal > weak_upper_) {
      return nullptr;
    }
    auto prob = GetEvaluation(eval_goal).ProbGreaterEqual();
    if (prob > 0.9) {
      return this->template BestChild<WINNING>(eval_goal, n_thread_multiplier);
    } else {
      return this->template BestChild<DEFAULT>(eval_goal, n_thread_multiplier);
    }
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

  virtual void ResetNoLock(
      BitPattern player, BitPattern opponent, int depth,
      u_int8_t evaluator, EvalLarge leaf_eval, Square eval_depth,
      Eval weak_lower, Eval weak_upper);

  void UpdateFatherNoLock() {
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
    bool shallow = GetNVisited() < 50000;
    for (auto child = start; child != end; ++child) {
      UpdateWithChild(**child, shallow);
    }
    for (int i = WeakLower(); i <= WeakUpper(); i += 2) {
      MutableEvaluation(i)->Finalize(shallow);
      assert(GetEvaluation(i).IsSolved() == (i < lower_ || i > upper_));
    }
    assert(kMinEval <= lower_ && lower_ <= upper_ && upper_ <= kMaxEval);
  }

  virtual Evaluation* MutableEvaluation(Eval eval_goal) final {
    assert((eval_goal - kMinEval) % 2 == 1);
    assert(eval_goal >= WeakLower() && eval_goal <= WeakUpper());
    assert(min_evaluation_ <= WeakLower());
    int index = ToEvaluationIndex(eval_goal);
    assert(index >= 0 && index <= (WeakUpper() - min_evaluation_) / 2);
    return &evaluations_[index];
  }

  double GetValueDefault(const Evaluation& father, int eval_goal, float n_thread_multiplier, NVisited father_visited) const {
    const Evaluation& eval = GetEvaluation(eval_goal);
    NVisited n_visited = GetNVisited();
    return
        (double) (n_visited * 2000.0 < father_visited ? -kLogDerivativeMinusInf : 0)
        + eval.LogDerivative(father)
        - n_visited / (n_visited + eval.ProofNumber())
        - n_thread_multiplier * NThreadsWorking() * pow(1 - father.ProbGreaterEqual(), 3);
  }

  double GetValueWinning(const Evaluation& father, int eval_goal) const {
    const Evaluation& child_eval = GetEvaluation(eval_goal);
    auto proof_number_using_this =
        ByteToProofNumber(Evaluation::kCombineProb.disproof_to_proof_number[child_eval.DisproofNumberSmall()][child_eval.ProbGreaterEqualSmall()]);
    auto ratio = GetNVisited() / father.ProofNumber();
    float multiplier = -1;
    if (ratio < 1E-5) {
      multiplier = -0.1;
    } else if (ratio < 1E-4) {
      multiplier = -0.4;
    } else if (ratio < 1E-3) {
      multiplier = -0.8;
    } else if (ratio < 1E-2) {
      multiplier = -0.9;
    }
    return proof_number_using_this * multiplier;
  }

  virtual void UpdateWithChild(const TreeNode& child, bool shallow) {
    auto child_guard = child.ReadLock();
    assert(min_evaluation_ <= WeakLower());
    lower_ = MaxEval(lower_, (Eval) -child.Upper());
    upper_ = MaxEval(upper_, (Eval) -child.Lower());
    // TODO: Make weak_lower_ >= lower_ to avoid extra computations.
    weak_lower_ = MaxEval(weak_lower_, (Eval) -child.weak_upper_);
    weak_upper_ = MinEval(weak_upper_, (Eval) -child.weak_lower_);
    for (int i = weak_lower_; i <= weak_upper_; i += 2) {
      Evaluation* father_eval = MutableEvaluation(i);
//      assert(-i >= child.WeakLower() && -i <= child.WeakUpper());
      assert(-i >= child.WeakLower() && -i <= child.WeakUpper());
      const Evaluation& eval = child.GetEvaluation(-i);
      if (shallow) {
        father_eval->UpdateFatherWithThisChild<true>(eval);
      } else {
        father_eval->UpdateFatherWithThisChild<false>(eval);
      }
    }
  }

  virtual void SetLeafNoLock(EvalLarge leaf_eval, Square depth, Eval weak_lower, Eval weak_upper) {
    assert(IsLeaf());
    assert(kMinEvalLarge <= leaf_eval && leaf_eval <= kMaxEvalLarge);
    assert(depth > 0 && depth <= 4);

    eval_depth_ = depth;
    leaf_eval_ = leaf_eval;
    weak_lower_ = weak_lower;
    weak_upper_ = weak_upper;
    EnlargeEvaluations();
    assert(min_evaluation_ <= WeakLower());
  }

  void UpdateLeafEvaluations() {
    assert(IsLeaf());
    for (int i = weak_lower_; i <= weak_upper_; i += 2) {
      UpdateLeafEvaluation(i);
    }
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
      assert(eval_depth_ >= 1 && eval_depth_ <= 4);
      assert(n_empties_ >= 0 && n_empties_ <= 63);
      EvalLarge i_large = EvalToEvalLarge(i);
      evaluation->SetLeaf(player_, opponent_, i_large, leaf_eval_, eval_depth_, n_empties_);
    }
    assert(GetEvaluation(i).IsSolved() == (i < lower_ || i > upper_));
  }

  void AddFather(TreeNode* father) {
    auto guard = WriteLock();
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
          next_board = Board(0UL, 0UL);
        }
      }
      if (!found) {
        throw ChildError("Missing child");
      }
    }
    return true;
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

  void SetChildrenNoLock(std::vector<TreeNode*> children) {
    n_children_ = (Square) children.size();
    children_ = new TreeNode*[n_children_];
    for (int i = 0; i < n_children_; ++i) {
      TreeNode* child = children[i];
      assert((child->WeakLower() - kMinEval) % 2 == 1);
      child->AddFather(this);
      children_[i] = child;
    }
    UpdateFatherNoLock();
    assert(AreChildrenCorrect());
  }
};

std::ostream& operator<<(std::ostream& stream, const TreeNode& b);

template<class Node>
class LeafToUpdate {
 public:
  static std::optional<LeafToUpdate> BestDescendant(Node* node, float n_thread_multiplier) {
    auto leaf = node->AsLeaf();
    if (leaf == std::nullopt) {
      return std::nullopt;
    }
    return BestDescendant(*leaf, n_thread_multiplier);
  }

  static LeafToUpdate Leaf(std::vector<Node*> sequence) {
    LeafToUpdate leaf = sequence[0]->AsLeaf(1);
    sequence[0]->IncreaseNThreadsWorking();
    for (int i = 1; i < sequence.size(); ++i) {
      Node* descendant = sequence[i];
      leaf.ToChild(descendant, 0);
      descendant->IncreaseNThreadsWorking();
    }
    return leaf;
  }

  static std::optional<LeafToUpdate> BestDescendant(LeafToUpdate& result, float n_thread_multiplier) {
    if (!result.leaf_->IsLeaf()) {
      result.leaf_->IncreaseNThreadsWorking();
    }
    while (!result.leaf_->IsLeaf()) {
      Node* best_child = result.leaf_->BestChild(result.eval_goal_, n_thread_multiplier);
      if (best_child == nullptr) {
        for (Node* node : result.parents_) {
          node->DecreaseNThreadsWorking();
        }
        return std::nullopt;
      }
      result.ToChild(best_child, 0);
    }
    if (result.Leaf()->TryLockLeaf()) {
      return result;
    }
    for (Node* node : result.parents_) {
      if (!node->IsLeaf()) {
        node->DecreaseNThreadsWorking();
      }
    }
    return std::nullopt;
  }

  static std::vector<LeafToUpdate> BestDescendants(Node* node) {
    std::vector<LeafToUpdate> result;

    int eval_goal = node->NextPositionEvalGoal(0, 1);
    LeafToUpdate best_descendant = node->AsLeaf(eval_goal);
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

  void UpdateFirstNode(TreeNode* first_node) {
    parents_[0]->DecreaseNThreadsWorking();
    parents_[0] = first_node;
    first_node->IncreaseNThreadsWorking();
  }
 protected:
  friend class TreeNode;
  Node* leaf_;
  std::vector<Node*> parents_;
  Eval eval_goal_;
  Eval alpha_;
  Eval beta_;
  double loss_;

  static bool leaf_less(const LeafToUpdate& left, const LeafToUpdate& right) {
    return left.Loss() < right.Loss();
  }

  typedef std::priority_queue<
      LeafToUpdate, std::vector<LeafToUpdate>, decltype(&leaf_less)>
      NextNodesPriorityQueue;

  Node* NextChildren(NextNodesPriorityQueue* next_nodes) const {
    auto prob = leaf_->GetEvaluation(eval_goal_).ProbGreaterEqual();
    if (prob == 0) {
      return NextChildren<LOSING>(next_nodes);
    } else if (prob > 0.9) {
      return NextChildren<WINNING>(next_nodes);
    } else {
      return NextChildren<DEFAULT>(next_nodes);
    }
  }
  template<ChildValueType type>
  Node* NextChildren(NextNodesPriorityQueue* next_nodes) const {
    Node* best_child = leaf_->template BestChild<type>(EvalGoal(), 0);
    const Evaluation& father_eval = leaf_->GetEvaluation(EvalGoal());
    const Evaluation& best_child_eval = best_child->GetEvaluation(-EvalGoal());
    float best_child_prob = best_child_eval.ProbGreaterEqual();
    assert(best_child->GetNVisited() > 0);
    float prob = father_eval.ProbGreaterEqual();

    auto start = leaf_->ChildrenStart();
    auto end = leaf_->ChildrenEnd();
    auto child_eval_goal = -EvalGoal();

    double best_child_value = best_child->template GetValue<type>(father_eval, child_eval_goal, 0, leaf_->GetNVisited());
    for (auto iter = start; iter != end; ++iter) {
      Node* child = (Node*) *iter;
      if (child_eval_goal <= child->Lower() ||
          child_eval_goal >= child->Upper()) {
        continue;
      }
      Evaluation child_eval = child->GetEvaluation(child_eval_goal);
      assert(child->GetNVisited() > 0);
      double extra_loss =
          child->template GetValue<type>(father_eval, child_eval_goal, 0, leaf_->GetNVisited()) -
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
  }

  static void BestDescendants(
      Node* root, int eval_goal, std::vector<LeafToUpdate>* result) {
    assert(!root->IsSolved());
    assert(!root->GetEvaluation(eval_goal).IsSolved());
    NextNodesPriorityQueue next_nodes(&leaf_less);

    next_nodes.push(root->AsLeaf(eval_goal));

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
    loss_ += extra_loss;
    leaf_->template UpdateAlphaBeta<Node>(this);
    parents_.push_back(leaf_);
  }

  LeafToUpdate CopyToChild(Node* child, double extra_loss) const {
    LeafToUpdate leaf(*this);
    leaf.ToChild(child, extra_loss);
    assert(leaf.alpha_ <= leaf.eval_goal_ &&
           leaf.eval_goal_ <= leaf.beta_);
    return leaf;
  }
};

typedef LeafToUpdate<TreeNode> TreeNodeLeafToUpdate;
#endif  // OTHELLOSENSEI_EVALUATEDERIVATIVE_TREE_NODE_H