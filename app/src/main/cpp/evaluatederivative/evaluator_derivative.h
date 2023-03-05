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

#ifndef OTHELLOSENSEI_EVALUATEDERIVATIVE_EVALUATOR_DERIVATIVE_H
#define OTHELLOSENSEI_EVALUATEDERIVATIVE_EVALUATOR_DERIVATIVE_H

#if ANDROID
#include <android/log.h>
#endif
#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include <limits>
#include <optional>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include "tree_node.h"
#include "../constants.h"
#include "../evaluatealphabeta/evaluator_alpha_beta.h"
#include "../utils/misc.h"
#include "../evaluatedepthone/evaluator_depth_one_base.h"
#include "../evaluatedepthone/pattern_evaluator.h"

constexpr bool kUseTranspositions = true;
constexpr int kVisitedEndgameStart = 10000;
constexpr int kVisitedEndgameGoal = 10000;
constexpr int kMaxChildrenUpdates = 5;

enum Status {
    NONE = 0,
    RUNNING = 1,
    STOPPED_TIME = 2,
    STOPPED_POSITIONS = 3,
    SOLVED = 4,
    KILLING = 5,
    KILLED = 6,
    FAILED = 7,
    STOPPED_TREE_POSITIONS = 8,
};

typedef std::tuple<BitPattern, BitPattern, u_int8_t> NodeKey;

struct TupleEqual
{
  std::size_t operator()(const NodeKey& node1, const NodeKey& node2) const {
    return
        std::get<0>(node1) == std::get<0>(node2) &&
        std::get<1>(node1) == std::get<1>(node2) &&
        std::get<2>(node1) == std::get<2>(node2);
  }
};

struct TupleHasher
{
  std::size_t operator()(const NodeKey& node) const {
    return HashFull(std::get<0>(node), std::get<1>(node) + std::get<2>(node));
  }
};

class TreeNodeSupplier {
 public:
  TreeNodeSupplier() : num_tree_nodes_(0) {
    tree_nodes_ = new TreeNode[kDerivativeEvaluatorSize];
    tree_node_index_ = new int[kHashMapSize];
    std::fill_n(tree_node_index_, kHashMapSize, kDerivativeEvaluatorSize);
  }
  ~TreeNodeSupplier() {
    delete[] tree_nodes_;
    delete[] tree_node_index_;
  }
  const TreeNode* const Get(const Board& b, u_int8_t evaluator_index) {
    return Get(b.Player(), b.Opponent(), evaluator_index);
  }

  TreeNode* Get(BitPattern player, BitPattern opponent, u_int8_t evaluator_index) {
    return GetNoLock(player, opponent, evaluator_index);
  }

  void Reset() {
    num_tree_nodes_ = 0;
  }

  int NumTreeNodes() {
    return num_tree_nodes_;
  }

  TreeNode* AddTreeNode(BitPattern player, BitPattern opponent, Square depth, u_int8_t evaluator_index) {
    bool newly_inserted;
    return AddTreeNode(player, opponent, depth, evaluator_index, &newly_inserted);
  }
  TreeNode* AddTreeNode(BitPattern player, BitPattern opponent, Square depth, u_int8_t evaluator_index, bool* newly_inserted) {
    assert(tree_nodes_.size() < kDerivativeEvaluatorSize);
    if (kUseTranspositions) {
      TreeNode* node = GetNoLock(player, opponent, evaluator_index);
      if (node != nullptr) {
        *newly_inserted = false;
        return node;
      }
    }

    int hash = HashNoLock(player, opponent, evaluator_index);
    int node_id = num_tree_nodes_++;
    assert(node_id < kDerivativeEvaluatorSize);
    TreeNode& node = tree_nodes_[node_id];
    node.Reset(player, opponent, depth, evaluator_index);
    tree_node_index_[hash] = node_id;
    *newly_inserted = true;
    return &node;
  }

 private:
  TreeNode* tree_nodes_;
  int* tree_node_index_;
  std::atomic_int num_tree_nodes_;

  int HashNoLock(BitPattern player, BitPattern opponent, u_int8_t evaluator) {
    return Hash(player, opponent) ^ std::hash<u_int8_t>{}(evaluator);
  }

