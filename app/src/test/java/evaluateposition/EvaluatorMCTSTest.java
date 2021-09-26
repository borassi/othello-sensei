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

import bitpattern.BitPattern;
import board.Board;
import board.GetMovesCache;
import board.PossibleMovesFinderImproved;
import constants.Constants;
import evaluatedepthone.DiskDifferenceEvaluatorPlusTwo;
import static junit.framework.TestCase.assertEquals;
import org.junit.BeforeClass;
import org.junit.Test;

/**
 *
 * @author michele
 */
public class EvaluatorMCTSTest {
  final PossibleMovesFinderImproved POSSIBLE_MOVES_FINDER = PossibleMovesFinderImproved.load();

  @BeforeClass
  public static void setUpClass() {
    StoredBoard.EVALUATOR_MIDGAME = new EvaluatorMock();
  }
  
  private StoredBoardBestDescendant getPositionToImprove(EvaluatorMCTS evaluator, String sequence) {
    return StoredBoardBestDescendant.fixedDescendant(evaluator.firstPosition, 100, sequence);
  }
  
  @Test
  public void testDoubleFather() {
    EvaluatorMCTS evaluator = new EvaluatorMCTS(20, 20, new HashMap(), () -> new DiskDifferenceEvaluatorPlusTwo());
    
    evaluator.setFirstPosition(new Board("e6f4"));
    evaluator.addChildren(getPositionToImprove(evaluator, ""));
    evaluator.addChildren(getPositionToImprove(evaluator, "c3"));
    evaluator.addChildren(getPositionToImprove(evaluator, "c3c4"));

    evaluator.addChildren(getPositionToImprove(evaluator, "d3"));
    
    StoredBoard doubleFather = evaluator.get(new Board("e6f4c3c4d3"));
    assertEquals(1, doubleFather.fathers.size());
    assertEquals(evaluator.get(new Board("e6f4c3c4")), doubleFather.fathers.get(0));

    evaluator.addChildren(getPositionToImprove(evaluator, "d3c4"));
    
    if (!Constants.IGNORE_TRANSPOSITIONS) {
      assertEquals(2, doubleFather.fathers.size());
      assertEquals(evaluator.get(new Board("e6f4c3c4")), doubleFather.fathers.get(0));
      assertEquals(evaluator.get(new Board("e6f4d3c4")), doubleFather.fathers.get(1));
    }
  }
  
  @Test
  public void testPass() {
    EvaluatorMCTS evaluator = new EvaluatorMCTS(10, 10, new HashMap(), () -> new DiskDifferenceEvaluatorPlusTwo());
    Board passB = Board.pass();
    evaluator.setFirstPosition(passB);
    evaluator.addChildren(getPositionToImprove(evaluator, ""));

    StoredBoard pass = evaluator.get(passB);
    StoredBoard afterPass = evaluator.get(passB.getOpponent(), passB.getPlayer());
    assert afterPass != null;
    assertEquals(afterPass.getPlayer(), pass.getOpponent());
    assertEquals(afterPass.getOpponent(), pass.getPlayer());
    assertEquals(afterPass.fathers.size(), 1);    
    assertEquals(afterPass.fathers.get(0), evaluator.get(pass.getPlayer(), pass.getOpponent()));
    assertEquals(evaluator.get(pass.getBoard()).children.length, 1);    
    assertEquals(evaluator.get(pass.getBoard()).children[0],
                 evaluator.get(afterPass.getBoard()));
  }
  
  @Test
  public void testDoublePass() {
    EvaluatorMCTS evaluator = new EvaluatorMCTS(10, 10, new HashMap(), () -> new DiskDifferenceEvaluatorPlusTwo());
    Board passB = Board.bothPass();
    evaluator.setFirstPosition(passB);
    evaluator.addChildren(getPositionToImprove(evaluator, ""));
    assertEquals(1, evaluator.size);

    int eval = passB.getPlayerDisks() - passB.getOpponentDisks() + 
               passB.getEmptySquares() * (int) Math.signum(
                   passB.getPlayerDisks() - 
                   passB.getOpponentDisks());
    
    StoredBoard passSB = evaluator.get(passB);
    assertEquals(eval * 100, passSB.eval);
    assertEquals(eval * 100, passSB.lower);
    assertEquals(eval * 100, passSB.upper);
  }
  
