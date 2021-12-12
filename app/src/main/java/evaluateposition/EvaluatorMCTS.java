// Copyright 2019 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package evaluateposition;

import com.sun.xml.internal.bind.v2.runtime.reflect.opt.Const;

import bitpattern.BitPattern;
import board.Board;
import board.GetMovesCache;
import constants.Constants;
import evaluatedepthone.PatternEvaluatorImproved;

import java.util.Random;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.ReentrantReadWriteLock;
import java.util.function.Supplier;
import java.util.logging.Level;
import java.util.logging.Logger;

public class EvaluatorMCTS extends HashMapVisitedPositions {
  PatternEvaluatorImproved evaluator;

  int threadWaitingForNextPos = 0;
  private final EvaluatorInterface nextEvaluator;
  private long maxNVisited;
  private long stopTimeMillis;
  private final HashMap hashMap;
  static final ReentrantReadWriteLock editNextPositionLock = new ReentrantReadWriteLock();
  static final ReentrantReadWriteLock.WriteLock nextPositionLock = editNextPositionLock.writeLock();
  static final ReentrantReadWriteLock.ReadLock editLock = editNextPositionLock.readLock();
  private boolean justStarted = false;
  private int weakLower = -6300;
  private int weakUpper = 6300;
  private long lastUpdateWeak = 0;

  private final Condition isNextPositionAvailable = nextPositionLock.newCondition();
  
  int lower = -6300;
  int upper = 6300;
  
  public enum Status {
    NONE,
    RUNNING,
    STOPPED_TIME,
    STOPPED_POSITIONS,
    SOLVED,
    KILLING,
    KILLED,
    FAILED,
  }
  private Status status = Status.KILLED;
  private EvaluatorThread[] threads;
  private double constant = 0;
  private int lastEvalGoal = -6500;

  private class EvaluatorThread implements Runnable {    
    private final EvaluatorInterface nextEvaluator;
    private final EvaluatorMCTS main;

    private EvaluatorThread(EvaluatorInterface nextEvaluator, EvaluatorMCTS main) {
      this.nextEvaluator = nextEvaluator;
      this.main = main;
    }

    class PartialEval {
      int eval;
      long nVisited;
      public PartialEval(int eval, long nVisited) {
        this.eval = eval;
        this.nVisited = nVisited;
      }
    }
    public long addChildren(StoredBoardBestDescendant position) {
      StoredBoard father = position.eval.getStoredBoard();
      int depth = father.nEmpties < 22 ? 2 : 4;
      int evalGoal = position.eval.evalGoal;
      long nVisited = 0;
      assert father.isLeaf();

      long[] moves = GetMovesCache.getAllMoves(father.getPlayer(), father.getOpponent());

      if (moves == null) {
        editLock.lock();
        int finalEval = BitPattern.getEvaluationGameOver(father.getPlayer(), father.getOpponent());
        father.setSolved(finalEval, getAlpha(father), getBeta(father));
        editLock.unlock();
        return 1;
      }

      PartialEval[] childrenEval = new PartialEval[moves.length];
      StoredBoard[] children = new StoredBoard[moves.length];
      for (int i = 0; i < moves.length; ++i) {
        long flip = moves[i];
        long newPlayer = father.getOpponent() & ~flip;
        long newOpponent = father.getPlayer() | flip;
        StoredBoard child = getOrAdd(newPlayer, newOpponent, father.depth + 1);
        children[i] = child;
        if (child.leafEval == -6500) {
          int eval = nextEvaluator.evaluate(child.getPlayer(), child.getOpponent(), depth, -6400, 6400);
          childrenEval[i] = new PartialEval(Math.min(6400, Math.max(-6400, eval)), nextEvaluator.getNVisited());
        }
      }
      editLock.lock();
      int alpha = getAlpha(father);
      int beta = getBeta(father);

      synchronized (father) {
        for (int i = 0; i < children.length; ++i) {
          StoredBoard child = children[i];
          synchronized (child) {
            PartialEval childEval = childrenEval[i];
            if (childEval != null) {
              nVisited += childEval.nVisited;
              if (child.leafEval == -6500) {
                child.getOrAddEvaluation(-evalGoal).addDescendants(childEval.nVisited + 1);
                child.setBusy(-beta, -alpha);
                child.setLeaf(childEval.eval, -beta, -alpha);
              }
            }
            child.addFather(father);
          }
        }
        father.children = children;
      }
      father.setFree(alpha, beta);
      editLock.unlock();
      return nVisited;
    }

