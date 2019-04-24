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

/**
 *
 * @author michele
 */
public class StableDisks implements PatternHashInterface {
  final static long pattern = BitPattern.parsePattern(
            "--------\n"
          + "--------\n"
          + "--------\n"
          + "--------\n"
          + "--------\n"
          + "--------\n" 
          + "--------\n"
          + "XXXXXXXX\n");
  
  public final static long[] stableDisks = new long[65536];

  public static int hash(long player, long opponent) {
    return (int) ((player & pattern) + 256 * (opponent & pattern));
  }
  
  public StableDisks() {
//    stableDisks = new long[65536]; // 2^16
    for (long player : BitPattern.allSubBitPatterns(pattern)) {
      for (long opponent : BitPattern.allSubBitPatterns(pattern)) {
        if ((player & opponent) == 0) {
          stableDisks[hash(player, opponent)] = 
            findStableDisks(player, opponent, pattern, true);
        }
      }
    }
  }
  
  private static long findStableDisks(long player, long opponent, long stable, boolean allowPass) {
    int x, y;
    final long empties = ~(player | opponent);

    if (stable == 0 || empties == 0) {
      return stable;
    }

    for (x = 0; x < 8; ++x) {
      long move = 1 << x;
      if ((empties & move) != 0) { // move is an empty square
        long flip = move;
        if (x > 1) { // flip left discs
          long curFlip = 0;
          for (y = x - 1; y >= 0; --y) {
            long currentCase = 1 << y;
            if ((currentCase & opponent) != 0) {
              curFlip |= currentCase;
            } else if ((currentCase & player) != 0) {
              flip |= curFlip;
              break;
            } else {
              break;
            }
          }
        }
        if (x < 6) { // flip left discs
          long curFlip = 0;
          for (y = x + 1; y < 8; ++y) {
            long currentCase = 1 << y;
            if ((currentCase & opponent) != 0) {
              curFlip |= currentCase;
            } else if ((currentCase & player) != 0) {
              flip |= curFlip;
              break;
            } else {
              break;
            }
          }
        }
        stable = findStableDisks(opponent & ~flip, player | flip, 
          stable & ~flip, true); // next move
      }
    }
    if (allowPass) {
      return findStableDisks(opponent, player, stable, false);
    }
    return stable;
  }
  @Override
  public int maxSize() {
    return 4161; // 64 * 65 + 1
  }

  @Override
  public int[] hashes(Board[] transp) {
    return new int[] {hash(transp)};
  }
  
  public final static int hash(Board[] transp) {
    long stable = 0;
    long player = transp[0].getPlayer();
    long opponent = transp[0].getOpponent();
    stable = stableDisks[hash(player, opponent)];
    
    player = BitPattern.verticalMirror(player);
    opponent = BitPattern.verticalMirror(opponent);
    stable = BitPattern.verticalMirror(stable) | stableDisks[hash(player, opponent)];
    
    player = BitPattern.transpose(player);
    opponent = BitPattern.transpose(opponent);
    stable = BitPattern.transpose(stable) | stableDisks[hash(player, opponent)];
    
    player = BitPattern.verticalMirror(player);
    opponent = BitPattern.verticalMirror(opponent);
    stable = BitPattern.verticalMirror(stable) | stableDisks[hash(player, opponent)];
    
    return Long.bitCount(stable & player) + 65 * Long.bitCount(stable & opponent);
  }

  public final static float eval(Board[] transp, float[] evaluators) {
    return evaluators[hash(transp)];
  }
}
