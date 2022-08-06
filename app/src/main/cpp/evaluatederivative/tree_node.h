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

#include <algorithm>
#include <atomic>
#include <float.h>
#include <functional>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <queue>
#include <string.h>
#include "endgame_time_estimator.h"
#include "evaluation.h"
#include "../board/bitpattern.h"
#include "../board/board.h"
#include "../board/get_moves.h"
#include "../utils/misc.h"

constexpr float kErrors[] = {
      0, 0, 0, 0, 5.30, 5.85, 6.36, 6.96, 7.36, 7.80, 7.94, 8.28, 8.14, 8.21, 8.07, 7.97, 7.72, 7.68, 7.53, 7.32,
      7.09, 6.80, 5.52, 4.74, 4.80, 4.48, 4.61, 3.97, 4.29, 3.80, 4.03, 3.62, 3.72, 3.28, 3.38, 3.00, 3.32, 3.03,
      3.11, 2.90, 3.00, 2.65, 2.67, 2.50, 2.50, 2.50, 2.50, 2.50, 2.50, 2.50, 2.50, 2.50, 2.50, 2.50, 2.50, 2.50,
      2.50, 2.50, 2.50, 2.50, 2.50};
constexpr int kNextPositionSize = 100000;

class ChildError: public std::exception {
 public:
  ChildError() : ChildError("Generic child error") {}
  ChildError(std::string msg) : msg_(msg) {}
 private:
  std::string msg_;
};

class TreeNode;

class LeafToUpdate {
 public:
  LeafToUpdate(TreeNode* leaf, int eval_goal);

  LeafToUpdate ToChild(TreeNode* child, double extra_loss) const;

  Eval EvalGoal() const { return eval_goal_; }
  Eval Alpha() const { return alpha_; }
  Eval Beta() const { return beta_; }
  Eval WeakLower() const { return weak_lower_; }
  Eval WeakUpper() const { return weak_upper_; }
  double Loss() const { return loss_; }
  const std::vector<TreeNode*>& Parents() const { return parents_; }
  TreeNode* Leaf() const { return leaf_; }

 private:
  TreeNode* leaf_;
  std::vector<TreeNode*> parents_;
  Eval eval_goal_;
  Eval alpha_;
  Eval beta_;
  Eval weak_lower_;
  Eval weak_upper_;
  double loss_;
};

class TreeNode {
 public:
  TreeNode() : n_children_(0), n_fathers_(0), leaf_eval_(kLessThenMinEvalLarge), evaluator_(255) {}
  TreeNode(const TreeNode&) = delete;
  ~TreeNode() {
    if (n_fathers_ != 0) {
      free(fathers_);
      n_fathers_ = 0;
    }
    if (n_children_ != 0) {
      delete[] children_;
      n_children_ = 0;
    }
    if (IsValid()) {
      free(evaluations_);
    }
  }

  Square NEmpties() {
    return n_empties_;
  }

  Square Depth() {
    return depth_;
  }

  Board ToBoard() {
    return Board(player_, opponent_);
  }

  BitPattern Player() const {
    return player_;
  }

  BitPattern Opponent() const {
    return opponent_;
  }

  Eval Lower() const {
    return lower_;
  }

  Eval Upper() const {
    return upper_;
  }

  Eval WeakLower() const {
    return weak_lower_;
  }

  Eval WeakUpper() const {
    return weak_upper_;
  }

  float ProofNumber(Eval eval_goal) const {
    assert(eval_goal >= weak_lower_ && eval_goal <= weak_upper_);
    return GetEvaluation(eval_goal).ProofNumber();
  }

  float DisproofNumber(Eval eval_goal) const {
    assert(eval_goal >= weak_lower_ && eval_goal <= weak_upper_);
    return GetEvaluation(eval_goal).DisproofNumber();
  }

  float ProbGreaterEqual(Eval eval_goal) const {
    assert(eval_goal >= weak_lower_ && eval_goal <= weak_upper_);
    return GetEvaluation(eval_goal).ProbGreaterEqual();
  }

