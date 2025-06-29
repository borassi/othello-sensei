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

constexpr int kMutexAtDepthBits = 10;
constexpr int kMutexAtDepthSize = 1 << kMutexAtDepthBits;

namespace {

std::vector<std::vector<std::mutex>> BuildMutexAtDepth() {
  std::vector<std::vector<std::mutex>> mutex_at_depth(120);
  for (int i = 0; i < 120; ++i) {
    mutex_at_depth[i] = std::vector<std::mutex>(kMutexAtDepthSize);
  }
  return mutex_at_depth;
}
}  // namespace

std::atomic_bool TreeNode::extend_eval_failed_(false);
std::vector<std::vector<std::mutex>> TreeNode::mutex_at_depth_(BuildMutexAtDepth());

std::ostream& operator<<(std::ostream& stream, const Node& b) {
  stream << b.Player() << " " << b.Opponent() << ": " << b.LeafEval()
         << " [" << (int) b.Lower() << " " << (int) b.Upper() << "]";
  for (int i = b.WeakLower(); i <= b.WeakUpper(); i += 2) {
    Evaluation eval = b.GetEvaluation(i);
    double p = eval.ProbGreaterEqual();
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
  stream << " (vis: " << b.GetNVisited() << ")";
  return stream;
}

std::ostream& operator<<(std::ostream& stream, const TreeNode& n) {
  auto guard = n.GetGuard();
  stream << (Node) n;
  return stream;
}

std::vector<Node> TreeNode::Fathers() {
  std::vector<Node> result;
  for (unsigned int i = 0; i < n_fathers_; ++i) {
    result.push_back(fathers_[i]);
  }
  return result;
}

void TreeNode::SetSolved(EvalLarge lower, EvalLarge upper, const EvaluatorDerivative& evaluator) {
  auto guard = GetGuard();
  assert(lower % 16 == 0);
  assert(upper % 16 == 0);
  assert(IsLeafNoLock());
  assert(kMinEvalLarge <= leaf_eval_ && leaf_eval_ <= kMaxEvalLarge);
  // NOTE: If depth == 0, we exploit that evaluator.GetWeakLowerUpper() does not
  // need locks. Otherwise, the evaluator would lock again this position and
  // get stuck.
  auto weak_lower_upper = evaluator.GetWeakLowerUpper(depth_);
  SetSolvedNoUpdate(lower, upper);
  UpdateLeafWeakLowerUpper(weak_lower_upper.first, weak_lower_upper.second);
}

void TreeNode::Reset(BitPattern player, BitPattern opponent, int depth,
                     uint8_t evaluator) {
  mutex_ = &TreeNode::mutex_at_depth_[depth % 120][Hash<kMutexAtDepthBits>(player, opponent)];
  auto guard = GetGuard();
  ResetNoLock(player, opponent, depth, evaluator);
}

void TreeNode::ResetNoLock(
    BitPattern player, BitPattern opponent, int depth, uint8_t evaluator) {
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
    fathers_ = nullptr;
  }
  if (n_children_ != 0) {
    delete[] children_;
    n_children_ = 0;
    is_leaf_ = true;
    children_ = nullptr;
  }
  assert(is_leaf_);
  assert(children_ == nullptr);

  depth_ = depth;
  min_evaluation_ = kLessThenMinEval;

  n_threads_working_ = 0;
  descendants_ = 0;

  leaf_eval_ = kLessThenMinEvalLarge;
}

void TreeNode::SetChildren(const std::vector<TreeNode*>& children, const EvaluatorDerivative& evaluator) {
  auto guard = GetGuard();
  auto [weak_lower, weak_upper] = evaluator.GetWeakLowerUpper(Depth());
  for (TreeNode* child : children) {
    auto child_guard = child->GetGuard();
    if (child->IsLeafNoLock()) {
      child->UpdateLeafWeakLowerUpper(-weak_upper, -weak_lower);
    } else {
      // This fixes a weird edge case where ExtendEval fails:
      // 1. We ExtendEval on a leaf N.
      // 2. We run AddChildren on N (this function), and 'child' is internal,
      //    not yet updated (NOW).
      // In this case, we set a flag to ensure the caller knows this failed.
      // This is thread-safe because we are keeping the lock on 'child', which
      // means that we set the flag before ExtendEval finishes (because it needs
      // to update 'child' before finishing).
      // NOTE: We cannot rerun the ExtendEval in EvaluatorDerivative, because we
      // don't know if the error propagated to the first position.
      if (child->weak_lower_ > -weak_upper || child->weak_upper_ < -weak_lower) {
        extend_eval_failed_ = true;
      }
    }
  }
  SetChildrenNoLock(children);
}

double Node::RemainingWork(Eval lower, Eval upper) const {
  assert((lower - kMinEval) % 2 == 1);
  assert((upper - kMinEval) % 2 == 1);
  assert(n_empties_ >= 0 && n_empties_ <= 64);
  if (Node::IsSolved(lower, upper, false)) {
    return 0;
  }
  Eval weak_lower = MaxEval(lower_ + 1, weak_lower_);
  Eval weak_upper = MinEval(upper_ - 1, weak_upper_);
  lower = MaxEval(lower, weak_lower);
  upper = MinEval(upper, weak_upper);
  int disproof = GetPercentileLower(0.5F);
  int proof = disproof - 2;

  if (disproof <= lower) {
    return GetEvaluation(std::min(lower, weak_upper)).DisproofNumber();
  } else if (proof >= upper) {
    return GetEvaluation(std::max(upper, weak_lower)).ProofNumber();
  }
  assert(disproof >= lower && disproof <= upper);
  assert(proof >= lower && proof <= upper);
  return GetEvaluation(proof).ProofNumber() + GetEvaluation(disproof).DisproofNumber();
}