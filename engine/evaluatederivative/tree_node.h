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
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <set>
#include <stdexcept>
#include <string.h>
#include <unordered_set>

#include "evaluation.h"
#include "../board/bitpattern.h"
#include "../board/board.h"
#include "../board/get_moves.h"
#include "../utils/misc.h"
#include "../utils/serializable_boolean_vector.h"

constexpr float kProbIncreaseWeakEval = 0.05F;

class ChildError: public std::exception {
 public:
  ChildError() : ChildError("Generic child error") {}
  ChildError(std::string msg) : msg_(msg) {}
 private:
  std::string msg_;
};

enum ChildValueType {
  DEFAULT = 0,
  WINNING = 1
};

template<class Node>
class LeafToUpdate;

class EvaluatorDerivative;

class Node {
 public:
  Node() :
      evaluations_(nullptr),
      leaf_eval_(kLessThenMinEvalLarge),
      evaluator_(255),
      is_leaf_(true) {}

  Node(const Node& other) :
      player_(other.player_),
      opponent_(other.opponent_),
      evaluations_(nullptr),
      n_empties_(other.n_empties_),
      // We need to set evaluations_ to nullptr otherwise it tries to free it.
      is_leaf_(other.is_leaf_) {
    FromOther(other);
  }

  // We cannot convert to Node directly, because it calls a copy constructor
  // (which creates an infinite loop). The pointers do the trick.
  Node(const Node* const other) : Node(*other) {}

  virtual ~Node() {
    if (evaluations_ != nullptr) {
      free(evaluations_);
    }
  }

  void FromOther(const Node& other) {
    assert(Board(player_, opponent_).Unique() == other.ToBoard().Unique());
    assert(n_empties_ == other.n_empties_);

    if (evaluations_ != nullptr) {
      free(evaluations_);
    }
    evaluations_ = nullptr;
    descendants_ = other.descendants_.load();
    leaf_eval_ = other.leaf_eval_;
    lower_ = other.lower_;
    upper_ = other.upper_;
    weak_lower_ = other.weak_lower_;
    weak_upper_ = other.weak_upper_;
    min_evaluation_ = other.min_evaluation_;
    eval_depth_ = other.eval_depth_;
    evaluator_ = other.evaluator_;
    EnlargeEvaluations();
    for (int i = MaxEval(lower_ + 1, weak_lower_); i <= MinEval(upper_ - 1, weak_upper_); i += 2) {
      *MutableEvaluation(i) = other.GetEvaluation(i);
    }
  }

  static Node Deserialize(const std::vector<char>& serialized, int version, std::vector<CompressedFlip>* father_flips) {
    Node n;
    Board board = Board::Deserialize(serialized.begin());
    n.player_ = board.Player();
    n.opponent_ = board.Opponent();
    n.n_empties_ = board.NEmpties();
    n.depth_ = kNoSquare;
    int i = kSerializedBoardSize;

    while (true) {
      CompressedFlip compressed_flip = 0;
      compressed_flip |= (uint8_t) serialized[i] | ((uint8_t) serialized[i+1] << 8) | ((uint8_t) serialized[i+2] << 16);
      i += 3;
      if (compressed_flip == 0) {
        break;
      }
      if (father_flips) {
        father_flips->push_back(compressed_flip >> 2);
      }
      if ((compressed_flip & 1) == 0) {
        break;
      }
    }

    n.descendants_ = (NVisited) *((float*) &(serialized[i]));
    i += sizeof(float);
    n.lower_ = (Eval) serialized[i++];
    n.upper_ = (Eval) serialized[i++];
    n.weak_lower_ = -63;
    n.weak_upper_ = 63;
    n.min_evaluation_ = -63;
    Eval last_1 = (Eval) serialized[i++];
    Eval first_0 = (Eval) serialized[i++];
    n.is_leaf_ = (bool) serialized[i++];

    n.EnlargeEvaluations();

    for (int eval = last_1 + 2; eval <= first_0 - 2; eval += 2) {
      Probability prob = serialized[i++];
      PN proof_number = serialized[i++];
      PN disproof_number = serialized[i++];
      uint32_t max_log_derivative = 0;
      uint8_t first_byte = serialized[i];
      if (first_byte & (1U << 7)) {
        max_log_derivative |= (first_byte & ~(1U << 7)) << 16;
        i++;
      }
      max_log_derivative |= (uint8_t) serialized[i++] << 8;
      max_log_derivative |= (uint8_t) serialized[i++];
      n.MutableEvaluation(eval)->Set(prob, proof_number, disproof_number, -(int) max_log_derivative);
    }
    PN proof = 0;
    PN disproof = 0;
    if (n.lower_ + 1 <= last_1) {
      proof = serialized[i++];
    }
    if (n.upper_ - 1 >= first_0) {
      disproof = serialized[i++];
    }
    SerializableBooleanVector serialized_bool(serialized.begin() + i, serialized.end(), 8 * (serialized.size() - i));

    int j = 0;
    if (version >= 1) {
      // Max: 64 * 8 - 64 * 8 = 128 * 8 = 1024 => 11 bits.
      uint32_t leaf_eval_small = serialized_bool.Get(0, 11);
      n.leaf_eval_ = leaf_eval_small + kMinEvalLarge;
      j += 11;
    }
    if (n.lower_ + 1 <= last_1) {
      n.MutableEvaluation(n.lower_ + 1)->SetProving(proof);

      for (int eval = n.lower_ + 3; eval <= last_1; eval += 2) {
        while (serialized_bool.Get(j++) == 0) {
          proof++;
        }
        n.MutableEvaluation(eval)->SetProving(proof);
      }
    }
    if (n.upper_ - 1 >= first_0) {
      n.MutableEvaluation(n.upper_ - 1)->SetDisproving(disproof);

      for (int eval = n.upper_ - 3; eval >= first_0; eval -= 2) {
        while (serialized_bool.Get(j++) == 0) {
          disproof++;
        }
        n.MutableEvaluation(eval)->SetDisproving(disproof);
      }
    }
    if (version < 1) {
      n.leaf_eval_ = (EvalLarge) std::round(n.GetEval() * 8);
    }
    return n;
  }

