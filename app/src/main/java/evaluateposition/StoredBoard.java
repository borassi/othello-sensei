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

import com.sun.xml.internal.bind.v2.runtime.reflect.opt.Const;

import bitpattern.BitPattern;
import board.Board;
import board.GetMoves;
import board.GetMovesCache;
import constants.Constants;
import helpers.Gaussian;
import helpers.Utils;
import java.util.ArrayList;

public class StoredBoard {

  private static final int[] ERRORS = {
      0, 0, 0, 0, 530, 585, 636, 696, 736, 780, 794, 828, 814, 821, 807, 797, 772, 768, 753, 732,
      709, 680, 552, 474, 480, 448, 461, 397, 429, 380, 403, 362, 372, 328, 338, 300, 332, 303,
      311, 290, 300, 265, 267, 249, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250,
      250, 250, 250, 250, 250};

//  public static class ExtraInfo {
//    public float minProofGreaterEqual = Float.POSITIVE_INFINITY;
//    public float minDisproofStrictlyGreater = Float.POSITIVE_INFINITY;
//    public float minProofGreaterEqualVar = Float.POSITIVE_INFINITY;
//    public float minDisproofStrictlyGreaterVar = Float.POSITIVE_INFINITY;
//    public boolean isFinished = false;
//    public boolean proofBeforeFinished = false;
//    public boolean disproofBeforeFinished = false;
//  }

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
    long descendants = 0;

    public float proofNumber() { return proofNumber; }
    public float disproofNumber() { return disproofNumber; }
    public int maxLogDerivative() { return maxLogDerivative; }

    public Evaluation(int evalGoal) {
      assert evalGoal <= 6400 && evalGoal >= -6400;
      assert (evalGoal + 6400) % 200 == 100;
      this.evalGoal = evalGoal;
      this.prob = PROB_STEP;
    }

    public boolean hasValidDescendants() {
      return !isSolved() && threadId == 0;
    }

    public boolean isSolved() {
      return proofNumber == 0 || disproofNumber == 0;
    }

    public float getProb() {
      return (float) prob / PROB_STEP;
    }

    int childLogDerivative(@NonNull Evaluation child) {
      synchronized (child) {
        if (prob == 0 || prob == PROB_STEP || child.prob == 0 || child.prob == PROB_STEP || child.getStoredBoard().threadId != 0) {
          return LOG_DERIVATIVE_MINUS_INF;
        }
        return child.maxLogDerivative - LOG_DERIVATIVE[child.prob] + LOG_DERIVATIVE[PROB_STEP - prob];
      }
    }

    public Evaluation bestChild() {
      assert !isLeaf();
      assert EvaluatorMCTS.nextPositionLock.isHeldByCurrentThread();
      assert bestChildDisproof == null || Utils.arrayContains(getStoredBoard().getChildren(), bestChildDisproof.getStoredBoard());
      assert bestChildProof == null || Utils.arrayContains(getStoredBoard().getChildren(), bestChildProof.getStoredBoard());
      assert prob == 0 || prob == PROB_STEP || Utils.arrayContains(getStoredBoard().getChildren(), bestChildMidgame.getStoredBoard());
      if (getProb() == 0) {
        return bestChildDisproof;
      } else if (getProb() >= 1) {
        return bestChildProof;
      }
      assert bestChildMidgame.getStoredBoard().threadId == 0;
      assert bestChildMidgame.maxLogDerivative > LOG_DERIVATIVE_MINUS_INF;
      return bestChildMidgame;
    }

    public Evaluation bestChildProof() {
      if (getProb() < 0.5) {
        return bestChild();
      }
      return bestChildProof;
    }

    void setLeaf(float prob, float proofNumber, float disproofNumber) {
      assert isLeaf();
      assert threadId == Thread.currentThread().getId() || EvaluatorMCTS.nextPositionLock.isHeldByCurrentThread();
      bestChildMidgame = null;
      bestChildProof = null;
      bestChildDisproof = null;
      this.prob = roundProb(prob);
      this.proofNumber = proofNumber;
      this.disproofNumber = disproofNumber;
      if (getProb() == 0 || getProb() == 1) {
        maxLogDerivative = LOG_DERIVATIVE_MINUS_INF;
      } else {
        maxLogDerivative = (int) Math.round(LOG_DERIVATIVE_MULTIPLIER * Constants.LEAF_MULTIPLIER * (Math.log(getProb() * (1 - getProb()))));
      }
      assert maxLogDerivative < 0;
    }

    StoredBoard getStoredBoard() {
      return StoredBoard.this;
    }

