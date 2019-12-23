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
import it.unimi.dsi.fastutil.objects.ObjectArrayList;

public class StoredBoard {
  public final static int N_SAMPLES = 20;
  public long player = 0;
  public long opponent = 0;
  // Keep lower <= eval <= upper?????
  public short eval;
  public short lower = -6400;
  public short upper = 6400;
  public short bestVariationPlayer;
  public short bestVariationOpponent;
  
  public boolean playerIsStartingPlayer;
  public short[] samples = new short[N_SAMPLES];
  public int descendants;
  public StoredBoard next = null;
  public StoredBoard prev = null;
  public ObjectArrayList<StoredBoard> fathers = new ObjectArrayList<>();
  public StoredBoard[] children = null;

  public final static GaussianNumberGenerator RANDOM = new GaussianNumberGenerator();

  public StoredBoard(Board board, int eval, int variance) {
    this.player = board.getPlayer();
    this.opponent = board.getOpponent();
    this.updateEval(eval, variance);
    this.descendants = 0;
  }
  
  public StoredBoard(Board board, int eval, int variance, int deltas[]) {
    this.player = board.getPlayer();
    this.opponent = board.getOpponent();
    this.updateEval(eval, variance, deltas);
    this.descendants = 0;
  }
  
  public boolean isLeaf() {
    return children == null;
  }
    
  public static StoredBoard randomStoredBoard(Board b) {
    return new StoredBoard(b, (int) ((Math.random() - 0.5) * 6400), 600, new int[StoredBoard.N_SAMPLES]);
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
  }
  
  public final void setEvalGoalForLeaf(int evalGoal) {
    assert(this.isLeaf());
    assert(evalGoal <= 6400 && evalGoal >= -6400);
    this.bestVariationOpponent = lower >= evalGoal ? 6600 : eval;
    this.bestVariationPlayer = upper <= evalGoal ? -6600 : eval;
  }
  
  public final void updateEval(int newEval, int variance) {
    this.eval = (short) Math.max(this.lower, Math.min(this.upper, newEval));

    for (int i = 0; i < N_SAMPLES; i++) {
      this.samples[i] = (short) Math.max(this.lower, Math.min(
          this.upper,
          (int) (newEval + RANDOM.next() * variance + 0.5)));
    }
  }
  
  public final void updateEval(int newEval, int variance, int deltas[]) {
    this.eval = (short) Math.max(this.lower, Math.min(this.upper, newEval));

    double weightNew = 0.6;
    double weightOld = Math.sqrt(1 - weightNew * weightNew);
    for (int i = 0; i < N_SAMPLES; i++) {
//      System.out.println(deltas[i]);
      this.samples[i] = (short) Math.round(Math.max(this.lower, Math.min(
          this.upper,
          newEval + RANDOM.next() * variance * weightNew - deltas[i] * weightOld)));
    }
    this.bestVariationOpponent = eval;
    this.bestVariationPlayer = eval;
  }

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

  public int getUpperBound() {
    int result = eval;
    for (short s : samples) {
      result = Math.max(result, s);
    }
    return result;
  }

  public int getLowerBound() {
    int result = eval;
    for (short s : samples) {
      result = Math.min(result, s);
    }
    return result;
  }

  public int getEval() {
    return eval;
  }
}
