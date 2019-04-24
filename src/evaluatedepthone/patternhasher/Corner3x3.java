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

public class Corner3x3 implements PatternHashInterface {
  final static long pattern = BitPattern.parsePattern(
          "--------\n"
        + "--------\n"
        + "--------\n"
        + "--------\n"
        + "--------\n"
        + "-----XXX\n"
        + "-----XXX\n"
        + "-----XXX\n");
  final static int toMove1Pattern = (int) BitPattern.parsePattern(
          "--------\n"
        + "--------\n"
        + "--------\n"
        + "--------\n"
        + "--------\n"
        + "--XX----\n" 
        + "--------\n"
        + "--------\n");
  final static int toMove2Pattern = (int) BitPattern.parsePattern(
          "--------\n"
        + "--------\n"
        + "--------\n"
        + "--------\n"
        + "--------\n"
        + "----XX--\n" 
        + "--------\n"
        + "--------\n");
  final static int finalMask = (int) BitPattern.parsePattern(
          "--------\n"
        + "--------\n"
        + "--------\n"
        + "--------\n"
        + "--------\n"
        + "------XX\n" 
        + "XXXXXXXX\n"
        + "XXXXXXXX\n");

  /**
   * Needed to silence a warning.
   */
  private static final long serialVersionUID = 1L;
  private static final int N_TRANSPOSITIONS = 4;
  
  private static final int[] HASHES = new int[N_TRANSPOSITIONS];
  
  static final int hash(long player, long opponent) {
    int firstHash = (int) ((player & pattern) | ((opponent & pattern) << 3));
    return (firstHash | 
            ((firstHash & toMove1Pattern) >>> 6) | 
            ((firstHash & toMove2Pattern) >>> 12)) & finalMask;
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
    return 262144; // 2^18
  }

  public final static float eval(Board r1, Board r2, Board r3, Board r4, float[] evaluators) {
    return evaluators[hash(r1.getPlayer(), r1.getOpponent())] +
      evaluators[hash(r2.getPlayer(), r2.getOpponent())] +
      evaluators[hash(r3.getPlayer(), r3.getOpponent())] +
      evaluators[hash(r4.getPlayer(), r4.getOpponent())];
  }
}