    public long solvePosition(StoredBoardBestDescendant position) {
      StoredBoard board = position.eval.getStoredBoard();
      assert board.isLeaf();
      int alpha = position.alpha;
      int beta = position.beta;
      int eval = nextEvaluator.evaluate(
          board.getPlayer(), board.getOpponent(), board.nEmpties, alpha, beta);
      if (eval < beta) {
        board.setUpper(eval);
      }
      if (eval > alpha) {
        board.setLower(eval);
      }
      long seenPositions = nextEvaluator.getNVisited() + 1;
      synchronized(main) {
        constant = Math.max(0, constant + 0.05 * (10000 - seenPositions));
      }
      editLock.lock();
      board.setFree(getAlpha(board), getBeta(board));
      editLock.unlock();
      return seenPositions;
    }

    public long deepenPosition(StoredBoardBestDescendant position) {
      StoredBoard board = position.eval.getStoredBoard();
      int curEval = nextEvaluator.evaluate(
            board.getPlayer(), board.getOpponent(), 2, -6400, 6400);
      int d;
      long seenPositions = 0;
      for (d = 4; seenPositions < board.getDescendants() * 2; d += 2) {
        if (board.nEmpties - d < Constants.EMPTIES_FOR_FORCED_MIDGAME + 2) {
          return seenPositions + solvePosition(position);
        }
        curEval = nextEvaluator.evaluate(
            board.getPlayer(), board.getOpponent(), d, -6400, 6400);
        seenPositions += nextEvaluator.getNVisited();
      }
      editLock.lock();
      board.setLeaf(curEval, getAlpha(board), getBeta(board));
      editLock.unlock();
      return seenPositions;
    }

    @Override
    public void run() {
      StoredBoardBestDescendant position;
      long nVisited;
      while (true) {
        position = getNextPosition();
        if (position == null) {
          break;
        }
        StoredBoard board = position.eval.getStoredBoard();
        assert board != firstPosition;
        if (toBeSolved(position.eval) && board.isLeaf()) {
          nVisited = solvePosition(position);
        } else if (size < 0.6 * maxSize ||
                   (size < 0.8 * maxSize && board.depth < 9) ||
                   (size < maxSize && board.depth < 5)) {
          nVisited = addChildren(position);
        } else {
          nVisited = deepenPosition(position);
        }
        finalizePosition(position, nVisited);
      }
    }
  }

  public EvaluatorMCTS(int maxSize, int arraySize, HashMap hashMap) {
    this(maxSize, arraySize, hashMap, () -> new EvaluatorAlphaBeta(hashMap));
  }

  public EvaluatorMCTS(int maxSize, int arraySize) {
    this(maxSize, arraySize, new HashMap());
  }

  public EvaluatorMCTS(int maxSize, int arraySize, HashMap hashMap,
                       Supplier<EvaluatorInterface> evaluatorMidgameBuilder) {
    super(maxSize, arraySize);
    this.hashMap = hashMap;
    threads = new EvaluatorThread[Constants.MAX_PARALLEL_TASKS];
    for (int i = 0; i < threads.length; ++i) {
      threads[i] = new EvaluatorThread(evaluatorMidgameBuilder.get(), this);
    }
    this.firstPosition = StoredBoard.initialStoredBoard(new Board(0, 0));
    this.nextEvaluator = evaluatorMidgameBuilder.get();
  }
  
  public float getEval() {
    return firstPosition.getEval();
  }

  public long getNVisited() {
    return this.firstPosition.getDescendants();
  }
  
  public long getNStored() {
    return this.size;
  }

  public synchronized void stop() {
    if (status == Status.RUNNING) {
      status = Status.KILLING;
    }
  }

