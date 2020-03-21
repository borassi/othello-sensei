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
import helpers.Gaussian;
import java.util.ArrayList;
import java.util.Arrays;

public class StoredBoard {
  public final static double C_ATTACK = 0.01;
  public final static double C_DEFENSE = 0.1;
  public final static int N_SAMPLES = 20;
  public long player = 0;
  public long opponent = 0;
  // Keep lower <= eval <= upper?????
  public short eval;
  public double safeEval;
  public short ciLower = -6400;
  public short ciUpper = 6400;
  public short lower = -6400;
  public short upper = 6400;
  public short bestVariationPlayer;
  public short bestVariationOpponent;
  
  public boolean playerIsStartingPlayer;
//  public short[] samples = new short[N_SAMPLES];
  public long descendants;
  public StoredBoard next = null;
  public StoredBoard prev = null;
  public ArrayList<StoredBoard> fathers = new ArrayList<>();
  public StoredBoard[] children = null;
  public double costUntilLeafAttack = 0;
  public double costUntilLeafDefense = 0;
  public double costUntilLeafAttackBestPath = 0;
  public double costUntilLeafDefenseBestPath = 0;
  static int proofNumberSize = 64;
  public float[] proofNumbers = new float[proofNumberSize + 1];
  public float[] disproofNumbers = new float[proofNumberSize + 1];
  public long expectedToSolve = 0;

  public final static GaussianNumberGenerator RANDOM = new GaussianNumberGenerator();
  
  public StoredBoard(Board board) {
    this.player = board.getPlayer();
    this.opponent = board.getOpponent();
  }

  public StoredBoard(Board board, int eval, float variance, long descendants) {
    this.descendants = descendants;
    this.player = board.getPlayer();
    this.opponent = board.getOpponent();
    this.setEval(eval, variance);
  }
  
  public void addChildren(StoredBoard[] children) {
    this.children = children;
  }
//  public StoredBoard(Board board, int eval, float variance, int deltas[]) {
//    this.player = board.getPlayer();
//    this.opponent = board.getOpponent();
//    this.updateEval(eval, variance, deltas);
//    this.descendants = 0;
//  }
  
  public boolean isLeaf() {
    return children == null;
  }
    
  public static StoredBoard randomStoredBoard(Board b) {
    return new StoredBoard(b, (int) ((Math.random() - 0.5) * 6400), 600, 1);
  }
  
  public Board getBoard() {
    return new Board(player, opponent);
  }

//  public EvaluatedBoard toEvaluatedBoard() {
//    if (player == 0 && opponent == 0) {
//      return null;
//    }
//    return new EvaluatedBoard(getLowerBound(), 1, getUpperBound(), 1);
//  }

  static int invertProofNumberOffset(int offset) {
    return 64 - offset;
  }

  static int evalToProofNumberOffset(int eval) {
    return (eval + 6400) / 200;
  }
  
  static int proofNumberOffsetToEval(int offset) {
    return offset * 200 - 6400;
  }

  public final void setSolved(int newEval, int evalGoal) {
    this.setLower(newEval, evalGoal);
    this.setUpper(newEval, evalGoal);
    this.costUntilLeafAttackBestPath = -2 * Math.log(this.descendants);
    this.costUntilLeafDefenseBestPath = this.costUntilLeafAttackBestPath;
    this.costUntilLeafAttack = Double.NEGATIVE_INFINITY;
    this.costUntilLeafDefense = Double.NEGATIVE_INFINITY;
  }

  public final void setLower(int newLower, int evalGoal) {
    assert(this.isLeaf());
    this.lower = (short) newLower;
    this.eval = (short) Math.max(newLower, eval);
    this.safeEval = this.eval;
//    for (int i = 0; i < N_SAMPLES; i++) {
//      samples[i] = (short) Math.max(newLower, samples[i]);
//    }
//    if (evalGoal <= 6400 && evalGoal >= -6400) {
      this.setEvalGoalForLeaf(0);
//    }
    updateCI();
  }

