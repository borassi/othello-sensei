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

public class SideImproved implements PatternHashInterface {
  public static final String sides[] = {"Bottom", "Left", "Top", "Right"};

  final static long isSide = BitPattern.parsePattern("--------\n" 
                                                   + "--------\n"
                                                   + "--------\n"
                                                   + "--------\n"
                                                   + "--------\n"
                                                   + "--------\n"
                                                   + "--------\n"
                                                   + "--------\n");

  final static long isEdge = BitPattern.parsePattern("--------\n" 
                                                   + "--------\n"
                                                   + "--------\n"
                                                   + "--------\n"
                                                   + "--------\n"
                                                   + "--------\n"
                                                   + "-----X--\n"
                                                   + "--------\n");

  final static long isCenter = BitPattern.parsePattern("--------\n" 
                                                     + "--------\n"
                                                     + "--------\n"
                                                     + "--------\n"
                                                     + "--------\n"
                                                     + "--------\n"
                                                     + "----X---\n"
                                                     + "--------\n");

  final static long isSweet = BitPattern.parsePattern("--------\n" 
                                                    + "--------\n"
                                                    + "--------\n"
                                                    + "--------\n"
                                                    + "--------\n"
                                                    + "--------\n"
                                                    + "----X--X\n"
                                                    + "--------\n");

  final static long patternSide = BitPattern.parsePattern("--------\n" 
                                                        + "--------\n"
                                                        + "--------\n"
                                                        + "--------\n"
                                                        + "--------\n"
                                                        + "--------\n"
                                                        + "------X-\n"
                                                        + "----XXXX\n");

  final static long mainPartSide = BitPattern.parsePattern("--------\n" 
                                                         + "--------\n"
                                                         + "--------\n"
                                                         + "--------\n"
                                                         + "--------\n"
                                                         + "--------\n"
                                                         + "--------\n"
                                                         + "----XXXX\n");

  final static long xSquareSide = BitPattern.parsePattern("--------\n" 
                                                        + "--------\n"
                                                        + "--------\n"
                                                        + "--------\n"
                                                        + "--------\n"
                                                        + "--------\n"
                                                        + "------X-\n"
                                                        + "--------\n");

  final static long maskEdge = BitPattern.parsePattern("--------\n" 
                                                     + "--------\n"
                                                     + "--------\n"
                                                     + "--------\n"
                                                     + "--------\n"
                                                     + "--------\n"
                                                     + "------X-\n"
                                                     + "-------X\n");

  final static long patternEdge = BitPattern.parsePattern("--------\n" 
                                                        + "--------\n"
                                                        + "--------\n"
                                                        + "--------\n"
                                                        + "--------\n"
                                                        + "--------\n"
                                                        + "----XX--\n"
                                                        + "----XXX-\n");

  final static long lastButOneRowEdge = BitPattern.parsePattern("--------\n" 
                                                              + "--------\n"
                                                              + "--------\n"
                                                              + "--------\n"
                                                              + "--------\n"
                                                              + "--------\n"
                                                              + "----XX--\n"
                                                              + "--------\n");
//
  final static long lastRowEdge = BitPattern.parsePattern("--------\n" 
                                                        + "--------\n"
                                                        + "--------\n"
                                                        + "--------\n"
                                                        + "--------\n"
                                                        + "--------\n"
                                                        + "--------\n"
                                                        + "----XXX-\n");

  final static long lastRowCenter = BitPattern.parsePattern("--------\n" 
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "----XX--\n"
                                                          + "--------\n"
                                                          + "--------\n");
  final static long lastRowSweet = BitPattern.parsePattern("--------\n" 
                                                         + "--------\n"
                                                         + "--------\n"
                                                         + "--------\n"
                                                         + "----XX--\n"
                                                         + "--------\n"
                                                         + "--------\n"
                                                         + "--------\n");


  final static long maskCenter = BitPattern.parsePattern("--------\n" 
                                                       + "--------\n"
                                                       + "--------\n"
                                                       + "--------\n"
                                                       + "--------\n"
                                                       + "--------\n"
                                                       + "------X-\n"
                                                       + "----XXXX\n");

  final static long patternCenter = BitPattern.parsePattern("--------\n"
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "--------\n" 
                                                          + "----XX--\n"
                                                          + "----XX--\n"
                                                          + "--------\n");

  final static long maskSweet = BitPattern.parsePattern("--------\n"
                                                      + "--------\n"
                                                      + "--------\n"
                                                      + "--------\n"
                                                      + "--------\n"
                                                      + "--------\n"
                                                      + "----XXX-\n" 
                                                      + "----XXXX\n");

  final static long patternSweet = BitPattern.parsePattern("--------\n" 
                                                         + "--------\n"
                                                         + "--------\n"
                                                         + "--------\n"
                                                         + "----XX--\n"
                                                         + "----XX--\n"
                                                         + "--------\n"
                                                         + "--------\n");
  private static final int N_TRANSPOSITIONS = 4;

  /**
   * Needed to silence a warning.
   */
  private static final long serialVersionUID = 1L;
  
  protected static int halfHash(long player, long opponent) {
    long mask = player | opponent;
    return (int) (
      (mask & maskEdge) != 0 ? 
              (player & mainPartSide) | ((player & xSquareSide) >>> 5) |  
              ((opponent & mainPartSide) << 5) | (opponent & xSquareSide) |
              SideImproved.isSide :
        (mask & maskCenter) != 0 ? 
                ((player & lastRowEdge) >>> 1) | ((player & lastButOneRowEdge) >>> 7) |  
                ((opponent & lastRowEdge) << 4) | ((opponent & lastButOneRowEdge) >>> 2) |
                SideImproved.isEdge :
          (mask & maskSweet) != 0 ? 
                ((player & lastButOneRowEdge) >>> 10) | ((player & lastRowCenter) >>> 16) |  
                ((opponent & lastButOneRowEdge) >>> 6) | ((opponent & lastRowCenter) >>> 12) |
                SideImproved.isCenter :

                ((player & lastRowCenter) >>> 18) | ((player & lastRowSweet) >>> 24) |  
                ((opponent & lastRowCenter) >>> 14) | ((opponent & lastRowSweet) >>> 20) |
                  SideImproved.isSweet
    );
  }
  protected static int hash(long player, long opponent, long horizPlayer, long horizOpponent) {
    return halfHash(player, opponent) + 2560 * halfHash(horizPlayer, horizOpponent);
  }

  @Override
  public int[] hashes(Board[] transp) {
    int[] result = new int[N_TRANSPOSITIONS];
  
    for (int i = 0; i < N_TRANSPOSITIONS; i++) {
      Board t = transp[i];
      Board u = transp[i + N_TRANSPOSITIONS];
      result[i] = hash(t.getPlayer(), t.getOpponent(), u.getPlayer(), u.getOpponent());
    }
    return result;
  }

//  public final static float eval(Board r1, Board r2, Board r3, Board r4, float[] evaluators) {
//    return evaluators[hash(r1.getPlayer(), r1.getOpponent())] +
//      evaluators[hash(r2.getPlayer(), r2.getOpponent())] +
//      evaluators[hash(r3.getPlayer(), r3.getOpponent())] +
//      evaluators[hash(r4.getPlayer(), r4.getOpponent())];
//  }
  
  @Override
  public int maxSize() {
    return 6553600; // 2560 ^ 2
  }
}
