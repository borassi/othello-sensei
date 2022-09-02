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
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;

public class StoredBoard extends TreeNodeInterface {

  private static final int[] ERRORS = {
      0, 0, 0, 0, 530, 585, 636, 696, 736, 780, 794, 828, 814, 821, 807, 797, 772, 768, 753, 732,
      709, 680, 552, 474, 480, 448, 461, 397, 429, 380, 403, 362, 372, 328, 338, 300, 332, 303,
      311, 290, 300, 265, 267, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250,
      250, 250, 250, 250, 250};

  static AtomicInteger lastEvalGoal = new AtomicInteger(-6500);
  final static EndgameTimeEstimatorInterface endgameTimeEstimator = new EndgameTimeEstimator();

  private final long player;
  private final long opponent;
  private AtomicBoolean updating;
  StoredBoard[] children;
  short nEmpties;
  short depth;
  short lower = -6400;
  short upper = 6400;
  short weakLower = -6500;
  short weakUpper = -6500;
  short leafEval = -6500;
  public final ArrayList<StoredBoard> fathers;
  private final Evaluation[] evaluations = new Evaluation[64];

  StoredBoard next;
  StoredBoard prev;

  public static AtomicInteger avoidNextPosFailCoeff = new AtomicInteger(2000);
  public static AtomicInteger proofNumberForAlphaBeta = new AtomicInteger(0);

  private static final short PROB_STEP = 255;
  private static final short[] COMBINE_PROB;
  private static final int[] LOG_DERIVATIVE;
  public static final int LOG_DERIVATIVE_MINUS_INF = -1000000000;
  public static final int LOG_DERIVATIVE_MULTIPLIER = 10000;
  private int nThreadsWorking = 0;

  static {
    ProbCombiner combiner = Constants.PROB_COMBINER;
    COMBINE_PROB = new short[(PROB_STEP + 1) * (PROB_STEP + 1)];
    LOG_DERIVATIVE = new int[(PROB_STEP + 1)];
    for (int i = 0; i <= PROB_STEP; ++i) {
      double x1 = i / (double) PROB_STEP;
      LOG_DERIVATIVE[i] = (int) Math.round(Math.max(LOG_DERIVATIVE_MINUS_INF,
          -LOG_DERIVATIVE_MULTIPLIER * Math.log(combiner.derivative(x1) / combiner.f(x1))));

      for (int j = 0; j <= PROB_STEP; ++j) {
        double x2 = j / (double) PROB_STEP;
        COMBINE_PROB[(i << 8) | j] = (short) Math.round(combiner.inverse(combiner.f(x1) * combiner.f(x2)) * PROB_STEP);
      }
    }
  }

  public class Evaluation {
    private short prob;
    final short evalGoal;
    protected float proofNumber;
    protected float disproofNumber;
    int maxLogDerivative;
    long descendants = 0;

    public float proofNumber() { return proofNumber; }
    public float disproofNumber() { return disproofNumber; }
    public int maxLogDerivative() { return maxLogDerivative; }

    public Evaluation(short evalGoal) {
      assert evalGoal <= 6400 && evalGoal >= -6400;
      assert (evalGoal + 6400) % 200 == 100;
      if (Constants.ASSERT_LOCKS) {
        assert Thread.holdsLock(StoredBoard.this);
      }
      this.evalGoal = evalGoal;
      this.prob = (short) PROB_STEP;
    }

    private boolean isSolved() {
      if (Constants.ASSERT_LOCKS) {
        assert Thread.holdsLock(StoredBoard.this);
      }
      return proofNumber == 0 || disproofNumber == 0;
    }

    private float getProb() {
      if (Constants.ASSERT_LOCKS) {
        assert Thread.holdsLock(StoredBoard.this);
      }
      return (float) prob / PROB_STEP;
    }

