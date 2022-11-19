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

class TreeNode;

class LeafToUpdate {
 public:
  LeafToUpdate(TreeNode* node, int eval_goal);

  LeafToUpdate Copy() const {
    LeafToUpdate result = LeafToUpdate(leaf_, eval_goal_, alpha_, beta_, weak_lower_, weak_upper_, loss_);
    result.parents_ = {leaf_};
    return result;
  }
  LeafToUpdate CopyToChild(TreeNode* node, double extra_loss) const;

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

  void UpdateAlphaBeta(TreeNode* leaf);

  LeafToUpdate(TreeNode* leaf, Eval eval_goal, Eval alpha, Eval beta, Eval weak_lower, Eval weak_upper, double loss)
    : leaf_(leaf), eval_goal_(eval_goal), alpha_(alpha), beta_(beta), weak_lower_(weak_lower), weak_upper_(weak_upper), loss_(loss) {}
};

bool leaf_less(const LeafToUpdate& left, const LeafToUpdate& right) {
  return left.Loss() < right.Loss();
};

class TreeNode {
 public:
  TreeNode() :
      evaluations_(nullptr),
      leaf_eval_(kLessThenMinEvalLarge),
      n_children_(0),
      n_fathers_(0),
      evaluator_(255) {}
  TreeNode(const TreeNode&) = delete;
  ~TreeNode() {
    Reset(0, 0, 0, 0);
  }

  Square NEmpties() const {
    return n_empties_;
  }

  Square Depth() const {
    return depth_;
  }

