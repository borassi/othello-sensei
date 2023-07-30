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

#include <limits>
#include "tree_node.h"
#include "evaluator_derivative.h"

std::ostream& operator<<(std::ostream& stream, const TreeNode& b) {
  Board board = b.ToBoard();
  stream << b.Player() << " " << b.Opponent() << ": ["
         << (int) b.Lower() << " " << (int) b.Upper() << "]";
  for (int i = b.WeakLower(); i <= b.WeakUpper(); i += 2) {
    Evaluation eval = b.GetEvaluation(i);
    float p = eval.ProbGreaterEqual();
    stream << " " << i << ":";
    if (p == 0) {
      assert(eval.MaxLogDerivative() == kLogDerivativeMinusInf);
      stream << std::setprecision(0) << "D " << eval.DisproofNumber();
    } else if (p == 1) {
      assert(eval.MaxLogDerivative() == kLogDerivativeMinusInf);
      stream << std::setprecision(0) << "P " << eval.ProofNumber();
    } else {
      stream
          << "(" << std::setprecision(4) << p << " "
          << std::setprecision(0) << eval.MaxLogDerivative() << " "
          << eval.ProofNumber() << " " << eval.DisproofNumber() << ")";
    }
  }
  stream << (b.IsLeaf() ? " (leaf)" : " (internal)");
  if (b.Fathers().empty()) {
    stream << " (no fathers)";
  } else {
    stream << " (fathers: ";
    for (const auto& father : b.Fathers()) {
      stream << father;
    }
    stream << ")";
  }
  stream << " (vis: " << b.GetNVisited() << ")";
  return stream;
}

void TreeNode::SetSolved(EvalLarge lower, EvalLarge upper, const EvaluatorDerivative& evaluator) {
  auto guard = WriteLock();
  assert(depth_ > 0);  // Otherwise, the first position might lock the evaluator and viceversa.
  assert(lower % 16 == 0);
  assert(upper % 16 == 0);
  assert(IsLeaf());
  assert(kMinEvalLarge <= leaf_eval_ && leaf_eval_ <= kMaxEvalLarge);
  auto [weak_lower, weak_upper] = evaluator.GetWeakLowerUpper(depth_);

  Eval lower_small = EvalLargeToEvalRound(lower);
  Eval upper_small = EvalLargeToEvalRound(upper);
  lower_ = MaxEval(lower_, lower_small);
  upper_ = MinEval(upper_, upper_small);
  assert(lower_ <= 64);
  assert(upper_ >= -64);
  UpdateLeafEvalNoLock();
  SetLeafNoLock(leaf_eval_, eval_depth_, weak_lower, weak_upper);
  for (int i = weak_lower_; i <= weak_upper_; i += 2) {
    assert(GetEvaluation(i).IsSolved() == (i < lower_ || i > upper_));
  }
}

void TreeNode::Reset(
    BitPattern player, BitPattern opponent, int depth, EvalLarge leaf_eval,
    Square eval_depth, EvaluatorDerivative* evaluator) {
  auto guard = WriteLock();
  auto [weak_lower, weak_upper] = evaluator->GetWeakLowerUpper(depth);
  ResetNoLock(player, opponent, depth, evaluator->Index(), leaf_eval, eval_depth, weak_lower, weak_upper);
}

void TreeNode::Reset(BitPattern player, BitPattern opponent, int depth,
                     u_int8_t evaluator, EvalLarge leaf_eval, Square eval_depth,
                     Eval weak_lower, Eval weak_upper) {
  auto guard = WriteLock();
  ResetNoLock(player, opponent, depth, evaluator, leaf_eval, eval_depth, weak_lower, weak_upper);
}

void TreeNode::ResetNoLock(
    BitPattern player, BitPattern opponent, int depth,
    u_int8_t evaluator, EvalLarge leaf_eval, Square eval_depth,
    Eval weak_lower, Eval weak_upper) {
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
  min_evaluation_ = kLessThenMinEval;

  n_threads_working_ = 0;
  descendants_ = 0;
  SetLeafNoLock(leaf_eval, eval_depth, weak_lower, weak_upper);
}

double TreeNode::RemainingWork(Eval lower, Eval upper) const {
  auto guard = ReadLock();
  assert((lower - kMinEval) % 2 == 1);
  assert((upper - kMinEval) % 2 == 1);
  assert(n_empties_ >= 0 && n_empties_ <= 60);
  lower = MaxEval(lower, weak_lower_);
  upper = MinEval(upper, weak_upper_);
  int disproof = GetPercentileLower(0.5F);
  int proof = disproof - 2;

  if (disproof <= lower) {
    return GetEvaluation(std::min(lower, weak_upper_)).DisproofNumber();
  } else if (proof >= upper) {
    return GetEvaluation(std::max(upper, weak_lower_)).ProofNumber();
  }
  assert(disproof >= lower && disproof <= upper);
  assert(proof >= lower && proof <= upper);
  return GetEvaluation(proof).ProofNumber() + GetEvaluation(disproof).DisproofNumber();
}
//std::optional<LeafToUpdate<TreeNode>> TreeNode::AsLeaf(int last_eval_goal) {
//  return AsLeaf<TreeNode>(last_eval_goal);
//}
//
//LeafToUpdate<TreeNode> TreeNode::AsLeafWithGoal(Eval eval_goal) {
//  auto guard = ReadLock();
//  return AsLeafNoLock<TreeNode>(eval_goal);
//}