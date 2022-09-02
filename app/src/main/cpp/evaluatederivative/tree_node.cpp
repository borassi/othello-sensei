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

#include <limits>
#include "tree_node.h"

LeafToUpdate::LeafToUpdate(TreeNode* leaf, int eval_goal) :
    leaf_(leaf),
    eval_goal_(static_cast<Eval>(eval_goal)),
    weak_lower_(leaf->WeakLower()),
    weak_upper_(leaf->WeakUpper()),
    loss_(0),
    parents_({leaf}),
    proof_number_delta_(FLT_MAX) {
  alpha_ = weak_lower_;
  beta_ = weak_upper_;
  UpdateAlphaBeta(leaf);
  assert(weak_lower_ <= alpha_ && alpha_ <= eval_goal_ && eval_goal_ <= beta_
         && beta_ <= weak_upper_);
}

void LeafToUpdate::UpdateAlphaBeta(TreeNode* node) {
  alpha_ = MinEval(eval_goal_, MaxEval(alpha_, node->GetPercentileLower(kProbForEndgameAlphaBeta) + 1));
  beta_ = MaxEval(eval_goal_, MinEval(beta_, node->GetPercentileUpper(kProbForEndgameAlphaBeta) - 1));
}

LeafToUpdate LeafToUpdate::CopyToChild(TreeNode* node, double extra_loss) const {
    LeafToUpdate leaf(node, -eval_goal_, -beta_, -alpha_, -weak_upper_, -weak_lower_, loss_ + extra_loss, proof_number_delta_);
    leaf.parents_ = parents_;
    leaf.UpdateAlphaBeta(node);
    leaf.parents_.push_back(node);
    assert(weak_lower_ <= alpha_ && alpha_ <= eval_goal_ && eval_goal_ <= beta_
           && beta_ <= weak_upper_);
    return leaf;
  }