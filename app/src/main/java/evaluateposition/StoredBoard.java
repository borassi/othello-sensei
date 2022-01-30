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

import androidx.annotation.NonNull;

import board.Board;
import board.GetMoves;
import board.GetMovesCache;
import constants.Constants;
import constants.Stats;
import helpers.Gaussian;
import helpers.Utils;
import java.util.ArrayList;
import java.util.concurrent.atomic.AtomicInteger;

public class StoredBoard {

  private static final int[] ERRORS = {
      0, 0, 0, 0, 530, 585, 636, 696, 736, 780, 794, 828, 814, 821, 807, 797, 772, 768, 753, 732,
      709, 680, 552, 474, 480, 448, 461, 397, 429, 380, 403, 362, 372, 328, 338, 300, 332, 303,
      311, 290, 300, 265, 267, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250,
      250, 250, 250, 250, 250};


  private final long player;
  private final long opponent;
  static AtomicInteger lastEvalGoal = new AtomicInteger(-6500);

  final static EndgameTimeEstimatorInterface endgameTimeEstimator = new EndgameTimeEstimator();
  public final ArrayList<StoredBoard> fathers;
  StoredBoard[] children;

  int nEmpties;
  private final Evaluation[] evaluations = new Evaluation[64];
  public int depth;

  StoredBoard next;
  StoredBoard prev;
  int leafEval = -6500;
  int lower = -6400;
  int upper = 6400;
  int weakLower = -6300;
  int weakUpper = 6300;

  public static AtomicInteger avoidNextPosFailCoeff = new AtomicInteger(2000);
  public static AtomicInteger proofNumberForAlphaBeta = new AtomicInteger(0);

  private static final int PROB_STEP = 255;
  private static final int[] COMBINE_PROB;
  private static final int[] LOG_DERIVATIVE;
  public static final int LOG_DERIVATIVE_MINUS_INF = -20000000;
  public static final int LOG_DERIVATIVE_MULTIPLIER = 10000;
  private int nThreadsWorking = 0;

  static {
    ProbCombiner combiner = Constants.PROB_COMBINER;
    COMBINE_PROB = new int[(PROB_STEP + 1) * (PROB_STEP + 1)];
    LOG_DERIVATIVE = new int[(PROB_STEP + 1)];
    for (int i = 0; i <= PROB_STEP; ++i) {
      double x1 = i / (double) PROB_STEP;
      LOG_DERIVATIVE[i] = (int) Math.round(Math.max(LOG_DERIVATIVE_MINUS_INF,
          -LOG_DERIVATIVE_MULTIPLIER * Math.log(combiner.derivative(x1) / combiner.f(x1))));

      for (int j = 0; j <= PROB_STEP; ++j) {
        double x2 = j / (double) PROB_STEP;
        COMBINE_PROB[(i << 8) | j] = (int) Math.round(combiner.inverse(combiner.f(x1) * combiner.f(x2)) * PROB_STEP);
      }
    }
  }

  public class Evaluation {
    private int prob;
    protected float proofNumber;
    protected float disproofNumber;
    int maxLogDerivative;
    final int evalGoal;
    float bestDisproofNumberValue = Float.NEGATIVE_INFINITY;
    long descendants = 0;

    public float proofNumber() { return proofNumber; }
    public float disproofNumber() { return disproofNumber; }
    public int maxLogDerivative() { return maxLogDerivative; }

    public Evaluation(int evalGoal) {
      assert evalGoal <= 6400 && evalGoal >= -6400;
      assert (evalGoal + 6400) % 200 == 100;
      assert Thread.holdsLock(StoredBoard.this);
      this.evalGoal = evalGoal;
      this.prob = PROB_STEP;
    }

    private boolean isSolved() {
      assert Thread.holdsLock(StoredBoard.this);
      return proofNumber == 0 || disproofNumber == 0;
    }

    private float getProb() {
      assert Thread.holdsLock(StoredBoard.this);
      return (float) prob / PROB_STEP;
    }

