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

#ifndef OTHELLOSENSEI_EVALUATEDERIVATIVE_EVALUATOR_DERIVATIVE_H
#define OTHELLOSENSEI_EVALUATEDERIVATIVE_EVALUATOR_DERIVATIVE_H

#if ANDROID
#include <android/log.h>
#endif
#include <atomic>
#include <chrono>
#include <climits>
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
constexpr float kNextPositionFailGoal = 0.05;

using namespace std::chrono_literals;

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
  TreeNodeSupplier() : first_valid_index_(1), num_nodes_(0), tree_node_index_(kHashMapSize) {
    tree_nodes_ = new TreeNode[kDerivativeEvaluatorSize];
    FullResetHashMap();
  }
  ~TreeNodeSupplier() {
    delete[] tree_nodes_;
  }
  const TreeNode* const Get(const Board& b, u_int8_t evaluator_index) {
    return Get(b.Player(), b.Opponent(), evaluator_index);
  }

  TreeNode* Get(BitPattern player, BitPattern opponent, u_int8_t evaluator_index) {
    for (int hash = HashNode(player, opponent, evaluator_index);
         true;
         hash = (hash + 1) % tree_node_index_.size()) {
      uint32_t index = tree_node_index_[hash];
      if (!IsValid(index)) {
        return nullptr;
      }
      assert(index - first_valid_index_ >= 0 && index - first_valid_index_ < num_nodes_);
      TreeNode& node = tree_nodes_[index - first_valid_index_];
      if (node.Player() == player && node.Opponent() == opponent && node.Evaluator() == evaluator_index) {
        return &node;
      }
    }
  }

  void Reset() {
    assert(num_nodes_ <= kDerivativeEvaluatorSize);
    first_valid_index_ += num_nodes_;
    if (first_valid_index_ >= UINT32_MAX - kDerivativeEvaluatorSize - 1) {
      FullResetHashMap();
    }
    num_nodes_ = 0;
  }

  int NumTreeNodes() {
    return num_nodes_;
  }

  TreeNode* AddTreeNode(
      BitPattern player, BitPattern opponent, Square depth,
      EvalLarge weak_eval, Square eval_depth, EvaluatorDerivative* evaluator) {
    bool newly_inserted;
    return AddTreeNode(player, opponent, depth, weak_eval, eval_depth, evaluator, &newly_inserted);
  }
  TreeNode* AddTreeNode(
      BitPattern player, BitPattern opponent, Square depth,
      EvalLarge leaf_eval,
      Square eval_depth, EvaluatorDerivative* evaluator, bool* newly_inserted);

 private:
  TreeNode* tree_nodes_;
  std::vector<std::atomic_uint32_t> tree_node_index_;
  std::atomic_uint32_t num_nodes_;
  uint32_t first_valid_index_;

  void FullResetHashMap() {
    for (int i = 0; i < tree_node_index_.size(); ++i) {
      tree_node_index_[i] = 0;
    }
    first_valid_index_ = 1;
  }

  bool IsValid(uint32_t index) {
    return index >= first_valid_index_ && index < first_valid_index_ + num_nodes_;
  }

  void AddToHashMap(BitPattern player, BitPattern opponent, u_int8_t evaluator_index, uint32_t node_id) {
    assert(first_valid_index_ <= UINT32_MAX - node_id);
    int hash = HashNode(player, opponent, evaluator_index);
    uint32_t next_node = first_valid_index_ + node_id;
    while (IsValid(next_node)) {
      next_node = tree_node_index_[hash].exchange(next_node);
      hash = (hash + 1) % tree_node_index_.size();
    }
  }

  uint32_t HashNode(BitPattern player, BitPattern opponent, u_int8_t evaluator) {
    uint32_t hash = (HashFull(player, opponent) ^ std::hash<u_int8_t>{}(evaluator)) % tree_node_index_.size();
    assert(hash >= 0 && hash < tree_node_index_.size());
    return hash;
  }
};

