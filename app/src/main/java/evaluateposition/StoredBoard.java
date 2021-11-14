// Copyright 2021 Google LLC
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

import androidx.annotation.NonNull;

import bitpattern.BitPattern;
import board.Board;
import board.GetMoves;
import board.GetMovesCache;
import constants.Constants;
import helpers.Utils;
import java.util.ArrayList;

public class StoredBoard {
  public static class ExtraInfo {
    public float minProofGreaterEqual = Float.POSITIVE_INFINITY;
    public float minDisproofStrictlyGreater = Float.POSITIVE_INFINITY;
    public float minProofGreaterEqualVar = Float.POSITIVE_INFINITY;
    public float minDisproofStrictlyGreaterVar = Float.POSITIVE_INFINITY;
    public boolean isFinished = false;
    public boolean proofBeforeFinished = false;
    public boolean disproofBeforeFinished = false;
  }

  public class Evaluation {
    private int prob;
    protected float proofNumber;
    protected float disproofNumber;
    int maxLogDerivative;
    final int evalGoal;
    Evaluation bestChildMidgame;
    Evaluation bestChildProof;
    Evaluation bestChildDisproof;
    float bestDisproofNumberValue = Float.NEGATIVE_INFINITY;
    boolean isLeaf = true;
    long descendants = 0;

    public synchronized float proofNumber() { return proofNumber; }
    public synchronized float disproofNumber() { return disproofNumber; }
    public synchronized int maxLogDerivative() { return maxLogDerivative; }

    public Evaluation(int evalGoal) {
      assert evalGoal <= 6400 && evalGoal >= -6400;
      assert (evalGoal + 6400) % 200 == 100;
      this.evalGoal = evalGoal;
      this.prob = PROB_STEP;
    }

    public synchronized boolean hasValidDescendants() {
      return !isSolved() && threadId == 0;
    }

    public synchronized boolean isSolved() {
      return proofNumber == 0 || disproofNumber == 0;
    }

    public synchronized float getProb() {
      return (float) prob / PROB_STEP;
    }

    synchronized int childLogDerivative(@NonNull Evaluation child) {
      synchronized (child) {
        if (prob == 0 || prob == PROB_STEP || child.prob == 0 || child.prob == PROB_STEP || child.getStoredBoard().threadId != 0) {
          return LOG_DERIVATIVE_MINUS_INF;
        }
        return child.maxLogDerivative - LOG_DERIVATIVE[child.prob] + LOG_DERIVATIVE[PROB_STEP - prob];
      }
    }

    public synchronized Evaluation bestChild() {
      assert EvaluatorMCTS.nextPositionLock.isHeldByCurrentThread();
      assert bestChildDisproof == null || Utils.arrayContains(getStoredBoard().getChildren(), bestChildDisproof.getStoredBoard());
      assert bestChildProof == null || Utils.arrayContains(getStoredBoard().getChildren(), bestChildProof.getStoredBoard());
      assert prob == 0 || prob == PROB_STEP || Utils.arrayContains(getStoredBoard().getChildren(), bestChildMidgame.getStoredBoard());
      if (getProb() == 0) {
        return bestChildDisproof;
      } else if (getProb() > Constants.PROB_FOR_PROOF) {
        return bestChildProof;
      }
      assert bestChildMidgame.maxLogDerivative > LOG_DERIVATIVE_MINUS_INF;
      return bestChildMidgame;
    }

    synchronized void setLeaf(float prob, float proofNumber, float disproofNumber) {
      assert isLeaf;
      assert threadId == Thread.currentThread().getId();
      bestChildMidgame = this;
      bestChildProof = this;
      bestChildDisproof = this;
      this.prob = roundProb(prob);
      this.proofNumber = proofNumber;
      this.disproofNumber = disproofNumber;
      if (getProb() == 0 || getProb() == 1) {
        maxLogDerivative = LOG_DERIVATIVE_MINUS_INF;
      } else {
        maxLogDerivative = (int) Math.round(LOG_DERIVATIVE_MULTIPLIER * (Math.log(getProb() * (1 - getProb()))));
      }
      assert maxLogDerivative < 0;
    }