    int logDerivative(@NonNull Evaluation father) {
      assert Thread.holdsLock(father.getStoredBoard());
      assert Thread.holdsLock(getStoredBoard());
      if (prob == 0 || prob == PROB_STEP || father.prob == 0 || father.prob == PROB_STEP) {
        return LOG_DERIVATIVE_MINUS_INF;
      }
      return maxLogDerivative - LOG_DERIVATIVE[prob] + LOG_DERIVATIVE[PROB_STEP - father.prob];
    }

    private double getValue(Evaluation father, boolean proof) {
      synchronized (StoredBoard.this) {
        float prob = father.getProb();
        Evaluation child = getEvaluation(-father.evalGoal);
        if (isSolved()) {
          return Double.NEGATIVE_INFINITY;
        }
        if (prob == 0) {
          return child.proofNumber - child.getStoredBoard().getNThreadsWorking() * 1.0E15;
        } else if (prob == 1 || (proof && prob > 0.5)) {
          return -Math.max(1, child.disproofNumber / Math.max(0.0001F, 1 - child.getProb()));
        } else {
//          System.out.println(avoidNextPosFailCoeff.get());
          return child.logDerivative(father) -
              (1 / Math.min(prob, 0.5)) * avoidNextPosFailCoeff.get() * child.getStoredBoard().getNThreadsWorking();
        }
      }
    }

    public Evaluation bestChild(boolean proof) {
      synchronized (StoredBoard.this) {
        assert !isLeaf();
//        assert EvaluatorMCTS.nextPositionLock.isHeldByCurrentThread();
        Evaluation bestChild = null;
        double bestChildValue = Double.NEGATIVE_INFINITY;
        for (StoredBoard childBoard : getChildren()) {
          synchronized (childBoard) {
            Evaluation child = childBoard.getEvaluation(-evalGoal);
            double childValue = child.getValue(this, proof);
            if (childValue > bestChildValue) {
              bestChild = child;
              bestChildValue = childValue;
            }
          }
        }
        assert isNThreadsWorkingOK();
        if (bestChild == null) {
          return null;
        }
        if (bestChild.getStoredBoard().isLeaf() && bestChild.getStoredBoard().getNThreadsWorking() > 0) {
          return null;
        }
        return bestChild;
      }
    }

    public StoredBoardBestDescendant bestDescendant(int alpha, int beta) {
      boolean proof = getProb() > 1 - Constants.PROB_FOR_PROOF || getProb() < Constants.PROB_FOR_PROOF;
      return bestDescendant(alpha, beta, proof, new ArrayList<>());
    }
    public StoredBoardBestDescendant bestDescendant(int alpha, int beta, boolean proof, ArrayList<Evaluation> ancestors) {
//      assert EvaluatorMCTS.nextPositionLock.isHeldByCurrentThread();
      Evaluation bestChild;
      synchronized (StoredBoard.this) {
        increaseNThreadsWorking();
        ancestors.add(this);
        if (isSolved() || (isLeaf() && getNThreadsWorking() > 1)) {
          bestChild = null;
        } else {
          if (isLeaf()) {
            avoidNextPosFailCoeff.addAndGet(-1);
            Stats.addToNSuccessNextPosition(1);
            return new StoredBoardBestDescendant(
                this,
                ancestors,
                alpha,
                beta
            );
          }
          alpha = Math.max(alpha, Math.min(getPercentileLower(Constants.PROB_FOR_ENDGAME_ALPHA_BETA) + 100, evalGoal));
          beta = Math.min(beta, Math.max(getPercentileUpper(Constants.PROB_FOR_ENDGAME_ALPHA_BETA) - 100, evalGoal));

          bestChild = bestChild(proof);
        }
      }
      if (bestChild == null) {
        avoidNextPosFailCoeff.addAndGet(10);
        for (Evaluation eval : ancestors) {
          eval.getStoredBoard().decreaseNThreadsWorking();
        }
        Stats.addToNFailNextPosition(1);
        return null;
      }
      return bestChild.bestDescendant(-beta, -alpha, proof, ancestors);
    }