  public int nextPositionEvalGoal(StoredBoard board) {
    assert nextPositionLock.isHeldByCurrentThread();
    int lower = weakLower;
    int upper = weakUpper;
    StoredBoard.Evaluation bestEval = null;
    long bestValue = Long.MIN_VALUE;
    for (int eval = lower; eval <= upper; eval += 200) {
      StoredBoard.Evaluation curEval = board.getEvaluation(eval);
      if (curEval.proofNumber == 0 || curEval.disproofNumber == 0) {
        continue;
      }
      long curValue = 0;
      curValue += curEval.getProb() > Constants.ZERO_PERC_FOR_WEAK && curEval.getProb() < 1 - Constants.ZERO_PERC_FOR_WEAK ?
                      -StoredBoard.LOG_DERIVATIVE_MINUS_INF * 1000L : 0;
      curValue += curEval.maxLogDerivative * 1000L;
      curValue += curEval.evalGoal == lastEvalGoal ? 0 : 1;

      if (curValue > bestValue) {
        bestValue = curValue;
        bestEval = curEval;
      }
    }
    if (bestEval == null) {
//      System.out.println("Nothing good found." + lower + " " + upper + " " +
//                             board.getEvaluation(lower).proofNumber + " " + board.getEvaluation(upper).proofNumber);
      return -6500;
    }
    lastEvalGoal = bestEval.evalGoal;
    return bestEval.evalGoal;
  }

  protected void finalizePosition(StoredBoardBestDescendant position, long nVisited) {
    editLock.lock();
    try {
      position.updateDescendants(nVisited);
      justStarted = false;
    } finally {
      editLock.unlock();
    }

    if (threadWaitingForNextPos > 0) {
      nextPositionLock.lock();
      try {
        threadWaitingForNextPos = 0;
        isNextPositionAvailable.signalAll();
      } finally {
        nextPositionLock.unlock();
      }
    }
  }
  
  protected boolean checkFinished() {
    if (hashMap.size() > Constants.hashMapSize() / 2) {
      hashMap.reset();
    }

//    if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {
//      if (this.firstPosition.isSolved()) {
//        this.firstPosition.setIsFinished(true);
//      }
//    }

    if (status == Status.KILLING) {
      status = Status.KILLED;
      return true;
    }
    if (status == Status.KILLED) {
      return true;
    }
    if (this.firstPosition.getDescendants() > maxNVisited && !justStarted) {
      status = Status.STOPPED_POSITIONS;
      return true;
    }
    if (System.currentTimeMillis() > stopTimeMillis) {
      status = Status.STOPPED_TIME;
      return true;
    }
    if (isSolved()) {
      status = Status.SOLVED;
      return true;
    }
    return false;
  }

  private void updateWeakLowerUpper() {
    assert nextPositionLock.isHeldByCurrentThread();
    if (firstPosition.getDescendants() < this.lastUpdateWeak * 1.2 &&
        firstPosition.lower - 200 <= weakLower  &&
        firstPosition.upper + 200 >= weakUpper) {
      return;
    }
//    System.out.println(firstPosition.lower + " " + weakLower + " " + firstPosition.upper + " " + weakUpper);

    int newWeakLower = Math.max(-6300, firstPosition.getWeakLower() - 100);
    int newWeakUpper = Math.min(6300, firstPosition.getWeakUpper() + 100);

    if (newWeakLower < weakLower && firstPosition.getPercentileLower(0.2F) > weakLower) {
//      System.out.println("Not updating lower");
      newWeakLower = weakLower;
    }
    if (newWeakUpper > weakUpper && firstPosition.getPercentileUpper(0.2F) < weakUpper) {
//      System.out.println("Not updating upper");
      newWeakUpper = weakUpper;
    }
//    System.out.println(firstPosition.getDescendants() + ": (" + weakLower + ", " + weakUpper + ") -> (" + newWeakLower + ", " + newWeakUpper + ")");

    if (newWeakUpper > weakUpper) {
      //System.out.println("Updating descendants " + (weakUpper + 200) + " " + newWeakUpper);
      firstPosition.updateDescendantsRecursive(weakUpper + 200, newWeakUpper);
    }
    if (newWeakLower < weakLower) {
      //System.out.println("Updating descendants " + (newWeakLower) + " " + (weakLower - 200));
      firstPosition.updateDescendantsRecursive(newWeakLower, weakLower - 200);
    }
    lastUpdateWeak = this.getNVisited();
    weakLower = newWeakLower;
    weakUpper = newWeakUpper;
  }

