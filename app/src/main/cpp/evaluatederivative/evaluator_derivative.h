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

#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include <limits>
#include <optional>
#include <thread>
#include "tree_node.h"
#include "../constants.h"
#include "../evaluatealphabeta/evaluator_alpha_beta.h"
#include "../utils/misc.h"
#include "../evaluatedepthone/evaluator_depth_one_base.h"
#include "../evaluatedepthone/pattern_evaluator.h"

constexpr bool kUseTranspositions = true;
constexpr int kVisitedEndgameStart = 10000;
constexpr int kVisitedEndgameGoal = 10000;

enum Status {
    NONE = 0,
    RUNNING = 1,
    STOPPED_TIME = 2,
    STOPPED_POSITIONS = 3,
    SOLVED = 4,
    KILLING = 5,
    KILLED = 6,
    FAILED = 7,
};

class TreeNodeSupplier {
 public:
  TreeNodeSupplier() : num_tree_nodes_(0) {
    tree_nodes_ = new TreeNode[kDerivativeEvaluatorSize];
    tree_node_index_ = new std::atomic_int[kHashMapSize];
    for (int i = 0; i < kHashMapSize; ++i) {
      tree_node_index_[i] = kDerivativeEvaluatorSize;
    }
  }
  ~TreeNodeSupplier() {
    delete[] tree_nodes_;
    delete[] tree_node_index_;
  }

  const TreeNode* const Get(const Board& b) {
    return Get(b.GetPlayer(), b.GetOpponent());
  }

  TreeNode* Get(BitPattern player, BitPattern opponent) {
    int index = tree_node_index_[Hash(player, opponent)];
    if (index >= num_tree_nodes_) {
      return nullptr;
    }
    TreeNode& node = tree_nodes_[index];
    if (node.Player() == player && node.Opponent() == opponent) {
      return &node;
    }
    return nullptr;
  }

  void Reset() {
    num_tree_nodes_ = 0;
  }

  int NumTreeNodes() { return num_tree_nodes_; }

  TreeNode* AddTreeNode(BitPattern player, BitPattern opponent, Square depth, u_int8_t evaluator_index) {
    if (kUseTranspositions) {
      TreeNode* existing_node = Get(player, opponent);
      if (existing_node != nullptr && existing_node->Evaluator() == evaluator_index) {
        return existing_node;
      }
    }
    int hash = Hash(player, opponent);
    int node_id = num_tree_nodes_++;
    TreeNode& node = tree_nodes_[node_id];
    node.Reset(player, opponent, depth, evaluator_index);
    tree_node_index_[hash] = node_id;
    return &node;
  }

 private:
  std::atomic_int num_tree_nodes_;
  TreeNode* tree_nodes_;
  std::atomic_int* tree_node_index_;
};

class EvaluatorThread {
 public:
  EvaluatorThread(HashMap* hash_map, EvaluatorFactory evaluator_depth_one,
                  std::atomic_uint64_t* visited_midgame,
                  TreeNodeSupplier* tree_node_supplier,
                  uint8_t index) :
      evaluator_alpha_beta_(hash_map, evaluator_depth_one),
      visited_midgame_(visited_midgame),
      tree_node_supplier_(tree_node_supplier),
      index_(index) {}

  void Run(std::vector<LeafToUpdate>* leaves_, std::atomic_int* current_leaf_, int max_proof) {
    visited_for_endgame_ = 0;
    n_endgames_ = 0;
    int n_visited;
    for (int i = (*current_leaf_)++; i < leaves_->size(); i = (*current_leaf_)++) {
      LeafToUpdate leaf = (*leaves_)[i];
      TreeNode* node = leaf.Leaf();
      assert(node->IsLeaf());
      if (node->ToBeSolved(leaf.Alpha(), leaf.Beta(), max_proof)) {
        n_visited = SolvePosition(leaf);
      } else {
        n_visited = AddChildren(leaf);
      }
      for (TreeNode* parent : leaf.Parents()) {
        parent->AddDescendants(n_visited);
        parent->DecreaseNThreadsWorking();
      }
      node->UpdateFathers();
    }
  }

  int VisitedEndgames() const { return visited_for_endgame_; }
  int NEndgames() const { return n_endgames_; }

 private:
  EvaluatorAlphaBeta evaluator_alpha_beta_;
  int visited_for_endgame_;
  int n_endgames_;
  std::atomic_uint64_t* visited_midgame_;
  TreeNodeSupplier* tree_node_supplier_;
  u_int8_t index_;

