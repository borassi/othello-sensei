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
import board.PossibleMovesFinder;
import board.PossibleMovesFinderImproved;
import evaluatedepthone.BoardWithEvaluation;
import evaluatedepthone.DepthOneEvaluator;
import evaluatedepthone.MultilinearRegression;
import evaluateposition.EvaluatorAlphaBeta;
import evaluateposition.EvaluatorMCTS;
import helpers.LoadDataset;
import java.util.ArrayList;

/**
 *
 * @author michele
 */
public class TestThor {
  public ArrayList<BoardWithEvaluation> boards;
  DepthOneEvaluator eval1 = MultilinearRegression.load(EvaluatorAlphaBeta.DEPTH_ONE_EVALUATOR_FILEPATTERN);
  PossibleMovesFinderImproved pmf = new PossibleMovesFinderImproved();
//  EvaluatorMCTS eval = new EvaluatorMCTS(20000, 10000, pmf, eval1);
  EvaluatorAlphaBeta eval = new EvaluatorAlphaBeta();
  
  public TestThor() {
    boards = LoadDataset.loadTrainingSet(1977, 1980);
  }
  
  public int errorSingleBoard(BoardWithEvaluation be) {
    Board b = be.board;
    int d = 4;
//    if (b.getEmptySquares() > 4) {
//      return 0F;
//    }

//    if (eval.evaluatePosition(b, d, -64, 64) != be.evaluation) {
//      System.out.print(b);
//      System.out.println(eval1.eval(b) + " " + eval.evaluatePosition(b, d, -64, 64) + " " + be.evaluation);
//    }
//    System.out.println(eval1.eval(b));
//    System.out.print(b);
//    System.out.println(eval1.eval(b) + " " + eval.evaluatePosition(b, d, -64, 64) + " " + be.evaluation);
//    System.out.println();
//    if (Math.abs(eval.evaluatePosition(b, 1) - be.evaluation) > 30) {
//      System.out.println(eval.evaluatePosition(b, 1));
//      System.out.println(b);
//    }
//    System.out.println(eval1.eval(b));
//    return eval1.eval(b);// 0.4F * eval.evaluatePosition(b, 8) + 0.6F * eval1.eval(b) - be.evaluation;
    return eval1.eval(b) - be.evaluation;
  }
  
  public void run() {
    int i = 0;
    float totalError = 0;
    float nVisitedPositions = 0;
    long t = System.currentTimeMillis();
    for (BoardWithEvaluation be : boards) {
      if (be.board.getEmptySquares() < 10 || pmf.haveToPass(be.board)) {
        continue;
      }
      i++;
      if (i % 1000 == 0) {
        System.out.println(i + ": " + Math.sqrt(totalError / i) + " " + 
          nVisitedPositions / i + " " + i / (System.currentTimeMillis() - t));
      }
      eval.resetNVisitedPositions();
      float curError = errorSingleBoard(be);
      nVisitedPositions += eval.getNVisitedPositions();
      totalError += curError * curError;
    }
  }
  
  public static void main(String args[]) {
    new TestThor().run();
  }
}
