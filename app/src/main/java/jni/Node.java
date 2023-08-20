// Copyright 2023 Michele Borassi
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
package jni;

import java.util.ArrayList;

import board.Board;
import constants.Constants;

public class Node {

  private final long player;
  private final long opponent;
  private final long descendants;
  private final float eval;
  private final float leafEval;
  private final int lower;
  private final int upper;
  private final int weakLower;
  private final int weakUpper;
  private final int percentileLower;
  private final int percentileUpper;
  private final ArrayList<Evaluation> evaluations;

  public Node(
      long player,
      long opponent,
      long descendants,
      float eval,
      float leafEval,
      int lower,
      int upper,
      int weakLower,
      int weakUpper,
      int percentileLower,
      int percentileUpper,
      ArrayList<Evaluation> evaluations) {
    this.player = player;
    this.opponent = opponent;
    this.descendants = descendants;
    this.eval = eval;
    this.leafEval = leafEval;
    this.lower = lower;
    this.upper = upper;
    this.weakLower = weakLower;
    this.weakUpper = weakUpper;
    this.evaluations = evaluations;
    this.percentileLower = percentileLower;
    this.percentileUpper = percentileUpper;
  }

  public long getDescendants() { return descendants; }

  public boolean isSolved() { return lower == upper; }

  public float getEval() { return eval; }

  public float getLeafEval() { return leafEval; }

  public int getPercentileLower() { return percentileLower; }

  public int getPercentileUpper() { return percentileUpper; }

  public int getLower() { return lower; }

  public int getUpper() { return upper; }

  public int getWeakLower() { return weakLower; }

  public int getWeakUpper() { return weakUpper; }

  private int evalGoalToOffset(int evalGoal) {
    assert((evalGoal + 64) % 2 == 1);
    assert(evalGoal >= weakLower);
    assert(evalGoal <= weakUpper);
    return (evalGoal - weakLower) / 2;
  }

  public float proofNumber(int evalGoal) {
    return evaluations.get(evalGoalToOffset(evalGoal)).proofNumber();
  }

  public float disproofNumber(int evalGoal) {
    return evaluations.get(evalGoalToOffset(evalGoal)).disproofNumber();
  }

  public double getProb(int evalGoal) {
    return evaluations.get(evalGoalToOffset(evalGoal)).probGreaterEqual();
  }

  public int maxLogDerivative(int evalGoal) {
    return evaluations.get(evalGoalToOffset(evalGoal)).maxLogDerivative();
  }

//  public native int childLogDerivative(Node child, int evalGoal);

  public long getPlayer() { return player; }
  public long getOpponent() { return opponent; }

  public Board getBoard() {
    return new Board(getPlayer(), getOpponent());
  }

  public String getLines() {
    int lower = getPercentileLower();
    int upper = getPercentileUpper();
    String lines;
    String evalFormatter;
    if (isSolved()) {
      evalFormatter = "%+.0f";
    } else {
      evalFormatter = "%+.2f";
    }
    return
        String.format(evalFormatter, -getEval() / 100.0) + "\n" +
        JNI.prettyPrintDouble(getDescendants()) + "\n" + (
            lower == upper ?
                JNI.prettyPrintDouble(proofNumber(lower)) + " " +
                    JNI.prettyPrintDouble(disproofNumber(lower)) :
                ("[" + (-upper/100) + ", " + (-lower/100) + "]")
        );
  }
}
