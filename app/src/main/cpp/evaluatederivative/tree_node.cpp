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

int TreeNode::visited_for_endgame_(5000);
float TreeNode::prob_leaf_easy_(0);
//int TreeNode::easy_leaves_(0);
//int TreeNode::leaves_(0);

constexpr int kVisitedEndgameGoal = 6500;

void TreeNode::UpdateVisitedForEndgame(int visited_cur_endgame) {
//  if (easy_leaves_ > leaves_ * 0.8) {
//    return;
//  }
//  if (i++ % 10 == 0) {
//    std::cout << visited_for_endgame_ << " " << visited_cur_endgame << " " << prob_leaf_easy_ << "\n";
//  }
  visited_for_endgame_ = std::min(50000, std::max(1000, visited_for_endgame_ - (visited_cur_endgame - kVisitedEndgameGoal) / 10));
}

void TreeNode::ResetVisitedForEndgame() {
  visited_for_endgame_ = 6000;
  prob_leaf_easy_ = 0;
//  easy_leaves_ = 0;
//  leaves_ = 0;
}