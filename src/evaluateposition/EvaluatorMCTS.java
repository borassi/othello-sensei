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
import java.util.ArrayList;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReadWriteLock;
import java.util.concurrent.locks.ReentrantReadWriteLock;
import java.util.function.Supplier;
import java.util.logging.Level;
import java.util.logging.Logger;

public class EvaluatorMCTS extends HashMapVisitedPositions {
  PatternEvaluatorImproved evaluator;

  private EvaluatorInterface nextEvaluator;
  private long maxNVisited;
  private long stopTimeMillis;
  private HashMap hashMap;
  private final ReadWriteLock editNextPositionLock = new ReentrantReadWriteLock();
  private final Lock editLock = editNextPositionLock.readLock();
  private final Lock nextPositionLock = editNextPositionLock.writeLock();
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
  private long nextUpdateEvalGoal;
  private final EvaluatorThread[] threads;
  private double constant = 0;

  private class EvaluatorThread implements Runnable {    
    private final EvaluatorInterface nextEvaluator;
    private final EvaluatorMCTS main;

    private EvaluatorThread(EvaluatorInterface nextEvaluator, EvaluatorMCTS main) {
      this.nextEvaluator = nextEvaluator;
      this.main = main;
    }

    public long addChildren(StoredBoardBestDescendant position) {
      StoredBoard board = position.board;
      assert board.isLeaf();
      assert board.isBusy;
      long player = board.getPlayer();
      long opponent = board.getOpponent();
      int depth = board.nEmpties < 22 ? 2 : 4;
      // Includes passes.
      long[] moves = GetMovesCache.getAllMoves(player, opponent);

      if (moves == null) {
        board.setSolved(BitPattern.getEvaluationGameOver(player, opponent));
        return 0;
      }
      StoredBoard[] children = new StoredBoard[moves.length];
      long nVisited = 0;
      for (int i = 0; i < moves.length; ++i) {
        long flip = moves[i];
        long newPlayer = opponent & ~flip;
        long newOpponent = player | flip;
        StoredBoard child = board.getChild(newPlayer, newOpponent);
        int eval = nextEvaluator.evaluate(newPlayer, newOpponent, depth, -6400, 6400);
        child.addDescendants(nextEvaluator.getNVisited() + 1);
        child.setEval(eval);
        nVisited += child.getDescendants();
        children[i] = child;
        assert !child.isBusy;
      }
      board.children = children;
      return nVisited;
    }

    public long solvePosition(StoredBoardBestDescendant position) {
      StoredBoard board = position.board;
      int alpha = position.getAlpha();
      int beta = position.getBeta();
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
      return seenPositions;
    }