  public void testBoard(StoredBoard b, int eval, double proofNumberCurEval, double proofNumberNextEval,
      double disproofNumberNextEval, double disproofNumberCurEval) {
    assertEquals(eval, b.getEval());
    // TODO.
//    assertEquals(proofNumberCurEval, b.getProofNumber(b.getEvalGoal() - 100), 1);
//    assertEquals(disproofNumberCurEval, b.getDisproofNumber(b.getEvalGoal() + 100), 1);
  }

  private void addChildrenWorstEvaluation(EvaluatorMCTS evaluator, String board) {
    StoredBoardBestDescendant pos = getPositionToImprove(evaluator, board);
    evaluator.addChildren(pos);
    for (StoredBoard child : pos.board.children) {
      child.setSolved(6400);
    }
  }
  
  class EndgameTimeEstimatorForTests extends EndgameTimeEstimatorInterface {
    
    @Override
    public double proofNumber(long player, long opponent, int lower, int approxEval) {
      Board b = new Board(player, opponent);
      if (b.equals(new Board("e6f4c3d6"))) {
        if (lower == -2100) return 10;
        if (lower == -1900) return 20;
      } else if (b.equals(new Board("e6f4e3"))) {
        if (lower == 1900) return 30;
        if (lower == 2100) return 40;
      } else if (b.equals(new Board("e6d6"))) {
        if (lower == -1900) return 1;
      }
      return 1000000;
    }
    @Override
    public double disproofNumber(long player, long opponent, int lower, int approxEval) {
      Board b = new Board(player, opponent);
      if (b.equals(new Board("e6f4c3d6"))) {
        if (lower == -2100) return 1000;
        if (lower == -1900) return 1000;
      } else if (b.equals(new Board("e6f4e3"))) {
        if (lower == 1900) return 1000;
        if (lower == 2100) return 1000;
      } else if (b.equals(new Board("e6d6"))) {
        if (lower == -1900) return 1000;
      }
      return 1000000;
    }    
  }