    StoredBoard getStoredBoard() {
      return StoredBoard.this;
    }

    public void addDescendants(long newDescendants) {
      synchronized (StoredBoard.this) {
        descendants += newDescendants;
      }
    }

    void setLeaf(float prob, float proofNumber, float disproofNumber) {
      assert Thread.holdsLock(StoredBoard.this);
      assert isLeaf();
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

    void setDisproved() {
      assert Thread.holdsLock(StoredBoard.this);
      setLeaf(0, Float.POSITIVE_INFINITY, 0);
    }

    void setProved() {
      assert Thread.holdsLock(StoredBoard.this);
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
      for (StoredBoard childBoard : getChildren()) {
        synchronized (childBoard) {
          Evaluation child = childBoard.getEvaluation(-evalGoal);
          // When updating descendants, this Evaluation would be missing if any
          // child's evaluation is missing. So child must not be null.
          assert child != null;
          if (child.isSolved()) {
            if (child.getProb() > 0.5) {
              continue;
            } else {
              prob = 0;
              proofNumber = 0;
              disproofNumber = Float.POSITIVE_INFINITY;
              maxLogDerivative = LOG_DERIVATIVE_MINUS_INF;
              break;
            }
          }
          prob = combineProb(prob, child.prob);

          float curProofNumber = Math.max(1, child.disproofNumber / Math.max(0.0001F, 1 - child.getProb()));
          if (curProofNumber < proofNumber) {
            assert Utils.arrayContains(getChildren(), child.getStoredBoard());
            proofNumber = curProofNumber;
          }

          assert child.proofNumber > 0;
          disproofNumber += child.proofNumber;
          if (child.proofNumber > bestDisproofNumberValue) {
            assert Utils.arrayContains(getChildren(), child.getStoredBoard());
            bestDisproofNumberValue = child.proofNumber;
          }
          if (child.getProb() > 0 && child.getProb() < 1) {
            int currentLogDerivative = child.maxLogDerivative - LOG_DERIVATIVE[child.prob];
            if (currentLogDerivative > maxLogDerivative) {
              assert Utils.arrayContains(getChildren(), child.getStoredBoard());
              maxLogDerivative = currentLogDerivative;
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
      assert Constants.MAX_PARALLEL_TASKS > 1 || isChildLogDerivativeOK();
    }

    private boolean isChildLogDerivativeOK() {
      if (isLeaf()) {
        return true;
      }
      int expectedLogDerivative = LOG_DERIVATIVE_MINUS_INF;
      String childLogDerivatives = "";
      for (StoredBoard child : children) {
        synchronized (child) {
          Evaluation childEval = child.getEvaluation(-evalGoal);
          if (childEval.getProb() > 0 && childEval.getProb() < 1) {
            assert childEval.logDerivative(this) <= child.maxLogDerivative(-evalGoal);
            childLogDerivatives += "  " + childEval.logDerivative(this) + "\n";
            expectedLogDerivative = Math.max(expectedLogDerivative, childEval.logDerivative(this));
          }
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
  }
  
  public static StoredBoard initialStoredBoard(Board b) {
    return initialStoredBoard(b.getPlayer(), b.getOpponent());
  }
  
  public static StoredBoard initialStoredBoard(long player, long opponent) {
    return new StoredBoard(player, opponent, 0);
  }
  
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

  synchronized void increaseNThreadsWorking() {
    nThreadsWorking++;
  }

  synchronized void decreaseNThreadsWorking() {
    nThreadsWorking--;
  }

  synchronized int getNThreadsWorking() {
    return nThreadsWorking;
  }

  synchronized double stepsUntilEnd(int weakLower, int weakUpper) {
    // Lower means "better".
    double maxDerivative = Double.NEGATIVE_INFINITY;
    double stepsUntilEndProof = Double.POSITIVE_INFINITY;
    double stepsUntilEndDisproof = Double.POSITIVE_INFINITY;
    for (int eval = weakLower; eval <= weakUpper; eval += 200) {
      StoredBoard.Evaluation curEval = getEvaluation(eval);
      if (curEval.getProb() >= 1 - Constants.ZERO_PERC_FOR_WEAK) {
        stepsUntilEndProof = Math.max(stepsUntilEndProof, curEval.proofNumber());
      } else if (curEval.getProb() < Constants.ZERO_PERC_FOR_WEAK) {
        stepsUntilEndDisproof = Math.max(stepsUntilEndDisproof, curEval.disproofNumber());
      } else {
        maxDerivative = Math.max(maxDerivative, curEval.maxLogDerivative);
      }
    }
    if (maxDerivative == Double.NEGATIVE_INFINITY) {
      return stepsUntilEndProof + stepsUntilEndDisproof;
    }
    return (-LOG_DERIVATIVE_MINUS_INF + maxDerivative) * 1E20;
  }

  protected synchronized void updateFather(int alpha, int beta) {
    assert Constants.MAX_PARALLEL_TASKS > 1 || weakLower <= alpha;
    assert Constants.MAX_PARALLEL_TASKS > 1 || weakUpper >= beta;
    weakLower = Math.max(weakLower, alpha);
    weakUpper = Math.min(weakUpper, beta);
//    System.out.print(weakLower + " " + weakUpper + " " + alpha + " " + beta + " ");
    lower = -6400;
    upper = -6400;
    for (StoredBoard childBoard : getChildren()) {
      synchronized (childBoard) {
        lower = Math.max(lower, -childBoard.upper);
        upper = Math.max(upper, -childBoard.lower);
      }
    }
    for (int i = weakLower; i <= weakUpper; i += 200) {
//      System.out.print(i + " ");
      Evaluation eval = getEvaluation(i);
      // Eval can be null if some other thread is updating all descendants.
      if (eval != null) {
        eval.updateFather();
      }
    }
//    System.out.println();
    assert isLowerUpperOK();
  }

  protected void updateFathers(int alpha, int beta) {
    synchronized (this) {
      if (!isLeaf()) {
        updateFather(alpha, beta);
      }
    }
    // Use an index to avoid co-modification (if some other thread adds fathers in the meantime).
    for (int i = 0; i < fathers.size(); ++i) {
      StoredBoard father = fathers.get(i);
      father.updateFathers(-beta, -alpha);
    }
  }
  public synchronized int getWeakLower() { return weakLower; }
  public synchronized int getWeakUpper() { return weakUpper; }

  public synchronized int getEval() {
    float eval = weakLower - 100;
    for (int evalGoal = weakLower; evalGoal <= weakUpper; evalGoal += 200) {
      if (getEvaluation(evalGoal) == null) {
        System.out.println(evalGoal + " " + weakLower + " " + weakUpper + "\n" + this);
      }
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
    if (evalGoal < weakLower) {
      if (getEvaluation(weakLower).getProb() > 1 - Constants.PROB_INCREASE_WEAK_EVAL) {
        return 1;
      } else {
        return -1;
      }
    }
    if (evalGoal > weakUpper) {
      if (getEvaluation(weakLower).getProb() < Constants.PROB_INCREASE_WEAK_EVAL) {
        return 0;
      } else {
        return -1;
      }
    }
    return getEvaluation(evalGoal).getProb();
  }

  public synchronized float disproofNumber(int evalGoal) {
    if (evalGoal < weakLower || evalGoal > weakUpper) {
      return Float.MAX_VALUE;
    }
    return getEvaluation(evalGoal).disproofNumber();
  }

  public synchronized float proofNumber(int evalGoal) {
    if (evalGoal < weakLower || evalGoal > weakUpper) {
      return Float.MAX_VALUE;
    }
    return getEvaluation(evalGoal).proofNumber();
  }

  public synchronized int maxLogDerivative(int evalGoal) {
    if (evalGoal < weakLower || evalGoal > weakUpper) {
      return LOG_DERIVATIVE_MINUS_INF;
    }
    return getEvaluation(evalGoal).maxLogDerivative();
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

  public synchronized int getUpper() {
    assert isLowerUpperOK();
    return upper;
  }

  public synchronized int getLower() {
    assert isLowerUpperOK();
    return lower;
  }
  
  public synchronized long getDescendants() {
    long descendants = 0;
    for (int evalGoal = -6300; evalGoal <= 6300; evalGoal += 200) {
      descendants += getDescendants(evalGoal);
    }
    return descendants;
  }

  public synchronized long getDescendants(int evalGoal) {
    Evaluation eval = getEvaluation(evalGoal);
    if (eval == null) {
      return 0;
    }
    return eval.getDescendants();
  }

  public synchronized StoredBoardBestDescendant bestDescendant() {
    int evalGoal = nextPositionEvalGoal();
    if (evalGoal == -6500) {
      return null;
    }
    return getEvaluation(evalGoal).bestDescendant(weakLower, weakUpper);
  }

  protected synchronized int nextPositionEvalGoal() {
//    assert EvaluatorMCTS.nextPositionLock.isHeldByCurrentThread();
    StoredBoard.Evaluation bestEval = null;
    long bestValue = Long.MIN_VALUE;
    for (int eval = weakLower; eval <= weakUpper; eval += 200) {
      StoredBoard.Evaluation curEval = getEvaluation(eval);
      if (curEval.proofNumber == 0 || curEval.disproofNumber == 0) {
        continue;
      }
      long curValue = 0;
      curValue += curEval.getProb() > Constants.MIN_PROB_EVAL_GOAL && curEval.getProb() < 1 - Constants.MIN_PROB_EVAL_GOAL ?
                      -LOG_DERIVATIVE_MINUS_INF * 1000L : 0;
      curValue += curEval.maxLogDerivative * 1000L;
      curValue += curEval.evalGoal == lastEvalGoal.get() ? 0 : 1;

      if (curValue > bestValue) {
        bestValue = curValue;
        bestEval = curEval;
      }
    }
    if (bestEval == null) {
      assert Constants.MAX_PARALLEL_TASKS > 1;
      return -6500;
    }
    lastEvalGoal.set(bestEval.evalGoal);
    return bestEval.evalGoal;
  }

  public final synchronized boolean isLeaf() {
    return children == null;
  }

  public synchronized boolean toBeSolved(int evalGoal) {
    Evaluation eval = getEvaluation(evalGoal);
    if (nEmpties > Constants.EMPTIES_FOR_FORCED_MIDGAME + 3) {
      return false;
    } else if (nEmpties <= Constants.EMPTIES_FOR_FORCED_MIDGAME) {
      return true;
    }
    if (eval.getProb() >= 1 - Constants.PROB_FOR_EARLY_MIDGAME) {
      return eval.proofNumber < proofNumberForAlphaBeta.get();
    } else if (eval.getProb() <= Constants.PROB_FOR_EARLY_MIDGAME) {
      return eval.disproofNumber < proofNumberForAlphaBeta.get();
    }
    return false;
  }

  public final synchronized void setSolved(int newEval, int alpha, int beta) {
    setUpper(newEval);
    setLower(newEval);
    updateFathers(alpha, beta);
  }

  public final synchronized void setLower(int newLower) {
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

  public final synchronized void setUpper(int newUpper) {
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

  public synchronized void setLeaf(int leafEval) {
    setLeaf(leafEval, 1);
  }
  protected synchronized void setLeaf(int leafEval, int alpha, int beta, double stddevMultiplier) {
    weakLower = Math.max(weakLower, alpha);
    weakUpper = Math.min(weakUpper, beta);
    setLeaf(leafEval, stddevMultiplier);
  }

  protected synchronized void setLeaf(int leafEval, double stddevMultiplier) {
    assert isLeaf();
    assert leafEval != -6500;
    this.leafEval = leafEval;
    for (int i = weakLower; i <= weakUpper; i += 200) {
      Evaluation evaluation = getOrAddEvaluation(i);
      if (i < lower) {
        evaluation.setProved();
      } else if (i > upper) {
        evaluation.setDisproved();
      } else {
        float prob = 1 - (float) Gaussian.CDF(i, leafEval, ERRORS[nEmpties] * Constants.MULT_STDDEV * stddevMultiplier);
        float proofNumber = (float) (StoredBoard.endgameTimeEstimator.proofNumber(getPlayer(), getOpponent(), i, leafEval));
        assert Float.isFinite(proofNumber) && proofNumber > 0;
        float disproofNumber = (float) (StoredBoard.endgameTimeEstimator.disproofNumber(getPlayer(), getOpponent(), i, leafEval));
        assert Float.isFinite(disproofNumber) && disproofNumber > 0;
        evaluation.setLeaf(prob, proofNumber, disproofNumber);
      }
    }
  }

  public void updateDescendantsRecursive(int alpha, int beta) {
    synchronized (this) {
      if (isLeaf()) {
        weakLower = Math.min(weakLower, alpha);
        weakUpper = Math.max(weakUpper, beta);
        assert leafEval != -6500;
        setLeaf(leafEval, Constants.MULT_STDDEV);
        return;
      }
    }
    for (StoredBoard child : children) {
      child.updateDescendantsRecursive(-beta, -alpha);
    }
    synchronized (this) {
      weakLower = Math.min(weakLower, alpha);
      weakUpper = Math.max(weakUpper, beta);
      for (int i = alpha; i <= beta; i += 200) {
        getOrAddEvaluation(i).updateFather();
      }
      assert isLowerUpperOK();
    }
  }

  protected Evaluation getEvaluation(int evalGoal) {
    assert Thread.holdsLock(this);
    return evaluations[toEvaluationIndex(evalGoal)];
  }

  private Evaluation addEvaluation(int evalGoal) {
//    assert threadId == Thread.currentThread().getId();
    int index = toEvaluationIndex(evalGoal);
    if (evaluations[index] != null) {
      return null;
    }
    Evaluation eval = new Evaluation(evalGoal);
    evaluations[index] = eval;
    return eval;
  }

  protected synchronized Evaluation getOrAddEvaluation(int evalGoal) {
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

  public int childLogDerivative(StoredBoard child, int evalGoal) {
    return child.getEvaluation(-evalGoal).logDerivative(getEvaluation(-evalGoal));
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
      if (child.proofNumber(evalGoal) != Float.POSITIVE_INFINITY) {
        result = Math.max(result, child.proofNumber(evalGoal));
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
        && (this.proofNumber(evalGoal) == 0 || this.proofNumber(evalGoal) == Float.POSITIVE_INFINITY)) {
      return true;
    }
    return true;
  }

  synchronized boolean isNThreadsWorkingOK() {
    if (this.nThreadsWorking < 0) {
      throw new AssertionError("Got " + this.nThreadsWorking +
                                   " threads.");
    }
    if (this.nThreadsWorking > Constants.MAX_PARALLEL_TASKS) {
      throw new AssertionError("Got " + this.nThreadsWorking +
                                   " threads. Expected  at most "
                                   + Constants.MAX_PARALLEL_TASKS);
    }
    if (fathers.size() == 0) {
      return true;
    }
    int nThreadsWorkingFathers = 0;
    for (StoredBoard father : fathers) {
      nThreadsWorkingFathers += father.nThreadsWorking;
    }
    if (nThreadsWorkingFathers != this.nThreadsWorking && Constants.MAX_PARALLEL_TASKS == 1) {
      throw new AssertionError("Got " + this.nThreadsWorking +
                                   " threads. Expected " + nThreadsWorkingFathers);
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
    if (!isNThreadsWorkingOK()) {
      throw new AssertionError("Wrong isNThreadsWorking.");
    }
    return true;
  }
}