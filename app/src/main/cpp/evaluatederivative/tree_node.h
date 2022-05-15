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
#include <float.h>
#include <iomanip>
#include <iostream>
#include <mutex>
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
constexpr int kEmptiesForForcedMidgame = 14;

class ChildError: public std::exception {
 public:
  ChildError() : ChildError("Generic child error") {}
  ChildError(std::string msg) : msg_(msg) {}
 private:
  std::string msg_;
};

class TreeNode;

struct LeafToUpdate {
  LeafToUpdate() :
      leaf(nullptr),
      eval_goal(kLessThenMinEval),
      alpha(kMinEval),
      beta(kMaxEval),
      weak_lower(kMinEval),
      weak_upper(kMaxEval) {}

  TreeNode* leaf;
  std::vector<TreeNode*> parents;
  Eval eval_goal;
  Eval alpha;
  Eval beta;
  Eval weak_lower;
  Eval weak_upper;
};

class TreeNode {
 public:
  TreeNode() : lower_(kMinEval), upper_(kMaxEval), weak_lower_(-63), weak_upper_(63), leaf_eval_(kLessThenMinEval) {}
  TreeNode(const TreeNode& copy) :
      player_(copy.player_),
      opponent_(copy.opponent_),
      children_(copy.children_),
      fathers_(copy.fathers_),
      n_empties_(copy.n_empties_),
      depth_(copy.depth_),
      lower_(copy.lower_),
      upper_(copy.upper_),
      weak_lower_(copy.weak_lower_),
      weak_upper_(copy.weak_upper_),
      min_evaluation_(copy.min_evaluation_),
      n_threads_working_(copy.n_threads_working_) {
    evaluations_ = copy.evaluations_;
  }

  Square NEmpties() {
    std::lock_guard<std::mutex> guard(mutex_);
    return n_empties_;
  }

  Square Depth() {
    std::lock_guard<std::mutex> guard(mutex_);
    return depth_;
  }

  Board ToBoard() {
    std::lock_guard<std::mutex> guard(mutex_);
    return Board(player_, opponent_);
  }

  BitPattern Player() const {
    std::lock_guard<std::mutex> guard(mutex_);
    return player_;
  }

  BitPattern Opponent() const {
    std::lock_guard<std::mutex> guard(mutex_);
    return opponent_;
  }

  Eval Lower() const {
    std::lock_guard<std::mutex> guard(mutex_);
    return lower_;
  }

  Eval Upper() const {
    std::lock_guard<std::mutex> guard(mutex_);
    return upper_;
  }

  Eval WeakLower() const {
    std::lock_guard<std::mutex> guard(mutex_);
    return weak_lower_;
  }

  Eval WeakUpper() const {
    std::lock_guard<std::mutex> guard(mutex_);
    return weak_upper_;
  }

  int ProofNumber(Eval eval_goal) const {
    std::lock_guard<std::mutex> guard(mutex_);
    return GetEvaluation(eval_goal, guard).ProofNumber();
  }

  int DisproofNumber(Eval eval_goal) const {
    std::lock_guard<std::mutex> guard(mutex_);
    return GetEvaluation(eval_goal, guard).DisproofNumber();
  }

  float ProbGreaterEqual(Eval eval_goal) const {
    std::lock_guard<std::mutex> guard(mutex_);
    return GetEvaluation(eval_goal, guard).ProbGreaterEqual();
  }

  int MaxLogDerivative(Eval eval_goal) const {
    std::lock_guard<std::mutex> guard(mutex_);
    return GetEvaluation(eval_goal, guard).MaxLogDerivative();
  }

  void IncreaseNThreadsWorking() {
    std::lock_guard<std::mutex> guard(mutex_);
    n_threads_working_++;
  }

  void DecreaseNThreadsWorking() {
    std::lock_guard<std::mutex> guard(mutex_);
    n_threads_working_--;
  }

  int NThreadsWorking() const {
    std::lock_guard<std::mutex> guard(mutex_);
    return n_threads_working_;
  }

  bool IsValid() const {
    std::lock_guard<std::mutex> guard(mutex_);
    return leaf_eval_ >= kMinEvalLarge && leaf_eval_ <= kMaxEvalLarge;
  }

  float GetEval() const {
    std::lock_guard<std::mutex> guard(mutex_);
    float eval = weak_lower_ - 1;
    for (int i = weak_lower_; i <= weak_upper_; i += 2) {
      float prob = GetEvaluation(i, guard).ProbGreaterEqual();
      if (prob >= 1 - kProbIncreaseWeakEval) {
        prob = 1;
      } else if (prob <= kProbIncreaseWeakEval) {
        prob = 0;
      }
      eval += 2 * prob;
    }
    return eval;
  }

