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

import board.Board;
import java.util.ArrayList;

public class StoredBoard {
  public long player = 0;
  public long opponent = 0;
  // Keep lower <= eval <= upper?????
  public short eval;
  public short lower = -6400;
  public short upper = 6400;
//  public short bestVariationPlayer;
//  public short bestVariationOpponent;
  
  public boolean playerIsStartingPlayer;
  public long descendants;
  public StoredBoard next = null;
  public StoredBoard prev = null;
  public ArrayList<StoredBoard> fathers = new ArrayList<>();
  public StoredBoard[] children = null;
  public double disproofNumberCurEval = 0;
  public double proofNumberCurEval = 0;
  public double disproofNumberNextEval = 0;
  public double proofNumberNextEval = 0;

  public final static GaussianNumberGenerator RANDOM = new GaussianNumberGenerator();

  public StoredBoard(Board board) {
    this.player = board.getPlayer();
    this.opponent = board.getOpponent();
  }

  public StoredBoard(Board board, int eval, int evalGoal) {
    this.player = board.getPlayer();
    this.opponent = board.getOpponent();
    this.updateEval(eval, evalGoal);
    this.descendants = 0;
  }
  
  public boolean isLeaf() {
    return children == null;
  }
    
  public static StoredBoard randomStoredBoard(Board b) {
    return new StoredBoard(b, (int) ((Math.random() - 0.5) * 6400), 0);
  }
  
  public Board getBoard() {
    return new Board(player, opponent);
  }

  public EvaluatedBoard toEvaluatedBoard() {
    if (player == 0 && opponent == 0) {
      return null;
    }
    return new EvaluatedBoard(getLowerBound(), 1, getUpperBound(), 1);
  }

  public final void setSolved(int newEval, int evalGoal) {
    this.setLower(newEval, evalGoal);
    this.setUpper(newEval, evalGoal);
  }

  public final void setLower(int newLower, int evalGoal) {
    this.lower = (short) newLower;
    this.eval = (short) Math.max(newLower, eval);
//    for (int i = 0; i < Constants.N_SAMPLES; i++) {
//      samples[i] = (short) Math.max(newLower, samples[i]);
//    }
    if (evalGoal <= 6400 && evalGoal >= -6400) {
      this.setEvalGoalForLeaf(evalGoal);
    }
  }

  public final void setUpper(int newUpper, int evalGoal) {
    this.upper = (short) newUpper;
    this.eval = (short) Math.min(newUpper, eval);
    if (evalGoal <= 6400 && evalGoal >= -6400) {
      this.setEvalGoalForLeaf(evalGoal);
    }
  }
  
  public final void setEvalGoalForLeaf(int evalGoal) {
    assert(this.isLeaf());
    assert(evalGoal <= 6400 && evalGoal >= -6400);
    if (lower > evalGoal - 100) {
      proofNumberCurEval = 0;
      disproofNumberNextEval = Double.POSITIVE_INFINITY;
    } else if (upper < evalGoal - 100) {
      proofNumberCurEval = Double.POSITIVE_INFINITY;
      this.disproofNumberNextEval = 0;
    } else {
      proofNumberCurEval = EndgameTimeEstimator.proofNumber(
          this.getBoard(), evalGoal - 100, this.eval);   
      disproofNumberNextEval = EndgameTimeEstimator.disproofNumber(
          this.getBoard(), evalGoal - 100, this.eval);
    }
    if (upper < evalGoal + 100) {
      proofNumberNextEval = Double.POSITIVE_INFINITY;
      disproofNumberCurEval = 0;
    } else if (lower > evalGoal + 100) {
      proofNumberNextEval = 0;
      disproofNumberCurEval = Double.POSITIVE_INFINITY;    
    } else {
      proofNumberNextEval = EndgameTimeEstimator.proofNumber(
          this.getBoard(), evalGoal + 100, eval);
      disproofNumberCurEval = EndgameTimeEstimator.disproofNumber(
          this.getBoard(), evalGoal + 100, eval);
//      System.out.println(proofNumberNextEval + " " + evalGoal + " " + eval);
    }
    assert(proofNumberCurEval <= proofNumberNextEval);
    assert(disproofNumberCurEval <= disproofNumberNextEval);

    assert((proofNumberCurEval == 0) == (disproofNumberNextEval == Double.POSITIVE_INFINITY));
    assert((proofNumberCurEval == Double.POSITIVE_INFINITY) == (disproofNumberNextEval == 0));
    assert((proofNumberNextEval == 0) == (disproofNumberCurEval == Double.POSITIVE_INFINITY));
    assert((proofNumberNextEval == Double.POSITIVE_INFINITY) == (disproofNumberCurEval == 0));
  }
  
