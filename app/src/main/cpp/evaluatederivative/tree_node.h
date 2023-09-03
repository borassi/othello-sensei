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
#include <concepts>
#include <float.h>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <mutex>
#include <optional>
#include <queue>
#include <set>
#include <string.h>
#include <unordered_set>

#include "evaluation.h"
#include "../board/bitpattern.h"
#include "../board/board.h"
#include "../board/get_moves.h"
#include "../utils/misc.h"
#include "../utils/serializable_boolean_vector.h"

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
  WINNING = 1
};

template<class Node>
class LeafToUpdate;

class EvaluatorDerivative;

class Node {
 public:
  Node() :
      evaluations_(nullptr),
      is_leaf_(true),
      leaf_eval_(kLessThenMinEvalLarge),
      evaluator_(255) {}

  Node(const Node& other) :
      evaluations_(nullptr),
      is_leaf_(other.is_leaf_),
      player_(other.player_),
      opponent_(other.opponent_),
      n_empties_(other.n_empties_),
      descendants_(other.descendants_.load()),
      leaf_eval_(other.leaf_eval_),
      lower_(other.lower_),
      upper_(other.upper_),
      weak_lower_(other.weak_lower_),
      weak_upper_(other.weak_upper_),
      min_evaluation_(other.min_evaluation_),
      depth_(other.depth_),
      eval_depth_(other.eval_depth_),
      evaluator_(other.evaluator_) {
    EnlargeEvaluationsInternal();
    for (int i = weak_lower_; i <= weak_upper_; i += 2) {
      *MutableEvaluation(i) = other.GetEvaluation(i);
    }
  }
  // We cannot convert to Node directly, because it calls a copy constructor
  // (which creates an infinite loop). The pointers do the trick.
  Node(const Node* const other) : Node(*other) {}

  ~Node() {
    if (evaluations_ != nullptr) {
      free(evaluations_);
    }
  }

