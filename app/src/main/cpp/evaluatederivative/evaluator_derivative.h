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
#include <optional>
#include <stdatomic.h>
#include <thread>
#include "tree_node.h"
#include "../constants.h"
#include "../evaluatealphabeta/evaluator_alpha_beta.h"
#include "../utils/misc.h"

constexpr bool kUseTranspositions = true;

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

class EvaluatorDerivative {
 public:
  EvaluatorDerivative(EvaluatorAlphaBeta* next_evaluator) :
      num_tree_nodes_(0),
      next_evaluator_(next_evaluator) {
    tree_nodes_ = new TreeNode[kDerivativeEvaluatorSize];
    tree_node_index_ = new atomic_int[kHashMapSize];
  }
  ~EvaluatorDerivative() {
    delete[] tree_nodes_;
    delete[] tree_node_index_;
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

  void Evaluate(
      BitPattern player, BitPattern opponent, Eval lower, Eval upper,
      NVisited max_n_visited, double max_time, bool approx = false) {
    assert((lower - kMinEval) % 2 == 1);
    assert((upper - kMinEval) % 2 == 1);
    assert(lower <= upper);
    TreeNode* first_position = GetFirstPosition();
    approx_ = approx;
    if (first_position == nullptr || first_position->Player() != player || first_position->Opponent() != opponent) {
      SetFirstPosition(player, opponent);
    }
    max_n_visited_ = max_n_visited;
    max_time_ = max_time;
    elapsed_time_ = ElapsedTime();
    just_started_ = true;
//    EvaluatorAlphaBeta.resetConstant();
//    synchronized (this) {
    status_ = RUNNING;
//    }
    lower_ = lower;
    upper_ = upper;
//    first_position.SetLeaf(0, 2, -63, 63, 1, 1);

    Run();
//    this.bestStepsUntilEnd = Double.POSITIVE_INFINITY;
//
//    this.maxNVisited = maxNVisited;
//    this.justStarted = true;
//    if (firstPosition.getPlayer() != board.getPlayer() ||
//        firstPosition.getOpponent() != board.getOpponent()) {
//      setFirstPosition(board.getPlayer(), board.getOpponent());
//    }
//    this.startTimeMillis = System.currentTimeMillis();
//    this.maxTimeMillis = maxTimeMillis;
//
//    Future<?>[] futures = new Future<?>[threads.length];
//    for (int i = 0; i < threads.length; ++i) {
//      futures[i] = executor.submit(threads[i]);
//    }
//    for (Future<?> future : futures) {
//      try {
//        future.get();
//      } catch (ExecutionException | InterruptedException e) {
//        e.printStackTrace();
//        status = Status.FAILED;
//      }
//    }
//    return (short) -firstPosition.getEval();
  }

  void Stop() {
    if (status_ == RUNNING || status_ == STOPPED_TIME) {
      status_ = KILLING;
    }
  }

  TreeNode* const GetFirstPosition() {
    if (num_tree_nodes_ == 0) {
      return nullptr;
    }
    return &tree_nodes_[0];
  }

  const TreeNode* const Get(const Board& b) {
    return Get(b.GetPlayer(), b.GetOpponent());
  }

  Status GetStatus() { return status_; }

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

  int NumTreeNodes() { return num_tree_nodes_; }
//  Eval Lower() { return lower_; }
//  Eval Upper() { return upper_; }

//  NVisited GetNStored() {
//    return this.size;
//  }

  void Reset() { num_tree_nodes_ = 0; }

 private:
  atomic_int num_tree_nodes_;
  NVisited max_n_visited_;
  double start_time_;
  double max_time_;
  NVisited last_update_weak_ = 0;
  Eval lower_ = -63;
  Eval upper_ = 63;
  Status status_ = SOLVED;
  TreeNode* tree_nodes_;
  atomic_int* tree_node_index_;
  EvaluatorAlphaBeta* next_evaluator_;
  ElapsedTime elapsed_time_;
  bool just_started_;
  bool approx_;

  void Run() {
    LeafToUpdate next_leaf;
    NVisited n_visited;
    TreeNode* first_position = GetFirstPosition();
    while (true) {
      next_leaf = GetNextPosition();
      TreeNode* leaf = next_leaf.leaf;
      if (next_leaf.leaf == nullptr) {
        break;
      }
//      firstPosition.setNewLowerUpper(board);
      assert(leaf->IsLeaf());
      if (leaf != first_position && leaf->ToBeSolved(next_leaf.eval_goal, num_tree_nodes_)) {
        n_visited = SolvePosition(next_leaf);
      } else {
        n_visited = AddChildren(next_leaf);
      }
//      else {
//        nVisited = deepenPosition(position);
//      }
      for (TreeNode* parent : next_leaf.parents) {
        parent->AddDescendants(n_visited);
      }
      just_started_ = false;
    }
  }
  void SetFirstPosition(BitPattern player, BitPattern opponent) {
//    Empty();
//    StoredBoard.proofNumberForAlphaBeta.set(0);
//    StoredBoard.avoidNextPosFailCoeff.set(8000);
    EvalLarge eval = next_evaluator_->Evaluate(player, opponent, 4, kMinEvalLarge, kMaxEvalLarge);
    num_tree_nodes_ = 0;
    TreeNode* first_position = AddTreeNode(player, opponent, 0);
    first_position->SetLeaf(eval, 4, next_evaluator_->GetNVisited(), 1);
    last_update_weak_ = 0;
    next_evaluator_->AddEpoch();
//    if (Constants.ASSERT_EXTENDED) {
//      assertIsAllOKRecursive(firstPosition);
//    }
//    assert firstPosition.isLowerUpperOK();
  }

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


  LeafToUpdate GetNextPosition() {
//     if (Constants.ASSERT_EXTENDED) {
//       assertIsAllOKRecursive(firstPosition);
//     }

    LeafToUpdate result;
    while (true) {
      //      System.out.println(weakLower + " " + weakUpper + " " + firstPosition.weakLower + " " + firstPosition.weakUpper);
      UpdateWeakLowerUpper();
      if (CheckFinished()) {
        return LeafToUpdate();
      }
      //      System.out.println("  " + weakLower + " " + weakUpper + " " + firstPosition.weakLower + " " + firstPosition.weakUpper);
      result = GetFirstPosition()->BestDescendant();
      if (result.leaf != nullptr) {
        break;
      }
      using namespace std::chrono_literals;
      std::this_thread::sleep_for(100ms);
    }
    assert(result.leaf->IsLeaf());
    return result;
  }

  NVisited AddChildren(const LeafToUpdate& leaf) {
    TreeNode* node = leaf.leaf;
    assert(node->IsLeaf());
    int depth = node->NEmpties() < 24 ? 2 : 4;
//    int evalGoal = position.eval.evalGoal;
    int eval_goal = leaf.eval_goal;
    NVisited n_visited = 0;
    BitPattern player = node->Player();
    BitPattern opponent = node->Opponent();

    auto moves = GetAllMovesWithPass(player, opponent);

    if (moves.empty()) {
      int final_eval = GetEvaluationGameOver(player, opponent);
      node->SetSolved(final_eval);
      node->UpdateFathers();
      return 1;
    }

    std::pair<EvalLarge, NVisited> children_eval[moves.size()];
    std::vector<TreeNode*> children(moves.size());
    for (int i = 0; i < moves.size(); ++i) {
      BitPattern flip = moves[i];
      BitPattern new_player = NewPlayer(flip, opponent);
      BitPattern new_opponent = NewOpponent(flip, player);
      TreeNode* child = AddTreeNode(new_player, new_opponent, node->Depth() + 1);
      children[i] = child;
      if (!child->IsValid()) {
       int eval = next_evaluator_->Evaluate(new_player, new_opponent, depth, kMinEvalLarge, kMaxEvalLarge);
       n_visited += next_evaluator_->GetNVisited();
       children_eval[i] = {eval, next_evaluator_->GetNVisited()};
      }
    }
//    synchronized (father) {
    for (int i = 0; i < moves.size(); ++i) {
      TreeNode* child = children[i];
//      synchronized (child) {
      if (!child->IsValid()) {
        child->SetWeakLowerUpper(-leaf.weak_upper, -leaf.weak_lower);
        child->SetLeaf(
            children_eval[i].first,
            depth,
            children_eval[i].second,
            -eval_goal);
      }
//     }
//      assert father.isLowerUpperOK();
    }
    node->SetChildren(children);
    node->UpdateFathers();
    return n_visited;
  }

  NVisited SolvePosition(const LeafToUpdate& leaf) {
    TreeNode* node = leaf.leaf;
    assert(node->IsLeaf());
    Eval alpha = leaf.alpha;
    Eval beta = leaf.beta;
    Eval eval = EvalLargeToEvalRound(next_evaluator_->Evaluate(
        node->Player(), node->Opponent(), node->NEmpties(), EvalToEvalLarge(alpha), EvalToEvalLarge(beta)));

    if (eval < beta) {
      node->SetUpper(eval);
    }
    if (eval > alpha) {
      node->SetLower(eval);
    }
    NVisited seen_positions = next_evaluator_->GetNVisited() + 1;
    node->SetWeakLowerUpper(leaf.weak_lower, leaf.weak_upper);
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
        || num_tree_nodes_ > kDerivativeEvaluatorSize - 60) {
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

  TreeNode* AddTreeNode(BitPattern player, BitPattern opponent, Square depth) {
    if (kUseTranspositions) {
      TreeNode* existing_node = Get(player, opponent);
      if (existing_node != nullptr) {
        return existing_node;
      }
    }
    int hash = Hash(player, opponent);
    int old_index = tree_node_index_[hash];
    if (old_index < num_tree_nodes_) {
      auto& old_tree_node = tree_nodes_[old_index];
      if (Hash(old_tree_node.Player(), old_tree_node.Opponent()) != hash) {
        // Node is outdated. There is a tiny probability that the hash remains
        // the same and this check fails, but it does not affect correctness,
        // only efficiency.
        old_index = num_tree_nodes_ + 1;
      }
    }
    int node_id = num_tree_nodes_++;
    TreeNode& node = tree_nodes_[node_id];
    node.Reset(player, opponent, depth);
    if (old_index >= num_tree_nodes_) {
      tree_node_index_[hash] = node_id;
    }
    return &node;
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