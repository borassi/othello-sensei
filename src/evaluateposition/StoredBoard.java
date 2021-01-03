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
import evaluatedepthone.PatternEvaluatorImproved;
import helpers.Gaussian;
import helpers.Utils;
import java.util.ArrayList;
import java.util.HashSet;

public class StoredBoard {
  public class ExtraInfo {
    public double minProofGreaterEqual = 0;
    public double minDisproofStrictlyGreater = 0;
    public double minDeltaProofGreaterEqual = 0;
    public double minDeltaDisproofStrictlyGreater = 0;
    public double nDescendants = 0;
    public boolean isFinished = false;
    
    public void setNDescendants(double value) {
      if (isFinished) {
        return;
      }
      nDescendants = value;
    }
  }
  private static HashMap HASH_MAP = new HashMap(6000, 3000);
  private static EvaluatorMidgame EVALUATOR_MIDGAME = new EvaluatorMidgame(PatternEvaluatorImproved.load(), HASH_MAP);
  
  private final long player;
  private final long opponent;

  static EndgameTimeEstimatorInterface endgameTimeEstimator = new EndgameTimeEstimator();
  final ArrayList<StoredBoard> fathers;
  StoredBoard[] children;
  
  int eval;
  int lower;
  int upper;
  int nEmpties;
  public double probGreaterEqual;
  public double probStrictlyGreater;
  // Positions to prove that eval >= evalGoal.
  double proofNumberGreaterEqual;
  // Positions to prove that eval <= evalGoal.
  double disproofNumberStrictlyGreater;
  public double maxLogDerivativeProbStrictlyGreater;
  public double maxLogDerivativeProbGreaterEqual;
  public double maxLogDerivativeDisproofNumberStrictlyGreater;
  public double maxLogDerivativeProofNumberGreaterEqual;
  int evalGoal;
  public boolean playerIsStartingPlayer;
  long descendants;

  StoredBoard next;
  StoredBoard prev;
  public ExtraInfo extraInfo;

  public final static GaussianNumberGenerator RANDOM = new GaussianNumberGenerator();

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
  
  StoredBoard getChild(long player, long opponent) {
    return new StoredBoard(player, opponent, !playerIsStartingPlayer);
  }
  
  public void addChildren(EvaluatorMCTS evaluator) {
    GetMovesCache mover = new GetMovesCache();
    
    long curMoves = mover.getMoves(player, opponent);
    int nMoves = Long.bitCount(curMoves);
    
    if (nMoves == 0) {
      if (mover.getNMoves(opponent, player) == 0) {
        setSolved(BitPattern.getEvaluationGameOver(player, opponent));
        assert isAllOK();
        return;
      } else {
        ++nMoves;
      }
    }
    children = new StoredBoard[nMoves];

    for (int i = 0; i < nMoves; ++i) {
      int move = Long.numberOfTrailingZeros(curMoves);
      long moveBit = 1L << move;
      curMoves = curMoves & (~moveBit);
      long flip = move == 64 ? 0 : mover.getFlip(move) & (opponent | moveBit);
      long newPlayer = opponent & ~flip;
      long newOpponent = player | flip;
      StoredBoard child = evaluator.get(newPlayer, newOpponent);
      if (child == null) {
        child = getChild(newPlayer, newOpponent);
        child.evalGoal = -evalGoal;
        evaluator.add(child);
      } else {
        if (this.getEvalGoal() != -child.getEvalGoal()) {
          child.setEvalGoalRecursive(-this.getEvalGoal());
        }
      }
      child.addFather(this);
      if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {
        child.extraInfo.isFinished = this.extraInfo.isFinished;
      }
      children[i] = child;
    }
  }
  
