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
import board.GetMovesCache;
import constants.Constants;
import evaluatedepthone.PatternEvaluatorImproved;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
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
      long player = board.getPlayer();
      long opponent = board.getOpponent();
      int depth = board.nEmpties < 22 ? 2 : 4;
      // Includes passes.
      long[] moves = GetMovesCache.getAllMoves(player, opponent);

      if (moves == null) {
        board.setSolved(BitPattern.getEvaluationGameOver(player, opponent));
        board.updateAndSetFree(BitPattern.getEvaluationGameOver(player, opponent));
        return 0;
      }
      StoredBoard[] children = new StoredBoard[moves.length];
      long nVisited = 0;
      for (int i = 0; i < moves.length; ++i) {
        long flip = moves[i];
        long newPlayer = opponent & ~flip;
        long newOpponent = player | flip;
        StoredBoard child = new StoredBoard(newPlayer, newOpponent, !board.playerIsStartingPlayer);
        child.setBusy();
        int eval = nextEvaluator.evaluate(newPlayer, newOpponent, depth, -6400, 6400);
        child.getEvaluation(-position.eval.evalGoal).addDescendants(nextEvaluator.getNVisited() + 1);
        child.updateAndSetFree(eval);
        child.fathers.add(board);
        nVisited += child.getDescendants();
        children[i] = child;
      }
      board.children = children;
      for (StoredBoard.Evaluation eval : board.evaluations) {
        eval.isLeaf = false;
      }
      editLock.lock();
      {
        for (int i = 0; i < board.children.length; ++i) {
          StoredBoard child = board.children[i];
          StoredBoard childInEvaluator = get(child.getPlayer(), child.getOpponent());
          if (Constants.IGNORE_TRANSPOSITIONS) {
            childInEvaluator = null;
          }
          if (childInEvaluator == null) {
            add(child);
          } else {
            board.children[i] = childInEvaluator;
            assert !childInEvaluator.fathers.contains(board);
            childInEvaluator.addFather(board);
            if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {
              childInEvaluator.extraInfo.isFinished = board.extraInfo.isFinished;
            }
          }
        }
        for (StoredBoard.Evaluation eval : board.evaluations) {
          eval.updateFather();
        }
        board.setFree();
      }
      editLock.unlock();
      return nVisited;
    }

    public long solvePosition(StoredBoardBestDescendant position) {
      StoredBoard board = position.eval.getStoredBoard();
      int alpha = position.eval.evalGoal;
      int beta = position.eval.evalGoal;
      int eval = nextEvaluator.evaluate(
          board.getPlayer(), board.getOpponent(), board.nEmpties, alpha, beta);
      if (eval <= alpha) {
        // Tricky but correct.
        board.setUpper(eval);
      } else if (eval >= beta) {
        board.setLower(eval);
      } else {
        board.setSolved(eval);
      }
      long seenPositions = nextEvaluator.getNVisited() + 1;
      synchronized(main) {
        constant = Math.max(0, constant + 0.05 * (10000 - seenPositions));
      }
      editLock.lock();
      board.setFree();
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
      board.updateAndSetFree(curEval);
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
        if (position.eval.getStoredBoard() != firstPosition && toBeSolved(position.eval)) {
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

  public int nextPositionEvalGoal() {
    assert nextPositionLock.isHeldByCurrentThread();
//    int eval = (int) (Math.round(firstPosition.getEval() / 200.0) * 200);
    int bestLogDerivative = StoredBoard.LOG_DERIVATIVE_MINUS_INF;
    int bestEvalProof = -6300;
    int bestEvalDisproof = 6300;
    int bestEval = 0;
    for (int eval = lower; eval <= upper; eval += 200) {
      if (firstPosition.maxLogDerivative(eval) > bestLogDerivative) {
        bestLogDerivative = firstPosition.maxLogDerivative(eval);
        bestEval = eval;
      }
      if (firstPosition.getProb(eval) == 1) {
        bestEvalProof = eval;
      }
      if (bestEvalDisproof == 6300 && firstPosition.getProb(eval) == 0) {
        bestEvalDisproof = eval;
      }
    }
    if (bestLogDerivative > StoredBoard.LOG_DERIVATIVE_MINUS_INF) {
      return bestEval;
    }
    if (firstPosition.getProofNumber(bestEvalProof) == 0) {
      return bestEvalDisproof;
    }
    if (firstPosition.getDisproofNumber(bestEvalDisproof) == 0) {
      return bestEvalProof;
    }
    lastDoubtGreaterEqual = !lastDoubtGreaterEqual;
    return lastDoubtGreaterEqual ? bestEvalProof : bestEvalDisproof;
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
    if (this.firstPosition.children == null) {
      return false;
    }
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

  protected StoredBoardBestDescendant getNextPosition() {
    StoredBoardBestDescendant result = null;
    nextPositionLock.lock();
    try {
      if (checkFinished()) {
        return null;
      }
      result = StoredBoardBestDescendant.bestDescendant(firstPosition.getEvaluation(nextPositionEvalGoal()));
      while (result == null) {
        threadWaitingForNextPos = true;
        isNextPositionAvailable.await();
        assert nextPositionLock.isHeldByCurrentThread();
        if (checkFinished()) {
          return null;
        }
        result = StoredBoardBestDescendant.bestDescendant(firstPosition.getEvaluation(nextPositionEvalGoal()));
      }
      editLock.lock();
    } catch (InterruptedException ex) {
      Logger.getLogger(EvaluatorMCTS.class.getName()).log(Level.SEVERE, null, ex);
    } finally {
      nextPositionLock.unlock();
    }
    try {
      result.eval.getStoredBoard().setBusy();
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

  void setFirstPosition(long player, long opponent) {
    empty();
    int quickEval = nextEvaluator.evaluate(player, opponent, 2, lower, upper);
    firstPosition = StoredBoard.initialStoredBoard(player, opponent);
    firstPosition.evaluations[30].addDescendants(nextEvaluator.getNVisited() + 1);
    firstPosition.setBusy();
    firstPosition.updateAndSetFree(quickEval);
    add(firstPosition);
  }

  public boolean isSolved() {
    if (Constants.APPROX_ONLY) {
      for (int eval = lower; eval <= upper; eval += 200) {
        if (firstPosition.getProb(eval) > 0 && firstPosition.getProb(eval) < 1) {
          return false;
        }
      }
      return true;
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
    ExecutorService es = Executors.newFixedThreadPool(Math.min(Constants.MAX_PARALLEL_TASKS, Runtime.getRuntime().availableProcessors()));
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
    
    for (int i = 0; i < threads.length; ++i) {
      threads[i] = new Thread(this.threads[i]);
      threads[i].start();
    }
    for (Thread t : threads) {
      try {
        t.join();
      } catch (InterruptedException | AssertionError ex) {
        Logger.getLogger(EvaluatorMCTS.class.getName()).log(Level.SEVERE, null, ex);
      }
    }
    es.shutdown();
    return (short) -firstPosition.getEval();
  }

  public StoredBoard getFirstPosition() {
    return firstPosition;
  }

  public void addChildren(StoredBoardBestDescendant position) {
    position.eval.setBusy();
    threads[0].addChildren(position);
    finalizePosition(position, 0);
  }

  public void addChildren(StoredBoard board) {
    addChildren(new StoredBoardBestDescendant(board.getEvaluation(nextPositionEvalGoal())));
  }
}