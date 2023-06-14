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
#include <unistd.h>
#include <vector>

#include "evaluator_derivative.h"
#include "tree_node.h"

TreeNode* TreeNodeSupplier::AddTreeNode(
    BitPattern player, BitPattern opponent, Square depth,
    EvalLarge leaf_eval,
    Square eval_depth, EvaluatorDerivative* evaluator, bool* newly_inserted) {
  auto evaluator_index = evaluator->Index();
  if (kUseTranspositions) {
    TreeNode* node = Get(player, opponent, evaluator_index);
    if (node != nullptr) {
      *newly_inserted = false;
      return node;
    }
  }

  int node_id = num_nodes_++;
  assert(node_id < kDerivativeEvaluatorSize);
  TreeNode& node = tree_nodes_[node_id];
  node.Reset(player, opponent, depth, leaf_eval, eval_depth, evaluator);
  AddToHashMap(player, opponent, evaluator_index, node_id);
  *newly_inserted = true;
  return &node;
}

void EvaluatorThread::Run() {
  auto time = std::chrono::system_clock::now();
  auto duration = std::chrono::system_clock::now().time_since_epoch();
  int n_visited;
  stats_.Reset();
  TreeNode* first_position = evaluator_->GetFirstPosition();
  while (!evaluator_->CheckFinished(first_position)) {
    auto leaf_opt = TreeNodeLeafToUpdate::BestDescendant(
        first_position, evaluator_->NThreadMultiplier());
    if (leaf_opt) {
      stats_.Add(1, NEXT_POSITION_SUCCESS);
      evaluator_->UpdateNThreadMultiplierSuccess();
    } else {
      stats_.Add(1, NEXT_POSITION_FAIL);
      evaluator_->UpdateNThreadMultiplierFail();
      std::this_thread::sleep_for(1ms);
      continue;
    }
    auto leaf = *leaf_opt;
    TreeNode* node = (TreeNode*) leaf.Leaf();
    assert(node->IsLeaf());
//    leaf.UpdateFirstNode(first_position);
    auto visited_for_endgame = evaluator_->VisitedForEndgame();
    if (node->ToBeSolved(leaf.Alpha(), leaf.Beta(), visited_for_endgame)) {
      n_visited = SolvePosition(leaf, visited_for_endgame);
    } else {
      n_visited = AddChildren(leaf);
    }
    leaf.Finalize(n_visited);
    evaluator_->UpdateWeakLowerUpper(*first_position);
    evaluator_->just_started_ = false;
  }
}

NVisited EvaluatorThread::AddChildren(const TreeNodeLeafToUpdate& leaf) {
  TreeNode* node = (TreeNode*) leaf.Leaf();
  assert(node->IsLeaf());
  assert(node->NThreadsWorking() == 1);
  NVisited n_visited = 0;
  BitPattern player = node->Player();
  BitPattern opponent = node->Opponent();
  Stats stats;
  stats.Add(1, TREE_NODE);

  auto moves = GetAllMovesWithPass(player, opponent);

  if (moves.empty()) {
    int final_eval = EvalToEvalLarge(GetEvaluationGameOver(player, opponent));
    node->SetSolved(final_eval, final_eval, *evaluator_);
    stats_.Add(1, TREE_NODE);
    return 1;
  }

  std::vector<TreeNode*> children;
  children.reserve(moves.size());

  evaluator_depth_one_->Setup(player, opponent);
  evaluator_depth_one_->Invert();
  EvalLarge child_eval_goal = -EvalToEvalLarge(leaf.EvalGoal());
  int child_n_empties = node->NEmpties() - 1;
  int depth;
  float remaining_work = node->RemainingWork(leaf.Alpha(), leaf.Beta());

  for (int i = 0; i < moves.size(); ++i) {
    BitPattern flip = moves[i];
    BitPattern square = flip & ~(opponent | player);
    BitPattern new_player = NewPlayer(flip, opponent);
    BitPattern new_opponent = NewOpponent(flip, player);
    bool newly_inserted = false;
    if (flip != 0) {
      evaluator_depth_one_->Update(square, flip);
    }
    EvalLarge eval;
    EvalLarge quick_eval = evaluator_depth_one_->Evaluate();
    EvalLarge delta = abs(quick_eval - child_eval_goal);
//      int expected_error = 8 * kErrors[child_n_empties];
    // Example:
    //      +4
    // -4   +0   +10
    if (remaining_work > 10000000 ||
        (delta < 16 * 8 && remaining_work > 5000000) ||
        (delta < 8 * 8 && remaining_work > 1000000)) {
      depth = 4;
    } else if (remaining_work > 2000000 ||
        (delta < 16 * 8 && remaining_work > 1000000) ||
        (delta < 8 * 8)) {
      depth = 3;
    } else {
      depth = 2;
    }
    NVisited cur_n_visited;
    eval = evaluator_alpha_beta_.Evaluate(new_player, new_opponent, depth, kMinEvalLarge, kMaxEvalLarge);
    const Stats& cur_stats = evaluator_alpha_beta_.GetStats();
    stats_.Merge(cur_stats);
    cur_n_visited = cur_stats.GetAll();
    n_visited += cur_n_visited;
    TreeNode* child = evaluator_->AddTreeNode(new_player, new_opponent, node->Depth() + 1, eval, depth, &newly_inserted);
    children.push_back(child);
//    child->SetLeafIfInvalid(eval, depth, *evaluator_);
    child->AddDescendants(n_visited);
    if (flip != 0) {
      evaluator_depth_one_->UndoUpdate(square, flip);
    }
  }
  assert(children.size() == moves.size());
  node->SetChildren(children);
  assert(!node->IsLeaf());
  assert((node->WeakLower() - kMinEval) % 2 == 1);
  return n_visited;
}

NVisited EvaluatorThread::SolvePosition(const TreeNodeLeafToUpdate& leaf,
                                        int max_proof) {
  TreeNode* node = (TreeNode*) leaf.Leaf();
  assert(node->IsLeaf());
  EvalLarge alpha = EvalToEvalLarge(leaf.Alpha());
  EvalLarge beta = EvalToEvalLarge(leaf.Beta());
  NVisited seen_positions;
  EvalLarge eval;
  eval = evaluator_alpha_beta_.Evaluate(
      node->Player(), node->Opponent(), node->NEmpties(), alpha, beta,
      std::max(max_proof * 5, 50000));
  seen_positions = evaluator_alpha_beta_.GetNVisited() + 1;
  stats_.Merge(evaluator_alpha_beta_.GetStats());
  stats_.Add(1, TREE_NODE);

  if (eval == kLessThenMinEvalLarge) {
    return seen_positions + AddChildren(leaf);
  }
  assert(node->NThreadsWorking() == 1);
  // No need to lock, because this is the only thread that can touch this node.
  assert(node->IsLeaf());
  assert(eval >= kMinEvalLarge && eval <= kMaxEvalLarge);
  node->SetSolved(
      eval > alpha ? eval : kMinEvalLarge,
      eval < beta ? eval : kMaxEvalLarge,
      *evaluator_);
  return seen_positions;
}
