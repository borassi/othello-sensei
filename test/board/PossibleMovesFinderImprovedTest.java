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

package board;

import static org.junit.Assert.*;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.List;

import org.junit.Test;

import bitpattern.BitPattern;
import bitpattern.PositionIJ;
import it.unimi.dsi.fastutil.longs.LongArrayList;
import it.unimi.dsi.fastutil.objects.ObjectArrayList;

public class PossibleMovesFinderImprovedTest {

  public static <T> void assertListEqualsIgnoreOrder(List<T> list1, List<T> list2) {
    HashSet<T> tmp1 = new HashSet<>(list1);
    HashSet<T> tmp2 = new HashSet<>(list2);
    assertEquals(tmp1, tmp2);
  }
  
  @Test
  public void testFindPossibleMoves() {
    PossibleMovesFinderImproved movesFinder = new PossibleMovesFinderImproved();
    String boardString = Board.START_BOARD_STRING;
    Board boardBlack = new Board(boardString, true);
    ArrayList<Long> possibleMovesBlack = new ArrayList<>(Arrays.asList(
            BitPattern.parsePattern("--------\n"
                             + "--------\n" 
                             + "---X----\n"
                             + "---X----\n"
                             + "--------\n"
                             + "--------\n"
                             + "--------\n"
                             + "--------\n"),
            BitPattern.parsePattern("--------\n"
                             + "--------\n" 
                             + "--------\n"
                             + "--XX----\n"
                             + "--------\n"
                             + "--------\n"
                             + "--------\n"
                             + "--------\n"),
            BitPattern.parsePattern("--------\n"
                             + "--------\n" 
                             + "--------\n"
                             + "--------\n"
                             + "----XX--\n"
                             + "--------\n"
                             + "--------\n"
                             + "--------\n"),
            BitPattern.parsePattern("--------\n"
                             + "--------\n" 
                             + "--------\n"
                             + "--------\n"
                             + "----X---\n"
                             + "----X---\n"
                             + "--------\n"
                             + "--------\n")));

    assertListEqualsIgnoreOrder(possibleMovesBlack, new LongArrayList(movesFinder.possibleMoves(boardBlack)));
    assertListEqualsIgnoreOrder(possibleMovesBlack, new LongArrayList(movesFinder.possibleMoves(boardBlack)));
  }
  

  @Test
  public void testFindPossibleMovesManyDir() {
    PossibleMovesFinderImproved movesFinder = new PossibleMovesFinderImproved();
    Board board = new Board("--------\n"
                          + "-O-O----\n" 
                          + "--XX--O-\n"
                          + "---X-X--\n"
                          + "---XX---\n"
                          + "--------\n"
                          + "--------\n"
                          + "--------\n", false);
    ArrayList<Long> possibleMoves = new ArrayList<>(Arrays.asList(
            BitPattern.parsePattern("--------\n"
                             + "--------\n" 
                             + "---X----\n"
                             + "---X-X--\n"
                             + "---XX---\n"
                             + "---X----\n"
                             + "--------\n"
                             + "--------\n"),
            BitPattern.parsePattern("--------\n"
                             + "--------\n" 
                             + "--X-----\n"
                             + "---X----\n"
                             + "----X---\n"
                             + "-----X--\n"
                             + "--------\n"
                             + "--------\n"),
            BitPattern.parsePattern("--------\n"
                             + "--------\n" 
                             + "--X-----\n"
                             + "-X------\n"
                             + "--------\n"
                             + "--------\n"
                             + "--------\n"
                             + "--------\n")));

    assertListEqualsIgnoreOrder(possibleMoves, new LongArrayList(movesFinder.possibleMoves(board)));
  }
  
  @Test
  public void testPlayMove() {
    PossibleMovesFinderImproved movesFinder = new PossibleMovesFinderImproved();
    Board board = new Board(Board.START_BOARD_STRING, true);
    long move = BitPattern.parsePattern("--------\n"
                                      + "--------\n" 
                                      + "--------\n"
                                      + "--------\n"
                                      + "----X---\n"
                                      + "----X---\n"
                                      + "--------\n"
                                      + "--------\n");
    assertEquals(new Board(BoardTest.secondMoveBoardString, false), board.move(move));
    assertEquals(new Board(BoardTest.secondMoveBoardString, false), 
            movesFinder.moveIfPossible(board, new PositionIJ(5, 4)));
  }
  
  @Test
  public void testFindPossibleMovesRandom() {
    PossibleMovesFinderImproved movesFinder = new PossibleMovesFinderImproved();
    int n = 100000;
 
    for (int i = 0; i < n; i++) {
      Board b = Board.randomBoard();
      LongArrayList movesBasic = movesFinder.possibleMovesBasic(b);
      LongArrayList movesClever = new LongArrayList(movesFinder.possibleMoves(b));

      BoardTest.assertListEqualsIgnoreOrder(movesBasic, movesClever);
    }
  }
  
//  @Test
//  public void updateNonEmptyBitPatterns() {
//    PossibleMovesFinderImproved movesFinder = new PossibleMovesFinderImproved();
//    int n = 100000;
// 
//    for (int i = 0; i < n; i++) {
//      Board b = Board.randomBoard();
//      LongArrayList movesBasic = PossibleMovesFinderImproved.possibleMovesBasic(b);
//      PossibleMovesFinderImproved tmp = movesFinder.updateNonFullBitPatterns(b);
//      LongArrayList movesClever = new LongArrayList(tmp.possibleMoves(b));
//
//      BoardTest.assertListEqualsIgnoreOrder(movesBasic, movesClever);
//    }
//  }
  
}
