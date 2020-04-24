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

public class StoredBoard {
  public long player = 0;
  public long opponent = 0;

  public StoredBoard next = null;
  public StoredBoard prev = null;
  public ArrayList<StoredBoard> fathers = new ArrayList<>();
  public StoredBoard[] children = null;

  public int eval;
  public long descendants;
  final static int N_EVAL_SPLITS = 64;
  private static float PRECISION = 1.E-10F;
  public float[] probEvalGreaterThan = new float[N_EVAL_SPLITS];
  public float[] maxChildrenLogDerivative = new float[N_EVAL_SPLITS];

  public final static GaussianNumberGenerator RANDOM = new GaussianNumberGenerator();
  
  public StoredBoard(Board board) {
    this.player = board.getPlayer();
    this.opponent = board.getOpponent();
    for (int i = 0; i < probEvalGreaterThan.length; ++i) {
      probEvalGreaterThan[i] = 0.5F;
      maxChildrenLogDerivative[i] = 0;
    }
  }

  public StoredBoard(Board board, int eval, float variance, long descendants) {
    this(board);
    this.descendants = descendants;
    this.setEval(eval, variance);
  }
  
  public void addChildren(StoredBoard[] children) {
    this.children = children;
  }

  public boolean isLeaf() {
    return children == null;
  }
    
  public static StoredBoard randomStoredBoard(Board b) {
    return new StoredBoard(b, (int) ((Math.random() - 0.5) * 6400), 600, 1);
  }
  
  public Board getBoard() {
    return new Board(player, opponent);
  }
  
  public float dProbEvalGreaterThanLog(int offset, StoredBoard father) {
   
    return (float) Math.log((1 - father.probEvalGreaterThan[invertOffset(offset)]) / probEvalGreaterThan[offset]);
  }

  public static int invertOffset(int offset) {
    return 63 - offset;
  }

  static int evalToOffset(int eval) {
    return (eval + 6300) / 200;
  }
  
  public static int offsetToEval(int offset) {
    return offset * 200 - 6300;
  }

  public final void setSolved(int newEval) {
    this.setLower(newEval);
    this.setUpper(newEval);
  }

  public final void setLower(int newLower) {
    assert(this.isLeaf());
    this.eval = Math.max(eval, newLower);
    for (int i = -6300; i <= newLower; i += 200) {
      probEvalGreaterThan[evalToOffset(i)] = 1;
    }
  }

  public final void setUpper(int newUpper) {
    assert(this.isLeaf());
    this.eval = Math.min(eval, newUpper);
    for (int i = 6300; i >= newUpper; i -= 200) {
      probEvalGreaterThan[evalToOffset(i)] = 0;
    }
  }
  
  public int getBestOffset() {
    for (int i = 0; i < probEvalGreaterThan.length - 1; ++i) {
      if (probEvalGreaterThan[i] > 0.5 && probEvalGreaterThan[i+1] < 0.5) {
        System.out.println("I" + i);
        return (int) (i + Math.random() * 2);
      }
    }
    return probEvalGreaterThan.length - 1;
  }
  
  private static float bound(float value) {
    final float minValue = PRECISION;
    return (float) Math.max(minValue, Math.min(1 - minValue, value));
  }
  
  public final void setEval(int newEval, float variance) {
    assert(this.isLeaf());
    this.eval = newEval;
    for (int i = 0; i < probEvalGreaterThan.length; ++i) {
      if (probEvalGreaterThan[i] > 0 && probEvalGreaterThan[i] < 1) {
        probEvalGreaterThan[i] = bound(1 - (float) Gaussian.CDF(offsetToEval(i), newEval, variance));
      }
    }
  }
  
  public float childMaxLogDerivative(int offset, StoredBoard child) {
    return child.dProbEvalGreaterThanLog(offset, this) + child.maxChildrenLogDerivative[offset];
  }

  protected void updateFather() {
    assert(!this.isLeaf());
    eval = Short.MIN_VALUE;

    for (StoredBoard child : children) {
      eval = Math.max(eval, -child.eval);
    }
    for (int i = 0; i < N_EVAL_SPLITS; ++i) {
      float probAllChildrenEvalGreaterThanI = 1;
      for (StoredBoard child : children) {
        probAllChildrenEvalGreaterThanI *= child.probEvalGreaterThan[i];

      }
      this.probEvalGreaterThan[invertOffset(i)] = bound(1 - probAllChildrenEvalGreaterThanI);
      
    }
    for (int i = 0; i < N_EVAL_SPLITS; ++i) {
      float maxChildrenLogDerivativeI = Float.NEGATIVE_INFINITY;
      for (StoredBoard child : children) {
        maxChildrenLogDerivativeI = Math.max(maxChildrenLogDerivativeI, childMaxLogDerivative(i, child));
      }
      
      this.maxChildrenLogDerivative[invertOffset(i)] = maxChildrenLogDerivativeI;
    }
  }

  protected void updateFathers() {
    for (StoredBoard father : fathers) {
      father.updateFather();
      father.updateFathers();
    }
  }
  
  public float getProbEvalGreaterThan(int eval) {
    return this.probEvalGreaterThan[evalToOffset(eval)];
  }

  public boolean isSolved() {
    for (float f : this.probEvalGreaterThan) {
      if (f > 0 && f < 1) {
        return false;
      }
    }
    return true;
  }

  public String getInterestingProbabilities() {
    String result = "";
    for (int i = probEvalGreaterThan.length - 1; i >= 0; --i) {
      if (Math.abs(offsetToEval(i) - eval) < 500) {
        result += String.format("%d:%.2g (%.2g)\n", -offsetToEval(i) / 100, 1 - probEvalGreaterThan[i],
            fathers.size() > 0 ? fathers.get(0).childMaxLogDerivative(i, this) : 0);
      }
    }
    return result;
  }

  public int getEval() {
    return eval;
  }
}