  static Node Deserialize(const std::vector<char>& serialized, int version, std::vector<CompressedFlip>* father_flips) {
    Node n;
    Board board = Board::Deserialize(serialized.begin());
    n.player_ = board.Player();
    n.opponent_ = board.Opponent();
    n.n_empties_ = board.NEmpties();
    int i = kSerializedBoardSize;

    while (true) {
      CompressedFlip compressed_flip = 0;
      compressed_flip |= (u_int8_t) serialized[i] | ((u_int8_t) serialized[i+1] << 8) | ((u_int8_t) serialized[i+2] << 16);
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

    n.descendants_ = (u_int64_t) *((float*) &(serialized[i]));
    i += sizeof(float);
    n.lower_ = (Eval) serialized[i++];
    n.upper_ = (Eval) serialized[i++];
    n.weak_lower_ = -63;
    n.weak_upper_ = 63;
    n.min_evaluation_ = -63;
    Eval last_1 = (Eval) serialized[i++];
    Eval first_0 = (Eval) serialized[i++];
    n.is_leaf_ = (bool) serialized[i++];

    n.EnlargeEvaluationsInternal();

    for (int eval = n.weak_lower_; eval <= n.lower_; eval += 2) {
      n.MutableEvaluation(eval)->SetProved();
    }
    for (int eval = n.weak_upper_; eval >= std::max(n.upper_, n.weak_lower_); eval -= 2) {
      n.MutableEvaluation(eval)->SetDisproved();
    }
    for (int eval = last_1 + 2; eval <= first_0 - 2; eval += 2) {
      Probability prob = serialized[i++];
      PN proof_number = serialized[i++];
      PN disproof_number = serialized[i++];
      u_int32_t max_log_derivative = 0;
      u_int8_t first_byte = serialized[i];
      if (first_byte & (1U << 7)) {
        max_log_derivative |= (first_byte & ~(1U << 7)) << 16;
        i++;
      }
      max_log_derivative |= (u_int8_t) serialized[i++] << 8;
      max_log_derivative |= (u_int8_t) serialized[i++];
      n.MutableEvaluation(eval)->Set(prob, proof_number, disproof_number, -max_log_derivative);
    }
    PN proof, disproof;
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
    } else {
      n.leaf_eval_ = 0;
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
    return n;
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

  float LeafEval() const { return leaf_eval_ / 8.0F; }

  float GetEval() const {
    int lower = std::max(lower_ + 1, (int) weak_lower_);
    int upper = std::min(upper_ - 1, (int) weak_upper_);
    double eval = lower - 1;
    for (int i = lower; i <= upper; i += 2) {
      double prob = GetEvaluation(i).ProbGreaterEqual();
      eval += 2 * prob;
    }
    return (float) eval;
  }

  const Evaluation& GetEvaluation(int eval_goal) const {
    assert((eval_goal - kMinEval) % 2 == 1);
    assert(eval_goal >= weak_lower_ && eval_goal <= weak_upper_);
//    assert(eval_goal >= lower_ && eval_goal <= upper_);
    assert(min_evaluation_ <= weak_lower_);
    int index = ToEvaluationIndex(eval_goal);
    assert(index >= 0 && index <= (weak_upper_ - min_evaluation_) / 2);
    assert(evaluations_ != nullptr);
    return evaluations_[index];
  }

  virtual double SolveProbability(Eval lower, Eval upper) const {
    return std::max(SolveProbabilityUpper(upper), SolveProbabilityLower(lower));
  }

  virtual double SolveProbabilityUpper(Eval upper) const {
    auto goal = MinEval(GetPercentileLower(0.5F), upper);
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
    assert(goal <= weak_upper_);
    // This can rarely happen when we are in dire need of updating the eval.
    // Only happens with multiple threads.
    if (goal < weak_lower_) {
      return 0.5;
    }
    return 1.0 - GetEvaluation(goal).ProbGreaterEqual();
  }

  Eval GetPercentileUpper(double p) const {
    int lower = std::max(lower_ - 1, (int) weak_lower_);
    int upper = std::min(upper_ + 1, (int) weak_upper_);
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
    int lower = std::max(lower_ - 1, (int) weak_lower_);
    int upper = std::min(upper_ + 1, (int) weak_upper_);
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

  void EnlargeEvaluationsInternal() {
    min_evaluation_ = weak_lower_;
    int desired_size = ToEvaluationIndex(weak_upper_) + 1;
    if (evaluations_) {
      evaluations_ = (Evaluation*) realloc(evaluations_, desired_size * sizeof(Evaluation));
    } else {
      evaluations_ = (Evaluation*) malloc(desired_size * sizeof(Evaluation));
    }
  }

  void AddDescendants(NVisited n) {
    descendants_ += n;
  }

  Eval NextPositionEvalGoal(float prob_min, float prob_max, int last_eval_goal) const {
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

  Square Depth() const { return depth_; }

  // TODO: Avoid code duplication with SetSolved.
  void SetUpper(Eval upper) {
    upper_ = std::min(upper, upper_);
    leaf_eval_ = std::min(leaf_eval_, EvalToEvalLarge(upper_));
    for (int i = weak_upper_; i >= std::max(upper_, weak_lower_); i -= 2) {
      MutableEvaluation(i)->SetDisproved();
    }
  }

  void SetLower(Eval lower) {
    lower_ = std::max(lower, lower_);
    leaf_eval_ = std::max(leaf_eval_, EvalToEvalLarge(lower_));
    for (int i = weak_lower_; i <= std::min(lower_, weak_upper_); i += 2) {
      MutableEvaluation(i)->SetProved();
    }
  }

  // Measures the progress towards solving the position (lower is better).
  // Starts from 0, decreases until kLogDerivativeMinusInf until partially
  // solved, then decreases more until becoming -inf.
  double Advancement(Eval lower, Eval upper) const {
    double result = -std::numeric_limits<double>::infinity();
    for (int i = weak_lower_; i <= weak_upper_; i += 2) {
      result = std::max(result, (double) GetEvaluation(i).MaxLogDerivative());
    }
    if (result == kLogDerivativeMinusInf) {
      result += log(RemainingWork(lower, upper)) - 1E5;
    }
    return result;
  }

  virtual double RemainingWork(Eval lower, Eval upper) const;

  u_int8_t Evaluator() const { return evaluator_; }

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

  virtual bool IsSolved(Eval lower, Eval upper, bool approx) {
    assert((lower - kMinEval) % 2 == 1);
    return upper_ <= lower || lower_ >= upper || lower_ == upper_ || (approx && SolveProbability(lower, upper) < 0.1 && (GetEval() - leaf_eval_ / 8.0) < 0.1);
  }

  bool IsSolved() {
    return IsSolved(-63, 63, false);
  }

  virtual Evaluation* MutableEvaluation(Eval eval_goal) final {
    assert((eval_goal - kMinEval) % 2 == 1);
    assert(eval_goal >= weak_lower_ && eval_goal <= weak_upper_);
//    assert(eval_goal >= lower_ && eval_goal <= upper_);
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
  u_int8_t evaluator_;
  bool is_leaf_;

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

    for (int i = weak_lower_; i <= weak_upper_; i += 2) {
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
      n_children_(0),
      fathers_(nullptr),
      n_fathers_(0),
      n_threads_working_(0) {}

  TreeNode() :
      Node(),
      children_(nullptr),
      n_children_(0),
      fathers_(nullptr),
      n_fathers_(0),
      n_threads_working_(0) {}

  ~TreeNode() {
    if (n_fathers_ != 0) {
      free(fathers_);
    }
    if (n_children_ != 0) {
      delete[] children_;
    }
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

  void SetSolved(EvalLarge lower, EvalLarge upper, const EvaluatorDerivative& evaluator_derivative);

  bool IsLeaf() const override {
    ReadLock();
    return IsLeafNoLock();
  }

  bool TryLockLeaf() {
    auto guard = ReadLock();
    // Some other thread has added this node's children when I was not looking.
    if (!IsLeafNoLock()) {
      return false;
    }
    uint8_t expected = 0;
    bool locked = n_threads_working_.compare_exchange_strong(expected, (uint8_t) 1);
    assert((locked && expected == 0) || (!locked && expected == 1));
    return locked;
  }

  void UpdateFathers() {
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

  std::vector<Node> GetChildren() {
    auto guard = ReadLock();
    std::vector<Node> children;
    auto start = ChildrenStart();
    auto end = ChildrenEnd();
    for (auto child = start; child != end; ++child) {
      children.push_back(**child);
    }
    return children;
  }

  void SetChildren(std::vector<TreeNode*> children) {
    auto guard = WriteLock();
    SetChildrenNoLock(children);
  }

  virtual std::vector<Node> Fathers();

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

  virtual TreeNode** ChildrenStart() {
    return children_;
  }

  virtual TreeNode** ChildrenEnd() {
    return children_ + n_children_;
  }

  void ExtendEval(Eval weak_lower, Eval weak_upper) {
    {
      auto guard = WriteLock();
      if (weak_lower >= weak_lower_ && weak_upper <= weak_upper_) {
        return;
      }
      if (IsLeafNoLock()) {
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

  TreeNode* BestChild(int eval_goal, float n_thread_multiplier) {
    auto guard = ReadLock();
    if (eval_goal < weak_lower_ || eval_goal > weak_upper_) {
      return nullptr;
    }
    auto prob = GetEvaluation(eval_goal).ProbGreaterEqual();
    if (prob > 0.99) {
      return this->template BestChild<WINNING>(eval_goal, n_thread_multiplier);
    } else {
      return this->template BestChild<DEFAULT>(eval_goal, n_thread_multiplier);
    }
  }

  template<class T>
  std::optional<LeafToUpdate<T>> AsLeaf(int last_eval_goal) {
    auto guard = ReadLock();
    auto eval_goal = NextPositionEvalGoal(0, 1, last_eval_goal);
    if (eval_goal == kLessThenMinEval) {
      return std::nullopt;
    }
    return AsLeafNoLock<T>(eval_goal);
  }

  template<class T>
  LeafToUpdate<T> AsLeafWithGoal(Eval eval_goal) {
    auto guard = ReadLock();
    return AsLeafNoLock<T>(eval_goal);
  }

  template<class T>
  void UpdateAlphaBeta(LeafToUpdate<T>* leaf) {
    auto guard = ReadLock();
    UpdateAlphaBetaNoLock(leaf);
  }

  double SolveProbability(Eval lower, Eval upper) const override {
    auto guard = ReadLock();
    return Node::SolveProbability(lower, upper);
  }

  bool IsSolved(Eval lower, Eval upper, bool approx) override {
    auto guard = ReadLock();
    return Node::IsSolved(lower, upper, approx);
  }

  double RemainingWork(Eval lower, Eval upper) const override {
    auto guard = ReadLock();
    return Node::RemainingWork(lower, upper);
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

  void IncreaseNThreadsWorking() {
    n_threads_working_++;
  }

  void DecreaseNThreadsWorking() {
    n_threads_working_--;
  }

  int NThreadsWorking() const {
    return n_threads_working_;
  }

 protected:
  mutable std::mutex mutex_;
  TreeNode** children_;
  TreeNode** fathers_;
  u_int32_t n_fathers_;
  Square n_children_;
  std::atomic_uint8_t n_threads_working_;

  void UpdateLeafEvaluations() {
    assert(IsLeafNoLock());
    for (int i = weak_lower_; i <= weak_upper_; i += 2) {
      UpdateLeafEvaluation(i);
    }
  }

  virtual bool IsLeafNoLock() const {
    assert(is_leaf_ == (n_children_ == 0));
    assert(is_leaf_ == (children_ == nullptr));
    return Node::IsLeaf();
  }

  template<ChildValueType type>
  TreeNode* BestChild(int eval_goal, float n_thread_multiplier) {
    assert(!IsLeafNoLock());
    assert(eval_goal >= weak_lower_ && eval_goal <= weak_upper_);
    double best_child_value = -INFINITY;
    TreeNode* best_child = nullptr;
    auto start = ChildrenStart();
    auto end = ChildrenEnd();
    int child_eval_goal = -eval_goal;
    for (auto iter = start; iter != end; ++iter) {
      TreeNode* child = *iter;
      auto guard = child->ReadLock();
      if (child_eval_goal <= child->Lower() || child_eval_goal >= child->Upper() ||
          child_eval_goal < child->weak_lower_ || child_eval_goal > child->weak_upper_) {
        continue;
      }
      assert(!child->GetEvaluation(child_eval_goal).IsSolved());
//      assert(father_eval.ProbGreaterEqual() >= 1 - child->GetEvaluation(child_eval_goal).ProbGreaterEqual() - 0.001);
      double cur_loss = child->template GetValue<type>(*this, child_eval_goal, n_thread_multiplier);
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

  template<ChildValueType type>
  double GetValue(const TreeNode& father, int eval_goal, float n_thread_multiplier) const {
    if (type == WINNING) {
      return GetValueWinning(father, eval_goal);
    } else {
      return GetValueDefault(father, eval_goal, n_thread_multiplier);
    }
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

  virtual void SetLeafNoLock(EvalLarge leaf_eval, Square depth, Eval weak_lower, Eval weak_upper) {
    assert(IsLeafNoLock());
    assert(kMinEvalLarge <= leaf_eval && leaf_eval <= kMaxEvalLarge);
    assert(depth > 0 && depth <= 4);

    eval_depth_ = depth;
    leaf_eval_ = std::max(
        EvalToEvalLarge(lower_), std::min(EvalToEvalLarge(upper_), leaf_eval));
    weak_lower_ = weak_lower;
    weak_upper_ = weak_upper;
    EnlargeEvaluations();
    assert(leaf_eval_ >= EvalToEvalLarge(lower_) && leaf_eval_ <= EvalToEvalLarge(upper_));
    assert(min_evaluation_ <= weak_lower_);
  }

  void EnlargeEvaluations() {
    EnlargeEvaluationsInternal();
    if (!IsLeafNoLock()) {
      UpdateFatherNoLock();
    } else {
      UpdateLeafEvaluations();
    }
  }

  std::lock_guard<std::mutex> ReadLock() const {
    return std::lock_guard<std::mutex>{mutex_};
  }

  std::lock_guard<std::mutex> WriteLock() const {
    return std::lock_guard<std::mutex>{mutex_};
  }

  virtual void ResetNoLock(
      BitPattern player, BitPattern opponent, int depth,
      u_int8_t evaluator, EvalLarge leaf_eval, Square eval_depth,
      Eval weak_lower, Eval weak_upper);

  void UpdateFatherNoLock() {
    assert((weak_lower_ - kMinEval) % 2 == 1);
    assert((weak_upper_ - kMinEval) % 2 == 1);
    assert((lower_ - kMinEval) % 2 == 0);
    assert((upper_ - kMinEval) % 2 == 0);
    assert(!IsLeafNoLock());
    Eval old_upper = upper_;
    upper_ = kLessThenMinEval;
    leaf_eval_ = EvalToEvalLarge(lower_);
    for (int i = weak_lower_; i <= weak_upper_; i += 2) {
      MutableEvaluation(i)->Initialize();
    }
    auto start = ChildrenStart();
    auto end = ChildrenEnd();
    bool shallow = GetNVisited() < 50000;
    for (auto child = start; child != end; ++child) {
      UpdateWithChild(**child, shallow);
    }
    if (old_upper < upper_) {
      upper_ = old_upper;
      leaf_eval_ = std::min(leaf_eval_, EvalToEvalLarge(upper_));
    }
    for (int i = weak_lower_; i <= weak_upper_; i += 2) {
      if (i >= lower_ && i <= upper_) {
        MutableEvaluation(i)->Finalize(shallow);
      } else if (i > upper_) {
        MutableEvaluation(i)->SetDisproved();
      } else {
        MutableEvaluation(i)->SetProved();
      }
      assert(GetEvaluation(i).IsSolved() == (i < lower_ || i > upper_));
    }
    assert(kMinEval <= lower_ && lower_ <= upper_ && upper_ <= kMaxEval);
    assert(leaf_eval_ >= EvalToEvalLarge(lower_) && leaf_eval_ <= EvalToEvalLarge(upper_));
  }

  bool IsUnderAnalyzed(const TreeNode& father, int eval_goal) const {
    NVisited father_visited = father.GetNVisited();
    float final_visited = 0.00005 * (father_visited + father.Node::RemainingWork(eval_goal, eval_goal));
    float current_visited;
    if (leaf_eval_ < -father.leaf_eval_ + 2 * 8) {
      current_visited = 0.02 * father_visited;
    } else if (leaf_eval_ < -father.leaf_eval_ + 4 * 8) {
      current_visited = 0.01 * father_visited;
    } else if (leaf_eval_ < -father.leaf_eval_ + 8 * 8) {
      current_visited = 0.005 * father_visited;
    } else if (leaf_eval_ < -father.leaf_eval_ + 16 * 8) {
      current_visited = 0.002 * father_visited;
    } else {
      current_visited = 0.001 * father_visited;
    }
    return GetNVisited() < std::min(current_visited, final_visited);
  }

  double GetValueDefault(
      const TreeNode& father, int eval_goal, float n_thread_multiplier) const {
    const Evaluation& eval = GetEvaluation(eval_goal);
    const Evaluation& father_eval = father.GetEvaluation(-eval_goal);

    return
        IsUnderAnalyzed(father, eval_goal) ? -kLogDerivativeMinusInf : 0
        + eval.LogDerivative(father_eval)
        - 0.0001 * leaf_eval_
        - n_thread_multiplier * NThreadsWorking() * father_eval.ProbLowerCubed();
  }

  double GetValueWinning(const TreeNode& father, int eval_goal) const {
    const Evaluation& eval = GetEvaluation(eval_goal);
    const Evaluation& father_eval = father.GetEvaluation(-eval_goal);
    auto proof_number_using_this = eval.DisproofNumber() / (1 - eval.ProbGreaterEqual());
    auto ratio = GetNVisited() / father_eval.ProofNumber();
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
    return
//        IsUnderAnalyzed(father, eval_goal) ? kMaxProofNumber : 0
        proof_number_using_this * multiplier
        - 0.01 * leaf_eval_;
  }

  virtual void UpdateWithChild(const TreeNode& child, bool shallow) {
    auto child_guard = child.ReadLock();
    // We cannot check the upper, because we are currently updating it.
    assert(leaf_eval_ >= EvalToEvalLarge(lower_));
    assert(child.leaf_eval_ >= EvalToEvalLarge(child.lower_) && child.leaf_eval_ <= EvalToEvalLarge(child.upper_));
    assert(min_evaluation_ <= weak_lower_);
    lower_ = MaxEval(lower_, (Eval) -child.Upper());
    upper_ = MaxEval(upper_, (Eval) -child.Lower());
    leaf_eval_ = std::max(leaf_eval_, -child.leaf_eval_);
    assert(leaf_eval_ >= EvalToEvalLarge(lower_));
    // TODO: Make weak_lower_ >= lower_ to avoid extra computations.
    weak_lower_ = MaxEval(weak_lower_, (Eval) -child.weak_upper_);
    weak_upper_ = MinEval(weak_upper_, (Eval) -child.weak_lower_);
    for (int i = weak_lower_; i <= weak_upper_; i += 2) {
      Evaluation* father_eval = MutableEvaluation(i);
      assert(-i >= child.weak_lower_ && -i <= child.weak_upper_);
      const Evaluation& eval = child.GetEvaluation(-i);
      if (shallow) {
        father_eval->UpdateFatherWithThisChild<true>(eval);
      } else {
        father_eval->UpdateFatherWithThisChild<false>(eval);
      }
    }
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

  void SetChildrenNoLock(std::vector<TreeNode*> children) {
    assert(n_children_ == 0 || n_children_ == 255);
    n_children_ = (Square) children.size();
    children_ = new TreeNode*[n_children_];
    is_leaf_ = false;
    for (int i = 0; i < n_children_; ++i) {
      TreeNode* child = children[i];
      assert((child->weak_lower_ - kMinEval) % 2 == 1);
      assert((child->weak_upper_ - kMinEval) % 2 == 1);
      child->AddFather(this);
      children_[i] = child;
    }
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
  static std::optional<LeafToUpdate> BestDescendant(Node* node, float n_thread_multiplier, int last_eval_goal) {
    auto leaf = node->template AsLeaf<Node>(last_eval_goal);
    if (leaf == std::nullopt) {
      return std::nullopt;
    }
    return BestDescendant(*leaf, n_thread_multiplier);
  }

  static LeafToUpdate Leaf(std::vector<Node*> sequence) {
    LeafToUpdate leaf = sequence[0]->template AsLeafWithGoal<Node>(1);
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
};

typedef LeafToUpdate<TreeNode> TreeNodeLeafToUpdate;
#endif  // OTHELLOSENSEI_EVALUATEDERIVATIVE_TREE_NODE_H