// Copyright 2021 Google LLC
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

import bitpattern.BitPattern;
import board.Board;
import board.GetFlip;
import board.GetMoves;
import board.GetFlipNoWrite;
import board.GetMovesCache;
import constants.Constants;
import constants.Stats;
import evaluatedepthone.BoardWithEvaluation;
import evaluatedepthone.FindStableDisks;
import helpers.LoadDataset;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author michele
 */
public class EvaluatorLastMoves {
  public static int adaptEval(int eval) {
    int tmp = ((eval + 100000) / 200) * 2 - 1000;
    return eval - (tmp * 100) > 0 ? tmp + 1 : tmp;
  }
  public static EvalWithVisited evaluateCPP(
      long player, long opponent, int lower, int upper) {
    EvalWithVisited eval = evaluateCPPInternal(player, opponent, adaptEval(lower), adaptEval(upper));
    Stats.addToNLastMoves(1);
    Stats.addToNVisitedLastMoves(eval.nVisited);
    eval.eval = eval.eval * 100;
    return eval;
  }
  public static native EvalWithVisited evaluateCPPInternal(
      long player, long opponent, int lower, int upper);
  
  static {
    if (Constants.MOBILE) {
      System.loadLibrary("evaluator_last_moves");
    } else {
      try {
        System.load(System.getProperty("user.dir") + "/app/build/evaluateindepth/libevaluator_last_moves.so");
      } catch (UnsatisfiedLinkError e) {
        System.load(System.getProperty("user.dir") + "/build/evaluateindepth/libevaluator_last_moves.so");
      }
    }
  }
}