  protected int getAlpha(StoredBoard board) {
    return board.depth % 2 == 0 ? weakLower : -weakUpper;
  }

  protected int getBeta(StoredBoard board) {
    return board.depth % 2 == 0 ? weakUpper : -weakLower;
  }

  protected StoredBoardBestDescendant getNextPosition() {
    StoredBoardBestDescendant result;
    nextPositionLock.lock();
//    System.out.println(Thread.currentThread().getName() + " locked!");
    if (checkFinished()) {
//      System.out.println(Thread.currentThread().getName() + " finished, unlock");
      nextPositionLock.unlock();
      return null;
    }
    if (Constants.ASSERT_EXTENDED) {
      assertIsAllOKRecursive(firstPosition);
    }
    updateWeakLowerUpper();
    int evalGoal = nextPositionEvalGoal(firstPosition);
    while (evalGoal == -6500) {
      threadWaitingForNextPos++;
      if (threadWaitingForNextPos >= Constants.MAX_PARALLEL_TASKS) {
        stop();
        isNextPositionAvailable.signalAll();
        nextPositionLock.unlock();
        assert false;
      }
      try {
        isNextPositionAvailable.await();
      } catch (InterruptedException e) {
        e.printStackTrace();
      }
      assert nextPositionLock.isHeldByCurrentThread();
      if (checkFinished()) {
        nextPositionLock.unlock();
        return null;
      }
      evalGoal = nextPositionEvalGoal(firstPosition);
    }
    result = StoredBoardBestDescendant.bestDescendant(firstPosition.getEvaluation(evalGoal), weakLower, weakUpper);
//    System.out.println(Thread.currentThread().getName() + " downgrade!");
    editLock.lock();
    nextPositionLock.unlock();
    StoredBoard board = result.eval.getStoredBoard();
    assert board.isLeaf();
    board.setBusy(getAlpha(board), getBeta(board));
//    System.out.println(Thread.currentThread().getName() + " unlock!");
    editLock.unlock();
    return result;
  }

  public short evaluatePosition(Board board) {
    return evaluatePosition(board, -6400, 6400, Long.MAX_VALUE, Long.MAX_VALUE, true);
  }

  public short evaluatePosition(Board board, long maxNVisited, long maxTimeMillis) {
    return evaluatePosition(board, -6400, 6400, maxNVisited, maxTimeMillis, true);
  }
  
  public Status getStatus() {
    return status;
  }

  public static int evalToBoundary(int eval) {
    if (eval >= 6300) {
      return 6300;
    }
    if (eval <= -6300) {
      return 6300;
    }
    return 200 * Math.round((eval + 100) / 200) - 100;
  }

  void setFirstPosition(Board b) {
    setFirstPosition(b.getPlayer(), b.getOpponent());
  }

  Random random = new Random();
  void setFirstPosition(long player, long opponent) {
    empty();
    random.setSeed(42);
    firstPosition = StoredBoard.initialStoredBoard(player, opponent);
    add(firstPosition);
    this.weakUpper = 6300;
    this.weakLower = -6300;
    nextPositionLock.lock();
    firstPosition.setBusy(-6300, 6300);
    firstPosition.setLeaf(0, -6300, 6300);
    StoredBoardBestDescendant firstPositionDescendant = StoredBoardBestDescendant.bestDescendant(
        firstPosition.getEvaluation(-100), -6300, 6300);
    firstPosition.setBusy(-6300, 6300);
    threads[0].addChildren(firstPositionDescendant);
    nextPositionLock.unlock();
    this.lastUpdateWeak = 0;
    if (Constants.ASSERT_EXTENDED) {
      assertIsAllOKRecursive(firstPosition);
    }
  }