  public final void setUpper(int newUpper, int evalGoal) {
    assert(this.isLeaf());
    this.upper = (short) newUpper;
    this.eval = (short) Math.min(newUpper, eval);
    this.safeEval = this.eval;
//    for (int i = 0; i < N_SAMPLES; i++) {
//      this.samples[i] = (short) Math.min(newUpper, samples[i]);
//    }
//    if (evalGoal <= 6400 && evalGoal >= -6400) {
      this.setEvalGoalForLeaf(0);
//    }
    updateCI();
  }
  
  public final void setEval(int newEval, float variance) {
    assert(this.isLeaf());
    this.eval = (short) Math.max(this.lower, Math.min(this.upper, newEval));
    this.safeEval = this.eval;
    this.costUntilLeafAttack = -2 * Math.log(this.descendants);
    this.costUntilLeafAttackBestPath = this.costUntilLeafAttack;
    this.costUntilLeafDefense = this.costUntilLeafAttack;
    this.costUntilLeafDefenseBestPath = this.costUntilLeafDefense;
    this.ciLower = (short) getLowerCI();
    this.ciUpper = (short) getUpperCI();

    this.setEvalGoalForLeaf(0);
//    for (int i = 0; i < N_SAMPLES; i++) {
//      this.samples[i] = (short) Math.max(this.lower, Math.min(
//          this.upper,
//          (int) (newEval + RANDOM.next() * variance + 0.5)));
//    }
  }
//  
//  public final double getCost() {
//    for (StoredBoard sibling : this.fathers)
//    return Math.log(C)
//  }
  
  public static double edgeCost(StoredBoard father, StoredBoard child, boolean attack) {
    double c = attack ? C_ATTACK : C_DEFENSE;
    assert(child.fathers.contains(father));
    // Always negative -> maximize.
//    return C * (-child.safeEval - father.safeEval - 1 / C * Math.log(father.children.length));
    return c * (-child.eval - father.eval) - Math.log(father.children.length);
  }
  
  protected synchronized void updateFather(int evalGoalForBoard) {
    StoredBoard bestChild = bestChild(evalGoalForBoard);
    eval = Short.MIN_VALUE;
    lower = Short.MIN_VALUE;
    upper = Short.MIN_VALUE;
    bestVariationPlayer = Short.MIN_VALUE;
    bestVariationOpponent = Short.MIN_VALUE;
    ciLower = Short.MIN_VALUE;
    ciUpper = Short.MIN_VALUE;
    expectedToSolve = children[0].expectedToSolve;
    double sumSafeEval = 0;

    bestVariationOpponent = (short) -bestChild.bestVariationPlayer;

    for (StoredBoard child : children) {
      sumSafeEval += Math.exp(C_ATTACK * (-child.safeEval));
      if (eval < -child.eval) {
        eval = (short) -child.eval;
        this.costUntilLeafAttackBestPath = edgeCost(this, child, true) + child.costUntilLeafDefenseBestPath;
        this.costUntilLeafDefenseBestPath = edgeCost(this, child, false) + child.costUntilLeafAttackBestPath;
      }
//      eval = (short) Math.max(eval, -child.eval);
      bestVariationPlayer = (short) Math.max(bestVariationPlayer,
          -child.bestVariationOpponent);
      lower = (short) Math.max(lower, -child.upper);
      upper = (short) Math.max(upper, -child.lower);
      ciLower = (short) Math.max(ciLower, -child.ciUpper);
      ciUpper = (short) Math.max(ciUpper, -child.ciLower);
      if (eval >= evalGoalForBoard) {
        expectedToSolve = Math.min(expectedToSolve, child.expectedToSolve);
      } else {
        expectedToSolve += child.expectedToSolve;        
      }
    }
    this.costUntilLeafAttack = Double.NEGATIVE_INFINITY;
    this.costUntilLeafDefense = Double.NEGATIVE_INFINITY;
    for (StoredBoard child : children) {
      this.costUntilLeafAttack = Math.max(this.costUntilLeafAttack, edgeCost(this, child, true) + child.costUntilLeafDefense);
      this.costUntilLeafDefense = Math.max(this.costUntilLeafDefense, edgeCost(this, child, false) + child.costUntilLeafAttack);
    }
    
    this.safeEval = 1 / C_ATTACK * Math.log(sumSafeEval / children.length);
    for (int i = 0; i < this.proofNumbers.length; ++i) {
      int j = invertProofNumberOffset(i);
      this.proofNumbers[i] = Float.POSITIVE_INFINITY;
      this.disproofNumbers[i] = 0;
      for (StoredBoard child : children) {
        this.proofNumbers[i] = Math.min(this.proofNumbers[i], child.disproofNumbers[j]);
        this.disproofNumbers[i] += child.proofNumbers[j];
      }
    }
//    for (int i = 0; i < StoredBoard.N_SAMPLES; ++i) {
//      short minChild = Short.MIN_VALUE;
//      for (StoredBoard child : children) {
//        minChild = (short) Math.max(minChild, -child.samples[i]);
//      }
//      samples[i] = minChild;
//    }
    ciLower = (short) Math.max(ciLower, this.getLowerCI());
    ciUpper = (short) Math.min(ciUpper, this.getUpperCI());
  }
  
