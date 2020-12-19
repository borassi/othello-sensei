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

import org.junit.Test;

import board.Board;
import board.PossibleMovesFinderImproved;
import junit.framework.TestCase;

public class HashMapVisitedPositionsTest extends TestCase {
  PossibleMovesFinderImproved POSSIBLE_MOVES_FINDER = new PossibleMovesFinderImproved();

  @Test
  public void testAddGet() {
    EvaluatorMCTS eval = new EvaluatorMCTS(20, 10);
    StoredBoard s = StoredBoard.initialStoredBoard(new Board());
    s.addChildren(eval);
    StoredBoard t = eval.get(new Board("e6"));
    HashMapVisitedPositions visitedPositions = new HashMapVisitedPositions(11, 5);

    assertEquals(0, visitedPositions.size);
    
    assertEquals(visitedPositions.get(s.getPlayer(), s.getOpponent()), null);
    assertEquals(visitedPositions.get(s.getPlayer(), s.getOpponent()), null);
    
    visitedPositions.add(s); 
    assertEquals(123, visitedPositions.get(s.getPlayer(), s.getOpponent()).getEval());
    
    s.eval = 789;
    assertEquals(789, visitedPositions.get(s.getPlayer(), s.getOpponent()).getEval());

    visitedPositions.add(t); 
    assertEquals(456, visitedPositions.get(t.getPlayer(), t.getOpponent()).getEval());
  }

//  @Test
//  public void testNextPositionToImproveEndgame() {
//    for (int i = 0; i < 10000; i++) {
//      if (i % 1000 == 0) {
//        System.out.println("Done " + i);
//      }
//      int nElements = 10 + (int) (Math.random() * 10);
//      int totalSize = nElements + (int) (Math.random() * 10);
//
//      HashMapVisitedPositions evaluator = new HashMapVisitedPositions(
//        totalSize, totalSize, POSSIBLE_MOVES_FINDER);
//      Board start = Board.randomBoard();
//      if (start.getPlayer() == 0 && start.getOpponent() == 0) {
//        continue;
//      }
//      evaluator.addFirstPosition(new StoredBoard(start, 10, 10));
//      
//      for (int j = 0; j < nElements; j++) {
//        PositionToImprove nextPos = evaluator.nextPositionToImproveRandom(true);
//        if (nextPos == null) {
//          break;
//        }
//        StoredBoard next = nextPos.board;
//        long[] moves = POSSIBLE_MOVES_FINDER.possibleMovesAdvanced(next.player, next.opponent);
//        if (moves.length == 0) {
//          next.setSolved(BitPattern.getEvaluationGameOver(next.player, next.opponent), evaluator.getEvalGoalForBoard(next));
//          next.updateFathers();
//          continue;
//        }
//        StoredBoard[] nextBoards = new StoredBoard[moves.length];
//        for (int m = 0; m < moves.length; m++) {
//          long move = moves[m];
//          Board b = next.getBoard().move(move);
//          int eval = (int) ((Math.random() - 0.5) * 6400);
//          nextBoards[m] = new StoredBoard(b, eval, (int) (Math.random() * 100));
//        }
//        evaluator.add(nextBoards, next);
//        if (!evaluator.allCorrect()) {
//          assert(false);
//        }
////        if (Math.random() < 0.2) {
////          evaluator.setEvalGoal((int) (Math.random() * 3200));
////        }
//        boolean playerVariates = Math.random() > 0.5;
////        if (playerVariates && evaluator.firstPosition.bestVariationPlayer == -6600) {
////          continue;
////        }
////        if (!playerVariates && evaluator.firstPosition.bestVariationOpponent == 6600) {
////          continue;
////        }
////        ArrayList<StoredBoard> parents = new ArrayList<>();
////        PositionToImprove sb = evaluator.nextPositionToImproveEndgame(evaluator.firstPosition, playerVariates, true, parents);
////
////        for (int p = 0; p < sb.parents.size() - 1; ++p) {
////          assert(sb.parents.get(p+1).fathers.contains(sb.parents.get(p)));
////        }
////        if (playerVariates) {
////          if (sb.playerIsStartingPlayer) {
////            if (sb.board.bestVariationPlayer != evaluator.firstPosition.bestVariationPlayer) {
////              System.out.println(sb.board);
////              System.out.println(evaluator.firstPosition);
////              System.out.println(playerVariates);
////              System.out.println(evaluator);
////              assert(false);
////            }
////          } else {
////            if (sb.board.bestVariationOpponent != -evaluator.firstPosition.bestVariationPlayer) {
////              System.out.println(sb.board);
////              System.out.println(evaluator.firstPosition);
////              System.out.println(playerVariates);
////              System.out.println(evaluator);
////              assert(false);            
////            }
////          }
////        } else {
////          if (sb.playerIsStartingPlayer) {
////            if (sb.board.bestVariationOpponent != evaluator.firstPosition.bestVariationOpponent) {
////              System.out.println(sb.board);
////              System.out.println(evaluator.firstPosition);
////              System.out.println(playerVariates);
////              System.out.println(evaluator);
////              assert(false);
////            }
////          } else {
////            if (sb.board.bestVariationPlayer != -evaluator.firstPosition.bestVariationOpponent) {
////              System.out.println(sb.board);
////              System.out.println(evaluator.firstPosition);
////              System.out.println(playerVariates);
////              System.out.println(evaluator);
////              assert(false);            
////            }
////          }
////        }
//      }
//    }
//  }
//
}