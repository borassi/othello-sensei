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

package evaluatedepthone;

import bitpattern.BitPattern;
import board.Board;
import board.PossibleMovesFinderImproved;
import java.util.Arrays;
import static org.junit.Assert.assertEquals;
import org.junit.Test;

/**
 *
 * @author michele
 */
public class FindStableDisksTest {
  private final FindStableDisks STABLE_DISKS = FindStableDisks.load();
  
  @Test
  public void testSimple() {
    final String testBoardString = "--------\n"
                                 + "--------\n" 
                                 + "--------\n"
                                 + "--OOO---\n"
                                 + "--------\n"
                                 + "--------\n"
                                 + "--------\n"
                                 + "-------X\n";
    Board testBoard = new Board(testBoardString, true);
    long stable = STABLE_DISKS.getStableDisks(testBoard);
    long expected = BitPattern.parsePattern("--------\n"
                                          + "--------\n" 
                                          + "--------\n"
                                          + "--------\n"
                                          + "--------\n"
                                          + "--------\n"
                                          + "--------\n"
                                          + "-------X\n");
    assertEquals(expected, stable);
  }
  @Test
  public void testLeft() {
    final String testBoardString = "X-------\n"
                                 + "X-------\n" 
                                 + "X-------\n"
                                 + "X-------\n"
                                 + "--------\n"
                                 + "X-------\n"
                                 + "X-------\n"
                                 + "X-------\n";
    Board testBoard = new Board(testBoardString, true);
    long stable = STABLE_DISKS.getStableDisks(testBoard);
    long expected = testBoard.getPlayer();
    assertEquals(expected, stable);
  }
  @Test
  public void testCenter() {
    final String testBoardString = "----O---\n"
                                 + "X---O---\n" 
                                 + "-O--O--O\n"
                                 + "--OOO-O-\n"
                                 + "---OOO--\n"
                                 + "XXXXXXXX\n"
                                 + "---XXX--\n"
                                 + "--X-X-X-\n";
    Board testBoard = new Board(testBoardString, true);
    long stable = STABLE_DISKS.getStableDisks(testBoard);
    long expected = BitPattern.parsePattern("--------\n"
                                          + "--------\n" 
                                          + "--------\n"
                                          + "--------\n"
                                          + "--------\n"
                                          + "----X---\n"
                                          + "--------\n"
                                          + "--------\n");
    assertEquals(stable, expected);
  }
  @Test
  public void testUpdate() {
    final String testBoardString = "XXXXOOO-\n"
                                 + "XXXXO---\n" 
                                 + "XOOOO--O\n"
                                 + "XOOOO-O-\n"
                                 + "--------\n"
                                 + "-------O\n"
                                 + "-----XXO\n"
                                 + "---OOOOO\n";
    Board testBoard = new Board(testBoardString, true);
    long stable = STABLE_DISKS.getStableDisks(testBoard);
    long expected = BitPattern.parsePattern("XXXX----\n"
                                          + "XXX-----\n" 
                                          + "X-------\n"
                                          + "X-------\n"
                                          + "--------\n"
                                          + "-------X\n"
                                          + "-------X\n"
                                          + "---XXXXX\n");
    System.out.println(BitPattern.patternToString(stable));
    assertEquals(stable, expected);
  }
  @Test
  public void testGetFullRowsColumns() {
    long input = BitPattern.parsePattern(
          "--------\n"
        + "XXXXXXXX\n" 
        + "XXXX-XXX\n"
        + "XXXXXXX-\n"
        + "-XXXXXXX\n"
        + "XXX---XX\n"
        + "--------\n"
        + "--------\n");
    long output = FindStableDisks.getFullRows(~input);
    long expected = BitPattern.parsePattern(
          "--------\n"
        + "XXXXXXXX\n"
        + "--------\n"
        + "--------\n"
        + "--------\n"
        + "--------\n"
        + "--------\n"
        + "--------\n");
    assertEquals(expected, output);
    input = BitPattern.transpose(input);
    expected = BitPattern.transpose(expected);
    assertEquals(expected, FindStableDisks.getFullColumns(~input));
  }
  
  @Test
  public void testGetFullDiags() {
    long input = BitPattern.parsePattern(
          "---X----\n"
        + "-----XXX\n"
        + "----XXXX\n"
        + "X--XXX--\n"
        + "--XXXX--\n"
        + "-XXXX--X\n"
        + "-XXX--X-\n"
        + "XXX--XXX\n");
    long output = FindStableDisks.getFullRevDiags(~input);
    long expected = BitPattern.parsePattern(
          "--------\n"
        + "-------X\n"
        + "------X-\n"
        + "-----X--\n"
        + "----X---\n"
        + "---X---X\n"
        + "--X---X-\n"
        + "-X---X-X\n");
    assertEquals(expected, output);
    input = BitPattern.horizontalMirror(input);
    expected = BitPattern.horizontalMirror(expected);
    assertEquals(expected, FindStableDisks.getFullDiags(~input));
  }
  
  @Test
  public void testAll() {
    PossibleMovesFinderImproved pmf = new PossibleMovesFinderImproved();
    for (int i = 0; i < 100000; i++) {
      Board orig = Board.randomBoard();
      Board b = orig.deepCopy();
      long stable = STABLE_DISKS.getStableDisks(b);
      long stablePlayer = stable & b.getPlayer();
      long stableOpponent = stable & b.getOpponent();
      for (int d = 0; d < 5; ++d) {
        long tmp = stableOpponent;
        stableOpponent = stablePlayer;
        stablePlayer = tmp;
        long[] moves = pmf.possibleMoves(b);
        if (moves.length == 0) {
          continue;
        }
        long move = moves[(int) (Math.random() * moves.length)];
        
        b.moveInPlace(move);
        if ((stablePlayer & b.getOpponent()) != 0) {
          System.out.println(orig);
          System.out.println(b);
          System.out.println(BitPattern.patternToString(stable));
          assert(false);
        }
        assert((stableOpponent & b.getPlayer()) == 0);
      }
    }
  }
}
