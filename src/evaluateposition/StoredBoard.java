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
import board.GetMoves;
import board.GetMovesCache;
import constants.Constants;
import helpers.Gaussian;
import helpers.Utils;
import java.util.ArrayList;
import java.util.HashSet;

public class StoredBoard {
  public class ExtraInfo {
    public float minProofGreaterEqual = Float.POSITIVE_INFINITY;
    public float minDisproofStrictlyGreater = Float.POSITIVE_INFINITY;
    public float minProofGreaterEqualVar = Float.POSITIVE_INFINITY;
    public float minDisproofStrictlyGreaterVar = Float.POSITIVE_INFINITY;
    public boolean isFinished = false;
    public boolean proofBeforeFinished = false;
    public boolean disproofBeforeFinished = false;
  }
  protected static final HashMap HASH_MAP;
  protected static final EvaluatorInterface EVALUATOR_MIDGAME;
  
  static {
    HASH_MAP = new HashMap(6000);
    EVALUATOR_MIDGAME = new EvaluatorAlphaBeta(HASH_MAP);
  }
  
  private final long player;
  private final long opponent;

  static EndgameTimeEstimatorInterface endgameTimeEstimator = new EndgameTimeEstimator();
  public final ArrayList<StoredBoard> fathers;
  public StoredBoard[] children;
  
  int eval;
  int lower;
  int upper;
  int nEmpties;
  private int probGreaterEqual;
  private int probStrictlyGreater;
  // Positions to prove that eval >= evalGoal.
  float proofNumberGreaterEqual;
  // Positions to prove that eval <= evalGoal.
  float disproofNumberStrictlyGreater;
  public int maxLogDerivativeProbStrictlyGreater;
  public int maxLogDerivativeProbGreaterEqual;
  int evalGoal;
  public boolean playerIsStartingPlayer;
  private long descendants;
  boolean isBusy = false;
  
  private static final int PROB_STEP = 255;
  private static final int[] COMBINE_PROB;
  private static final int[] LOG_DERIVATIVE;
  public static final int LOG_DERIVATIVE_MINUS_INF = -20000000;
  private static final int LOG_DERIVATIVE_MULTIPLIER = 10000;
  private StoredBoard bestChildMidgameGreaterEqual;
  private StoredBoard bestChildMidgameStrictlyGreater;
  private StoredBoard bestChildEndgameGreaterEqual;
  private StoredBoard bestChildEndgameStrictlyGreater;
  
  static {
    COMBINE_PROB = new int[(PROB_STEP + 1) * (PROB_STEP + 1) * 64];
    LOG_DERIVATIVE = new int[(PROB_STEP + 1) * (PROB_STEP + 1) * 64];
    for (int leadingZerosDescendants = 0; leadingZerosDescendants < 64; ++leadingZerosDescendants) {
      float lambda = -1 / Math.max(1, 3 * (63 - leadingZerosDescendants)) - 1F;
      for (int i = 0; i <= PROB_STEP; ++i) {
        for (int j = 0; j <= PROB_STEP; ++j) {
          COMBINE_PROB[(leadingZerosDescendants << 16) | (i << 8) | j] = 
              (int) Math.round(
                  Math.pow(
                      Math.pow(i / (double) PROB_STEP, lambda) +
                      Math.pow(j / (double) PROB_STEP, lambda) - 1,
                      1 / lambda
                  ) * PROB_STEP);
          
          if (i == PROB_STEP || j == 0) {
            LOG_DERIVATIVE[(leadingZerosDescendants << 16) | (i << 8) | j] = LOG_DERIVATIVE_MINUS_INF;
          } else {
            LOG_DERIVATIVE[(leadingZerosDescendants << 16) | (i << 8) | j] =
                // (1 - lambda) * log((1 - p_f) / p_c), with (1-p_f >= p_c).
                // <= 0, >= (1 - (-2)) * log(1/255 / 1) >= -17
                // Max value: -17 * 100 * 60 = 102000. Should be fine with 2M
                (int) Math.round(
                    LOG_DERIVATIVE_MULTIPLIER *
                    Math.min(0,
                             (1 - lambda) * (float) Math.log((PROB_STEP - i) / (float) j)));
            if (LOG_DERIVATIVE[(leadingZerosDescendants << 16) | (i << 8) | j] == -2147483648) {
              System.out.println(leadingZerosDescendants + " " + i + " " + j);
            }
          }
        }
      }
    }
  }
  private static int combineProb(int leadingZerosDescendants, int p1, int p2) {
    return COMBINE_PROB[(leadingZerosDescendants << 16) | (p1 << 8) | p2];
  }
  private static int logDerivative(int leadingZerosDescendants, int fatherProb, int childProb) {
    return LOG_DERIVATIVE[(leadingZerosDescendants << 16) | (fatherProb << 8) | childProb];
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
  }
  
