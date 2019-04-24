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

import bitpattern.BitPattern;
import board.Board;
import board.PossibleMovesFinderImproved;
import java.util.HashSet;
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
  public void testSetEmpties() {
    LAST_MOVE_EVALUATOR.setEmpties(THREE_EMPTIES);
    assertEquals(3, LAST_MOVE_EVALUATOR.getNEmpties());
    assertEquals(LAST_MOVE_EVALUATOR.getEmpties()[0], 0);
    assertEquals(LAST_MOVE_EVALUATOR.getEmpties()[2], 30);
    assertEquals(LAST_MOVE_EVALUATOR.getEmpties()[1], 63);
  }
  @Test
  public void testSetEmptiesOrder() {
    Board b = new Board("--XXO---\n"
                            + "XXXXOOOO\n"
                            + "XXXXOOOO\n"
                            + "XXXXOOOO\n"
                            + "XXXXOOOO\n"
                            + "XXXXOOOO\n"
                            + "XXXXOOOO\n"
                            + "XXXXOOO-\n",
                            true);
    LAST_MOVE_EVALUATOR.setEmpties(b);
    assertEquals(1, LAST_MOVE_EVALUATOR.spaces[0].nEmpties);
    assertEquals(3, LAST_MOVE_EVALUATOR.spaces[1].nEmpties);
    assertEquals(2, LAST_MOVE_EVALUATOR.spaces[2].nEmpties);
    assertEquals(0, LAST_MOVE_EVALUATOR.spaces[3].nEmpties);
    
    assertEquals(56, LAST_MOVE_EVALUATOR.spaces[1].firstEmpty.next.position);
    assertEquals(58, LAST_MOVE_EVALUATOR.spaces[1].firstEmpty.next.next.position);
    assertEquals(57, LAST_MOVE_EVALUATOR.spaces[1].firstEmpty.next.next.next.position);
  }


  @Test
  public void testFindPossibleMoves() {
    for (int i = 0; i < 10000; i++) {
      Board b = Board.randomBoard();
      long[] movesArray = POSSIBLE_MOVES_FINDER.possibleMoves(b);
      HashSet<Long> moves = new HashSet<>();
      for (long move : movesArray) {
        moves.add(move);
      }

      LAST_MOVE_EVALUATOR.setEmpties(b);
      int nEmpties = LAST_MOVE_EVALUATOR.getNEmpties(); 
      for (int j = 0; j < nEmpties; j++) {   
        int position = LAST_MOVE_EVALUATOR.getEmpties()[j];
        long flip = LAST_MOVE_EVALUATOR.getFlip(position, b.getPlayer(), b.getOpponent());

        if (flip != 0 && !moves.remove(flip)) {
          System.out.println("Found wrong move\n" + BitPattern.patternToString(flip) +
            "\nin board\n" + b);
          assert(false);          
        }
      }
      if (!moves.isEmpty()) {
        System.out.println("Did not find move\n" + 
          BitPattern.patternToString((long) moves.toArray()[0]) + "\nin board\n" +
          b);
        assert(false);
      }
    }
  }

  @Test
  public void testDoublePass() {
    Board b = new Board("--OOOOOOX-OOOOO-XOOOOOXXXOOXOXXOXOOOXXOOXOOOOOOOXOOOOO--XXXXXXXX", true);
    assertEquals(0, LAST_MOVE_EVALUATOR.evaluatePosition(b, -64, 64));
//    b = new Board("--OOOOOOX-OOOOO-XOOOOOXXXOOXOXXOXOOOXXOOXOOOOOOOXOOOOO--XXXXXXOX", true);
//    assertEquals(-4, LAST_MOVE_EVALUATOR.evaluatePosition(b, -64, 64));
  }

  @Test
  public void testNVisitedOneMove() {
    LAST_MOVE_EVALUATOR.nVisited = 0;
    Board b = new Board("OOOOOOOO"
        + "OOOOOOOO"
        + "OOOOOOOO"
        + "OOOOOOOO"
        + "OOOOOOOO"
        + "OOOOOOOO"
        + "XXXXXXXX"
        + "XXXXXXO-", true);
    assertEquals(-3200, LAST_MOVE_EVALUATOR.evaluatePosition(b, -6400, 6400));
    assertEquals(0, LAST_MOVE_EVALUATOR.nVisited);
  }
  
  @Test
  public void testNVisitedTwoMoves() {
    LAST_MOVE_EVALUATOR.nVisited = 0;
    Board b = new Board("OOOOOOOO"
        + "OOOOOOOO"
        + "OOOOOOOO"
        + "OOOOOOOO"
        + "OOOOOOOO"
        + "OOOOOOOO"
        + "XXXXXXO-"
        + "XXXXXXO-", true);
    assertEquals(-3200, LAST_MOVE_EVALUATOR.evaluatePosition(b, -6400, 6400));
    assertEquals(2, LAST_MOVE_EVALUATOR.nVisited);
  }

  @Test
  public void testE2E() {
    EvaluatorAlphaBeta eval = new EvaluatorAlphaBeta();
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
      float result = eval.evaluatePosition(b, b.getEmptySquares(), -6400, 6400);
      int alpha = (int) (result + (Math.random() - 0.5) * 1600);
      int beta = alpha + 1 + (int) (Math.random() * 800);

      int result1 = LAST_MOVE_EVALUATOR.evaluatePosition(b, alpha, beta);
      if (result < alpha) {
        assert(result <= result1 && result1 <= alpha);
      } else if (result > beta) {
        assert(beta <= result1 && result1 <= result);
      } else {
        assertEquals((int) result, result1);
      }
    }
  }
  
}
