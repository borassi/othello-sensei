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

public class Corner6x2 implements PatternHashInterface {
//  final static long X_CORNER = BitPattern.parsePattern(
//            "--------\n"
//          + "--------\n"
//          + "--------\n"
//          + "--------\n"
//          + "--------\n"
//          + "--------\n" 
//          + "------X-\n"
//          + "-------X\n");
  final static long LAST_ROW_BIT_PATTERN = BitPattern.parsePattern(
            "--------\n"
          + "--------\n"
          + "--------\n"
          + "--------\n"
          + "--------\n"
          + "--------\n" 
          + "--------\n"
          + "--XXXXXX\n");
  final static long LAST_BUT_ONE_ROW_BIT_PATTERN = BitPattern.parsePattern(
            "--------\n"
          + "--------\n"
          + "--------\n"
          + "--------\n"
          + "--------\n"
          + "--------\n" 
          + "--XXXXXX\n"
          + "--------\n");
  final static long pattern = BitPattern.parsePattern(
            "--------\n"
          + "--------\n"
          + "--------\n"
          + "--------\n"
          + "--------\n"
          + "--------\n" 
          + "--XXXXXX\n"
          + "--XXXXXX\n");
  private static final int N_TRANSPOSITIONS = 8;

  /**
   * Needed to silence a warning.
   */
  private static final long serialVersionUID = 1L;
  
  private static final int[] HASHES = new int[N_TRANSPOSITIONS];

  static final int hash(long player, long opponent) {
//    if (((player | opponent) & X_CORNER) == 0) {
//      return 0;
//    }
    return (int) (
      (player & LAST_ROW_BIT_PATTERN) |
      (opponent & LAST_ROW_BIT_PATTERN) << 6 |
      (player & LAST_BUT_ONE_ROW_BIT_PATTERN) << 4 |
      (opponent & LAST_BUT_ONE_ROW_BIT_PATTERN) << 10);
  }
  
  @Override
  public int[] hashes(Board[] transp) {  
    for (int i = 0; i < N_TRANSPOSITIONS; i++) {
      Board t = transp[i];
      HASHES[i] = hash(t.getPlayer(), t.getOpponent());
    }
    return HASHES;
  }

  @Override
  public int maxSize() {
    return 16777216; // 2^24
  }

  public final static float eval(Board[] allTransp, float[] evaluators) {
    float result = 0;
    for (Board transp : allTransp) {
      result += evaluators[hash(transp.getPlayer(), transp.getOpponent())];
    }
    return result;
  }
}