    StoredBoard getStoredBoard() {
      return StoredBoard.this;
    }

    public synchronized void addDescendants(long newDescendants) {
      if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {
        if (extraInfo.isFinished) {
          return;
        }
      }
      descendants += newDescendants;
    }

    synchronized void setDisproved() {
      setLeaf(0, Float.POSITIVE_INFINITY, 0);
    }

    synchronized void setProved() {
      setLeaf(1, 0, Float.POSITIVE_INFINITY);
    }
//
//    public void setBusy() {
//      synchronized (this) {
//        assert isLeaf;
//      }
//      this.updateFathers();
//    }

    protected synchronized void updateFather() {
      assert !isLeaf;
      proofNumber = Float.POSITIVE_INFINITY;
      disproofNumber = 0;
      prob = 0;
      maxLogDerivative = LOG_DERIVATIVE_MINUS_INF;
      bestDisproofNumberValue = Float.NEGATIVE_INFINITY;
      assert getChildren().length > 0;
      bestChildDisproof = null;
      bestChildProof = null;
      bestChildMidgame = null;
      for (StoredBoard childBoard : getChildren()) {
        Evaluation child = childBoard.getEvaluation(-evalGoal);
        synchronized (child) {
          if (child.getStoredBoard().threadId != 0 || child.isSolved()) {
            if (child.getProb() > 0.5) {
              continue;
            } else {
              prob = PROB_STEP;
              proofNumber = 0;
              disproofNumber = Float.POSITIVE_INFINITY;
              maxLogDerivative = LOG_DERIVATIVE_MINUS_INF;
              bestChildDisproof = null;
              bestChildProof = null;
              bestChildMidgame = null;
              break;
            }
          }
          prob = combineProb(prob, child.prob);

          float curProofNumber = Math.max(1, child.disproofNumber / Math.max(0.0001F, 1 - child.getProb()));
          if (curProofNumber < proofNumber) {
            assert Utils.arrayContains(getChildren(), child.getStoredBoard());
            bestChildProof = child;
            proofNumber = curProofNumber;
            assert !bestChildProof.isSolved();
          }

          disproofNumber += child.proofNumber;
          if (child.proofNumber > bestDisproofNumberValue) {
            assert Utils.arrayContains(getChildren(), child.getStoredBoard());
            bestChildDisproof = child;
            bestDisproofNumberValue = child.proofNumber;
            assert !bestChildDisproof.isSolved();
          }
          if (child.getProb() > 0 && child.getProb() < 1) {
            int currentLogDerivative = child.maxLogDerivative - LOG_DERIVATIVE[child.prob];
            if (currentLogDerivative > maxLogDerivative) {
              assert Utils.arrayContains(getChildren(), child.getStoredBoard());
              bestChildMidgame = child;
              maxLogDerivative = currentLogDerivative;
              assert !bestChildMidgame.isSolved();
            }
          }
        }
      }
      if (getProb() == 1 || getProb() == 0) {
        maxLogDerivative = LOG_DERIVATIVE_MINUS_INF;
      } else {
        maxLogDerivative = maxLogDerivative + LOG_DERIVATIVE[255 - prob];
        assert maxLogDerivative > LOG_DERIVATIVE_MINUS_INF;
      }
      assert isChildLogDerivativeOK();
//      if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {
//        extraInfo.minProofGreaterEqual = Float.POSITIVE_INFINITY;
//        extraInfo.minProofGreaterEqualVar = Float.POSITIVE_INFINITY;
//        extraInfo.minDisproofStrictlyGreater = 0;
//        extraInfo.minDisproofStrictlyGreaterVar = 0;
//        float extraCostDisproofStrictlyGreaterVar = Float.POSITIVE_INFINITY;
//        extraInfo.proofBeforeFinished = false;
//        extraInfo.disproofBeforeFinished = true;
//        for (StoredBoard child : getChildren()) {
//          extraInfo.proofBeforeFinished = extraInfo.proofBeforeFinished || child.extraInfo.disproofBeforeFinished;
//          extraInfo.minProofGreaterEqual = Math.min(extraInfo.minProofGreaterEqual, child.extraInfo.minDisproofStrictlyGreater);
//          extraInfo.minProofGreaterEqualVar = Math.min(extraInfo.minProofGreaterEqualVar, child.extraInfo.minDisproofStrictlyGreaterVar);
//          extraInfo.disproofBeforeFinished =extraInfo.disproofBeforeFinished && child.extraInfo.proofBeforeFinished;
//          extraInfo.minDisproofStrictlyGreater += child.extraInfo.minProofGreaterEqual;
//          extraInfo.minDisproofStrictlyGreaterVar += Math.min(child.extraInfo.minProofGreaterEqual, child.extraInfo.minProofGreaterEqualVar);
//          if (extraInfo.minDisproofStrictlyGreaterVar == Float.POSITIVE_INFINITY) {
//            extraCostDisproofStrictlyGreaterVar = 0;
//          } else {
//            extraCostDisproofStrictlyGreaterVar = Math.min(extraCostDisproofStrictlyGreaterVar, child.extraInfo.minProofGreaterEqualVar - child.extraInfo.minProofGreaterEqual);
//          }
//        }
//        extraInfo.minDisproofStrictlyGreaterVar += Math.max(0, extraCostDisproofStrictlyGreaterVar);
//        assert isExtraInfoOK();
//      }
    }