  public String getInterestingProofNumbers() {
    int start = Math.max(-6400, Math.min(5600, (int) Math.round(this.eval / 200.0 - 2) * 200));
    String result = "";
    for (int e = start + 800; e >= start; e -= 200) {
      int offset = evalToProofNumberOffset(e);
      result += String.format("%d %.2g %.2g\n", -e / 100, disproofNumbers[offset], proofNumbers[offset]);
    }
    return result;
  }
  
  private static float getProofNumber(int empties, int eval, int proofNumbersEval) {
    double tmp = Math.exp(
        -0.0694
        +0.7058 * empties
        -0.000518 * Math.abs(eval - proofNumbersEval)
        -0.4783 * (eval > proofNumbersEval ? 1 : 0)) /
        Math.max(0.0001, Gaussian.CDF(proofNumbersEval, eval, 600));
//    System.out.println(empties);
    return((float) tmp);
  }
  
  public final void setEvalGoalForLeaf(int evalGoal) {
    assert(this.isLeaf());
    assert(evalGoal <= 6400 && evalGoal >= -6400);
    this.bestVariationOpponent = lower >= evalGoal ? 6600 : eval;
    this.bestVariationPlayer = upper <= evalGoal ? -6600 : eval;
    int empties = 64 - Long.bitCount(player | opponent);
    for (int i = 0; i < proofNumbers.length; ++i) {
      int proofNumbersEval = proofNumberOffsetToEval(i);
      if (proofNumbersEval <= lower) {
        proofNumbers[i] = 0;
      } else if (proofNumbersEval > upper) {
        proofNumbers[i] = Float.POSITIVE_INFINITY;
      } else {
        proofNumbers[i] = getProofNumber(empties, this.eval, proofNumbersEval);
      }
      if (proofNumbersEval < lower) {
        disproofNumbers[i] = Float.POSITIVE_INFINITY;
      } else if (proofNumbersEval >= upper) {
        disproofNumbers[i] = 0;
      } else {
        disproofNumbers[i] = getProofNumber(empties, -this.eval, -proofNumbersEval);
      }
    }
  }
  
//  public final void updateEval(int newEval, float variance, int deltas[]) {
//    this.eval = (short) Math.max(this.lower, Math.min(this.upper, newEval));
//    this.ciLower = (short) Math.max(this.lower, eval - getError(descendants));
//    this.ciUpper = (short) Math.max(this.lower, eval + getError(descendants));
////    double weightNew = 0.8;
////    double weightOld = Math.sqrt(1 - weightNew * weightNew);
////    for (int i = 0; i < N_SAMPLES; i++) {
////      this.samples[i] = (short) Math.round(Math.max(this.lower, Math.min(
////          this.upper,
////          newEval + RANDOM.next() * variance * weightNew - deltas[i] * weightOld)));
////    }
//    this.bestVariationOpponent = eval;
//    this.bestVariationPlayer = eval;
//  }

