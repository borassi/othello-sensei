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

import static org.junit.Assert.*;

import org.junit.Test;

import board.Board;
import board.BoardTest;
import board.PossibleMovesFinderImproved;
import evaluatedepthone.BoardWithEvaluation;
import evaluatedepthone.DiskDifferenceEvaluatorPlusTwo;

public class EvaluatorAlphaBetaTest {
  PossibleMovesFinderImproved POSSIBLE_MOVES_FINDER = new PossibleMovesFinderImproved();
  EvaluatorAlphaBeta EVALUATOR = new EvaluatorAlphaBeta(
    new DiskDifferenceEvaluatorPlusTwo(), POSSIBLE_MOVES_FINDER, 
    new HashMapVisitedPositionsForAlphaBeta(2000, 1000));
  EvaluatorBasic evaluatorBasic = new EvaluatorBasic(
          new DiskDifferenceEvaluatorPlusTwo(), POSSIBLE_MOVES_FINDER);

  final static String THIRD_MOVE_BOARD_STRING = "--------\n"
                                              + "--------\n" 
                                              + "--------\n"
                                              + "---OOO--\n"
                                              + "---XX---\n"
                                              + "----X---\n"
                                              + "--------\n"
                                              + "--------\n";
  
  @Test
  public void testStartingPosition() {
    Board b = new Board();

    // At depth 1, Black has 4 disks, White has 1. +3 for Black.
    // (+1 because of the bias).
    assert(EVALUATOR.evaluatePosition(b, 1, 700, 700) <= 700);
    assert(EVALUATOR.evaluatePosition(b, 1, 0, 0) >= 0);
    assertEquals(EVALUATOR.evaluatePosition(b, 1, 0, 200), 100);

    // At depth 2, Black has 3 disks, White has 3. +0 for Black
    // (+2 because of the bias).
    assert(EVALUATOR.evaluatePosition(b, 2, 300, 300) <= 300);
    assert(EVALUATOR.evaluatePosition(b, 2, -100, -100) >= -100);
    assertEquals(EVALUATOR.evaluatePosition(b, 2, -100, 600), 200);

    // At depth 3, Black has 5 disks, White has 2. +3 for Black.
    // Unless you play e6d6, in which case Black can get 6 disks.
    // (the result is +1 because of the bias).
    assert(EVALUATOR.evaluatePosition(b, 3, 700, 700) <= 700);
    assert(EVALUATOR.evaluatePosition(b, 3, 0, 0) >= 0);
    assertEquals(EVALUATOR.evaluatePosition(b, 3, -1000, 200), 100);
  }

  @Test
  public void testStartingPositionHashMap() {
    Board b = new Board();

    // At depth 1, Black has 4 disks, White has 1. +3 for Black.
    // (+1 because of the bias).
    assert(EVALUATOR.evaluatePositionWithHashMap(b, 1, 700, 700) <= 700);
    assert(EVALUATOR.evaluatePositionWithHashMap(b, 1, 0, 0) >= 0);
    assertEquals(EVALUATOR.evaluatePositionWithHashMap(b, 1, -100, 200), 100);

    // At depth 2, Black has 3 disks, White has 3. +0 for Black
    // (+2 because of the bias).
    assert(EVALUATOR.evaluatePositionWithHashMap(b, 2, 300, 300) <= 300);
    assert(EVALUATOR.evaluatePositionWithHashMap(b, 2, -100, -100) >= -100);
    assertEquals(EVALUATOR.evaluatePositionWithHashMap(b, 2, -100, 600), 200);

    // At depth 3, Black has 5 disks, White has 2. +3 for Black.
    // Unless you play e6d6, in which case Black can get 6 disks.
    // (the result is +1 because of the bias).
    assert(EVALUATOR.evaluatePositionWithHashMap(b, 3, 700, 800) <= 700);
    assert(EVALUATOR.evaluatePositionWithHashMap(b, 3, -100, 0) >= 0);
    assertEquals(EVALUATOR.evaluatePositionWithHashMap(b, 3, -100, 200), 100);
  }

//  @Test
//  public void testEvaluatorHashMap() {
//    Board b = new Board(
//              "--------\n"
//            + "------XO\n" 
//            + "--------\n"
//            + "------OX\n"
//            + "--------\n"
//            + "------XO\n"
//            + "--------\n"
//            + "------OX\n", true);
//    evaluator.evaluatePosition(b, 10, -6400, 6400);
//    assertEquals(14, evaluator.getNVisitedPositions());
//    evaluator.resetNVisitedPositions();
//    evaluator.evaluateAlphaBetaWithHashMap(b, 10, -6400, 6400);
//    // We already have the initial position in the hash map.
//    assertEquals(0, evaluator.getNVisitedPositions());
//    evaluator = new EvaluatorAlphaBeta();
//
//    evaluator.evaluateAlphaBetaWithHashMap(b, 10, -6400, 6400);
//    assertEquals(10, evaluator.getNVisitedPositions());
//  }
  
