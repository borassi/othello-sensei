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
  EvaluatorMCTS eval = new EvaluatorMCTS(100, 10);
//  EvaluatorAlphaBeta eval = new EvaluatorAlphaBeta();
  
  public TestThor() {
    boards = LoadDataset.loadTestingSet();
  }
  
  public int errorSingleBoard(BoardWithEvaluation be) {
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
    return eval * 15 / 30 + eval1.eval(b) * 15 / 30 - be.evaluation;
  }
  
  public void run() {
    int i = 0;
    float totalError = 0;
    float nVisitedPositions = 0;
    long t = System.currentTimeMillis();
    for (BoardWithEvaluation be : boards) {
      if (be.board.getEmptySquares() > 64 || be.board.getEmptySquares() < 4 || pmf.haveToPass(be.board)) {
        continue;
      }
      i++;
      float curError = errorSingleBoard(be);
      totalError += curError * curError;

      if (i % 1000 == 0) {
        System.out.println(i + ": " + Math.sqrt(totalError / i));
      }
    }
  }
  
  public static void main(String args[]) {
    new TestThor().run();
  }
}
