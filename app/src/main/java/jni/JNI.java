// Copyright 2022 Google LLC
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

import android.content.res.AssetManager;

import java.util.ArrayList;

import board.Board;
import constants.Constants;
import constants.Stats;
import evaluateposition.EvalWithVisited;
import evaluateposition.EvaluatorDerivativeInterface;
import evaluateposition.EvaluatorMCTS;
import evaluateposition.Status;
import evaluateposition.StoredBoard;
import evaluateposition.TreeNodeInterface;

public class JNI implements EvaluatorDerivativeInterface {
  private long pointer;

  public long getPointer() {
    return pointer;
  }

  public JNI(AssetManager assetManager) {
    pointer = 0;
    create(assetManager);
  }
  public native void create(AssetManager assetManager);

  @Override
  public native void finalize();

  private void setup(long pointer) {
    this.pointer = pointer;
  }

  static {
    if (Constants.MOBILE) {
      System.loadLibrary("jni");
    } else {
      System.load(System.getProperty("user.dir") + "/build/jni/libjni.so");
    }
  }
  @Override
  public void evaluate(Board board, int lower, int upper, long maxNVisited, int maxTimeMillis, float gap) {
    evaluate(board.getPlayer(), board.getOpponent(), lower, upper, maxNVisited, maxTimeMillis, gap);
  }

  public native void evaluate(long player, long opponent, int lower,
                              int upper, long maxNVisited, int maxTimeMillis, float gap);

  @Override
  public native void empty();

  @Override
  public native void stop();

  @Override
  public native Status getStatus();

  @Override
  public native TreeNodeInterface getFirstPosition();

  @Override
  public native TreeNodeInterface get(long player, long opponent);

  @Override
  public native boolean finished(long maxNVisited);

  public static EvalWithVisited evaluateCPP(
      long player, long opponent, int depth, int lower, int upper) {
    EvalWithVisited eval = evaluateCPPInternal(player, opponent, depth, lower, upper);
    Stats.addToNLastMoves(1);
    Stats.addToNVisitedLastMoves(eval.nVisited);
    return eval;
  }

  public static native EvalWithVisited evaluateCPPInternal(
      long player, long opponent, int depth, int lower, int upper);

}