  int temp = 0;
  public boolean isSolved() {
    if (Constants.APPROX_ONLY) {
      if (firstPosition.isPartiallySolved()) {
        if (temp++ > this.size * Constants.SIZE_FOR_APPROX) {
          return true;
        }
      } else {
        temp = 0;
      }
    }
    return this.firstPosition.isSolved() || this.firstPosition.getUpper() <= lower || this.firstPosition.getLower() >= upper;
  }

  public boolean toBeSolved(StoredBoard.Evaluation eval) {
    StoredBoard b = eval.getStoredBoard();
    if (b.nEmpties > Constants.EMPTIES_FOR_FORCED_MIDGAME + 3) {
      return false;
    } else if (b.nEmpties <= Constants.EMPTIES_FOR_FORCED_MIDGAME) {
      return true;
    }
    if (eval.getProb() == 1) {
      return eval.proofNumber < constant;
    } else if (eval.getProb() == 0) {
      return eval.disproofNumber < constant;
    }
    return false;
  }

  public short evaluatePosition(
      Board board, int lower, int upper, long maxNVisited, long maxTimeMillis, boolean reset) {
    assert Math.abs(lower % 200) == 100;
    assert Math.abs(upper % 200) == 100;
    assert(lower <= upper);
    constant = 0;
    status = Status.RUNNING;
    this.lower = lower;
    this.upper = upper;
    this.maxNVisited = maxNVisited;
    this.justStarted = true;
    if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {
      this.firstPosition.setIsFinished(false);
    }
    if (firstPosition.getPlayer() != board.getPlayer() ||
        firstPosition.getOpponent() != board.getOpponent() ||
        !firstPosition.fathers.isEmpty()) {
      StoredBoard boardStored = null;

      if (!reset) {
        boardStored = get(board.getPlayer(), board.getOpponent());
        firstPosition = boardStored;
      }
      if (boardStored == null || boardStored.getDescendants() == 0) {
        setFirstPosition(board.getPlayer(), board.getOpponent());
      }
    }
    this.stopTimeMillis = System.currentTimeMillis() + maxTimeMillis;

    Thread[] threads = new Thread[this.threads.length];

    Thread.setDefaultUncaughtExceptionHandler((t, e) -> {
      this.status = Status.FAILED;
      e.printStackTrace();
    });
    for (int i = 0; i < threads.length; ++i) {
      threads[i] = new Thread(this.threads[i]);
      threads[i].start();
    }
    for (Thread t : threads) {
      try {
        t.join();
      } catch (Exception ex) {
        Logger.getLogger(EvaluatorMCTS.class.getName()).log(Level.SEVERE, null, ex);
      }
    }
    return (short) -firstPosition.getEval();
  }

  public StoredBoard getFirstPosition() {
    return firstPosition;
  }

  public void assertIsAllOKRecursive(StoredBoard board) {
    assertIsOK(board);
    if (board.isLeaf()) {
      return;
    }
    for (StoredBoard child : board.getChildren()) {
      assertIsAllOKRecursive(child);
    }
  }
  public void assertIsOK(StoredBoard board) {
    if (board.isLeaf()) {
      return;
    }
    for (int i = getAlpha(board); i <= getBeta(board); i += 200) {
      if (board.getEvaluation(i).bestChildMidgame != null && board.getEvaluation(i).bestChildMidgame.getStoredBoard().threadId != 0) {
        System.out.println(board);
        System.out.println(board.getEvaluation(i).bestChildMidgame.getStoredBoard());
        System.out.println("Thread: " + board.getEvaluation(i).bestChildMidgame.getStoredBoard().threadId);
        System.out.println("N visited: " + this.getNVisited());
        System.out.println("Tree size: " + this.size);
        assert false;
      }
    }
  }

  public void addChildren(StoredBoardBestDescendant position) {
    position.eval.getStoredBoard().setBusy(-6300, 6300);
    threads[0].addChildren(position);
    finalizePosition(position, 0);
  }

  public void addChildren(StoredBoard board) {
    addChildren(new StoredBoardBestDescendant(board.getEvaluation(nextPositionEvalGoal(board))));
  }
}