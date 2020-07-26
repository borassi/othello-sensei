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
import evaluatedepthone.DepthOneEvaluator;
import evaluatedepthone.DiskDifferenceEvaluatorPlusTwo;
import java.util.ArrayList;
import static junit.framework.TestCase.assertEquals;
import org.junit.Test;
import static org.junit.Assert.*;

/**
 *
 * @author michele
 */
public class EvaluatorMCTSTest {
  EvaluatorInterface EVALUATOR_BASIC = new DiskDifferenceEvaluatorPlusTwo();
  PossibleMovesFinderImproved POSSIBLE_MOVES_FINDER = PossibleMovesFinderImproved.load();
  
  public void testBoard(StoredBoard b, int eval, int bestVariationPlayer, int bestVariationOpponent) {
    assertEquals(eval, b.getEval());
//    assertEquals(bestVariationPlayer, b.bestVariationPlayer);
//    assertEquals(bestVariationOpponent, b.bestVariationOpponent);
  }

  private HashMapVisitedPositions.PositionToImprove getPositionToImprove(EvaluatorMCTS evaluator, Board board) {
    StoredBoard b = evaluator.get(board);
    ArrayList<StoredBoard> fathers = new ArrayList<>();
    StoredBoard curFather = b;
    
    while (curFather.fathers != null && !curFather.fathers.isEmpty()) {
      curFather = curFather.fathers.get((int) (curFather.fathers.size() * Math.random()));
      fathers.add(curFather);
    }
    
    return new HashMapVisitedPositions.PositionToImprove(b, Math.random() > 0.5, fathers);
  }

  private HashMapVisitedPositions.PositionToImprove getPositionToImprove(EvaluatorMCTS evaluator, String sequence) {
    return getPositionToImprove(evaluator, new Board(sequence));
  }
  
  @Test
  public void testDoubleFather() {
    EvaluatorMCTS evaluator = new EvaluatorMCTS(20, 20, EVALUATOR_BASIC);
    
    evaluator.setFirstPosition(new Board("e6f4"));
    evaluator.addChildren(getPositionToImprove(evaluator, "e6f4"));
    evaluator.addChildren(getPositionToImprove(evaluator, "e6f4c3"));
    evaluator.addChildren(getPositionToImprove(evaluator, "e6f4c3c4"));

    evaluator.addChildren(getPositionToImprove(evaluator, "e6f4d3"));
    
    StoredBoard doubleFather = evaluator.get(new Board("e6f4c3c4d3"));
    assertEquals(1, doubleFather.fathers.size());
    assertEquals(evaluator.get(new Board("e6f4c3c4")), doubleFather.fathers.get(0));

    evaluator.addChildren(getPositionToImprove(evaluator, "e6f4d3c4"));

    assertEquals(2, doubleFather.fathers.size());
    assertEquals(evaluator.get(new Board("e6f4c3c4")), doubleFather.fathers.get(0));
    assertEquals(evaluator.get(new Board("e6f4d3c4")), doubleFather.fathers.get(1));
  }
  