  TreeNode* GetNoLock(BitPattern player, BitPattern opponent, u_int8_t
                      evaluator) {
    int index = tree_node_index_[HashNoLock(player, opponent, evaluator)];
    if (index >= num_tree_nodes_) {
      return nullptr;
    }
    TreeNode& node = tree_nodes_[index];
    if (node.Player() == player && node.Opponent() == opponent && node.Evaluator() == evaluator) {
      return &node;
    }
    return nullptr;
  }
};

class EvaluatorThread {
 public:
  EvaluatorThread(HashMap* hash_map, EvaluatorFactory evaluator_depth_one,
                  TreeNodeSupplier* tree_node_supplier,
                  std::atomic_bool* thread_finished,
                  uint8_t index) :
      evaluator_alpha_beta_(hash_map, evaluator_depth_one),
      evaluator_depth_one_(evaluator_depth_one()),
      tree_node_supplier_(tree_node_supplier),
      index_(index),
      thread_finished_(thread_finished) {}

  void Run(std::vector<TreeNodeLeafToUpdate>* leaves_,
           std::atomic_int* current_leaf_, int max_proof) {
    int n_visited;
    stats_.Reset();
    bool transposition = false;
    for (int i = (*current_leaf_)++; i < leaves_->size(); i = (*current_leaf_)++) {
      TreeNodeLeafToUpdate leaf = (*leaves_)[i];
      TreeNode* node = (TreeNode*) leaf.Leaf();
      assert(node->IsLeaf());
      if (node->ToBeSolved(leaf.Alpha(), leaf.Beta(), max_proof)) {
        n_visited = SolvePosition(leaf, &transposition, max_proof);
      } else {
        n_visited = AddChildren(leaf, &transposition);
      }
      leaf.Finalize(n_visited);
    }
    *thread_finished_ = true;
  }

  const Stats& GetStats() const { return stats_; }

 private:
  EvaluatorAlphaBeta evaluator_alpha_beta_;
  std::unique_ptr<EvaluatorDepthOneBase> evaluator_depth_one_;
  Stats stats_;
  TreeNodeSupplier* tree_node_supplier_;
  u_int8_t index_;
  std::atomic_bool* thread_finished_;

  int ExpectedLoss(Evaluation root_eval, float original_prob) {
    return root_eval.MaxLogDerivative()
        - kCombineProb.log_derivative[(int) round(original_prob * 255)]
        + kCombineProb.log_derivative[(int) round(root_eval.ProbGreaterEqual() * 255)]
        - kCombineProb.log_derivative[(int) round((1 - original_prob) * 255)]
        + kCombineProb.log_derivative[(int) round((1 - root_eval.ProbGreaterEqual()) * 255)];
  }

  NVisited AddManyChildren(const TreeNodeLeafToUpdate& leaf, int max_proof) {
    auto root = leaf.Leaf();
    TreeNode* root_node = (TreeNode*) root;
    int eval_goal = leaf.EvalGoal();
    int initial_log_derivative = root->MaxLogDerivative(eval_goal);
    bool transposition = false;

    float original_prob = root->ProbGreaterEqual(eval_goal);
    float original_work = std::min(root->ProofNumber(eval_goal), root->DisproofNumber(eval_goal));
    NVisited n_visited = AddChildren(leaf, &transposition);
    const Evaluation& evaluation = root->GetEvaluation(eval_goal);
    if (evaluation.IsSolved()) {
      return n_visited;
    }
    NVisited cur_n_visited;
    int i = 0;
    bool proving = original_prob < 0.02 || original_prob > 0.98;
    while (true) {
      ++i;
      if (evaluation.IsSolved() ||
          (proving
           &&
           std::min(root->ProofNumber(eval_goal), root->DisproofNumber(eval_goal)) > 1.2 * original_work + 40000
          ) ||
          (!proving
           && (leaf.Loss() + ExpectedLoss(evaluation, original_prob) -
           initial_log_derivative < -3 * kLogDerivativeMultiplier)
           && i > 5
          ) ||
          *thread_finished_ ||
          transposition ||
          i > 30) {
//        std::cout << i << " " << proving << " " << original_prob << " " << leaf
//        .Loss() << " + " << ExpectedLoss(evaluation, original_prob) -
//        initial_log_derivative << " < " <<
//        -kLogDerivativeMultiplier * 1.5 << "\n";
//        std::cout << std::min(root->ProofNumber(eval_goal),
//                              root->DisproofNumber(eval_goal)) << " " <<
//                              original_work << "\n";
        break;
      }
      assert(!leaf.Leaf()->GetEvaluation(eval_goal).IsSolved());
      TreeNodeLeafToUpdate new_leaf_start = leaf.Copy();
      auto new_leaf_opt = TreeNodeLeafToUpdate::BestDescendant(new_leaf_start);
      if (!new_leaf_opt) {
        break;
      }
      auto& new_leaf = *new_leaf_opt;
      TreeNode* new_leaf_node = new_leaf.Leaf();
      assert(new_leaf_node->IsLeaf());
      assert(new_leaf_node->NThreadsWorking() == 1);
      if (new_leaf_node->ToBeSolved(new_leaf.Alpha(), new_leaf.Beta(),
                                    max_proof)) {
        cur_n_visited = SolvePosition(new_leaf, &transposition, max_proof);
      } else {
        cur_n_visited = AddChildren(new_leaf, &transposition);
      }
      new_leaf.Finalize(cur_n_visited);
      n_visited += cur_n_visited;
      root_node->UpdateFather();
    }
    return n_visited;
  }

