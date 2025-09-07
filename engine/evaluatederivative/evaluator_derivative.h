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
#include <thread>
#include <unordered_map>
#include <unordered_set>

#include "tree_node.h"
#include "../utils/constants.h"
#include "../evaluatealphabeta/evaluator_alpha_beta.h"
#include "../utils/misc.h"
#include "../evaluatedepthone/evaluator_depth_one_base.h"
#include "../evaluatedepthone/pattern_evaluator.h"

constexpr bool kUseTranspositions = true;

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

typedef std::tuple<BitPattern, BitPattern, uint8_t> NodeKey;

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
  TreeNodeSupplier() :
      tree_node_index_(kHashMapSize),
      num_nodes_(0),
      first_valid_index_(1) {
    tree_nodes_ = new TreeNode[kDerivativeEvaluatorSize];
    FullResetHashMap();
  }
  ~TreeNodeSupplier() {
    delete[] tree_nodes_;
  }
  std::unique_ptr<Node> Get(const Board& b, Square depth, uint8_t evaluator_index) const {
    return Get(b.Player(), b.Opponent(), depth, evaluator_index);
  }

  std::unique_ptr<Node> Get(BitPattern player, BitPattern opponent, Square depth, uint8_t evaluator_index) const {
    TreeNode* tree_node = Mutable(player, opponent, depth, evaluator_index);
    if (tree_node) {
      return std::make_unique<Node>(*tree_node);
    } else {
      return nullptr;
    }
  }

  TreeNode* Mutable(BitPattern player, BitPattern opponent, Square depth, uint8_t evaluator_index) const {
    return MutableInternal(player, opponent, depth, evaluator_index);
  }

  void Reset() {
    assert(num_nodes_ <= (unsigned) kDerivativeEvaluatorSize);
    first_valid_index_ += num_nodes_;
    if (first_valid_index_ >= UINT32_MAX - kDerivativeEvaluatorSize - 1) {
      FullResetHashMap();
    }
    num_nodes_ = 0;
  }

  int NumTreeNodes() {
    return num_nodes_;
  }

  std::pair<TreeNode*, bool> AddTreeNode(
      BitPattern player, BitPattern opponent, Square depth, uint8_t evaluator_index);

 private:
  TreeNode* tree_nodes_;
  std::vector<std::atomic_uint32_t> tree_node_index_;
  std::atomic_uint32_t num_nodes_;
  uint32_t first_valid_index_;
  Random random_;

  TreeNode* MutableInternal(BitPattern player, BitPattern opponent, Square depth, uint8_t evaluator_index) const {
    for (int hash = HashNode(player, opponent, depth, evaluator_index);
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
    assert(false);
  }

  void FullResetHashMap() {
    for (int i = 0; i < tree_node_index_.size(); ++i) {
      tree_node_index_[i] = 0;
    }
    first_valid_index_ = 1;
  }

  bool IsValid(uint32_t index) const {
    return index >= first_valid_index_ && index < first_valid_index_ + num_nodes_;
  }

  void AddToHashMap(BitPattern player, BitPattern opponent, Square depth, uint8_t evaluator_index, uint32_t node_id) {
    assert(first_valid_index_ <= UINT32_MAX - node_id);
    int hash = HashNode(player, opponent, depth, evaluator_index);
    uint32_t next_node = first_valid_index_ + node_id;
    while (IsValid(next_node)) {
      next_node = tree_node_index_[hash].exchange(next_node);
      hash = (hash + 1) % tree_node_index_.size();
    }
  }

  uint32_t HashNode(BitPattern player, BitPattern opponent, Square depth, uint8_t evaluator) const {
    uint32_t hash = (uint32_t) ((HashFull(player, opponent) ^ std::hash<int>{}((depth << 8) | evaluator)) % tree_node_index_.size());
    assert(hash >= 0 && hash < tree_node_index_.size());
    return hash;
  }
};

class EvaluatorDerivative;

class EvaluatorThread {
 public:
  EvaluatorThread(HashMap<kBitHashMap>* hash_map, EvaluatorFactory evaluator_depth_one,
                  EvaluatorDerivative* evaluator) :
      evaluator_alpha_beta_(hash_map, evaluator_depth_one),
      evaluator_depth_one_(evaluator_depth_one()),
      evaluator_(evaluator) {}

  void Run();

  void ResetStats() { stats_.Reset(); }

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
  EvaluatorDerivative(TreeNodeSupplier* tree_node_supplier, HashMap<kBitHashMap>* hash_map, EvaluatorFactory evaluator_depth_one, uint8_t index = 0) :
      threads_(),
      tree_node_supplier_(tree_node_supplier),
      first_position_(nullptr),
      index_(index),
      evaluator_depth_one_(evaluator_depth_one),
      hash_map_(hash_map) {
    threads_.push_back(std::make_unique<EvaluatorThread>(hash_map, evaluator_depth_one, this));
  }