  NVisited AddChildren(const LeafToUpdate& leaf) {
    TreeNode* node = leaf.Leaf();
    assert(node->IsLeaf());
    assert(node->NThreadsWorking() == 1);
    int depth = node->NEmpties() < 24 ? (node->NEmpties() < 22 ? 2 : 3) : 4;
    NVisited n_visited = 0;
    BitPattern player = node->Player();
    BitPattern opponent = node->Opponent();

    auto moves = GetAllMovesWithPass(player, opponent);

    if (moves.empty()) {
      int final_eval = GetEvaluationGameOver(player, opponent);
      node->SetSolved(final_eval);
      (*visited_midgame_)++;
      return 1;
    }

    std::vector<TreeNode*> children(moves.size());

    for (int i = 0; i < moves.size(); ++i) {
      BitPattern flip = moves[i];
      BitPattern new_player = NewPlayer(flip, opponent);
      BitPattern new_opponent = NewOpponent(flip, player);
      TreeNode* child = tree_node_supplier_->AddTreeNode(new_player, new_opponent, node->Depth() + 1, index_);
      children[i] = child;
      if (child->IsValid()) {
        continue;
      }
      int eval = evaluator_alpha_beta_.Evaluate(new_player, new_opponent, depth, kMinEvalLarge, kMaxEvalLarge);
      auto cur_visited = evaluator_alpha_beta_.GetNVisited();
      n_visited += cur_visited;
      child->SetWeakLowerUpper(-leaf.WeakUpper(), -leaf.WeakLower());
      child->SetLeaf(eval, depth, cur_visited);
    }
    node->SetChildren(children);
    node->UpdateFather();
    *visited_midgame_ += n_visited;
    return n_visited;
  }

  NVisited SolvePosition(const LeafToUpdate& leaf) {
    TreeNode* node = leaf.Leaf();
    assert(node->IsLeaf());
    EvalLarge alpha = EvalToEvalLarge(leaf.Alpha());
    EvalLarge beta = EvalToEvalLarge(leaf.Beta());
    NVisited seen_positions;
    EvalLarge eval;
    // for (int i = 0; i < 10; ++i) {
    eval = evaluator_alpha_beta_.Evaluate(
        node->Player(), node->Opponent(), node->NEmpties(), alpha, beta, 100000);
    // }
    seen_positions = evaluator_alpha_beta_.GetNVisited() + 1;

    visited_for_endgame_ += seen_positions;
    n_endgames_++;

    if (eval == kLessThenMinEvalLarge) {
      return seen_positions + AddChildren(leaf);
    }
    assert(node->NThreadsWorking() == 1);
    // No need to lock, because this is the only thread that can touch this node.
    node->SetWeakLowerUpper(leaf.WeakLower(), leaf.WeakUpper());

    if (eval < beta) {
      node->SetUpper(eval);
    }
    if (eval > alpha) {
      node->SetLower(eval);
    }
    return seen_positions;
  }
};

class EvaluatorDerivative {
 public:
  EvaluatorDerivative(TreeNodeSupplier* tree_node_supplier, HashMap* hash_map, EvaluatorFactory evaluator_depth_one, int n_threads, u_int8_t index = 0) :
      tree_node_supplier_(tree_node_supplier),
      threads_(),
      index_(index),
      first_position_(nullptr),
      proving_(false) {
    for (int i = 0; i < n_threads; ++i) {
      threads_.push_back(EvaluatorThread(hash_map, evaluator_depth_one, &visited_midgame_, tree_node_supplier_, index_));
    }
  }

  double Progress(float gap) {
    if (GetStatus() == SOLVED) {
      return -INFINITY;
    }
    TreeNode* board = GetFirstPosition();
    double evalEffect = -board->GetEval() / std::max(1.0F, gap);
    return evalEffect - log(board->GetNVisited()) / log(2);
  }

  void ContinueEvaluate(NVisited max_n_visited, double max_time) {
    max_n_visited_ = max_n_visited;
    max_time_ = max_time;
    elapsed_time_ = ElapsedTime();
    just_started_ = true;
    status_ = RUNNING;
    Run();
  }

  void Evaluate(
      BitPattern player, BitPattern opponent, Eval lower, Eval upper,
      NVisited max_n_visited, double max_time, bool approx = false) {
    assert((lower - kMinEval) % 2 == 1);
    assert((upper - kMinEval) % 2 == 1);
    assert(lower <= upper);
    TreeNode* first_position = GetFirstPosition();
    approx_ = approx;

//    EvalLarge eval = next_evaluators_[0].Evaluate(player, opponent, 4, kMinEvalLarge, kMaxEvalLarge);
    first_position_ = tree_node_supplier_->AddTreeNode(player, opponent, 0, index_);
    first_position_->SetLeaf(0, 4, 1);
    last_update_weak_ = 0;
    visited_for_endgame_ = kVisitedEndgameStart;
    visited_midgame_ = 0;
    lower_ = lower;
    upper_ = upper;
    num_batches_ = 0;
    sum_batch_sizes_ = 0;
    proving_ = false;
    ContinueEvaluate(max_n_visited, max_time);
  }

  void Stop() {
    if (status_ == RUNNING || status_ == STOPPED_TIME) {
      status_ = KILLING;
    }
  }

  TreeNode* const GetFirstPosition() {
    return first_position_;
  }

