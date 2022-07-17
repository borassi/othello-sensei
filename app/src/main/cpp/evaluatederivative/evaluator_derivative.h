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

#include <chrono>
#include <functional>
#include <limits>
#include <optional>
#include <stdatomic.h>
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
    tree_node_index_ = new atomic_int[kHashMapSize];
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
//    if (old_index >= num_tree_nodes_) {
      tree_node_index_[hash] = node_id;
//    }
    return &node;
  }

 private:
  atomic_int num_tree_nodes_;
  TreeNode* tree_nodes_;
  atomic_int* tree_node_index_;
};

class EvaluatorDerivative {
 public:
  EvaluatorDerivative(TreeNodeSupplier* tree_node_supplier, HashMap* hash_map, EvaluatorFactory evaluator_depth_one, int n_threads, u_int8_t index = 0) :
      tree_node_supplier_(tree_node_supplier),
      n_threads_(n_threads),
      next_evaluators_(),
      index_(index),
      first_position_(nullptr) {
    for (int i = 0; i < n_threads_; ++i) {
      next_evaluators_.push_back(EvaluatorAlphaBeta(hash_map, evaluator_depth_one));
    }
  }
//  EvaluatorDerivative(int max_size, int array_size, HashMap hashMap) {
//    this(maxSize, arraySize, hashMap, () -> new EvaluatorAlphaBeta(hashMap));
//  }
//
//  EvaluatorDerivative(int max_size, int array_size) {
//    this(max_size, array_size, new HashMap());
//  }
//
//  public EvaluatorMCTS(int maxSize, int arraySize, HashMap hashMap,
//                       Supplier<EvaluatorInterface> evaluatorMidgameBuilder) {
//    super(maxSize, arraySize);
//    this.hashMap = hashMap;
//    threads = new EvaluatorThread[Constants.MAX_PARALLEL_TASKS];
//    for (int i = 0; i < threads.length; ++i) {
//      threads[i] = new EvaluatorThread(evaluatorMidgameBuilder.get());
//    }
//  }

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

    EvalLarge eval = next_evaluators_[0].Evaluate(player, opponent, 4, kMinEvalLarge, kMaxEvalLarge);
    first_position_ = tree_node_supplier_->AddTreeNode(player, opponent, 0, index_);
    first_position_->SetLeaf(eval, 4, next_evaluators_[0].GetNVisited());
    last_update_weak_ = 0;
    visited_for_endgame_ = kVisitedEndgameStart;
    visited_midgame_ = 0;
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

  NVisited GetNVisitedMidgame() { return visited_midgame_; }

  float AverageBatchSize() { return sum_batch_sizes_ / num_batches_; }

 private:
  int n_threads_;
  int visited_for_endgame_;
  NVisited visited_midgame_;
  NVisited max_n_visited_;
  double start_time_;
  double max_time_;
  NVisited last_update_weak_ = 0;
  Eval lower_ = -63;
  Eval upper_ = 63;
  Status status_ = SOLVED;
  std::vector<EvaluatorAlphaBeta> next_evaluators_;
  ElapsedTime elapsed_time_;
  bool just_started_;
  bool approx_;
  TreeNodeSupplier* tree_node_supplier_;
  TreeNode* first_position_;
  u_int8_t index_;
  float num_batches_;
  float sum_batch_sizes_;

  void Run() {
    std::vector<LeafToUpdate> next_leaves;
    NVisited n_visited;
    TreeNode* first_position = GetFirstPosition();
    while (!CheckFinished()) {
      next_leaves = GetNextPosition();
      num_batches_++;
      sum_batch_sizes_ += next_leaves.size();
      for (const auto& next_leaf : next_leaves) {
        TreeNode* leaf = next_leaf.leaf;
        assert(leaf->IsLeaf());
        if (leaf != first_position && leaf->ToBeSolved(next_leaf.alpha, next_leaf.beta, visited_for_endgame_)) {
          n_visited = SolvePosition(next_leaf);
        } else {
          n_visited = AddChildren(next_leaf);
        }
        for (TreeNode* parent : next_leaf.parents) {
          parent->AddDescendants(n_visited);
          parent->DecreaseNThreadsWorking();
        }
      }
      assert (first_position->NThreadsWorking() == 0);
      just_started_ = false;
    }
  }

  std::vector<LeafToUpdate> GetNextPosition() {
    std::vector<LeafToUpdate> result;
    UpdateWeakLowerUpper();
    if (CheckFinished()) {
      return {};
    }
    result = GetFirstPosition()->BestDescendants();
    assert (!result.empty());
    return result;
  }

