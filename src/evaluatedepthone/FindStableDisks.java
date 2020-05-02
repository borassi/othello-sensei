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
import static bitpattern.BitPattern.FIRST_ROW_BIT_PATTERN;
import board.Board;
import static bitpattern.BitPattern.MAIN_DIAG9_BIT_PATTERN;
import static bitpattern.BitPattern.MAIN_DIAG7_BIT_PATTERN;

/**
 *
 * @author michele
 */
public class FindStableDisks {
  final static long TOP_EDGE_PATTERN = BitPattern.parsePattern(
            "XXXXXXXX\n"
          + "--------\n"
          + "--------\n"
          + "--------\n"
          + "--------\n"
          + "--------\n"
          + "--------\n" 
          + "--------\n");
  final static long LEFT_EDGE_PATTERN = BitPattern.parsePattern(
            "X-------\n"
          + "X-------\n"
          + "X-------\n"
          + "X-------\n"
          + "X-------\n"
          + "X-------\n" 
          + "X-------\n"
          + "X-------\n");
  final static long RIGHT_EDGE_PATTERN = BitPattern.parsePattern(
            "-------X\n"
          + "-------X\n"
          + "-------X\n"
          + "-------X\n"
          + "-------X\n"
          + "-------X\n" 
          + "-------X\n"
          + "-------X\n");
  final static long BOTTOM_EDGE_PATTERN = BitPattern.parsePattern(
            "--------\n"
          + "--------\n"
          + "--------\n"
          + "--------\n"
          + "--------\n"
          + "--------\n" 
          + "--------\n"
          + "XXXXXXXX\n");
  final static long BOTTOM_MINUS_ONE_EDGE_PATTERN = BitPattern.parsePattern(
            "--------\n"
          + "--------\n"
          + "--------\n"
          + "--------\n"
          + "--------\n"
          + "--------\n"
          + "XXXXXXXX\n" 
          + "--------\n");
  final static long ALL_MINUS_FIRST_COLUMN_PATTERN = BitPattern.parsePattern(
            "-XXXXXXX\n" 
          + "-XXXXXXX\n" 
          + "-XXXXXXX\n" 
          + "-XXXXXXX\n" 
          + "-XXXXXXX\n" 
          + "-XXXXXXX\n" 
          + "-XXXXXXX\n" 
          + "-XXXXXXX\n");
  final static long ALL_MINUS_FIRST2_COLUMN_PATTERN = BitPattern.parsePattern(
            "--XXXXXX\n" 
          + "--XXXXXX\n" 
          + "--XXXXXX\n" 
          + "--XXXXXX\n" 
          + "--XXXXXX\n" 
          + "--XXXXXX\n" 
          + "--XXXXXX\n" 
          + "--XXXXXX\n");
  final static long ALL_MINUS_FIRST4_COLUMN_PATTERN = BitPattern.parsePattern(
            "----XXXX\n"
          + "----XXXX\n" 
          + "----XXXX\n" 
          + "----XXXX\n" 
          + "----XXXX\n" 
          + "----XXXX\n" 
          + "----XXXX\n" 
          + "----XXXX\n");
  final static long ALL_MINUS_LAST_COLUMN_PATTERN = BitPattern.parsePattern(
            "XXXXXXX-\n" 
          + "XXXXXXX-\n" 
          + "XXXXXXX-\n" 
          + "XXXXXXX-\n" 
          + "XXXXXXX-\n" 
          + "XXXXXXX-\n" 
          + "XXXXXXX-\n" 
          + "XXXXXXX-\n");
  final static long ALL_MINUS_LAST2_COLUMN_PATTERN = BitPattern.parsePattern(
            "XXXXXX--\n" 
          + "XXXXXX--\n" 
          + "XXXXXX--\n" 
          + "XXXXXX--\n" 
          + "XXXXXX--\n" 
          + "XXXXXX--\n" 
          + "XXXXXX--\n" 
          + "XXXXXX--\n");
  final static long ALL_MINUS_LAST4_COLUMN_PATTERN = BitPattern.parsePattern(
            "XXXX----\n" 
          + "XXXX----\n" 
          + "XXXX----\n" 
          + "XXXX----\n" 
          + "XXXX----\n" 
          + "XXXX----\n" 
          + "XXXX----\n" 
          + "XXXX----\n");
  final static long CENTRAL = BitPattern.parsePattern(
            "--------\n" 
          + "-XXXXXX-\n" 
          + "-XXXXXX-\n" 
          + "-XXXXXX-\n" 
          + "-XXXXXX-\n" 
          + "-XXXXXX-\n" 
          + "-XXXXXX-\n" 
          + "--------\n");
  final long STABLE_DISKS[];
  
  private final static int hash(long player, long opponent) {
    assert(player < 256);
    assert(opponent < 256);
    return (int) (player | opponent << 8);
  }
  
  public FindStableDisks() {
    STABLE_DISKS = new long[65536]; // 2^16
    for (long player : BitPattern.allSubBitPatterns(BOTTOM_EDGE_PATTERN)) {
      for (long opponent : BitPattern.allSubBitPatterns(BOTTOM_EDGE_PATTERN)) {
        if ((player & opponent) == 0) {
          STABLE_DISKS[hash(player, opponent)] = 
            findStableDisks(player, opponent, BOTTOM_EDGE_PATTERN, false);
        }
      }
    }
  }
  