class EvaluatorDerivative;

class EvaluatorThread {
 public:
  EvaluatorThread(HashMap* hash_map, EvaluatorFactory evaluator_depth_one,
                  EvaluatorDerivative* evaluator) :
      evaluator_alpha_beta_(hash_map, evaluator_depth_one),
      evaluator_depth_one_(evaluator_depth_one()),
      evaluator_(evaluator) {}

  void Run();

  const Stats& GetStats() const { return stats_; }

  NVisited AddChildren(const TreeNodeLeafToUpdate& leaf);

 private:
  EvaluatorAlphaBeta evaluator_alpha_beta_;
  std::unique_ptr<EvaluatorDepthOneBase> evaluator_depth_one_;
  Stats stats_;
  EvaluatorDerivative* evaluator_;
  ElapsedTime t;

  NVisited SolvePosition(const TreeNodeLeafToUpdate& leaf, int max_proof);
};

class EvaluatorDerivative {
 public:
  EvaluatorDerivative(TreeNodeSupplier* tree_node_supplier, HashMap* hash_map, EvaluatorFactory evaluator_depth_one, int n_threads, u_int8_t index = 0) :
      threads_(),
      tree_node_supplier_(tree_node_supplier),
      first_position_(nullptr),
      index_(index) {
    for (int i = 0; i < n_threads; ++i) {
      threads_.push_back(std::make_shared<EvaluatorThread>(hash_map, evaluator_depth_one, this));
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
    max_time_ = max_time;
    elapsed_time_ = ElapsedTime();
    start_visited_ = GetFirstPosition()->GetNVisited();
    max_n_visited_ = start_visited_ + max_n_visited;
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
    stats_.Reset();
    approx_ = approx;
    lower_ = lower;
    upper_ = upper;
    weak_lower_ = lower_;
    weak_upper_ = upper_;
    n_thread_multiplier_ = 1000;
    first_position_ = tree_node_supplier_->AddTreeNode(player, opponent, 0, 0, 1, this);
//    first_position_->SetLeafIfInvalid(0, 1, *this);
    auto leaf = TreeNodeLeafToUpdate::BestDescendant(first_position_, NThreadMultiplier());
    assert(leaf);
    leaf->Finalize(threads_[0]->AddChildren(*leaf));
    best_advancement_ = 0;
    is_updating_weak_lower_upper_.clear();
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

  u_int8_t Index() const { return index_; }

  std::pair<Eval, Eval> GetWeakLowerUpper(Square depth) const {
    if (depth & 1) {
      return std::make_pair(-weak_upper_, -weak_lower_);
    } else {
      return std::make_pair(weak_lower_.load(), weak_upper_.load());
    }
  }

 private:
  friend class EvaluatorThread;
  Stats stats_;
  NVisited max_n_visited_;
  double max_time_;
  Eval lower_;
  Eval upper_;
  std::atomic_int8_t weak_lower_;
  std::atomic_int8_t weak_upper_;
  Status status_ = SOLVED;
  std::vector<std::shared_ptr<EvaluatorThread>> threads_;
  ElapsedTime elapsed_time_;
  NVisited start_visited_;
  std::atomic_bool just_started_;
  bool approx_;
  TreeNodeSupplier* tree_node_supplier_;
  TreeNode* first_position_;
  u_int8_t index_;
  std::atomic_int n_thread_multiplier_;
  std::atomic_flag is_updating_weak_lower_upper_;
  double best_advancement_;

  int VisitedForEndgame() {
    float done = first_position_->GetNVisited();
    float done_tree_nodes = tree_node_supplier_->NumTreeNodes();
    float solve_probability = first_position_->SolveProbability(lower_, upper_);
    float goal = std::max(300.0F, std::min(1800.0F, 120 / pow(solve_probability, 0.5F)));
    float result = 5000 - (done - done_tree_nodes * goal) / 40;

    return std::max(2000, std::min(100000, (int) result));
  }

  void Run() {
    std::vector<std::future<void>> futures;
    futures.push_back(std::async(std::launch::deferred, &EvaluatorThread::Run, threads_[0].get()));
    for (int i = 1; i < threads_.size(); ++i) {
      std::this_thread::sleep_for(1ms);
      futures.push_back(std::async(std::launch::async, &EvaluatorThread::Run, threads_[i].get()));
    }
    for (int i = 0; i < threads_.size(); ++i) {
      futures[i].get();
      stats_.Merge(threads_[i]->GetStats());
    }
  }

  bool CheckFinished(TreeNode* first_position) {
    auto advancement = first_position->Advancement(lower_, upper_);
    best_advancement_ = std::min(best_advancement_, advancement);
    bool good_stop = advancement <= best_advancement_;
    if (status_ == KILLING) {
      status_ = KILLED;
      return true;
    }
    if (status_ == KILLED) {
      return true;
    }
    auto time = elapsed_time_.Get();
    if (time > max_time_ || time > 0.8 * max_time_ && good_stop) {
      status_ = STOPPED_TIME;
      return true;
    }
    if (first_position->IsSolved(lower_, upper_, approx_)) {
      status_ = SOLVED;
      return true;
    }
    NVisited visited_goal = max_n_visited_ - start_visited_;
    NVisited visited_actual = first_position->GetNVisited() - start_visited_;
    if (!just_started_ && (
        visited_actual > visited_goal ||
        visited_actual > 0.8 * visited_goal && good_stop)) {
      status_ = STOPPED_POSITIONS;
      return true;
    }
    if (tree_node_supplier_->NumTreeNodes() > kDerivativeEvaluatorSize - 100000) {
      status_ = STOPPED_TREE_POSITIONS;
      return true;
    }
    return false;
  }

  void UpdateWeakLowerUpper(const TreeNode& first_position) {
    if (is_updating_weak_lower_upper_.test_and_set()) {
      return;
    }
    assert((lower_ - kMinEval) % 2 == 1);
    assert((upper_ - kMinEval) % 2 == 1);
    assert(kMinEval <= lower_ && lower_ <= kMaxEval);
    assert(kMinEval <= upper_ && upper_ <= kMaxEval);
    assert(weak_lower_ >= lower_ && weak_upper_ <= upper_);
    auto [weak_lower, weak_upper, new_weak_lower, new_weak_upper] =
        first_position.ExpectedWeakLowerUpper();

    weak_lower_ = std::max(lower_, new_weak_lower);
    weak_upper_ = std::min(upper_, new_weak_upper);

    if (new_weak_lower < weak_lower || new_weak_upper > weak_upper) {
      first_position_->ExtendEval(weak_lower_, weak_upper_);
    }
    is_updating_weak_lower_upper_.clear();
  }

  bool IsWeakLowerUpperOK() {
    return
        first_position_->WeakLower() <= weak_lower_
        && weak_lower_ < weak_upper_
        && weak_upper_ <= first_position_->WeakUpper();
  }

  TreeNode* AddTreeNode(
      BitPattern player, BitPattern opponent, Square depth,
      EvalLarge leaf_eval, Square eval_depth, bool* newly_inserted) {
    return tree_node_supplier_->AddTreeNode(
        player, opponent, depth, leaf_eval, eval_depth,
        this, newly_inserted);
  }

  void UpdateNThreadMultiplierSuccess() {
    n_thread_multiplier_ -= 100;
    n_thread_multiplier_ = std::max(n_thread_multiplier_.load(), 1000);
  }

  void UpdateNThreadMultiplierFail() {
    n_thread_multiplier_ += 2000;
    n_thread_multiplier_ = std::min(n_thread_multiplier_.load(), 1000000);
  }

  float NThreadMultiplier() {
    return n_thread_multiplier_ / 1000.0;
  }
};

#endif  // OTHELLOSENSEI_EVALUATEDERIVATIVE_EVALUATOR_DERIVATIVE_H