    private boolean isChildLogDerivativeOK() {
      if (isLeaf) {
        return true;
      }
      int expectedLogDerivative = LOG_DERIVATIVE_MINUS_INF;
      String childLogDerivatives = "";
      for (StoredBoard child : children) {
        Evaluation childEval = child.getEvaluation(-evalGoal);
        if (childEval.getProb() > 0 && childEval.getProb() < 1) {
          assert childLogDerivative(childEval) <= child.maxLogDerivative(-evalGoal);
          childLogDerivatives += "  " + childLogDerivative(childEval) + "\n";
          expectedLogDerivative = Math.max(expectedLogDerivative, childLogDerivative(childEval));
        }
      }
      if (maxLogDerivative != expectedLogDerivative) {
        throw new AssertionError("Wrong maxLogDerivative. Expected " + expectedLogDerivative + ", got " + maxLogDerivative + ".\n" + childLogDerivatives);
      }
      return maxLogDerivative == expectedLogDerivative;
    }

    protected void updateFathers() {
      for (int i = 0; i < fathers.size(); ++i) {
        Evaluation father;
        synchronized (StoredBoard.this) {
          father = fathers.get(i).getEvaluation(-evalGoal);
        }
        if (father != null && !father.isLeaf) {
          father.updateFather();
          father.updateFathers();
        }
      }
    }

    public long getDescendants() {
      return descendants;
    }
  }

  protected static final HashMap HASH_MAP;
  protected static EvaluatorInterface EVALUATOR_MIDGAME;
  
  static {
    HASH_MAP = new HashMap(6000);
    EVALUATOR_MIDGAME = new EvaluatorAlphaBeta(HASH_MAP);
  }
  
  private final long player;
  private final long opponent;

  final static EndgameTimeEstimatorInterface endgameTimeEstimator = new EndgameTimeEstimator();
  public final ArrayList<StoredBoard> fathers;
  StoredBoard[] children;

  int nEmpties;
  private Evaluation[] evaluations = new Evaluation[64];
  public boolean playerIsStartingPlayer;
  
  private static final int PROB_STEP = 255;
  private static final int[] COMBINE_PROB;
  private static final int[] LOG_DERIVATIVE;
  public static final int LOG_DERIVATIVE_MINUS_INF = -20000000;
  private static final int LOG_DERIVATIVE_MULTIPLIER = 10000;

