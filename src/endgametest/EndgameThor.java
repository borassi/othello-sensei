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
import evaluatedepthone.BoardWithEvaluation;
import evaluateposition.EvaluatorLastMoves;
import evaluateposition.EvaluatorMCTS;
import helpers.LoadDataset;
import java.text.NumberFormat;
import java.util.ArrayList;
import java.util.Locale;

/**
 *
 * @author michele
 */
public class EndgameThor {
  public ArrayList<BoardWithEvaluation> boards;
  EvaluatorMCTS eval = new EvaluatorMCTS(400000, 200000);
//  EvaluatorLastMoves eval = new EvaluatorLastMoves();
  PossibleMovesFinder pmf = new PossibleMovesFinder();
  
  public EndgameThor() {
    boards = LoadDataset.loadTrainingSet(1977, 1984);
  }
  
  long t = 0;
  public boolean runSingleBoard(BoardWithEvaluation be) {
    Board b = be.board;
//      System.out.println(b);
//    if (Math.random() < 0.2) {
//      eval.resetHashMapVisitedPositions();
//    }
    t -= System.currentTimeMillis();
    int result = eval.evaluatePosition(b, 100, -6400, 6400);
    t += System.currentTimeMillis();
    if (result - be.evaluation > 0) {
      System.out.println(b);
      System.out.println(b.toString().replace("\n", ""));
      System.out.println(result);
      System.out.println(be.evaluation);
      return false;
    }
    return true;
  }
  
  public void run() {
    NumberFormat numberFormat = NumberFormat.getNumberInstance(Locale.US);
    int i = 0;
    int j = 0;
    for (BoardWithEvaluation be : boards) {
      if (j++ % 1000 == 0) {
        System.out.println(j + " / " + boards.size());
      }
      int d = be.board.getEmptySquares();
      if (d != 12 || pmf.haveToPass(be.board)) {
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
//    EndgameThor e = new EndgameThor();
//    e.runSingleBoard(new BoardWithEvaluation(
//      new Board("O-OXXX--OOOOX---OXOXOXXXOOXXXOXXOXXXXXXXOOXOXXXXOOOXXXXXXXXXXXXX", true),
//      14
//    ));
//    e.eval.printHashMapVisitedPositions();
//    System.out.println("OK, WE GO");
//    e.runSingleBoard(new BoardWithEvaluation(
//      new Board("XOOOOO--XOOXO---XOXOXOOOXXOOOXOOXOOOOOOOXXOXOOOOXXXOOOOOOOOOOOOO", true),
//      -8
//    ));
//    e.runSingleBoard(new BoardWithEvaluation(
//      new Board("XXXXXXXOOOOXXXXOOOXOXXXOOOXXXXXOXOXXXOXO-OOOXXXO-XOOOXXOX--OO-X-", true),
//      0
//    )); 
    new EndgameThor().run();
  }
}
