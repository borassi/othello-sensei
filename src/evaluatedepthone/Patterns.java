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

public class Patterns {
  final static long patternCornerThree = BitPattern.parsePattern("--------\n"
                                                               + "--------\n"
                                                               + "--------\n"
                                                               + "--------\n"
                                                               + "--------\n"
                                                               + "-----XXX\n" 
                                                               + "-----XXX\n"
                                                               + "-----XXX\n");

  final static long patternCornerTwoFour = BitPattern.parsePattern("--------\n"
                                                                 + "--------\n"
                                                                 + "--------\n"
                                                                 + "--------\n"
                                                                 + "--------\n"
                                                                 + "--------\n" 
                                                                 + "----XXXX\n"
                                                                 + "----XXXX\n");

  final static long patternDiagonal = BitPattern.parsePattern("X-------\n" 
                                                            + "-X------\n"
                                                            + "--X-----\n"
                                                            + "---X----\n"
                                                            + "----X---\n"
                                                            + "-----X--\n"
                                                            + "------X-\n"
                                                            + "-------X\n");

  final static long patternSide = BitPattern.parsePattern("--------\n" 
                                                        + "--------\n"
      	                                                + "--------\n"
                                                        + "--------\n"
      	                                                + "--------\n"
                                                        + "--------\n"
      	                                                + "-X----X-\n"
                                                        + "XXXXXXXX\n");

  final static long maskEdge = BitPattern.parsePattern("--------\n" 
                                                     + "--------\n"
                                                     + "--------\n"
                                                     + "--------\n"
                                                     + "--------\n"
                                                     + "--------\n"
                                                     + "-X----X-\n"
                                                     + "X------X\n");

  final static long patternEdge = BitPattern.parsePattern("--------\n" 
                                                        + "--------\n"
                                                        + "--------\n"
                                                        + "--------\n"
                                                        + "--------\n"
                                                        + "--------\n"
                                                        + "--XXXX--\n"
                                                        + "-XXXXXX-\n");

  final static long maskCenter = BitPattern.parsePattern("--------\n" 
                                                       + "--------\n"
                                                       + "--------\n"
                                                       + "--------\n"
                                                       + "--------\n"
                                                       + "--------\n"
                                                       + "-X----X-\n"
                                                       + "XXXXXXXX\n");

  final static long patternCenter = BitPattern.parsePattern("--------\n"
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "--------\n" 
                                                          + "--XXXX--\n"
                                                          + "--XXXX--\n"
                                                          + "--------\n");

  final static long maskSweet = BitPattern.parsePattern("--------\n"
                                                      + "--------\n"
                                                      + "--------\n"
                                                      + "--------\n"
                                                      + "--------\n"
                                                      + "--------\n"
                                                      + "-XXXXXX-\n" 
                                                      + "XXXXXXXX\n");

  final static long patternSweet = BitPattern.parsePattern("--------\n" 
                                                         + "--------\n"
                                                         + "--------\n"
                                                         + "--------\n"
                                                         + "--XXXX--\n"
                                                         + "--XXXX--\n"
                                                         + "--------\n"
                                                         + "--------\n");

  /**
   * Returns a subpattern of the board, if the cases specified
   * by the mask are empty. Otherwise, returns the empty board.
   * 
   * The subpattern is a board, where the cases outside the pattern are empty.
   * 
   * @param board the board
   * @param pattern the pattern of interest
   * @param mask if the cases in the mask are not empty, the pattern
   * is automatically empty.
   * @return the resulting pattern.
   */
  final static Board getPattern(Board board, long pattern, long mask) {
    long player = board.getPlayer();
    long opponent = board.getOpponent();
    if (((player | opponent) & mask) == 0) {
      return new Board(player & pattern, opponent & pattern);
    }
    return new Board(0, 0);
  }
}