  static {
    COMBINE_PROB = new int[(PROB_STEP + 1) * (PROB_STEP + 1)];
    LOG_DERIVATIVE = new int[(PROB_STEP + 1)];
    for (int i = 0; i <= PROB_STEP; ++i) {
      if (i == 0 || i == PROB_STEP) {
        LOG_DERIVATIVE[i] = LOG_DERIVATIVE_MINUS_INF;
      } else {
        LOG_DERIVATIVE[i] =
            // (1 - lambda) * log((1 - p_c) / p_c).
            // <= 0, >= (1 - (-2)) * log(1/255 / 1) >= -17
            // Max value: -17 * 100 * 60 = 102000. Should be fine with 2M
            (int) Math.round(LOG_DERIVATIVE_MULTIPLIER * (1 - Constants.LAMBDA) * Math.log(((double) i) / PROB_STEP));
//            (int) Math.round(
//                LOG_DERIVATIVE_MULTIPLIER * (1 - Constants.LAMBDA) * Math.log((PROB_STEP - i) / (float) i));
      }
      for (int j = 0; j <= PROB_STEP; ++j) {
        COMBINE_PROB[(i << 8) | j] =
            PROB_STEP - (int) Math.round(
                Math.pow(
                    Math.pow(1 - i / (double) PROB_STEP, Constants.LAMBDA) +
                    Math.pow(j / (double) PROB_STEP, Constants.LAMBDA) - 1,
                    1 / Constants.LAMBDA
                ) * PROB_STEP);
        if (COMBINE_PROB[(i << 8) | j] / (double) PROB_STEP <= Constants.MIN_PROB) {
          COMBINE_PROB[(i << 8) | j] = 0;
        }
        if (COMBINE_PROB[(i << 8) | j] / (double) PROB_STEP >= 1 - Constants.MIN_PROB) {
          COMBINE_PROB[(i << 8) | j] = 255;
        }
      }
    }
  }
  private static int combineProb(int p1, int p2) {
    return COMBINE_PROB[(p1 << 8) | p2];
  }

  static int toEvaluationIndex(int eval) {
    assert ((eval + 6300) % 200 == 0);
    assert (eval >= -6300);
    assert (eval <= 6300);
    return (eval + 6300) / 200;
  }

  StoredBoard next;
  StoredBoard prev;
  public ExtraInfo extraInfo;
  public long threadId;

  protected StoredBoard(long player, long opponent, boolean playerIsStartingPlayer) {
    this.player = player;
    this.opponent = opponent;
    this.playerIsStartingPlayer = playerIsStartingPlayer;
    this.fathers = new ArrayList<>();
    this.children = null;
    this.prev = null;
    this.next = null;
    this.nEmpties = 64 - Long.bitCount(player | opponent);
    if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {
      extraInfo = new ExtraInfo();
    }
  }
  
  public static StoredBoard initialStoredBoard(Board b) {
    return initialStoredBoard(b.getPlayer(), b.getOpponent());
  }
  
  public static StoredBoard initialStoredBoard(long player, long opponent) {
    return new StoredBoard(player, opponent, true);
  }

  public synchronized void setIsFinished(boolean newValue) {
    if (newValue == this.extraInfo.isFinished) {
      return;
    }
    this.extraInfo.isFinished = true;
    if (getChildren() != null) {
      for (StoredBoard child : getChildren()) {
        child.setIsFinished(newValue);
      }
    }
  }
  
  public synchronized void addFather(StoredBoard father) {
    fathers.add(father);
  }

