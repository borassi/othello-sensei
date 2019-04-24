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
public class StableDisksTest {
  private final StableDisks STABLE_DISKS = new StableDisks();
  
  @Test
  public void testHash1() {
    final String testBoardString = "--------\n"
                                 + "--------\n" 
                                 + "--------\n"
                                 + "--------\n"
                                 + "--------\n"
                                 + "--------\n"
                                 + "--------\n"
                                 + "-------X\n";
    Board testBoard = new Board(testBoardString, true);
    int hash[] = STABLE_DISKS.hashes(Board.allTranspositions(testBoard));
 
    assertEquals(1, hash[0]);
  }  
  @Test
  public void testHash2() {
    final String testBoardString = "OOO-----\n"
                                 + "--------\n" 
                                 + "--------\n"
                                 + "--------\n"
                                 + "-------X\n"
                                 + "-------X\n"
                                 + "-------X\n"
                                 + "----XXXX\n";
    Board testBoard = new Board(testBoardString, true);
    int hash[] = STABLE_DISKS.hashes(Board.allTranspositions(testBoard));
 
    assertEquals(7 + 65 * 3, hash[0]);
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
                                 + "-XXXOOOX\n";
    Board testBoard = new Board(testBoardString, true);
    int hash[] = STABLE_DISKS.hashes(Board.allTranspositions(testBoard));
 
    assertEquals(1 + 65 * 3, hash[0]);
  }
  @Test
  public void testHash4() {
    final String testBoardString = "XOXOXOXO\n"
                                 + "--------\n" 
                                 + "--------\n"
                                 + "--------\n"
                                 + "--------\n"
                                 + "--------\n"
                                 + "--------\n"
                                 + "--------\n";
    Board testBoard = new Board(testBoardString, true);
    int hash[] = STABLE_DISKS.hashes(Board.allTranspositions(testBoard));
 
    assertEquals(4 + 65 * 4, hash[0]);
  }
  @Test
  public void testHashAll() {
    for (int i = 0; i < 100; i++) {
      Board b = Board.randomBoard();
      int hashes[] = STABLE_DISKS.hashes(Board.allTranspositions(b));
      assertEquals(hashes.length, 1);
      assert(hashes[0] < STABLE_DISKS.maxSize());
    }
  }
}