  double Progress(float gap) {
    if (GetStatus() == SOLVED) {
      return -DBL_MAX;
    }
    TreeNode* board = first_position_;
    double evalEffect = -board->GetEval() / std::max(1.0F, gap);
    return evalEffect - log(board->GetNVisited()) / log(2);
  }

  double GetElapsedTime() const { return previous_elapsed_time; }

  void ContinueEvaluate(NVisited max_n_visited, double max_time, int n_threads) {
    max_time_ = max_time;
    elapsed_time_ = ElapsedTime();
    start_visited_ = first_position_->GetNVisited();
    max_n_visited_ = start_visited_ + max_n_visited;
    just_started_ = true;
    status_ = RUNNING;
    Run(n_threads);
    previous_elapsed_time += elapsed_time_.Get();
  }

  void Evaluate(
      BitPattern player, BitPattern opponent, Eval lower, Eval upper,
      NVisited max_n_visited, double max_time, int n_threads,
      bool approx = false) {
    assert((lower - kMinEval) % 2 == 1);
    assert((upper - kMinEval) % 2 == 1);
    assert(kMinEval <= lower && lower <= kMaxEval);
    assert(kMinEval <= upper && upper <= kMaxEval);
    assert(lower <= upper);
    for (int i = 0; i < threads_.size(); ++i) {
      threads_[i]->ResetStats();
    }
    approx_ = approx;
    lower_ = lower;
    upper_ = upper;
    weak_lower_ = lower_;
    weak_upper_ = upper_;
    num_tree_nodes_ = 0;
    previous_elapsed_time = 0;
    n_thread_multiplier_ = 10000L * n_threads * n_threads;
    auto [first_position, just_added] = AddTreeNode(player, opponent, 0);
    assert(just_added);
    first_position_ = first_position;
    first_position_->SetLeafEval(0, 1);
    first_position_->UpdateLeafWeakLowerUpper(weak_lower_, weak_upper_);
    auto leaf = TreeNodeLeafToUpdate::BestDescendant(first_position_, NThreadMultiplier(), kLessThenMinEval);
    assert(leaf);
    leaf->Finalize(threads_[0]->AddChildren(*leaf));
    best_advancement_ = 0;
    is_updating_weak_lower_upper_.clear();
    ContinueEvaluate(max_n_visited, max_time, n_threads);
  }

  void Stop() {
    if (status_ == RUNNING || status_ == STOPPED_TIME) {
      status_ = KILLING;
    }
  }

  std::unique_ptr<Node> GetFirstPosition() const {
    return first_position_ ? std::make_unique<Node>(*first_position_) : nullptr;
  }

  Status GetStatus() { return status_.load(); }

  Stats GetStats() const {
    Stats stats;
    for (int i = 0; i < threads_.size(); ++i) {
      stats.Merge(threads_[i]->GetStats());
    }
    return stats;
  }

  uint8_t Index() const { return index_; }

  std::pair<Eval, Eval> GetWeakLowerUpper(Square depth) const {
    if (depth & 1) {
      return std::make_pair(-weak_upper_, -weak_lower_);
    } else {
      return std::make_pair(weak_lower_.load(), weak_upper_.load());
    }
  }

  double MaxTime() const {
    return max_time_;
  }

 private:
  friend class EvaluatorThread;
  NVisited max_n_visited_;
  double max_time_;
  Eval lower_;
  Eval upper_;
  std::atomic_uint32_t num_tree_nodes_;
  std::atomic_int8_t weak_lower_;
  std::atomic_int8_t weak_upper_;
  std::atomic<Status> status_ = SOLVED;
  std::vector<std::unique_ptr<EvaluatorThread>> threads_;
  ElapsedTime elapsed_time_;
  NVisited start_visited_;
  std::atomic_bool just_started_;
  bool approx_;
  TreeNodeSupplier* tree_node_supplier_;
  TreeNode* first_position_;
  uint8_t index_;
  std::atomic_uint64_t n_thread_multiplier_;
  std::atomic_flag is_updating_weak_lower_upper_;
  double best_advancement_;
  EvaluatorFactory evaluator_depth_one_;
  HashMap<kBitHashMap>* hash_map_;
  double previous_elapsed_time;

