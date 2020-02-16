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
  public final static int N_SAMPLES = 20;
  public long player = 0;
  public long opponent = 0;
  // Keep lower <= eval <= upper?????
  public short eval;
  public short ciLower = -6400;
  public short ciUpper = 6400;
  public short lower = -6400;
  public short upper = 6400;
  public short bestVariationPlayer;
  public short bestVariationOpponent;
  
  public boolean playerIsStartingPlayer;
  public short[] samples = new short[N_SAMPLES];
  public long descendants;
  public StoredBoard next = null;
  public StoredBoard prev = null;
  public ArrayList<StoredBoard> fathers = new ArrayList<>();
  public StoredBoard[] children = null;
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
    this.updateEval(eval, variance);
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
    for (int i = 0; i < N_SAMPLES; i++) {
      samples[i] = (short) Math.max(newLower, samples[i]);
    }
    if (evalGoal <= 6400 && evalGoal >= -6400) {
      this.setEvalGoalForLeaf(evalGoal);
    }
    updateCI();
  }

  public final void setUpper(int newUpper, int evalGoal) {
    this.upper = (short) newUpper;
    this.eval = (short) Math.min(newUpper, eval);
    for (int i = 0; i < N_SAMPLES; i++) {
      this.samples[i] = (short) Math.min(newUpper, samples[i]);
    }
    if (evalGoal <= 6400 && evalGoal >= -6400) {
      this.setEvalGoalForLeaf(evalGoal);
    }
    updateCI();
  }
  
  public final void setEvalGoalForLeaf(int evalGoal) {
    assert(this.isLeaf());
    assert(evalGoal <= 6400 && evalGoal >= -6400);
    this.bestVariationOpponent = lower >= evalGoal ? 6600 : eval;
    this.bestVariationPlayer = upper <= evalGoal ? -6600 : eval;
    // TODO: improve
    this.expectedToSolve = (long) (
        Math.pow(2.2, 64 - Long.bitCount(player | opponent)));
  }
  
  public final void updateEval(int newEval, float variance) {
    this.eval = (short) Math.max(this.lower, Math.min(this.upper, newEval));
//    System.out.println(descendants);
//    System.out.println(getError(descendants));
    this.ciLower = (short) getLowerCI();
    this.ciUpper = (short) getUpperCI();

    for (int i = 0; i < N_SAMPLES; i++) {
      this.samples[i] = (short) Math.max(this.lower, Math.min(
          this.upper,
          (int) (newEval + RANDOM.next() * variance + 0.5)));
    }
  }
  
//  public final void updateEval(int newEval, float variance, int deltas[]) {
//    this.eval = (short) Math.max(this.lower, Math.min(this.upper, newEval));
//    this.ciLower = (short) Math.max(this.lower, eval - getError(descendants));
//    this.ciUpper = (short) Math.max(this.lower, eval + getError(descendants));
////    double weightNew = 0.8;
////    double weightOld = Math.sqrt(1 - weightNew * weightNew);
////    for (int i = 0; i < N_SAMPLES; i++) {
//////      System.out.println(deltas[i]);
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
    for (int i = 0; i < N_SAMPLES; i++) {
      str += samples[i] + " ";
    }
    return str + "\n" + this.lower + "(" + this.bestVariationPlayer + ") " + this.upper + "(" + this.bestVariationOpponent + ")\n";
  }
  
  public boolean isSolved() {
    return (this.lower == this.upper);
  }

  boolean isPartiallySolved() {
    return getLowerBound() == getUpperBound();
  }

  public static final double getErrorBase(long descendants) {
    assert(descendants > 0);
    return 1 / (1.0 + Math.sqrt(descendants)); // TODO: WTF????
  }
  
  public static final int getError(long descendants) {
//    System.out.println(getErrorBase(descendants) + " " + getErrorBase(1));
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

  public int getUpperBound() {
//    return ciUpper;
    int result = eval;
    for (short s : samples) {
      result = Math.max(result, s);
    }
    return result;
  }
  
  public int getLowerBound() {
//    return ciLower;
    int result = eval;
    for (short s : samples) {
      result = Math.min(result, s);
    }
    return result;
  }

  public int getEval() {
    return eval;
  }
  
  protected StoredBoard bestChild(int evalGoalForBoard) {
    // Idea: maximize lower bound of "acceptables".
    StoredBoard bestChild = null;
    int bestEval = Integer.MIN_VALUE;
    
    for (StoredBoard child : children) {
      int effectiveEval = Math.min(-child.eval, evalGoalForBoard);
//      System.out.println(-child.eval + " " + curEvalGoal);
      if (bestChild == null || (effectiveEval > bestEval) ||
          (effectiveEval == bestEval && -child.bestVariationPlayer > -bestChild.bestVariationPlayer)) {
        bestChild = child;
        bestEval = effectiveEval;
      }
    }
    return bestChild;
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

    bestVariationOpponent = (short) -bestChild.bestVariationPlayer;

    for (StoredBoard child : children) {
      eval = (short) Math.max(eval, -child.eval);
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
    for (int i = 0; i < StoredBoard.N_SAMPLES; ++i) {
      short minChild = Short.MIN_VALUE;
      for (StoredBoard child : children) {
        minChild = (short) Math.max(minChild, -child.samples[i]);
      }
      samples[i] = minChild;
    }
    ciLower = (short) Math.max(ciLower, this.getLowerCI());
    ciUpper = (short) Math.min(ciUpper, this.getUpperCI());
  }
}
