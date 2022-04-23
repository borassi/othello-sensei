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

import constants.Constants;
import constants.Stats;
import evaluateposition.EvalWithVisited;

public class JNI {
  public static EvalWithVisited evaluateCPP(
      long player, long opponent, int lower, int upper) {
    EvalWithVisited eval = evaluateCPPInternal(player, opponent, lower, upper);
    Stats.addToNLastMoves(1);
    Stats.addToNVisitedLastMoves(eval.nVisited);
    return eval;
  }
  public static native EvalWithVisited evaluateCPPInternal(
      long player, long opponent, int lower, int upper);

  static {
    if (Constants.MOBILE) {
      System.loadLibrary("jni");
    } else {
      try {
        System.load(System.getProperty("user.dir") + "/app/build/jni/libjni.so");
      } catch (UnsatisfiedLinkError e) {
        System.load(System.getProperty("user.dir") + "/build/jni/libjni.so");
      }
    }
  }
}
