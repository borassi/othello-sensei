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

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package evaluatedepthone.patternhasher;

import bitpattern.BitPattern;
import board.Board;
import board.PossibleMovesFinderImproved;

/**
 *
 * @author michele
 */
public class StableDisksMiddle implements PatternHashInterface {
  final static long patternAll = BitPattern.parsePattern(
            "XXXXXXXX\n" 
          + "XXXXXXXX\n" 
          + "XXXXXXXX\n" 
          + "XXXXXXXX\n" 
          + "XXXXXXXX\n" 
          + "XXXXXXXX\n" 
          + "XXXXXXXX\n"
          + "XXXXXXXX\n");
  
  final static long patterns[] = PossibleMovesFinderImproved.allBitPatterns();

  @Override
  public int maxSize() {
    return 4161; // 64 * 65 + 1
  }
  
  @Override
  public int[] hashes(Board[] transp) {
    return new int[] {hash(transp[0])};
  }

  public final static int hash(Board b) {
    long player = b.getPlayer();
    long opponent = b.getOpponent();
    long all = player | opponent;
    long stable = patternAll;

    for (long pattern : patterns) {
      if ((pattern & all) != pattern) {
        stable = stable & ~pattern;
      }
    }
    return Long.bitCount(stable & player) + 65 * Long.bitCount(stable & opponent);
  }


  public final static float eval(Board r1, float[] evaluators) {
    return evaluators[hash(r1)];
  }
}