  // EvalGoal: +20.
  //                  e6         e6
  //                  +20   ->   +20
  //                 30 1040   +16 +66
  //               1001   10   +16 +66
  //               ____|________________________________
  //              |                        |            |
  //             e6f4      e6f4           e6f6         e6d6
  //             -20   ->  -20            -14          -12
  //            10  1K   -66 -16          0   0        0  1
  //          1040  30   -66 -16        INF INF      INF 1K
  //          _____|______
  //         |         ___|___
  //       e6f4c3     |e6f4e3 |     e6f4e3 >= +22
  //        +20       |  +24  | ->    +24
  //        0  1K     | 30 40 |      0   0
  //      INF  10     | 1K 1K |    INF INF
  //     ____|_____   |_______|
  //    |          |
  // e6f4c3c4  e6f4c3d6 <=-14
  //    -20       -16
  //    0 INF    10  20
  //  INF   0    1K  1K
//  @Test
//  public void testTree() {
//    StoredBoard.endgameTimeEstimator = new EndgameTimeEstimatorForTests();
//    EvaluatorMCTS evaluator = new EvaluatorMCTS(100, 100, EVALUATOR_BASIC);
//    
//    evaluator.setFirstPosition(new Board("e6"));
//    addChildrenWorstEvaluation(evaluator, "");
//    addChildrenWorstEvaluation(evaluator, "f4");
//    addChildrenWorstEvaluation(evaluator, "f4c3");
//
//    StoredBoard e6 = evaluator.get(new Board("e6"));
//    StoredBoard e6f4 = evaluator.get(new Board("e6f4"));
//    StoredBoard e6f4c3 = evaluator.get(new Board("e6f4c3"));
//    StoredBoard e6f4c3c4 = evaluator.get(new Board("e6f4c3c4"));
//    e6f4c3c4.setSolved(-2000);
//
//    StoredBoard e6f4c3d6 = evaluator.get(new Board("e6f4c3d6"));
//    e6f4c3d6.eval = -1600;
//    e6f4c3d6.lower = -6400;
//    e6f4c3d6.upper = 6400;
//    
//    StoredBoard e6f4e3 = evaluator.get(new Board("e6f4e3"));
//    e6f4e3.eval = 2400;
//    e6f4e3.lower = -6400;
//    e6f4e3.upper = 6400;
//    
//    StoredBoard e6f6 = evaluator.get(new Board("e6f6"));
//    e6f6.eval = -1400;
//    e6f6.lower = -1400;
//    e6f6.upper = -1400;
//    
//    StoredBoard e6d6 = evaluator.get(new Board("e6d6"));
//    e6d6.eval = -1200;
//    e6d6.lower = -2000;
//    e6d6.upper = 6400;
//
//    evaluator.setEvalGoal(2000);
//
//    testBoard(e6f4c3c4, -2000, 0, Double.POSITIVE_INFINITY, Double.POSITIVE_INFINITY, 0);
//    testBoard(e6f4c3d6, -1600, 10, 20, 1000, 1000);
//    testBoard(e6f4e3, 2400, 30, 40, 1000, 1000);
//    testBoard(e6f4c3, 2000, 0, 1000, Double.POSITIVE_INFINITY, 10);
//    testBoard(e6f4, -2000, 10, 1000, 1040, 30);
//    testBoard(e6f6, -1400, 0, 0, Double.POSITIVE_INFINITY, Double.POSITIVE_INFINITY);
//    testBoard(e6d6, -1200, 0, 1, Double.POSITIVE_INFINITY, 1000);
//    testBoard(e6, 2000, 30, 1040, 1001, 10);
//    assert !evaluator.isSolved();
//
//    assertEquals(e6f4c3d6, evaluator.getNextPosition().board);
////    assertEquals(e6f4e3, evaluator.getNextPosition().board);
//    e6f4e3.setLower(2200);
//    
//    testBoard(e6f4c3c4, -2000, 0, Double.POSITIVE_INFINITY, Double.POSITIVE_INFINITY, 0);
//    testBoard(e6f4c3d6, -1600, 10, 20, 1000, 1000);
//    testBoard(e6f4e3, 2400, 0, 0, Double.POSITIVE_INFINITY, Double.POSITIVE_INFINITY);
//    testBoard(e6f4c3, 2000, 0, 1000, Double.POSITIVE_INFINITY, 10);
//    testBoard(e6f4, -2000, 10, Double.POSITIVE_INFINITY, 1000, 0);
//    testBoard(e6f6, -1400, 0, 0, Double.POSITIVE_INFINITY, Double.POSITIVE_INFINITY);
//    testBoard(e6d6, -1200, 0, 1, Double.POSITIVE_INFINITY, 1000);
//    testBoard(e6, 2000, 0, 1000, Double.POSITIVE_INFINITY, 10);
//    assert !evaluator.isSolved();
//    
//    evaluator.upper = 2100;
//    assert !evaluator.isSolved();
//
//    evaluator.upper = 2000;
//    assert evaluator.isSolved();
//    StoredBoard.endgameTimeEstimator = new EndgameTimeEstimator();
//  }
  
  public void assertIsAllOK(EvaluatorMCTS eval) {
    for (StoredBoard firstBoard : eval.evaluationsHashMap) {
      for (StoredBoard board = firstBoard; board != null; board = board.next) {
//        isPrevNextOK() && areChildrenOK() && areThisBoardEvalsOK() && isEvalOK()
        if (!board.isAllOK()) {
          assert(false);
        }
      }
    }
  }
  
