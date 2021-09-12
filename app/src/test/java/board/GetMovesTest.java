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
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import static org.junit.Assert.*;

/**
 *
 * @author michele
 */
public class GetMovesTest {

  
//  @Test
//  public void testFindPossibleMovesRandom() {
//    int n = 100000;
//    GetMoves getMoves = new GetMoves();
// 
//    for (int i = 0; i < n; i++) {
//      Board b = Board.randomBoard();
//      ArrayList<Long> movesBasicList = PossibleMovesFinderImproved.possibleMovesBasic(b);
//      long movesBasic = 0;
//      long movesClever = getMoves.getMoves(b.getPlayer(), b.getOpponent());
//      for (long move : movesBasicList) {
//        movesBasic |= move;
//      }
//      movesBasic = movesBasic & ~(b.getPlayer() | b.getOpponent());
//      if (movesBasic != movesClever) {
//        System.out.println(b);
//        System.out.println(BitPattern.patternToString(movesBasic));
//        System.out.println(BitPattern.patternToString(movesClever));
//        assert(false);
//      }
//    }
//  }
  @Test
  public void testFindFlipRandom() {
    int n = 10000;
 
    for (int i = 0; i < n; i++) {
      Board b = Board.randomBoard();
      ArrayList<Long> flipBasic = PossibleMovesFinderImproved.possibleMovesBasic(b);
      
      int move = (int) (Math.random() * 64);
      long empties = ~(b.getPlayer() | b.getOpponent());
      if (((1L << move) & empties) == 0) {
        continue;
      }
      boolean found = false;
      long actualFlip = GetMoves.getFlip(move, b.getPlayer(), b.getOpponent());
      for (long flip : flipBasic) {
        if ((flip & (1L << move)) == 0) {
          continue;
        }
        if (flip != actualFlip) {
          System.out.println(b);
          System.out.println(move);
          System.out.println(BitPattern.patternToString(actualFlip));
          assert(false);
        }
        found = true;
        break;
      }
      if (!found && actualFlip != 0) {
        System.out.println(b);
        System.out.println(move);
        System.out.println(BitPattern.patternToString(actualFlip));
        assert(false);
      }
    }
  }
  @Test
  public void testFindFlipGenericRandom() {
    int n = 100000;
 
    for (int i = 0; i < n; i++) {
      Board b = Board.randomBoard();
//      b = new Board("--------\n" +
//                    "--------\n" +
//                    "--------\n" +
//                    "--------\n" +
//                    "----X---\n" +
//                    "---O----\n" +
//                    "--O-----\n" +
//                    "--------", true);
      ArrayList<Long> flipBasic = PossibleMovesFinderImproved.possibleMovesBasic(b);
      int move = (int) (Math.random() * 64);
      long empties = ~(b.getPlayer() | b.getOpponent());
      if (((1L << move) & empties) == 0) {
        continue;
      }
      boolean found = false;
      long actualFlip = GetMoves.getFlipGeneric(move, b.getPlayer(), b.getOpponent());
      for (long flip : flipBasic) {
//        flip = flip & BitPattern.getDiag7(move);
        if ((flip & (1L << move)) == 0) {
          continue;
        }
//        if ((flip == (1L << move))) {
//          continue;
//        }
        if (flip != actualFlip) {
          System.out.println(b);
          System.out.println(move);
          System.out.println(BitPattern.patternToString(actualFlip));
          System.out.println(BitPattern.patternToString(flip));
          assert(false);
        }
        found = true;
        break;
      }
      if (!found && actualFlip != 0) {
        System.out.println(b);
        System.out.println(move);
        System.out.println(BitPattern.patternToString(actualFlip));
        assert(false);
      }
    }
  }
  @Test
  public void maxNFlipsTest() {
    assertEquals(GetMoves.maxNFlips(0, 1), 6);
    assertEquals(GetMoves.maxNFlips(1, 1), 5);
    assertEquals(GetMoves.maxNFlips(2, 1), 4);
    for (int i = 0; i < 8; ++i) {
      for (int j = 0; j < 8; ++j) {
        assertEquals(GetMoves.maxNFlips(8 * i + j, 1), 7-j-1);
        assertEquals(GetMoves.maxNFlips(8 * i + j, -1), j-1);
        assertEquals(GetMoves.maxNFlips(8 * i + j, 8), 7-i-1);
        assertEquals(GetMoves.maxNFlips(8 * i + j, -8), i-1);
        assertEquals(GetMoves.maxNFlips(8 * i + j, 9), Math.min(7-j-1, 7-i-1));
        assertEquals(GetMoves.maxNFlips(8 * i + j, -9), Math.min(j-1, i-1));
//        assertEquals(GetMoves.maxNFlips(8 * i + j, 7), Math.min(j-1, 7-i-1));
//        assertEquals(GetMoves.maxNFlips(8 * i + j, -7), Math.min(7-j-1, i-1));
      }
    }
  }
}
