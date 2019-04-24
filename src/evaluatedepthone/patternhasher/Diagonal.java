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

public class Diagonal implements PatternHashInterface {
  final static long pattern = BitPattern.parsePattern(
          "X-------\n"
        + "-X------\n"
        + "--X-----\n"
        + "---X----\n"
        + "----X---\n"
        + "-----X--\n" 
        + "------X-\n"
        + "-------X\n");
  private static final int N_TRANSPOSITIONS = 2;
  
  private static final int hashes[] = new int[N_TRANSPOSITIONS];

  /**
   * Needed to silence a warning.
   */
  private static final long serialVersionUID = 1L;
  
  static final int hash(long player, long opponent) {
    return BitPattern.anyDiagonalToFirstRow(player & pattern) | 
            (BitPattern.anyDiagonalToFirstRow(opponent & pattern) << 8);
  }
    
  @Override
  public int[] hashes(Board[] transp) {  
    for (int i = 0; i < N_TRANSPOSITIONS; i++) {
      Board t = transp[i];
      hashes[i] = hash(t.getPlayer(), t.getOpponent());
    }
    return hashes;
  }

  public final static float eval(Board r1, Board r2, float[] evaluators) {
    return evaluators[hash(r1.getPlayer(), r1.getOpponent())] +
      evaluators[hash(r2.getPlayer(), r2.getOpponent())];
  }
  
  @Override
  public int maxSize() {
    return 65536; // 2^16
  }
}