  @Test
  public void testRoundEval() {
    EvaluatorMCTS eval = new EvaluatorMCTS(100, 100);
    eval.lower = -3100;
    eval.upper = 3100;
    assertEquals(0, eval.roundEval(99));
    assertEquals(200, eval.roundEval(101));
    assertEquals(-3200, eval.roundEval(-3500));
    assertEquals(3200, eval.roundEval(6300));

    assertEquals(0, eval.roundEval(100));
    assertEquals(200, eval.roundEval(101));
    assertEquals(-3200, eval.roundEval(-3100));
    assertEquals(-3000, eval.roundEval(-3099));
    assertEquals(3000, eval.roundEval(3100));
    assertEquals(3200, eval.roundEval(3101));
    assertEquals(1000, eval.roundEval(1091));
  }
  
  @Test
  public void testTreeIsCorrectAfterUpdates() {
    HashMap hashMap = new HashMap(20000);
    for (int i = 0; i < 10000; i++) {
      if (i % 100 == 0) {
        System.out.println("Done " + i);
      }
      int nElements = 5 + (int) (Math.random() * 100);
      int totalSize = nElements + (int) (Math.random() * 100);

      EvaluatorMCTS evaluator = new EvaluatorMCTS(totalSize, totalSize, hashMap, () -> new DiskDifferenceEvaluatorPlusTwo());
      Board start = Board.randomBoard();
      if (start.getPlayer() == 0 && start.getOpponent() == 0) {
        continue;
      }
      evaluator.setFirstPosition(start.getPlayer(), start.getOpponent());
      evaluator.lower = ((int) ((Math.random() - 0.5) * 16)) * 200 - 100;
      evaluator.upper = evaluator.lower + 200 * (int) (Math.random() * 16);

      for (int j = 0; j < nElements; j++) {
        StoredBoardBestDescendant nextPos = StoredBoardBestDescendant.randomDescendant(
            evaluator.firstPosition, evaluator.evalGoal + Math.random() > 0.5 ? 100 : -100);

        if (nextPos == null) {
          break;
        }
        StoredBoard next = nextPos.board;
        double d = Math.random();
        
        next.setBusy(100);
        long[] moves = POSSIBLE_MOVES_FINDER.possibleMovesAdvanced(next.getPlayer(), next.getOpponent());
        if (moves.length == 0 && (new GetMovesCache()).getMoves(next.getOpponent(), next.getPlayer()) == 0) {
          int correctEval = BitPattern.getEvaluationGameOver(next.getPlayer(), next.getOpponent());
          next.setSolved(correctEval);
          next.setFree(100);
          continue;
        }

        int eval = (int) ((Math.random() - 0.5) * 6400);
        eval = Math.max(Math.min(eval, next.getUpper()), next.getLower());
        if (d <= 0.05) {
          next.setLower(eval);
          next.setFree(100);
        } else if (d <= 0.01) {
          next.setUpper(eval);
          next.setFree(100);
        } else if (d <= 0.015 || nextPos.board.lower > -6400 || nextPos.board.upper < 6400) {
          next.setSolved(eval);
          next.setFree(100);
        } else {
          next.setFree(100);
          evaluator.addChildren(nextPos);
        }
        next.updateFathers();
        if (Math.random() < 0.1) {
          assertIsAllOK(evaluator);
        }
      }
      if (Math.random() < 0.1) {
        boolean allLeaves = true;
        for (StoredBoard firstBoard : evaluator.evaluationsHashMap) {
          for (StoredBoard board = firstBoard; board != null; board = board.next) {
            if (!board.isLeaf()) {
              allLeaves = false;
              board.eval = (short) (board.eval + (Math.random() - 0.5) * 20);
            }
          }
        }
        if (!allLeaves) {
          int tmp = evaluator.lower + (int) (Math.random() * (evaluator.upper - evaluator.lower));
          evaluator.setEvalGoal(evaluator.roundEval(tmp));
          assertIsAllOK(evaluator);
        }
      }
    }
  }
}