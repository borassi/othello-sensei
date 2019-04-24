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
import static org.junit.Assert.assertEquals;
import org.junit.Test;

/**
 *
 * @author michele
 */
public class StableDisksMiddleTest {
  private final StableDisksMiddle STABLE_DISKS = new StableDisksMiddle();
  
  @Test
  public void testHash1() {
    final String testBoardString = "XXO-----\n"
                                 + "XXXXXXXX\n" 
                                 + "XXO-----\n"
                                 + "-X-X----\n"
                                 + "-X--X---\n"
                                 + "-O---O--\n"
                                 + "-X----X-\n"
                                 + "-X-----X\n";
    Board testBoard = new Board(testBoardString, true);
    int hash[] = STABLE_DISKS.hashes(Board.allTranspositions(testBoard));
 
    assertEquals(1, hash[0]);
  }  
  @Test
  public void testHash2() {
    final String testBoardString = "OOOXXOOO\n"
                                 + "O-----OO\n" 
                                 + "O----O-O\n" 
                                 + "O---O--O\n" 
                                 + "O--O---O\n" 
                                 + "O-O----O\n" 
                                 + "OO-----O\n" 
                                 + "OOOOXXOO\n";
    Board testBoard = new Board(testBoardString, true);
    int hash[] = STABLE_DISKS.hashes(Board.allTranspositions(testBoard));
 
    assertEquals(2 * 65, hash[0]);
  }
  @Test
  public void testHash3() {
    final String testBoardString = "OOOOOOOO\n"
                                 + "OOOOOOOO\n"
                                 + "OOOOOOOO\n"
                                 + "OOOOOOOO\n"
                                 + "OOOOOOOO\n"
                                 + "OOOOOOOO\n"
                                 + "OOOOOOOO\n"
                                 + "OOOOOOOO\n";
    Board testBoard = new Board(testBoardString, true);
    int hash[] = STABLE_DISKS.hashes(Board.allTranspositions(testBoard));
 
    assertEquals(64 * 65, hash[0]);
  }
  @Test
  public void testHash4() {
    final String testBoardString = "OOOOOOOO\n"
                                 + "OOOXXXXO\n"
                                 + "OOXXXOOO\n"
                                 + "OOXXXXOO\n"
                                 + "OOXXXXOO\n"
                                 + "OOOXXXXO\n"
                                 + "OOXOOOOO\n"
                                 + "OOOOOOOO\n";
    Board testBoard = new Board(testBoardString, false);
    int hash[] = STABLE_DISKS.hashes(Board.allTranspositions(testBoard));
 
    assertEquals(44 + 20 * 65, hash[0]);
  }
  @Test
  public void testHashAll() {
    for (int i = 0; i < 10000; i++) {
      Board b = Board.randomBoard();
      int hashes[] = STABLE_DISKS.hashes(Board.allTranspositions(b));
      assertEquals(hashes.length, 1);
      assert(hashes[0] < STABLE_DISKS.maxSize());
    }
  }
}
