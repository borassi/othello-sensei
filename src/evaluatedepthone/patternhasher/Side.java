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

public class Side implements PatternHashInterface {
  public static final String sides[] = {"Bottom", "Right", "Top", "Left", "Bottom inv", "Left inv", "Right inv", "Top inv"};

  final static long isSide = BitPattern.parsePattern("--------\n" 
                                                   + "--------\n"
                                                   + "--------\n"
                                                   + "--------\n"
                                                   + "--------\n"
                                                   + "--XX----\n"
                                                   + "--------\n"
                                                   + "--------\n");

  final static long isEdge = BitPattern.parsePattern("--------\n" 
                                                   + "--------\n"
                                                   + "--------\n"
                                                   + "--------\n"
                                                   + "--------\n"
                                                   + "--X-----\n"
                                                   + "--------\n"
                                                   + "--------\n");

  final static long isCenter = BitPattern.parsePattern("--------\n" 
                                                     + "--------\n"
                                                     + "--------\n"
                                                     + "--------\n"
                                                     + "--------\n"
                                                     + "---X----\n"
                                                     + "--------\n"
                                                     + "--------\n");

  final static long isSweet = BitPattern.parsePattern("--------\n" 
                                                    + "--------\n"
                                                    + "--------\n"
                                                    + "--------\n"
                                                    + "--------\n"
                                                    + "--------\n"
                                                    + "--------\n"
                                                    + "--------\n");

  final static long patternSide = BitPattern.parsePattern("--------\n" 
                                                        + "--------\n"
                                                        + "--------\n"
                                                        + "--------\n"
                                                        + "--------\n"
                                                        + "--------\n"
                                                        + "-X----X-\n"
                                                        + "XXXXXXXX\n");

  final static long mainPartSide = BitPattern.parsePattern("--------\n" 
                                                         + "--------\n"
                                                         + "--------\n"
                                                         + "--------\n"
                                                         + "--------\n"
                                                         + "--------\n"
                                                         + "------X-\n"
                                                         + "XXXXXXXX\n");

  final static long xSquareSide = BitPattern.parsePattern("--------\n" 
                                                        + "--------\n"
                                                        + "--------\n"
                                                        + "--------\n"
                                                        + "--------\n"
                                                        + "--------\n"
                                                        + "-X------\n"
                                                        + "--------\n");

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

  final static long lastButOneRowEdge = BitPattern.parsePattern("--------\n" 
                                                              + "--------\n"
                                                              + "--------\n"
                                                              + "--------\n"
                                                              + "--------\n"
                                                              + "--------\n"
                                                              + "--XXXX--\n"
                                                              + "--------\n");

  final static long lastRowEdge = BitPattern.parsePattern("--------\n" 
                                                        + "--------\n"
                                                        + "--------\n"
                                                        + "--------\n"
                                                        + "--------\n"
                                                        + "--------\n"
                                                        + "--------\n"
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
  private static final int N_TRANSPOSITIONS = 4;

  /**
   * Needed to silence a warning.
   */
  private static final long serialVersionUID = 1L;
  
  static final int hash(long player, long opponent) {
    long mask = player | opponent;
    return (int) (
            (mask & maskEdge) != 0 ? 
                    (player & mainPartSide) | ((player & xSquareSide) >>> 6) |  
                    ((opponent & mainPartSide) << 10) | ((opponent & xSquareSide) << 4) |
                    Side.isSide :
              (mask & maskCenter) != 0 ? 
                      ((player & lastRowEdge) >>> 1) | ((player & lastButOneRowEdge) >>> 4) |  
                      ((opponent & lastRowEdge) << 9) | ((opponent & lastButOneRowEdge) << 6) |
                      Side.isEdge :
                (mask & maskSweet) != 0 ? 
                        ((player & patternCenter) >>> 10) | 
                        ((opponent & patternCenter) >>> 6) | 
                        Side.isCenter :
                  ((player & patternSweet) >>> 18) | ((opponent & patternSweet) >>> 14));
  }

  @Override
  public int[] hashes(Board[] transp) {
    int[] result = new int[N_TRANSPOSITIONS];
  
    for (int i = 0; i < N_TRANSPOSITIONS; i++) {
      Board t = transp[i];
      result[i] = hash(t.getPlayer(), t.getOpponent());
    }
    return result;
  }

  public final static float eval(Board r1, Board r2, Board r3, Board r4, float[] evaluators) {
    return evaluators[hash(r1.getPlayer(), r1.getOpponent())] +
      evaluators[hash(r2.getPlayer(), r2.getOpponent())] +
      evaluators[hash(r3.getPlayer(), r3.getOpponent())] +
      evaluators[hash(r4.getPlayer(), r4.getOpponent())];
  }
  
  @Override
  public int maxSize() {
    return 4194304; // 2^22
  }
}
