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
#include <vector>

#include "evaluator_derivative.h"
#include "tree_node.h"

std::pair<TreeNode*, bool> TreeNodeSupplier::AddTreeNode(
    BitPattern player, BitPattern opponent, Square depth, uint8_t evaluator_index) {
  if (kUseTranspositions) {
    TreeNode* node = Mutable(player, opponent, depth, evaluator_index);
    if (node != nullptr) {
      return std::make_pair(node, false);
    }
  }

  int node_id = num_nodes_++;
  assert(node_id < kDerivativeEvaluatorSize);
  TreeNode& node = tree_nodes_[node_id];
  node.Reset(player, opponent, depth, evaluator_index);
  AddToHashMap(player, opponent, depth, evaluator_index, node_id);
  return std::make_pair(&node, true);
}

void EvaluatorThread::Run() {
  NVisited n_visited;
  TreeNode* first_position = evaluator_->first_position_;
  int last_eval_goal = kLessThenMinEval;
  while (!evaluator_->CheckFinished()) {
    evaluator_->UpdateWeakLowerUpper();
    auto leaf_opt = TreeNodeLeafToUpdate::BestDescendant(
        first_position, evaluator_->NThreadMultiplier(), last_eval_goal);
    if (!leaf_opt) {
      stats_.Add(1, NEXT_POSITION_FAIL);
      evaluator_->UpdateNThreadMultiplierFail();
      continue;
    }
    stats_.Add(1, NEXT_POSITION_SUCCESS);
    evaluator_->UpdateNThreadMultiplierSuccess();
    auto leaf = *leaf_opt;
    TreeNode* node = (TreeNode*) leaf.Leaf();
    last_eval_goal = leaf.EvalGoal() * (node->Depth() % 2 == 0 ? 1 : -1);
    assert(leaf.Alpha() <= leaf.EvalGoal() && leaf.EvalGoal() <= leaf.Beta());
    assert(node->IsLeaf());
    if (leaf.Leaf()->ToBeSolved(leaf.Alpha(), leaf.Beta(), evaluator_->num_tree_nodes_, first_position->GetNVisited())) {
      n_visited = SolvePosition(leaf, (int) std::max(50000.0, leaf.Leaf()->RemainingWork(leaf.Alpha(), leaf.Beta())));
    } else {
      n_visited = AddChildren(leaf);
    }
    leaf.Finalize(n_visited);
    evaluator_->just_started_ = false;
  }
}

NVisited EvaluatorThread::AddChildren(const TreeNodeLeafToUpdate& leaf) {
  TreeNode* node = (TreeNode*) leaf.Leaf();
  assert(node->IsLeaf());
  assert(node->NThreadsWorking() == 1);
  BitPattern player = node->Player();
  BitPattern opponent = node->Opponent();
  NVisited n_visited = 1;
  stats_.Add(1, TREE_NODE);

  auto moves = GetAllMovesWithPass(player, opponent);

  if (moves.empty()) {
    int final_eval = EvalToEvalLarge(GetEvaluationGameOver(player, opponent));
    node->SetSolved(final_eval, final_eval, *evaluator_);
    return n_visited;
  }

  std::vector<TreeNode*> children;
  children.reserve(moves.size());

  evaluator_depth_one_->Setup(player, opponent);
  evaluator_depth_one_->Invert();
  EvalLarge child_eval_goal = -EvalToEvalLarge(leaf.EvalGoal());
  int depth;
  double remaining_work = node->RemainingWork(leaf.Alpha(), leaf.Beta());

  for (int i = 0; i < moves.size(); ++i) {
    BitPattern flip = moves[i];
    BitPattern square = flip & ~(opponent | player);
    BitPattern new_player = NewPlayer(flip, opponent);
    BitPattern new_opponent = NewOpponent(flip, player);
    if (flip != 0) {
      evaluator_depth_one_->Update(square, flip);
    }
    auto [child, newly_inserted] = evaluator_->AddTreeNode(new_player, new_opponent, node->Depth() + 1);
    if (newly_inserted || !child->HasLeafEval()) {
      EvalLarge eval;
      EvalLarge quick_eval = evaluator_depth_one_->Evaluate();
      EvalLarge delta = abs(quick_eval - child_eval_goal);
  //      int expected_error = 8 * kErrors[child_n_empties];
      // Example:
      //      +4
      // -4   +0   +10
      if (node->Depth() > 0 && node->Depth() <= 2 && remaining_work > 100000000) {
        depth = 5;
      } else if (remaining_work > 20000000 ||
          (delta < 16 * 8 && remaining_work > 10000000) ||
          (delta < 8 * 8 && remaining_work > 2000000)) {
        depth = 4;
      } else if (remaining_work > 4000000 ||
          (delta < 16 * 8 && remaining_work > 2000000) ||
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
      child->SetLeafEval(eval, std::min(4, depth));
      child->AddDescendants(n_visited);
    }
    children.push_back(child);
//    child->SetLeafIfInvalid(eval, depth, *evaluator_);
    if (flip != 0) {
      evaluator_depth_one_->UndoUpdate(square, flip);
    }
  }
  assert(children.size() == moves.size());
  node->SetChildren(children, *evaluator_);
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
      node->Player(), node->Opponent(), node->NEmpties(), alpha, beta, max_proof);
  seen_positions = evaluator_alpha_beta_.GetNVisited() + 1;
  stats_.Merge(evaluator_alpha_beta_.GetStats());
  stats_.Add(1, TREE_NODE);

  if (eval == kLessThenMinEvalLarge) {
    stats_.Add(seen_positions, SOLVED_TOO_EARLY);
    return seen_positions + AddChildren(leaf);
  }
  assert(node->NThreadsWorking() <= 1);
  assert(node->NThreadsWorking() >= 1);
  // No need to lock, because this is the only thread that can touch this node.
  assert(node->IsLeaf());
  assert(eval >= kMinEvalLarge && eval <= kMaxEvalLarge);
  node->SetSolved(
      eval > alpha ? eval : kMinEvalLarge,
      eval < beta ? eval : kMaxEvalLarge,
      *evaluator_);
  return seen_positions;
}
