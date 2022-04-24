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

import bitpattern.BitPattern;
import board.Board;
import board.GetMovesCache;
import constants.Constants;

import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.function.Supplier;

public class EvaluatorMCTS extends HashMapVisitedPositions implements EvaluatorDerivativeInterface {
  private long maxNVisited;
  private long startTimeMillis;
  private int maxTimeMillis;
  private final HashMap hashMap;
  private boolean justStarted = false;
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

  private class EvaluatorThread implements Runnable {
    private final EvaluatorInterface nextEvaluator;

    private EvaluatorThread(EvaluatorInterface nextEvaluator) {
      this.nextEvaluator = nextEvaluator;
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
        int finalEval = BitPattern.getEvaluationGameOver(father.getPlayer(), father.getOpponent());
        father.setSolved(finalEval);
        return 1;
      }

      PartialEval[] childrenEval = new PartialEval[moves.length];
      StoredBoard[] children = new StoredBoard[moves.length];
      for (int i = 0; i < moves.length; ++i) {
        long flip = moves[i];
        long newPlayer = father.getOpponent() & ~flip;
        long newOpponent = father.getPlayer() | flip;
        StoredBoard child = getOrAdd(newPlayer, newOpponent, (short) (father.depth + 1));
        children[i] = child;
        if (child.leafEval == -6500) {
          int eval = nextEvaluator.evaluate(child.getPlayer(), child.getOpponent(), depth, -6400, 6400);
          childrenEval[i] = new PartialEval(Math.min(6400, Math.max(-6400, eval)), nextEvaluator.getNVisited());
        }
      }
      synchronized (father) {
        for (int i = 0; i < children.length; ++i) {
          StoredBoard child = children[i];
          synchronized (child) {
            PartialEval childEval = childrenEval[i];
            if (childEval != null) {
              nVisited += childEval.nVisited;
              if (child.leafEval == -6500) {
                child.setWeakLowerUpper((short) Math.min(-father.weakUpper, -evalGoal), (short) Math.max(-father.weakLower, -evalGoal));
                child.getEvaluation(-evalGoal).addDescendants(childEval.nVisited + 1);
                child.setLeaf((short) childEval.eval, (short) 4);
              }
            }
            child.addFather(father);
          }
        }
        father.children = children;
        assert father.isLowerUpperOK();
      }
      father.updateFathers();
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
      StoredBoard.proofNumberForAlphaBeta.addAndGet((int) (Constants.PROOF_NUMBER_GOAL_FOR_MIDGAME - seenPositions) / 20);
      board.updateFathers();
      return seenPositions;
    }

    public long deepenPosition(StoredBoardBestDescendant position) {
      StoredBoard board = position.eval.getStoredBoard();
      int curEval = nextEvaluator.evaluate(
            board.getPlayer(), board.getOpponent(), 2, -6400, 6400);
      int d;
      long seenPositions = 0;
      for (d = 4; seenPositions < board.getDescendants() * 2; d += 2) {
        if (board.nEmpties - d < Constants.EMPTIES_FOR_FORCED_MIDGAME - 2) {
          seenPositions += solvePosition(position);
          return seenPositions;
        }
        curEval = nextEvaluator.evaluate(
            board.getPlayer(), board.getOpponent(), d, -6400, 6400);
        seenPositions += nextEvaluator.getNVisited();
      }
      board.setLeaf((short) curEval, (short) d);
      board.updateFathers();
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
        firstPosition.setNewLowerUpper(board);
        assert board.isLeaf();
        if (board != firstPosition && board.toBeSolved(position.eval.evalGoal)) {
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
      threads[i] = new EvaluatorThread(evaluatorMidgameBuilder.get());
    }
  }

  public int getLower() { return lower; }

  public int getUpper() { return upper; }

  public long getNVisited() {
    return this.firstPosition.getDescendants();
  }
  
  public long getNStored() {
    return this.size;
  }

  public synchronized void stop() {
    if (status == Status.RUNNING || status == Status.STOPPED_TIME) {
      status = Status.KILLING;
    }
  }

  private double stepsUntilEnd() {
    return firstPosition.stepsUntilEnd();
  }

  protected void finalizePosition(StoredBoardBestDescendant position, long nVisited) {
    position.updateDescendants(nVisited);
    justStarted = false;
  }
  
