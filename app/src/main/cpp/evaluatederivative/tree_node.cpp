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
      stream << std::setprecision(0) << eval.DisproofNumber();
    } else if (p == 1) {
      stream << std::setprecision(0) << eval.ProofNumber();
    } else {
      stream << std::setprecision(2) << p;
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

std::optional<LeafToUpdate<TreeNode>> TreeNode::AsLeaf() {
  return AsLeaf<TreeNode>();
}

void TreeNode::SetSolved(EvalLarge lower, EvalLarge upper, const EvaluatorDerivative& evaluator) {
  auto guard = WriteLock();
  assert(depth_ > 0);  // Otherwise, the first position might lock the evaluator and viceversa.
  assert(IsValidNoLock());
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

void TreeNode::SetLeafIfInvalid(
    EvalLarge leaf_eval,
    Square depth,
    const EvaluatorDerivative &evaluator_derivative) {
  auto guard = WriteLock();
  if (!IsValidNoLock()) {
    auto [weak_lower, weak_upper] = evaluator_derivative.GetWeakLowerUpper(depth_);
    SetLeafNoLock(leaf_eval, depth, weak_lower, weak_upper);
  }
}

LeafToUpdate<TreeNode> TreeNode::AsLeaf(Eval eval_goal) {
  auto guard = ReadLock();
  return AsLeafNoLock<TreeNode>(eval_goal);
}