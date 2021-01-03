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
import evaluateposition.EvaluatorMidgame;
import helpers.LoadDataset;
import java.text.NumberFormat;
import java.util.ArrayList;
import java.util.Locale;
import java.util.Random;

/**
 *
 * @author michele
 */
public class EndgameThorUpperBound {
  public ArrayList<BoardWithEvaluation> boards;
//  EvaluatorMCTS eval = new EvaluatorMCTS(400000, 200000);
//  EvaluatorLastMoves eval = new EvaluatorLastMoves();
  EvaluatorMidgame eval = new EvaluatorMidgame();
  PossibleMovesFinderImproved pmf = new PossibleMovesFinderImproved();
  
  public EndgameThorUpperBound() {
    boards = LoadDataset.loadTrainingSet(1977, 1987);
  }
  
  long t = 0;
  public boolean runSingleBoard(BoardWithEvaluation be) {
    Board b = be.board;
    Random generator = new Random(1234);
    t -= System.currentTimeMillis();
    int resultDepth0 = eval.evaluatePosition(b, 0, -6400, 6400);
    int resultDepth1 = eval.evaluatePosition(b, 1, -6400, 6400);
    int resultDepth2 = eval.evaluatePosition(b, 2, -6400, 6400);
    int resultDepth4 = eval.evaluatePosition(b, 4, -6400, 6400);
    int resultDepth6 = eval.evaluatePosition(b, 6, -6400, 6400);
    int result = eval.evaluatePosition(b, 60, -6400, 6400);
    if (result < -3000 || result > 3000) {
      return true;
    }
    
    for (int alpha = -3000; alpha <= 3000; alpha += 200) {
      if (Math.random() > 0.1) {
        continue;
      }
      eval.resetNVisited();
      int resultCur = eval.evaluatePosition(b, 60, alpha, alpha + 1);
      System.out.print(b.getEmptySquares() + " " + result + " " + alpha + " " + eval.getNVisited() + " " + resultCur + " ");
      System.out.print(resultDepth0 + " " + resultDepth1 + " " + resultDepth2 + " ");
      System.out.println(resultDepth4 + " " + resultDepth6);
    }
    return true;
  }
  
  public void run() {
    NumberFormat numberFormat = NumberFormat.getNumberInstance(Locale.US);
    int i = 0;
    int j = 0;
    System.out.println("Empty Result Alpha NVisited Result Depth0 Depth1 Depth2 Depth4 Depth6");
    for (BoardWithEvaluation be : boards) {
//      if (j++ % 1000 == 0) {
//        System.out.println(j + " / " + boards.size());
//      }
      int d = be.board.getEmptySquares();
      if (d <= 4 || d >= 20 || pmf.haveToPass(be.board)) {
        continue;
      }
      i++;
      if (!runSingleBoard(be)) {
        System.out.println("FAIL");
        break;
      }
    }
    System.out.println(t + " " + numberFormat.format(eval.getNVisited()) + " " + i);
    System.out.println("Visited / s: " + numberFormat.format((int) (eval.getNVisited() * 1000. / t)));
    System.out.println("Visited / pos: " + numberFormat.format((int) (eval.getNVisited() / i)));
  }
  
  public static void main(String args[]) {    
    new EndgameThorUpperBound().run();
  }
}