  Board ToBoard() const {
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
    int lower = std::max(lower_, weak_lower_);
    int upper = std::min(upper_, weak_upper_);
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

  bool UpdateFatherWithThisChild(TreeNode* father) const {
    const std::lock_guard<std::mutex> guard(mutex_);
    if (!IsValid()) {
      // Another thread is still working on this node or a child. This father
      // will be updated by this other thread.
      return false;
    }
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
    return true;
  }

  void UpdateFathers() {
    if (NThreadsWorking() > 0) {
      return;
    }
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
    const std::lock_guard<std::mutex> guard(mutex_);
    lower_ = kLessThenMinEval;
    upper_ = kLessThenMinEval;
    assert(!IsLeaf());
    for (int i = weak_lower_; i <= weak_upper_; i += 2) {
      MutableEvaluation(i)->Initialize();
    }
    for (int i = 0; i < n_children_; ++i) {
      if (!children_[i]->UpdateFatherWithThisChild(this)) {
        leaf_eval_ = kLessThenMinEvalLarge;
        return false;
      }
    }
    for (int i = weak_lower_; i <= weak_upper_; i += 2) {
      MutableEvaluation(i)->Finalize();
//      if (GetEvaluation(i).ProbGreaterEqual() == 0) {
//        for (int j = 0; j < n_children_; ++j) {
//          assert(children_[j]->ProbGreaterEqual(-i) == 1);
//        }
//      }
    }
    leaf_eval_ = 0;
    return true;
  }

  std::vector<TreeNode*> GetChildren() const {
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
    {
      const std::lock_guard<std::mutex> guard(mutex_);
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

  void Reset(Board b, int depth, u_int8_t evaluator) {
    Reset(b.Player(), b.Opponent(), depth, evaluator);
  }

  void Reset(BitPattern player, BitPattern opponent, int depth, u_int8_t evaluator) {
    std::lock_guard<std::mutex> guard(mutex_);
    evaluator_ = evaluator;
    if (n_fathers_ != 0) {
      free(fathers_);
      n_fathers_ = 0;
    }
    if (n_children_ != 0) {
      delete[] children_;
      n_children_ = 0;
    }
    if (evaluations_ != nullptr) {
      free(evaluations_);
      evaluations_ = nullptr;
    }
    player_ = player;
    opponent_ = opponent;
    depth_ = depth;
    n_empties_ = ::NEmpties(player, opponent);
    lower_ = kMinEval - 1;
    upper_ = kMaxEval + 1;
    weak_lower_ = kLessThenMinEval;
    weak_upper_ = kLessThenMinEval;
    min_evaluation_ = weak_lower_;
    leaf_eval_ = kLessThenMinEvalLarge;
    eval_depth_ = 0;
    n_threads_working_ = 0;
    n_visited_ = 0;
  }

  void SetSolved(Eval weak_lower, Eval weak_upper, EvalLarge lower, EvalLarge upper, NVisited n_visited) {
    assert(IsValid());
    assert(eval_depth_ > 0);
    SetLeaf(weak_lower, weak_upper, lower, upper, leaf_eval_, eval_depth_, n_visited);
  }

  void SetLeaf(Eval weak_lower, Eval weak_upper, EvalLarge leaf_eval, Square depth, NVisited n_visited) {
    SetLeaf(weak_lower, weak_upper, kMinEvalLarge, kMaxEvalLarge, leaf_eval, depth, n_visited);
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

  bool IsSolvedIn(Eval lower, Eval upper) {
    return upper_ <= lower || lower_ >= upper || IsSolved();
  }

  bool IsSolved() {
    return lower_ >= upper_ - 2;
  }

  bool IsSolvedAtProb(float prob, Eval lower, Eval upper) {
    if (weak_lower_ <= lower && lower <= weak_upper_ &&
        GetEvaluation(lower).ProbGreaterEqual() <= prob) {
      return true;
    }
    if (weak_lower_ <= upper && upper <= weak_upper_ &&
        GetEvaluation(upper).ProbGreaterEqual() >= 1 - prob) {
      return true;
    }
    for (int i = weak_lower_; i <= weak_upper_ - 2; i += 2) {
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

  bool BestSingleDescendant(const LeafToUpdate& node, std::vector<LeafToUpdate>* descendants) {
    if (IsLeaf()) {
      IncreaseNThreadsWorking();
      descendants->push_back(node);
      return true;
    }
    assert(!GetEvaluation(node.EvalGoal()).IsSolved());
    TreeNode* child = BestChild(node.EvalGoal());
    if (child == nullptr) {
      return false;
    }
    assert(!child->GetEvaluation(-node.EvalGoal()).IsSolved());
    bool found = child->BestSingleDescendant(node.CopyToChild(child, 0), descendants);
    if (found) {
      IncreaseNThreadsWorking();
    }
    return found;
  }

  std::vector<LeafToUpdate> BestDescendants(int n_threads_working) {
    std::vector<LeafToUpdate> result;

    int eval_goal = NextPositionEvalGoal(0, 1);
    if (n_threads_working == 1) {
      BestDescendant(LeafToUpdate(this, eval_goal), &result);
    } else {
      BestDescendants2(this, &result, eval_goal, n_visited_);
      int secondary_eval_goal = kLessThenMinEval;
      if (ProbGreaterEqual(eval_goal) == 0) {
        secondary_eval_goal = NextPositionEvalGoal(0.5, 1);
      } else if (ProbGreaterEqual(eval_goal) == 1) {
        secondary_eval_goal = NextPositionEvalGoal(0, 0.5);
      }
      if (secondary_eval_goal != kLessThenMinEval) {
        BestDescendants2(this, &result, secondary_eval_goal, n_visited_);
      }
    }
    return result;
  }

  bool ToBeSolved(Eval alpha, Eval beta, NVisited max_proof) {
    assert(alpha <= beta);
    assert(alpha >= weak_lower_ && beta <= weak_upper_);
    assert(IsLeaf());
    assert(n_empties_ >= 0 && n_empties_ <= 60);

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
        && weak_lower_ >= lower + 2) {
      extend_lower = true;
    }
    if (prob_upper > kProbIncreaseWeakEval && weak_upper_ + 2 <= upper_
        && weak_upper_ <= upper - 2) {
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

  int ChildLogDerivative(TreeNode* child, int eval_goal) const {
    assert(Contains(GetChildren(), child));
    Evaluation eval = GetEvaluation(eval_goal);
    Evaluation child_eval = child->GetEvaluation(-eval_goal);
    return child_eval.LogDerivative(eval);
  }

  u_int8_t Evaluator() { return evaluator_; }

 private:
  BitPattern player_;
  BitPattern opponent_;
  std::atomic_uint64_t n_visited_;
  TreeNode** children_;
  TreeNode** fathers_;
  Evaluation* evaluations_;
  std::atomic_uint8_t n_threads_working_;
  EvalLarge leaf_eval_;
  Square n_children_;
  Square n_fathers_;
  Square n_empties_;
  Square depth_;
  Square eval_depth_;
  Eval lower_;
  Eval upper_;
  Eval weak_lower_;
  Eval weak_upper_;
  Eval min_evaluation_;
  u_int8_t evaluator_;
  mutable std::mutex mutex_;

  void SetLeaf(Eval weak_lower, Eval weak_upper, EvalLarge lower, EvalLarge upper,
               EvalLarge leaf_eval, Square depth, NVisited n_visited) {
    bool update_fathers;
    {
      const std::lock_guard<std::mutex> guard(mutex_);
      assert(lower % 16 == 0);
      assert(upper % 16 == 0);
      assert(IsLeaf());
      assert(kMinEvalLarge <= leaf_eval && leaf_eval <= kMaxEvalLarge);
      assert(n_visited > 0);

      SetWeakLowerUpper(weak_lower, weak_upper);

      eval_depth_ = depth;
      leaf_eval_ = std::max(lower, std::min(upper, leaf_eval));
      Eval lower_small = EvalLargeToEvalRound(lower);
      Eval upper_small = EvalLargeToEvalRound(upper);
      lower_ = MaxEval(lower_, lower_small - 1);
      upper_ = MinEval(upper_, upper_small + 1);

      for (int i = weak_lower_; i <= weak_upper_; i += 2) {
        UpdateLeafEvaluation(i);
      }
      AddDescendants(n_visited);
      assert(min_evaluation_ <= weak_lower_);
      assert(AllValidEvals());
      update_fathers = n_fathers_ > 0;
    }
    if (update_fathers) {
      UpdateFathers();
    }
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
    if (i <= lower_) {
      evaluation->SetProved();
    } else if (i >= upper_) {
      evaluation->SetDisproved();
    } else {
      assert(eval_depth_ >= 0 && eval_depth_ <= 4);
      assert(n_empties_ >= 0 && n_empties_ <= 60);
      float prob = 1 - (float) GaussianCDF(EvalToEvalLarge(i), leaf_eval_, 0.95F * 8 * std::max(3.0F, kErrors[eval_depth_][n_empties_]));
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
    depth_ = father->depth_ + 1;
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
    if (evaluations_ != nullptr) {
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
        std::cout << "Wrong depth\n" << std::flush;
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
        std::cout << "Missing child\n" << std::flush;
        throw ChildError("Missing child");
      }
    }
    return true;
  }

  TreeNode* BestChild(int eval_goal) {
    assert(!IsLeaf());
    assert(eval_goal >= weak_lower_ && eval_goal <= weak_upper_);
    const Evaluation& father_eval = GetEvaluation(eval_goal);
    assert(!father_eval.IsSolved());
    double best_child_value = -INFINITY;
    TreeNode* best_child = nullptr;
    for (int i = 0; i < n_children_; ++i) {
      TreeNode* child = children_[i];
      const Evaluation& child_eval = child->GetEvaluation(-eval_goal);
      if (child_eval.IsSolved()) {
        continue;
      }
      assert(father_eval.ProbGreaterEqual() >= 1 - child_eval.ProbGreaterEqual() - 0.001);
      double cur_loss = child_eval.GetValue(father_eval, child->GetNVisited());
      if (cur_loss > best_child_value) {
        best_child = child;
        best_child_value = cur_loss;
      }
    }
    return best_child;
  }

  bool BestDescendant(const LeafToUpdate& node, std::vector<LeafToUpdate>* descendants) {
    if (NThreadsWorking() > 0) {
      assert(!descendants->empty());
      return false;
    }
    if (IsLeaf()) {
      IncreaseNThreadsWorking();
      descendants->push_back(node);
      return true;
    }
    assert(!GetEvaluation(node.EvalGoal()).IsSolved());
    TreeNode* child = BestChild(node.EvalGoal());
    if (child == nullptr) {
      return false;
    }
    bool found = child->BestDescendant(node.CopyToChild(child, 0), descendants);
    if (found) {
      IncreaseNThreadsWorking();
    }
    return found;
  }

  static void BestDescendants2(TreeNode* root, std::vector<LeafToUpdate>*
      result, int eval_goal, int n_visited) {
    assert(!root->IsSolved());
    assert(!root->GetEvaluation(eval_goal).IsSolved());
    std::priority_queue<LeafToUpdate, std::vector<LeafToUpdate>, decltype(&leaf_less)> next_nodes(&leaf_less);
    std::unordered_set<TreeNode*> visited_nodes;

    next_nodes.push(LeafToUpdate(root, eval_goal));
    TreeNode* best_child;

    while (!next_nodes.empty() && result->size() < 200) {
      LeafToUpdate current = next_nodes.top();
      next_nodes.pop();
      while (!current.Leaf()->IsLeaf()) {
        TreeNode* father = current.Leaf();
        // Two different grandfathers could have added father, before computing
        // the children.
        if (visited_nodes.find(father) != visited_nodes.end()) {
          break;
        }
        visited_nodes.insert(father);
        best_child = father->BestChild(current.EvalGoal());
        const Evaluation& father_eval = father->GetEvaluation(current.EvalGoal());
        const Evaluation& best_child_eval = best_child->GetEvaluation(-current.EvalGoal());
        float best_child_prob = best_child_eval.ProbGreaterEqual();
        double best_child_value = best_child_eval.GetValue(father_eval,
                                                           best_child->GetNVisited());
        float prob = father_eval.ProbGreaterEqual();

        for (int i = 0; i < father->n_children_; ++i) {
          TreeNode* child = father->children_[i];
          Evaluation child_eval = child->GetEvaluation(-current.EvalGoal());
          double extra_loss =
              child_eval.GetValue(father_eval, child->GetNVisited()) - best_child_value
              + (child == best_child ? 0 : log(best_child_prob) * kLogDerivativeMultiplier);
          assert(extra_loss <= 0);
          if (child->NThreadsWorking() == 0 &&
              !child_eval.IsSolved() &&
              child != best_child &&
              visited_nodes.find(child) == visited_nodes.end() &&
              prob < 1 &&
              current.Loss() + extra_loss > -30000) {
            LeafToUpdate child_leaf = current.CopyToChild(child, extra_loss);
            next_nodes.push(child_leaf);
          }
        }
        assert(!best_child_eval.IsSolved());
        if (visited_nodes.find(best_child) != visited_nodes.end()) {
          assert(!current.Leaf()->IsLeaf());
          break;
        }
        current = current.CopyToChild(best_child, 0);
      }
      if (current.Leaf()->IsLeaf() &&
          visited_nodes.find(current.Leaf()) == visited_nodes.end() &&
          current.Leaf()->NThreadsWorking() == 0) {
        visited_nodes.insert(current.Leaf());
        for (TreeNode* parent : current.Parents()) {
          parent->IncreaseNThreadsWorking();
        }
        result->push_back(current);
        assert(current.Leaf()->NThreadsWorking() == 1);
      }
    }
  }

  void BestDescendants(
      const LeafToUpdate& node, std::vector<LeafToUpdate>* next_nodes) {
    assert(!IsSolved());
    assert(!node.Leaf()->GetEvaluation(node.EvalGoal()).IsSolved());
    assert(node.Leaf() == this);
    assert(NThreadsWorking() == 0);
    if (IsLeaf()) {
      IncreaseNThreadsWorking();
      next_nodes->push_back(node);
      return;
    }
    const Evaluation& father_eval = GetEvaluation(node.EvalGoal());
    TreeNode* best_child = BestChild(node.EvalGoal());
    const Evaluation& best_child_eval = best_child->GetEvaluation(-node.EvalGoal());
    float best_child_prob = best_child_eval.ProbGreaterEqual();
    double best_child_value = best_child_eval.GetValue(father_eval,
                                                       best_child->GetNVisited());
    float prob = father_eval.ProbGreaterEqual();
    bool proving = prob == 0 || prob == 1;
    for (int i = 0; i < n_children_; ++i) {
      TreeNode* child = children_[i];
      Evaluation child_eval = child->GetEvaluation(-node.EvalGoal());
      double extra_loss =
          child_eval.GetValue(father_eval, child->GetNVisited()) - best_child_value
          + (child == best_child ? 0 : log(best_child_prob) * kLogDerivativeMultiplier)
          ;
      assert(extra_loss <= 0);
      if (child->NThreadsWorking() > 0 ||
          child_eval.IsSolved() ||
          (proving && child != best_child && best_child_prob < 0.98) ||
          (!proving && child != best_child && node.Loss() + extra_loss < -30000)) {
        assert(child != best_child || (child->NThreadsWorking() > 0 && !next_nodes->empty()));
        continue;
      }
      LeafToUpdate child_leaf = node.CopyToChild(child, extra_loss);
      auto old_descendants = next_nodes->size();
      child->BestDescendants(child_leaf, next_nodes);
      IncreaseNThreadsWorking(next_nodes->size() - old_descendants);
    }
  }

  Eval NextPositionEvalGoal(float prob_min, float prob_max) {
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

  bool AllValidEvals() const {
    for (int i = weak_lower_; i <= weak_upper_; i += 2) {
      const Evaluation& eval = GetEvaluation(i);
      if (!eval.IsValid()) {
        return false;
      }
    }
    return true;
  }
};