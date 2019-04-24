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
  public final static int N_SAMPLES = 10;
  public long player = 0;
  public long opponent = 0;
  public short eval;
  public short playerVariates;
  public short opponentVariates;
  public short[] samples = new short[N_SAMPLES];
  public int descendants;
  public StoredBoard next = null;
  public StoredBoard prev = null;
  public ObjectArrayList<StoredBoard> fathers = new ObjectArrayList<>();
  public StoredBoard[] children = null;

  public final static GaussianNumberGenerator RANDOM = new GaussianNumberGenerator();
//
  public StoredBoard(Board board, int eval, int variance) {
    this(board, eval, variance, new int[StoredBoard.N_SAMPLES]);
  }
  
  public StoredBoard(Board board, int eval, int variance, int deltas[]) {
    this.player = board.getPlayer();
    this.opponent = board.getOpponent();
    this.updateEval(eval, variance, deltas);
    this.descendants = 1;
  }
  
  public boolean isLeaf() {
    return children == null;
  }
    
  public static StoredBoard randomStoredBoard(Board b) {
    return new StoredBoard(b, (int) ((Math.random() - 0.5) * 6400), 600);
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

  public final void setSolved(int newEval) {
    this.eval = (short) newEval;
    for (int i = 0; i < N_SAMPLES; i++) {
      this.samples[i] = (short) newEval;
    }
    this.opponentVariates = 6600;
    this.playerVariates = -6600;
  }

  public final void setLower(int newLower) {
    this.eval = (short) newLower;
    for (int i = 0; i < N_SAMPLES; i++) {
      this.samples[i] = (short) Math.max(newLower, eval);
    }
    this.opponentVariates = 6600;
  }

  public final void setUpper(int newUpper) {
    this.eval = (short) newUpper;
    for (int i = 0; i < N_SAMPLES; i++) {
      this.samples[i] = (short) Math.min(newUpper, eval);
    }
    this.playerVariates = -6600;
  }
  
  public final void updateEval(int newEval, int variance, int deltas[]) {
    this.eval = (short) Math.max(-6400, Math.min(6400, newEval));

    double weightNew = 0.2;
    double weightOld = Math.sqrt(1 - weightNew * weightNew);
    for (int i = 0; i < N_SAMPLES; i++) {
//      System.out.println(deltas[i]);
      this.samples[i] = (short) Math.round(Math.max(Math.min(
          newEval + RANDOM.next() * variance
              * weightNew + deltas[i] * weightOld
          , 6400), -6400));
    }
    this.opponentVariates = eval;
    this.playerVariates = eval;
  }

  @Override
  public String toString() {
    String str = new Board(player, opponent).toString() + eval + "\n";
    for (int i = 0; i < N_SAMPLES; i++) {
      str += samples[i] + " ";
    }
    return str + "\n" + this.playerVariates + " " + this.opponentVariates + "\n";
  }
  
  public boolean isSolved() {
    return this.playerVariates < -6400 && this.opponentVariates > 6400;
  }

  boolean isPartiallySolved() {
    int lower = getLowerBound();
    
    return lower == eval && getUpperBound() == lower;
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
