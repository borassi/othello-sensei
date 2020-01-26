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
import evaluateposition.EvaluatorAlphaBeta;
import evaluateposition.EvaluatorMCTS;
import evaluateposition.EvaluatorMidgame;
import evaluateposition.StoredBoard;
import helpers.LoadDataset;
import java.util.ArrayList;

/**
 *
 * @author michele
 */
public class TestThor {
  public ArrayList<BoardWithEvaluation> boards;
  PatternEvaluatorImproved eval1 = PatternEvaluatorImproved.load();
  PossibleMovesFinderImproved pmf = new PossibleMovesFinderImproved();
//  EvaluatorMCTS eval = new EvaluatorMCTS(100, 10);
  EvaluatorMidgame eval = new EvaluatorMidgame(eval1);
  long time;
//  EvaluatorAlphaBeta eval = new EvaluatorAlphaBeta();
  
  public TestThor() {
    boards = LoadDataset.loadTestingSet();
  }
  
  public int errorSingleBoardDepthOne(BoardWithEvaluation be, int depth) {
    Board b = be.board;
//    int d = 1;
//    if (b.getEmptySquares() > 4) {
//      return 0F;
//    }
    long[] moves = pmf.possibleMoves(b);
    int eval = Integer.MIN_VALUE;
    
    for (long move : moves) {
      eval = Math.max(eval, -eval1.eval(b.move(move)));
    }

//    System.out.println(eval + " " + this.eval.evaluatePosition(b, 1));
//    return -this.eval.evaluatePosition(b, 0) - be.evaluation;
    return eval * 30 / 30 + eval1.eval(b) * 0 / 30 - be.evaluation;
  }
  
  public int errorSingleBoard(BoardWithEvaluation be, int depth) {
    Board b = be.board;
//    int d = 1;
//    if (b.getEmptySquares() > 4) {
//      return 0F;
//    }
    long[] moves = pmf.possibleMoves(b);
//    Board sb = new StoredBoard(b, 0, 0);
    long startTime = System.currentTimeMillis();
    int curEval = eval.evaluatePosition(b, depth, -6400, 6400);
    time += System.currentTimeMillis() - startTime;
//    for (long move : moves) {
//      eval = Math.max(eval, -eval1.eval(b.move(move)));
//    }

//    System.out.println(eval + " " + this.eval.evaluatePosition(b, 1));
//    return -this.eval.evaluatePosition(b, 0) - be.evaluation;
    return curEval - be.evaluation;// eval * 30 / 30 + eval1.eval(b) * 0 / 30 - be.evaluation;
  }
  
  public void run() {
    int i = 0;
    int depth = 6;
    float totalError = 0;
    for (BoardWithEvaluation be : boards) {
      if (be.board.getEmptySquares() > 64 || be.board.getEmptySquares() < 4 || pmf.haveToPass(be.board)) {
        continue;
      }
      if (be.board.getEmptySquares() < depth + 12) {
        continue;
      }
      i++;
      float curError = errorSingleBoard(be, depth);
      totalError += curError * curError;

      if (i % 1000 == 0) {
        System.out.println(i + "/" + boards.size() + ": " + Math.sqrt(totalError / i));
        System.out.println("  Visited positions: " + String.format("%.0f", eval.getNVisitedPositions() * 1000. / this.time));
        System.out.println("  Computed moves:    " + String.format("%.0f", eval.getNComputedMoves() * 1000. / this.time));
        System.out.println("  Average positions: " + eval.getNVisitedPositions() / i);
        System.out.println("  Depth: " + depth);
      }
    }
  }
  
  public static void main(String args[]) {
    new TestThor().run();
  }
}