  public static StoredBoard initialStoredBoard(Board b) {
    return initialStoredBoard(b.getPlayer(), b.getOpponent());
  }
  
  public static StoredBoard initialStoredBoard(long player, long opponent) {
    return new StoredBoard(player, opponent, true);
  }
  
  synchronized StoredBoard getChild(long player, long opponent) {
    StoredBoard result = new StoredBoard(player, opponent, !playerIsStartingPlayer);
    result.evalGoal = -evalGoal;
    if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {
      result.extraInfo.isFinished = this.extraInfo.isFinished;
    }
    result.addFather(this);
    return result;
  }
  
  public synchronized void addDescendants(long newDescendants) {
    if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {
      if (extraInfo.isFinished) {
        return;
      }
    }
    descendants += newDescendants;
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
  
  public boolean isLeaf() {
    return children == null;
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
  
  public float getProbGreaterEqual() {
    return probGreaterEqual / (float) PROB_STEP;
  }
  public float getProbStrictlyGreater() {
    return probStrictlyGreater / (float) PROB_STEP;
  }
  
  public synchronized float getProofNumberGreaterEqual() {
    if (isBusy) {
      return (float) (endgameTimeEstimator.proofNumber(player, opponent, evalGoal - 100, this.eval));
    }
    return proofNumberGreaterEqual;
  }
  
  public synchronized float getDisproofNumberStrictlyGreater() {
    if (isBusy) {
      return (float) (endgameTimeEstimator.disproofNumber(player, opponent, evalGoal + 100, this.eval));
    }
    return disproofNumberStrictlyGreater;
  }
  
  public int getLower() {
    return lower;
  }
  
  public int getUpper() {
    return upper;
  }
  
  public long getDescendants() {
    return descendants;
  }
  
  public int getEvalGoal() {
    return evalGoal;
  }

  public final synchronized void setSolved(int newEval) {
    assert isBusy;
    assert isLeaf();
    this.lower = newEval;
    this.eval = newEval;
    this.upper = newEval;
//    this.updateFathers();
    assert areThisBoardEvalsOK();
  }

  public final synchronized void setLower(int newLower) {
    assert isBusy;
    assert isLeaf();
    this.lower = (short) Math.max(lower, newLower);
    this.eval = (short) Math.max(newLower, eval);
    assert areThisBoardEvalsOK();
  }

  public final synchronized void setUpper(int newUpper) {
    assert isBusy;
    assert isLeaf();
    this.upper = (short) Math.min(upper, newUpper);
    this.eval = (short) Math.min(newUpper, eval);
    assert areThisBoardEvalsOK();
  }
  
  public final synchronized void setEval(int newEval) {
    assert isLeaf();
    this.eval = (short) Math.max(this.lower, Math.min(this.upper, newEval));
//    this.setProofNumbersForLeaf();
    assert areThisBoardEvalsOK();
  }
  
  public void setFree() {
    assert isBusy;
    isBusy = false;
    if (isLeaf()) {
      this.setProofNumbersForLeaf();
    }
    else {
      for (StoredBoard child : children) {
        synchronized(child) {
          if (child.isLeaf() && !child.isBusy) {
            child.evalGoal = -this.evalGoal;
            child.setProofNumbersForLeaf();
          } else {
            assert child.evalGoal == -evalGoal;
          }
        }
      }
    }
    updateFathers();
  }

  public synchronized void setBusy() {
    assert !isBusy;
    isBusy = true;
    this.setProofNumbersForLeaf();
    assert isLeaf();
    this.updateFathers();
  }
  
  protected void setEvalGoalRecursive(int evalGoal) {
    setEvalGoalRecursive(evalGoal, new HashSet<>());
  }
  protected synchronized void setEvalGoalRecursive(int evalGoal, HashSet<StoredBoard> done) {
    if (done.contains(this)) {
      return;
    }
    this.evalGoal = evalGoal;
    if (isLeaf()) {
      if (!isBusy) {
        setProofNumbersForLeaf();
      }
      return;
    }
    for (StoredBoard child : children) {
      child.setEvalGoalRecursive(-evalGoal, done);
    }
    updateFather();
    done.add(this);
    assert areChildrenOK();
  }

  private static final int roundProb(float prob) {
    return Math.round(prob * PROB_STEP);
  }

  protected synchronized void updateFather() {
    assert !isLeaf();
    if (isBusy) {
      return;
    }
    eval = Short.MIN_VALUE;
    lower = Short.MIN_VALUE;
    upper = Short.MIN_VALUE;
//    probGreaterEqual = 1 - children.length;
//    probStrictlyGreater = probGreaterEqual;
    proofNumberGreaterEqual = Float.POSITIVE_INFINITY;
    disproofNumberStrictlyGreater = 0;
    probGreaterEqual = PROB_STEP;
    probStrictlyGreater = PROB_STEP;
    int leadingZerosDescendants = Long.numberOfLeadingZeros(descendants);

    for (StoredBoard child : children) {
      assert child.evalGoal == -evalGoal;
//      synchronized(child) {
      eval = Math.max(eval, -child.eval);
      lower = Math.max(lower, -child.upper);
      upper = Math.max(upper, -child.lower);
      probGreaterEqual = combineProb(leadingZerosDescendants, probGreaterEqual, child.probStrictlyGreater);
      probStrictlyGreater = combineProb(leadingZerosDescendants, probStrictlyGreater, child.probGreaterEqual);
      proofNumberGreaterEqual = Math.min(proofNumberGreaterEqual, child.disproofNumberStrictlyGreater / Math.max(0.002F, 1 - child.getProbStrictlyGreater()));
      disproofNumberStrictlyGreater += child.proofNumberGreaterEqual;
    }
    probGreaterEqual = PROB_STEP - probGreaterEqual;
    probStrictlyGreater = PROB_STEP - probStrictlyGreater;
    if (lower < evalGoal && upper >= evalGoal) {
      proofNumberGreaterEqual *= Math.max(0.002F, this.getProbGreaterEqual());
    }
    assert probGreaterEqual >= probStrictlyGreater;
    maxLogDerivativeProbStrictlyGreater = LOG_DERIVATIVE_MINUS_INF;
    maxLogDerivativeProbGreaterEqual = LOG_DERIVATIVE_MINUS_INF;

    float bestEndgameGreaterEqualValue = Float.NEGATIVE_INFINITY;
    float curEndgameGreaterEqualValue;
    float bestEndgameStrictlyGreaterValue = Float.NEGATIVE_INFINITY;
    int currentLogDerivative;

    for (StoredBoard child : children) {
      curEndgameGreaterEqualValue = -child.disproofNumberStrictlyGreater / (1-child.getProbStrictlyGreater());
      if (curEndgameGreaterEqualValue > bestEndgameGreaterEqualValue) {
        bestChildEndgameGreaterEqual = child;
        bestEndgameGreaterEqualValue = curEndgameGreaterEqualValue;
      }
      if (child.proofNumberGreaterEqual > bestEndgameStrictlyGreaterValue) {
        bestChildEndgameStrictlyGreater = child;
        bestEndgameStrictlyGreaterValue = child.proofNumberGreaterEqual;
      }

      currentLogDerivative = logDerivativeProbStrictlyGreater(child);
      if (currentLogDerivative > maxLogDerivativeProbStrictlyGreater) {
        maxLogDerivativeProbStrictlyGreater = currentLogDerivative;
        bestChildMidgameStrictlyGreater = child;
      }
      currentLogDerivative = logDerivativeProbGreaterEqual(child);
      if (currentLogDerivative > maxLogDerivativeProbGreaterEqual) {
        maxLogDerivativeProbGreaterEqual = currentLogDerivative;
        bestChildMidgameGreaterEqual = child;
      }
    }
    if (probStrictlyGreater == 0) {
      bestChildMidgameStrictlyGreater = bestChildEndgameStrictlyGreater;
    }
    if (probGreaterEqual == PROB_STEP) {
      bestChildMidgameGreaterEqual = bestChildEndgameGreaterEqual;
    }
    assert maxLogDerivativeProbStrictlyGreater >= LOG_DERIVATIVE_MINUS_INF;
    assert maxLogDerivativeProbGreaterEqual >= LOG_DERIVATIVE_MINUS_INF;
    if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {
      extraInfo.minProofGreaterEqual = Float.POSITIVE_INFINITY;
      extraInfo.minProofGreaterEqualVar = Float.POSITIVE_INFINITY;
      extraInfo.minDisproofStrictlyGreater = 0;
      extraInfo.minDisproofStrictlyGreaterVar = 0;
      float extraCostDisproofStrictlyGreaterVar = Float.POSITIVE_INFINITY;
      extraInfo.proofBeforeFinished = false;
      extraInfo.disproofBeforeFinished = true;
      for (StoredBoard child : children) {
        extraInfo.proofBeforeFinished = extraInfo.proofBeforeFinished || child.extraInfo.disproofBeforeFinished;
        extraInfo.minProofGreaterEqual = Math.min(extraInfo.minProofGreaterEqual, child.extraInfo.minDisproofStrictlyGreater);
        extraInfo.minProofGreaterEqualVar = Math.min(extraInfo.minProofGreaterEqualVar, child.extraInfo.minDisproofStrictlyGreaterVar);
        extraInfo.disproofBeforeFinished =extraInfo.disproofBeforeFinished && child.extraInfo.proofBeforeFinished;
        extraInfo.minDisproofStrictlyGreater += child.extraInfo.minProofGreaterEqual;
        extraInfo.minDisproofStrictlyGreaterVar += Math.min(child.extraInfo.minProofGreaterEqual, child.extraInfo.minProofGreaterEqualVar);
        if (extraInfo.minDisproofStrictlyGreaterVar == Float.POSITIVE_INFINITY) {
          extraCostDisproofStrictlyGreaterVar = 0;
        } else {
          extraCostDisproofStrictlyGreaterVar = Math.min(extraCostDisproofStrictlyGreaterVar, child.extraInfo.minProofGreaterEqualVar - child.extraInfo.minProofGreaterEqual);
        }
      }
      extraInfo.minDisproofStrictlyGreaterVar += Math.max(0, extraCostDisproofStrictlyGreaterVar); 
      assert isExtraInfoOK();
    }
    assert isAllOK();
  }
  
  public synchronized int logDerivativeProbStrictlyGreater(StoredBoard child) {
    return Math.max(LOG_DERIVATIVE_MINUS_INF, child.maxLogDerivativeProbGreaterEqual + logDerivativeEdgeProbStrictlyGreater(child));
  }
  
  public synchronized int logDerivativeEdgeProbStrictlyGreater(StoredBoard child) {
    assert Utils.arrayContains(children, child);
//    if (evalGoal != -child.evalGoal) {
//      System.out.println(evalGoal + " " + child.evalGoal);
//      System.out.println(child.fathers.size());
//    }
    assert evalGoal == -child.evalGoal;
    return logDerivative(Long.numberOfLeadingZeros(child.descendants), probStrictlyGreater, child.probGreaterEqual);
//    if (child.probGreaterEqual == 0 || probStrictlyGreater == PROB_STEP) {
//      return LOG_DERIVATIVE_MINUS_INF;
//    }
//    return (int) Math.round(
//        LOG_DERIVATIVE_MULTIPLIER * Math.min(0, (1 - lambda()) * (float) Math.log((PROB_STEP - probStrictlyGreater) / (float) child.probGreaterEqual)));
  }
  
  public synchronized int logDerivativeProbGreaterEqual(StoredBoard child) {
    return Math.max(LOG_DERIVATIVE_MINUS_INF, child.maxLogDerivativeProbStrictlyGreater + logDerivativeEdgeProbGreaterEqual(child));
  }
  public synchronized int logDerivativeEdgeProbGreaterEqual(StoredBoard child) {
    assert Utils.arrayContains(children, child);
    return logDerivative(Long.numberOfLeadingZeros(child.descendants), probGreaterEqual, child.probStrictlyGreater);
//    if (child.probStrictlyGreater == 0 || probGreaterEqual == PROB_STEP) {
//      return LOG_DERIVATIVE_MINUS_INF;
//    }
////    System.out.println(LOG_DERIVATIVE_MULTIPLIER * Math.min(0, (1 - lambda()) * (float) Math.log((PROB_STEP - probGreaterEqual) / (float) child.probStrictlyGreater)));
//    return (int) Math.round(
//        LOG_DERIVATIVE_MULTIPLIER * Math.min(0, (1 - lambda()) * (float) Math.log((PROB_STEP - probGreaterEqual) / (float) child.probStrictlyGreater)));
  }

  protected synchronized void updateFathers() {
    if (!isLeaf()) {
      updateFather();
    }
    for (StoredBoard father : fathers) {
      if (father.getEvalGoal() == -evalGoal) {
        father.updateFathers();
      }
    }
  }

  private synchronized void setProofNumbersForLeaf() {
    assert this.isLeaf();
    assert evalGoal <= 6400 && evalGoal >= -6400;
    assert descendants > 0 || extraInfo.isFinished;
    assert probGreaterEqual >= probStrictlyGreater;

    float mult = LOG_DERIVATIVE_MULTIPLIER;
    bestChildMidgameGreaterEqual = this;
    bestChildMidgameStrictlyGreater = this;
    bestChildEndgameGreaterEqual = this;
    bestChildEndgameStrictlyGreater = this;

    if (lower >= evalGoal || isBusy) {
      probGreaterEqual = PROB_STEP;
      proofNumberGreaterEqual = 0;
      maxLogDerivativeProbGreaterEqual = LOG_DERIVATIVE_MINUS_INF;
    } else if (upper < evalGoal) {
      proofNumberGreaterEqual = Float.POSITIVE_INFINITY;
      probGreaterEqual = 0;
      maxLogDerivativeProbGreaterEqual = LOG_DERIVATIVE_MINUS_INF;
    } else {
      probGreaterEqual = roundProb((1-(float) Gaussian.CDF(evalGoal-100, eval, 500 - 3 * nEmpties)));
      if (getProbGreaterEqual() == 0 || getProbGreaterEqual() == 1) {
        maxLogDerivativeProbGreaterEqual = LOG_DERIVATIVE_MINUS_INF;
      } else {
        maxLogDerivativeProbGreaterEqual = (int) Math.round(mult * Math.log(getProbGreaterEqual() * (1 - getProbGreaterEqual())));
      }
      proofNumberGreaterEqual = (float) (endgameTimeEstimator.proofNumber(player, opponent, evalGoal - 100, this.eval));
      assert Float.isFinite(proofNumberGreaterEqual) && proofNumberGreaterEqual > 0;
    }
    
    if (upper <= evalGoal || isBusy) {
      probStrictlyGreater = 0;
      disproofNumberStrictlyGreater = 0;
      maxLogDerivativeProbStrictlyGreater = LOG_DERIVATIVE_MINUS_INF;
    } else if (lower > evalGoal) {
      probStrictlyGreater = (int) PROB_STEP;
      disproofNumberStrictlyGreater = Float.POSITIVE_INFINITY;
      maxLogDerivativeProbStrictlyGreater = LOG_DERIVATIVE_MINUS_INF;
    } else {
      probStrictlyGreater = roundProb((1-(float) Gaussian.CDF(evalGoal+100, eval, 500 - 3 * nEmpties)));
      if (getProbStrictlyGreater() == 0 || getProbStrictlyGreater() == 1) {
        maxLogDerivativeProbStrictlyGreater = LOG_DERIVATIVE_MINUS_INF;
      } else {
        maxLogDerivativeProbStrictlyGreater = (int) Math.round(mult * Math.log(getProbStrictlyGreater() * (1 - getProbStrictlyGreater())));
      }
      disproofNumberStrictlyGreater = (float) (endgameTimeEstimator.disproofNumber(player, opponent, evalGoal + 100, this.eval));
      assert Float.isFinite(disproofNumberStrictlyGreater) && disproofNumberStrictlyGreater > 0;
    }
    
    if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {     
      if (lower >= evalGoal) {
        if (!extraInfo.isFinished) {
          extraInfo.proofBeforeFinished = true;
        }
        HASH_MAP.reset();
        EVALUATOR_MIDGAME.evaluate(this.getPlayer(), this.getOpponent(), this.nEmpties, evalGoal-100, evalGoal-101);
        extraInfo.minProofGreaterEqual = Math.min(extraInfo.proofBeforeFinished ? descendants : Float.POSITIVE_INFINITY, EVALUATOR_MIDGAME.getNVisited());
        extraInfo.minProofGreaterEqualVar = Float.POSITIVE_INFINITY;
      } else {
        extraInfo.minProofGreaterEqual = Float.POSITIVE_INFINITY;
        extraInfo.minProofGreaterEqualVar = proofNumberGreaterEqual;
      }
      if (upper <= evalGoal) {
        if (!extraInfo.isFinished) {
          extraInfo.disproofBeforeFinished = true;
        }
        HASH_MAP.reset();
        EVALUATOR_MIDGAME.evaluate(this.getPlayer(), this.getOpponent(), this.nEmpties, evalGoal+100, evalGoal+101);
        extraInfo.minDisproofStrictlyGreater = Math.min(extraInfo.disproofBeforeFinished ? descendants : Float.POSITIVE_INFINITY, EVALUATOR_MIDGAME.getNVisited());
        extraInfo.minDisproofStrictlyGreaterVar = Float.POSITIVE_INFINITY;
      } else {
        extraInfo.minDisproofStrictlyGreater = Float.POSITIVE_INFINITY;
        extraInfo.minDisproofStrictlyGreaterVar = disproofNumberStrictlyGreater;
      }
    }
    assert areThisBoardEvalsOK();
  }
  
//  public boolean toBeSolved() {
//    if (nEmpties > Constants.EMPTIES_FOR_FORCED_MIDGAME + 3) {
//      return false;
//    } else if (nEmpties <= Constants.EMPTIES_FOR_FORCED_MIDGAME) {
//      return true;
//    }
////    System.out.println(probGreaterEqual);
//    return
//        (endgameTimeEstimator.proofNumber(player, opponent, evalGoal - 100, this.eval) 
//        < Constants.PROOF_NUMBER_FOR_ENDGAME && this.probGreaterEqual > 0.98);
////        (getDisproofNumberStrictlyGreater() < Constants.PROOF_NUMBER_FOR_ENDGAME && this.probGreaterEqual < 0.02);
////        (this.probGreaterEqual > 0.99 || this.probGreaterEqual < 0.01) &&
////        (getProofNumberGreaterEqual() * this.probGreaterEqual
////         + getDisproofNumberStrictlyGreater() * (1 - this.probGreaterEqual)
////         < Constants.PROOF_NUMBER_FOR_ENDGAME);
//  }

  @Override
  public synchronized String toString() {
    String str = new Board(player, opponent).toString() + eval + "\n";
    return str;// + "\n" + this.lower + "(" + this.bestVariationPlayer + ") " + this.upper + "(" + this.bestVariationOpponent + ")\n";
  }
  
  public synchronized boolean isPartiallySolved() {
    return probGreaterEqual == PROB_STEP && probStrictlyGreater == 0;
  }
  
  public synchronized boolean isSolved() {
    return this.lower == this.upper;
  }
  
  public synchronized StoredBoard[] getChildren() {
    return children;
  }

  // TODO!!!
  public synchronized StoredBoard bestChildProofStrictlyGreater() {
    StoredBoard best = null;
    float bestValue = Float.POSITIVE_INFINITY;
    for (StoredBoard child : children) {
      assert child.extraInfo.minProofGreaterEqual < Float.POSITIVE_INFINITY;
      if (child.extraInfo.minProofGreaterEqualVar - child.extraInfo.minProofGreaterEqual < bestValue) {
        best = child;
        bestValue = child.extraInfo.minProofGreaterEqualVar - child.extraInfo.minProofGreaterEqual;
      }
    }
    return best;
  }
  
  public synchronized StoredBoard bestChildProofGreaterEqual() {
    StoredBoard best = null;
    float bestValue = Float.POSITIVE_INFINITY;
    for (StoredBoard child : children) {
      if (child.extraInfo.minDisproofStrictlyGreaterVar < bestValue) {
        best = child;
        bestValue = child.extraInfo.minDisproofStrictlyGreaterVar;
      }
    }
    return best;
  }

  public synchronized StoredBoard bestChildEndgameStrictlyGreater() {
    assert bestChildEndgameStrictlyGreater != null && !bestChildEndgameStrictlyGreater.isBusy;
    return bestChildEndgameStrictlyGreater;
  }

  public synchronized StoredBoard bestChildEndgameGreaterEqual() {
    assert bestChildEndgameGreaterEqual != null && !bestChildEndgameGreaterEqual.isBusy;
    return bestChildEndgameGreaterEqual;
  }

  public synchronized StoredBoard bestChildMidgameStrictlyGreater() {
    assert probStrictlyGreater < PROB_STEP;
    assert !isLeaf();
    assert this.maxLogDerivativeProbStrictlyGreater == logDerivativeProbStrictlyGreater(bestChildMidgameStrictlyGreater);
    assert bestChildMidgameStrictlyGreater != null && !bestChildMidgameStrictlyGreater.isBusy;
    assert logDerivativeProbGreaterEqual(bestChildMidgameStrictlyGreater) <= 0;
    return bestChildMidgameStrictlyGreater;
  }

  public synchronized StoredBoard bestChildMidgameGreaterEqual() {
    assert !isLeaf();
    assert this.maxLogDerivativeProbGreaterEqual == logDerivativeProbGreaterEqual(bestChildMidgameGreaterEqual);
    assert bestChildMidgameGreaterEqual != null && !bestChildMidgameGreaterEqual.isBusy;
    assert logDerivativeProbGreaterEqual(bestChildMidgameGreaterEqual) <= 0;
    return bestChildMidgameGreaterEqual;
  }
  
  public synchronized float maxFiniteChildrenProofNumber() {
    float result = 0;
    for (StoredBoard child : children) {
      if (child.proofNumberGreaterEqual != Float.POSITIVE_INFINITY) {
        result = Math.max(result, child.proofNumberGreaterEqual);
      }
    }
    return result;
  }
 synchronized StoredBoard findLeastCommonAncestor() {
    if (fathers.isEmpty()) {
      return null;
    }
    if (fathers.size() == 1) {
      return fathers.get(0);
    }
    
    HashSet<StoredBoard> currentAncestors = new HashSet<>();
    HashSet<StoredBoard> nextAncestors = new HashSet<>();
    
    currentAncestors.add(this);
    
    while (true) {
      for (StoredBoard b : currentAncestors) {
        for (StoredBoard father : b.fathers) {
          if (father.nEmpties == b.nEmpties) {
            // father is a pass: it cannot be the least common ancestor,
            // because it has 1 child.
            assert(father.fathers.get(0).nEmpties == b.nEmpties + 1);
            nextAncestors.addAll(father.fathers);
          } else {
            assert(father.nEmpties == b.nEmpties + 1);
            nextAncestors.add(father);
          }
        }
      }
      if (nextAncestors.size() == 1) {
        return nextAncestors.iterator().next();
      }
      currentAncestors = nextAncestors;
      nextAncestors = new HashSet<>();
    }
  }

  boolean areThisBoardEvalsOK() {
    if (this.lower > this.eval || this.eval > this.upper) {
      throw new AssertionError("Wrong lower:" + lower + " / eval:" + eval + " / upper:" + upper + " for board\n" + this.getBoard());
    }
    return true;
  }

  boolean areDescendantsOK() {
    if (fathers.isEmpty() || isBusy) {
      return true;
    }
    int maxDescendants = 0;
    String fatherDesc = "";
    for (StoredBoard father : fathers) {
      if (father.isBusy) {
        return true;
      }
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
    if (isBusy || child.isBusy) {
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
    if (eval < -child.eval || lower < -child.upper || upper < -child.lower) {
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
  
  boolean isEvalOK() {
    if ((new GetMovesCache()).getNMoves(player, opponent) == 0
        && (new GetMovesCache()).getNMoves(opponent, player) == 0
        && (this.getProofNumberGreaterEqual() == 0 || this.getProofNumberGreaterEqual() == Float.POSITIVE_INFINITY)) {
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
      if (child.isBusy) {
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
    if (!areThisBoardEvalsOK() || !isEvalOK() || !areDescendantsOK()) {
      throw new AssertionError("Wrong areThisBoardEvalsOK or isEvalOK or areDescendantsOK.");
    }
    if (isBusy && !isLeaf()) {
      throw new AssertionError("A non-leaf cannot be busy.");
    }
    return isExtraInfoOK();// && isLeastSingleAncestorOK();
  }
}