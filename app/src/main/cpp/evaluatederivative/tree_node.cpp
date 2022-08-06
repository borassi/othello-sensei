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

#include "tree_node.h"

LeafToUpdate::LeafToUpdate(TreeNode* leaf, int eval_goal) :
    leaf_(leaf),
    eval_goal_(static_cast<Eval>(eval_goal)),
    alpha_(static_cast<Eval>(std::min(eval_goal, std::max((int) leaf->WeakLower(), leaf->GetPercentileLower(kProbForEndgameAlphaBeta) + 1)))),
    beta_(static_cast<Eval>(std::max(eval_goal, std::min((int) leaf->WeakUpper(), leaf->GetPercentileUpper(kProbForEndgameAlphaBeta) - 1)))),
    weak_lower_(leaf->WeakLower()),
    weak_upper_(leaf->WeakUpper()),
    loss_(0),
    parents_({leaf}) {
  assert(weak_lower_ <= alpha_ && alpha_ <= eval_goal_ && eval_goal_ <= beta_
         && beta_ <= weak_upper_);
}

LeafToUpdate LeafToUpdate::ToChild(TreeNode* child, double extra_loss) const {
    LeafToUpdate leaf = LeafToUpdate(child, -eval_goal_);
    leaf.weak_upper_ = static_cast<Eval>(-weak_lower_);
    leaf.weak_lower_ = static_cast<Eval>(-weak_upper_);
    leaf.alpha_ = MaxEval(static_cast<Eval>(-beta_), leaf.alpha_);
    leaf.beta_ = MinEval(static_cast<Eval>(-alpha_), leaf.beta_);
    leaf.loss_ = loss_ + extra_loss;
    leaf.parents_ = parents_;
    leaf.parents_.push_back(child);
    assert(weak_lower_ <= alpha_ && alpha_ <= eval_goal_ && eval_goal_ <= beta_
           && beta_ <= weak_upper_);
    return leaf;
  }