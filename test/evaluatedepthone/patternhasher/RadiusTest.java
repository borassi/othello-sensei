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

import bitpattern.BitPattern;
import board.Board;
import static org.junit.Assert.assertEquals;
import org.junit.Test;

/**
 *
 * @author michele
 */
public class RadiusTest {
  private final Radius RADIUS = new Radius();
  
  @Test
  public void extend() {
    final String testBoardString = "--------\n"
                                 + "--------\n" 
                                 + "-X------\n"
                                 + "--------\n"
                                 + "--------\n"
                                 + "--------\n"
                                 + "--------\n"
                                 + "--------\n";
    final long extendHorizontal = BitPattern.parsePattern(
        "--------\n"
      + "--------\n" 
      + "XXXXXXXX\n"
      + "--------\n"
      + "--------\n"
      + "--------\n"
      + "--------\n"
      + "--------\n");
    final long extendVertical = BitPattern.parsePattern(
        "-X------\n"
      + "-X------\n" 
      + "-X------\n" 
      + "-X------\n" 
      + "-X------\n" 
      + "-X------\n" 
      + "-X------\n" 
      + "-X------\n");
    final long extendDiag1 = BitPattern.parsePattern(
        "--------\n"
      + "X-------\n" 
      + "-X------\n" 
      + "--X-----\n" 
      + "---X----\n" 
      + "----X---\n" 
      + "-----X--\n" 
      + "------X-\n");
    final long extendDiag2 = BitPattern.parsePattern(
        "---X----\n"
      + "--X-----\n" 
      + "-X------\n" 
      + "X-------\n" 
      + "--------\n" 
      + "--------\n" 
      + "--------\n" 
      + "--------\n");
    Board testBoard = new Board(testBoardString, true);
    assertEquals(Radius.extendHorizontal(testBoard.getPlayer()), extendHorizontal);
    assertEquals(Radius.extendVertical(testBoard.getPlayer()), extendVertical);
    assertEquals(Radius.extendDiag1(testBoard.getPlayer()), extendDiag1);
    assertEquals(Radius.extendDiag2(testBoard.getPlayer()), extendDiag2);
  }
  
  @Test
  public void getHashTestEasy() {
    final String testBoardString = "X-------\n"
                                 + "--------\n" 
                                 + "--------\n"
                                 + "--------\n"
                                 + "--------\n"
                                 + "--------\n"
                                 + "--------\n"
                                 + "--------\n";
    final Board testBoard = new Board(testBoardString, true);
    assertEquals(15 + 12 * 16, 
                 Radius.getHash(testBoard.getPlayer(), testBoard.getOpponent()));
  }
  @Test
  public void getHashTestAll() {
    final String testBoardString = "----XXXO\n"
                                 + "--XXXXXO\n"
                                 + "-OOOOOOO\n"
                                 + "XOXXXXXO\n"
                                 + "XXOXXXXO\n"
                                 + "XXXOXXX-\n"
                                 + "--XXOXOO\n"
                                 + "--XXXOXO\n";
    final Board testBoard = new Board(testBoardString, true);
    assertEquals(0 + 1 * 16 + 1 * 256 + 1 * 4096 + 1 * 65536,
                 Radius.getHash(testBoard.getPlayer(), testBoard.getOpponent()));
  }

  
}