  void UpdateFatherWithThisChild(TreeNode* father, const std::lock_guard<std::mutex>& father_guard) const {
    std::lock_guard<std::mutex> guard(mutex_);
    assert(std::find_if(fathers_.begin(), fathers_.end(), [father](TreeNode* f) { return father == f; }) != fathers_.end());
//    if (Constants.ASSERT_LOCKS) {
//      assert(Thread.holdsLock(father));
//    }
    father->lower_ = std::max(father->lower_, (Eval) -upper_);
    father->upper_ = std::max(father->upper_, (Eval) -lower_);
    father->weak_lower_ = std::max(father->weak_lower_, (Eval) -weak_upper_);
    assert(father->min_evaluation_ <= father->weak_lower_);
    father->weak_upper_ = std::min(father->weak_upper_, (Eval) -weak_lower_);
    assert(AllValidEvals(guard));
    for (int i = father->weak_lower_; i <= father->weak_upper_; i += 2) {
      const Evaluation& eval = GetEvaluation(-i, guard);
      assert(eval.IsValid());
      Evaluation* father_eval = father->MutableEvaluation(i, father_guard);
      assert(father_eval->IsValid());
      father_eval->UpdateFatherWithThisChild(eval);
    }
  }

  void UpdateFathers() {
    {
      std::lock_guard<std::mutex> guard(mutex_);
      if (!IsLeaf(guard)) {
        UpdateFather(guard);
      }
    }
    // Use an index to avoid co-modification (if some other thread adds fathers in the meantime).
    for (int i = 0; i < fathers_.size(); ++i) {
      fathers_[i]->UpdateFathers();
    }
  }

  void UpdateFather(const std::lock_guard<std::mutex>& guard) {
    lower_ = kMinEval;
    upper_ = kMinEval;
    assert(!IsLeaf(guard));
    for (int i = weak_lower_; i <= weak_upper_; i += 2) {
      MutableEvaluation(i, guard)->Initialize();
    }
    for (const TreeNode* const child : GetChildren()) {
      child->UpdateFatherWithThisChild(this, guard);
    }
    for (int i = weak_lower_; i <= weak_upper_; i += 2) {
      MutableEvaluation(i, guard)->Finalize();
//      assert Constants.MAX_PARALLEL_TASKS > 1 || eval.isChildLogDerivativeOK();
    }
//    assert isLowerUpperOK();
  }

  std::vector<TreeNode*>* MutableChildren() { return &children_; }
  const std::vector<TreeNode*>& GetChildren() { return children_; }

  bool IsLeaf() const {
    std::lock_guard<std::mutex> guard(mutex_);
    return IsLeaf(guard);
  }

  void SetChildren(std::vector<TreeNode*> children) {
    std::lock_guard<std::mutex> guard(mutex_);
    children_ = children;
    for (TreeNode* child : children) {
      assert(child->IsValid());
      child->depth_ = depth_ + 1;
      child->AddFather(this);
    }
    assert(AreChildrenCorrect(guard));
  }

  void Reset(const Board& b, int depth) {
    Reset(b.GetPlayer(), b.GetOpponent(), depth);
  }

  void Reset(BitPattern player, BitPattern opponent, int depth) {
    std::lock_guard<std::mutex> guard(mutex_);
    player_ = player;
    opponent_ = opponent;
    depth_ = depth;
    fathers_.clear();
    children_.clear();
    n_empties_ = ::NEmpties(player, opponent);
    lower_ = kMinEval;
    upper_ = kMaxEval;
    weak_lower_ = kMinEval + 1;
    weak_upper_ = kMaxEval - 1;
    min_evaluation_ = weak_lower_;
    leaf_eval_ = kLessThenMinEvalLarge;
    n_threads_working_ = 0;
    n_visited_ = 0;
    evaluations_.clear();
  }

  void SetWeakLowerUpper(Eval weak_lower, Eval weak_upper) {
    std::lock_guard<std::mutex> guard(mutex_);
    assert(IsLeaf(guard));
    assert(weak_lower >= kMinEval && weak_lower <= kMaxEval && (weak_lower - kMinEval) % 2 == 1);
    assert(weak_upper >= kMinEval && weak_upper <= kMaxEval && (weak_upper - kMinEval) % 2 == 1);
    assert(weak_lower <= weak_upper);
    assert(weak_lower >= weak_lower_);
    assert(weak_upper <= weak_upper_);
    weak_lower_ = weak_lower;
    weak_upper_ = weak_upper;
  }