  public synchronized boolean addChildren(EvaluatorMCTS evaluator) {
    assert threadId == Thread.currentThread().getId();
    if (getChildren() != null) {
      return true;
    }
    long[] moves = GetMovesCache.getAllMoves(player, opponent);
    if (moves == null) {
      return false;
    }
    children = new StoredBoard[moves.length];

    for (int i = 0; i < moves.length; ++i) {
      long flip = moves[i];
      long newPlayer = opponent & ~flip;
      long newOpponent = player | flip;
      StoredBoard child = evaluator.get(newPlayer, newOpponent);
      if (child == null || Constants.IGNORE_TRANSPOSITIONS) {
        child = new StoredBoard(newPlayer, newOpponent, !playerIsStartingPlayer);
        evaluator.add(child);
      }
      child.addFather(this);
      children[i] = child;
    }
    return true;
  }
  
  public Board getBoard() {
    return new Board(player, opponent);
  }
  
  public long getPlayer() {
    return player;
  }
  
  public long getOpponent() {
    return opponent;
  }

  public synchronized void setBusyNoUpdate() {
    assert threadId == 0;
    threadId = Thread.currentThread().getId();
  }
  public void setBusy(int alpha, int beta) {
    setBusyNoUpdate();
    for (int i = alpha; i <= beta; i += 200) {
      Evaluation eval = getEvaluation(i);
      if (eval != null) {
        eval.updateFathers();
      }
    }
  }

  public synchronized void setFreeNoUpdate() {
    assert threadId == Thread.currentThread().getId();
    threadId = 0;
  }

  public void setFree(int alpha, int beta) {
    setFreeNoUpdate();
    for (int i = alpha; i <= beta; i += 200) {
      Evaluation eval = getEvaluation(i);
//      if (eval != null) {
      eval.updateFathers();
//      }
    }
  }

  public int getEval() {
    float eval = 0;
    float lastProb = 1;
    int weakLower = getWeakLower();
    int weakUpper = getWeakUpper();
    if (weakLower == weakUpper) {
      return weakLower;
    }
    for (int evalGoal = weakLower + 100; evalGoal <= weakUpper + 100; evalGoal += 200) {
      if (evalGoal > 6300) {
        return -6500;
      }
      Evaluation curEval = getEvaluation(evalGoal);
      if (curEval == null) {
        return -6500;
      }

      float curProb = curEval.getProb();
      eval += (evalGoal - 100) * Math.abs(lastProb - curProb);
      lastProb = curProb;
    }
    return Math.round(eval);
  }

  public float getProb(int evalGoal) {
    return getEvaluation(evalGoal).getProb();
  }

  public float getDisproofNumber(int evalGoal) {
    return getEvaluation(evalGoal).disproofNumber;
  }

  public float getProofNumber(int evalGoal) {
    return getEvaluation(evalGoal).proofNumber;
  }

  public boolean isSolved() {
    return getLower() == getUpper();
  }

  public synchronized boolean isPartiallySolved() {
    return getWeakLower() == getWeakUpper();
  }

  public synchronized int getPercentileUpper(float p) {
    for (int eval = -6300; eval < 6300; eval += 200) {
      Evaluation curEval = getEvaluation(eval);
      if (curEval != null && curEval.getProb() <= p) {
        return eval - 100;
      }
    }
    return 6400;
  }
  public synchronized int getPercentileLower(float p) {
    for (int eval = 6300; eval > -6300; eval -= 200) {
      Evaluation curEval = getEvaluation(eval);
      if (curEval != null && curEval.getProb() >= 1 - p) {
        return eval + 100;
      }
    }
    return -6400;
  }

  public synchronized int getWeakUpper() {
    return getPercentileUpper(0);
  }
  public synchronized int getWeakLower() {
    return getPercentileLower(0);
  }

  public int getUpper() {
    for (int eval = -6300; eval < 6300; eval += 200) {
      Evaluation curEval = getEvaluation(eval);
      if (curEval != null && curEval.disproofNumber == 0) {
        return eval - 100;
      }
    }
    return 6400;
  }

  public int getLower() {
    for (int eval = 6300; eval > -6300; eval -= 200) {
      Evaluation curEval = getEvaluation(eval);
      if (curEval != null && curEval.proofNumber == 0) {
        return eval + 100;
      }
    }
    return -6400;
  }
  