  public final void updateEval(int newEval, int evalGoal) {
    this.eval = (short) Math.max(this.lower, Math.min(this.upper, newEval));
    this.setEvalGoalForLeaf(evalGoal);
  }

  @Override
  public String toString() {
    String str = new Board(player, opponent).toString() + eval + "\n";
    return str;// + "\n" + this.lower + "(" + this.bestVariationPlayer + ") " + this.upper + "(" + this.bestVariationOpponent + ")\n";
  }
  
  public boolean isSolved() {
    return (this.lower == this.upper);
  }

  boolean isPartiallySolved() {    
    return getLowerBound() == getUpperBound();
  }

  public int getUpperBound() {
    int result = eval;
    return result;
  }

  public int getLowerBound() {
    int result = eval;
    return result;
  }

  public int getEval() {
    return eval;
  }
  
  protected StoredBoard bestChild() {
    // Idea: maximize lower bound of "acceptables".
    StoredBoard bestChild = null;
    
    for (StoredBoard child : children) {
      if (bestChild == null ||
          child.disproofNumberCurEval / Math.exp(Math.sqrt(4 * Math.log(descendants) / child.descendants)) < 
          bestChild.disproofNumberCurEval / Math.exp(Math.sqrt(4 * Math.log(descendants) / bestChild.descendants))) {
        bestChild = child;
      }
    }
    return bestChild;
  }

  protected synchronized void updateFathers() {
    for (StoredBoard father : fathers) {
      father.updateFather();
      father.updateFathers();
    }
  }
  
  protected synchronized void updateFather() {
    StoredBoard bestChild = bestChild();
    eval = Short.MIN_VALUE;
    lower = Short.MIN_VALUE;
    upper = Short.MIN_VALUE;
//    bestVariationPlayer = Short.MIN_VALUE;
//    bestVariationOpponent = Short.MIN_VALUE;

//    bestVariationOpponent = (short) -bestChild.bestVariationPlayer;
    proofNumberCurEval = Double.POSITIVE_INFINITY;
    proofNumberNextEval = Double.POSITIVE_INFINITY;
    disproofNumberCurEval = 0;
    disproofNumberNextEval = 0;

    for (StoredBoard child : children) {
      eval = (short) Math.max(eval, -child.eval);
//      bestVariationPlayer = (short) Math.max(bestVariationPlayer,
//          -child.bestVariationOpponent);
      lower = (short) Math.max(lower, -child.upper);
      upper = (short) Math.max(upper, -child.lower);
      proofNumberCurEval = Math.min(proofNumberCurEval, child.disproofNumberCurEval);
      proofNumberNextEval = Math.min(proofNumberNextEval, child.disproofNumberNextEval);
      disproofNumberCurEval += child.proofNumberCurEval;
      disproofNumberNextEval += child.proofNumberNextEval;
    }
    assert(proofNumberCurEval <= proofNumberNextEval);
    assert(disproofNumberCurEval <= disproofNumberNextEval);

    assert((proofNumberCurEval == 0) == (disproofNumberNextEval == Double.POSITIVE_INFINITY));
    assert((proofNumberCurEval == Double.POSITIVE_INFINITY) == (disproofNumberNextEval == 0));
    assert((proofNumberNextEval == 0) == (disproofNumberCurEval == Double.POSITIVE_INFINITY));
    assert((proofNumberNextEval == Double.POSITIVE_INFINITY) == (disproofNumberCurEval == 0));
  }
}