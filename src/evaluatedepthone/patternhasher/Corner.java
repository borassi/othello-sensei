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

public class Corner implements PatternHashInterface {
  public static final String sides[] = {"Bottom", "Right", "Top", "Left", "Bottom inv", "Left inv", "Right inv", "Top inv"};

  final static long IS_HALF = BitPattern.parsePattern("--------\n" 
                                                    + "--------\n"
                                                    + "--------\n"
                                                    + "--------\n"
                                                    + "--------\n"
                                                    + "-X------\n"
                                                    + "--------\n"
                                                    + "--------\n");

  final static long IS_INNER = BitPattern.parsePattern("--------\n" 
                                                     + "--------\n"
                                                     + "--------\n"
                                                     + "--------\n"
                                                     + "--------\n"
                                                     + "X-------\n"
                                                     + "--------\n"
                                                     + "--------\n");

  final static long PATTERN_CORNER = BitPattern.parsePattern("--------\n" 
                                                           + "--------\n"
                                                           + "--------\n"
                                                           + "-------X\n"
                                                           + "-------X\n"
                                                           + "-------X\n"
                                                           + "------XX\n"
                                                           + "---XXXXX\n");

  final static long MASK_HALF = BitPattern.parsePattern("--------\n" 
                                                      + "--------\n"
                                                      + "--------\n"
                                                      + "--------\n"
                                                      + "--------\n"
                                                      + "--------\n"
                                                      + "------X-\n"
                                                      + "-------X\n");

  final static long PATTERN_HALF = BitPattern.parsePattern("--------\n" 
                                                         + "--------\n"
                                                         + "--------\n"
                                                         + "-------X\n"
                                                         + "-------X\n"
                                                         + "-----XXX\n"
                                                         + "-----X-X\n"
                                                         + "---XXXX-\n");

  final static long MASK_INNER = BitPattern.parsePattern("--------\n" 
                                                       + "--------\n"
                                                       + "--------\n"
                                                       + "--------\n"
                                                       + "--------\n"
                                                       + "--------\n"
                                                       + "------XX\n"
                                                       + "------XX\n");

  final static long PATTERN_INNER = BitPattern.parsePattern("--------\n" 
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "-------X\n"
                                                          + "------XX\n"
                                                          + "-----XXX\n"
                                                          + "----XX--\n"
                                                          + "---XXX--\n");

  private static final int N_TRANSPOSITIONS = 4;

  /**
   * Needed to silence a warning.
   */
  private static final long serialVersionUID = 1L;
  
  
  static final int hashInner(long player, long opponent) {
    player = player & PATTERN_INNER;
    opponent = opponent & PATTERN_INNER;
    long resultPlayer = (player & BitPattern.LAST_TWO_ROWS_BIT_PATTERN) >>> 2;
    resultPlayer |= (player & (BitPattern.LAST_ROW_BIT_PATTERN << 16)) >>> 13;
    resultPlayer |= (player & (BitPattern.LAST_ROW_BIT_PATTERN << 24)) >>> 18;
    resultPlayer |= (player >> 32) << 10;
    long resultOpponent = (opponent & BitPattern.LAST_TWO_ROWS_BIT_PATTERN) >>> 2;
    resultOpponent |= (opponent & (BitPattern.LAST_ROW_BIT_PATTERN << 16)) >>> 13;
    resultOpponent |= (opponent & (BitPattern.LAST_ROW_BIT_PATTERN << 24)) >>> 18;
    resultOpponent |= (opponent >> 32) << 10;
    return (int) (resultPlayer | (resultOpponent << 11) | IS_INNER);
  }

  static final int hashCorner(long player, long opponent) {
    player = player & PATTERN_CORNER;
    opponent = opponent & PATTERN_CORNER;
    long resultPlayer = player & BitPattern.LAST_TWO_ROWS_BIT_PATTERN;
    player = player & ~BitPattern.LAST_TWO_ROWS_BIT_PATTERN;
    resultPlayer |= (player * BitPattern.MAIN_DIAG9_BIT_PATTERN) >>> 54;
    long resultOpponent = opponent & BitPattern.LAST_TWO_ROWS_BIT_PATTERN;
    opponent = opponent & ~BitPattern.LAST_TWO_ROWS_BIT_PATTERN;
    resultOpponent |= (opponent * BitPattern.MAIN_DIAG9_BIT_PATTERN) >>> 54;
    return (int) (resultPlayer | (resultOpponent << 10));
  }
  
  static final int hashHalf(long player, long opponent) {
    player = player & PATTERN_HALF;
    opponent = opponent & PATTERN_HALF;
    long resultPlayer = player & BitPattern.LAST_TWO_ROWS_BIT_PATTERN;
    resultPlayer |= (player & (BitPattern.LAST_ROW_BIT_PATTERN << 16)) >>> 11;
    resultPlayer |= (player >> 24) & BitPattern.LAST_ROW_BIT_PATTERN;
    resultPlayer |= (player >> 32) << 9;
    long resultOpponent = opponent & BitPattern.LAST_TWO_ROWS_BIT_PATTERN;
    resultOpponent |= (opponent & (BitPattern.LAST_ROW_BIT_PATTERN << 16)) >>> 11;
    resultOpponent |= (opponent >> 24) & BitPattern.LAST_ROW_BIT_PATTERN;
    resultOpponent |= (opponent >> 32) << 9;
    return (int) (resultPlayer | (resultOpponent << 11) | IS_HALF);
  }
  
  static final int hash(long player, long opponent) {
    long mask = player | opponent;
    if ((mask & MASK_HALF) != 0) {
      return hashCorner(player, opponent);
    }
    if ((mask & MASK_INNER) != 0) {
      return hashHalf(player, opponent);
    }
    return hashInner(player, opponent);
  }

  int[] HASHES = new int[N_TRANSPOSITIONS];
  @Override
  public int[] hashes(Board[] transp) {
  
    for (int i = 0; i < N_TRANSPOSITIONS; i++) {
      Board t = transp[i];
      HASHES[i] = hash(t.getPlayer(), t.getOpponent());
    }
    return HASHES;
  }

  public final static float eval(Board r1, Board r2, Board r3, Board r4, float[] evaluators) {
    return evaluators[hash(r1.getPlayer(), r1.getOpponent())] +
      evaluators[hash(r2.getPlayer(), r2.getOpponent())] +
      evaluators[hash(r3.getPlayer(), r3.getOpponent())] +
      evaluators[hash(r4.getPlayer(), r4.getOpponent())];
  }
  
  @Override
  public int maxSize() {
    return 16777216; // SHOULD BE 2^24 (MAYBE LESS) NOW IS 2^20
  }
}