  public long getDescendants() {
    long descendants = 0;
    for (Evaluation eval : evaluations) {
      if (eval != null) {
        descendants += eval.descendants;
      }
    }
    return descendants;
  }

  public final void setSolved(int newEval) {
    setUpper(newEval);
    setLower(newEval);
  }

  public final void setLower(int newLower) {
    for (int eval = -6300; eval < newLower; eval += 200) {
      Evaluation e = getEvaluation(eval);
      if (e != null) {
        e.setProved();
      }
    }
  }

  public final void setUpper(int newUpper) {
    for (int eval = newUpper + 100; eval <= 6300; eval += 200) {
      Evaluation e = getEvaluation(eval);
      if (e != null) {
        e.setDisproved();
      }
    }
  }
//
//  public void setFree(int evalGoal) {
//    getEvaluation(evalGoal).setFree();
//  }

//  public void updateAndSetFree(int eval, int evalGoal) {
//    Evaluation evaluation = getEvaluation(evalGoal);
//    evaluation.updateAndSetFree(prob, proofNumber, disproofNumber);
//  }

  public synchronized Evaluation getEvaluation(int evalGoal) {
    return evaluations[toEvaluationIndex(evalGoal)];
  }

  public synchronized Evaluation addEvaluation(int evalGoal) {
    assert threadId == Thread.currentThread().getId();
    int index = toEvaluationIndex(evalGoal);
    if (evaluations[index] != null) {
      return null;
    }
    Evaluation eval = new Evaluation(evalGoal);
    evaluations[index] = eval;
    return eval;
  }
//
//  public void setBusy(int evalGoal) {
//    assert !EvaluatorMCTS.nextPositionLock.isHeldByCurrentThread();
//    getEvaluation(evalGoal).setBusy();
//  }

  private static int roundProb(float prob) {
    if (prob <= Constants.MIN_PROB_LEAF) {
      return 0;
    } else if (prob >= 1 - Constants.MIN_PROB_LEAF) {
      return PROB_STEP;
    }
    return Math.round(prob * PROB_STEP);
  }

  public int maxLogDerivative(int evalGoal) {
    Evaluation eval = getEvaluation(evalGoal);
    return eval.maxLogDerivative();
  }

  public int childLogDerivative(StoredBoard child, int evalGoal) {
    return child.maxLogDerivative(-evalGoal);
  }

  @NonNull
  @Override
  public String toString() {
    return new Board(player, opponent).toString() + "\n";
  }
  
  public synchronized StoredBoard[] getChildren() {
    return children;
  }

  // TODO!!!
//  public StoredBoard bestChildProofStrictlyGreater() {
//    StoredBoard best = null;
//    float bestValue = Float.POSITIVE_INFINITY;
//    for (StoredBoard child : children) {
//      assert child.extraInfo.minProofGreaterEqual < Float.POSITIVE_INFINITY;
//      if (child.extraInfo.minProofGreaterEqualVar - child.extraInfo.minProofGreaterEqual < bestValue) {
//        best = child;
//        bestValue = child.extraInfo.minProofGreaterEqualVar - child.extraInfo.minProofGreaterEqual;
//      }
//    }
//    return best;
//  }
//
//  public StoredBoard bestChildProofGreaterEqual() {
//    StoredBoard best = null;
//    float bestValue = Float.POSITIVE_INFINITY;
//    for (StoredBoard child : children) {
//      if (child.extraInfo.minDisproofStrictlyGreaterVar < bestValue) {
//        best = child;
//        bestValue = child.extraInfo.minDisproofStrictlyGreaterVar;
//      }
//    }
//    return best;
//  }
  
  public float maxFiniteChildrenProofNumber(int evalGoal) {
    float result = 0;
    for (StoredBoard child : getChildren()) {
      if (child.getProofNumber(evalGoal) != Float.POSITIVE_INFINITY) {
        result = Math.max(result, child.getProofNumber(evalGoal));
      }
    }
    return result;
  }

