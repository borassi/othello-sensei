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
    for (int i = 0; i < tree_node_index_.size(); ++i) {
      tree_node_index_[i] = 0;
    }
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
    first_valid_index_ += num_nodes_;
    if (first_valid_index_ >= UINT32_MAX - kDerivativeEvaluatorSize) {
      first_valid_index_ = 0;
    }
    num_nodes_ = 0;
  }

  int NumTreeNodes() {
    return num_nodes_;
  }

  TreeNode* AddTreeNode(BitPattern player, BitPattern opponent, Square depth, Eval weak_lower, Eval weak_upper, u_int8_t evaluator_index) {
    bool newly_inserted;
    return AddTreeNode(player, opponent, depth, weak_lower, weak_upper, evaluator_index, &newly_inserted);
  }
  TreeNode* AddTreeNode(
      BitPattern player, BitPattern opponent, Square depth,
      Eval weak_lower, Eval weak_upper,
      u_int8_t evaluator_index, bool* newly_inserted) {
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
    node.Reset(player, opponent, depth, weak_lower, weak_upper, evaluator_index);
    AddToHashMap(player, opponent, evaluator_index, node_id);
    *newly_inserted = true;
    return &node;
  }

 private:
  TreeNode* tree_nodes_;
  std::vector<std::atomic_uint32_t> tree_node_index_;
  std::atomic_uint32_t num_nodes_;
  uint32_t first_valid_index_;

  bool IsValid(uint32_t index) {
    return index >= first_valid_index_ && index < first_valid_index_ + num_nodes_;
  }

  void AddToHashMap(BitPattern player, BitPattern opponent, u_int8_t evaluator_index, uint32_t node_id) {
    assert(first_valid_index_ <= UINT32_MAX - node_id);
    int hash = HashNode(player, opponent, evaluator_index);
    uint32_t shifted_node_id = first_valid_index_ + node_id;
    uint32_t next_node = shifted_node_id;
    while (IsValid(next_node)) {
      next_node = tree_node_index_[hash].exchange(next_node);
      hash = (hash + 1) % tree_node_index_.size();
    }
  }

  int HashNode(BitPattern player, BitPattern opponent, u_int8_t evaluator) {
    return Hash(player, opponent) ^ std::hash<u_int8_t>{}(evaluator);
  }
};

class EvaluatorDerivative;

class EvaluatorThread {
 public:
  EvaluatorThread(HashMap* hash_map, EvaluatorFactory evaluator_depth_one,
                  EvaluatorDerivative* evaluator,
                  uint8_t index) :
      evaluator_alpha_beta_(hash_map, evaluator_depth_one),
      evaluator_depth_one_(evaluator_depth_one()),
      evaluator_(evaluator) {}

  void Run();

  const Stats& GetStats() const { return stats_; }

 private:
  EvaluatorAlphaBeta evaluator_alpha_beta_;
  std::unique_ptr<EvaluatorDepthOneBase> evaluator_depth_one_;
  Stats stats_;
  EvaluatorDerivative* evaluator_;
  ElapsedTime t;

//  int ExpectedLoss(Evaluation root_eval, float original_prob) {
//    return root_eval.MaxLogDerivative()
//        - kCombineProb.log_derivative[(int) round(original_prob * 255)]
//        + kCombineProb.log_derivative[(int) round(root_eval.ProbGreaterEqual() * 255)]
//        - kCombineProb.log_derivative[(int) round((1 - original_prob) * 255)]
//        + kCombineProb.log_derivative[(int) round((1 - root_eval.ProbGreaterEqual()) * 255)];
//  }

//  NVisited AddManyChildren(const TreeNodeLeafToUpdate& leaf, int max_proof);

  NVisited AddChildren(const TreeNodeLeafToUpdate& leaf,
                       bool* transposition);

  NVisited SolvePosition(const TreeNodeLeafToUpdate& leaf,
                         bool* transposition, int max_proof);
};

