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

package endgametest;

import board.Board;
import board.PossibleMovesFinderImproved;
import evaluatedepthone.BoardWithEvaluation;
import evaluatedepthone.DepthOneEvaluator;
import evaluatedepthone.PatternEvaluatorImproved;
import evaluateposition.EvaluatorMCTS;
import evaluateposition.EvaluatorAlphaBeta;
import evaluateposition.StoredBoard;
import helpers.LoadDataset;
import java.util.ArrayList;

/**
 *
 * @author michele
 */
public class TestThor {
  public final ArrayList<BoardWithEvaluation> boards;
  final PatternEvaluatorImproved eval1 = new PatternEvaluatorImproved();
  final PossibleMovesFinderImproved pmf = new PossibleMovesFinderImproved();
  final EvaluatorAlphaBeta eval = new EvaluatorAlphaBeta();
  long time;
  
  public TestThor() {
    boards = LoadDataset.loadTestingSet();
  }
  
  public int errorSingleBoardDepthOne(BoardWithEvaluation be, int depth) {
    Board b = be.board;
    long[] moves = pmf.possibleMoves(b);
    int eval = Integer.MIN_VALUE;
    
    for (long move : moves) {
      eval = Math.max(eval, -eval1.eval(b.move(move)));
    }

    return eval * 20 / 30 + eval1.eval(b) * 10 / 30 - be.evaluation;
  }
  
  public int errorSingleBoard(BoardWithEvaluation be, int depth) {
    Board b = be.board;
    long[] moves = pmf.possibleMoves(b);
    long startTime = System.currentTimeMillis();
    int curEval = eval.evaluate(b, depth, -6400, 6400);
    time += System.currentTimeMillis() - startTime;
//    for (long move : moves) {
//      eval = Math.max(eval, -eval1.eval(b.move(move)));
//    }

//    System.out.println(eval + " " + this.eval.evaluatePosition(b, 1));
//    return -this.eval.evaluatePosition(b, 0) - be.evaluation;
    return curEval - be.evaluation;// eval * 30 / 30 + eval1.eval(b) * 0 / 30 - be.evaluation;
  }
  
  public void run() {
    int n = 0;
    int depth = 4;
    long visitedPositions = 0;
    float totalError = 0;
    double[] totalErrors = new double[64];
    long[] nPositions = new long[64];

    for (BoardWithEvaluation be : boards) {
      int nEmpties = be.board.getEmptySquares();
      if (nEmpties > 64 || nEmpties < 4 || pmf.haveToPass(be.board)) {
        continue;
      }
      n++;
//      if (be.board.getEmptySquares() < depth + 12) {
//        continue;
//      }
      float curError = errorSingleBoard(be, nEmpties < 22 ? 2 : 4);
      visitedPositions += eval.getNVisited();
      totalErrors[nEmpties] += (curError * curError);
      nPositions[nEmpties]++;
      totalError += curError * curError;
      if (n > 0 && n % 1000 == 0) {
        System.out.println(n + "/" + boards.size() + ": " + Math.sqrt(totalError / n));
        System.out.println("  Positions / sec: " + String.format("%.0f", visitedPositions * 1000. / this.time));
//        System.out.println("  Computed moves:    " + String.format("%.0f", eval.getNComputedMoves() * 1000. / this.time));
        System.out.println("  Average positions: " + visitedPositions / n);
        System.out.println("  Depth: " + depth);
      }
    }
    for (int i = 0; i < totalErrors.length; ++i) {
      System.out.println(i + " " + Math.sqrt(totalErrors[i] / nPositions[i]));
    }
  }
  
  public static void main(String args[]) {
    new TestThor().run();
  }
}
