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

import bitpattern.BitPattern;
import java.util.ArrayList;
import java.util.Arrays;
import org.junit.Test;
import static org.junit.Assert.*;

/**
 *
 * @author michele
 */
public class GetFlipTest {
  GetFlip FLIPPER = GetFlip.load();
  
  @Test
  public void testFlip() {
    Board board = new Board();
    ArrayList<Long> possibleMoves = new ArrayList<>(Arrays.asList(
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

    for (long flip : possibleMoves) {
      int move = Long.numberOfTrailingZeros(flip & ~(board.getPlayer() | board.getOpponent()));
      long newFlip = FLIPPER.getFlip(move, board.getPlayer(), board.getOpponent());
      if (flip != newFlip) {
        System.out.println(board);
        System.out.println(BitPattern.patternToString(flip));
        System.out.println(BitPattern.patternToString(newFlip));
        assert(false);
      }
    }
    assertEquals(0, FLIPPER.getFlip(0, board.getPlayer(), board.getOpponent()));
  }
  

  @Test
  public void testFlipManyDir() {
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

    for (long flip : possibleMoves) {
      int move = Long.numberOfTrailingZeros(flip & ~(board.getPlayer() | board.getOpponent()));
      long newFlip = FLIPPER.getFlip(move, board.getPlayer(), board.getOpponent());
      if (flip != newFlip) {
        System.out.println(board);
        System.out.println(BitPattern.patternToString(flip));
        System.out.println(BitPattern.patternToString(newFlip));
        assert(false);
      }
    }
  }
  
  @Test
  public void testFindPossibleMovesRandom() {
    PossibleMovesFinderImproved movesFinder = new PossibleMovesFinderImproved();
    int n = 100000;
    boolean movesFound[] = new boolean[64];
 
    for (int i = 0; i < n; i++) {
      Arrays.fill(movesFound, false);

      Board board = Board.randomBoard();
      long[] flips = movesFinder.possibleMoves(board);

      for (long flip : flips) {
        int move = Long.numberOfTrailingZeros(flip & ~(board.getPlayer() | board.getOpponent()));
        long newFlip = 0;
        if (board.getEmptySquares() == 1) {
          newFlip = FLIPPER.getFlipLast(move, board.getOpponent());
          newFlip = newFlip & ~board.getPlayer();
        } else {
          newFlip = FLIPPER.getFlip(move, board.getPlayer(), board.getOpponent());          
        }
        if (flip != newFlip) {
          System.out.println(board);
          System.out.println(board.getEmptySquares());
          System.out.println(BitPattern.patternToString(flip));
          System.out.println(BitPattern.patternToString(newFlip));
          assert(false);
        }
        movesFound[move] = true;
      }
      for (int move = 0; move < movesFound.length; ++move) {
        if (!movesFound[move]) {
          if (FLIPPER.getFlip(move, board.getPlayer(), board.getOpponent()) != 0) {
            System.out.println(board);
            System.out.println(move);
            assert(false);
          }
        }
      }
    }
  }
}
