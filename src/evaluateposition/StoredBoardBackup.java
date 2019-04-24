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

public class StoredBoardBackup {
  public final static int N_SAMPLES = 20;
  public long player = 0;
  public long opponent = 0;
  public short eval;
  public short playerVariates;
  public short opponentVariates;
  public short[] samples = new short[N_SAMPLES];
  public ObjectArrayList<StoredBoardBackup> fathers = new ObjectArrayList<>();
  public StoredBoardBackup prevToRemove = null;
  public StoredBoardBackup nextToRemove = null;
  public StoredBoardBackup[] children = null;
  public StoredBoardBackup nextInHashMap = null;
  public StoredBoardBackup prevInHashMap = null;
  public int positionInHashMap = 0;
  public int descendants = 0;
  private final static GaussianNumberGenerator RANDOM = new GaussianNumberGenerator();

  public StoredBoardBackup(Board board) {
    this.reset(board.getPlayer(), board.getOpponent());
  }

  public StoredBoardBackup(Board board, int eval, int variance) {
    this(board);
    this.updateEval(eval, variance);
  }
  
  public final void reset(long player, long opponent) {
    this.player = player;
    this.opponent = opponent;
    this.eval = 0;
    
    this.playerVariates = 6600;
    this.opponentVariates = -6600;

    samples[0] = 6600;
    samples[1] = -6600;
    for (int i = 2; i < N_SAMPLES; i++) {
      samples[i] = 0;
    }
    this.fathers = new ObjectArrayList();
    this.prevToRemove = null;
    this.nextToRemove = null;
    this.prevInHashMap = null;
    this.nextInHashMap = null;
    this.children = null;
    this.descendants = 1;
  }

  public boolean isLeaf() {
    return children == null;
  }
  
  public static StoredBoardBackup randomStoredBoard(Board b) {
    int eval = (int) ((Math.random() - 0.5) * 6400);

    return new StoredBoardBackup(b, eval, 600);
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
    this.playerVariates = eval;
  }

  public final void setUpper(int newUpper) {
    this.eval = (short) newUpper;
    for (int i = 0; i < N_SAMPLES; i++) {
      this.samples[i] = (short) Math.min(newUpper, eval);
    }
    this.playerVariates = -6600;
    this.opponentVariates = eval;
  }
  
  public final void updateEval(int newEval, int variance) {
    this.eval = (short) Math.max(-6400, Math.min(6400, newEval));

    for (int i = 0; i < N_SAMPLES; i++) {
      this.samples[i] = (short) Math.round(Math.max(Math.min(newEval + RANDOM.next() * variance, 6400), -6400));
    }
    this.opponentVariates = -6600;
    this.playerVariates = eval;
  }

  @Override
  public String toString() {
    if (isNull()) {
      return "()\n";
    }
    String str = new Board(player, opponent).toString() + eval + "\n";
    for (int i = 0; i < N_SAMPLES; i++) {
      str += samples[i] + " ";
    }
    return str + "\n" + this.getLowerBound() + " " + this.getUpperBound() + "\n";
  }
  
  public boolean isSolved() {
    return this.playerVariates < -6400 && this.opponentVariates > 6400;
  }
  
  public boolean hasEval() {
    return this.playerVariates <= 6400 || this.opponentVariates >= -6400;
  }

  public boolean isNull() {
    return player == 0 && opponent == 0;
  }

  public int getUpperBound() {
    int result = -6400;
    for (short s : samples) {
      result = Math.max(result, -s);
    }
    return result;
  }

  public int getLowerBound() {
    int result = 6400;
    for (short s : samples) {
      result = Math.min(result, -s);
    }
    return result;
  }

  boolean isPartiallySolved() {
    return getUpperBound() == getLowerBound();
  }

  public int getEval() {
    return -eval;
  }
}
