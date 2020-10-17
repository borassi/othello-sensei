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
  private final long player;
  private final long opponent;

  static EndgameTimeEstimatorInterface endgameTimeEstimator = new EndgameTimeEstimator();
  final ArrayList<StoredBoard> fathers;
  StoredBoard[] children;
  
  int eval;
  int lower;
  int upper;
  public double probGreaterEqualEvalGoal;
  public double probStrictlyGreaterEvalGoal;
  // Positions to prove that eval >= evalGoal.
  double proofNumberCurEval;
  // Positions to prove that eval > evalGoal.
  double proofNumberNextEval;
  // Positions to prove that eval <= evalGoal.
  double disproofNumberCurEval;
  // Positions to prove that eval < evalGoal.
  double disproofNumberNextEval;
  public double minLogDerivativePlayerVariates;
  public double minLogDerivativeOpponentVariates;
  int evalGoal;
  final boolean playerIsStartingPlayer;
  long descendants;

  StoredBoard next;
  StoredBoard prev;

  public final static GaussianNumberGenerator RANDOM = new GaussianNumberGenerator();

  private StoredBoard(
      long player, long opponent, int eval, int evalGoal,
      boolean playerIsStartingPlayer, long descendants) {
    this.player = player;
    this.opponent = opponent;
    this.playerIsStartingPlayer = playerIsStartingPlayer;
    this.lower = -6400;
    this.upper = 6400;
    this.fathers = new ArrayList<>();
    this.children = null;
    this.evalGoal = evalGoal;
    this.prev = null;
    this.next = null;
    this.descendants = descendants;
    this.setEval(eval);
  }
  
  public static StoredBoard initialStoredBoard(Board b, int eval, int evalGoal, long descendants) {
    return initialStoredBoard(b.getPlayer(), b.getOpponent(), eval, evalGoal, descendants);
  }
  
  public static StoredBoard initialStoredBoard(
      long player, long opponent, int eval, int evalGoal, long descendants) {
    return new StoredBoard(
        player, opponent, eval, evalGoal, true, descendants);
  }

  public static StoredBoard childStoredBoard(
      Board b, StoredBoard father, int eval, long descendants) {
    return childStoredBoard(b.getPlayer(), b.getOpponent(), father, eval, descendants);
  }
  
  public static StoredBoard childStoredBoard(
      long player, long opponent, StoredBoard father, int eval, long descendants) {
    StoredBoard result = new StoredBoard(
        player, opponent, eval, -father.evalGoal, !father.playerIsStartingPlayer,
        descendants);
    result.addFather(father);
    return result;
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
  
  public double getProofNumberCurEval() {
    return proofNumberCurEval;
  }
  
  public double getProofNumberNextEval() {
    return proofNumberNextEval;
  }
  
  public double getDisproofNumberCurEval() {
    return disproofNumberCurEval;
  }

  public double getDisproofNumberNextEval() {
    return disproofNumberNextEval;
  }
  
  public boolean isPartiallySolved() {
    return getDescendants() >= 0.05 * (proofNumberCurEval + disproofNumberCurEval);
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
    this.updateFathers();
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

  protected void updateFather() {
    assert(!isLeaf());
    eval = Short.MIN_VALUE;
    lower = Short.MIN_VALUE;
    upper = Short.MIN_VALUE;
    proofNumberCurEval = Double.POSITIVE_INFINITY;
    probGreaterEqualEvalGoal = 1 - children.length;
    proofNumberNextEval = Double.POSITIVE_INFINITY;
    probStrictlyGreaterEvalGoal = 1 - children.length;
    disproofNumberCurEval = 0;
    disproofNumberNextEval = 0;

    for (StoredBoard child : children) {
      eval = Math.max(eval, -child.eval);
      lower = Math.max(lower, -child.upper);
      upper = Math.max(upper, -child.lower);
      proofNumberCurEval = Math.min(proofNumberCurEval, child.disproofNumberCurEval);
      proofNumberNextEval = Math.min(proofNumberNextEval, child.disproofNumberNextEval);
      disproofNumberCurEval += child.proofNumberCurEval;
      disproofNumberNextEval += child.proofNumberNextEval;
      probGreaterEqualEvalGoal += Math.pow(child.probStrictlyGreaterEvalGoal, Constants.LAMBDA);
      probStrictlyGreaterEvalGoal += Math.pow(child.probGreaterEqualEvalGoal, Constants.LAMBDA);
    }
    probGreaterEqualEvalGoal = 1 - Math.pow(probGreaterEqualEvalGoal, 1 / Constants.LAMBDA);
    probStrictlyGreaterEvalGoal = 1 - Math.pow(probStrictlyGreaterEvalGoal, 1 / Constants.LAMBDA);

    minLogDerivativePlayerVariates = Double.NEGATIVE_INFINITY;
    minLogDerivativeOpponentVariates = Double.NEGATIVE_INFINITY;
    for (StoredBoard child : children) {
      minLogDerivativePlayerVariates = Math.max(
          minLogDerivativePlayerVariates,
          child.minLogDerivativeOpponentVariates + logDerivativePlayerVariates(child));
      minLogDerivativeOpponentVariates = Math.max(
          minLogDerivativeOpponentVariates,
          child.minLogDerivativePlayerVariates + logDerivativeOpponentVariates(child));
    }
    assert areThisBoardEvalsOK();
    assert isEvalOK();
    assert areDescendantsOK();
  }
  
  public double logDerivativePlayerVariates(StoredBoard child) {
    assert Utils.arrayContains(children, child);
    if (child.probGreaterEqualEvalGoal == 0) {
      return Double.NEGATIVE_INFINITY;
    }
    if (Math.log((1 - probStrictlyGreaterEvalGoal) / child.probGreaterEqualEvalGoal) > 1.E-8) {
      System.out.print(1 - probStrictlyGreaterEvalGoal + " > ");
      for (StoredBoard child1 : children) {
        System.out.print(child1.probGreaterEqualEvalGoal + ", ");
      }
      System.out.println("BIG MISTAKE!!");
    }
    return Math.min(0, (1 - Constants.LAMBDA) * Math.log((1 - probStrictlyGreaterEvalGoal) / child.probGreaterEqualEvalGoal));
  }
  
  public double logDerivativeOpponentVariates(StoredBoard child) {
    assert Utils.arrayContains(children, child);
    if (child.probStrictlyGreaterEvalGoal == 0) {
      return Double.NEGATIVE_INFINITY;
    }
    if (-Math.log((1 - probGreaterEqualEvalGoal) / child.probStrictlyGreaterEvalGoal) < -1.E-8) {
      System.out.print(1 - probGreaterEqualEvalGoal + " > ");
      for (StoredBoard child1 : children) {
        System.out.print(child1.probStrictlyGreaterEvalGoal + ", ");
      }
      System.out.println("BIG MISTAKE!");
    }
    return Math.min(0, (1 - Constants.LAMBDA) * Math.log((1 - probGreaterEqualEvalGoal) / child.probStrictlyGreaterEvalGoal));
  }

  protected void updateFathers() {
    for (StoredBoard father : fathers) {
      father.updateFather();
      father.updateFathers();
    }
  }
  
  public final void setProofNumbersForLeaf() {
    assert this.isLeaf();
    assert evalGoal <= 6400 && evalGoal >= -6400;
    assert descendants > 0;
    probGreaterEqualEvalGoal = 1 - Gaussian.CDF(evalGoal-100, eval, 400);
    double maxProbGreaterEqualEvalGoal = 1 - 
        Math.min(
        Constants.PPN_MIN_COST_LEAF,
        Constants.PPN_EPSILON * endgameTimeEstimator.proofNumber(
            player, opponent, evalGoal - 100, this.eval));
    double minProbGreaterEqualEvalGoal =
        Math.min(
        Constants.PPN_MIN_COST_LEAF,
        Constants.PPN_EPSILON * endgameTimeEstimator.disproofNumber(
            player, opponent, evalGoal - 100, this.eval));
    probGreaterEqualEvalGoal = Math.max(minProbGreaterEqualEvalGoal, Math.min(probGreaterEqualEvalGoal, maxProbGreaterEqualEvalGoal));

    probStrictlyGreaterEvalGoal = 1 - Gaussian.CDF(evalGoal+100, eval, 400);
    double maxProbStrictlyGreaterEvalGoal = 1 - 
        Math.min(
        Constants.PPN_MIN_COST_LEAF,
        Constants.PPN_EPSILON * endgameTimeEstimator.proofNumber(
            player, opponent, evalGoal + 100, this.eval));
    double minProbStrictlyGreaterEvalGoal =
        Math.min(
        Constants.PPN_MIN_COST_LEAF,
        Constants.PPN_EPSILON * endgameTimeEstimator.disproofNumber(
            player, opponent, evalGoal + 100, this.eval));

    probStrictlyGreaterEvalGoal = Math.max(minProbStrictlyGreaterEvalGoal, Math.min(probStrictlyGreaterEvalGoal, maxProbStrictlyGreaterEvalGoal));

    if (lower > evalGoal - 100) {
      proofNumberCurEval = 0;
      probGreaterEqualEvalGoal = 1;
      disproofNumberNextEval = Double.POSITIVE_INFINITY;
    } else if (upper < evalGoal - 100) {
      proofNumberCurEval = Double.POSITIVE_INFINITY;
      probGreaterEqualEvalGoal = 0;
      disproofNumberNextEval = 0;
    } else {
      proofNumberCurEval = endgameTimeEstimator.proofNumber(
          player, opponent, evalGoal - 100, this.eval) / probGreaterEqualEvalGoal;
      disproofNumberNextEval = endgameTimeEstimator.disproofNumber(
          player, opponent, evalGoal - 100, this.eval) / (1-probGreaterEqualEvalGoal);
    }
    if (upper < evalGoal + 100) {
      proofNumberNextEval = Double.POSITIVE_INFINITY;
      probStrictlyGreaterEvalGoal = 0;
      disproofNumberCurEval = 0;
    } else if (lower > evalGoal + 100) {
      proofNumberNextEval = 0;
      probStrictlyGreaterEvalGoal = 1;
      disproofNumberCurEval = Double.POSITIVE_INFINITY;
    } else {
      proofNumberNextEval = endgameTimeEstimator.proofNumber(
          player, opponent, evalGoal + 100, eval) / probStrictlyGreaterEvalGoal;
      disproofNumberCurEval = endgameTimeEstimator.disproofNumber(
          player, opponent, evalGoal + 100, eval) / (1-probStrictlyGreaterEvalGoal);
    }
    this.minLogDerivativePlayerVariates = lower > evalGoal + 100 ? Double.NEGATIVE_INFINITY :
        -Math.log(this.descendants) + (1 - Constants.LAMBDA) / 2 * Math.log(probStrictlyGreaterEvalGoal * (1 - probStrictlyGreaterEvalGoal));
    this.minLogDerivativeOpponentVariates = lower > evalGoal - 100 ? Double.NEGATIVE_INFINITY :
        -Math.log(this.descendants) +(1 - Constants.LAMBDA) / 2 * Math.log(probGreaterEqualEvalGoal * (1 - probGreaterEqualEvalGoal));;
    assert areThisBoardEvalsOK();
  }

  @Override
  public String toString() {
    String str = new Board(player, opponent).toString() + eval + "\n";
    return str;// + "\n" + this.lower + "(" + this.bestVariationPlayer + ") " + this.upper + "(" + this.bestVariationOpponent + ")\n";
  }
  
  public boolean isSolved() {
    return this.lower == this.upper;
  }
  
  public void setChildren(StoredBoard[] children) {
    this.children = children;
    this.updateFather();
    this.updateFathers();
    assert areChildrenOK();
  }
  
  public StoredBoard[] getChildren() {
    return children;
  }

  public StoredBoard bestChildMidgameOpponentVariates() {
    assert !isLeaf();
    StoredBoard best = null;
    for (StoredBoard child : children) {
      if (best == null ||
          logDerivativeOpponentVariates(child) + child.minLogDerivativePlayerVariates >
          logDerivativeOpponentVariates(best) + best.minLogDerivativePlayerVariates) {
        best = child;
      }
    }
    assert best != null;
    assert logDerivativeOpponentVariates(best) + best.minLogDerivativePlayerVariates < 0;
    return best;
  }

  public StoredBoard bestChildMidgamePlayerVariates() {
    assert !isLeaf();
    StoredBoard best = null;
    for (StoredBoard child : children) {
      if (best == null ||
          logDerivativePlayerVariates(child) + child.minLogDerivativeOpponentVariates >
          logDerivativePlayerVariates(best) + best.minLogDerivativeOpponentVariates) {
        best = child;
      }
    }
    assert best != null;
    assert logDerivativePlayerVariates(best) + best.minLogDerivativeOpponentVariates < 0;
    return best;
  }
  
  public double childValuePlayerVariates(StoredBoard child) {
    assert Utils.arrayContains(children, child);
    return child.disproofNumberNextEval / Math.min(1.E20, Math.exp(0.1 * Math.pow(disproofNumberCurEval, 0.35) / Math.sqrt(child.getDescendants())));
  }
  
  public double childValueOpponentVariates(StoredBoard child) {
    assert Utils.arrayContains(children, child);
    return child.disproofNumberCurEval / Math.exp(0.1 * Math.pow(proofNumberCurEval, 0.35) / Math.sqrt(child.getDescendants()));
  }

//  // Minimize proofNumberNextEval (= min disproofNumberNextEval of children).
//  public StoredBoard bestChildPlayerVariates() {
//    assert !isLeaf();
//    StoredBoard best = null;
//    for (StoredBoard child : children) {
//      if (child.disproofNumberNextEval == 0 || child.disproofNumberNextEval == Double.POSITIVE_INFINITY) continue;
//      if (best == null ||
//          childValuePlayerVariates(child) < childValuePlayerVariates(best)) {
//        best = child;
//      }
//    }
//    assert best != null;
//    return best;
//  }
//  
//  // Minimize proofNumberCurEval (= min disproofNumberCurEval of children).
//  public StoredBoard bestChildOpponentVariates() {
//    assert !isLeaf();
//    StoredBoard best = null;
//    for (StoredBoard child : children) {
//      if (child.disproofNumberCurEval == 0 || child.disproofNumberCurEval == Double.POSITIVE_INFINITY) continue;
//      if (best == null ||
//          childValueOpponentVariates(child) < childValueOpponentVariates(best)) {
//        best = child;
//      }
//    }
//    assert best != null;
//    return best;
//  }

  boolean areThisBoardEvalsOK() {
    if (this.lower > this.eval || this.eval > this.upper) {
      throw new AssertionError("Wrong lower:" + lower + " / eval:" + eval + " / upper:" + upper + " for board\n" + this.getBoard());
    }
    if (proofNumberCurEval > proofNumberNextEval
        || disproofNumberCurEval > disproofNumberNextEval) {
      throw new AssertionError("Wrong proof/disproof numbers:\n" + proofNumberCurEval + " " + proofNumberNextEval + "\n"
          + disproofNumberCurEval + " " + disproofNumberNextEval + " for board\n" + this.getBoard());
    }
    return
        ((proofNumberCurEval == 0) == (disproofNumberNextEval == Double.POSITIVE_INFINITY))
        && ((proofNumberCurEval == Double.POSITIVE_INFINITY) == (disproofNumberNextEval == 0))
        && ((proofNumberNextEval == 0) == (disproofNumberCurEval == Double.POSITIVE_INFINITY))
        && ((proofNumberNextEval == Double.POSITIVE_INFINITY) == (disproofNumberCurEval == 0));
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
    return 
        child.playerIsStartingPlayer == !playerIsStartingPlayer
        && eval >= -child.eval
        && lower >= -child.upper
        && upper >= -child.lower
        && evalGoal == -child.evalGoal
        && found
        && Utils.arrayContains(children, child);
  }

  boolean areChildrenOK() {
    if (children == null) {
      return true;
    }
    for (StoredBoard child : children) {
      if (!isChildOK(child)) {
        return false;
      }
    }
    if (children.length != Long.bitCount(GetMoves.getMoves(player, opponent)) && !(
        Long.bitCount(GetMoves.getMoves(player, opponent)) == 0
        && Long.bitCount(GetMoves.getMoves(opponent, player)) != 0
        && children.length == 1)) {
      return false;
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
        && (this.getProofNumberCurEval() == 0 || this.getProofNumberCurEval() == Double.POSITIVE_INFINITY)) {
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
    return isPrevNextOK() && areChildrenOK() && areThisBoardEvalsOK() && isEvalOK() && areDescendantsOK();
  }
}