  @Test
  public void testEvaluatorBasic() {
    Board b = new Board();
    // See above for the results.
    assertEquals(evaluatorBasic.evaluatePosition(b, 0), 200);
    assertEquals(evaluatorBasic.evaluatePosition(b, 1), 100);
    assertEquals(evaluatorBasic.evaluatePosition(b, 2), 200);
    assertEquals(evaluatorBasic.evaluatePosition(b, 3), 100);
  }

  @Test
  public void testEvaluatorAlphaBetaRandom() {
    int n = 0;
 
    for (int i = 0; i < n; i++) {
      Board b = Board.randomBoard();
      for (int d = 1; d < 5; d++) {
        int eval = evaluatorBasic.evaluatePosition(b, d);
        assert(EVALUATOR.evaluatePosition(b, d, -6400, eval - 200) >= eval - 200);
        assert(EVALUATOR.evaluatePosition(b, d, eval + 200, 6400) <= eval + 200);
        assertEquals(EVALUATOR.evaluatePosition(b, d, -6400, 6400), eval);
      }
    }
  }

  @Test
  public void testEvaluatorRandom1() {
    Board b = new Board ("OOOXOOOXXXXX-XXOOOOXXXXXXOOOOXXXXO-OOXXOXXXOOX-XXOO-OXOOXXXXOXXX", true);

    for (int d = 4; d < 5; d++) {
      System.out.println("EVAL: " + EVALUATOR.evaluatePosition(b, d));
      System.out.println("EVAL: " + evaluatorBasic.evaluatePosition(b, d));
      assertEquals(EVALUATOR.evaluatePosition(b, d), evaluatorBasic.evaluatePosition(b, d));
    }
  }
  

  @Test
  public void testEvaluatorRandom() {
    int n = 10000;
 
    for (int i = 0; i < n; i++) {
      if (i % 100 == 0) {
        System.out.println(i);
      }
      Board b = Board.randomBoard();
      EVALUATOR.resetHashMapVisitedPositions();
      if (b.getPlayer() == 0 && b.getOpponent() == 0) {
        continue;
      }
      for (int d = 1; d < 5; d++) {
        int f1 = EVALUATOR.evaluatePosition(b, d);
        int f2 = evaluatorBasic.evaluatePosition(b, d);
        if (f1 != f2) {
          System.out.println("\n\n" + d);
          System.out.println(b.getPlayerDisks());
          System.out.println(f1);
          System.out.println(f2);
          System.out.println(b);
          assert(false);
        }
      }
    }
  }

  @Test
  public void testEvaluatorPass() {
    Board b = new Board(
            "--------\n" +
            "--------\n" +
            "--------\n" +
            "---OOOOX\n" +
            "----OOOX\n" +
            "--XOOXXX\n" +
            "-XXXXXXX\n" +
            "XXXXXXXX\n", true);

    // The board has 21 Black, 9 White. In 1 move, Black can flip at most 5 disks.
    int eval = 2100;
    assertEquals(evaluatorBasic.evaluatePosition(b, 1), eval);
    assert(EVALUATOR.evaluatePosition(b, 1, eval - 2, eval - 2) >= eval - 2);
    assert(EVALUATOR.evaluatePosition(b, 1, eval + 2, eval + 2) <= eval + 2);
    assertEquals(EVALUATOR.evaluatePosition(b, 1, -6400, 6400), eval);
    
    // In 2 moves, Black can flip all white disks.
    eval = 3000;
    assertEquals(eval, evaluatorBasic.evaluatePosition(b, 2), 0.01);
    assert(EVALUATOR.evaluatePosition(b, 2, eval - 200, eval - 200) >= eval - 200);
    assert(EVALUATOR.evaluatePosition(b, 2, eval + 200, eval + 200) <= eval + 200);
    assertEquals(eval, EVALUATOR.evaluatePosition(b, 2, -6400, 6400));

    // In 3 moves, Black can flip all white disks (+31 because of bias).
    // However, in 2 moves, Black can finish the game (+32, no bias).
    eval = 6400;
    assertEquals(eval, evaluatorBasic.evaluatePosition(b, 3), 0.01);
    assert(EVALUATOR.evaluatePosition(b, 3, eval - 400, eval - 400) > eval - 400);
    assert(EVALUATOR.evaluatePosition(b, 3, eval + 400, eval + 400) < eval + 400);
    assertEquals(eval, EVALUATOR.evaluatePosition(b, 3, eval-100, eval+100));
  }