  Status GetStatus() { return status_; }

  NVisited GetNVisitedMidgame() { return visited_midgame_; }

  float AverageBatchSize() { return sum_batch_sizes_ / num_batches_; }

 private:
  std::atomic_int visited_for_endgame_;
  std::atomic_uint64_t visited_midgame_;
  NVisited max_n_visited_;
  double start_time_;
  double max_time_;
  NVisited last_update_weak_ = 0;
  Eval lower_ = -63;
  Eval upper_ = 63;
  Status status_ = SOLVED;
  std::vector<EvaluatorThread> threads_;
  ElapsedTime elapsed_time_;
  bool just_started_;
  bool approx_;
  TreeNodeSupplier* tree_node_supplier_;
  TreeNode* first_position_;
  u_int8_t index_;
  float num_batches_;
  float sum_batch_sizes_;
  bool proving_;

  void Run() {
    std::vector<LeafToUpdate> next_leaves;
    std::atomic_int current_leaf;
    while (!CheckFinished()) {
      next_leaves = GetNextPosition();
      current_leaf = 0;
      num_batches_++;
      sum_batch_sizes_ += next_leaves.size();
      std::vector<std::future<void>> futures;
      futures.reserve(threads_.size());
      for (int i = 0; i < threads_.size(); ++i) {
        futures.push_back(std::async(std::launch::async, &EvaluatorThread::Run, &threads_[i], &next_leaves, &current_leaf, int(visited_for_endgame_)));
      }
      int total_visited_endgames = 0;
      int total_endgames = 0;
      for (int i = 0; i < threads_.size(); ++i) {
        futures[i].get();
        const EvaluatorThread& evaluator_thread = threads_[i];
        total_visited_endgames += evaluator_thread.VisitedEndgames();
        total_endgames += evaluator_thread.NEndgames();
      }
      visited_for_endgame_ = std::min(50000, std::max(1000, visited_for_endgame_ - ((int) total_visited_endgames - kVisitedEndgameGoal * total_endgames) / 30));
      assert (GetFirstPosition()->NThreadsWorking() == 0);
      just_started_ = false;
    }
  }

  std::vector<LeafToUpdate> GetNextPosition() {
    std::vector<LeafToUpdate> result;
    UpdateWeakLowerUpper();
    if (CheckFinished()) {
      return {};
    }
    if (!proving_ && GetFirstPosition()->IsSolvedAtProb(0)) {
      proving_ = true;
      // std::cout << " PROVING " << GetFirstPosition()->GetNVisited() << " ";
    } else if (proving_ && !GetFirstPosition()->IsSolvedAtProb(0.02)) {
      // std::cout << " BACK " << GetFirstPosition()->GetNVisited() << " " << GetFirstPosition()->GetEval() << "\n";
      proving_ = false;
    }
    result = GetFirstPosition()->BestDescendants(proving_, threads_.size());
    assert (!result.empty());
    return result;
  }

  bool CheckFinished() {
//     synchronized (firstPosition) {
//       assert firstPosition.isLowerUpperOK();
//    double stepsUntilEnd = stepsUntilEnd();
//    this.bestStepsUntilEnd = std::min(this.bestStepsUntilEnd, stepsUntilEnd);

    auto first_position = GetFirstPosition();
    if (status_ == KILLING) {
      status_ = KILLED;
//      std::cout << "KILLED\n";
      return true;
    }
    if (status_ == KILLED) {
//      std::cout << "KILLED\n";
      return true;
    }
    if (first_position->IsSolved() || (first_position->IsPartiallySolved() && approx_)) {
      status_ = SOLVED;
//      std::cout << "SOLVED\n";
      return true;
    }
    if ((first_position->GetNVisited() > max_n_visited_ && !just_started_)
        || tree_node_supplier_->NumTreeNodes() > kDerivativeEvaluatorSize - 60) {
//      std::cout << "NVISITED\n";
      status_ = STOPPED_POSITIONS;
      return true;
    }
//    if (first_position->GetNVisited() > 0.8 * maxNVisited && !justStarted) {
//      if (firstPosition.getDescendants() > maxNVisited
//          || stepsUntilEnd == bestStepsUntilEnd) {
//        status = Status.STOPPED_POSITIONS;
//        return true;
//      }
//    }
    if (elapsed_time_.Get() > max_time_) {
//      std::cout << "TIME\n";
      status_ = STOPPED_TIME;
      return true;
    }
//    if (currentTime - startTimeMillis > maxTimeMillis * 0.8) {
//      if (currentTime - startTimeMillis > maxTimeMillis
//          || stepsUntilEnd == bestStepsUntilEnd) {
//        status = Status.STOPPED_TIME;
//        return true;
//      }
//    }
    return false;
  }

  void UpdateWeakLowerUpper() {
    auto first_position = GetFirstPosition();
    while (first_position->UpdateWeakLowerUpper()) {
      last_update_weak_ = NVisited();
    }
  }

};