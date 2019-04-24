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

package evaluatedepthone.patternhasher;

import board.Board;
import java.util.Arrays;
import static org.junit.Assert.assertEquals;
import org.junit.Test;

/**
 *
 * @author michele
 */
public class ParityTest {
  
  @Test
  public void testHash1() {
    final String testBoardString = "--------\n"
                                 + "--------\n" 
                                 + "--------\n"
                                 + "--------\n"
                                 + "----XXXX\n"
                                 + "----XXXX\n"
                                 + "----XX--\n"
                                 + "----XX--\n";
    Board testBoard = new Board(testBoardString, true);
    int hash = new Parity().hash(testBoard.getPlayer(), testBoard.getOpponent());
    assertEquals(4 + 8 /*Player cannot move*/ + 0 /*opponent can move*/, hash);
  }
  @Test
  public void testHash2() {
    final String testBoardString = "--------\n"
                                 + "--------\n" 
                                 + "--------\n"
                                 + "--------\n"
                                 + "----XXXX\n"
                                 + "----XXXX\n"
                                 + "----XX-O\n"
                                 + "----XX--\n";
    Board testBoard = new Board(testBoardString, true);
    int hash = Parity.hash(testBoard.getPlayer(), testBoard.getOpponent());
    assertEquals(3 + 0 /*Player can move*/ + 0 /*opponent can move*/, hash);
  }
  @Test
  public void testHash3() {
    final String testBoardString = "--------\n"
                                 + "--------\n" 
                                 + "--------\n"
                                 + "--------\n"
                                 + "--------\n"
                                 + "--------\n"
                                 + "--------\n"
                                 + "--------\n";
    Board testBoard = new Board(testBoardString, true);
    int hash = Parity.hash(testBoard.getPlayer(), testBoard.getOpponent());
    assertEquals(7 + 0 /*Player can move*/ + 0 /*opponent can move*/, hash);
  }
  @Test
  public void testHash4() {
    final String testBoardString = "--------\n"
                                 + "--------\n" 
                                 + "--------\n"
                                 + "--------\n"
                                 + "----XXXX\n"
                                 + "----XXXX\n"
                                 + "----XXXX\n"
                                 + "----XXXX\n";
    Board testBoard = new Board(testBoardString, true);
    int hash = Parity.hash(testBoard.getPlayer(), testBoard.getOpponent());
    assertEquals(0 + 8 /*Player cannot move*/ + 16 /*opponent cannot move*/, hash);
  }
  @Test
  public void testHashAll() {
    for (int i = 0; i < 100; i++) {
      System.out.println(i);
      Board b = Board.randomBoard();
      Parity p = new Parity();
      int hashes[] = new int[8];
      Board[] transpositions = Board.allTranspositions(b);
      for (int j = 0; j < 8; j++) {
        Board t = transpositions[j];
        hashes[j] = Parity.hash(t.getPlayer(), t.getOpponent());
      }
      Arrays.sort(hashes);
      assertEquals(hashes[0], hashes[1]);
      assertEquals(hashes[2], hashes[3]);
      assertEquals(hashes[4], hashes[5]);
      assertEquals(hashes[6], hashes[7]);
      int[] computedHashes = p.hashes(Board.allTranspositions(b));
      assertEquals(computedHashes.length, 1);
      assertEquals(computedHashes[0], hashes[0] + hashes[2] * 32 
        + hashes[4] * 32 * 32 + hashes[6] * 32 * 32 * 32);
      assert(computedHashes[0] < p.maxSize());
    }
  }
}