  protected synchronized boolean checkFinished() {
    synchronized (firstPosition) {
      assert firstPosition.isLowerUpperOK();
      if (hashMap.size() > Constants.HASH_MAP_SIZE / 2) {
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
      if (isSolved()) {
        status = Status.SOLVED;
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
      return false;
    }
  }

  private void updateWeakLowerUpper() {
    synchronized (firstPosition) {
      if (getNVisited() < this.lastUpdateWeak * 1.3) {
        if (firstPosition.getProb(firstPosition.getWeakLower()) > Constants.PROB_INCREASE_WEAK_EVAL &&
                firstPosition.getProb(firstPosition.getWeakUpper()) < 1 - Constants.PROB_INCREASE_WEAK_EVAL) {
          return;
        }
      }
    }
    if (!firstPosition.updateWeakLowerUpper()) {
      lastUpdateWeak = getNVisited();
    }
  }

  protected StoredBoardBestDescendant getNextPosition() {
    if (Constants.ASSERT_EXTENDED) {
      assertIsAllOKRecursive(firstPosition);
    }

    StoredBoardBestDescendant result;
    while (true) {
//      System.out.println(weakLower + " " + weakUpper + " " + firstPosition.weakLower + " " + firstPosition.weakUpper);
      updateWeakLowerUpper();
      if (checkFinished()) {
        return null;
      }
//      System.out.println("  " + weakLower + " " + weakUpper + " " + firstPosition.weakLower + " " + firstPosition.weakUpper);
      result = firstPosition.bestDescendant();
      if (result != null) {
        break;
      }
      try {
        Thread.sleep(0, 100);
      } catch (InterruptedException e) {
        e.printStackTrace();
      }
    }
    assert result.eval.getStoredBoard().isLeaf();
    return result;
  }
  
  public synchronized Status getStatus() {
    return status;
  }

  void setFirstPosition(Board b) {
    setFirstPosition(b.getPlayer(), b.getOpponent());
  }

  void setFirstPosition(long player, long opponent) {
    empty();
    StoredBoard.proofNumberForAlphaBeta.set(0);
    StoredBoard.avoidNextPosFailCoeff.set(8000);
    int eval = threads[0].nextEvaluator.evaluate(player, opponent, 4, -6400, 6400);
    firstPosition = StoredBoard.initialStoredBoard(player, opponent, (short) eval);
    add(firstPosition);
    this.lastUpdateWeak = 0;
    if (Constants.ASSERT_EXTENDED) {
      assertIsAllOKRecursive(firstPosition);
    }
    assert firstPosition.isLowerUpperOK();
  }

  public boolean isSolved() {
    if (Constants.APPROX_ONLY) {
      if (firstPosition.isPartiallySolved()) {
        return true;
      }
    }
    return this.firstPosition.isSolved() || this.firstPosition.getUpper() <= lower || this.firstPosition.getLower() >= upper;
  }

  public void evaluate(Board board) {
    evaluate(board, -6300, 6300, Long.MAX_VALUE, Integer.MAX_VALUE);
  }

  public void evaluate(Board board, long maxNVisited, int maxTimeMillis) {
    evaluate(board, -6300, 6300, maxNVisited, maxTimeMillis);
  }

  ExecutorService executor = Executors.newFixedThreadPool(Constants.MAX_PARALLEL_TASKS);

  @Override
  public void evaluate(
      Board board, int lower, int upper, long maxNVisited, int maxTimeMillis) {
    assert Math.abs(lower % 200) == 100;
    assert Math.abs(upper % 200) == 100;
    assert(lower <= upper);
    EvaluatorAlphaBeta.resetConstant();
    synchronized (this) {
      status = Status.RUNNING;
    }
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

    Future<?>[] futures = new Future<?>[threads.length];
    for (int i = 0; i < threads.length; ++i) {
      futures[i] = executor.submit(threads[i]);
    }
    for (Future<?> future : futures) {
      try {
        future.get();
      } catch (ExecutionException | InterruptedException e) {
        e.printStackTrace();
        status = Status.FAILED;
      }
    }
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
    assert board.areChildrenOK();
  }

  public void addChildren(StoredBoardBestDescendant position) {
//    position.eval.getStoredBoard().setBusy(-6300, 6300);
    threads[0].addChildren(position);
    finalizePosition(position, 0);
  }

  public void addChildren(StoredBoard board) {
    addChildren(board.bestDescendant());
  }
}