  @Override
  public String toString() {
    String str = new Board(player, opponent).toString() + eval + "\n";
//    for (int i = 0; i < N_SAMPLES; i++) {
//      str += samples[i] + " ";
//    }
    return str + "\n" + this.lower + "(" + this.bestVariationPlayer + ") " + this.upper + "(" + this.bestVariationOpponent + ")\n";
  }
  
  public int getEvalGoal() {
    float bestValue = Float.POSITIVE_INFINITY;
    int best = -1;
    for (int i = 0; i < this.proofNumbers.length; ++i) {
//      if (proofNumbers[i] == 0) {
//        continue;
//      }
      float curValue = proofNumbers[i] + disproofNumbers[i];
      if (curValue < bestValue) {
        best = i;
        bestValue = curValue;
      }
    }
//    System.out.println(StoredBoard.proofNumberOffsetToEval(best) + " " + proofNumbers[best] + " " + disproofNumbers[best]);
//    System.out.println((proofNumbers[best] > disproofNumbers[best] ? -100 : 100));
    return StoredBoard.proofNumberOffsetToEval(best) + (proofNumbers[best] > disproofNumbers[best] ? -100 : 100);
  }
  
  public float getProofNumber(int evalGoal) {
    return this.proofNumbers[evalToProofNumberOffset(evalGoal)];
  }
  
  public float getDisproofNumber(int evalGoal) {
    return this.disproofNumbers[evalToProofNumberOffset(evalGoal)];
  }
  
  public boolean isSolved() {
    return (this.lower == this.upper);
  }

  boolean isPartiallySolved() {
    if (isSolved()) {
      return true;
    }
//    double expectedCost = ((-this.getBoard().getEmptySquares() + 10) * 2.1 - 600 * C_ATTACK);
    return this.costUntilLeafAttack < this.costUntilLeafAttackBestPath - 400 * C_ATTACK &&
        this.costUntilLeafDefense < this.costUntilLeafDefenseBestPath - 400 * C_ATTACK;
//    return getLowerBound() == getUpperBound();
  }

  public static final double getErrorBase(long descendants) {
    assert(descendants > 0);
    return 1 / (1.0 + Math.sqrt(descendants)); // TODO: WTF????
  }
  
  public static final int getError(long descendants) {
    return (int) (800 * getErrorBase(descendants) / getErrorBase(1) + 100);
  }
  
  private void updateCI() {
    this.ciLower = (short) getLowerCI();
    this.ciUpper = (short) getUpperCI();
  }
  
  private int getLowerCI() {
    return Math.max(lower, eval - getError(descendants));
  }
  
  private int getUpperCI() {
    return Math.min(upper, eval + getError(descendants));
  }

//  public int getUpperBound() {
//    return ciUpper;
////    int result = eval;
//////    for (short s : samples) {
//////      result = Math.max(result, s);
//////    }
////    return result;
//  }
//  
//  public int getLowerBound() {
//    return ciLower;
////    int result = eval;
//////    for (short s : samples) {
//////      result = Math.min(result, s);
//////    }
////    return result;
//  }

  public int getEval() {
    return eval;
  }

  public int getSafeEval() {
    return (int) safeEval;
  }
  
  protected StoredBoard bestChild(int evalGoalForBoard) {
    // Idea: maximize lower bound of "acceptables".
    StoredBoard bestChild = null;
    int bestEval = Integer.MIN_VALUE;
    
    for (StoredBoard child : children) {
      int effectiveEval = Math.min(-child.eval, evalGoalForBoard);
      if (bestChild == null || (effectiveEval > bestEval) ||
          (effectiveEval == bestEval && -child.bestVariationPlayer > -bestChild.bestVariationPlayer)) {
        bestChild = child;
        bestEval = effectiveEval;
      }
    }
    return bestChild;
  }
}