    public long deepenPosition(StoredBoardBestDescendant position) {
      StoredBoard board = position.board;
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
      board.setEval(curEval);
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
        if (position.board != firstPosition && toBeSolved(position.board)) {
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
    this.firstPosition = StoredBoard.initialStoredBoard(new Board());
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

  int roundEval(int eval) {
    assert (lower + 6400) % 200 == 100;
    assert (upper + 6400) % 200 == 100;
    assert lower >= -6300 && lower <= 6300;
    assert upper >= -6300 && upper <= 6300;
    if (eval <= lower) {
      return lower - 100;
    }
    if (eval > upper) {
      return upper + 100;
    }
    return ((eval + 6499) / 200) * 200 - 6400;
  }
  
  protected void setEvalGoal(int evalGoal) {
    evalGoal = roundEval(evalGoal);
    assert(evalGoal >= lower - 100 && evalGoal <= upper + 100);
    assert evalGoal % 200 == 0;
//    if (evalGoal != this.getEvalGoal()) {
      this.nextUpdateEvalGoal = (long) (this.firstPosition.getDescendants() * 1.1);
      this.firstPosition.setEvalGoalRecursive(roundEval(evalGoal));
//    }
  }
  
  public void updateEvalGoalIfNeeded() {
    int evalGoal = firstPosition.getEvalGoal();
    if (this.firstPosition.getProbStrictlyGreater() == 1 && evalGoal <= upper) {
      this.setEvalGoal(evalGoal + 200);
//        System.out.println("Eval goal forced " + this.firstPosition.getEvalGoal() + " after " + this.firstPosition.descendants);
    }
    if (this.firstPosition.getProbGreaterEqual() == 0 && evalGoal >= lower) {
      this.setEvalGoal(evalGoal - 200);
//        System.out.println("Eval goal forced " + this.firstPosition.getEvalGoal() + " after " + this.firstPosition.descendants);
    }
    if (this.firstPosition.getDescendants() > nextUpdateEvalGoal) {
      if (this.firstPosition.getEval() >= evalGoal + 200 && evalGoal + 200 <= upper) {
        this.setEvalGoal(evalGoal + 200);
//        System.out.println("Eval goal " + this.firstPosition.getEvalGoal() + " after " + this.firstPosition.descendants);
      }
      if (this.firstPosition.getEval() <= evalGoal - 200 && evalGoal - 200 >= lower) {
        this.setEvalGoal(evalGoal - 200);
//        System.out.println("Eval goal " + this.firstPosition.getEvalGoal() + " after " + this.firstPosition.descendants);
      }
    }
  }
  
  public void stop() {
    status = Status.KILLING;
  }

  private boolean positionToImproveAndEvalGoalAreOk(StoredBoard position, boolean playerVariates) {
    int evalGoal = position.evalGoal;
    int posUpper = position.playerIsStartingPlayer ? upper : -lower;
    int posLower = position.playerIsStartingPlayer ? lower : -upper;

    if (playerVariates) {
      if (evalGoal >= posUpper) {
        throw new AssertionError(
            "Player should not try to prove that [value > ] evalGoal (" + evalGoal
            + ") >= upper (" + posUpper + ") in board\n" + position.getBoard());
      }      
    } else {
      if (evalGoal <= posLower) {
        throw new AssertionError(
            "Opponent should not try to prove that [value < ] evalGoal (" + evalGoal
            + ") <= lower (" + posLower + ") in board\n" + position.getBoard());
      }
    }
    return true;
  }

  public boolean nextPositionGreaterEqual() {
    if (firstPosition.getEvalGoal() > upper || firstPosition.getEvalGoal() >= firstPosition.upper) {
      return true;
    } else if (firstPosition.getEvalGoal() <= lower || firstPosition.getEvalGoal() <= firstPosition.lower) {
      return false;
    }
    if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {
      if (this.firstPosition.isSolved()) {
        return firstPosition.extraInfo.minDisproofStrictlyGreater - firstPosition.extraInfo.minDisproofStrictlyGreaterVar <
            firstPosition.extraInfo.minProofGreaterEqual - firstPosition.extraInfo.minProofGreaterEqualVar;
      }
    }
    if (firstPosition.isLeaf()) {
      return true;
    }
    if (this.firstPosition.getProbGreaterEqual() == 1 && this.firstPosition.getProbStrictlyGreater() == 0) {
      return this.firstPosition.proofNumberGreaterEqual > this.firstPosition.disproofNumberStrictlyGreater;
    }
    return this.firstPosition.getProbGreaterEqual() < 1 - this.firstPosition.getProbStrictlyGreater();
  }
  
  protected void finalizePosition(StoredBoardBestDescendant position, long nVisited) {
    editLock.lock();
    try {
      StoredBoard board = position.board;
      if (board.children != null) {
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
            childInEvaluator.addFather(board);
            if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {
              childInEvaluator.extraInfo.isFinished = board.extraInfo.isFinished;
            }
            if (board.getEvalGoal() != -childInEvaluator.getEvalGoal()) {
              childInEvaluator.setEvalGoalRecursive(-board.getEvalGoal());
            }
          }
        }
      }
      position.updateDescendants(nVisited);
      board.setFree();
    } finally {
      editLock.unlock();
    }

    nextPositionLock.lock();
    try {
      isNextPositionAvailable.signalAll();
    } finally {
      nextPositionLock.unlock();
    }
  }
  
