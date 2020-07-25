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
import helpers.Utils;
import java.util.ArrayList;
import java.util.HashSet;

public class StoredBoard {
  private final long player;
  private final long opponent;

  final ArrayList<StoredBoard> fathers;
  StoredBoard[] children;
  
  int eval;
  int lower;
  int upper;
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

  private StoredBoard(long player, long opponent, int eval, int evalGoal, boolean playerIsStartingPlayer, long descendants) {
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
  
  public static StoredBoard initialStoredBoard(long player, long opponent, int eval, int evalGoal, long descendants) {
    return new StoredBoard(player, opponent, eval, evalGoal, true, descendants);
  }

  public static StoredBoard childStoredBoard(Board b, StoredBoard father, int eval, long descendants) {
    return childStoredBoard(b.getPlayer(), b.getOpponent(), father, eval, descendants);
  }
  
  public static StoredBoard childStoredBoard(long player, long opponent, StoredBoard father, int eval, long descendants) {
    StoredBoard result = new StoredBoard(player, opponent, eval, -father.evalGoal, !father.playerIsStartingPlayer, descendants);
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
    return descendants >= 0.05 * (proofNumberCurEval + disproofNumberCurEval);
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
    assert(isLeaf());
    this.lower = (short) Math.max(lower, newLower);
    this.eval = (short) Math.max(newLower, eval);
    this.setProofNumbersForLeaf();
    this.updateFathers();
    assert areThisBoardEvalsOK();
  }

  public final void setUpper(int newUpper) {
    assert(isLeaf());
    this.upper = (short) Math.min(upper, newUpper);
    this.eval = (short) Math.min(newUpper, eval);
    this.setProofNumbersForLeaf();
    this.updateFathers();
    assert areThisBoardEvalsOK();
  }
  
  public final void setEval(int newEval) {
    assert(isLeaf());
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
    proofNumberNextEval = Double.POSITIVE_INFINITY;
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
    }

    minLogDerivativePlayerVariates = Double.MAX_VALUE;
    minLogDerivativeOpponentVariates = Double.MAX_VALUE;
    for (StoredBoard child : children) {
      minLogDerivativePlayerVariates = Math.min(
          minLogDerivativePlayerVariates,
          child.minLogDerivativeOpponentVariates + logDerivativePlayerVariates(child));
      minLogDerivativeOpponentVariates = Math.min(
          minLogDerivativeOpponentVariates,
          child.minLogDerivativePlayerVariates + logDerivativeOpponentVariates(child));
    }
    assert areThisBoardEvalsOK();
    assert isEvalOK();
  }
  
  public double logDerivativePlayerVariates(StoredBoard child) {
    assert Utils.arrayContains(children, child);
    return Math.log(children.length) + Constants.C_PLAYER_VARIATES * (eval + child.eval);
  }
  
