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
import helpers.Gaussian;
import helpers.Utils;
import java.util.ArrayList;
import java.util.HashSet;

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
    private Evaluation bestChildMidgame;
    private Evaluation bestChildProof;
    private Evaluation bestChildDisproof;
    float bestDisproofNumberValue = Float.NEGATIVE_INFINITY;
    boolean isBusy = false;
    boolean isLeaf = true;
    long descendants = 0;

    public Evaluation(int evalGoal) {
      assert evalGoal <= 6400 && evalGoal >= -6400;
      assert (evalGoal + 6400) % 200 == 100;
      this.evalGoal = evalGoal;
    }

    public float getProb() {
      return (float) prob / PROB_STEP;
    }

    int maxLogDerivative() {
      return maxLogDerivative;
    }

    int childLogDerivative(Evaluation child) {
      return Math.max(LOG_DERIVATIVE_MINUS_INF,
          child.maxLogDerivative - LOG_DERIVATIVE[child.prob] + LOG_DERIVATIVE[255 - prob]);
    }

    public Evaluation bestChild() {
      if (prob == 0) {
        return bestChildDisproof;
      } else if (prob == PROB_STEP) {
        return bestChildProof;
      }
      return bestChildMidgame;
    }

    private void setDisproved() {
      prob = 0;
      proofNumber = Float.POSITIVE_INFINITY;
      disproofNumber = 0;
      maxLogDerivative = LOG_DERIVATIVE_MINUS_INF;
    }

    private void setProved() {
      prob = PROB_STEP;
      proofNumber = 0;
      disproofNumber = Float.POSITIVE_INFINITY;
      maxLogDerivative = LOG_DERIVATIVE_MINUS_INF;
    }

    private void setLeaf() {
      assert isLeaf;
      bestChildMidgame = this;
      bestChildProof = this;
      bestChildDisproof = this;
      if (isBusy) {
        if (eval > evalGoal) {
          setProved();
        } else {
          setDisproved();
        }
        return;
      }

      if (lower >= evalGoal) {
        setProved();
      } else if (upper < evalGoal) {
        setDisproved();
      } else {
        prob = roundProb((1-(float) Gaussian.CDF(evalGoal, eval, 600 - 5 * nEmpties)));
        proofNumber = (float) (endgameTimeEstimator.proofNumber(player, opponent, evalGoal, eval));
        assert Float.isFinite(proofNumber) && proofNumber > 0;
        disproofNumber = (float) (endgameTimeEstimator.disproofNumber(player, opponent, evalGoal, eval));
        assert Float.isFinite(disproofNumber) && disproofNumber > 0;
        if (prob == 0 || prob == PROB_STEP) {
          maxLogDerivative = LOG_DERIVATIVE_MINUS_INF;
        } else {
          maxLogDerivative = (int) Math.round(LOG_DERIVATIVE_MULTIPLIER * (Math.log(getProb() * (1 - getProb()))));
        }
      }
    }

    StoredBoard getStoredBoard() {
      return StoredBoard.this;
    }

    public void addDescendants(long newDescendants) {
      if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {
        if (extraInfo.isFinished) {
          return;
        }
      }
      descendants += newDescendants;
    }

    public void setFree() {
      assert isBusy;
      isBusy = false;
      if (isLeaf) {
        this.setLeaf();
      }
    }

    public void setBusy() {
      synchronized (this) {
        assert isLeaf;
        assert !isBusy;
        isBusy = true;
        this.setLeaf();
      }
    }

    protected synchronized void updateFather() {
      proofNumber = Float.POSITIVE_INFINITY;
      disproofNumber = 0;
      prob = 0;
      maxLogDerivative = LOG_DERIVATIVE_MINUS_INF;
      bestDisproofNumberValue = Float.NEGATIVE_INFINITY;
      for (StoredBoard childBoard : children) {
        synchronized (childBoard) {
          Evaluation child = childBoard.getEvaluation(-evalGoal);
          prob = combineProb(prob, child.prob);

          float curProofNumber = child.disproofNumber / Math.max(0.001F, 1 - child.getProb());
          if (curProofNumber < proofNumber) {
            bestChildProof = child;
            proofNumber = curProofNumber;
          }

          disproofNumber += child.proofNumber;
          if (child.proofNumber > bestDisproofNumberValue) {
            bestChildDisproof = child;
            bestDisproofNumberValue = child.proofNumber;
          }
          if (child.getProb() > 0 && child.getProb() < 1) {
            int currentLogDerivative = child.maxLogDerivative - LOG_DERIVATIVE[child.prob];
            if (currentLogDerivative > maxLogDerivative) {
              bestChildMidgame = child;
              maxLogDerivative = currentLogDerivative;
            }
          }
        }
      }
      if (getProb() == 1) {
        maxLogDerivative = LOG_DERIVATIVE_MINUS_INF;
      } else {
        maxLogDerivative = Math.max(
            LOG_DERIVATIVE_MINUS_INF,
            maxLogDerivative + LOG_DERIVATIVE[255 - prob]);
      }
//      if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {
//        extraInfo.minProofGreaterEqual = Float.POSITIVE_INFINITY;
//        extraInfo.minProofGreaterEqualVar = Float.POSITIVE_INFINITY;
//        extraInfo.minDisproofStrictlyGreater = 0;
//        extraInfo.minDisproofStrictlyGreaterVar = 0;
//        float extraCostDisproofStrictlyGreaterVar = Float.POSITIVE_INFINITY;
//        extraInfo.proofBeforeFinished = false;
//        extraInfo.disproofBeforeFinished = true;
//        for (StoredBoard child : children) {
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
  public StoredBoard[] children;
  
  int eval;
  int lower;
  int upper;
  int nEmpties;
  Evaluation[] evaluations = new Evaluation[64];
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

  protected StoredBoard(long player, long opponent, boolean playerIsStartingPlayer) {
    this.player = player;
    this.opponent = opponent;
    this.playerIsStartingPlayer = playerIsStartingPlayer;
    this.lower = -6400;
    this.upper = 6400;
    this.fathers = new ArrayList<>();
    this.children = null;
    this.prev = null;
    this.next = null;
    this.nEmpties = 64 - Long.bitCount(player | opponent);
    if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {
      extraInfo = new ExtraInfo();
    }
    for (int i = -6300; i <= 6300; i += 200) {
      evaluations[toEvaluationIndex(i)] = new Evaluation(i);
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
    if (children != null) {
      for (StoredBoard child : children) {
        child.setIsFinished(newValue);
      }
    }
  }
  
  public synchronized void addFather(StoredBoard father) {
    fathers.add(father);
  }
  
  public synchronized Board getBoard() {
    return new Board(player, opponent);
  }
  
  public synchronized long getPlayer() {
    return player;
  }
  
  public synchronized long getOpponent() {
    return opponent;
  }
  
  public synchronized int getEval() {
    return eval;
  }

  public Evaluation getEvaluation(int eval) {
    return evaluations[toEvaluationIndex(eval)];
  }

  public float getProb(int evalGoal) {
    return getEvaluation(evalGoal).getProb();
  }

  public synchronized float getDisproofNumber(int evalGoal) {
    return getEvaluation(evalGoal).disproofNumber;
  }

  public synchronized float getProofNumber(int evalGoal) {
    return getEvaluation(evalGoal).proofNumber;
  }
  
  public int getLower() {
    return lower;
  }
  
  public int getUpper() {
    return upper;
  }
  
  public long getDescendants() {
    long descendants = 0;
    for (Evaluation eval : evaluations) {
      descendants += eval.descendants;
    }
    return descendants;
  }

  public final synchronized void setSolved(int newEval) {
    this.lower = newEval;
    this.eval = newEval;
    this.upper = newEval;
//    this.updateFathers();
    assert areThisBoardEvalsOK();
  }

  public final synchronized void setLower(int newLower) {
    this.lower = (short) Math.max(lower, newLower);
    this.eval = (short) Math.max(newLower, eval);
    assert areThisBoardEvalsOK();
  }

  public final synchronized void setUpper(int newUpper) {
    this.upper = (short) Math.min(upper, newUpper);
    this.eval = (short) Math.min(newUpper, eval);
    assert areThisBoardEvalsOK();
  }
  
  public final synchronized void setEval(int newEval) {
    this.eval = (short) Math.max(this.lower, Math.min(this.upper, newEval));
    assert areThisBoardEvalsOK();
  }

  public void setFree() {
    synchronized (this) {
      for (int i = -6300; i <= 6300; i += 200) {
        getEvaluation(i).setFree();
      }
    }
    updateFathers();
  }

  public void setBusy() {
    synchronized (this) {
      for (int i = -6300; i <= 6300; i += 200) {
        getEvaluation(i).setBusy();
      }
    }
    this.updateFathers();
  }

  private static int roundProb(float prob) {
    return Math.round(prob * PROB_STEP);
  }

  protected synchronized void updateFather() {
    eval = Short.MIN_VALUE;
    lower = Short.MIN_VALUE;
    upper = Short.MIN_VALUE;
    for (int eval = -6300; eval <= 6300; eval += 200) {
      getEvaluation(eval).updateFather();
    }
    for (StoredBoard child : children) {
      synchronized (child) {
        eval = Math.max(eval, -child.eval);
        lower = Math.max(lower, -child.getUpper());
        upper = Math.max(upper, -child.getLower());
      }
    }
//    if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {
//      extraInfo.minProofGreaterEqual = Float.POSITIVE_INFINITY;
//      extraInfo.minProofGreaterEqualVar = Float.POSITIVE_INFINITY;
//      extraInfo.minDisproofStrictlyGreater = 0;
//      extraInfo.minDisproofStrictlyGreaterVar = 0;
//      float extraCostDisproofStrictlyGreaterVar = Float.POSITIVE_INFINITY;
//      extraInfo.proofBeforeFinished = false;
//      extraInfo.disproofBeforeFinished = true;
//      for (StoredBoard child : children) {
//        extraInfo.proofBeforeFinished = extraInfo.proofBeforeFinished || child.extraInfo.disproofBeforeFinished;
//        extraInfo.minProofGreaterEqual = Math.min(extraInfo.minProofGreaterEqual, child.extraInfo.minDisproofStrictlyGreater);
//        extraInfo.minProofGreaterEqualVar = Math.min(extraInfo.minProofGreaterEqualVar, child.extraInfo.minDisproofStrictlyGreaterVar);
//        extraInfo.disproofBeforeFinished =extraInfo.disproofBeforeFinished && child.extraInfo.proofBeforeFinished;
//        extraInfo.minDisproofStrictlyGreater += child.extraInfo.minProofGreaterEqual;
//        extraInfo.minDisproofStrictlyGreaterVar += Math.min(child.extraInfo.minProofGreaterEqual, child.extraInfo.minProofGreaterEqualVar);
//        if (extraInfo.minDisproofStrictlyGreaterVar == Float.POSITIVE_INFINITY) {
//          extraCostDisproofStrictlyGreaterVar = 0;
//        } else {
//          extraCostDisproofStrictlyGreaterVar = Math.min(extraCostDisproofStrictlyGreaterVar, child.extraInfo.minProofGreaterEqualVar - child.extraInfo.minProofGreaterEqual);
//        }
//      }
//      extraInfo.minDisproofStrictlyGreaterVar += Math.max(0, extraCostDisproofStrictlyGreaterVar);
//      assert isExtraInfoOK();
//    }
    assert isAllOK();
  }

  protected void updateFathers() {
    if (children != null) {
      updateFather();
    }
    for (int i = 0; i < fathers.size(); ++i) {
      fathers.get(i).updateFathers();
    }
  }

  public synchronized int maxLogDerivative(int evalGoal) {
    Evaluation eval = getEvaluation(evalGoal);
    return eval.maxLogDerivative();
  }

  public synchronized int childLogDerivative(StoredBoard child, int evalGoal) {
    Evaluation eval = evaluations[toEvaluationIndex(evalGoal)];
    return child.maxLogDerivative(-evalGoal);
  }

  @NonNull
  @Override
  public synchronized String toString() {
    return new Board(player, opponent).toString() + eval + "\n";
  }
  
  public synchronized boolean isSolved() {
    return this.lower == this.upper;
  }
  
  public synchronized StoredBoard[] getChildren() {
    return children;
  }

  // TODO!!!
//  public synchronized StoredBoard bestChildProofStrictlyGreater() {
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
//  public synchronized StoredBoard bestChildProofGreaterEqual() {
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
  
  public synchronized float maxFiniteChildrenProofNumber(int evalGoal) {
    float result = 0;
    for (StoredBoard child : children) {
      if (child.getProofNumber(evalGoal) != Float.POSITIVE_INFINITY) {
        result = Math.max(result, child.getProofNumber(evalGoal));
      }
    }
    return result;
  }

  boolean areThisBoardEvalsOK() {
    if (this.lower > this.eval || this.eval > this.upper) {
      throw new AssertionError("Wrong lower:" + lower + " / eval:" + eval + " / upper:" + upper + " for board\n" + this.getBoard());
    }
    return true;
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
    if (Constants.MAX_PARALLEL_TASKS == 1 && (eval < -child.eval || lower < -child.upper || upper < -child.lower)) {
      throw new AssertionError(
          "Wrong eval=" + eval + "/lower=" + lower + "/upper=" + upper +
          " (should be eval>=" + (-child.eval) + "/lower>=" + (-child.upper) + "/upper>=" + (-child.lower) +
          ". Board:\n" + this + "\nchild:\n" + child);
    }
    return 
        child.playerIsStartingPlayer == !playerIsStartingPlayer
        && Utils.arrayContains(children, child);
  }

  boolean areChildrenOK() {
    if (children == null) {
      return true;
    }
    for (StoredBoard child : children) {
      if (!isChildOK(child)) {
        throw new AssertionError("Child\n" + child + "\n of board\n" + this + "\nis not OK");
      }
    }
    if (children.length != Long.bitCount(GetMoves.getMoves(player, opponent)) && !(
        Long.bitCount(GetMoves.getMoves(player, opponent)) == 0
        && Long.bitCount(GetMoves.getMoves(opponent, player)) != 0
        && children.length == 1)) {
      throw new AssertionError(
          "Board " + this + " should have " + Long.bitCount(GetMoves.getMoves(player, opponent))
              + " children. Found " + children.length);
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
      if (eval != correctEval || lower != correctEval || upper != correctEval) {
        throw new AssertionError(
            "Got wrong eval/lower/upper = " + eval + "/" + lower + "/" + upper + 
            " (expected all " + correctEval + ") for board\n" + this.getBoard());
      }
      return true;
    }
    if (children == null) {
      return true;
    }
    int correctEval = Integer.MIN_VALUE;
    int correctLower = Integer.MIN_VALUE;
    int correctUpper = Integer.MIN_VALUE;
    String childEval = "";
    for (StoredBoard child : children) {
      if (Constants.MAX_PARALLEL_TASKS > 1) {
        return true;
      }
      correctEval = Math.max(correctEval, -child.getEval());
      correctLower = Math.max(correctLower, -child.getUpper());
      correctUpper = Math.max(correctUpper, -child.getLower());
    }
    if (eval != correctEval || lower != correctLower || upper != correctUpper) {
      throw new AssertionError(
          "Wrong eval/lower/upper. Expected: " + correctEval + "/" + correctLower + "/" + correctUpper + 
          ". Got: " + eval + "/" + lower + "/" + upper + ".\nChildren eval/lower/upper:\n" + childEval);
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
    if (!areThisBoardEvalsOK() || !areDescendantsOK()) {
      throw new AssertionError("Wrong areThisBoardEvalsOK or isEvalOK or areDescendantsOK.");
    }
    return isExtraInfoOK();// && isLeastSingleAncestorOK();
  }
}