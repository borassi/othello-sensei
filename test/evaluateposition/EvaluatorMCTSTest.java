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

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package evaluateposition;

import board.Board;
import board.PossibleMovesFinderImproved;
import evaluatedepthone.DepthOneEvaluator;
import evaluatedepthone.DiskDifferenceEvaluatorPlusTwo;
import static evaluateposition.StoredBoard.N_SAMPLES;
import it.unimi.dsi.fastutil.objects.ObjectArrayList;
import org.junit.Test;
import static org.junit.Assert.*;

/**
 *
 * @author michele
 */
public class EvaluatorMCTSTest {
  PossibleMovesFinderImproved POSSIBLE_MOVES_FINDER = new PossibleMovesFinderImproved();
  DepthOneEvaluator DEPTH_ONE_EVALUATOR = new DiskDifferenceEvaluatorPlusTwo();
  EvaluatorLastMoves EVALUATOR_LAST_MOVES = new EvaluatorLastMoves();
  
  public void testBoard(StoredBoard b, int eval, int playerVariates, int opponentVariates) {
    assertEquals(eval, b.eval);
    assertEquals(playerVariates, b.playerVariates);
    assertEquals(opponentVariates, b.opponentVariates);
  }
  
  @Test
  public void testPass() {
    EvaluatorMCTS evaluator = new EvaluatorMCTS(10, 10, POSSIBLE_MOVES_FINDER, DEPTH_ONE_EVALUATOR, EVALUATOR_LAST_MOVES);
    StoredBoard pass = new StoredBoard(Board.pass(), 0, 0);
    pass.playerVariates = 5;
    pass.opponentVariates = 3;
    evaluator.addFirstPosition(pass);
    evaluator.addPositions(pass);
    StoredBoard afterPass = evaluator.get(pass.opponent, pass.player);
    assert(afterPass != null);
    assertEquals(afterPass.player, pass.opponent);
    assertEquals(afterPass.opponent, pass.player);
    assertEquals(afterPass.fathers.size(), 1);    
    assertEquals(afterPass.fathers.get(0), evaluator.get(pass.getBoard()));
    assertEquals(evaluator.get(pass.getBoard()).children.length, 1);    
    assertEquals(evaluator.get(pass.getBoard()).children[0],
                 evaluator.get(afterPass.getBoard()));
  }
  
  @Test
  public void testDoublePass() {
    EvaluatorMCTS evaluator = new EvaluatorMCTS(10, 10, POSSIBLE_MOVES_FINDER, DEPTH_ONE_EVALUATOR, EVALUATOR_LAST_MOVES);
    StoredBoard pass = new StoredBoard(Board.bothPass(), 0, 6);
    pass.playerVariates = 5;
    pass.opponentVariates = 3;
    evaluator.addFirstPosition(pass);
    evaluator.addPositions(pass);
    assertEquals(1, evaluator.size);
    int eval = (pass.getBoard().getPlayerDisks() - 
                  pass.getBoard().getOpponentDisks()) + 
                 pass.getBoard().getEmptySquares() * (int) Math.signum(
                   pass.getBoard().getPlayerDisks() - 
                   pass.getBoard().getOpponentDisks());
    
    StoredBoard passSB = evaluator.get(pass.getBoard());
    assertEquals(eval * 100, passSB.eval);
    for (short sample : passSB.samples) {
      assertEquals(sample, (short) (eval * 100));
    }
    assertEquals(passSB.playerVariates, -6600);
    assertEquals(passSB.opponentVariates, 6600);
  }

  //                  e6
  //                  +20
  //                +16 +24
  //               ____|_______________________________
  //              |                       |            |
  //             e6f4                    e6f6         e6d6
  //             -20                     -14          -12
  //           -24 -16                 -30 -14      -12 -12
  //          _____|______
  //         |         ___|___
  //       e6f4c3     |e6f4e3 |
  //        +20       |  +24  |
  //      +16 +66     |+24 +24|
  //     ____|_____   |_______|
  //    |          |
  // e6f4c3c4  e6f4c3d6 <= -14
  //   -20       -14
  // -66 +66   -66 -16
  @Test
  public void testTree() {
    EvaluatorMCTS evaluator = new EvaluatorMCTS(30, 30, POSSIBLE_MOVES_FINDER, DEPTH_ONE_EVALUATOR, EVALUATOR_LAST_MOVES);

    StoredBoard e6 = new StoredBoard(Board.e6(), 0, 0);

    StoredBoard e6f4 = new StoredBoard(Board.e6f4(), 0, 0);
    StoredBoard e6f6 = new StoredBoard(Board.e6f6(), -1400, 0);
    e6f6.playerVariates = -3000;
    StoredBoard e6d6 = new StoredBoard(Board.e6d6(), -1200, 0);

    StoredBoard e6f4c3 = new StoredBoard(new Board("e6f4c3"), 0, 0);
    StoredBoard e6f4e3 = new StoredBoard(new Board("e6f4e3"), 2400, 0);
    
    StoredBoard e6f4c3c4 = new StoredBoard(new Board("e6f4c3c4"), 0, 0);
    e6f4c3c4.setSolved(-2000);
    StoredBoard e6f4c3d6 = new StoredBoard(new Board("e6f4c3d6"), -1600, 0);
    e6f4c3d6.setUpper(-1400);

    evaluator.addFirstPosition(e6);
    evaluator.add(new StoredBoard[] {e6f4, e6f6}, e6);
    evaluator.add(new StoredBoard[] {e6f4c3, e6f4e3}, e6f4);
    evaluator.add(new StoredBoard[] {e6f4c3c4, e6f4c3d6}, e6f4c3);
    
    assertEquals(evaluator.get(e6.getBoard()), e6);
    assertEquals(evaluator.get(e6f4c3c4.getBoard()), e6f4c3c4);
    
    testBoard(e6f4c3c4, -2000, -6600, +6600);
    testBoard(e6f4c3d6, -1400, -6600, -1600);

    testBoard(e6f4c3, +2000, +1600, +6600);
    testBoard(e6f4e3, +2400, +2400, +2400);
    
    testBoard(e6f4, -2000, -2400, -1600);
    testBoard(e6f6, -1400, -3000, -1400);
    testBoard(e6d6, -1200, -1200, -1200);
    
    testBoard(e6, 2000, 1600, 2400);
    
    HashMapVisitedPositions.PositionToImprove nextPosition = 
        evaluator.nextPositionToImproveEndgame(e6, true, true);
    assertEquals(e6f4c3d6, nextPosition.board);
    assertEquals(-2001, nextPosition.alpha);
    assertEquals(-1999, nextPosition.beta);
    
    nextPosition =
        evaluator.nextPositionToImproveEndgame(e6, false, true);
    assertEquals(e6f4e3, nextPosition.board);
    assertEquals(true, nextPosition.playerIsStartingPlayer);
    assertEquals(+1999, nextPosition.alpha);
    assertEquals(+2001, nextPosition.beta);
    
    evaluator.updateEndgame(nextPosition, +2200);
    testBoard(e6f4e3, +2200, +2400, +6600);
    testBoard(e6, 2000, 1600, 6600);
    
  }
}