  NVisited AddChildren(const TreeNodeLeafToUpdate& leaf,
                       bool* transposition) {
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
      node->SetSolved(final_eval, final_eval);
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
      TreeNode* child = tree_node_supplier_->AddTreeNode(new_player, new_opponent, node->Depth() + 1, index_, &newly_inserted);
      *transposition = *transposition || !newly_inserted;
      children.push_back(child);
      if (!newly_inserted) {
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
      if (child_n_empties > 28 || (quick_eval < child_eval_goal + 4 * 8 && child_n_empties > 22)) {
        depth = 4;
      } else if (child_n_empties > 24 || (quick_eval < child_eval_goal + 12 * 8 && child_n_empties > 20)) {
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
      child->SetLeaf(-leaf.WeakUpper(), -leaf.WeakLower(), eval, depth);
      // Usually a no-op, but another thread might have added a father before
      // the node was valid. We need to update that father.
      child->UpdateFathers();
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

  NVisited SolvePosition(const TreeNodeLeafToUpdate& leaf,
                         bool* transposition, int max_proof) {
    TreeNode* node = (TreeNode*) leaf.Leaf();
    assert(node->IsLeaf());
    EvalLarge alpha = EvalToEvalLarge(leaf.Alpha());
    EvalLarge beta = EvalToEvalLarge(leaf.Beta());
    NVisited seen_positions;
    EvalLarge eval;
    eval = evaluator_alpha_beta_.Evaluate(
        node->Player(), node->Opponent(), node->NEmpties(), alpha, beta,
        std::max(max_proof, 50000) * 10);
    seen_positions = evaluator_alpha_beta_.GetNVisited() + 1;
    stats_.Merge(evaluator_alpha_beta_.GetStats());
    stats_.Add(1, TREE_NODE);

    if (eval == kLessThenMinEvalLarge) {
      return seen_positions + AddChildren(leaf, transposition);
    }
    assert(node->NThreadsWorking() == 1);
    // No need to lock, because this is the only thread that can touch this node.
    assert(node->IsLeaf());
    assert(eval >= kMinEvalLarge && eval <= kMaxEvalLarge);
    node->SetSolved(
        eval > alpha ? eval : kMinEvalLarge,
        eval < beta ? eval : kMaxEvalLarge);
    node->UpdateLeafEval();
    return seen_positions;
  }
};

class EvaluatorDerivative {
 public:
  EvaluatorDerivative(TreeNodeSupplier* tree_node_supplier, HashMap* hash_map, EvaluatorFactory evaluator_depth_one, int n_threads, u_int8_t index = 0) :
      threads_(),
      tree_node_supplier_(tree_node_supplier),
      first_position_(nullptr),
      index_(index) {
    for (int i = 0; i < n_threads; ++i) {
      threads_.push_back(std::make_shared<EvaluatorThread>(hash_map, evaluator_depth_one, tree_node_supplier_, &thread_finished_, index_));
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
    assert(kMinEval <= lower && lower <= kMaxEval);
    assert(kMinEval <= upper && upper <= kMaxEval);
    assert(lower <= upper);
    approx_ = approx;
    first_position_ = tree_node_supplier_->AddTreeNode(player, opponent, 0, index_);
    first_position_->SetLeaf(lower, upper, 0, 4);
    last_update_weak_ = 0;
    stats_.Reset();
    lower_ = lower;
    upper_ = upper;
    num_batches_ = 0;
    sum_batch_sizes_ = 0;
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

  const Stats& GetStats() const { return stats_; }

  float AverageBatchSize() { return sum_batch_sizes_ / num_batches_; }

  u_int8_t Index() const { return index_; }

 private:
  Stats stats_;
  NVisited max_n_visited_;
  double max_time_;
  NVisited last_update_weak_ = 0;
  Eval lower_ = -63;
  Eval upper_ = 63;
  Status status_ = SOLVED;
  std::vector<std::shared_ptr<EvaluatorThread>> threads_;
  ElapsedTime elapsed_time_;
  bool just_started_;
  bool approx_;
  TreeNodeSupplier* tree_node_supplier_;
  TreeNode* first_position_;
  u_int8_t index_;
  float num_batches_;
  float sum_batch_sizes_;
  std::atomic_bool thread_finished_;

  int VisitedForEndgame() {
    auto remaining_work = first_position_->RemainingWork();
    auto visited = first_position_->GetNVisited();
    float position = (float) visited / (visited + remaining_work);
    float remaining_nodes = (visited + remaining_work) / 1500.0F;
    float current_nodes = tree_node_supplier_->NumTreeNodes();
    return std::max(2000, std::min(100000, 20000 +
        (int) ((current_nodes - pow(position, 0.7) * remaining_nodes) * 10)));
  }

  void Run() {
    std::vector<TreeNodeLeafToUpdate> next_leaves;
    std::atomic_int current_leaf;
    UpdateWeakLowerUpper();
    while (!CheckFinished()) {
      thread_finished_ = false;
      ElapsedTime time_next;
      next_leaves = GetNextPosition();
      stats_.AddTimeNextPosition(time_next.Get());
      current_leaf = 0;
      num_batches_++;
      sum_batch_sizes_ += next_leaves.size();
      std::vector<std::future<void>> futures;
      futures.reserve(threads_.size());
      int num_threads = std::min(threads_.size(), next_leaves.size());
      int visited_for_endgame = VisitedForEndgame();
      ElapsedTime time_deepen;
      for (int i = 0; i < num_threads; ++i) {
        futures.push_back(std::async(
            i == 0 ? std::launch::deferred : std::launch::async,
            &EvaluatorThread::Run, threads_[i].get(), &next_leaves, &current_leaf,
            visited_for_endgame));
      }

      Stats stats;
      for (int i = 0; i < num_threads; ++i) {
        futures[i].get();
        stats_.Merge(threads_[i]->GetStats());
      }
      stats_.AddTimeDeepen(time_deepen.Get());
      assert (GetFirstPosition()->NThreadsWorking() == 0);
      just_started_ = false;
      UpdateWeakLowerUpper();
    }
  }

  std::vector<TreeNodeLeafToUpdate> GetNextPosition() {
    auto first_position = GetFirstPosition();
    std::vector<TreeNodeLeafToUpdate> result;
    if (threads_.size() > 1) {
      result = TreeNodeLeafToUpdate::BestDescendants(first_position);
    } else {
      auto best_descendant =
          TreeNodeLeafToUpdate::BestDescendant(first_position);
      assert(best_descendant);
      result = {*best_descendant};
    }

    assert (!result.empty());
    return result;
  }

  bool CheckFinished() {
    auto first_position = GetFirstPosition();
    if (status_ == KILLING) {
      status_ = KILLED;
      return true;
    }
    if (status_ == KILLED) {
      return true;
    }
    if (first_position->IsSolvedIn(lower_, upper_) ||
        (first_position->IsPartiallySolved(lower_, upper_) && approx_)) {
      status_ = SOLVED;
      return true;
    }
    if (first_position->GetNVisited() > max_n_visited_ && !just_started_) {
      status_ = STOPPED_POSITIONS;
      return true;
    }
    if (tree_node_supplier_->NumTreeNodes() > kDerivativeEvaluatorSize - 60 * threads_.size() * kMaxChildrenUpdates) {
      status_ = STOPPED_TREE_POSITIONS;
      return true;
    }
    if (elapsed_time_.Get() > max_time_) {
      status_ = STOPPED_TIME;
      return true;
    }
    return false;
  }

  void UpdateWeakLowerUpper() {
    auto first_position = GetFirstPosition();
    while (first_position->UpdateWeakLowerUpper(lower_, upper_)) {
      last_update_weak_ = NVisited();
    }
  }

};

#endif  // OTHELLOSENSEI_EVALUATEDERIVATIVE_EVALUATOR_DERIVATIVE_H