  void Run(int n_threads) {
    std::vector<std::future<void>> futures;
    while (threads_.size() <= n_threads) {
      threads_.push_back(std::make_unique<EvaluatorThread>(hash_map_, evaluator_depth_one_, this));
    }
    futures.push_back(std::async(std::launch::deferred, &EvaluatorThread::Run, threads_[0].get()));
    for (int i = 1; i < n_threads; ++i) {
      std::this_thread::sleep_for(1ms);
      futures.push_back(std::async(std::launch::async, &EvaluatorThread::Run, threads_[i].get()));
    }
    for (int i = 0; i < n_threads; ++i) {
      futures[i].get();
    }
    UpdateWeakLowerUpper();
  }

  bool CheckFinished() {
    if (status_ == KILLING) {
      status_ = KILLED;
      return true;
    }
    if (status_ == KILLED) {
      return true;
    }
    if (first_position_->IsSolved(lower_, upper_, approx_)) {
      status_ = SOLVED;
      return true;
    }
    if (tree_node_supplier_->NumTreeNodes() > kDerivativeEvaluatorSize - 100000) {
      status_ = STOPPED_TREE_POSITIONS;
      return true;
    }
    auto time = elapsed_time_.Get();
    NVisited visited_goal = max_n_visited_ - start_visited_;
    NVisited visited_actual = first_position_->GetNVisited() - start_visited_;
    bool good_stop = false;
    if (time > 0.8 * max_time_ || visited_actual > 0.8 * visited_goal) {
      auto advancement = first_position_->Advancement(lower_, upper_);
      best_advancement_ = std::min(best_advancement_, advancement);
      good_stop = advancement <= best_advancement_;
    }
    if (time > max_time_ || (time > 0.8 * max_time_ && good_stop)) {
      status_ = STOPPED_TIME;
      return true;
    }
    if (!just_started_ && (
        visited_actual > visited_goal ||
        (visited_actual > 0.8 * visited_goal && good_stop))) {
      status_ = STOPPED_POSITIONS;
      return true;
    }
    return false;
  }

  void UpdateWeakLowerUpper() {
    if (is_updating_weak_lower_upper_.test_and_set()) {
      return;
    }
    assert((lower_ - kMinEval) % 2 == 1);
    assert((upper_ - kMinEval) % 2 == 1);
    assert(lower_ <= upper_);
    assert(kMinEval <= lower_ && lower_ <= kMaxEval);
    assert(kMinEval <= upper_ && upper_ <= kMaxEval);
    assert(weak_lower_ >= lower_ && weak_upper_ <= upper_);
    bool extended = true;
    while (extended) {
      auto [new_weak_lower, new_weak_upper] = first_position_->ExpectedWeakLowerUpper();

      // We need both max and min for the cases where we proved that first_position.eval > upper_ or
      // first_position.eval < lower_. In these cases, first_position->ExpectedWeakLowerUpper() is
      // upper_ + 1 or lower_ - 1.
      new_weak_lower = std::max(lower_, std::min(upper_, new_weak_lower));
      new_weak_upper = std::max(lower_, std::min(upper_, new_weak_upper));
      assert(new_weak_lower <= new_weak_upper);

      if (new_weak_lower < weak_lower_) {
        weak_lower_ = new_weak_lower;
      } else if (new_weak_upper > weak_upper_) {
        weak_upper_ = new_weak_upper;
      } else {
        weak_lower_ = new_weak_lower;
        weak_upper_ = new_weak_upper;
        extended = false;
      }
      if (extended) {
        first_position_->ExtendEval(weak_lower_, weak_upper_);
      }
      assert(first_position_->WeakLowerUpperContains(weak_lower_, weak_upper_));
    }
    is_updating_weak_lower_upper_.clear();
  }

  bool IsWeakLowerUpperOK() {
    return
        first_position_->WeakLower() <= weak_lower_
        && weak_lower_ < weak_upper_
        && weak_upper_ <= first_position_->WeakUpper();
  }

  std::pair<TreeNode*, bool> AddTreeNode(
      BitPattern player, BitPattern opponent, Square depth) {
    auto pair = tree_node_supplier_->AddTreeNode(player, opponent, depth, Index());
    // Add 1 if it's a new node, 0 otherwise.
    num_tree_nodes_ += pair.second;
    return pair;
  }

  void UpdateNThreadMultiplierSuccess() {
    if (n_thread_multiplier_ > 40000 * threads_.size()) {
      n_thread_multiplier_ -= 2000 * threads_.size();
    }
  }

  void UpdateNThreadMultiplierFail() {
    if (n_thread_multiplier_ < 40000000000UL - 80000 * threads_.size()) {
      n_thread_multiplier_ += 4000 * threads_.size();
    }
  }

  float NThreadMultiplier() {
    return n_thread_multiplier_ / 1000.0F;
  }
};

#endif  // OTHELLOSENSEI_EVALUATEDERIVATIVE_EVALUATOR_DERIVATIVE_H