  NVisited AddChildren(const LeafToUpdate& leaf) {
    TreeNode* node = leaf.leaf;
    assert(node->IsLeaf());
    int depth = node->NEmpties() < 24 ? (node->NEmpties() < 22 ? 2 : 3) : 4;
    NVisited n_visited = 0;
    BitPattern player = node->Player();
    BitPattern opponent = node->Opponent();

    auto moves = GetAllMovesWithPass(player, opponent);

    if (moves.empty()) {
      int final_eval = GetEvaluationGameOver(player, opponent);
      node->SetSolved(final_eval);
      node->UpdateFathers();
      visited_midgame_++;
      return 1;
    }

    std::pair<EvalLarge, NVisited> children_eval[moves.size()];
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
      int eval = next_evaluators_[0].Evaluate(new_player, new_opponent, depth, kMinEvalLarge, kMaxEvalLarge);
      auto cur_visited = next_evaluators_[0].GetNVisited();
      n_visited += cur_visited;
      child->SetWeakLowerUpper(-leaf.weak_upper, -leaf.weak_lower);
      child->SetLeaf(eval, depth, cur_visited);
    }
    node->SetChildren(children);
    node->UpdateFathers();
    visited_midgame_ += n_visited;
    return n_visited;
  }

  NVisited SolvePosition(const LeafToUpdate& leaf) {
    TreeNode* node = leaf.leaf;
    assert(node->IsLeaf());
    EvalLarge alpha = EvalToEvalLarge(leaf.alpha);
    EvalLarge beta = EvalToEvalLarge(leaf.beta);
    EvalLarge eval = next_evaluators_[0].Evaluate(
        node->Player(), node->Opponent(), node->NEmpties(), alpha, beta, 300000);
    NVisited seen_positions = next_evaluators_[0].GetNVisited() + 1;
    visited_for_endgame_ = std::min(50000, std::max(1000, visited_for_endgame_ - ((int) seen_positions - kVisitedEndgameGoal) / 30));
//    if (rand() % 100 == 0) {
//      std::cout << visited_for_endgame_ << "\n";
//    }
    if (eval == kLessThenMinEvalLarge) {
      return seen_positions + AddChildren(leaf);
    }
    node->SetWeakLowerUpper(leaf.weak_lower, leaf.weak_upper);

    node->DecreaseNThreadsWorking();
    if (eval < beta) {
      node->SetUpper(eval);
    }
    if (eval > alpha) {
      node->SetLower(eval);
    }
    node->IncreaseNThreadsWorking();
//    StoredBoard.proofNumberForAlphaBeta.addAndGet(
//        (int) (Constants.PROOF_NUMBER_GOAL_FOR_MIDGAME - seenPositions) / 20);
    node->UpdateFathers();
    return seen_positions;
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
//     synchronized (firstPosition) {
    auto first_position = GetFirstPosition();
//    if (NVisited() < this->last_update_weak_ * 1.3) {
//      if (first_position->ProbGreaterEqual(first_position->WeakLower())
//          > kProbIncreaseWeakEval &&
//          first_position->ProbGreaterEqual(first_position->WeakUpper())
//          < 1 - kProbIncreaseWeakEval) {
//        return;
//      }
//    }
//     }
    while (first_position->UpdateWeakLowerUpper()) {
      last_update_weak_ = NVisited();
    }
  }

//   double bestStepsUntilEnd = 0;
//   private final HashMap hashMap;
//   bool justStarted = false;