  void SetLeaf(EvalLarge leaf_eval, Square depth, NVisited n_visited, Eval eval_goal) {
    std::lock_guard<std::mutex> guard(mutex_);
    assert(IsLeaf(guard));
    assert(kMinEvalLarge <= leaf_eval && leaf_eval <= kMaxEvalLarge);
    assert(n_visited > 0);
    assert(eval_goal >= weak_lower_ && eval_goal <= weak_upper_);
    leaf_eval_ = leaf_eval;
    min_evaluation_ = weak_lower_;
    EnlargeEvaluations(guard);
    for (int i = weak_lower_; i <= weak_upper_; i += 2) {
      UpdateLeafEvaluation(i, depth, guard);
    }
    AddDescendants(n_visited);
    assert(min_evaluation_ <= weak_lower_);
    assert(AllValidEvals(guard));
  }

  void SetSolved(int eval) {
    SetUpper(eval);
    SetLower(eval);
    // updateFathers();
  }

  void SetLower(int eval) {
    std::lock_guard<std::mutex> guard(mutex_);
    assert(eval % 2 == 0);
    assert(IsLeaf(guard));
    leaf_eval_ = (Eval) std::max(leaf_eval_, eval);
    lower_ = std::max(lower_, eval);
    for (int i = weak_lower_; i <= std::min(eval, weak_upper_); i += 2) {
      MutableEvaluation(i, guard)->SetProved();
    }
    // assert isLowerUpperOK();
  }

  void SetUpper(int eval) {
    std::lock_guard<std::mutex> guard(mutex_);
    assert(eval % 2 == 0);
    assert(IsLeaf(guard));
    leaf_eval_ = (Eval) std::min(leaf_eval_, eval);
    upper_ = std::min(upper_, eval);
    for (int i = weak_upper_; i >= std::max(eval, weak_lower_); i -= 2) {
      MutableEvaluation(i, guard)->SetDisproved();
    }
    // assert isLowerUpperOK();
  }

  const Evaluation& GetEvaluation(int eval_goal) const {
    std::lock_guard<std::mutex> guard(mutex_);
    return GetEvaluation(eval_goal, guard);
  }

  TreeNode* BestChild(bool proof, Eval eval_goal) {
    assert(!IsLeaf());
    assert(eval_goal >= weak_lower_ && eval_goal <= weak_upper_);
    TreeNode* best_child = nullptr;
    double best_child_value = -DBL_MAX;
    Evaluation cur_evaluation = GetEvaluation(eval_goal);
    for (TreeNode* child : children_) {
//      synchronized(childBoard)
//      {
        Evaluation child_eval = child->GetEvaluation(-eval_goal);
        double child_value = child_eval.GetValue(cur_evaluation, proof);
        if (child_value > best_child_value) {
          best_child = child;
          best_child_value = child_value;
        }
//      }
    }
    if (best_child != nullptr && best_child->IsLeaf() && best_child->NThreadsWorking() > 0) {
      return nullptr;
    }
    return best_child;
  }

  NVisited GetNVisited() {
    std::lock_guard<std::mutex> guard(mutex_);
    return n_visited_;
  }

  bool IsSolved() {
    return lower_ == upper_;
  }

  bool IsPartiallySolved() {
    std::lock_guard<std::mutex> guard(mutex_);
    return GetPercentileLower(kZeroPercForWeak, guard) == GetPercentileUpper(kZeroPercForWeak, guard);
  }

  Eval GetPercentileUpper(float p) {
    std::lock_guard<std::mutex> guard(mutex_);
    assert(p >= kProbIncreaseWeakEval && p <= 1 - kProbIncreaseWeakEval);
    return GetPercentileUpper(p, guard);
  }

  Eval GetPercentileLower(float p) {
    std::lock_guard<std::mutex> guard(mutex_);
    assert(p >= kProbIncreaseWeakEval && p <= 1 - kProbIncreaseWeakEval);
    return GetPercentileLower(p, guard);
  }

  LeafToUpdate BestDescendant() {
    int eval_goal = NextPositionEvalGoal();
    if (eval_goal == kLessThenMinEval) {
      return LeafToUpdate();
    }
    assert(eval_goal >= weak_lower_ && eval_goal <= weak_upper_);
    const Evaluation& first_eval = GetEvaluation(eval_goal);
    bool proof = first_eval.ProbGreaterEqual() > 1 - kProbForProof || first_eval.ProbGreaterEqual() < kProbForProof;
    std::vector<TreeNode*> parents;
    LeafToUpdate result;
    result.eval_goal = eval_goal;
    result.alpha = std::min(eval_goal, GetPercentileLower(kProbForEndgameAlphaBeta) + 1);
    result.beta = std::max(eval_goal, GetPercentileUpper(kProbForEndgameAlphaBeta) - 1);
    result.weak_lower = weak_lower_;
    result.weak_upper = weak_upper_;
    BestDescendant(proof, &result);
    return result;
  }

