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
import board.PossibleMovesFinderImproved;
import evaluatedepthone.DepthOneEvaluator;
import evaluatedepthone.MultilinearRegressionImproved;
import evaluatedepthone.PatternEvaluatorImproved;
import org.junit.Test;
import static org.junit.Assert.*;

/**
 *
 * @author michele
 */
public class EvaluatorLastMovesTest {
  static EvaluatorLastMoves LAST_MOVE_EVALUATOR = new EvaluatorLastMoves();
  static PossibleMovesFinderImproved POSSIBLE_MOVES_FINDER = 
    new PossibleMovesFinderImproved();

  final static Board THREE_EMPTIES =
        new Board("-XXXOOOO\n"
                + "XXXXOOOO\n"
                + "XXXXOOOO\n"
                + "XXXXOOOO\n"
                + "X-XXOOOO\n"
                + "XXXXOOOO\n"
                + "XXXXOOOO\n"
                + "XXXXOOO-\n",
                true);

  @Test
  public void testDoublePass() {
    Board b = new Board("--OOOOOOX-OOOOO-XOOOOOXXXOOXOXXOXOOOXXOOXOOOOOOOXOOOOO--XXXXXXXX", true);
    assertEquals(0, LAST_MOVE_EVALUATOR.evaluatePosition(b, -6400, 6400, 1L << (int) (Math.random() * 64)));
    b = new Board("--OOOOOOX-OOOOO-XOOOOOXXXOOXOXXOXOOOXXOOXOOOOOOOXOOOOO--XXXXXXOX", true);
    assertEquals(-400, LAST_MOVE_EVALUATOR.evaluatePosition(b, -6400, 6400, 0));
  }

  @Test
  public void testManyPass() {
    Board b = new Board("XXXXXXXXXXXX-XXXXXXXXXXXXXOXXXXX-XXXXXX-XXXX-XX-XX-XXXXXXXX-XXXX", true);
    assertEquals(2400, LAST_MOVE_EVALUATOR.evaluatePosition(b, -6400, 6400, 1L << (int) (Math.random() * 64)));
  }

  @Test
  public void testNVisitedOneMove() {
    LAST_MOVE_EVALUATOR.resetNVisited();
    Board b = new Board("OOOOOOOO"
        + "OOOOOOOO"
        + "OOOOOOOO"
        + "OOOOOOOO"
        + "OOOOOOOO"
        + "OOOOOOOO"
        + "XXXXXXXX"
        + "XXXXXXO-", true);
    assertEquals(-3200, LAST_MOVE_EVALUATOR.evaluatePosition(b, -6400, 6400, 1L << (int) (Math.random() * 64)));
    // This should be 0, but we get 1 because we did not evaluate the position at depth 2.
    assertEquals(1, LAST_MOVE_EVALUATOR.nVisited);
  }
//  
//  @Test
//  public void testGetMoves() {
//    PossibleMovesFinderImproved pmf = new PossibleMovesFinderImproved();
//    EvaluatorLastMoves.Move[] moves = new EvaluatorLastMoves.Move[64];
//    for (int i = 0; i < 64; ++i) {
//      moves[i] = new EvaluatorLastMoves.Move();
//    }
//    for (int iter = 0; iter < 1000; ++iter) {
//      Board b = Board.randomBoard();
//      LAST_MOVE_EVALUATOR.depthOneEval.setup(b.getPlayer(), b.getOpponent());
//      LAST_MOVE_EVALUATOR.depthOneEval.invert();
//      LAST_MOVE_EVALUATOR.getMovesAdvanced(b.getPlayer(), b.getOpponent(), (int) (Math.random() * 64), moves);
//      long movesTest[] = pmf.possibleMovesAdvanced(b.getPlayer(), b.getOpponent());
//      LongArrayList movesFromEvaluator = new LongArrayList();
//
//      for (EvaluatorLastMoves.Move m : moves) {
//        if (m.move < 0) {
//          break;
//        }
//        m.flip = m.flip & ~b.getPlayer();
//        if (!new LongArrayList(movesTest).contains(m.flip)) {
//          System.out.println(b);
//          System.out.println(BitPattern.patternToString(m.flip));
//          assert(false);
//        }
//        movesFromEvaluator.add(m.flip);
//      }
//
////      assertListEqualsIgnoreOrder(movesBasic, movesFromEvaluator);
//      assertListEqualsIgnoreOrder(new LongArrayList(movesTest), movesFromEvaluator);
//    }
//  }
//  
//  @Test
//  public void testNVisitedTwoMoves() {
//    LAST_MOVE_EVALUATOR.resetNVisited();
//    Board b = new Board("OOOOOOOO"
//        + "OOOOOOOO"
//        + "OOOOOOOO"
//        + "OOOOOOOO"
//        + "OOOOOOOO"
//        + "OOOOOOOO"
//        + "XXXXXXO-"
//        + "XXXXXXO-", true);
//    assertEquals(-3200, LAST_MOVE_EVALUATOR.evaluatePosition(b, -6400, 6400, 1L << (int) (Math.random() * 64)));
//    assertEquals(2, LAST_MOVE_EVALUATOR.nVisited);
//  }

  @Test
  public void testE2E() {
    EvaluatorBasic eval = new EvaluatorBasic(PatternEvaluatorImproved.load(), new PossibleMovesFinderImproved());
    for (int i = 0; i < 10000; i++) {
      double percBlack = Math.random() * 0.9;
      double percWhite = 0.9 - percBlack;
      Board b = Board.randomBoard(percBlack, percWhite);
      if (b.getEmptySquares() > 8) {
        i--;
        continue;
      }
      if (i % 1000 == 0) {
        System.out.println(i);
      }
      float result = eval.evaluatePosition(b, b.getEmptySquares() + 3, -6400, 6400);
      int alpha = (int) (result + (Math.random() - 0.5) * 1600);
      int beta = alpha + 1 + (int) (Math.random() * 800);

      int result1 = LAST_MOVE_EVALUATOR.evaluatePosition(b, alpha, beta, 1L << (int) (Math.random() * 64));
      if (result < alpha) {
        if (result > result1 || result1 > alpha) {
          System.out.println(alpha + " " + beta);
          System.out.println(b.toString().replace("\n", ""));
          assert(false);          
        }
        assert(result <= result1 && result1 <= alpha);
      } else if (result > beta) {
        if (beta > result1 || result1 > result) {
          System.out.println("FAILED");
          System.out.println(result);
          System.out.println(result1);
          System.out.println(alpha + " " + beta);
          System.out.println(b.toString().replace("\n", ""));
          assert(false);
        }
        assert(beta <= result1 && result1 <= result);
      } else {
        if (result != result1) {
          System.out.println(b);
          System.out.println(result);
          System.out.println(result1);
          assert(false);          
        }
      }
    }
  }
}