  protected boolean checkFinished() {
    updateEvalGoalIfNeeded();
    if (hashMap.size() > Constants.HASH_MAP_SIZE / 2) {
      hashMap.reset();
    }

    if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {
      if (this.firstPosition.isSolved()) {
        this.firstPosition.setIsFinished(true);
      }
    }

    if (Constants.APPROX_ONLY && this.firstPosition.getProbGreaterEqual() > 1 - 0.02 && this.firstPosition.getProbStrictlyGreater() < 0.02) {
      status = Status.SOLVED;
      return true;
    }
    if (status == Status.KILLING) {
      status = Status.KILLED;
      return true;
    }
    if (this.firstPosition.getDescendants() > maxNVisited) {
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
    nextPositionLock.lock();
    StoredBoardBestDescendant result = null;
    try {
      if (checkFinished()) {
        return null;
      }
      result = StoredBoardBestDescendant.bestDescendant(firstPosition);
      while (result == null) {
        isNextPositionAvailable.await();
        if (checkFinished()) {
          return null;
        }
        result = StoredBoardBestDescendant.bestDescendant(firstPosition);
      }
      editLock.lock();
    } catch (InterruptedException ex) {
      Logger.getLogger(EvaluatorMCTS.class.getName()).log(Level.SEVERE, null, ex);
    } finally {
      nextPositionLock.unlock();
    }
    try {
      result.board.setBusy();
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
    firstPosition.eval = quickEval;
    firstPosition.addDescendants(nextEvaluator.getNVisited() + 1);
    firstPosition.setEvalGoalRecursive(roundEval(quickEval));
    
    add(firstPosition);
  }

  public boolean isSolved() {
    return this.firstPosition.isSolved() || this.firstPosition.getUpper() <= lower || this.firstPosition.getLower() >= upper;
  }

  public boolean toBeSolved(StoredBoard b) {
//    System.out.println(constant);
    return b.nEmpties <= Constants.EMPTIES_FOR_FORCED_MIDGAME + 3
        && (
        (b.getProofNumberGreaterEqual() < constant && b.getEval() > b.getEvalGoal() + 1000) ||
        (b.getDisproofNumberStrictlyGreater() < constant && b.getEval() < b.getEvalGoal() - 1000) ||
        b.nEmpties <= Constants.EMPTIES_FOR_FORCED_MIDGAME);    
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
      if (boardStored == null) {
        setFirstPosition(board.getPlayer(), board.getOpponent());
      }
    }

    StoredBoard[] firstPositionChildren = firstPosition.children;
    if (firstPosition.fathers.isEmpty() && firstPosition.getChildren() != null) {
      ArrayList<StoredBoard> firstPositionUnsolvedChildren = new ArrayList<>();
      for (StoredBoard child : firstPosition.children) {
        if (!child.isSolved()) {
          firstPositionUnsolvedChildren.add(child);
        }
      }
      firstPosition.children = firstPositionUnsolvedChildren.toArray(new StoredBoard[firstPositionUnsolvedChildren.size()]);
      firstPosition.updateFather();
    }
    setEvalGoal(firstPosition.getEval());
    
    Thread threads[] = new Thread[this.threads.length];
    
    for (int i = 0; i < threads.length; ++i) {
      threads[i] = new Thread(this.threads[i]);
      threads[i].start();
    }
    for (Thread t : threads) {
      try {
        t.join();
      } catch (InterruptedException ex) {
        Logger.getLogger(EvaluatorMCTS.class.getName()).log(Level.SEVERE, null, ex);
      }
    }

    if (firstPositionChildren != null) {
      firstPosition.children = firstPositionChildren;
      firstPosition.updateFather();
    }
    es.shutdown();
    return (short) -firstPosition.getEval();
  }
  
  public int getEvalGoal() {
    return firstPosition.getEvalGoal();
  }

  public StoredBoard getFirstPosition() {
    return firstPosition;
  }

  public void addChildren(StoredBoardBestDescendant position) {
    position.board.setBusy();
    threads[0].addChildren(position);
    finalizePosition(position, 0);
  }

  public void addChildren(StoredBoard board) {
    addChildren(new StoredBoardBestDescendant(board, true));
  }
}