//   private final EvaluatorThread[] threads;
//
//   private class EvaluatorThread implements Runnable {
//     private final EvaluatorInterface nextEvaluator;
//
//     private EvaluatorThread(EvaluatorInterface nextEvaluator) {
//       this.nextEvaluator = nextEvaluator;
//     }
//
//     class PartialEval {
//       int eval;
//       long nVisited;
//       public PartialEval(int eval, long nVisited) {
//         this.eval = eval;
//         this.nVisited = nVisited;
//       }
//     }
//     public long addChildren(StoredBoardBestDescendant position) {
//       StoredBoard father = position.eval.getStoredBoard();
//       int depth = father.nEmpties < 22 ? 2 : 4;
//       int evalGoal = position.eval.evalGoal;
//       long nVisited = 0;
//       assert father.isLeaf();
//
//       long[] moves = GetMovesCache.getAllMoves(father.getPlayer(), father.getOpponent());
//
//       if (moves == null) {
//         int finalEval = BitPattern.getEvaluationGameOver(father.getPlayer(), father.getOpponent());
//         father.setSolved(finalEval);
//         return 1;
//       }
//
//       PartialEval[] childrenEval = new PartialEval[moves.length];
//       StoredBoard[] children = new StoredBoard[moves.length];
//       for (int i = 0; i < moves.length; ++i) {
//         long flip = moves[i];
//         long newPlayer = father.getOpponent() & ~flip;
//         long newOpponent = father.getPlayer() | flip;
//         StoredBoard child = getOrAdd(newPlayer, newOpponent, (short) (father.depth + 1));
//         children[i] = child;
//         if (child.leafEval == -6500) {
//           int eval = nextEvaluator.evaluate(child.getPlayer(), child.getOpponent(), depth, -6400, 6400);
//           childrenEval[i] = new PartialEval(Math.min(6400, Math.max(-6400, eval)), nextEvaluator.getNVisited());
//         }
//       }
//       synchronized (father) {
//         for (int i = 0; i < children.length; ++i) {
//           StoredBoard child = children[i];
//           synchronized (child) {
//             PartialEval childEval = childrenEval[i];
//             if (childEval != null) {
//               nVisited += childEval.nVisited;
//               if (child.leafEval == -6500) {
//                 child.setWeakLowerUpper((short) Math.min(-father.weakUpper, -evalGoal), (short) Math.max(-father.weakLower, -evalGoal));
//                 child.getEvaluation(-evalGoal).addDescendants(childEval.nVisited + 1);
//                 child.setLeaf((short) childEval.eval, (short) 4);
//               }
//             }
//             child.addFather(father);
//           }
//         }
//         father.children = children;
//         assert father.isLowerUpperOK();
//       }
//       father.updateFathers();
//       return nVisited;
//     }
//
//     public long solvePosition(StoredBoardBestDescendant position) {
//       StoredBoard board = position.eval.getStoredBoard();
//       assert board.isLeaf();
//       int alpha = position.alpha;
//       int beta = position.beta;
//       int eval = nextEvaluator.evaluate(
//           board.getPlayer(), board.getOpponent(), board.nEmpties, alpha, beta);
//       if (eval < beta) {
//         board.setUpper(eval);
//       }
//       if (eval > alpha) {
//         board.setLower(eval);
//       }
//       long seenPositions = nextEvaluator.getNVisited() + 1;
//       StoredBoard.proofNumberForAlphaBeta.addAndGet((int) (Constants.PROOF_NUMBER_GOAL_FOR_MIDGAME - seenPositions) / 20);
//       board.updateFathers();
//       return seenPositions;
//     }
//
//     public long deepenPosition(StoredBoardBestDescendant position) {
//       StoredBoard board = position.eval.getStoredBoard();
//       int curEval = nextEvaluator.evaluate(
//             board.getPlayer(), board.getOpponent(), 2, -6400, 6400);
//       int d;
//       long seenPositions = 0;
//       for (d = 4; seenPositions < board.getDescendants() * 2; d += 2) {
//         if (board.nEmpties - d < Constants.EMPTIES_FOR_FORCED_MIDGAME - 2) {
//           seenPositions += solvePosition(position);
//           return seenPositions;
//         }
//         curEval = nextEvaluator.evaluate(
//             board.getPlayer(), board.getOpponent(), d, -6400, 6400);
//         seenPositions += nextEvaluator.getNVisited();
//       }
//       board.setLeaf((short) curEval, (short) d);
//       board.updateFathers();
//       return seenPositions;
//     }
//

//
//
//   private double stepsUntilEnd() {
//     return firstPosition.stepsUntilEnd();
//   }
//
//
//   protected StoredBoardBestDescendant getNextPosition() {
//     if (Constants.ASSERT_EXTENDED) {
//       assertIsAllOKRecursive(firstPosition);
//     }
//
//     StoredBoardBestDescendant result;
//     while (true) {
// //      System.out.println(weakLower + " " + weakUpper + " " + firstPosition.weakLower + " " + firstPosition.weakUpper);
//       updateWeakLowerUpper();
//       if (checkFinished()) {
//         return null;
//       }
// //      System.out.println("  " + weakLower + " " + weakUpper + " " + firstPosition.weakLower + " " + firstPosition.weakUpper);
//       result = firstPosition.bestDescendant();
//       if (result != null) {
//         break;
//       }
//       try {
//         Thread.sleep(0, 100);
//       } catch (InterruptedException e) {
//         e.printStackTrace();
//       }
//     }
//     assert result.eval.getStoredBoard().isLeaf();
//     return result;
//   }
//
//   public synchronized Status getStatus() {
//     return status;
//   }
//
//   public boolean isSolved() {
//     if (Constants.APPROX_ONLY) {
//       if (firstPosition.isPartiallySolved()) {
//         return true;
//       }
//     }
//     return this.firstPosition.isSolved() || this.firstPosition.getUpper() <= lower || this.firstPosition.getLower() >= upper;
//   }
//
//   public short evaluatePosition(Board board) {
//     return evaluatePosition(board, -6300, 6300, Long.MAX_VALUE, Integer.MAX_VALUE);
//   }
//
//   public short evaluatePosition(Board board, long maxNVisited, int maxTimeMillis) {
//     return evaluatePosition(board, -6300, 6300, maxNVisited, maxTimeMillis);
//   }
//
//   ExecutorService executor = Executors.newFixedThreadPool(Constants.MAX_PARALLEL_TASKS);
//
//
//   public StoredBoard getFirstPosition() {
//     return firstPosition;
//   }
//
//   public void assertIsAllOKRecursive(StoredBoard board) {
//     assertIsOK(board);
//     if (board.isLeaf()) {
//       return;
//     }
//     for (StoredBoard child : board.getChildren()) {
//       assertIsAllOKRecursive(child);
//     }
//   }
//   public void assertIsOK(StoredBoard board) {
//     if (board.isLeaf()) {
//       return;
//     }
//     assert board.areChildrenOK();
//   }
//
//   public void addChildren(StoredBoardBestDescendant position) {
// //    position.eval.getStoredBoard().setBusy(-6300, 6300);
//     threads[0].addChildren(position);
//     finalizePosition(position, 0);
//   }
//
//   public void addChildren(StoredBoard board) {
//     addChildren(board.bestDescendant());
//   }
};