class EvaluatorDerivative {
 public:
  EvaluatorDerivative(TreeNodeSupplier* tree_node_supplier, HashMap* hash_map, EvaluatorFactory evaluator_depth_one, int n_threads, u_int8_t index = 0) :
      threads_(),
      tree_node_supplier_(tree_node_supplier),
      first_position_(nullptr),
      index_(index) {
    for (int i = 0; i < n_threads; ++i) {
      threads_.push_back(std::make_shared<EvaluatorThread>(hash_map, evaluator_depth_one, this, index_));
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
    lower_ = lower;
    upper_ = upper;
    weak_lower_ = lower_;
    weak_upper_ = upper_;
    n_thread_multiplier_ = 1000;
    next_update_ = 1000;
    first_position_ = tree_node_supplier_->AddTreeNode(player, opponent, 0, weak_lower_, weak_upper_, index_);
    first_position_->SetLeaf(0, 4);
    stats_.Reset();
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

  Eval WeakLower() const { return first_position_->WeakLower(); }

 private:
  friend class EvaluatorThread;
  Stats stats_;
  NVisited max_n_visited_;
  double max_time_;
  Eval lower_ = -63;
  Eval upper_ = 63;
  Eval weak_lower_ = -63;
  Eval weak_upper_ = 63;
  Status status_ = SOLVED;
  std::vector<std::shared_ptr<EvaluatorThread>> threads_;
  ElapsedTime elapsed_time_;
  std::atomic_bool just_started_;
  bool approx_;
  TreeNodeSupplier* tree_node_supplier_;
  TreeNode* first_position_;
  u_int8_t index_;
  std::atomic_int n_thread_multiplier_;
  std::mutex mutex_;
  std::condition_variable condition_variable_;
  NVisited next_update_;

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
    std::vector<std::future<void>> futures;
    for (int i = 0; i < threads_.size(); ++i) {
      futures.push_back(std::async(
          i == 0 ? std::launch::deferred : std::launch::async,
          &EvaluatorThread::Run, threads_[i].get()));
      std::this_thread::sleep_for(1ms);
    }
    for (int i = 0; i < threads_.size(); ++i) {
      futures[i].get();
      stats_.Merge(threads_[i]->GetStats());
    }
  }

  bool CheckFinished() {
    if (status_ == KILLING) {
      status_ = KILLED;
      return true;
    }
    if (status_ == KILLED) {
      return true;
    }
    if (elapsed_time_.Get() > max_time_) {
      status_ = STOPPED_TIME;
      return true;
    }
    auto first_position = GetFirstPosition();
    if (first_position->IsSolved(lower_, upper_, approx_)) {
      status_ = SOLVED;
      return true;
    }
    if (first_position->GetNVisited() > max_n_visited_ && !just_started_) {
      status_ = STOPPED_POSITIONS;
      return true;
    }
    if (tree_node_supplier_->NumTreeNodes() > kDerivativeEvaluatorSize - 20) {
      status_ = STOPPED_TREE_POSITIONS;
      return true;
    }
    return false;
  }

  bool NeedUpdateWeakLowerUpper() {
    NVisited n_visited = first_position_->GetNVisited();
    if (n_visited < next_update_) {
      return false;
    }
    next_update_ = (NVisited) (n_visited * 1.1);
    return true;
  }

  void UpdateWeakLowerUpper() {
    assert((lower_ - kMinEval) % 2 == 1);
    assert((upper_ - kMinEval) % 2 == 1);
    assert(kMinEval <= lower_ && lower_ <= kMaxEval);
    assert(kMinEval <= upper_ && upper_ <= kMaxEval);
    assert(weak_lower_ >= lower_ && weak_upper_ <= upper_);
    std::unique_lock<std::mutex> lock(mutex_); // TODO: This blocks everybody.
    if (!NeedUpdateWeakLowerUpper()) {
      return;
    }
    bool done_something = true;
    while (done_something) {
      TreeNodeSummarized n = GetFirstPosition()->ToTreeNodeSummarized(weak_lower_, weak_upper_);
      bool extend_lower = false;
      bool extend_upper = false;
      bool reduced = false;
      if (n.prob_weak_lower < 1 - kProbIncreaseWeakEval
          && n.weak_lower - 2 >= n.lower
          && n.weak_lower - 2 >= lower_) {
        extend_lower = true;
      }
      if (n.prob_weak_upper > kProbIncreaseWeakEval
          && weak_upper_ + 2 <= n.upper
          && weak_upper_ + 2 <= upper_) {
        extend_upper = true;
      }
      if (!extend_lower && !extend_upper && n.prob_weak_lower_next >= 1) {
        weak_lower_ += 2;
        assert(weak_lower_ <= weak_upper_);
        reduced = true;
      }
      if (!extend_lower && !extend_upper && n.prob_weak_upper_prev <= 0) {
        weak_upper_ -= 2;
        assert(weak_lower_ <= weak_upper_);
        reduced = true;
      }
      if (extend_lower) {
        weak_lower_ -= 2;
        first_position_->ExtendEval(weak_lower_);
      }
      if (extend_upper) {
        weak_upper_ += 2;
        first_position_->ExtendEval(weak_upper_);
      }
      done_something = extend_lower || extend_upper || reduced;
    }
  }

  std::pair<Eval, Eval> GetWeakLowerUpper(Square depth) {
    std::unique_lock<std::mutex> lock(mutex_);
    return std::make_pair(
        (depth & 1) == 0 ? weak_lower_ : -weak_upper_,
        (depth & 1) == 0 ? weak_upper_ : -weak_lower_);
  }

  void SetWeakLowerUpper(TreeNode* node) {
    auto weak_lower_upper = GetWeakLowerUpper(node->Depth());
    node->SetWeakLowerUpper(weak_lower_upper.first, weak_lower_upper.second);
  }

  TreeNode* AddTreeNode(BitPattern player, BitPattern opponent, Square depth, bool* newly_inserted) {
    auto weak_lower_upper = GetWeakLowerUpper(depth);
    return tree_node_supplier_->AddTreeNode(player, opponent, depth, weak_lower_upper.first, weak_lower_upper.second, index_, newly_inserted);
  }

  void UpdateNThreadMultiplierSuccess() {
//    std::lock_guard<std::mutex> lock(mutex_);
//    if (rand() % 100 == 0)
//      std::cout << "Success " << n_thread_multiplier_ << "\n";
    n_thread_multiplier_ -= 100;
    n_thread_multiplier_ = std::max(n_thread_multiplier_.load(), 1000);
  }

  void UpdateNThreadMultiplierFail() {
//    std::lock_guard<std::mutex> lock(mutex_);
//    if (rand() % 100 == 0)
//      std::cout << "Fail " << n_thread_multiplier_ << "\n";
    n_thread_multiplier_ += 2000;
    n_thread_multiplier_ = std::min(n_thread_multiplier_.load(), 1000000);
  }

  float NThreadMultiplier() {
    return n_thread_multiplier_ / 1000.0;
  }

  void Wait() {
    std::unique_lock<std::mutex> lock(mutex_);
    condition_variable_.wait_for(lock, 100ms, [] {return true;});
  }

  void NotifyAll() {
    condition_variable_.notify_all();
  }

  void Finalize(LeafToUpdate<TreeNode> leaf, NVisited n_visited) {
    leaf.Finalize(n_visited);
    just_started_ = false;
  }
};

#endif  // OTHELLOSENSEI_EVALUATEDERIVATIVE_EVALUATOR_DERIVATIVE_H