  Square NEmpties() const { return n_empties_; }
  BitPattern Player() const { return player_; }
  BitPattern Opponent() const { return opponent_; }
  Eval Lower() const { return lower_; }
  Eval Upper() const { return upper_; }
  Eval WeakLower() const { return MaxEval(weak_lower_, lower_ + 1); }
  Eval WeakUpper() const { return MinEval(weak_upper_, upper_ - 1); }
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
      return FLT_MAX;
    }
    return GetEvaluation(eval_goal).ProofNumber();
  }

  float DisproofNumber(Eval eval_goal) const {
    assert((eval_goal - kMinEval) % 2 == 1);
    assert(eval_goal >= weak_lower_ && eval_goal <= weak_upper_);
    if (eval_goal < lower_) {
      return FLT_MAX;
    } else if (eval_goal > upper_) {
      return 0;
    }
    return GetEvaluation(eval_goal).DisproofNumber();
  }

  double ProbGreaterEqual(Eval eval_goal) const {
    assert((eval_goal - kMinEval) % 2 == 1);
    assert(eval_goal >= weak_lower_ && eval_goal <= weak_upper_);
    return GetEvaluation(eval_goal).ProbGreaterEqual();
  }

  int MaxLogDerivative(Eval eval_goal) const {
    assert((eval_goal - kMinEval) % 2 == 1);
    assert(eval_goal >= weak_lower_ && eval_goal <= weak_upper_);
    return GetEvaluation(eval_goal).MaxLogDerivative();
  }

  float LeafEval() const { return leaf_eval_ / 8.0F; }

  double GetEval() const {
    int lower = std::max(lower_ + 1, (int) weak_lower_);
    int upper = std::min(upper_ - 1, (int) weak_upper_);
    double eval = lower - 1;
    for (int i = lower; i <= upper; i += 2) {
      double prob = GetEvaluation(i).ProbGreaterEqual();
      eval += 2 * prob;
    }
    return eval;
  }

  double Uncertainty() const {
    int lower = std::max(lower_ + 1, (int) weak_lower_);
    int upper = std::min(upper_ - 1, (int) weak_upper_);
    double e_x = lower - 1;
    double e_x2 = (lower - 1) * (lower - 1);
    for (int i = lower; i <= upper; i += 2) {
      double prob = GetEvaluation(i).ProbGreaterEqual();
      e_x += 2 * prob;
      e_x2 += 4 * i * prob; // (i+1)^2 - (i-1)^2
    }
    return (e_x2 - e_x * e_x) / 4;
  }

  const Evaluation& GetEvaluation(int eval_goal) const {
    assert((eval_goal - kMinEval) % 2 == 1);
    assert(eval_goal >= weak_lower_ && eval_goal <= weak_upper_);
    assert(eval_goal >= lower_ && eval_goal <= upper_);
    assert(min_evaluation_ <= weak_lower_);
    int index = ToEvaluationIndex(eval_goal);
    assert(index >= 0 && index <= (weak_upper_ - min_evaluation_) / 2);
    assert(evaluations_ != nullptr);
    return evaluations_[index];
  }

  double SolveProbabilityNoLock(Eval lower, Eval upper) const {
    return std::max(SolveProbabilityUpper(upper), SolveProbabilityLower(lower));
  }

  virtual double SolveProbability(Eval lower, Eval upper) const {
    return SolveProbabilityNoLock(lower, upper);
  }

  virtual double SolveProbabilityUpper(Eval upper) const {
    auto goal = MinEval(GetPercentileLower(0.5F), upper);
    if (goal > upper_ || goal < lower_) {
      // Goal > upper: Prob(goal - 2) > 0.5; Prob(goal) = 0.
      // Goal < lower: Only possibility: upper < lower_. We already proved that
      // it's >= upper, so nothing else to do here.
      return 0;
    }
    assert(goal >= weak_lower_);
    // This can rarely happen when we are in dire need of updating the eval.
    // Only happens with multiple threads.
    if (goal > weak_upper_) {
      return 0.5;
    }
    return GetEvaluation(goal).ProbGreaterEqual();
  }

  virtual double SolveProbabilityLower(Eval lower) const {
    auto goal = MaxEval(GetPercentileUpper(0.5F), lower);
    if (goal < lower_ || goal > upper_) {
      // Same as SolveProbabilityUpper.
      return 0;
    }
    assert(goal <= weak_upper_);
    // This can rarely happen when we are in dire need of updating the eval.
    // Only happens with multiple threads.
    if (goal < weak_lower_) {
      return 0.5;
    }
    return 1.0 - GetEvaluation(goal).ProbGreaterEqual();
  }

  // Returns the last score with probability >= p.
  Eval GetPercentileUpper(double p) const {
    int lower = std::max(lower_ + 1, (int) weak_lower_);
    int upper = std::min(upper_ - 1, (int) weak_upper_);
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
    int lower = std::max(lower_ + 1, (int) weak_lower_);
    int upper = std::min(upper_ - 1, (int) weak_upper_);
    for (int i = lower; i <= upper; i += 2) {
      const Evaluation& eval = GetEvaluation(i);
      if (eval.ProbGreaterEqual() < 1 - p) {
        return i;
      }
    }
    return upper + 2;
  }

  int ChildLogDerivative(Node* child, int eval_goal) const {
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

  void EnlargeEvaluations() {
    min_evaluation_ = weak_lower_;
    int desired_size = ToEvaluationIndex(weak_upper_) + 1;
    if (evaluations_) {
      evaluations_ = (Evaluation*) realloc(evaluations_, desired_size * sizeof(Evaluation));
    } else {
      evaluations_ = (Evaluation*) malloc(desired_size * sizeof(Evaluation));
    }
  }

  void AddDescendants(NVisited n) { descendants_ += n; }

  void ResetDescendants() { descendants_ = 0; }

  Eval NextPositionEvalGoal(float prob_min, float prob_max, int last_eval_goal) const {
    Eval best_eval = kLessThenMinEval;
    double best_value = -DBL_MAX;
    for (int i = std::max(lower_ + 1, (int) weak_lower_); i <= std::min(upper_ - 1, (int) weak_upper_); i += 2) {
      const Evaluation& eval = GetEvaluation(i);
      if (eval.ProbGreaterEqual() < prob_min || eval.ProbGreaterEqual() > prob_max ||
          eval.ProofNumber() == 0 || eval.DisproofNumber() == 0 ||
          (i + 2 < upper_ && i < weak_upper_ && GetEvaluation(i + 2).ProbGreaterEqual() > 1 - kMinProbEvalGoal) ||
          (i - 2 > lower_ && i > weak_lower_ && GetEvaluation(i - 2).ProbGreaterEqual() < kMinProbEvalGoal)) {
        continue;
      }
      double cur_value = i == last_eval_goal ? 2 * kLogDerivativeMinusInf : eval.MaxLogDerivative();

      if (cur_value > best_value) {
        best_value = cur_value;
        best_eval = i;
      }
    }
    return best_eval;
  }

  bool operator==(const Node& other) const {
    return Equals(other, false);
  }

  Square Depth() const {
    assert(depth_ != kNoSquare);
    return depth_;
  }

  // TODO: Avoid code duplication with SetSolved.
  void SetUpper(Eval upper) {
    upper_ = std::min(upper, upper_);
    leaf_eval_ = std::min(leaf_eval_, EvalToEvalLarge(upper_));
  }

  void SetLower(Eval lower) {
    lower_ = std::max(lower, lower_);
    leaf_eval_ = std::max(leaf_eval_, EvalToEvalLarge(lower_));
  }

  // Measures the progress towards solving the position (lower is better).
  // Starts from 0, decreases until kLogDerivativeMinusInf until partially
  // solved, then decreases more until becoming -inf.
  virtual double Advancement(Eval lower, Eval upper) const {
    double result = -DBL_MAX;
    Eval actual_lower = std::max(lower_ + 1, (int) weak_lower_);
    Eval actual_upper = std::min(upper_ - 1, (int) weak_upper_);
    for (int i = actual_lower; i <= actual_upper; i += 2) {
      result = std::max(result, (double) GetEvaluation(i).MaxLogDerivative());
    }
    if (result == kLogDerivativeMinusInf) {
      result += log(RemainingWork(actual_lower, actual_upper)) - 1E5;
    }
    return result;
  }

  double RemainingWork(Eval lower, Eval upper) const;

  bool ToBeSolved(Eval lower, Eval upper, int num_tree_nodes, NVisited total_visited) {
    double remaining_work = RemainingWork(lower, upper);
    EvalLarge delta = std::max(EvalToEvalLarge(lower) - leaf_eval_, leaf_eval_ - EvalToEvalLarge(upper));

    double mult = 1;
    double frac = num_tree_nodes * 2000.0 / total_visited;
    if (frac < 1) {
      mult = 5 * (frac - 0.8);
    }
    return remaining_work < mult * std::min(70000, 10000 + std::max(0, delta) * 250);
  }

  uint8_t Evaluator() const { return evaluator_; }

  bool Equals(const Node& other, bool approx) const {
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

  virtual bool IsSolved(Eval lower, Eval upper, bool approx) const {
    assert((lower - kMinEval) % 2 == 1);
    return upper_ <= lower || lower_ >= upper || lower_ == upper_ || (approx && SolveProbabilityNoLock(lower, upper) < 0.05);
  }

  bool IsSolved() const {
    return IsSolved(-63, 63, false);
  }

  virtual Evaluation* MutableEvaluation(Eval eval_goal) final {
    assert((eval_goal - kMinEval) % 2 == 1);
    assert(eval_goal >= weak_lower_ && eval_goal <= weak_upper_);
    assert(eval_goal >= lower_ && eval_goal <= upper_);
    assert(min_evaluation_ <= weak_lower_);
    int index = ToEvaluationIndex(eval_goal);
    assert(index >= 0 && index <= (weak_upper_ - min_evaluation_) / 2);
    return &evaluations_[index];
  }

  virtual bool IsLeaf() const {
    return is_leaf_;
  }

 protected:
  BitPattern player_;
  BitPattern opponent_;
  std::atomic_uint64_t descendants_;
  Evaluation* evaluations_;
  EvalLarge leaf_eval_;
  Square n_empties_;
  Eval lower_;
  Eval upper_;
  Eval weak_lower_;
  Eval weak_upper_;
  Eval min_evaluation_;
  Square depth_;
  Square eval_depth_;
  uint8_t evaluator_;
  bool is_leaf_;

  void UpdateLeafEvaluation(int i) {
    assert(i >= weak_lower_ && i <= weak_upper_);
    assert(i >= lower_ && i <= upper_);
    assert(eval_depth_ >= 1 && eval_depth_ <= 4);
    assert(n_empties_ >= 0 && n_empties_ <= 63);
    Evaluation* evaluation = MutableEvaluation(i);
    assert(evaluation != nullptr);
    EvalLarge i_large = EvalToEvalLarge(i);
    evaluation->SetLeaf(player_, opponent_, i_large, leaf_eval_, eval_depth_, n_empties_);
    assert(
        (evaluation->ProbGreaterEqual() == 0 || evaluation->ProbGreaterEqual() == 1) ==
        (evaluation->MaxLogDerivative() == kLogDerivativeMinusInf));
  }

  bool EqualsExceptDescendants(const Node& other) const {
    if (ToBoard() != other.ToBoard() ||
        lower_ != other.lower_ ||
        upper_ != other.upper_ ||
        weak_lower_ != other.weak_lower_ ||
        weak_upper_ != other.weak_upper_ ||
        leaf_eval_ != other.leaf_eval_) {
      return false;
    }

    for (int i = std::max(lower_ + 1, (int) weak_lower_); i <= std::min(upper_ - 1, (int) weak_upper_); i += 2) {
      if (GetEvaluation(i) != other.GetEvaluation(i)) {
        return false;
      }
    }
    return true;
  }
};

class TreeNode : public Node {
 public:
  TreeNode(const TreeNode&) = delete;

  TreeNode(const Node& node) :
      Node(node),
      children_(nullptr),
      fathers_(nullptr),
      n_fathers_(0),
      n_children_(0),
      n_threads_working_(0) {}

  TreeNode() :
      Node(),
      children_(nullptr),
      fathers_(nullptr),
      n_fathers_(0),
      n_children_(0),
      n_threads_working_(0) {}

  ~TreeNode() {
    if (n_fathers_ != 0) {
      free(fathers_);
    }
    if (n_children_ != 0) {
      delete[] children_;
    }
  }

  virtual void Reset(Board b, int depth, uint8_t evaluator) {
    Reset(b.Player(), b.Opponent(), depth, evaluator);
  }

  virtual void Reset(BitPattern player, BitPattern opponent, int depth,
                     uint8_t evaluator);

  void SetSolved(EvalLarge lower, EvalLarge upper, const EvaluatorDerivative& evaluator_derivative);

  void SetSolvedNoUpdate(EvalLarge lower, EvalLarge upper) {
    assert(lower % 16 == 0);
    assert(upper % 16 == 0);
    assert(IsLeafNoLock());
    assert(kMinEvalLarge <= leaf_eval_ && leaf_eval_ <= kMaxEvalLarge);

    leaf_eval_ = std::min(upper, std::max(lower, leaf_eval_));
    Eval lower_small = EvalLargeToEvalRound(lower);
    Eval upper_small = EvalLargeToEvalRound(upper);
    lower_ = MaxEval(lower_, lower_small);
    upper_ = MinEval(upper_, upper_small);
    assert(lower_ <= 64);
    assert(upper_ >= -64);
  }

  bool IsLeaf() const override {
    auto guard = GetGuard();
    return IsLeafNoLock();
  }

  virtual bool IsLockableLeaf(Eval solved_lower, Eval solved_upper) {
    return IsLeafNoLock() && !Node::IsSolved(solved_lower, solved_upper, false);
  }

  // Locks a leaf by setting n_threads_working_ = 1. If n_threads_working_ > 0,
  // the lock fails. We should never have n_threads_working_ > 1 for a leaf.
  bool TryLockLeaf(Eval solved_lower, Eval solved_upper) {
    auto guard = GetGuard();
    // Some other thread has added this node's children or solved this node
    // before locking it.
    if (!IsLockableLeaf(solved_lower, solved_upper)) {
      return false;
    }
    uint8_t expected = 0;
    assert(n_threads_working_ <= 1);
    bool locked = n_threads_working_.compare_exchange_strong(expected, (uint8_t) 1);
    assert((locked && expected == 0 && n_threads_working_ == 1) || (!locked && expected == 1));
    return locked;
  }

  void UpdateFathers() {
    // Use an index to avoid co-modification (if some other thread adds fathers in the meantime).
    unsigned int n_fathers;
    {
      auto guard = GetGuard();
      n_fathers = n_fathers_;
    }
    for (unsigned int i = 0; i < n_fathers; ++i) {
      TreeNode* father = fathers_[i];
      assert(!father->IsLeaf());
      father->UpdateFather();
      father->UpdateFathers();
    }
  }

  virtual void UpdateFather() {
    auto guard = GetGuard();
    UpdateFatherNoLock();
  }

  std::vector<Node> GetChildren() {
    auto guard = GetGuard();
    std::vector<Node> children;
    auto start = ChildrenStart();
    auto end = ChildrenEnd();
    for (auto child = start; child != end; ++child) {
      children.push_back(**child);
    }
    return children;
  }

  void SetChildren(const std::vector<TreeNode*>& children, const EvaluatorDerivative& evaluator);

  bool HasLeafEval() {
    auto guard = GetGuard();
    return HasLeafEvalNoLock();
  }

  bool HasLeafEvalNoLock() {
    return leaf_eval_ != kLessThenMinEvalLarge;
  }

  void SetChildren(const std::vector<TreeNode*>& children) {
    auto guard = GetGuard();
    SetChildrenNoLock(children);
  }

  virtual std::vector<Node> Fathers();

  void ExtendToAllEvals() {
    ExtendEval(-63, 63);
  }

  void CopyAndEnlargeToAllEvals(const Node& node) {
    FromOther(node);
    min_evaluation_ = -63;
    weak_lower_ = -63;
    weak_upper_ = 63;
    EnlargeEvaluations();

    for (int i = WeakLower(); i <= WeakUpper(); i += 2) {
      if (i < node.WeakLower()) {
        MutableEvaluation(i)->SetProving(node.GetEvaluation(node.WeakLower()), node.WeakLower() - i);
      } else if (i > node.WeakUpper()) {
        MutableEvaluation(i)->SetDisproving(node.GetEvaluation(node.WeakUpper()), i - node.WeakUpper());
      } else {
        *MutableEvaluation(i) = Evaluation(node.GetEvaluation(i));
      }
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

  TreeNode* BestChild(int eval_goal, float n_thread_multiplier) {
    auto guard = GetGuard();
    if (IsLeafNoLock()) {
      return this;
    }
    assert(!IsLeafNoLock());
    // If another thread has already solved this node, or if the weak lower /
    // upper changed.
    if (eval_goal < weak_lower_ || eval_goal > weak_upper_ || Node::IsSolved(eval_goal, eval_goal, false)) {
      return nullptr;
    }
    double best_child_value = -DBL_MAX;
    TreeNode* best_child = nullptr;
    auto start = ChildrenStart();
    auto end = ChildrenEnd();
    int child_eval_goal = -eval_goal;
    for (auto iter = start; iter != end; ++iter) {
      TreeNode* child = *iter;
      auto child_guard = child->GetGuard();
      if (child_eval_goal <= child->lower_ || child_eval_goal >= child->upper_ ||
          child_eval_goal < child->weak_lower_ || child_eval_goal > child->weak_upper_) {
        continue;
      }
      assert(!child->Node::IsSolved(child_eval_goal, child_eval_goal, false));
      assert(!child->GetEvaluation(child_eval_goal).IsSolved());
//      assert(father_eval.ProbGreaterEqual() >= 1 - child->GetEvaluation(child_eval_goal).ProbGreaterEqual() - 0.001);
      double cur_loss = child->GetValue(*this, child_eval_goal, n_thread_multiplier);
      if (cur_loss > best_child_value) {
        best_child = child;
        best_child_value = cur_loss;
      }
    }
    return best_child;
  }

  virtual TreeNode** ChildrenStart() {
    return children_;
  }

  virtual TreeNode** ChildrenEnd() {
    return children_ + n_children_;
  }

  void ExtendEval(Eval weak_lower, Eval weak_upper) {
    do {
      extend_eval_failed_ = false;
      ExtendEvalInternal(weak_lower, weak_upper);
    } while (extend_eval_failed_);
  }

  bool WeakLowerUpperContains(Eval weak_lower, Eval weak_upper) {
    auto guard = GetGuard();
    return weak_lower_ <= weak_lower && weak_upper <= weak_upper_;
  }

  template<class T>
  std::unique_ptr<LeafToUpdate<T>> AsLeaf(int last_eval_goal) {
    auto guard = GetGuard();
    auto eval_goal = NextPositionEvalGoal(0, 1, Node::SolveProbability(-63, 63) > 0.05 ? kLessThenMinEval : last_eval_goal);
    if (eval_goal == kLessThenMinEval) {
      return nullptr;
    }
    return std::make_unique<LeafToUpdate<T>>(AsLeafNoLock<T>(eval_goal));
  }

  template<class T>
  LeafToUpdate<T> AsLeafWithGoal(Eval eval_goal) {
    auto guard = GetGuard();
    return AsLeafNoLock<T>(eval_goal);
  }

  template<class T>
  void UpdateAlphaBeta(LeafToUpdate<T>* leaf) {
    auto guard = GetGuard();
    UpdateAlphaBetaNoLock(leaf);
  }

  double SolveProbability(Eval lower, Eval upper) const override {
    auto guard = GetGuard();
    return Node::SolveProbability(lower, upper);
  }

  bool IsSolved(Eval lower, Eval upper, bool approx) const override {
    auto guard = GetGuard();
    return Node::IsSolved(
        // In rare race conditions, lower_ and upper_ might have changed after lo.
        MaxEval(lower_ + 1, lower),
        MinEval(upper_ - 1, upper),
        approx);
  }

  bool ToBeSolved(Eval lower, Eval upper, int num_tree_nodes, NVisited total_visited) {
    auto guard = GetGuard();
    return Node::ToBeSolved(lower, upper, num_tree_nodes, total_visited);
  }

  double RemainingWork(Eval lower, Eval upper) const {
    auto guard = GetGuard();
    return Node::RemainingWork(lower, upper);
  }

  std::pair<Eval, Eval> ExpectedWeakLowerUpper() const {
    auto guard = GetGuard();
    Eval expected_weak_lower = std::min(upper_ - 1, std::max(lower_ + 1, (int) weak_lower_));
    Eval expected_weak_upper = std::max(lower_ + 1, std::min(upper_ - 1, (int) weak_upper_));
    assert(expected_weak_lower <= expected_weak_upper);

    // NOTE: expected_weak_lower < upper_, no need to check it here.
    if (expected_weak_lower - 2 > lower_ && expected_weak_lower >= weak_lower_ && expected_weak_lower <= weak_upper_ && ProbGreaterEqual(expected_weak_lower) < 1 - kZeroPercForWeak) {
      expected_weak_lower -= 2;
    } else {
      for (int i = expected_weak_lower + 2; i <= std::min(upper_ - 1, (int) weak_upper_); i += 2) {
        if (ProbGreaterEqual(i) < 0.99) {
          expected_weak_lower = i - 2;
          break;
        }
      }
    }
    if (expected_weak_upper + 2 < upper_ && expected_weak_upper >= weak_lower_ && expected_weak_upper <= weak_upper_ && ProbGreaterEqual(expected_weak_upper) > kZeroPercForWeak) {
      expected_weak_upper += 2;
    } else {
      for (int i = expected_weak_upper - 2; i >= std::max(lower_ + 1, (int) weak_lower_); i -= 2) {
        if (ProbGreaterEqual(i) > 0.01) {
          expected_weak_upper = i + 2;
          break;
        }
      }
    }
    return {expected_weak_lower, expected_weak_upper};
  }

  void IncreaseNThreadsWorking() {
    assert(!IsLeaf());
    n_threads_working_++;
  }

  void DecreaseNThreadsWorking() {
    assert(n_threads_working_ > 0);
    n_threads_working_--;
  }

  int NThreadsWorking() const {
    return n_threads_working_;
  }

  double Advancement(Eval lower, Eval upper) const override {
    auto guard = GetGuard();
    return Node::Advancement(lower, upper);
  }

  void SetLeafNeverSolved() {
    lower_ = -64;
    upper_ = 64;
    ExtendToAllEvals();
    assert(weak_lower_ == kMinEval + 1);
    assert(weak_upper_ == kMaxEval - 1);
    assert(min_evaluation_ == weak_lower_);
    for (int i = weak_lower_; i <= weak_upper_; i += 2) {
      MutableEvaluation(i)->SetLeaf(player_, opponent_, EvalToEvalLarge(i), EvalToEvalLarge(i), 1, n_empties_);
      assert(GetEvaluation(i).ProbGreaterEqual() > 0 &&
             GetEvaluation(i).ProbGreaterEqual() < 1);
    }
    leaf_eval_ = 4;
  }

  virtual void SetLeafEval(EvalLarge leaf_eval, Square eval_depth) {
    auto guard = GetGuard();
    if (HasLeafEvalNoLock()) {
      return;
    }
    assert(IsLeafNoLock());
    assert(kMinEvalLarge <= leaf_eval && leaf_eval <= kMaxEvalLarge);
    assert(eval_depth > 0 && eval_depth <= 4);

    eval_depth_ = eval_depth;
    leaf_eval_ = std::max(
        EvalToEvalLarge(lower_), std::min(EvalToEvalLarge(upper_), leaf_eval));
    assert(leaf_eval_ >= EvalToEvalLarge(lower_) && leaf_eval_ <= EvalToEvalLarge(upper_));
  }

  void UpdateLeafWeakLowerUpper(Eval weak_lower, Eval weak_upper) {
    assert(IsLeafNoLock());
    assert(weak_lower <= weak_upper);
    weak_lower_ = weak_lower;
    weak_upper_ = weak_upper;
    EnlargeEvaluations();
    UpdateLeafEvaluations();
    assert(min_evaluation_ <= weak_lower_);
  }

  void SetChildrenNoUpdate(const std::vector<TreeNode*>& children) {
    assert(n_children_ == 0 || n_children_ == 255);
    n_children_ = (Square) children.size();
    children_ = new TreeNode*[n_children_];
    is_leaf_ = false;
    for (int i = 0; i < n_children_; ++i) {
      TreeNode* child = children[i];
      child->AddFather(this);
      children_[i] = child;
    }
  }

 protected:
  std::mutex* mutex_;
  TreeNode** children_;
  TreeNode** fathers_;
  uint32_t n_fathers_;
  Square n_children_;
  std::atomic_uint8_t n_threads_working_;
  static std::atomic_bool extend_eval_failed_;
  static std::vector<std::vector<std::mutex>> mutex_at_depth_;

  friend std::ostream& operator<<(std::ostream& stream, const TreeNode& n);
  
  virtual std::optional<std::lock_guard<std::mutex>> GetGuard() const {
    return std::optional<std::lock_guard<std::mutex>>{*mutex_};
  }

  void ExtendEvalInternal(Eval weak_lower, Eval weak_upper) {
    {
      auto guard = GetGuard();
      // NOTE: We cannot return if weak_lower_, weak_upper_ of this node are OK,
      // because there might be a thread working on this node's descendants with
      // old weak_lower, weak_upper. And we cannot check that n_threads_working
      // is 0, because the thread might have gotten to this node's descendants
      // through a transposition.
      if (IsLeafNoLock()) {
        UpdateLeafWeakLowerUpper(weak_lower, weak_upper);
        return;
      }
    }
    for (int i = 0; i < n_children_; ++i) {
      children_[i]->ExtendEvalInternal(-weak_upper, -weak_lower);
    }
    {
      auto guard = GetGuard();
      weak_lower_ = weak_lower;
      weak_upper_ = weak_upper;
      assert(weak_lower_ <= weak_upper_);
      EnlargeEvaluations();
      UpdateFatherNoLock();
    }
  }

  void UpdateLeafEvaluations() {
    assert(IsLeafNoLock());
    for (int i = MaxEval(lower_ + 1, weak_lower_); i <= MinEval(upper_ - 1, weak_upper_); i += 2) {
      UpdateLeafEvaluation(i);
    }
  }

  virtual bool IsLeafNoLock() const {
    assert(is_leaf_ == (n_children_ == 0));
    assert(is_leaf_ == (children_ == nullptr));
    return Node::IsLeaf();
  }

  template<class T>
  LeafToUpdate<T> AsLeafNoLock(Eval eval_goal) {
    LeafToUpdate<T> leaf;
    leaf.leaf_ = (T*) this;
    leaf.eval_goal_ = eval_goal;
    leaf.alpha_ = std::max(lower_ + 1, (int) weak_lower_);
    leaf.beta_ = std::min(upper_ - 1, (int) weak_upper_);

    UpdateAlphaBetaNoLock(&leaf);

    assert(leaf.alpha_ <= leaf.eval_goal_ && leaf.eval_goal_ <= leaf.beta_);
    return leaf;
  }

  virtual void ResetNoLock(
      BitPattern player, BitPattern opponent, int depth, uint8_t evaluator);

  void UpdateFatherNoLock() {
    assert((weak_lower_ - kMinEval) % 2 == 1);
    assert((weak_upper_ - kMinEval) % 2 == 1);
    assert((lower_ - kMinEval) % 2 == 0);
    assert((upper_ - kMinEval) % 2 == 0);
    assert(!IsLeafNoLock());
    Eval new_upper = lower_;
    leaf_eval_ = EvalToEvalLarge(lower_);
    for (int i = std::max(lower_ + 1, (int) weak_lower_); i <= std::min(upper_ - 1, (int) weak_upper_); i += 2) {
      MutableEvaluation(i)->Initialize();
    }
    auto start = ChildrenStart();
    auto end = ChildrenEnd();
    for (auto child = start; child != end; ++child) {
      UpdateWithChild(**child);
      new_upper = MaxEval(new_upper, (Eval) -(*child)->Lower());
    }
    if (new_upper < upper_) {
      upper_ = new_upper;
    }
    leaf_eval_ = std::min(leaf_eval_, EvalToEvalLarge(upper_));
    for (int i = MaxEval(lower_ + 1, weak_lower_); i <= MinEval(upper_ - 1, weak_upper_); i += 2) {
      MutableEvaluation(i)->Finalize();
    }
    assert(kMinEval <= lower_ && lower_ <= upper_ && upper_ <= kMaxEval);
    assert(leaf_eval_ >= EvalToEvalLarge(lower_) && leaf_eval_ <= EvalToEvalLarge(upper_));
  }

  bool IsUnderAnalyzed(const TreeNode& father, int father_eval_goal) const {
    double remaining_work = father.Node::RemainingWork(father_eval_goal, father_eval_goal);
    if (remaining_work < 100000) {
      return false;
    }

    if (leaf_eval_ < -father.leaf_eval_ + 2 * 8) {
      return rand() % 200 == 0;
    } else if (leaf_eval_ < -father.leaf_eval_ + 4 * 8) {
      return rand() % 400 == 0;
    } else if (leaf_eval_ < -father.leaf_eval_ + 6 * 8) {
      return rand() % 800 == 0;
    } else if (leaf_eval_ < -father.leaf_eval_ + 8 * 8) {
      return rand() % 1000 == 0;
    } else if (leaf_eval_ < -father.leaf_eval_ + 16 * 8) {
      return rand() % 20000 == 0;
    } else {
      return rand() % 100000 == 0;
    }
  }

  double GetValue(
      const TreeNode& father, int eval_goal, float n_thread_multiplier) const {
    const Evaluation& eval = GetEvaluation(eval_goal);
    const Evaluation& father_eval = father.GetEvaluation(-eval_goal);

//    double result = IsUnderAnalyzed(father, -eval_goal) ? -2 * kLogDerivativeMinusInf : 0;
    if (father_eval.ProbGreaterEqual() < 0.99) {
      return
          eval.LogDerivative(father_eval)
          - leaf_eval_ / (double) (kMaxEvalLarge - kMinEvalLarge)
          - n_thread_multiplier * NThreadsWorking() * father_eval.ProbLowerCubed()
          ;
    }
    return -eval.DisproofNumberSmall() - 0.4 * leaf_eval_ / 8.0;
  }

  virtual void UpdateWithChild(const TreeNode& child) {
    auto child_guard = child.GetGuard();
    // We cannot check the upper, because we are currently updating it.
    assert(leaf_eval_ >= EvalToEvalLarge(lower_));
    assert(child.leaf_eval_ >= EvalToEvalLarge(child.lower_) && child.leaf_eval_ <= EvalToEvalLarge(child.upper_));
    assert(min_evaluation_ <= weak_lower_);
    lower_ = MaxEval(lower_, (Eval) -child.upper_);
    leaf_eval_ = std::max(leaf_eval_, -child.leaf_eval_);
    assert(leaf_eval_ >= EvalToEvalLarge(lower_));
    // TODO: Make weak_lower_ >= lower_ to avoid extra computations.
    weak_lower_ = MaxEval(weak_lower_, (Eval) -child.weak_upper_);
    weak_upper_ = MinEval(weak_upper_, (Eval) -child.weak_lower_);
    for (int i = std::max(lower_ + 1, (int) weak_lower_); i <= std::min(upper_ - 1, (int) weak_upper_); i += 2) {
      Evaluation* father_eval = MutableEvaluation(i);
      assert(-i >= child.weak_lower_ && -i <= child.weak_upper_);
      assert(-i <= child.upper_);
      if (-i < child.lower_) {
        continue;
      }
      const Evaluation& eval = child.GetEvaluation(-i);
      father_eval->UpdateFatherWithThisChild(eval);
    }
    assert(weak_lower_ <= weak_upper_);
  }

  virtual void AddFather(TreeNode* father) {
    auto guard = GetGuard();
    if (n_fathers_ == 0) {
      fathers_ = (TreeNode**) malloc(sizeof(TreeNode*));
    } else {
      fathers_ = (TreeNode**) realloc(fathers_, (n_fathers_ + 1) * sizeof(TreeNode*));
    }
    fathers_[n_fathers_] = father;
    depth_ = father->depth_ == kNoSquare ? kNoSquare : father->depth_ + 1;
    n_fathers_++;
  }

  void SetChildrenNoLock(const std::vector<TreeNode*>& children) {
    SetChildrenNoUpdate(children);
    UpdateFatherNoLock();
    assert(AreChildrenCorrect());
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

  template<class T>
  void UpdateAlphaBetaNoLock(LeafToUpdate<T>* leaf) {
    leaf->alpha_ = MinEval(leaf->eval_goal_, MaxEval(
        leaf->alpha_, GetPercentileLower(kProbForEndgameAlphaBeta)));
    leaf->beta_ = MaxEval(leaf->eval_goal_, MinEval(
        leaf->beta_, GetPercentileUpper(kProbForEndgameAlphaBeta)));
  }
};

std::ostream& operator<<(std::ostream& stream, const Node& b);

template<class Node>
class LeafToUpdate {
 public:
  static std::unique_ptr<LeafToUpdate> BestDescendant(Node* node, float n_thread_multiplier, int last_eval_goal) {
    auto leaf = node->template AsLeaf<Node>(last_eval_goal);
    if (leaf == nullptr) {
      return nullptr;
    }
    return BestDescendant(*leaf, n_thread_multiplier);
  }

  static std::unique_ptr<LeafToUpdate> BestDescendant(Node* node, float n_thread_multiplier, int last_eval_goal, const std::vector<Node*>& parents) {
    std::unique_ptr<LeafToUpdate> leaf_opt = BestDescendant(node, n_thread_multiplier, last_eval_goal);
    if (!leaf_opt) {
      return nullptr;
    }
    LeafToUpdate leaf = *leaf_opt;
    leaf.parents_.insert(leaf.parents_.begin(), parents.begin(), parents.end());
    for (auto parent : parents) {
      parent->IncreaseNThreadsWorking();
    }
    return std::make_unique<LeafToUpdate>(leaf);
  }

  static LeafToUpdate Leaf(const std::vector<Node*>& sequence) {
    LeafToUpdate leaf = sequence[0]->template AsLeafWithGoal<Node>(1);
    for (int i = 1; i < sequence.size(); ++i) {
      Node* descendant = sequence[i];
      leaf.ToChild(descendant, 0);
    }
    if (!leaf.Leaf()->TryLockLeaf(-63, 63)) {
      throw std::invalid_argument("Cannot lock a leaf");
    }
    return leaf;
  }

  static std::unique_ptr<LeafToUpdate> BestDescendant(LeafToUpdate& result, float n_thread_multiplier) {
    while (true) {
      Node* best_child = result.leaf_->BestChild(result.eval_goal_, n_thread_multiplier);
      if (best_child == nullptr) {
        for (Node* node : result.parents_) {
          node->DecreaseNThreadsWorking();
        }
        return nullptr;
      } else if (best_child == result.leaf_) {
        // This means that result.leaf_ is a leaf.
        if (result.leaf_->TryLockLeaf(result.Alpha(), result.Beta())) {
          assert(!result.leaf_->IsSolved(result.Alpha(), result.Beta(), false));
          return std::make_unique<LeafToUpdate<Node>>(result);
        } else {
          for (Node* node : result.parents_) {
            node->DecreaseNThreadsWorking();
          }
          return nullptr;
        }
      }
      result.ToChild(best_child, 0);
    }
  }

  Eval EvalGoal() const { return eval_goal_; }
  Eval Alpha() const { return alpha_; }
  Eval Beta() const { return beta_; }
  double Loss() const { return loss_; }
  const std::vector<Node*>& Parents() const { return parents_; }
  Node* Leaf() const { return leaf_; }

  bool operator==(const LeafToUpdate& other) const {
    return
        leaf_ == other.leaf_
        && parents_ == other.parents_
        && eval_goal_ == other.eval_goal_
        && alpha_ == other.alpha_
        && beta_ == other.beta_
        && loss_ == other.loss_;
  }
  bool operator!=(const LeafToUpdate& other) const { return !operator==(other); }

  void Finalize(NVisited n_visited) {
    leaf_->UpdateFathers();
    leaf_->AddDescendants(n_visited);
    leaf_->DecreaseNThreadsWorking();
    for (auto parent : Parents()) {
      parent->AddDescendants(n_visited);
      parent->DecreaseNThreadsWorking();
    }
  }

 protected:
  friend class TreeNode;
  Node* leaf_;
  std::vector<Node*> parents_;
  Eval eval_goal_;
  Eval alpha_;
  Eval beta_;
  double loss_;

  void ToChild(Node* child, double extra_loss) {
    int tmp;

    leaf_->IncreaseNThreadsWorking();
    assert(leaf_->NThreadsWorking() > 0);
    parents_.push_back(leaf_);
    leaf_ = child;
    eval_goal_ = -eval_goal_;
    tmp = alpha_;
    alpha_ = -beta_;
    beta_ = -tmp;
    loss_ += extra_loss;
    leaf_->template UpdateAlphaBeta<Node>(this);
  }
};

typedef LeafToUpdate<TreeNode> TreeNodeLeafToUpdate;
#endif  // OTHELLOSENSEI_EVALUATEDERIVATIVE_TREE_NODE_H