  public long getStableDisksEdges(long player, long opponent) {
    long stableBottom = STABLE_DISKS[hash(player & BOTTOM_EDGE_PATTERN, opponent & BOTTOM_EDGE_PATTERN)];
    long stableTop = STABLE_DISKS[hash(player >>> 56, opponent >>> 56)] << 56;
    long stableRight = ((STABLE_DISKS[hash(((player & RIGHT_EDGE_PATTERN) * MAIN_DIAG9_BIT_PATTERN) >>> 56,
        ((opponent & RIGHT_EDGE_PATTERN) * MAIN_DIAG9_BIT_PATTERN) >>> 56)]
        * MAIN_DIAG9_BIT_PATTERN) >>> 7) & RIGHT_EDGE_PATTERN;
    long stableLeft = (STABLE_DISKS[hash((((player & LEFT_EDGE_PATTERN) >>> 7) * MAIN_DIAG9_BIT_PATTERN) >>> 56,
        (((opponent & LEFT_EDGE_PATTERN) >>> 7) * MAIN_DIAG9_BIT_PATTERN) >>> 56)]
        * MAIN_DIAG9_BIT_PATTERN) & LEFT_EDGE_PATTERN;

    return stableBottom | stableTop | stableRight | stableLeft;
  }

  protected static long getFullRevDiags(long empty) {
    long emptyL = empty | ((empty >>> 7) & ALL_MINUS_LAST_COLUMN_PATTERN);
    emptyL = emptyL | ((emptyL >>> 14) & ALL_MINUS_LAST2_COLUMN_PATTERN);
    emptyL = emptyL | ((emptyL >>> 28) & ALL_MINUS_LAST4_COLUMN_PATTERN);

    long emptyR = empty | ((empty << 7) & ALL_MINUS_FIRST_COLUMN_PATTERN);
    emptyR = emptyR | ((emptyR << 14) & ALL_MINUS_FIRST2_COLUMN_PATTERN);
    emptyR = emptyR | ((emptyR << 28) & ALL_MINUS_FIRST4_COLUMN_PATTERN);
    
    return ~(emptyL | emptyR);
  }

  protected static long getFullDiags(long empty) {
    long emptyL = empty | ((empty << 9) & ALL_MINUS_LAST_COLUMN_PATTERN);
    emptyL = emptyL | ((emptyL << 18) & ALL_MINUS_LAST2_COLUMN_PATTERN);
    emptyL = emptyL | ((emptyL << 36) & ALL_MINUS_LAST4_COLUMN_PATTERN);

    long emptyR = empty | ((empty >>> 9) & ALL_MINUS_FIRST_COLUMN_PATTERN);
    emptyR = emptyR | ((emptyR >>> 18) & ALL_MINUS_FIRST2_COLUMN_PATTERN);
    emptyR = emptyR | ((emptyR >>> 36) & ALL_MINUS_FIRST4_COLUMN_PATTERN);
    
    return ~(emptyL | emptyR);
  }

  protected static long getFullColumns(long empty) {
    long emptyU = empty | (empty << 8);
    emptyU = emptyU | (emptyU << 16);
    emptyU = emptyU | (emptyU << 32);

    long emptyD = empty | (empty >>> 8);
    emptyD = emptyD | (emptyD >>> 16);
    emptyD = emptyD | (emptyD >>> 32);
    
    return ~(emptyU | emptyD);
  }

  protected static long getFullRows(long empty) {
    long emptyL = empty | ((empty << 1) & ALL_MINUS_LAST_COLUMN_PATTERN);
    emptyL = emptyL | ((emptyL << 2) & ALL_MINUS_LAST2_COLUMN_PATTERN);
    emptyL = emptyL | ((emptyL << 4) & ALL_MINUS_LAST4_COLUMN_PATTERN);

    long emptyR = empty | ((empty >>> 1) & ALL_MINUS_FIRST_COLUMN_PATTERN);
    emptyR = emptyR | ((emptyR >>> 2) & ALL_MINUS_FIRST2_COLUMN_PATTERN);
    emptyR = emptyR | ((emptyR >>> 4) & ALL_MINUS_FIRST4_COLUMN_PATTERN);
    
    return ~(emptyL | emptyR);
  }
  
  public long getStableDisks(long player, long opponent) {
    long stable = getStableDisksEdges(player, opponent);
    long empties = ~(player | opponent);
    long fullRows = getFullRows(empties);
    long fullColumns = getFullColumns(empties);
    long fullDiags = getFullDiags(empties);
    long fullRevDiags = getFullRevDiags(empties);
    long stablePlayer = stable & player;
    
    stable = stable | (fullRows & fullColumns & fullDiags & fullRevDiags);
    long newStable = stablePlayer;
//    System.out.println(BitPattern.patternToString(stablePlayer));
    while (newStable != 0) {
      newStable = ((stablePlayer << 1) | (stablePlayer >> 1) | fullRows);
      newStable &= ((stablePlayer << 8 | (stablePlayer >> 8) | fullColumns));
      newStable &= ((stablePlayer << 7 | (stablePlayer >> 7) | fullRevDiags));
      newStable &= ((stablePlayer << 9 | (stablePlayer >> 9) | fullDiags));
      newStable = newStable & player & CENTRAL & ~stablePlayer;
      stablePlayer |= newStable;
    }
    
    return stable | stablePlayer;
  }
  
  public int getLowerBound(long player, long opponent) {
    return 200 * Long.bitCount(getStableDisks(player, opponent) & player) - 6400;
  }
  
  public int getUpperBound(long player, long opponent) {
    return 6400 - 200 * Long.bitCount(getStableDisks(opponent, player) & opponent);
  }
  
  public long getStableDisks(Board b) {
    return getStableDisks(b.getPlayer(), b.getOpponent());
  }
  
  private static long findStableDisks(long player, long opponent, long stable, boolean passed) {
    int x, y;
    final long empties = ~(player | opponent);
    stable = stable & ~empties;

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
          stable & ~flip, false); // next move
      }
    }
    if (!passed) {
      return findStableDisks(opponent, player, stable, true);
    }
    return stable;
  }
}