  bool ToBeSolved(Eval eval_goal, int tree_nodes) {
    assert(eval_goal >= weak_lower_ && eval_goal <= weak_upper_);
    std::lock_guard<std::mutex> guard(mutex_);
    Evaluation eval = GetEvaluation(eval_goal, guard);
    if (n_empties_ <= kEmptiesForForcedMidgame - 1) {
      return true;
    }
    if (tree_nodes < 20000 || n_empties_ >= kEmptiesForForcedMidgame + 4) {
      return false;
    }
    if (tree_nodes > 0.8 * kDerivativeEvaluatorSize && n_empties_ <= kEmptiesForForcedMidgame + 4) {
      return true;
    }
    assert(IsLeaf(guard));
    assert(eval.ProofNumber() > 0 && eval.DisproofNumber() > 0);
    assert(n_empties_ > 0 && n_empties_ <= 60);
//    return eval.ProofNumber() < 20000 || eval.DisproofNumber() < 20000;
    if (eval.ProbGreaterEqual() >= 1 - 0.2) {
      return eval.ProofNumber() < 10000; // proofNumberForAlphaBeta.get();
    } else if (eval.ProbGreaterEqual() <= 0.2) {
      return eval.DisproofNumber() < 10000; // proofNumberForAlphaBeta.get();
    }
    return false;
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
//    synchronized (this) {
      float prob_lower = ProbGreaterEqual(weak_lower_);
      float prob_upper = ProbGreaterEqual(weak_upper_);
      float prob_lower_next = ProbGreaterEqual(weak_lower_ + 2);
      float prob_upper_prev = ProbGreaterEqual(weak_upper_ - 2);
      if (prob_lower < 1 - kProbIncreaseWeakEval && weak_lower_ - 2 >= lower_) {
        extend_lower = true;
        assert(upper_ > lower_ + 2);
      }
      if (prob_upper > kProbIncreaseWeakEval && weak_upper_ + 2 <= upper_) {
        extend_upper = true;
      }
      if (!extend_lower && !extend_upper && prob_lower_next > 1 - kProbReduceWeakEval) {
        weak_lower_ += 2;
        assert(weak_lower_ < weak_upper_);
        reduced = true;
      }
      if (!extend_lower && !extend_upper && prob_upper_prev < kProbReduceWeakEval) {
        weak_upper_ -= 2;
        assert(weak_lower_ < weak_upper_);
        reduced = true;
      }
//    }
    {
      std::lock_guard<std::mutex> guard(mutex_);
      if (extend_lower) {
        ExtendEval(weak_lower_ - 2, guard);
      }
      if (extend_upper) {
        ExtendEval(weak_upper_ + 2, guard);
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

 private:
  BitPattern player_;
  BitPattern opponent_;
  std::vector<TreeNode*> children_;
  std::vector<TreeNode*> fathers_;
  Square n_empties_;
  Square depth_;
  Eval lower_;
  Eval upper_;
  Eval weak_lower_;
  Eval weak_upper_;
  Eval min_evaluation_;
  EvalLarge leaf_eval_;
  std::vector<Evaluation> evaluations_;
  int n_threads_working_;
  mutable std::mutex mutex_;
  NVisited n_visited_;

  void UpdateLeafEvaluation(int i, int depth, const std::lock_guard<std::mutex>& guard) {
    assert(i >= weak_lower_ && i <= weak_upper_);
    Evaluation* evaluation = MutableEvaluation(i, guard);
    if (i < lower_) {
      evaluation->SetProved();
    } else if (i > upper_) {
      evaluation->SetDisproved();
    } else {
      float prob = 1 - (float) GaussianCDF(EvalToEvalLarge(i), leaf_eval_, 8 * kErrors[n_empties_] * kMultStddev);
      float proof_number = ::ProofNumber(player_, opponent_, EvalToEvalLarge(i), leaf_eval_);
      assert(isfinite(proof_number) && proof_number > 0);
      float disproof_number = ::DisproofNumber(player_, opponent_, EvalToEvalLarge(i), leaf_eval_);
      assert(isfinite(disproof_number) && disproof_number > 0);
      evaluation->SetLeaf(prob, proof_number, disproof_number, depth < 12 ? (float) (-0.5F * pow(std::max(0, depth - 4), 2)) : -100000);
    }
  }

//   static AtomicInteger lastEvalGoal = new AtomicInteger(-6500);
//   final static EndgameTimeEstimatorInterface endgameTimeEstimator = new EndgameTimeEstimator();
//   private AtomicBoolean updating;
//
//  public static AtomicInteger avoidNextPosFailCoeff = new AtomicInteger(2000);
//  public static AtomicInteger proofNumberForAlphaBeta = new AtomicInteger(0);

  void AddFather(TreeNode* father) {
    std::lock_guard<std::mutex> guard(mutex_);
    fathers_.push_back(father);
  }

  Eval GetPercentileUpper(float p, const std::lock_guard<std::mutex>& guard) {
    for (int i = weak_lower_; i <= weak_upper_; i += 2) {
      const Evaluation& eval = GetEvaluation(i, guard);
      if (eval.IsValid() && eval.ProbGreaterEqual() <= p) {
        return i - 1;
      }
    }
    return 64;
  }
  Eval GetPercentileLower(float p, const std::lock_guard<std::mutex>& guard) {
    for (int i = weak_upper_; i >= weak_lower_; i -= 2) {
      const Evaluation& eval = GetEvaluation(i, guard);
      if (eval.IsValid() && eval.ProbGreaterEqual() >= 1 - p) {
        return i + 1;
      }
    }
    return -64;
  }

  const Evaluation& GetEvaluation(int eval_goal, const std::lock_guard<std::mutex>& guard) const {
    assert((eval_goal - kMinEval) % 2 == 1);
    assert(eval_goal >= weak_lower_ && eval_goal <= weak_upper_);
    assert(min_evaluation_ <= weak_lower_);
    int index = ToEvaluationIndex(eval_goal);
    assert(index >= 0 && index < evaluations_.size());
    return evaluations_[index];
  }

  Evaluation* MutableEvaluation(Eval eval_goal, const std::lock_guard<std::mutex>& guard) {
    assert((eval_goal - kMinEval) % 2 == 1);
    assert(eval_goal >= weak_lower_ && eval_goal <= weak_upper_);
    assert(min_evaluation_ <= weak_lower_);
    int index = ToEvaluationIndex(eval_goal);
    assert(index >= 0 && index < evaluations_.size());
    return &evaluations_[index];
  }

  void EnlargeEvaluations(const std::lock_guard<std::mutex>& lock_guard) {
    int desired_size = ToEvaluationIndex(weak_upper_) + 1;
    if (evaluations_.size() < desired_size) {
      evaluations_.insert(evaluations_.end(), desired_size - evaluations_.size(), Evaluation());
    }
  }

  int ToEvaluationIndex(Eval eval) const {
    assert((eval - kMinEval) % 2 == 1);
    assert(eval >= kMinEval);
    assert(eval <= kMaxEval);
    return ((int) eval - min_evaluation_) / 2;
  }

  bool AreChildrenCorrect(const std::lock_guard<std::mutex>& guard) {
    std::vector<BitPattern> flips = GetAllMovesWithPass(player_, opponent_);
    if (children_.size() != flips.size()) {
      throw ChildError("Wrong children size");
    }

    for (TreeNode* child : children_) {
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

  bool IsLeaf(const std::lock_guard<std::mutex>& guard) const {
    return children_.empty();
  }

  void BestDescendant(bool proof, LeafToUpdate* result) {
    TreeNode* best_child;
//    Evaluation bestChild;
//    synchronized (StoredBoard.this) {
//      increaseNThreadsWorking();
    result->parents.push_back(this);
    if (IsSolved()
//          || (IsLeaf() && getNThreadsWorking() > 1)
        ) {
      result->leaf = nullptr;
    } else {
      if (IsLeaf()) {
//          avoidNextPosFailCoeff.addAndGet(-1);
//          Stats.addToNSuccessNextPosition(1);
        result->leaf = this;
        return;
      }
      result->alpha = std::max(result->alpha, std::min(GetPercentileLower(kProbForEndgameAlphaBeta) + 1, result->eval_goal));
      result->beta = std::min(result->beta, std::max(GetPercentileUpper(kProbForEndgameAlphaBeta) - 1, result->eval_goal));
      best_child = BestChild(proof, result->eval_goal);
    }
//    }
    if (best_child == nullptr) {
//      avoidNextPosFailCoeff.addAndGet(10);
//      for (Evaluation eval : ancestors) {
//        eval.getStoredBoard().decreaseNThreadsWorking();
//      }
//      Stats.addToNFailNextPosition(1);
      result->leaf = nullptr;
      return;
    }
    auto tmp = result->alpha;
    result->alpha = -result->beta;
    result->beta = -tmp;
    result->eval_goal = -result->eval_goal;
    tmp = result->weak_lower;
    result->weak_lower = -result->weak_upper;
    result->weak_upper = -tmp;
    best_child->BestDescendant(proof, result);
  }

  Eval NextPositionEvalGoal() {
    Eval best_eval = kLessThenMinEval;
    double best_value = -DBL_MAX;
    for (int i = weak_lower_; i <= weak_upper_; i += 2) {
      const Evaluation& eval = GetEvaluation(i);
      if (eval.ProofNumber() == 0 || eval.DisproofNumber() == 0 ||
              (i < weak_upper_ && GetEvaluation(i + 2).ProbGreaterEqual() > 1 - kMinProbEvalGoal) ||
              (i > weak_lower_ && GetEvaluation(i - 2).ProbGreaterEqual() < kMinProbEvalGoal)) {
        continue;
      }
      double cur_value = 0;
      cur_value += (eval.ProbGreaterEqual() > kMinProbEvalGoal && eval.ProbGreaterEqual() < 1 - kMinProbEvalGoal) ?
                      0 : kLogDerivativeMinusInf * 1000.0;
      cur_value += eval.MaxLogDerivative() * 10;
//      cur_value += i == lastEvalGoal ? 0 : 1;

      if (cur_value > best_value) {
        best_value = cur_value;
        best_eval = i;
      }
    }
    // assert best_eval != kLessThenMinEval || Constants.MAX_PARALLEL_TASKS > 1;

//    lastEvalGoal.set(bestEval.evalGoal);
    return best_eval;
  }
//
//  static StoredBoard initialStoredBoard(Board b) {
//    return initialStoredBoard(b.getPlayer(), b.getOpponent(), 0);
//  }
//
//  public static StoredBoard initialStoredBoard(long player, long opponent, short eval) {
//    StoredBoard result = new StoredBoard(player, opponent, (short) 0);
//    result.updating = new AtomicBoolean(false);
//    int stddev = (int) (ERRORS[result.nEmpties] * Constants.MULT_STDDEV);
//    result.setWeakLowerUpper(roundEval(eval - 3 * stddev), roundEval(eval + 3 * stddev));
//    result.setLeaf(eval, (short) 2);
//    result.setLeaf((short) 0, (short) 1);
//    return result;
//  }
//
//  synchronized double stepsUntilEnd() {
//    assert isLowerUpperOK();
//    // Lower means "better".
//    double maxDerivative = Double.NEGATIVE_INFINITY;
//    double stepsUntilEndProof = Double.POSITIVE_INFINITY;
//    double stepsUntilEndDisproof = Double.POSITIVE_INFINITY;
//    for (int eval = weakLower; eval <= weakUpper; eval += 200) {
//      StoredBoard.Evaluation curEval = getEvaluation(eval);
//      if (curEval.getProb() >= 1 - Constants.ZERO_PERC_FOR_WEAK) {
//        stepsUntilEndProof = Math.max(stepsUntilEndProof, curEval.proofNumber());
//      } else if (curEval.getProb() < Constants.ZERO_PERC_FOR_WEAK) {
//        stepsUntilEndDisproof = Math.max(stepsUntilEndDisproof, curEval.disproofNumber());
//      } else {
//        maxDerivative = Math.max(maxDerivative, curEval.maxLogDerivative);
//      }
//    }
//    if (maxDerivative == Double.NEGATIVE_INFINITY) {
//      return stepsUntilEndProof + stepsUntilEndDisproof;
//    }
//    return (-LOG_DERIVATIVE_MINUS_INF + maxDerivative) * 1E20;
//  }
//
//  protected synchronized void setNewLowerUpper(StoredBoard board) {
//    if (Constants.ASSERT_LOCKS) {
//      assert Thread.holdsLock(board);
//    }
//    if (updating.get()) {
//      return;
//    }
//    board.setWeakLowerUpper(
//        (short) (board.depth % 2 == 0 ? weakLower : -weakUpper),
//        (short) (board.depth % 2 == 0 ? weakUpper : -weakLower));
//  }
//
//  public synchronized float getProb(int evalGoal) {
//    if (evalGoal < weakLower) {
//      if (getEvaluation(weakLower).getProb() > 1 - Constants.PROB_INCREASE_WEAK_EVAL) {
//        return 1;
//      } else {
//        return -1;
//      }
//    }
//    if (evalGoal > weakUpper) {
//      if (getEvaluation(weakLower).getProb() < Constants.PROB_INCREASE_WEAK_EVAL) {
//        return 0;
//      } else {
//        return -1;
//      }
//    }
//    return getEvaluation(evalGoal).getProb();
//  }
//
//
//
//
  void ExtendEval(Eval value, const std::lock_guard<std::mutex>& guard) {
//    synchronized (this) {
    if (value < weak_lower_) {
      assert(value == weak_lower_ - 2);
      if (value < min_evaluation_) {
        int shift = (min_evaluation_ - value) / 2;
        int old_size = ToEvaluationIndex(weak_upper_) + 1;
        min_evaluation_ = value;
        EnlargeEvaluations(guard);
        assert(evaluations_.size() >= old_size + shift);
//        if (evaluations_.size() < old_size + shift) {
//          std::cout << old_size << " " << shift << "misty\n";
//        }
        memmove(evaluations_.data() + shift,
                evaluations_.data(),
                sizeof(Evaluation) * old_size);
      }
      weak_lower_ = value;
    } else if (value > weak_upper_) {
      assert(value == weak_upper_ + 2);
      weak_upper_ = value;
      EnlargeEvaluations(guard);
    }
      if (IsLeaf(guard)) {
//        assert(IsValid());
        UpdateLeafEvaluation(value, 4, guard);
//        assert isLowerUpperOK();
        return;
      }
//    }
    for (TreeNode* child : *MutableChildren()) {
      child->ExtendEval(-value, guard);
    }
//    synchronized (this) {
    UpdateFather(guard);
    assert(AllValidEvals(guard));
//      assert isLowerUpperOK();
//    }
  }

  bool AllValidEvals(const std::lock_guard<std::mutex>& guard) const {
    for (int i = weak_lower_; i <= weak_upper_; i += 2) {
      if (!GetEvaluation(i, guard).IsValid()) {
        return false;
      }
    }
    return true;
  }
//
//
//  synchronized void setWeakLowerUpper(short weakLower, short weakUpper) {
//    this.weakLower = weakLower;
//    this.weakUpper = weakUpper;
//    for (short i = weakLower; i <= weakUpper; i += 200) {
//      addEvaluation(i);
//    }
//  }
//
//
//  private Evaluation addEvaluation(short evalGoal) {
////    assert threadId == Thread.currentThread().getId();
//    int index = toEvaluationIndex(evalGoal);
//    if (evaluations[index] != null) {
//      return null;
//    }
//    Evaluation eval = new Evaluation(evalGoal);
//    evaluations[index] = eval;
//    return eval;
//  }
//
//  private static short roundProb(float prob) {
//    if (prob <= Constants.MIN_PROB_LEAF) {
//      return 0;
//    } else if (prob >= 1 - Constants.MIN_PROB_LEAF) {
//      return PROB_STEP;
//    }
//    return (short) Math.round(prob * PROB_STEP);
//  }
//
//  public int childLogDerivative(StoredBoard child, int evalGoal) {
//    return child.getEvaluation(-evalGoal).logDerivative(getEvaluation(-evalGoal));
//  }
//
//  @NonNull
//  @Override
//  public String toString() {
//    return new Board(player, opponent).toString() + "\n";
//  }
//
//  public float maxFiniteChildrenProofNumber(int evalGoal) {
//    float result = 0;
//    for (StoredBoard child : getChildren()) {
//      if (child.proofNumber(evalGoal) != Float.POSITIVE_INFINITY) {
//        result = Math.max(result, child.proofNumber(evalGoal));
//      }
//    }
//    return result;
//  }
//
//  boolean areDescendantsOK() {
//    if (fathers.isEmpty() || Constants.MAX_PARALLEL_TASKS > 1) {
//      return true;
//    }
//    int maxDescendants = 0;
//    String fatherDesc = "";
//    for (StoredBoard father : fathers) {
//      maxDescendants += father.getDescendants();
//      fatherDesc += " " + father.getDescendants();
//    }
//    if (getDescendants() > maxDescendants) {
//      throw new AssertionError(
//          "Bad number of descendants " + getDescendants() + " > SUM_father descendants = "
//              + maxDescendants + ". Father descendants: " + fatherDesc);
//    }
//    return true;
//  }
//
//  boolean isLowerUpperOK() {
//    assert lower <= upper;
//    assert weakLower < weakUpper;
//    for (int i = weakLower; i <= weakUpper; i += 200) {
//      assert getEvaluation(i) != null;
//    }
//    if (!isLeaf() && Constants.ASSERT_LOWER_UPPER && Constants.MAX_PARALLEL_TASKS == 1) {
//      for (StoredBoard child : getChildren()) {
//        synchronized (child) {
//          assert weakLower >= -child.weakUpper;
//          if (weakUpper > -child.weakLower) {
//            System.out.println(this + " " + weakUpper + " " + (-child.weakLower));
//          }
//          assert weakUpper <= -child.weakLower;
//        }
//      }
//    }
//    return true;
//  }
//
//  boolean isChildOK(StoredBoard child) {
//    if (Constants.MAX_PARALLEL_TASKS > 1) {
//      return true;
//    }
//    boolean found = false;
//    for (StoredBoard father : child.fathers) {
//      if (father == this) {
//        if (found) {
//          throw new AssertionError("Found the same father twice. Father:\n" + this + "\nchild:\n" + child);
//        }
//        found = true;
//      }
//    }
//    if (!found) {
//      throw new AssertionError("Did not find father\n" + child + " for board\n" + this);
//    }
//    return
//        child.depth == depth + 1
//        && Utils.arrayContains(getChildren(), child);
//  }
//
//  boolean areChildrenOK() {
//    if (getChildren() == null) {
//      return true;
//    }
//    for (StoredBoard child : getChildren()) {
//      if (!isChildOK(child)) {
//        throw new AssertionError("Child\n" + child + "\n of board\n" + this + "\nis not OK");
//      }
//    }
//    if (getChildren().length != Long.bitCount(GetMoves.getMoves(player, opponent)) && !(
//        Long.bitCount(GetMoves.getMoves(player, opponent)) == 0
//        && Long.bitCount(GetMoves.getMoves(opponent, player)) != 0
//        && getChildren().length == 1)) {
//      throw new AssertionError(
//          "Board " + this + " should have " + Long.bitCount(GetMoves.getMoves(player, opponent))
//              + " children. Found " + getChildren().length);
//    }
//    return true;
//  }
//
//  boolean isPrevNextOK() {
//    return
//        prev != this
//        && next != this
//        && (prev == null || prev.next == this)
//        && (next == null || next.prev == this);
//  }
//
//  boolean isEvalOK(int evalGoal) {
//    if (Constants.MAX_PARALLEL_TASKS > 1) {
//      return true;
//    }
//    if ((new GetMovesCache()).getNMoves(player, opponent) == 0
//        && (new GetMovesCache()).getNMoves(opponent, player) == 0
//        && (this.proofNumber(evalGoal) == 0 || this.proofNumber(evalGoal) == Float.POSITIVE_INFINITY)) {
//      return true;
//    }
//    return true;
//  }
//
//  synchronized boolean isNThreadsWorkingOK() {
//    if (this.nThreadsWorking < 0) {
//      throw new AssertionError("Got " + this.nThreadsWorking +
//                                   " threads.");
//    }
//    if (this.nThreadsWorking > Constants.MAX_PARALLEL_TASKS) {
//      throw new AssertionError("Got " + this.nThreadsWorking +
//                                   " threads. Expected  at most "
//                                   + Constants.MAX_PARALLEL_TASKS);
//    }
//    if (fathers.size() == 0) {
//      return true;
//    }
//    int nThreadsWorkingFathers = 0;
//    for (StoredBoard father : fathers) {
//      nThreadsWorkingFathers += father.nThreadsWorking;
//    }
//    if (nThreadsWorkingFathers != this.nThreadsWorking && Constants.MAX_PARALLEL_TASKS == 1) {
//      throw new AssertionError("Got " + this.nThreadsWorking +
//                                   " threads. Expected " + nThreadsWorkingFathers);
//    }
//    return true;
//  }
//
//  boolean isAllOK() {
//    if (!isPrevNextOK()) {
//      throw new AssertionError("Wrong isPrevNextOK");
//    }
//    if (!areChildrenOK()) {
//      throw new AssertionError("Wrong areChildrenOK");
//    }
//    for (int goal = -6300; goal <= 6300; goal += 200) {
//      assert isEvalOK(goal);
//    }
//    if (!areDescendantsOK()) {
//      throw new AssertionError("Wrong areThisBoardEvalsOK or isEvalOK or areDescendantsOK.");
//    }
//    if (!isNThreadsWorkingOK()) {
//      throw new AssertionError("Wrong isNThreadsWorking.");
//    }
//    return true;
//  }
};