  public void setIsFinished(boolean newValue) {
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
  
  public void addFather(StoredBoard father) {
    fathers.add(father);
  }
  
  public boolean isLeaf() {
    return children == null;
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
  
  public int getEval() {
    return eval;
  }
  
  public double getProofNumberGreaterEqual() {
    return proofNumberGreaterEqual;
  }
  
  public double getDisproofNumberStrictlyGreater() {
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

  public final void setSolved(int newEval) {
    assert isLeaf();
    this.lower = newEval;
    this.eval = newEval;
    this.upper = newEval;
    this.setProofNumbersForLeaf();
    this.updateFathers();
    assert areThisBoardEvalsOK();
  }

  public final void setLower(int newLower) {
    assert isLeaf();
    this.lower = (short) Math.max(lower, newLower);
    this.eval = (short) Math.max(newLower, eval);
    this.setProofNumbersForLeaf();
    this.updateFathers();
    assert areThisBoardEvalsOK();
  }

  public final void setUpper(int newUpper) {
    assert isLeaf();
    this.upper = (short) Math.min(upper, newUpper);
    this.eval = (short) Math.min(newUpper, eval);
    this.setProofNumbersForLeaf();
    this.updateFathers();
    assert areThisBoardEvalsOK();
  }
  
  public final void setEval(int newEval) {
    assert isLeaf();
    this.eval = (short) Math.max(this.lower, Math.min(this.upper, newEval));
//    this.updateFathers();
    this.setProofNumbersForLeaf();
    assert areThisBoardEvalsOK();
  }
  
  protected void setEvalGoalRecursive(int evalGoal) {
    setEvalGoalRecursive(evalGoal, new HashSet<>());
  }
  protected void setEvalGoalRecursive(int evalGoal, HashSet<StoredBoard> done) {
    if (done.contains(this)) {
      return;
    }
    this.evalGoal = evalGoal;
    if (isLeaf()) {
      setProofNumbersForLeaf();
      return;
    }
    for (StoredBoard child : children) {
      child.setEvalGoalRecursive(-evalGoal, done);
    }
    updateFather();
    done.add(this);
    assert areChildrenOK();
  }

  private static double roundProb(double prob) {
    if (prob > 1 - Constants.PPN_MIN_COST_LEAF) {
      return 1;
    } else if (prob < Constants.PPN_MIN_COST_LEAF) {
      return 0;
    }
    return prob;
  }
  
  protected void updateFather() {
    assert(!isLeaf());
    eval = Short.MIN_VALUE;
    lower = Short.MIN_VALUE;
    upper = Short.MIN_VALUE;
    proofNumberGreaterEqual = 0;
    probGreaterEqual = 1 - children.length;
    probStrictlyGreater = 1 - children.length;
    disproofNumberStrictlyGreater = 0;

    for (StoredBoard child : children) {
      eval = Math.max(eval, -child.eval);
      lower = Math.max(lower, -child.upper);
      upper = Math.max(upper, -child.lower);
      proofNumberGreaterEqual += Math.max(1.E-5, 1-child.probStrictlyGreater) * Math.pow(child.disproofNumberStrictlyGreater, Constants.PPN_MU);
      disproofNumberStrictlyGreater += child.proofNumberGreaterEqual;
      probGreaterEqual += Math.pow(child.probStrictlyGreater, Constants.PPN_LAMBDA);
      probStrictlyGreater += Math.pow(child.probGreaterEqual, Constants.PPN_LAMBDA);
    }
    probGreaterEqual = roundProb(1 - Math.pow(probGreaterEqual, 1 / Constants.PPN_LAMBDA));
    probStrictlyGreater = roundProb(1 - Math.pow(probStrictlyGreater, 1 / Constants.PPN_LAMBDA));
    proofNumberGreaterEqual = Math.pow(proofNumberGreaterEqual / Math.max(1.E-5, probGreaterEqual), 1 / Constants.PPN_MU);

    maxLogDerivativeProbStrictlyGreater = Double.NEGATIVE_INFINITY;
    maxLogDerivativeProbGreaterEqual = Double.NEGATIVE_INFINITY;
    maxLogDerivativeDisproofNumberStrictlyGreater = Double.NEGATIVE_INFINITY;
    maxLogDerivativeProofNumberGreaterEqual = Double.NEGATIVE_INFINITY;

    for (StoredBoard child : children) {
      maxLogDerivativeProbStrictlyGreater = Math.max(maxLogDerivativeProbStrictlyGreater, logDerivativeProbStrictlyGreater(child));
      maxLogDerivativeProbGreaterEqual = Math.max(maxLogDerivativeProbGreaterEqual, logDerivativeProbGreaterEqual(child));

      maxLogDerivativeDisproofNumberStrictlyGreater = Math.max(maxLogDerivativeDisproofNumberStrictlyGreater,
          logDerivativeDisproofNumberStrictlyGreater(child));

      maxLogDerivativeProofNumberGreaterEqual = Math.max(maxLogDerivativeProofNumberGreaterEqual,
          logDerivativeProofNumberGreaterEqual(child));
    }
    if (disproofNumberStrictlyGreater == Double.POSITIVE_INFINITY) {
      maxLogDerivativeDisproofNumberStrictlyGreater = Double.NEGATIVE_INFINITY;
    }

    if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {
      extraInfo.minProofGreaterEqual = Double.POSITIVE_INFINITY;
      extraInfo.minDisproofStrictlyGreater = 0;
      extraInfo.setNDescendants(0);
      extraInfo.minDeltaDisproofStrictlyGreater = Double.POSITIVE_INFINITY;
      extraInfo.minDeltaProofGreaterEqual = Double.POSITIVE_INFINITY;
      for (StoredBoard child : children) {
        extraInfo.minProofGreaterEqual = Math.min(extraInfo.minProofGreaterEqual, child.extraInfo.minDisproofStrictlyGreater);        
        extraInfo.minDisproofStrictlyGreater += child.extraInfo.minProofGreaterEqual;
        extraInfo.minDeltaDisproofStrictlyGreater = Math.min(extraInfo.minDeltaDisproofStrictlyGreater, child.extraInfo.minDeltaProofGreaterEqual);
//        extraInfo.minDeltaProofGreaterEqual = Math.min(extraInfo.minDeltaProofGreaterEqual, child.extraInfo.minDeltaDisproofStrictlyGreater);
        extraInfo.setNDescendants(extraInfo.nDescendants + child.extraInfo.nDescendants);
      }
      for (StoredBoard child : children) {
//        System.out.println("  " + child.extraInfo.minDeltaDisproofStrictlyGreater);
        extraInfo.minDeltaProofGreaterEqual = Math.min(
            extraInfo.minDeltaProofGreaterEqual,
            childValueProofGreaterEqual(child));
      }
//      System.out.println(extraInfo.minDeltaProofGreaterEqual);
    }
    assert areThisBoardEvalsOK();
    assert isEvalOK();
    assert areDescendantsOK();
  }
  
  // TODOTODO!!!
  public double childValueProofGreaterEqual(StoredBoard child) {
    if (extraInfo.minProofGreaterEqual == Double.POSITIVE_INFINITY) {
      return Double.POSITIVE_INFINITY;
    }
    double tmp = Double.POSITIVE_INFINITY;
    if (child.disproofNumberStrictlyGreater != 0) {
      return child.disproofNumberStrictlyGreater + child.extraInfo.nDescendants - extraInfo.minProofGreaterEqual;
    }
    return child.extraInfo.minDeltaDisproofStrictlyGreater + child.extraInfo.minDisproofStrictlyGreater - extraInfo.minProofGreaterEqual;
  }
  
  public double logDerivativeProbStrictlyGreater(StoredBoard child) {
    assert Utils.arrayContains(children, child);
    if (child.probGreaterEqual == 0) {
      return Double.NEGATIVE_INFINITY;
    }
    return child.maxLogDerivativeProbGreaterEqual + (1-Constants.PPN_LAMBDA) * Math.min(0, Math.log((1 - probStrictlyGreater) / child.probGreaterEqual));
  }
  
  public double logDerivativeProbGreaterEqual(StoredBoard child) {
    assert Utils.arrayContains(children, child);
    if (child.probStrictlyGreater == 0) {
      return Double.NEGATIVE_INFINITY;
    }
    return child.maxLogDerivativeProbStrictlyGreater + (1-Constants.PPN_LAMBDA) * Math.min(0, Math.log((1 - probGreaterEqual) / child.probStrictlyGreater));
  }
  
  public double logDerivativeDisproofNumberStrictlyGreater(StoredBoard child) {
    assert Utils.arrayContains(children, child);
    return child.maxLogDerivativeProofNumberGreaterEqual;
  }

  public double logDerivativeProofNumberGreaterEqual(StoredBoard child) {
    assert Utils.arrayContains(children, child);
    if (child.probStrictlyGreater == 1 || proofNumberGreaterEqual == 0) {
      return Double.NEGATIVE_INFINITY;
    }
    if (child.probStrictlyGreater == 0) {
      return child.maxLogDerivativeDisproofNumberStrictlyGreater +
          (1 - Constants.PPN_MU) * Math.log(this.proofNumberGreaterEqual / child.disproofNumberStrictlyGreater);
    }
    return Math.log((-1 / Constants.PPN_MU) * proofNumberGreaterEqual *
        Math.pow(proofNumberGreaterEqual / child.disproofNumberStrictlyGreater, -Constants.PPN_MU) / probGreaterEqual)
        + child.maxLogDerivativeProbStrictlyGreater;
  }

  protected void updateFathers() {
    if (!isLeaf()) {
      updateFather();
    }
    for (StoredBoard father : fathers) {
      father.updateFathers();
    }
  }
  
  public void setProofNumbersForLeaf() {
    assert this.isLeaf();
    assert evalGoal <= 6400 && evalGoal >= -6400;
    assert descendants > 0;
    probGreaterEqual = roundProb(1 - Gaussian.CDF(evalGoal-100, eval, 400 - nEmpties * 4));
    probStrictlyGreater = roundProb(1 - Gaussian.CDF(evalGoal+100, eval, 400 - nEmpties * 4));
    double disproofNumberGreaterEqual = endgameTimeEstimator.disproofNumber(player, opponent, evalGoal - 100, this.eval);
    double proofNumberStrictlyGreater = endgameTimeEstimator.proofNumber(player, opponent, evalGoal + 100, this.eval);

    boolean toBeSolved = this.toBeSolved();
    double n = 200;

    if (lower >= evalGoal) {
      this.proofNumberGreaterEqual = 0;
      probGreaterEqual = 1;
      maxLogDerivativeProbGreaterEqual = Double.NEGATIVE_INFINITY;
      maxLogDerivativeProofNumberGreaterEqual = Double.NEGATIVE_INFINITY;
    } else if (upper < evalGoal) {
      this.proofNumberGreaterEqual = Double.POSITIVE_INFINITY;
      probGreaterEqual = 0;
      maxLogDerivativeProbGreaterEqual = Double.NEGATIVE_INFINITY;
      maxLogDerivativeProofNumberGreaterEqual = Double.NEGATIVE_INFINITY;
    } else {
      this.proofNumberGreaterEqual = endgameTimeEstimator.proofNumber(player, opponent, evalGoal - 100, this.eval);
        
      if (toBeSolved) {
        this.maxLogDerivativeProbGreaterEqual = Math.log(probGreaterEqual * (1 - probGreaterEqual) / proofNumberGreaterEqual +
            probGreaterEqual * (1 - probGreaterEqual) / disproofNumberGreaterEqual);
        this.maxLogDerivativeProofNumberGreaterEqual = 0;
      } else {
        this.maxLogDerivativeProbGreaterEqual = Math.log(probGreaterEqual * (1 - probGreaterEqual) / n);
        this.maxLogDerivativeProofNumberGreaterEqual = Math.log(this.proofNumberGreaterEqual * 0.1 / n);
      }
    }
    if (upper <= evalGoal) {
      probStrictlyGreater = 0;
      this.disproofNumberStrictlyGreater = 0;
      this.maxLogDerivativeProbStrictlyGreater = Double.NEGATIVE_INFINITY;
      maxLogDerivativeDisproofNumberStrictlyGreater = Double.NEGATIVE_INFINITY;
    } else if (lower > evalGoal) {
      probStrictlyGreater = 1;
      this.disproofNumberStrictlyGreater = Double.POSITIVE_INFINITY;
      this.maxLogDerivativeProbStrictlyGreater = Double.NEGATIVE_INFINITY;
      maxLogDerivativeDisproofNumberStrictlyGreater = Double.NEGATIVE_INFINITY;
    } else {
      this.disproofNumberStrictlyGreater = endgameTimeEstimator.disproofNumber(player, opponent, evalGoal + 100, this.eval);
      
      if (toBeSolved) {
        this.maxLogDerivativeProbStrictlyGreater = Math.log(probStrictlyGreater * (1 - probStrictlyGreater) / proofNumberStrictlyGreater +
            probStrictlyGreater * (1 - probStrictlyGreater) / disproofNumberStrictlyGreater);
        this.maxLogDerivativeDisproofNumberStrictlyGreater = 0;
      } else {
        this.maxLogDerivativeProbStrictlyGreater = Math.log(probStrictlyGreater * (1 - probStrictlyGreater) / n);
        this.maxLogDerivativeDisproofNumberStrictlyGreater = Math.log(this.disproofNumberStrictlyGreater * 0.1 / n);
      }
    }
    if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {
      extraInfo.setNDescendants(this.descendants);
      extraInfo.minDeltaDisproofStrictlyGreater = Double.POSITIVE_INFINITY;
      extraInfo.minDeltaProofGreaterEqual = Double.POSITIVE_INFINITY;
      
      if (lower >= evalGoal) {
        HASH_MAP.reset();
        EVALUATOR_MIDGAME.resetNVisitedPositions();
        EVALUATOR_MIDGAME.evaluatePosition(this.getPlayer(), this.getOpponent(), this.nEmpties, evalGoal-100, evalGoal-101);
        extraInfo.minProofGreaterEqual = EVALUATOR_MIDGAME.getNVisited();
        extraInfo.setNDescendants(extraInfo.minProofGreaterEqual);
      } else {
        extraInfo.minProofGreaterEqual = Double.POSITIVE_INFINITY;
      }
      if (upper <= evalGoal) {
        HASH_MAP.reset();
        EVALUATOR_MIDGAME.resetNVisitedPositions();
        EVALUATOR_MIDGAME.evaluatePosition(this.getPlayer(), this.getOpponent(), this.nEmpties, evalGoal+100, evalGoal+101);
        extraInfo.minDisproofStrictlyGreater = EVALUATOR_MIDGAME.getNVisited();
        extraInfo.setNDescendants(extraInfo.minDisproofStrictlyGreater);
      } else {
        extraInfo.minDisproofStrictlyGreater = Double.POSITIVE_INFINITY;
      }
    }
    assert areThisBoardEvalsOK();
  }
  
  public boolean toBeSolved() {
    if (nEmpties > Constants.EMPTIES_FOR_FORCED_MIDGAME + 3) {
      return false;
    } else if (nEmpties <= Constants.EMPTIES_FOR_FORCED_MIDGAME) {
      return true;
    }
    return 
        (getProofNumberGreaterEqual() < Constants.PROOF_NUMBER_FOR_ENDGAME && getEval() > getEvalGoal() + 2000) ||
        (getDisproofNumberStrictlyGreater() < Constants.PROOF_NUMBER_FOR_ENDGAME && getEval() < getEvalGoal() - 2000);
  }

  @Override
  public String toString() {
    String str = new Board(player, opponent).toString() + eval + "\n";
    return str;// + "\n" + this.lower + "(" + this.bestVariationPlayer + ") " + this.upper + "(" + this.bestVariationOpponent + ")\n";
  }
  
  public boolean isPartiallySolved() {
    return probGreaterEqual > 1 - Constants.PPN_MIN_COST_LEAF && probStrictlyGreater < Constants.PPN_MIN_COST_LEAF;
//    return this.lower == this.upper;
  }
  
  public boolean isSolved() {
    return this.lower == this.upper;
  }
  
  public StoredBoard[] getChildren() {
    return children;
  }

  public double childValueStrictlyGreater(StoredBoard child) {
    assert Utils.arrayContains(children, child);
    if (child.proofNumberGreaterEqual == 0 || disproofNumberStrictlyGreater == Double.POSITIVE_INFINITY) {
      return Double.NEGATIVE_INFINITY;
    }
    return child.proofNumberGreaterEqual;
  }
  
  public double childValueGreaterEqual(StoredBoard child) {
    assert Utils.arrayContains(children, child);
    if (proofNumberGreaterEqual == 0 || child.disproofNumberStrictlyGreater == Double.POSITIVE_INFINITY) {
      return Double.NEGATIVE_INFINITY;
    }
    if (child.probStrictlyGreater > 0.9) {
      return -1E30 * (1 + child.probStrictlyGreater);
    }
    return -child.disproofNumberStrictlyGreater / Math.exp(0.1 * Math.pow(proofNumberGreaterEqual, 0.1) / Math.sqrt(child.descendants));
  }
  
  // TODO!!!
  public StoredBoard bestChildProofStrictlyGreater() {
    StoredBoard best = null;
    double bestValue = Double.POSITIVE_INFINITY;
    for (StoredBoard child : children) {
      if (child.extraInfo.minDeltaProofGreaterEqual < bestValue) {
        best = child;
        bestValue = child.extraInfo.minDeltaProofGreaterEqual;
      }
    }
//    if (best == null) {
//      throw new RuntimeException("\n" + best + "\n" + this + "\n" + children.length);
//    }
    return best;
  }
  
  public StoredBoard bestChildProofGreaterEqual() {
    StoredBoard best = null;
    double bestValue = Double.POSITIVE_INFINITY;
    for (StoredBoard child : children) {
      if (childValueProofGreaterEqual(child) < bestValue) {
        best = child;
        bestValue = childValueProofGreaterEqual(child);
      }
    }
//    if (best == null) {
//      throw new RuntimeException("\n" + best + "\n" + this + "\n" + children.length);
//    }
    return best;
  }

  public StoredBoard bestChildEndgameStrictlyGreater() {
    StoredBoard best = null;
    double bestValue = Double.NEGATIVE_INFINITY;
    for (StoredBoard child : children) {
      if (childValueStrictlyGreater(child) > bestValue) {
        best = child;
        bestValue = childValueStrictlyGreater(child);
      }
    }
    return best;
  }
  public StoredBoard bestChildEndgameGreaterEqual() {
    StoredBoard best = null;
    double bestValue = Double.NEGATIVE_INFINITY;
    for (StoredBoard child : children) {
      if (childValueGreaterEqual(child) > bestValue) {
        best = child;
        bestValue = childValueGreaterEqual(child);
      }
    }
    return best;
  }

  public StoredBoard bestChildMidgameStrictlyGreater() {
    assert !isLeaf();
    StoredBoard best = null;
    double bestValue = Double.NEGATIVE_INFINITY;

    for (StoredBoard child : children) {
      if (logDerivativeProbStrictlyGreater(child) > bestValue) {
        best = child;
        bestValue = logDerivativeProbStrictlyGreater(child);
      }
    }
    assert best != null;
    assert logDerivativeProbStrictlyGreater(best) + best.maxLogDerivativeProbStrictlyGreater < 0;
    return best;
  }

  public StoredBoard bestChildMidgameGreaterEqual() {
    assert !isLeaf();
    StoredBoard best = null;
    double bestValue = Double.NEGATIVE_INFINITY;

    for (StoredBoard child : children) {
      if (logDerivativeProbGreaterEqual(child) > bestValue) {
        best = child;
        bestValue = logDerivativeProbGreaterEqual(child);
      }
    }
    assert best != null;
    assert bestValue < 0;
    return best;
  }

  boolean areThisBoardEvalsOK() {
    if (this.lower > this.eval || this.eval > this.upper) {
      throw new AssertionError("Wrong lower:" + lower + " / eval:" + eval + " / upper:" + upper + " for board\n" + this.getBoard());
    }
    return true;
  }

  boolean areDescendantsOK() {
    if (fathers.isEmpty()) {
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
        && (this.getProofNumberGreaterEqual() == 0 || this.getProofNumberGreaterEqual() == Double.POSITIVE_INFINITY)) {
      int correctEval = BitPattern.getEvaluationGameOver(player, opponent);
      if (eval != correctEval || lower != correctEval || upper != correctEval) {
        throw new AssertionError(
            "Got wrong eval/lower/upper = " + eval + "/" + lower + "/" + upper + 
            " (expected " + correctEval + ") for board\n" + this.getBoard());
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
    return true;
  }
}