  @Test
  public void testPass() {
    EvaluatorMCTS evaluator = new EvaluatorMCTS(10, 10, EVALUATOR_BASIC);
    Board passB = Board.pass();
    evaluator.setFirstPosition(passB);
    evaluator.addChildren(getPositionToImprove(evaluator, passB));

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
    EvaluatorMCTS evaluator = new EvaluatorMCTS(10, 10, EVALUATOR_BASIC);
    Board passB = Board.bothPass();
    evaluator.setFirstPosition(passB);
    evaluator.addChildren(getPositionToImprove(evaluator, passB));
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
//
//  //                  e6        e6
//  //                  +20   ->  +20
//  //                +16 +24   +16 +66
//  //               ____|________________________________
//  //              |                        |            |
//  //             e6f4      e6f4           e6f6         e6d6
//  //             -20   ->  -20            -14          -12
//  //           -24 -16   -66 -16        -30 -14      -12 -12
//  //          _____|______
//  //         |         ___|___
//  //       e6f4c3     |e6f4e3 |    e6f4e3 >= +22
//  //        +20       |  +24  | ->   +24
//  //      +16 +66     |+24 +24|    +24 +66
//  //     ____|_____   |_______|
//  //    |          |
//  // e6f4c3c4  e6f4c3d6 <=-14
//  //   -20       -16
//  // -66 +66   -16 -16
//  @Test
//  public void testTree() {
//    EvaluatorMCTS evaluator = new EvaluatorMCTS(30, 30, POSSIBLE_MOVES_FINDER, EVALUATOR_MIDGAME);
//
//    StoredBoard e6 = new StoredBoard(Board.e6(), 0, 0);
//
//    StoredBoard e6f4 = new StoredBoard(Board.e6f4(), 0, 0);
//    StoredBoard e6f6 = new StoredBoard(Board.e6f6(), -1400, 0);
//    StoredBoard e6d6 = new StoredBoard(Board.e6d6(), -1200, 0);
//
//    StoredBoard e6f4c3 = new StoredBoard(new Board("e6f4c3"), 0, 0);
//    StoredBoard e6f4e3 = new StoredBoard(new Board("e6f4e3"), 2400, 0);
//    
//    StoredBoard e6f4c3c4 = new StoredBoard(new Board("e6f4c3c4"), 0, 0);
//    e6f4c3c4.setSolved(-2000, 7000);
//    StoredBoard e6f4c3d6 = new StoredBoard(new Board("e6f4c3d6"), -1600, 0);
//    e6f4c3d6.setUpper(-1400, 7000);
//
//    evaluator.addFirstPosition(e6);
//    evaluator.add(new StoredBoard[] {e6f4, e6f6, e6d6}, e6);
//    evaluator.add(new StoredBoard[] {e6f4c3, e6f4e3}, e6f4);
//    evaluator.add(new StoredBoard[] {e6f4c3c4, e6f4c3d6}, e6f4c3);
//    
//    assertEquals(evaluator.get(e6.getBoard()), e6);
//    assertEquals(evaluator.get(e6f4c3c4.getBoard()), e6f4c3c4);
//    evaluator.setEvalGoal(2000);
//    
//    testBoard(e6f4c3c4, -2000, -6600, +6600);
//    testBoard(e6f4c3d6, -1600, -1600, -1600);
//
//    testBoard(e6f4c3, +2000, +1600, +6600);
//    testBoard(e6f4e3, +2400, +2400, +2400);
//    
//    testBoard(e6f4, -2000, -2400, -1600);
//    testBoard(e6f6, -1400, -1400, -1400);
//    testBoard(e6d6, -1200, -1200, -1200);
//    
//    testBoard(e6, 2000, 1600, 2400);
//    
////    HashMapVisitedPositions.PositionToImprove nextPosition = 
////        evaluator.nextPositionToImproveEndgame(e6, true, true, new ArrayList<>());
////    assertEquals(e6f4c3d6, nextPosition.board);
////    assertEquals(-2001, nextPosition.alpha);
////    assertEquals(-2000, nextPosition.beta);
////
////    nextPosition =
////        evaluator.nextPositionToImproveEndgame(e6, false, true, new ArrayList<>());
////    assertEquals(e6f4e3, nextPosition.board);
////    assertEquals(true, nextPosition.playerIsStartingPlayer);
////    assertEquals(+1999, nextPosition.alpha);
////    assertEquals(+2000, nextPosition.beta);
//
////    evaluator.updateEndgame(nextPosition, +2200);
////    testBoard(e6f4e3, +2400, +2400, +6600);
////    testBoard(e6, 2000, 1600, 6600);    
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
  public void testTreeIsCorrectAfterUpdates() {
    for (int i = 0; i < 10000; i++) {
      if (i % 100 == 0) {
        System.out.println("Done " + i);
      }
      int nElements = 5 + (int) (Math.random() * 100);
      int totalSize = nElements + (int) (Math.random() * 100);

      EvaluatorMCTS evaluator = new EvaluatorMCTS(totalSize, totalSize, EVALUATOR_BASIC);
      Board start = Board.randomBoard();
      if (start.getPlayer() == 0 && start.getOpponent() == 0) {
        continue;
      }
      evaluator.setFirstPosition(start.getPlayer(), start.getOpponent());
      evaluator.lower = (int) ((Math.random() - 0.5) * 3200);
      evaluator.upper = (int) (evaluator.lower + Math.random() * 3200);
      
      for (int j = 0; j < nElements; j++) {
        HashMapVisitedPositions.PositionToImprove nextPos = evaluator.nextPositionToImproveRandom(
            evaluator.firstPosition, Math.random() > 0.5, new ArrayList<>());

        if (nextPos == null) {
          break;
        }
        StoredBoard next = nextPos.board;
        double d = Math.random();
        
        long[] moves = POSSIBLE_MOVES_FINDER.possibleMovesAdvanced(next.getPlayer(), next.getOpponent());
        if (moves.length == 0 && (new GetMovesCache()).getMoves(next.getOpponent(), next.getPlayer()) == 0) {
          int correctEval = BitPattern.getEvaluationGameOver(next.getPlayer(), next.getOpponent());
          next.setSolved(correctEval);
          continue;
        }

        int eval = (int) ((Math.random() - 0.5) * 6400);
        eval = Math.max(Math.min(eval, next.getUpper()), next.getLower());
        if (d <= 0.1) {
          next.setLower(eval);
          next.updateFathers();
          continue;
        } else if (d <= 0.2) {
          next.setUpper(eval);
          next.updateFathers();
          continue;
        } else if (d <= 0.3) {
          next.setSolved(eval);
          next.updateFathers();
          continue;
        }
        evaluator.addChildren(nextPos);
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
          evaluator.setEvalGoal((int) (evaluator.lower + (Math.random() * (evaluator.upper - evaluator.lower))));
          assertIsAllOK(evaluator);
        }
      }
    }
  }
}