  int MaxLogDerivative(Eval eval_goal) const {
    assert(eval_goal >= weak_lower_ && eval_goal <= weak_upper_);
    return GetEvaluation(eval_goal).MaxLogDerivative();
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

  inline bool IsValid() const {
    return leaf_eval_ != kLessThenMinEvalLarge;
  }

  float GetEval() const {
    int lower = std::max(lower_ - 1, (int) weak_lower_);
    int upper = std::min(upper_ + 1, (int) weak_upper_);
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

  bool ContainsFather(TreeNode* father) const {
    for (int i = 0; i < n_fathers_; ++i) {
      if (fathers_[i] == father) {
        return true;
      }
    }
    return false;
  }

  void UpdateFatherWithThisChild(TreeNode* father) const {
    const std::lock_guard<std::mutex> guard(mutex_);
    assert(ContainsFather(father));
    father->lower_ = MaxEval(father->lower_, (Eval) -upper_);
    father->upper_ = MaxEval(father->upper_, (Eval) -lower_);
    father->weak_lower_ = MaxEval(father->weak_lower_, (Eval) -weak_upper_);
    assert(father->min_evaluation_ <= father->weak_lower_);
    father->weak_upper_ = MinEval(father->weak_upper_, (Eval) -weak_lower_);
    assert(AllValidEvals());
    for (int i = father->weak_lower_; i <= father->weak_upper_; i += 2) {
      assert(-i >= weak_lower_ && -i <= weak_upper_);
      const Evaluation& eval = GetEvaluation(-i);
      assert(eval.IsValid());
      Evaluation* father_eval = father->MutableEvaluation(i);
      assert(father_eval->IsValid());
      father_eval->UpdateFatherWithThisChild(eval);
    }
  }

  void UpdateFathers() {
    if (NThreadsWorking() > 0) {
      return;
    }
    // Use an index to avoid co-modification (if some other thread adds fathers in the meantime).
    for (int i = 0; i < n_fathers_; ++i) {
      TreeNode* father = fathers_[i];
      assert(!father->IsLeaf());
      if (father->NThreadsWorking() > 0) {
        continue;
      }
      father->UpdateFather();
      father->UpdateFathers();
    }
  }

  void UpdateFather() {
    const std::lock_guard<std::mutex> guard(mutex_);
    lower_ = kMinEval;
    upper_ = kMinEval;
    assert(!IsLeaf());
    for (int i = weak_lower_; i <= weak_upper_; i += 2) {
      MutableEvaluation(i)->Initialize();
    }
    for (int i = 0; i < n_children_; ++i) {
      children_[i]->UpdateFatherWithThisChild(this);
    }
    for (int i = weak_lower_; i <= weak_upper_; i += 2) {
      MutableEvaluation(i)->Finalize();
//      assert Constants.MAX_PARALLEL_TASKS > 1 || eval.isChildLogDerivativeOK();
    }
//    assert isLowerUpperOK();
  }

//   std::vector<TreeNode*>* MutableChildren() { return &children_; }
  std::vector<TreeNode*> GetChildren() {
    std::vector<TreeNode*> children;
    for (int i = 0; i < n_children_; ++i) {
      children.push_back(children_[i]);
    }
    return children;
  }

  bool IsLeaf() const {
    return n_children_ == 0;
  }

  void SetChildren(std::vector<TreeNode*> children) {
    const std::lock_guard<std::mutex> guard(mutex_);
    n_children_ = (Square) children.size();
    children_ = new TreeNode*[n_children_];
    for (int i = 0; i < n_children_; ++i) {
      TreeNode* child = children[i];
      assert(child->IsValid());
      child->depth_ = depth_ + 1;
      child->AddFather(this);
      children_[i] = child;
    }
    assert(AreChildrenCorrect());
  }

  void Reset(const Board& b, int depth, u_int8_t evaluator) {
    Reset(b.GetPlayer(), b.GetOpponent(), depth, evaluator);
  }

  void Reset(BitPattern player, BitPattern opponent, int depth, u_int8_t evaluator) {
    evaluator_ = evaluator;
    if (n_fathers_ != 0) {
      free(fathers_);
      n_fathers_ = 0;
    }
    if (n_children_ != 0) {
      delete[] children_;
      n_children_ = 0;
    }
    if (IsValid()) {
      free(evaluations_);
    }
    player_ = player;
    opponent_ = opponent;
    depth_ = depth;
    n_empties_ = ::NEmpties(player, opponent);
    lower_ = kMinEval;
    upper_ = kMaxEval;
    weak_lower_ = kMinEval + 1;
    weak_upper_ = kMaxEval - 1;
    min_evaluation_ = weak_lower_;
    leaf_eval_ = kLessThenMinEvalLarge;
    n_threads_working_ = 0;
    n_visited_ = 0;
  }

  void SetWeakLowerUpper(Eval weak_lower, Eval weak_upper) {
    const std::lock_guard<std::mutex> guard(mutex_);
    assert(IsLeaf());
    assert(weak_lower >= kMinEval && weak_lower <= kMaxEval && (weak_lower - kMinEval) % 2 == 1);
    assert(weak_upper >= kMinEval && weak_upper <= kMaxEval && (weak_upper - kMinEval) % 2 == 1);
    assert(weak_lower <= weak_upper);
    assert(weak_lower >= weak_lower_);
    assert(weak_upper <= weak_upper_);
    weak_lower_ = weak_lower;
    weak_upper_ = weak_upper;
  }

  void SetLeaf(EvalLarge leaf_eval, Square depth, NVisited n_visited, bool endgame = false) {
    const std::lock_guard<std::mutex> guard(mutex_);
    assert(IsLeaf());
    assert(kMinEvalLarge <= leaf_eval && leaf_eval <= kMaxEvalLarge);
    assert(n_visited > 0);
    EnlargeEvaluations();
    leaf_eval_ = leaf_eval;
    min_evaluation_ = weak_lower_;
    for (int i = weak_lower_; i <= weak_upper_; i += 2) {
      UpdateLeafEvaluation(i, endgame);
    }
    AddDescendants(n_visited);
    assert(min_evaluation_ <= weak_lower_);
    assert(AllValidEvals());
  }

  void SetSolved(EvalLarge eval) {
    SetUpper(eval);
    SetLower(eval);
  }

  void SetLower(EvalLarge eval) {
    const std::lock_guard<std::mutex> guard(mutex_);
    assert(eval % 2 == 0);
    assert(IsLeaf());
    leaf_eval_ = std::max(leaf_eval_, eval);
    Eval eval_small = EvalLargeToEvalRound(eval);
    lower_ = MaxEval(lower_, eval_small);
    for (int i = weak_lower_; i <= weak_upper_; i += 2) {
      UpdateLeafEvaluation(i, false);
    }
  }

  void SetUpper(EvalLarge eval) {
    const std::lock_guard<std::mutex> guard(mutex_);
    assert(eval % 2 == 0);
    assert(IsLeaf());
    leaf_eval_ = std::min(leaf_eval_, eval);
    Eval eval_small = EvalLargeToEvalRound(eval);
    upper_ = MinEval(upper_, eval_small);
    for (int i = weak_lower_; i <= weak_upper_; i += 2) {
      UpdateLeafEvaluation(i, false);
    }
  }

  const Evaluation& GetEvaluation(int eval_goal) const {
    assert((eval_goal - kMinEval) % 2 == 1);
    assert(eval_goal >= weak_lower_ && eval_goal <= weak_upper_);
    assert(min_evaluation_ <= weak_lower_);
    int index = ToEvaluationIndex(eval_goal);
    assert(index >= 0 && index <= (weak_upper_ - min_evaluation_) / 2);
    return evaluations_[index];
  }

  NVisited GetNVisited() {
    return n_visited_;
  }

  bool IsSolved() {
    return lower_ == upper_;
  }

  bool IsSolvedAtProb(float prob) {
    for (int i = weak_lower_; i <= weak_upper_ - 2; i += 2) {
      if (GetEvaluation(i).ProbGreaterEqual() >= 1 - prob &&
          GetEvaluation(i + 2).ProbGreaterEqual() <= prob) {
        return true;
      }
    }
    return false;
  }

  bool IsPartiallySolved() {
    return IsSolvedAtProb(kZeroPercForWeak);
  }

  Eval GetPercentileUpper(float p) {
    assert(p >= kProbIncreaseWeakEval && p <= 1 - kProbIncreaseWeakEval);
    for (int i = weak_lower_; i <= weak_upper_; i += 2) {
      const Evaluation& eval = GetEvaluation(i);
      if (eval.IsValid() && eval.ProbGreaterEqual() <= p) {
        return i - 1;
      }
    }
    return 64;
  }
  Eval GetPercentileLower(float p) {
    assert(p >= kProbIncreaseWeakEval && p <= 1 - kProbIncreaseWeakEval);
    for (int i = weak_upper_; i >= weak_lower_; i -= 2) {
      const Evaluation& eval = GetEvaluation(i);
      if (eval.IsValid() && eval.ProbGreaterEqual() >= 1 - p) {
        return i + 1;
      }
    }
    return -64;
  }

  std::vector<LeafToUpdate> BestDescendants(bool proving, int n_threads_working) {
    std::vector<LeafToUpdate> result;

    int eval_goal = NextPositionEvalGoal(0, 1);
    double best_loss = kLogDerivativeMinusInf * 10000.0;
    if (n_threads_working == 1) {
      BestDescendant(LeafToUpdate(this, eval_goal), proving, &result);
    } else {
      BestDescendants(LeafToUpdate(this, eval_goal), proving, best_loss, &result);
    }
    return result;
  }

  bool ToBeSolved(Eval alpha, Eval beta, NVisited max_proof) {
    assert(alpha <= beta);
    assert(alpha >= weak_lower_ && beta <= weak_upper_);
    assert(IsLeaf());
    assert(n_empties_ > 0 && n_empties_ <= 60);

    for (Eval i = alpha; i <= beta; i += 2) {
      assert(i >= weak_lower_ && i <= weak_upper_);
      Evaluation eval = GetEvaluation(i);
      if (eval.RemainingWork() > max_proof) {
        return false;
      }
    }
    return true;
  }

  void AddDescendants(NVisited n) {
    n_visited_ += n;
  }

  bool UpdateWeakLowerUpper() {
//    if (updating.getAndSet(true)) {
//      return true;
//    }
    bool extend_lower = false;
    bool extend_upper = false;
    bool reduced = false;
    float prob_lower = ProbGreaterEqual(weak_lower_);
    float prob_upper = ProbGreaterEqual(weak_upper_);
    float prob_lower_next = weak_lower_ == weak_upper_ ? 0 : ProbGreaterEqual(weak_lower_ + 2);
    float prob_upper_prev = weak_lower_ == weak_upper_ ? 1 : ProbGreaterEqual(weak_upper_ - 2);
    if (prob_lower < 1 - kProbIncreaseWeakEval && weak_lower_ - 2 >= lower_) {
      extend_lower = true;
      assert(upper_ > lower_ + 2);
    }
    if (prob_upper > kProbIncreaseWeakEval && weak_upper_ + 2 <= upper_) {
      extend_upper = true;
    }
    if (!extend_lower && !extend_upper && prob_lower_next > 1 - kProbReduceWeakEval) {
      weak_lower_ += 2;
      assert(weak_lower_ <= weak_upper_);
      reduced = true;
    }
    if (!extend_lower && !extend_upper && prob_upper_prev < kProbReduceWeakEval) {
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
//    assert isLowerUpperOK();
//    updating.set(false);
//    if (reduced || extend_lower || extend_upper) {
//      std::cout << GetNVisited() << ": " << weak_lower_ << " " << weak_upper_ << "\n";
//    }
//    else {
//      std::cout << "FALSE\n";
//    }
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
  EvalLarge leaf_eval_;

 private:
  BitPattern player_;
  BitPattern opponent_;
  std::atomic_uint64_t n_visited_;
  TreeNode** children_;
  TreeNode** fathers_;
  Evaluation* evaluations_;
  std::atomic_uint8_t n_threads_working_;
  Square n_children_;
  Square n_fathers_;
  Square n_empties_;
  Square depth_;
  Eval lower_;
  Eval upper_;
  Eval weak_lower_;
  Eval weak_upper_;
  Eval min_evaluation_;
  u_int8_t evaluator_;
  mutable std::mutex mutex_;

  void UpdateLeafEvaluation(int i, bool to_be_solved) {
    assert(i >= weak_lower_ && i <= weak_upper_);
    Evaluation* evaluation = MutableEvaluation(i);
    if (i < lower_ || (n_threads_working_ > 0 && EvalLargeToEvalRound(leaf_eval_) >= i && to_be_solved)) {
      evaluation->SetProved();
    } else if (i > upper_ || (n_threads_working_ > 0 && EvalLargeToEvalRound(leaf_eval_) < i && to_be_solved)) {
      evaluation->SetDisproved();
    } else {
      float prob = 1 - (float) GaussianCDF(EvalToEvalLarge(i), leaf_eval_, 8 * kErrors[n_empties_] * 0.85);
      float proof_number = ::ProofNumber(player_, opponent_, EvalToEvalLarge(i), leaf_eval_);
      assert(isfinite(proof_number) && proof_number > 0);
      float disproof_number = ::DisproofNumber(player_, opponent_, EvalToEvalLarge(i), leaf_eval_);
      assert(isfinite(disproof_number) && disproof_number > 0);
      evaluation->SetLeaf(prob, proof_number, disproof_number);
    }
  }

  void AddFather(TreeNode* father) {
    const std::lock_guard<std::mutex> guard(mutex_);
    if (n_fathers_ == 0) {
      fathers_ = (TreeNode**) malloc(sizeof(TreeNode*));
    } else {
      fathers_ = (TreeNode**) realloc(fathers_, (n_fathers_ + 1) * sizeof(TreeNode*));
    }
    fathers_[n_fathers_] = father;
    n_fathers_++;
  }

  Evaluation* MutableEvaluation(Eval eval_goal) {
    assert((eval_goal - kMinEval) % 2 == 1);
    assert(eval_goal >= weak_lower_ && eval_goal <= weak_upper_);
    assert(min_evaluation_ <= weak_lower_);
    int index = ToEvaluationIndex(eval_goal);
    assert(index >= 0 && index <= (weak_upper_ - min_evaluation_) / 2);
    return &evaluations_[index];
  }

  void EnlargeEvaluations() {
    int desired_size = ToEvaluationIndex(weak_upper_) + 1;
    if (IsValid()) {
      evaluations_ = (Evaluation*) realloc(evaluations_, desired_size * sizeof(Evaluation));
    } else {
      evaluations_ = (Evaluation*) malloc(desired_size * sizeof(Evaluation));
    }
  }

  int ToEvaluationIndex(Eval eval) const {
    assert((eval - kMinEval) % 2 == 1);
    assert(eval >= kMinEval);
    assert(eval <= kMaxEval);
    return (eval - min_evaluation_) >> 1;
  }

  bool AreChildrenCorrect() {
    std::vector<BitPattern> flips = GetAllMovesWithPass(player_, opponent_);
    if (n_children_ != flips.size()) {
      throw ChildError("Wrong children size");
    }

    for (int i = 0; i < n_children_; ++i) {
      TreeNode* child = children_[i];
      if (child->depth_ != depth_ + 1) {
        throw ChildError("Wrong child depth");
      }
      bool found = false;
      for (int i = 0; i < flips.size(); ++i) {
        BitPattern flip = flips[i];
        if (NewOpponent(flip, player_) == child->opponent_
            && NewPlayer(flip, opponent_) == child->player_) {
          assert(!found);  // If it's true, there is a duplicate in the flips.
          flips[i] = 0;
          found = true;
        }
      }
      if (!found) {
        throw ChildError("Missing child");
      }
    }
    return true;
  }

  TreeNode* BestChild(int eval_goal, bool proving) {
    assert(!IsLeaf());
    double best_child_value = -INFINITY;
    Evaluation father_eval = GetEvaluation(eval_goal);
    TreeNode* best_child;
    for (int i = 0; i < n_children_; ++i) {
      TreeNode* child = children_[i];
      Evaluation child_eval = child->GetEvaluation(-eval_goal);
      if (child_eval.IsSolved()) {
        continue;
      }
      double cur_loss = child_eval.GetValue(father_eval, proving);
      if (cur_loss > best_child_value) {
        best_child = child;
        best_child_value = cur_loss;
      }
    }
    return best_child;
  }

  bool BestDescendant(const LeafToUpdate& node, bool proving, std::vector<LeafToUpdate>* descendants) {
    if (NThreadsWorking() > 0) {
      assert(!descendants->empty());
      return false;
    }
    if (IsLeaf()) {
      IncreaseNThreadsWorking();
      descendants->push_back(node);
      return true;
    }
    TreeNode* child = BestChild(node.EvalGoal(), proving);
    bool found = child->BestDescendant(node.ToChild(child, 0), proving, descendants);
    if (found) {
      IncreaseNThreadsWorking();
    }
    return found;
  }

  void BestDescendants(
      const LeafToUpdate& node, bool proving, int best_loss, std::vector<LeafToUpdate>* next_nodes) {
    assert(!IsSolved());
    assert(!node.Leaf()->GetEvaluation(node.EvalGoal()).IsSolved());
    assert(proving || node.Loss() > kLogDerivativeMinusInf);
    assert(NThreadsWorking() == 0);
    if (IsLeaf()) {
      IncreaseNThreadsWorking();
      next_nodes->push_back(node);
      return;
    }

    Evaluation father_eval = GetEvaluation(node.EvalGoal());
    TreeNode* best_child = BestChild(node.EvalGoal(), proving);
    float best_child_prob = best_child->GetEvaluation(-node.EvalGoal()).ProbGreaterEqual();

    for (int i = 0; i < n_children_; ++i) {
      TreeNode* child = children_[i];
      Evaluation child_eval = child->GetEvaluation(-node.EvalGoal());
      double extra_loss = child_eval.LogDerivative(father_eval) - father_eval.MaxLogDerivative();
      assert(extra_loss <= 0);
      if (child->NThreadsWorking() > 0 ||
          child_eval.IsSolved() ||
          (proving && child != best_child && best_child_prob < 0.98) ||
          (!proving && child != best_child && (node.Loss() + extra_loss < -25000 || ProbGreaterEqual(node.EvalGoal()) > 0.9))) {
        assert(child != best_child || (child->NThreadsWorking() > 0 && !next_nodes->empty()));
        continue;
      }
      LeafToUpdate child_leaf = node.ToChild(child, extra_loss);
      auto old_descendants = next_nodes->size();
      if (proving && (best_child_prob > 0.05 && best_child_prob < 0.95)) {
        child->BestDescendant(child_leaf, proving, next_nodes);
      } else {
        child->BestDescendants(child_leaf, proving, best_loss, next_nodes);
      }
      IncreaseNThreadsWorking(next_nodes->size() - old_descendants);
    }
  }

  Eval NextPositionEvalGoal() {
    return NextPositionEvalGoal(0, 1);
  }

  Eval NextPositionEvalGoal(float prob_min, float prob_max) {
    static Eval last_eval_goal = kLessThenMinEval;
    static int num_last_eval_goal = 0;
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

  void ExtendEval(Eval value) {
//    synchronized (this) {
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
//        assert(IsValid());
        UpdateLeafEvaluation(value, false);
//        assert isLowerUpperOK();
        return;
      }
//    }
    for (int i = 0; i < n_children_; ++i) {
      children_[i]->ExtendEval(-value);
    }
    UpdateFather();
    assert(AllValidEvals());
  }

  bool AllValidEvals() const {
    for (int i = weak_lower_; i <= weak_upper_; i += 2) {
      if (!GetEvaluation(i).IsValid()) {
        return false;
      }
    }
    return true;
  }
};