    public void addDescendants(long newDescendants) {
//      if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {
//        if (extraInfo.isFinished) {
//          return;
//        }
//      }
      descendants += newDescendants;
    }

    void setDisproved() {
      setLeaf(0, Float.POSITIVE_INFINITY, 0);
    }

    void setProved() {
      setLeaf(1, 0, Float.POSITIVE_INFINITY);
    }

    protected void updateFather() {
      assert !isLeaf();
      assert Thread.holdsLock(StoredBoard.this);
      proofNumber = Float.POSITIVE_INFINITY;
      disproofNumber = 0;
      prob = PROB_STEP;
      maxLogDerivative = LOG_DERIVATIVE_MINUS_INF;
      bestDisproofNumberValue = Float.NEGATIVE_INFINITY;
      assert getChildren().length > 0;
      bestChildDisproof = null;
      bestChildProof = null;
      bestChildMidgame = null;
      for (StoredBoard childBoard : getChildren()) {
        synchronized (childBoard) {
          Evaluation child = childBoard.getEvaluation(-evalGoal);
          assert child != null;
          if (childBoard.threadId != 0 || child.isSolved()) {
            if (child.getProb() > 0.5) {
              continue;
            } else {
              prob = 0;
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
          assert disproofNumber > 0;
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
        maxLogDerivative = maxLogDerivative + LOG_DERIVATIVE[prob];
        assert maxLogDerivative > LOG_DERIVATIVE_MINUS_INF;
      }
      prob = PROB_STEP - prob;
      if (Constants.MAX_PARALLEL_TASKS == 1) {
        assert bestChildMidgame == null || bestChildMidgame.getStoredBoard().threadId == 0;
        assert bestChildProof == null || bestChildProof.getStoredBoard().threadId == 0;
        assert bestChildDisproof == null || bestChildDisproof.getStoredBoard().threadId == 0;
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
      if (isLeaf() || !EvaluatorMCTS.nextPositionLock.isHeldByCurrentThread()) {
        return true;
      }
      int expectedLogDerivative = LOG_DERIVATIVE_MINUS_INF;
      String childLogDerivatives = "";
      for (StoredBoard child : children) {
        Evaluation childEval = child.getEvaluation(-evalGoal);
        if (childEval.getStoredBoard().threadId == 0 && childEval.getProb() > 0 && childEval.getProb() < 1) {
          assert childLogDerivative(childEval) <= child.maxLogDerivative(-evalGoal);
          childLogDerivatives += "  " + childLogDerivative(childEval) + "\n";
          expectedLogDerivative = Math.max(expectedLogDerivative, childLogDerivative(childEval));
        }
      }
      if (maxLogDerivative != expectedLogDerivative) {
        throw new AssertionError("Wrong maxLogDerivative. Expected " + expectedLogDerivative + ", got " + maxLogDerivative + ".\n" + childLogDerivatives);
      }
      return true;
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
  private final Evaluation[] evaluations = new Evaluation[64];
  public int depth;

  StoredBoard next;
  StoredBoard prev;
//  public ExtraInfo extraInfo;
  public long threadId;
  int leafEval = -6500;
  int lower = -6400;
  int upper = 6400;
  
  private static final int PROB_STEP = 255;
  private static final int[] COMBINE_PROB;
  private static final int[] LOG_DERIVATIVE;
  public static final int LOG_DERIVATIVE_MINUS_INF = -20000000;
  public static final int LOG_DERIVATIVE_MULTIPLIER = 10000;

  static {
    ProbCombiner combiner = Constants.PROB_COMBINER;
    COMBINE_PROB = new int[(PROB_STEP + 1) * (PROB_STEP + 1)];
    LOG_DERIVATIVE = new int[(PROB_STEP + 1)];
    for (int i = 0; i <= PROB_STEP; ++i) {
      double x = i / (double) PROB_STEP;
      LOG_DERIVATIVE[i] = (int) Math.round(Math.max(LOG_DERIVATIVE_MINUS_INF,
          -LOG_DERIVATIVE_MULTIPLIER * Math.log(combiner.derivative(x) / combiner.f(x))));

      for (int j = 0; j <= PROB_STEP; ++j) {
        double y = j / (double) PROB_STEP;
        COMBINE_PROB[(i << 8) | j] = (int) Math.round(combiner.inverse(combiner.f(x) * combiner.f(y)) * PROB_STEP);
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

  protected StoredBoard(long player, long opponent, int depth) {
    this.player = player;
    this.opponent = opponent;
    this.depth = depth;
    this.fathers = new ArrayList<>();
    this.children = null;
    this.prev = null;
    this.next = null;
    this.nEmpties = 64 - Long.bitCount(player | opponent);
//    if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {
//      extraInfo = new ExtraInfo();
//    }
  }
  
  public static StoredBoard initialStoredBoard(Board b) {
    return initialStoredBoard(b.getPlayer(), b.getOpponent());
  }
  
  public static StoredBoard initialStoredBoard(long player, long opponent) {
    return new StoredBoard(player, opponent, 0);
  }
//
//  public synchronized void setIsFinished(boolean newValue) {
//    if (newValue == this.extraInfo.isFinished) {
//      return;
//    }
//    this.extraInfo.isFinished = true;
//    if (getChildren() != null) {
//      for (StoredBoard child : getChildren()) {
//        child.setIsFinished(newValue);
//      }
//    }
//  }
  
  public synchronized void addFather(StoredBoard father) {
    fathers.add(father);
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
    synchronized (this) {
      assert isLeaf();
      setBusyNoUpdate();
    }
    updateFathers(alpha, beta);
  }

  public synchronized void setFreeNoUpdate() {
    assert threadId == Thread.currentThread().getId();
    threadId = 0;
  }

  public void setFree(int alpha, int beta) {
    synchronized (this) {
      setFreeNoUpdate();
      if (!isLeaf()) {
        updateFather(alpha, beta);
      }
    }
    updateFathers(alpha, beta);
  }

  protected synchronized void updateFather(int alpha, int beta) {
    lower = -6400;
    upper = -6400;
    for (StoredBoard child : children) {
      synchronized (child) {
        lower = Math.max(lower, -child.upper);
        upper = Math.max(upper, -child.lower);
      }
    }
    for (int i = alpha; i <= beta; i += 200) {
      getOrAddEvaluation(i).updateFather();
    }
    assert isLowerUpperOK();
  }

  protected void updateFathers(int alpha, int beta) {
    synchronized (this) {
      if (!isLeaf() && threadId == 0) {
        updateFather(alpha, beta);
      }
    }
    // To avoid co-modification (if some other thread adds fathers in the meantime).
    for (int i = 0; i < fathers.size(); ++i) {
      StoredBoard father = fathers.get(i);
      father.updateFathers(-beta, -alpha);
    }
  }

  public synchronized int getEval(int evaluatorLower, int evaluatorUpper) {
    if (depth % 2 == 1) {
      int tmp = -evaluatorLower;
      evaluatorLower = -evaluatorUpper;
      evaluatorUpper = tmp;
    }
    float eval = evaluatorLower - 100;
    for (int evalGoal = evaluatorLower; evalGoal <= evaluatorUpper; evalGoal += 200) {
      float prob = getEvaluation(evalGoal).getProb();
      if (prob >= 1 - Constants.PROB_INCREASE_WEAK_EVAL) {
        prob = 1;
      } else if (prob <= Constants.PROB_INCREASE_WEAK_EVAL) {
        prob = 0;
      }
      eval += 200 * prob;
    }
    return Math.round(eval);
  }

  public synchronized float getProb(int evalGoal) {
    return getEvaluation(evalGoal).getProb();
  }

  public synchronized float getDisproofNumber(int evalGoal) {
    return getEvaluation(evalGoal).disproofNumber;
  }

  public synchronized float getProofNumber(int evalGoal) {
    return getEvaluation(evalGoal).proofNumber;
  }

  public synchronized boolean isSolved() {
    return getLower() == getUpper();
  }

  public synchronized boolean isPartiallySolved() {
    return getPercentileLower(Constants.ZERO_PERC_FOR_WEAK) ==
               getPercentileUpper(Constants.ZERO_PERC_FOR_WEAK);
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

  public int getUpper() {
    assert isLowerUpperOK();
    return upper;
  }

  public int getLower() {
    assert isLowerUpperOK();
    return lower;
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

  public final void setSolved(int newEval, int alpha, int beta) {
    setUpper(newEval);
    setLower(newEval);
    setFree(alpha, beta);
  }

  public final void setLower(int newLower) {
    assert isLeaf();
    assert leafEval != -6500;
    this.leafEval = Math.max(this.leafEval, newLower);
    lower = Math.max(lower, newLower);
    for (int eval = -6300; eval < newLower; eval += 200) {
      Evaluation e = getEvaluation(eval);
      if (e != null) {
        e.setProved();
      }
    }
    assert isLowerUpperOK();
  }

  public final void setUpper(int newUpper) {
    assert isLeaf();
    assert leafEval != -6500;
    this.leafEval = Math.min(this.leafEval, newUpper);
    upper = Math.min(upper, newUpper);
    for (int eval = newUpper + 100; eval <= 6300; eval += 200) {
      Evaluation e = getEvaluation(eval);
      if (e != null) {
        e.setDisproved();
      }
    }
    assert isLowerUpperOK();
  }

  public final boolean isLeaf() {
    return children == null;
  }

  private synchronized void setLeafWithoutFree(int leafEval, int alpha, int beta, double stddevMultiplier) {
    assert isLeaf();
    assert threadId == 0 || threadId == Thread.currentThread().getId() || EvaluatorMCTS.nextPositionLock.isHeldByCurrentThread();
    assert leafEval != -6500;
    this.leafEval = leafEval;
    for (int i = alpha; i <= beta; i += 200) {
      Evaluation evaluation = getOrAddEvaluation(i);
      StoredBoard board = evaluation.getStoredBoard();
      float prob = 1 - (float) Gaussian.CDF(i, leafEval, ERRORS[board.nEmpties] * Constants.MULT_STDDEV * stddevMultiplier);
      float proofNumber = (float) (StoredBoard.endgameTimeEstimator.proofNumber(board.getPlayer(), board.getOpponent(), i, leafEval));
      assert Float.isFinite(proofNumber) && proofNumber > 0;
      float disproofNumber = (float) (StoredBoard.endgameTimeEstimator.disproofNumber(board.getPlayer(), board.getOpponent(), i, leafEval));
      assert Float.isFinite(disproofNumber) && disproofNumber > 0;
      evaluation.setLeaf(prob, proofNumber, disproofNumber);
    }
  }
  public void setLeaf(int leafEval, int alpha, int beta) {
    setLeaf(leafEval, alpha, beta, 1);
  }
  public void setLeaf(int leafEval, int alpha, int beta, double stddevMultiplier) {
    setLeafWithoutFree(leafEval, alpha, beta, stddevMultiplier);
    setFree(alpha, beta);
  }

  public synchronized void updateDescendantsRecursive(int alpha, int beta) {
    // Nobody else can held a StoredBoard lock.
    assert EvaluatorMCTS.nextPositionLock.isHeldByCurrentThread();
    assert isLeaf() || threadId == 0;
    if (isLeaf()) {
      assert leafEval != -6500;
      setLeafWithoutFree(leafEval, alpha, beta, 1);
      return;
    }
    for (StoredBoard child : children) {
      child.updateDescendantsRecursive(-beta, -alpha);
    }
    updateFather(alpha, beta);
    assert isLowerUpperOK();
  }

  public synchronized Evaluation getEvaluation(int evalGoal) {
    return evaluations[toEvaluationIndex(evalGoal)];
  }

  public synchronized Evaluation addEvaluation(int evalGoal) {
//    assert threadId == Thread.currentThread().getId();
    int index = toEvaluationIndex(evalGoal);
    if (evaluations[index] != null) {
      return null;
    }
    Evaluation eval = new Evaluation(evalGoal);
    evaluations[index] = eval;
    return eval;
  }

  public synchronized Evaluation getOrAddEvaluation(int evalGoal) {
    Evaluation eval = evaluations[toEvaluationIndex(evalGoal)];
    if (eval != null) {
      return eval;
    }
    return addEvaluation(evalGoal);
  }

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
    return getEvaluation(evalGoal).childLogDerivative(child.getEvaluation(-evalGoal));
  }

  @NonNull
  @Override
  public String toString() {
    return new Board(player, opponent).toString() + "\n";
  }
  
  public synchronized StoredBoard[] getChildren() {
    return children;
  }

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

  boolean isLowerUpperOK() {
    assert lower <= upper;
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
        child.depth == depth + 1
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
      return true;
    }
    return true;
  }
//
//  boolean isExtraInfoOK() {
//    if (!Constants.FIND_BEST_PROOF_AFTER_EVAL) {
//      return true;
//    }
//    if (Float.isNaN(extraInfo.minDisproofStrictlyGreaterVar) || Float.isNaN(extraInfo.minProofGreaterEqualVar)) {
//      throw new AssertionError(
//          "Got (minDisproofVar, minProofVar) = " + extraInfo.minDisproofStrictlyGreaterVar + " " + extraInfo.minProofGreaterEqualVar
//      );
//    }
//
//    return true;
//  }
  
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
    return true; //isExtraInfoOK();
  }
}