  public double logDerivativeOpponentVariates(StoredBoard child) {
    assert Utils.arrayContains(children, child);
    return Math.log(children.length) + Constants.C_OPPONENT_VARIATES * (eval + child.eval);
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
    this.minLogDerivativePlayerVariates = lower == upper ? Double.POSITIVE_INFINITY : Math.log(this.descendants);
    this.minLogDerivativeOpponentVariates = lower == upper ? Double.POSITIVE_INFINITY : Math.log(this.descendants);
    if (lower > evalGoal - 100) {
      proofNumberCurEval = 0;
      disproofNumberNextEval = Double.POSITIVE_INFINITY;
    } else if (upper < evalGoal - 100) {
      proofNumberCurEval = Double.POSITIVE_INFINITY;
      disproofNumberNextEval = 0;
    } else {
      proofNumberCurEval = EndgameTimeEstimator.proofNumber(
          player, opponent, evalGoal - 100, this.eval);
      disproofNumberNextEval = EndgameTimeEstimator.disproofNumber(
          player, opponent, evalGoal - 100, this.eval);
    }
    if (upper < evalGoal + 100) {
      proofNumberNextEval = Double.POSITIVE_INFINITY;
      disproofNumberCurEval = 0;
    } else if (lower > evalGoal + 100) {
      proofNumberNextEval = 0;
      disproofNumberCurEval = Double.POSITIVE_INFINITY;
    } else {
      proofNumberNextEval = EndgameTimeEstimator.proofNumber(
          player, opponent, evalGoal + 100, eval);
      disproofNumberCurEval = EndgameTimeEstimator.disproofNumber(
          player, opponent, evalGoal + 100, eval);
    }
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
          logDerivativeOpponentVariates(child) + child.minLogDerivativePlayerVariates <
          logDerivativeOpponentVariates(best) + best.minLogDerivativePlayerVariates) {
        best = child;
      }
    }
    assert best != null;
    return best;
  }

  public StoredBoard bestChildMidgamePlayerVariates() {
    assert !isLeaf();
    StoredBoard best = null;
    for (StoredBoard child : children) {
      if (best == null ||
          logDerivativePlayerVariates(child) + child.minLogDerivativeOpponentVariates <
          logDerivativePlayerVariates(best) + best.minLogDerivativeOpponentVariates) {
        best = child;
      }
    }
    assert best != null;
    return best;
  }
  
  public double childValuePlayerVariates(StoredBoard child) {
    assert Utils.arrayContains(children, child);
    return child.disproofNumberNextEval / Math.min(1.E20, Math.exp(0.1 * Math.pow(disproofNumberCurEval, 0.35) / Math.sqrt(child.descendants)));
  }
  
  public double childValueOpponentVariates(StoredBoard child) {
    assert Utils.arrayContains(children, child);
    return child.disproofNumberCurEval / Math.exp(0.1 * Math.pow(proofNumberCurEval, 0.35) / Math.sqrt(child.descendants));
  }

  // Minimize proofNumberNextEval (= min disproofNumberNextEval of children).
  // No error because we are trying to 
  public StoredBoard bestChildPlayerVariates() {
    assert !isLeaf();
    StoredBoard best = null;
    for (StoredBoard child : children) {
      if (best == null ||
          childValuePlayerVariates(child) < childValuePlayerVariates(best)) {
        best = child;
      }
    }
    assert best != null;
    return best;
  }
  
  // Minimize proofNumberCurEval (= min disproofNumberCurEval of children).
  public StoredBoard bestChildOpponentVariates() {
    assert !isLeaf();
    StoredBoard best = null;
    for (StoredBoard child : children) {
      if (best == null ||
          childValueOpponentVariates(child) < childValueOpponentVariates(best)) {
        best = child;
      }
    }
    assert best != null;
    return best;
  }

  boolean areThisBoardEvalsOK() {
    return
        this.lower <= this.eval && this.eval <= this.upper 
        && proofNumberCurEval <= proofNumberNextEval
        && disproofNumberCurEval <= disproofNumberNextEval
        && ((proofNumberCurEval == 0) == (disproofNumberNextEval == Double.POSITIVE_INFINITY))
        && ((proofNumberCurEval == Double.POSITIVE_INFINITY) == (disproofNumberNextEval == 0))
        && ((proofNumberNextEval == 0) == (disproofNumberCurEval == Double.POSITIVE_INFINITY))
        && ((proofNumberNextEval == Double.POSITIVE_INFINITY) == (disproofNumberCurEval == 0));
  }

  boolean isChildOK(StoredBoard child) {
    boolean found = false;
    for (StoredBoard father : child.fathers) {
      if (father == this) {
        if (found) {
          return false;
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
      return eval == correctEval && lower == correctEval && upper == correctEval;
    }
    if (children == null) {
      return true;
    }
    int correctEval = Integer.MIN_VALUE;
    int correctLower = Integer.MIN_VALUE;
    int correctUpper = Integer.MIN_VALUE;
//    int minDescendants = 1;
    for (StoredBoard child : children) {
      correctEval = Math.max(correctEval, -child.getEval());
      correctLower = Math.max(correctLower, -child.getUpper());
      correctUpper = Math.max(correctUpper, -child.getLower());
//      minDescendants += child.descendants;
    }
//    System.out.println(eval + " " + correctEval + " " + lower + " " + correctLower + " " + descendants + " " + minDescendants);
    return eval == correctEval && lower == correctLower && upper == correctUpper; // && descendants >= minDescendants;
  }
  
  boolean isAllOK() {
    return isPrevNextOK() && areChildrenOK() && areThisBoardEvalsOK() && isEvalOK();
  }
}