  boolean areDescendantsOK() {
    if (fathers.isEmpty() || Constants.MAX_PARALLEL_TASKS > 1) {
      return true;
    }
    int maxDescendants = 0;
    String fatherDesc = "";
    for (StoredBoard father : fathers) {
      maxDescendants += father.getDescendants();
      fatherDesc += " " + father.getDescendants();
    }
    if (getDescendants() > maxDescendants) {
      throw new AssertionError(
          "Bad number of descendants " + getDescendants() + " > SUM_father descendants = "
              + maxDescendants + ". Father descendants: " + fatherDesc);
    }
    return true;
  }

  boolean isChildOK(StoredBoard child) {
    if (Constants.MAX_PARALLEL_TASKS > 1) {
      return true;
    }
    boolean found = false;
    for (StoredBoard father : child.fathers) {
      if (father == this) {
        if (found) {
          throw new AssertionError("Found the same father twice. Father:\n" + this + "\nchild:\n" + child);
        }
        found = true;
      }
    }
    if (!found) {
      throw new AssertionError("Did not find father\n" + child + " for board\n" + this);
    }
    return 
        child.playerIsStartingPlayer == !playerIsStartingPlayer
        && Utils.arrayContains(getChildren(), child);
  }

  boolean areChildrenOK() {
    if (getChildren() == null) {
      return true;
    }
    for (StoredBoard child : getChildren()) {
      if (!isChildOK(child)) {
        throw new AssertionError("Child\n" + child + "\n of board\n" + this + "\nis not OK");
      }
    }
    if (getChildren().length != Long.bitCount(GetMoves.getMoves(player, opponent)) && !(
        Long.bitCount(GetMoves.getMoves(player, opponent)) == 0
        && Long.bitCount(GetMoves.getMoves(opponent, player)) != 0
        && getChildren().length == 1)) {
      throw new AssertionError(
          "Board " + this + " should have " + Long.bitCount(GetMoves.getMoves(player, opponent))
              + " children. Found " + getChildren().length);
    }
    return true;
  }

  boolean isPrevNextOK() {
    return 
        prev != this
        && next != this
        && (prev == null || prev.next == this)
        && (next == null || next.prev == this);
  }
  
  boolean isEvalOK(int evalGoal) {
    if (Constants.MAX_PARALLEL_TASKS > 1) {
      return true;
    }
    if ((new GetMovesCache()).getNMoves(player, opponent) == 0
        && (new GetMovesCache()).getNMoves(opponent, player) == 0
        && (this.getProofNumber(evalGoal) == 0 || this.getProofNumber(evalGoal) == Float.POSITIVE_INFINITY)) {
      int correctEval = BitPattern.getEvaluationGameOver(player, opponent);
      return true;
    }
    return true;
  }
  
  boolean isExtraInfoOK() {
    if (!Constants.FIND_BEST_PROOF_AFTER_EVAL) {
      return true;
    }
    if (Float.isNaN(extraInfo.minDisproofStrictlyGreaterVar) || Float.isNaN(extraInfo.minProofGreaterEqualVar)) {
      throw new AssertionError(
          "Got (minDisproofVar, minProofVar) = " + extraInfo.minDisproofStrictlyGreaterVar + " " + extraInfo.minProofGreaterEqualVar
      );      
    }
    
    return true;
  }
  
  boolean isAllOK() {
    if (!isPrevNextOK()) {
      throw new AssertionError("Wrong isPrevNextOK");
    }
    if (!areChildrenOK()) {
      throw new AssertionError("Wrong areChildrenOK");      
    }
    for (int goal = -6300; goal <= 6300; goal += 200) {
      assert isEvalOK(goal);
    }
    if (!areDescendantsOK()) {
      throw new AssertionError("Wrong areThisBoardEvalsOK or isEvalOK or areDescendantsOK.");
    }
    return isExtraInfoOK();// && isLeastSingleAncestorOK();
  }
}