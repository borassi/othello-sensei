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
#include <unistd.h>
#include <vector>

#include "evaluator_derivative.h"
#include "tree_node.h"

void EvaluatorThread::Run() {
  auto time = std::chrono::system_clock::now();
  auto duration = std::chrono::system_clock::now().time_since_epoch();
  int n_visited;
  stats_.Reset();
  TreeNode* first_position;
  first_position = evaluator_->GetFirstPosition();
//  first_position_copy_.FromOtherNode(first_position);
  while (!evaluator_->CheckFinished(first_position)) {
    ElapsedTime t;
    evaluator_->UpdateWeakLowerUpper(*first_position);
//    first_position.UpdateFather();
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
    int final_eval = GetEvaluationGameOver(player, opponent);
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

  for (int i = 0; i < moves.size(); ++i) {
    BitPattern flip = moves[i];
    BitPattern square = flip & ~(opponent | player);
    BitPattern new_player = NewPlayer(flip, opponent);
    BitPattern new_opponent = NewOpponent(flip, player);
    bool newly_inserted = false;
    TreeNode* child = evaluator_->AddTreeNode(new_player, new_opponent, node->Depth() + 1, &newly_inserted);
    children.push_back(child);
    if (!newly_inserted && child->IsValid()) {
      continue;
    }
    if (flip != 0) {
      evaluator_depth_one_->Update(square, flip);
    }
    EvalLarge eval;
    EvalLarge quick_eval = evaluator_depth_one_->Evaluate();
//      int expected_error = 8 * kErrors[child_n_empties];
    // Example:
    //      +4
    // -4   +0   +10
    if (child_n_empties > 28 || (quick_eval < child_eval_goal + 4 * 8 && child_n_empties > 20)) {
      depth = 4;
    } else if (child_n_empties > 24 || (quick_eval < child_eval_goal + 12 * 8 && child_n_empties > 18)) {
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
    child->SetLeafIfInvalid(eval, depth, *evaluator_);
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
      std::max(max_proof * 5, 30000));
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

//NVisited EvaluatorThread::AddManyChildren(const TreeNodeLeafToUpdate& leaf, int max_proof) {
//  auto root = leaf.Leaf();
//  TreeNode* root_node = (TreeNode*) root;
//  int eval_goal = leaf.EvalGoal();
//  int initial_log_derivative = root->MaxLogDerivative(eval_goal);
//  bool transposition = false;
//
//  float original_prob = root->ProbGreaterEqual(eval_goal);
//  float original_work = std::min(root->ProofNumber(eval_goal), root->DisproofNumber(eval_goal));
//  NVisited n_visited = AddChildren(leaf, &transposition);
//  const Evaluation& evaluation = root->GetEvaluation(eval_goal);
//  if (evaluation.IsSolved()) {
//    return n_visited;
//  }
//  NVisited cur_n_visited;
//  int i = 0;
//  bool proving = original_prob < 0.02 || original_prob > 0.98;
//  while (true) {
//    ++i;
//    if (evaluation.IsSolved() ||
//        (proving
//         &&
//         std::min(root->ProofNumber(eval_goal), root->DisproofNumber(eval_goal)) > 1.2 * original_work + 40000
//        ) ||
//        (!proving
//         && (leaf.Loss() + ExpectedLoss(evaluation, original_prob) -
//         initial_log_derivative < -3 * kLogDerivativeMultiplier)
//         && i > 5
//        ) ||
//        transposition ||
//        i > 30) {
////        std::cout << i << " " << proving << " " << original_prob << " " << leaf
////        .Loss() << " + " << ExpectedLoss(evaluation, original_prob) -
////        initial_log_derivative << " < " <<
////        -kLogDerivativeMultiplier * 1.5 << "\n";
////        std::cout << std::min(root->ProofNumber(eval_goal),
////                              root->DisproofNumber(eval_goal)) << " " <<
////                              original_work << "\n";
//      break;
//    }
//    assert(!leaf.Leaf()->GetEvaluation(eval_goal).IsSolved());
//    TreeNodeLeafToUpdate new_leaf_start = leaf.Copy();
//    auto new_leaf_opt = TreeNodeLeafToUpdate::BestDescendant(new_leaf_start, evaluator_->NThreadMultiplier());
//    if (!new_leaf_opt) {
//      break;
//    }
//    auto& new_leaf = *new_leaf_opt;
//    TreeNode* new_leaf_node = new_leaf.Leaf();
//    assert(new_leaf_node->IsLeaf());
//    assert(new_leaf_node->NThreadsWorking() == 1);
//    if (new_leaf_node->ToBeSolved(new_leaf.Alpha(), new_leaf.Beta(),
//                                  max_proof)) {
//      cur_n_visited = SolvePosition(new_leaf, &transposition, max_proof);
//    } else {
//      cur_n_visited = AddChildren(new_leaf, &transposition);
//    }
//    new_leaf.Finalize(cur_n_visited);
//    n_visited += cur_n_visited;
//    root_node->UpdateFather();
//  }
//  return n_visited;
//}