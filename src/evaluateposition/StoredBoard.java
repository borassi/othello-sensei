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
  private int nEmpties;
  public double probGreaterEqual;
  public double probStrictlyGreater;
  // Positions to prove that eval >= evalGoal.
  double proofNumberGreaterEqual;
  // Positions to prove that eval <= evalGoal.
  double disproofNumberStrictlyGreater;
  public double minLogDerivativeStrictlyGreater;
  public double minLogDerivativeGreaterEqual;
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
    this.nEmpties = 64 - Long.bitCount(player | opponent);
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
    return proofNumberGreaterEqual;
  }
  
  public double getDisproofNumberCurEval() {
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
    proofNumberGreaterEqual = 0;
    probGreaterEqual = 1 - children.length;
    probStrictlyGreater = 1 - children.length;
    disproofNumberStrictlyGreater = 0;
    double minProbGreaterEqual = 1;
    double minProbStrictlyGreater = 1;

    for (StoredBoard child : children) {
      eval = Math.max(eval, -child.eval);
      lower = Math.max(lower, -child.upper);
      upper = Math.max(upper, -child.lower);
      proofNumberGreaterEqual += (1-child.probStrictlyGreater) * Math.pow(child.disproofNumberStrictlyGreater, Constants.PPN_MU);
      disproofNumberStrictlyGreater += child.proofNumberGreaterEqual;
      probGreaterEqual += Math.pow(child.probStrictlyGreater, Constants.PPN_LAMBDA);
      probStrictlyGreater += Math.pow(child.probGreaterEqual, Constants.PPN_LAMBDA);
      minProbGreaterEqual = Math.min(child.probGreaterEqual, minProbGreaterEqual);
      minProbStrictlyGreater = Math.min(child.probStrictlyGreater, minProbStrictlyGreater);
    }
//    System.out.println(minProbStrictlyGreaterEvalGoal + " " + Math.pow(probGreaterEqualEvalGoal, 1 / Constants.LAMBDA));
    probGreaterEqual = 1 - Math.min(minProbStrictlyGreater, Math.pow(probGreaterEqual, 1 / Constants.PPN_LAMBDA));
    probStrictlyGreater = 1 - Math.min(minProbGreaterEqual, Math.pow(probStrictlyGreater, 1 / Constants.PPN_LAMBDA));
    proofNumberGreaterEqual = Math.pow(proofNumberGreaterEqual / probGreaterEqual, 1 / Constants.PPN_MU);

    minLogDerivativeStrictlyGreater = Double.NEGATIVE_INFINITY;
    minLogDerivativeGreaterEqual = Double.NEGATIVE_INFINITY;

    for (StoredBoard child : children) {
      minLogDerivativeStrictlyGreater = Math.max(minLogDerivativeStrictlyGreater,
          child.minLogDerivativeGreaterEqual + logProbDerivativeGreaterEqual(child));
      minLogDerivativeGreaterEqual = Math.max(minLogDerivativeGreaterEqual,
          child.minLogDerivativeStrictlyGreater + logProbDerivativeStrictlyGreater(child));
    }
    assert areThisBoardEvalsOK();
    assert isEvalOK();
    assert areDescendantsOK();
  }
  
  public double logProbDerivativeGreaterEqual(StoredBoard child) {
    assert Utils.arrayContains(children, child);
    if (child.probGreaterEqual == 0) {
      return Double.NEGATIVE_INFINITY;
    }
    return Math.min(0, Math.log((1 - probStrictlyGreater) / child.probGreaterEqual));
  }
  
  public double logProbDerivativeStrictlyGreater(StoredBoard child) {
    assert Utils.arrayContains(children, child);
    if (child.probStrictlyGreater == 0) {
      return Double.NEGATIVE_INFINITY;
    }
    return Math.min(0, Math.log((1 - probGreaterEqual) / child.probStrictlyGreater));
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
    probGreaterEqual = Math.max(1.E-8, Math.min(1 - 1.E-8, 1 - Gaussian.CDF(evalGoal-100, eval, 400 - nEmpties * 4)));
    probStrictlyGreater = Math.max(1.E-8, Math.min(1 - 1.E-8, 1 - Gaussian.CDF(evalGoal+100, eval, 400 - nEmpties * 4)));
    double disproofNumberGreaterEqual = endgameTimeEstimator.disproofNumber(player, opponent, evalGoal - 100, this.eval);
    double proofNumberStrictlyGreater = endgameTimeEstimator.proofNumber(player, opponent, evalGoal + 100, this.eval);

    boolean toBeSolved = this.toBeSolved();
    double n = 750;
    double mult = -1 / Constants.PPN_LAMBDA;

    if (lower > evalGoal - 100) {
      this.proofNumberGreaterEqual = 0;
      probGreaterEqual = 1;
      minLogDerivativeGreaterEqual = Double.NEGATIVE_INFINITY;
    } else if (upper < evalGoal - 100) {
      this.proofNumberGreaterEqual = Double.POSITIVE_INFINITY;
      probGreaterEqual = 0;
      minLogDerivativeGreaterEqual = Double.NEGATIVE_INFINITY;
    } else {
      this.proofNumberGreaterEqual = endgameTimeEstimator.proofNumber(player, opponent, evalGoal - 100, this.eval);
        
      if (toBeSolved) {
//        if (probGreaterEqual < Constants.PPN_MIN_COST_LEAF || probGreaterEqual > 1 - Constants.PPN_MIN_COST_LEAF) {
//          this.minLogDerivativeGreaterEqual = 0;
//        } else {
          this.minLogDerivativeGreaterEqual = mult * Math.log(probGreaterEqual * (1 - probGreaterEqual) / proofNumberGreaterEqual +
              probGreaterEqual * (1 - probGreaterEqual) / disproofNumberGreaterEqual);
//        }
      } else {
//        if (probGreaterEqual < Constants.PPN_MIN_COST_LEAF) {
//          this.minLogDerivativeGreaterEqual = 0;
//        } else if(probGreaterEqual > 1 - Constants.PPN_MIN_COST_LEAF) {
//          this.minLogDerivativeGreaterEqual = 0;
//        } else {
          this.minLogDerivativeGreaterEqual = mult * Math.log(probGreaterEqual * (1 - probGreaterEqual) / n);
//        }
      }
    }
    if (upper < evalGoal + 100) {
      probStrictlyGreater = 0;
      this.disproofNumberStrictlyGreater = 0;
      this.minLogDerivativeStrictlyGreater = Double.NEGATIVE_INFINITY;
    } else if (lower > evalGoal + 100) {
      probStrictlyGreater = 1;
      this.disproofNumberStrictlyGreater = Double.POSITIVE_INFINITY;
      this.minLogDerivativeStrictlyGreater = Double.NEGATIVE_INFINITY;
    } else {
      this.disproofNumberStrictlyGreater = endgameTimeEstimator.disproofNumber(player, opponent, evalGoal + 100, this.eval);
      
      if (toBeSolved) {
//        if (probStrictlyGreater < Constants.PPN_MIN_COST_LEAF || probStrictlyGreater > 1 - Constants.PPN_MIN_COST_LEAF) {
//          this.minLogDerivativeStrictlyGreater = 0;
//        } else {
        this.minLogDerivativeStrictlyGreater = mult * Math.log(probStrictlyGreater * (1 - probStrictlyGreater) / proofNumberStrictlyGreater +
            probStrictlyGreater * (1 - probStrictlyGreater) / disproofNumberStrictlyGreater);
//        }
      } else {
//        if (probStrictlyGreater < Constants.PPN_MIN_COST_LEAF) {
//          this.minLogDerivativeStrictlyGreater = 0;
//        } else if(probStrictlyGreater > 1 - Constants.PPN_MIN_COST_LEAF) {
//          this.minLogDerivativeStrictlyGreater = 0;
//        } else {
        this.minLogDerivativeStrictlyGreater = mult * Math.log(probStrictlyGreater * (1 - probStrictlyGreater) / n);
//        }
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
        (getProofNumberCurEval() < Constants.PROOF_NUMBER_FOR_ENDGAME && getEval() > getEvalGoal() + 2000) ||
        (getDisproofNumberCurEval() < Constants.PROOF_NUMBER_FOR_ENDGAME && getEval() < getEvalGoal() - 2000);
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
  
  public void setChildren(StoredBoard[] children) {
    this.children = children;
    this.updateFather();
    this.updateFathers();
    assert areChildrenOK();
  }
  
  public StoredBoard[] getChildren() {
    return children;
  }

  public StoredBoard bestChildMidgameStrictlyGreater() {
    assert !isLeaf();
    StoredBoard best = null;
    for (StoredBoard child : children) {
      if (best == null ||
          logProbDerivativeStrictlyGreater(child) + child.minLogDerivativeStrictlyGreater >
          logProbDerivativeStrictlyGreater(best) + best.minLogDerivativeStrictlyGreater) {
        best = child;
      }
    }
    assert best != null;
    assert logProbDerivativeStrictlyGreater(best) + best.minLogDerivativeStrictlyGreater < 0;
    return best;
  }

  public StoredBoard bestChildMidgameGreaterEqual() {
    assert !isLeaf();
    StoredBoard best = null;
    for (StoredBoard child : children) {
      if (best == null ||
          logProbDerivativeGreaterEqual(child) + child.minLogDerivativeGreaterEqual >
          logProbDerivativeGreaterEqual(best) + best.minLogDerivativeGreaterEqual) {
        best = child;
      }
    }
    assert best != null;
    assert logProbDerivativeGreaterEqual(best) + best.minLogDerivativeGreaterEqual < 0;
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