    int logDerivative(@NonNull Evaluation father) {
      if (Constants.ASSERT_LOCKS) {
        assert Thread.holdsLock(father.getStoredBoard());
        assert Thread.holdsLock(getStoredBoard());
      }
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
      assert !isLeaf();
      assert isLowerUpperOK();
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

    private StoredBoardBestDescendant bestDescendant(int alpha, int beta, boolean proof, ArrayList<Evaluation> ancestors) {
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
      setLeaf(prob, proofNumber, disproofNumber, 0);
    }
    void setLeaf(float prob, float proofNumber, float disproofNumber, float logDerivativeAdd) {
      if (Constants.ASSERT_LOCKS) {
        assert Thread.holdsLock(StoredBoard.this);
      }
      assert isLeaf();
      this.prob = roundProb(prob);
      this.proofNumber = proofNumber;
      this.disproofNumber = disproofNumber;
      if (getProb() == 0 || getProb() == 1) {
        maxLogDerivative = LOG_DERIVATIVE_MINUS_INF;
      } else {
        maxLogDerivative = (int) Math.round(LOG_DERIVATIVE_MULTIPLIER * Constants.LEAF_MULTIPLIER * (Math.log(getProb() * (1 - getProb())) + logDerivativeAdd));
      }
      assert maxLogDerivative < 0;
    }

    void setDisproved() {
      if (Constants.ASSERT_LOCKS) {
        assert Thread.holdsLock(StoredBoard.this);
      }
      setLeaf(0, Float.POSITIVE_INFINITY, 0);
    }

    void setProved() {
      if (Constants.ASSERT_LOCKS) {
        assert Thread.holdsLock(StoredBoard.this);
      }
      setLeaf(1, 0, Float.POSITIVE_INFINITY);
    }

    protected void updateFatherWithThisChild(Evaluation child) {
      assert child.evalGoal == -this.evalGoal;
      assert child != null;
      if (child.isSolved()) {
        if (child.getProb() > 0.5) {
          return;
        } else {
          prob = 0;
          proofNumber = 0;
          disproofNumber = Float.POSITIVE_INFINITY;
          maxLogDerivative = LOG_DERIVATIVE_MINUS_INF;
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
      if (child.getProb() > 0 && child.getProb() < 1) {
        int currentLogDerivative = child.maxLogDerivative - LOG_DERIVATIVE[child.prob];
        if (currentLogDerivative > maxLogDerivative) {
          assert Utils.arrayContains(getChildren(), child.getStoredBoard());
          maxLogDerivative = currentLogDerivative;
        }
      }
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

  private static short combineProb(int p1, int p2) {
    return COMBINE_PROB[(((int) p1) << 8) | p2];
  }

  static int toEvaluationIndex(int eval) {
    assert ((eval + 6300) % 200 == 0);
    assert (eval >= -6300);
    assert (eval <= 6300);
    return (eval + 6300) / 200;
  }

  public StoredBoard(long player, long opponent, short depth) {
    this.player = player;
    this.opponent = opponent;
    this.depth = depth;
    this.fathers = new ArrayList<>();
    this.children = null;
    this.prev = null;
    this.next = null;
    this.nEmpties = (short) (64 - Long.bitCount(player | opponent));
  }

  public static StoredBoard initialStoredBoard(Board b) {
    return initialStoredBoard(b.getPlayer(), b.getOpponent(), (short) 0);
  }

  public static short roundEval(int eval) {
    return (short) Math.max(-6300, Math.min(6300, ((eval + 20000) / 200) * 200 - 19900));
  }
  
  public static StoredBoard initialStoredBoard(long player, long opponent, short eval) {
    StoredBoard result = new StoredBoard(player, opponent, (short) 0);
    result.updating = new AtomicBoolean(false);
    int stddev = (int) (ERRORS[result.nEmpties] * Constants.MULT_STDDEV);
    result.setWeakLowerUpper(roundEval(eval - 3 * stddev), roundEval(eval + 3 * stddev));
    result.setLeaf(eval, (short) 4);
    return result;
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

  synchronized double stepsUntilEnd() {
    assert isLowerUpperOK();
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

  private synchronized void updateFatherWithThisChild(StoredBoard father) {
    assert fathers.contains(father);
    if (Constants.ASSERT_LOCKS) {
      assert Thread.holdsLock(father);
    }
    father.lower = (short) Math.max(father.lower, -upper);
    father.upper = (short) Math.max(father.upper, -lower);
    father.weakUpper = (short) Math.min(father.weakUpper, -weakLower);
    father.weakLower = (short) Math.max(father.weakLower, -weakUpper);
    for (int i = father.weakLower; i <= father.weakUpper; i += 200) {
      Evaluation eval = getEvaluation(-i);
      assert eval != null;
      Evaluation fatherEval = father.getEvaluation(i);
      fatherEval.updateFatherWithThisChild(eval);
    }
  }

  protected synchronized void updateFather() {
    lower = -6400;
    upper = -6400;
    assert !isLeaf();
    for (int i = weakLower; i <= weakUpper; i += 200) {
      Evaluation eval = getEvaluation(i);
      assert eval != null;
      eval.proofNumber = Float.POSITIVE_INFINITY;
      eval.disproofNumber = 0;
      eval.prob = (short) PROB_STEP;
      eval.maxLogDerivative = LOG_DERIVATIVE_MINUS_INF;
    }
    for (StoredBoard childBoard : getChildren()) {
      childBoard.updateFatherWithThisChild(this);
    }
    for (int i = weakLower; i <= weakUpper; i += 200) {
      Evaluation eval = getEvaluation(i);
      if (eval.getProb() == 1 || eval.getProb() == 0) {
        eval.maxLogDerivative = LOG_DERIVATIVE_MINUS_INF;
      } else {
        eval.maxLogDerivative = eval.maxLogDerivative + LOG_DERIVATIVE[eval.prob];
        assert eval.maxLogDerivative > LOG_DERIVATIVE_MINUS_INF;
      }
      eval.prob = (short) (PROB_STEP - eval.prob);
      assert Constants.MAX_PARALLEL_TASKS > 1 || eval.isChildLogDerivativeOK();
    }
    assert isLowerUpperOK();
  }

  protected synchronized void setNewLowerUpper(StoredBoard board) {
    if (Constants.ASSERT_LOCKS) {
      assert Thread.holdsLock(board);
    }
    if (updating.get()) {
      return;
    }
    board.setWeakLowerUpper(
        (short) (board.depth % 2 == 0 ? weakLower : -weakUpper),
        (short) (board.depth % 2 == 0 ? weakUpper : -weakLower));
  }

  protected void updateFathers() {
    synchronized (this) {
      if (!isLeaf()) {
        updateFather();
      }
    }
    // Use an index to avoid co-modification (if some other thread adds fathers in the meantime).
    for (int i = 0; i < fathers.size(); ++i) {
      StoredBoard father = fathers.get(i);
      father.updateFathers();
    }
  }
  public synchronized int getWeakLower() { return weakLower; }
  public synchronized int getWeakUpper() { return weakUpper; }

  public synchronized int getEval() {
    float eval = weakLower - 100;
    for (int evalGoal = weakLower; evalGoal <= weakUpper; evalGoal += 200) {
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

  @Override
  public int childLogDerivative(TreeNodeInterface child, int evalGoal) {
    return 0;
  }

  public synchronized boolean isSolved() {
    return getLower() == getUpper();
  }

  public synchronized boolean isPartiallySolved() {
    return getPercentileLower(Constants.ZERO_PERC_FOR_WEAK) ==
               getPercentileUpper(Constants.ZERO_PERC_FOR_WEAK);
  }

  public synchronized int getPercentileUpper(float p) {
    for (int eval = weakLower; eval <= weakUpper; eval += 200) {
      Evaluation curEval = getEvaluation(eval);
      if (curEval != null && curEval.getProb() <= p) {
        return eval - 100;
      }
    }
    return 6400;
  }
  public synchronized int getPercentileLower(float p) {
    for (int eval = weakUpper; eval >= weakLower; eval -= 200) {
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

  protected boolean updateWeakLowerUpper() {
    if (updating.getAndSet(true)) {
      return true;
    }
    boolean extendLower = false;
    boolean extendUpper = false;
    boolean reduced = false;
    synchronized (this) {
      float probLower = getProb(weakLower);
      float probUpper = getProb(weakUpper);
      float probLowerNext = getProb(weakLower + 200);
      float probUpperPrev = getProb(weakUpper - 200);
      if (probLower < 1 - Constants.PROB_INCREASE_WEAK_EVAL && weakLower - 200 >= lower) {
        extendLower = true;
        assert upper > lower + 200;
      }
      if (probUpper > Constants.PROB_INCREASE_WEAK_EVAL && weakUpper + 200 <= upper) {
        extendUpper = true;
      }
      if (!extendLower && !extendUpper && probLowerNext > 1 - Constants.PROB_REDUCE_WEAK_EVAL) {
        weakLower += 200;
        assert weakLower < weakUpper;
        reduced = true;
      }
      if (!extendLower && !extendUpper && probUpperPrev < Constants.PROB_REDUCE_WEAK_EVAL) {
        weakUpper -= 200;
        assert weakLower < weakUpper;
        reduced = true;
      }
    }
    if (extendLower) {
      this.extendEval((short) (weakLower - 200));
    }
    if (extendUpper) {
      this.extendEval((short) (weakUpper + 200));
    }
    assert isLowerUpperOK();
    updating.set(false);
//    if (reduced || extendUpper || extendLower) {
//      System.out.println(this.getDescendants() + ": " + weakLower + " " + weakUpper);
//    }
    return reduced || extendUpper || extendLower;
  }

  public void extendEval(short valueToUpdate) {
    synchronized (this) {
      addEvaluation(valueToUpdate);
      if (isLeaf()) {
        if (valueToUpdate < weakLower) {
          weakLower = valueToUpdate;
        } else if (valueToUpdate > weakUpper) {
          weakUpper = valueToUpdate;
        }
        assert leafEval != -6500;
        setLeaf(leafEval, (short) 4);
        assert isLowerUpperOK();
        return;
      }
    }
    for (StoredBoard child : children) {
      child.extendEval((short) -valueToUpdate);
    }
    synchronized (this) {
      if (valueToUpdate < weakLower) {
        weakLower = valueToUpdate;
      } else if (valueToUpdate > weakUpper) {
        weakUpper = valueToUpdate;
      }
      updateFather();
      assert isLowerUpperOK();
    }
  }

  public synchronized StoredBoardBestDescendant bestDescendant() {
    int evalGoal = nextPositionEvalGoal();
    if (evalGoal == -6500) {
      return null;
    }
    Evaluation firstEval = getEvaluation(evalGoal);
    boolean proof = firstEval.getProb() > 1 - Constants.PROB_FOR_PROOF || firstEval.getProb() < Constants.PROB_FOR_PROOF;
    int alpha = Math.min(evalGoal, getPercentileLower(Constants.PROB_FOR_ENDGAME_ALPHA_BETA));
    int beta = Math.max(evalGoal, getPercentileUpper(Constants.PROB_FOR_ENDGAME_ALPHA_BETA));
    return firstEval.bestDescendant(alpha, beta, proof, new ArrayList<>());
  }

  protected synchronized int nextPositionEvalGoal() {
    StoredBoard.Evaluation bestEval = null;
    long bestValue = Long.MIN_VALUE;
    for (int eval = weakLower; eval <= weakUpper; eval += 200) {
      StoredBoard.Evaluation curEval = getEvaluation(eval);
      if (curEval.proofNumber == 0 || curEval.disproofNumber == 0 ||
              (eval < weakUpper && getEvaluation(eval + 200).getProb() > 1 - Constants.MIN_PROB_EVAL_GOAL) ||
              (eval > weakLower && getEvaluation(eval - 200).getProb() < Constants.MIN_PROB_EVAL_GOAL)) {
        continue;
      }
      long curValue = 0;
      curValue += curEval.getProb() > Constants.MIN_PROB_EVAL_GOAL && curEval.getProb() < 1 - Constants.MIN_PROB_EVAL_GOAL ?
                      -LOG_DERIVATIVE_MINUS_INF * 10000L : 0;
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
    } else if (nEmpties <= Constants.EMPTIES_FOR_FORCED_MIDGAME - 2) {
      return true;
    }
    assert isLeaf();
    assert eval.proofNumber > 0 && eval.disproofNumber > 0;
    assert nEmpties > 0 && nEmpties <= 60;
    if (eval.getProb() >= 1 - Constants.PROB_FOR_EARLY_MIDGAME) {
      return eval.proofNumber < proofNumberForAlphaBeta.get();
    } else if (eval.getProb() <= Constants.PROB_FOR_EARLY_MIDGAME) {
      return eval.disproofNumber < proofNumberForAlphaBeta.get();
    }
    return false;
  }

  public final synchronized void setSolved(int newEval) {
    setUpper(newEval);
    setLower(newEval);
    updateFathers();
  }

  public final synchronized void setLower(int newLower) {
    assert isLeaf();
    assert leafEval != -6500;
    this.leafEval = (short) Math.max(this.leafEval, newLower);
    lower = (short) Math.max(lower, newLower);
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
    this.leafEval = (short) Math.min(this.leafEval, newUpper);
    upper = (short) Math.min(upper, newUpper);
    for (int eval = newUpper + 100; eval <= 6300; eval += 200) {
      Evaluation e = getEvaluation(eval);
      if (e != null) {
        e.setDisproved();
      }
    }
    assert isLowerUpperOK();
  }

  synchronized void setWeakLowerUpper(short weakLower, short weakUpper) {
    this.weakLower = weakLower;
    this.weakUpper = weakUpper;
    for (short i = weakLower; i <= weakUpper; i += 200) {
      addEvaluation(i);
    }
  }

  protected synchronized void setLeaf(short leafEval, short depth) {
    assert isLeaf();
    assert leafEval != -6500;
    this.leafEval = leafEval;
    for (int i = weakLower; i <= weakUpper; i += 200) {
      Evaluation evaluation = getEvaluation(i);
      assert evaluation != null;
      if (i < lower) {
        evaluation.setProved();
      } else if (i > upper) {
        evaluation.setDisproved();
      } else {
        double stddevMultiplier = 1;
        float prob = 1 - (float) Gaussian.CDF(i, leafEval, ERRORS[nEmpties] * Constants.MULT_STDDEV * stddevMultiplier);
        float proofNumber = (float) (StoredBoard.endgameTimeEstimator.proofNumber(getPlayer(), getOpponent(), i, leafEval));
        assert Float.isFinite(proofNumber) && proofNumber > 0;
        float disproofNumber = (float) (StoredBoard.endgameTimeEstimator.disproofNumber(getPlayer(), getOpponent(), i, leafEval));
        assert Float.isFinite(disproofNumber) && disproofNumber > 0;
        evaluation.setLeaf(prob, proofNumber, disproofNumber, depth < 12 ? (float) (-0.5F * Math.pow(depth - 4, 2)) : -100000);
      }
    }
  }

  protected Evaluation getEvaluation(int evalGoal) {
    if (Constants.ASSERT_LOCKS) {
      assert Thread.holdsLock(this);
    }
    return evaluations[toEvaluationIndex(evalGoal)];
  }

  private Evaluation addEvaluation(short evalGoal) {
//    assert threadId == Thread.currentThread().getId();
    int index = toEvaluationIndex(evalGoal);
    if (evaluations[index] != null) {
      return null;
    }
    Evaluation eval = new Evaluation(evalGoal);
    evaluations[index] = eval;
    return eval;
  }

  private static short roundProb(float prob) {
    if (prob <= Constants.MIN_PROB_LEAF) {
      return 0;
    } else if (prob >= 1 - Constants.MIN_PROB_LEAF) {
      return PROB_STEP;
    }
    return (short) Math.round(prob * PROB_STEP);
  }

  public int childLogDerivative(StoredBoard child, int evalGoal) {
    return child.getEvaluation(-evalGoal).logDerivative(getEvaluation(-evalGoal));
  }

  @NonNull
  @Override
  public String toString() {
    return new Board(player, opponent) + "\n";
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
    assert weakLower < weakUpper;
    for (int i = weakLower; i <= weakUpper; i += 200) {
      assert getEvaluation(i) != null;
    }
    if (!isLeaf() && Constants.ASSERT_LOWER_UPPER && Constants.MAX_PARALLEL_TASKS == 1) {
      for (StoredBoard child : getChildren()) {
        synchronized (child) {
          assert weakLower >= -child.weakUpper;
          if (weakUpper > -child.weakLower) {
            System.out.println(this + " " + weakUpper + " " + (-child.weakLower));
          }
          assert weakUpper <= -child.weakLower;
        }
      }
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