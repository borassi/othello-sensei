// Copyright 2022 Google LLC
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

import static evaluateposition.StoredBoard.LOG_DERIVATIVE_MINUS_INF;

import bitpattern.BitPattern;
import board.Board;
import board.GetMovesCache;
import constants.Constants;

import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.ReentrantReadWriteLock;
import java.util.function.Supplier;
import java.util.logging.Level;
import java.util.logging.Logger;

public class EvaluatorMCTS extends HashMapVisitedPositions {
  int threadWaitingForNextPos = 0;
  private long maxNVisited;
  private long startTimeMillis;
  private int maxTimeMillis;
  private final HashMap hashMap;
  static final ReentrantReadWriteLock editNextPositionLock = new ReentrantReadWriteLock();
  static final ReentrantReadWriteLock.WriteLock nextPositionLock = editNextPositionLock.writeLock();
  static final ReentrantReadWriteLock.ReadLock editLock = editNextPositionLock.readLock();
  private boolean justStarted = false;
  private int weakLower = -6300;
  private int weakUpper = 6300;
  private long lastUpdateWeak = 0;
  private double bestStepsUntilEnd = 0;

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
  private Status status = Status.SOLVED;
  private final EvaluatorThread[] threads;
  private double constant = 0;

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
//                child.setBusy(-beta, -alpha);
                child.setLeaf(childEval.eval, -beta, -alpha);
                child.updateFathers(-beta, -alpha);
              }
            }
            child.addFather(father);
          }
        }
        father.children = children;
      }
      father.updateFathers(alpha, beta);
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
      board.updateFathers(getAlpha(board), getBeta(board));
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
      board.setLeaf(curEval, getAlpha(board), getBeta(board), 0.2 / (0.2 + (d - 4)));
      board.updateFathers(getAlpha(board), getBeta(board));
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
        if (board != firstPosition && board.toBeSolved(position.eval.evalGoal, constant) && board.isLeaf()) {
          nVisited = solvePosition(position);
        } else if (size < 0.8 * maxSize ||
                   (size < 0.9 * maxSize && board.depth < 9) ||
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
  }

  public int getLower() { return lower; }

  public int getUpper() { return upper; }

  public int getWeakLower() { return weakLower; }

  public int getWeakUpper() { return weakUpper; }

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

  private double stepsUntilEnd() {
    return firstPosition.stepsUntilEnd(weakLower, weakUpper);
  }

  protected void finalizePosition(StoredBoardBestDescendant position, long nVisited) {
    editLock.lock();
    try {
      position.updateDescendants(nVisited);
      justStarted = false;
    } finally {
      editLock.unlock();
    }
  }
  
  protected boolean checkFinished() {
    synchronized (firstPosition) {
      if (hashMap.size() > Constants.hashMapSize() / 2) {
        hashMap.reset();
      }
      double stepsUntilEnd = this.stepsUntilEnd();
      this.bestStepsUntilEnd = Math.min(this.bestStepsUntilEnd, stepsUntilEnd);

      if (status == Status.KILLING) {
        status = Status.KILLED;
        return true;
      }
      if (status == Status.KILLED) {
        return true;
      }
      if (firstPosition.getDescendants() > 0.8 * maxNVisited && !justStarted) {
        if (firstPosition.getDescendants() > maxNVisited || stepsUntilEnd == bestStepsUntilEnd) {
          status = Status.STOPPED_POSITIONS;
          return true;
        }
      }
      double currentTime = System.currentTimeMillis();
      if (currentTime - startTimeMillis > maxTimeMillis * 0.8) {
        if (currentTime - startTimeMillis > maxTimeMillis || stepsUntilEnd == bestStepsUntilEnd) {
          status = Status.STOPPED_TIME;
          return true;
        }
      }
      if (isSolved()) {
        status = Status.SOLVED;
        return true;
      }
      return false;
    }
  }

  private void updateWeakLowerUpper() {
    long descendants = getNVisited();
    if (descendants < this.lastUpdateWeak * 1.2) {
      return;
    }
    int firstPosLower;
    int firstPosUpper;
    int newWeakLower;
    int newWeakUpper;
    float probLower;
    float probUpper;
    synchronized (firstPosition) {
      firstPosLower = firstPosition.getLower();
      firstPosUpper = firstPosition.getUpper();
      probLower = firstPosition.getProb(weakLower);
      probUpper = firstPosition.getProb(weakUpper);
      newWeakLower = Math.max(lower, firstPosition.getPercentileLower(Constants.PROB_REDUCE_WEAK_EVAL) - 100);
      newWeakUpper = Math.min(upper, firstPosition.getPercentileUpper(Constants.PROB_REDUCE_WEAK_EVAL) + 100);
    }

    if (newWeakLower > firstPosUpper) {
      assert false;
      newWeakLower = firstPosUpper;
    } else if (newWeakUpper < firstPosLower) {
      assert false;
      newWeakUpper = firstPosLower;
    }

    if (newWeakLower < weakLower && probLower > 1 - Constants.PROB_INCREASE_WEAK_EVAL) {
      newWeakLower = weakLower;
    }
    if (newWeakUpper > weakUpper && probUpper < Constants.PROB_INCREASE_WEAK_EVAL) {
      newWeakUpper = weakUpper;
    }
    nextPositionLock.lock();
    lastUpdateWeak = this.getNVisited();
//    System.out.println("(" + weakLower + ", " + weakUpper + ") -> (" + newWeakLower + " " + newWeakUpper + ")");
    // TODO: PROBLEM: WHEN I UPDATE, OTHER THREADS MIGHT GET IT WRONG.
    if (newWeakUpper > weakUpper) {
      firstPosition.updateDescendantsRecursive(weakUpper + 200, newWeakUpper);
    }
    if (newWeakLower < weakLower) {
      firstPosition.updateDescendantsRecursive(newWeakLower, weakLower - 200);
    }
    weakLower = newWeakLower;
    weakUpper = newWeakUpper;
    nextPositionLock.unlock();
  }

  protected int getAlpha(StoredBoard board) {
    return board.depth % 2 == 0 ? weakLower : -weakUpper;
  }

  protected int getBeta(StoredBoard board) {
    return board.depth % 2 == 0 ? weakUpper : -weakLower;
  }

  protected StoredBoardBestDescendant getNextPosition() {
    if (Constants.ASSERT_EXTENDED) {
      assertIsAllOKRecursive(firstPosition);
    }

    StoredBoardBestDescendant result;
    while (true) {
      updateWeakLowerUpper();
      if (checkFinished()) {
        return null;
      }
      editLock.lock();
      result = firstPosition.bestDescendant(weakLower, weakUpper);
      editLock.unlock();
      if (result != null) {
        break;
      }
      try {
        Thread.sleep(0, 100);
      } catch (InterruptedException e) {
        e.printStackTrace();
      }
    }
    assert result != null;
    assert result.eval.getStoredBoard().isLeaf();
    return result;
  }
  
  public synchronized Status getStatus() {
    return status;
  }

  public static int evalToBoundary(int eval) {
    if (eval >= 6300) {
      return 6300;
    }
    if (eval <= -6300) {
      return 6300;
    }
    return 200 * Math.round((eval + 100.0F) / 200) - 100;
  }

  void setFirstPosition(Board b) {
    setFirstPosition(b.getPlayer(), b.getOpponent());
  }

  void setFirstPosition(long player, long opponent) {
    empty();
    firstPosition = StoredBoard.initialStoredBoard(player, opponent);
    add(firstPosition);
    this.weakUpper = upper;
    this.weakLower = lower;
    firstPosition.setLeaf(0, -6300, 6300, 64);
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

  public short evaluatePosition(Board board) {
    return evaluatePosition(board, -6300, 6300, Long.MAX_VALUE, Integer.MAX_VALUE);
  }

  public short evaluatePosition(Board board, long maxNVisited, int maxTimeMillis) {
    return evaluatePosition(board, -6300, 6300, maxNVisited, maxTimeMillis);
  }

  public short evaluatePosition(
      Board board, int lower, int upper, long maxNVisited, int maxTimeMillis) {
    assert Math.abs(lower % 200) == 100;
    assert Math.abs(upper % 200) == 100;
    assert(lower <= upper);
    constant = 0;
    StoredBoard.avoidNextPosFailCoeff.set(4000);
    EvaluatorAlphaBeta.resetConstant();
    if (status == Status.KILLING) {
      status = Status.KILLED;
      return -6500;
    }
    status = Status.RUNNING;
    this.lower = lower;
    this.upper = upper;
    this.bestStepsUntilEnd = Double.POSITIVE_INFINITY;

    this.maxNVisited = maxNVisited;
    this.justStarted = true;
    if (firstPosition.getPlayer() != board.getPlayer() ||
        firstPosition.getOpponent() != board.getOpponent()) {
      setFirstPosition(board.getPlayer(), board.getOpponent());
    }
    this.startTimeMillis = System.currentTimeMillis();
    this.maxTimeMillis = maxTimeMillis;

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
    return (short) -firstPosition.getEval(weakLower, weakUpper);
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
  }

  public void addChildren(StoredBoardBestDescendant position) {
//    position.eval.getStoredBoard().setBusy(-6300, 6300);
    threads[0].addChildren(position);
    finalizePosition(position, 0);
  }

  public void addChildren(StoredBoard board) {
    addChildren(board.bestDescendant(weakLower, weakUpper));
  }
}