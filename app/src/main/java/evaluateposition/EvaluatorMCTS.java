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

import bitpattern.BitPattern;
import board.Board;
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

  boolean threadWaitingForNextPos = false;
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
  private boolean lastDoubtGreaterEqual = true;

  private class EvaluatorThread implements Runnable {    
    private final EvaluatorInterface nextEvaluator;
    private final EvaluatorMCTS main;

    private EvaluatorThread(EvaluatorInterface nextEvaluator, EvaluatorMCTS main) {
      this.nextEvaluator = nextEvaluator;
      this.main = main;
    }

    public long addChildren(StoredBoardBestDescendant position) {
      StoredBoard board = position.eval.getStoredBoard();
      int depth = board.nEmpties < 22 ? 2 : 4;
      int evalGoal = position.eval.evalGoal;
      long nVisited = 0;
      int alpha = board.depth % 2 == 0 ? weakLower : -weakUpper;
      int beta = board.depth % 2 == 0 ? weakUpper : -weakLower;
      assert evalGoal >= alpha;
      assert evalGoal <= beta;

      editLock.lock();
      assert board.getChildren() == null;

      if (!board.addChildren(EvaluatorMCTS.this)) {
        int finalEval = BitPattern.getEvaluationGameOver(board.getPlayer(), board.getOpponent());
        board.setSolved(finalEval);
        board.setFree(alpha, beta);
        editLock.unlock();
        return 1;
      }
      editLock.unlock();

      for (StoredBoard child : board.getChildren()) {
        if (child.leafEval != -6500) {
          continue;
        }
        child.setBusyNoUpdate();
        int eval = nextEvaluator.evaluate(child.getPlayer(), child.getOpponent(), depth, -6400, 6400);
        eval = Math.min(6400, Math.max(-6400, eval));
        child.setLeaf(eval, -beta, -alpha);
        StoredBoard.Evaluation childEval = child.getEvaluation(-evalGoal);
        childEval.addDescendants(nextEvaluator.getNVisited() + 1);
        nVisited += childEval.descendants;
        child.setFreeNoUpdate();
      }
      editLock.lock();

      for (int i = alpha; i <= beta; i += 200) {
        if (board.getEvaluation(i) == null) {
          board.addEvaluation(i);
        }
        board.getEvaluation(i).updateFather();
      }
      board.setFree(alpha, beta);
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
      for (int i = position.alpha; i <= position.beta; i += 200) {
        StoredBoard.Evaluation evaluation = board.getEvaluation(i);
        if (evaluation == null) {
          evaluation = board.addEvaluation(i);
        }
        if (eval <= i) {
          evaluation.setDisproved();
        } else if (eval >= i) {
          evaluation.setProved();
        }
      }
      long seenPositions = nextEvaluator.getNVisited() + 1;
      synchronized(main) {
        constant = Math.max(0, constant + 0.05 * (10000 - seenPositions));
      }
      editLock.lock();
      alpha = board.depth % 2 == 0 ? weakLower : -weakUpper;
      beta = board.depth % 2 == 0 ? weakUpper : -weakLower;
      board.setFree(alpha, beta);
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
        if (board.nEmpties - d < Constants.EMPTIES_FOR_FORCED_MIDGAME) {
          return seenPositions + solvePosition(position);
        }
        curEval = nextEvaluator.evaluate(
            board.getPlayer(), board.getOpponent(), d, -6400, 6400);
        seenPositions += nextEvaluator.getNVisited();
      }
      editLock.lock();
      board.setLeaf(curEval, position.alpha, position.beta);
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
        if (position.eval.getStoredBoard() != firstPosition && toBeSolved(position.eval) && position.eval.getStoredBoard().isLeaf()) {
          nVisited = solvePosition(position);
        } else if (size < maxSize) {
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

  int lastEvalGoal = -6500;

  public int nextPositionEvalGoal(StoredBoard board) {
    assert nextPositionLock.isHeldByCurrentThread();
    int bestLogDerivative = StoredBoard.LOG_DERIVATIVE_MINUS_INF;
    int bestEvalProof = -6300;
    int bestEvalDisproof = 6300;
//    int bestEval = 0;
    int lower = weakLower;
    int upper = weakUpper;
    double quantile = 0.5;
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
      curValue += curEval.evalGoal == lastEvalGoal ? 1 : 0;

      if (curValue > bestValue) {
        bestValue = curValue;
        bestEval = curEval;
      }
//      if (evalCur.getProb() < quantile) {
//        if (eval == lower && evalCur.disproofNumber != 0) {
//          return eval;
//        }
//        StoredBoard.Evaluation evalPrevious = board.getEvaluation(eval - 200);
//        if (evalPrevious.proofNumber() == 0) {
//          return eval;
//        }
//        if (evalCur.disproofNumber() == 0) {
//          return eval - 200;
//        }
//        double tmp = random.nextDouble();
//        if (tmp < Constants.PROB_RANDOM_VALUE) {
//          return eval;
//        } else if (tmp > 1 - Constants.PROB_RANDOM_VALUE) {
//          return eval - 200;
//        }
//        if (evalPrevious.maxLogDerivative() > evalCur.maxLogDerivative()) {
//          return eval - 200;
//        } else if (evalPrevious.maxLogDerivative() < evalCur.maxLogDerivative()) {
//          return eval;
//        }
//        lastDoubtGreaterEqual = !lastDoubtGreaterEqual;
//        return lastDoubtGreaterEqual ? eval - 200 : eval;
//      }
    }
//    System.out.println(bestEval + " " + board.getEvaluation(bestEval).evalGoal + " " + board.getWeakLower() + " " + board.getWeakUpper());
//    assert bestEval == 0 || (board.getEvaluation(bestEval).evalGoal >= board.getWeakLower() - 100 && board.getEvaluation(bestEval).evalGoal <= board.getWeakUpper() + 100);
//    assert bestEvalProof == -6300 || (board.getEvaluation(bestEvalProof).evalGoal >= board.getWeakLower() - 100 && board.getEvaluation(bestEvalProof).evalGoal <= board.getWeakUpper() + 100);
//    assert bestEvalDisproof == 6300 || (board.getEvaluation(bestEvalDisproof).evalGoal >= board.getWeakLower() - 100 && board.getEvaluation(bestEvalDisproof).evalGoal <= board.getWeakUpper() + 100);
//    if (bestLogDerivative > StoredBoard.LOG_DERIVATIVE_MINUS_INF) {
//      return bestEval;
//    }
//    if (board.getProofNumber(bestEvalProof) == 0) {
//      return bestEvalDisproof;
//    }
//    if (board.getDisproofNumber(bestEvalDisproof) == 0) {
//      return bestEvalProof;
//    }
//    lastDoubtGreaterEqual = !lastDoubtGreaterEqual;
//    return lastDoubtGreaterEqual ? bestEvalProof : bestEvalDisproof;
//    System.out.println(lower + " " + upper + " " + bestEval.evalGoal);
    lastEvalGoal = bestEval.evalGoal;
//    System.out.println(lastEvalGoal);
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

    if (threadWaitingForNextPos) {
      nextPositionLock.lock();
      try {
        threadWaitingForNextPos = false;
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

  private StoredBoard.Evaluation nextChild() {
    return firstPosition.getEvaluation(nextPositionEvalGoal(firstPosition)).bestChild();
//    ArrayList<StoredBoard> nextAvailable = new ArrayList<>();
//    for (StoredBoard next : firstPosition.getChildren()) {
//      if (!next.isSolved()) {
//        nextAvailable.add(next);
//      }
//    }
//    if (nextAvailable.size() == 0) {
//      return null;
//    }
//    return nextAvailable.get((int) (Math.random() * nextAvailable.size()));
  }

  private void updateWeakLowerUpper() {
    assert nextPositionLock.isHeldByCurrentThread();
    if (firstPosition.getDescendants() < this.lastUpdateWeak * 1.2) {
      return;
    }
    int newWeakLower = Math.max(-6300, firstPosition.getWeakLower() - 100);
    int newWeakUpper = Math.min(6300, firstPosition.getWeakUpper() + 100);
//    System.out.println(firstPosition.getDescendants() + ": (" + weakLower + ", " + weakUpper + ") -> (" + newWeakLower + ", " + newWeakUpper + ")");

    if (newWeakUpper > weakUpper) {
      firstPosition.updateDescendantsRecursive(weakUpper + 200, newWeakUpper);
    }
    if (newWeakLower < weakLower) {
      firstPosition.updateDescendantsRecursive(newWeakLower, weakLower - 200);
    }
    weakUpper = newWeakUpper;
    weakLower = newWeakLower;
    lastUpdateWeak = this.getNVisited();
  }

  protected StoredBoardBestDescendant getNextPosition() {
    StoredBoardBestDescendant result = null;
    nextPositionLock.lock();
    try {
      if (checkFinished()) {
        return null;
      }
      updateWeakLowerUpper();
      result = StoredBoardBestDescendant.bestDescendant(firstPosition.getEvaluation(nextPositionEvalGoal(firstPosition)), weakLower, weakUpper);
      while (result == null) {
        assert Constants.MAX_PARALLEL_TASKS > 1;
        threadWaitingForNextPos = true;
        isNextPositionAvailable.await();
        assert nextPositionLock.isHeldByCurrentThread();
        if (checkFinished()) {
          return null;
        }
        result = StoredBoardBestDescendant.bestDescendant(firstPosition.getEvaluation(nextPositionEvalGoal(firstPosition)), weakLower, weakUpper);
      }
      editLock.lock();
    } catch (InterruptedException ex) {
      Logger.getLogger(EvaluatorMCTS.class.getName()).log(Level.SEVERE, null, ex);
    } finally {
      nextPositionLock.unlock();
    }
    try {
      result.eval.getStoredBoard().setBusy(result.eval.evalGoal, result.eval.evalGoal);
    } finally {
      editLock.unlock();
    }
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

  void setFirstPosition(Board b) {
    setFirstPosition(b.getPlayer(), b.getOpponent());
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

  Random random = new Random();
  void setFirstPosition(long player, long opponent) {
    empty();
    random.setSeed(42);
    firstPosition = StoredBoard.initialStoredBoard(player, opponent);
    firstPosition.setBusy(-6300, 6300);
    add(firstPosition);
    firstPosition.setLeaf(0, -6300, 6300);
    firstPosition.addChildren(this);

    for (StoredBoard child : firstPosition.getChildren()) {
      child.setBusyNoUpdate();
      int quickEval = nextEvaluator.evaluate(child.getPlayer(), child.getOpponent(), 4, lower, upper);
      child.setLeaf(quickEval, -6300, 6300);
      child.getEvaluation(evalToBoundary(quickEval)).addDescendants(nextEvaluator.getNVisited() + 1);
      firstPosition.getEvaluation(evalToBoundary(-quickEval)).addDescendants(child.getDescendants());
      child.setFreeNoUpdate();
    }
    for (int i = -6300; i <= 6300; i += 200) {
      firstPosition.getEvaluation(i).updateFather();
    }
    firstPosition.setFree(-6300, 6300);
    this.weakUpper = 6300;
    this.weakLower = -6300;
    this.lastUpdateWeak = 0;
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
    this.stopTimeMillis = System.currentTimeMillis() + maxTimeMillis;
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

  public void addChildren(StoredBoardBestDescendant position) {
    position.eval.getStoredBoard().setBusy(-6300, 6300);
    threads[0].addChildren(position);
    finalizePosition(position, 0);
  }

  public void addChildren(StoredBoard board) {
    addChildren(new StoredBoardBestDescendant(board.getEvaluation(nextPositionEvalGoal(board))));
  }
}