  @Test
  public void testGetSortedMoves() {
    HashMapVisitedPositionsForAlphaBeta h = new HashMapVisitedPositionsForAlphaBeta();
    Board b = new Board(THIRD_MOVE_BOARD_STRING, true);
    long[] moves = POSSIBLE_MOVES_FINDER.possibleMoves(b);
    int depth[] =   { 5,  4,  5,  5,  5};
    int lower[] = {-400, -200, -500, -100, -300};
    int upper[] = {100, 100, 100, 200, 200};
    
    for (int i = 0; i < moves.length; i++) {
      Board next = b.move(moves[i]);
      h.updateLowerBound(next, lower[i], depth[i]);
      h.updateUpperBound(next, upper[i], depth[i]);
    }
    BoardWithEvaluation[] output = new BoardWithEvaluation[60];
    for (int i = 0; i < 60; i++) {
      output[i] = new BoardWithEvaluation(new Board(0, 0), Integer.MIN_VALUE);
    }
    EvaluatorAlphaBeta.getSortedMoves(b, POSSIBLE_MOVES_FINDER, h, 7, -1000, 0, new DiskDifferenceEvaluatorPlusTwo(), output);

    assertEquals(output[0].board, b.move(moves[2]));
    assertEquals(output[1].board, b.move(moves[0]));
    assertEquals(output[2].board, b.move(moves[1]));
    assertEquals(output[3].board, b.move(moves[4]));
    assertEquals(output[4].board, b.move(moves[3]));
    assertEquals(output[5].evaluation, Integer.MIN_VALUE, 1);
  }

  @Test
  public void testGetSortedMovesOneBetterMove() {
    HashMapVisitedPositionsForAlphaBeta h = new HashMapVisitedPositionsForAlphaBeta();
    Board b = new Board(THIRD_MOVE_BOARD_STRING, true);
    long[] moves = POSSIBLE_MOVES_FINDER.possibleMoves(b);
    int depth[] =   {7, 4, 5, 5, 5};
    int upper[] = {-400, -200, -500, -100, -300};
    
    for (int i = 0; i < moves.length; i++) {
      Board next = b.move(moves[i]);
      h.updateUpperBound(next, upper[i], depth[i]);
    }
    BoardWithEvaluation[] output = new BoardWithEvaluation[60];
    for (int i = 0; i < 60; i++) {
      output[i] = new BoardWithEvaluation(new Board(0, 0), Integer.MIN_VALUE);
    }
    assertEquals(400, EvaluatorAlphaBeta.getSortedMoves(b, POSSIBLE_MOVES_FINDER, h, 7, -1000, 200, new DiskDifferenceEvaluatorPlusTwo(), output), 0.01);
    assertEquals(Integer.MIN_VALUE, output[0].evaluation, 1);
  }

  @Test
  public void testGetSortedMovesWorseMove() {
    HashMapVisitedPositionsForAlphaBeta h = new HashMapVisitedPositionsForAlphaBeta(10, 5);
    Board b = new Board(THIRD_MOVE_BOARD_STRING, true);
    long[] moves = POSSIBLE_MOVES_FINDER.possibleMoves(b);
    int depth[] =   { 7, 7,  7,  5,  5};
    int lower[] = { 200, 400, -100, -100, -300};
    int upper[] = {1000, 900,  000, -300, -300};
    
    for (int i = 0; i < moves.length; i++) {
      Board next = b.move(moves[i]);
      h.updateLowerBound(next, lower[i], depth[i]);
      h.updateUpperBound(next, upper[i], depth[i]);
    }
    BoardWithEvaluation[] output = new BoardWithEvaluation[60];
    for (int i = 0; i < 60; i++) {
      output[i] = new BoardWithEvaluation(new Board(0, 0), Integer.MIN_VALUE);
    }
    System.out.println(h);
    assertEquals(-400, EvaluatorAlphaBeta.getSortedMoves(b, POSSIBLE_MOVES_FINDER, h, 7, -100, 200, new DiskDifferenceEvaluatorPlusTwo(), output), 0.01);
    assertEquals(output[0].board, b.move(moves[4]));
    assertEquals(output[1].board, b.move(moves[3]));
    assertEquals(output[2].board, b.move(moves[2]));
    assertEquals(output[3].evaluation, Integer.MIN_VALUE, 1);
  }

  @Test
  public void testSymmetries() {
    for (int i = 0; i < 1000; i++) {
      Board b = BoardTest.randomBoard();
      int depth = (int) (Math.random() * 5) + 1;
      int eval = EVALUATOR.evaluatePosition(b, depth);
      System.out.println(eval);
      for (Board p : Board.allTranspositions(b)) {
        assertEquals(eval, EVALUATOR.evaluatePosition(p, depth));
      }
    }
  }
}
