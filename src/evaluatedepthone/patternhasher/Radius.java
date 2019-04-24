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
 * @author michele
 * This class tries to compute the "radius" of the position, which measures how
 * much influence a player has.
 * The result is the sum of the following:
 * - the number of empty squares that are reachable from 0 directions.
 * - 2^4 * the number of empty squares that are reachable from 1 direction.
 * - 2^8 * the number of empty squares that are reachable from 2 directions.
 * - 2^12 * the number of empty squares that are reachable from 3 directions.
 * - 2^16 * the number of empty squares that are reachable from 4 directions.
 * All the numbers are capped at 15.
 * We exclude the 16 corner cases.
 */
public class Radius implements PatternHashInterface {
  final static long NON_CORNERS = BitPattern.parsePattern("--XXXX--\n"
                                                        + "--XXXX--\n"
                                                        + "XXXXXXXX\n"
                                                        + "XXXXXXXX\n"
                                                        + "XXXXXXXX\n"
                                                        + "XXXXXXXX\n"
                                                        + "--XXXX--\n"
                                                        + "--XXXX--\n");
  @Override
  public int maxSize() {
    return 2097152; // 2 ^ 21.
  }

  public final static int getHash(long player, long opponent) {
    long mask = (~(player | opponent)) & NON_CORNERS;

    long horiz = extendHorizontal(player);
    long vert = extendVertical(player);
    long diag1 = extendDiag1(player);
    long diag2 = extendDiag2(player);
    
    long zeros = (~(horiz | vert | diag1 | diag2)) & mask;
    long fours = horiz & vert & diag1 & diag2 & mask;
    long oneAndThrees = (horiz ^ vert ^ diag1 ^ diag2) & mask;
    // Among the oneAndThrees, (horiz | vert) & (diag1 | diag2) is true iff
    // the bit appears three times (if it appears once, LHS or RHS is false,
    // if it appears 3 times, they are both true).
    long threes = oneAndThrees & (horiz | vert) & (diag1 | diag2);

    int zero = Long.bitCount(zeros);
    int oneAndThree = Long.bitCount(oneAndThrees);
    int three = Long.bitCount(threes);
    int one = Long.bitCount(oneAndThrees) - three;
    int four = Long.bitCount(fours);
    int two = Long.bitCount(mask) - zero - four - oneAndThree;
//    
//    System.out.println(zero + " " + one + " " + two + " " + three + " " + four);
    
    return Math.min(zero, 15) + 16 * Math.min(one, 15) + 256 * Math.min(two, 15)
      + 4096 * Math.min(three, 15) + 65536 * Math.min(four, 15);
  }
  
  @Override
  public int[] hashes(Board[] transp) {
    Board b = transp[0];
    long player = b.getPlayer();
    long opponent = b.getOpponent();
    return new int[] {getHash(player, opponent), 1048576 + getHash(opponent, player)};
  }

  protected final static long extendVertical(long player) {
    player = player | (player >>> 8);
    player = player | (player >>> 16);
    player = player | (player >>> 32);
    player = player | (player << 8);
    player = player | (player << 16);
    player = player | (player << 32);
    return player;
  }


  protected final static long extendHorizontal(long player) {
    player = player | (player >> 1 & ~BitPattern.FIRST_COLUMN_BIT_PATTERN);
    player = player | (player >> 2 & ~BitPattern.FIRST_TWO_COLUMNS_BIT_PATTERN);
    player = player | (player >> 4 & ~BitPattern.FIRST_FOUR_COLUMNS_BIT_PATTERN);
    player = player | (player << 1 & ~BitPattern.LAST_COLUMN_BIT_PATTERN);
    player = player | (player << 2 & ~BitPattern.LAST_TWO_COLUMNS_BIT_PATTERN);
    player = player | (player << 4 & ~BitPattern.LAST_FOUR_COLUMNS_BIT_PATTERN);
    return player;
  }

  protected final static long extendDiag1(long player) {
    player = player | (player >>> 9 & ~BitPattern.FIRST_COLUMN_BIT_PATTERN);
    player = player | (player >>> 18 & ~BitPattern.FIRST_TWO_COLUMNS_BIT_PATTERN);
    player = player | (player >>> 36 & ~BitPattern.FIRST_FOUR_COLUMNS_BIT_PATTERN);
    player = player | (player << 9 & ~BitPattern.LAST_COLUMN_BIT_PATTERN);
    player = player | (player << 18 & ~BitPattern.LAST_TWO_COLUMNS_BIT_PATTERN);
    player = player | (player << 36 & ~BitPattern.LAST_FOUR_COLUMNS_BIT_PATTERN);
    return player;
  }

  protected final static long extendDiag2(long player) {
    player = player | (player << 7 & ~BitPattern.FIRST_COLUMN_BIT_PATTERN);
    player = player | (player << 14 & ~BitPattern.FIRST_TWO_COLUMNS_BIT_PATTERN);
    player = player | (player << 28 & ~BitPattern.FIRST_FOUR_COLUMNS_BIT_PATTERN);
    player = player | (player >>> 7 & ~BitPattern.LAST_COLUMN_BIT_PATTERN);
    player = player | (player >>> 14 & ~BitPattern.LAST_TWO_COLUMNS_BIT_PATTERN);
    player = player | (player >>> 28 & ~BitPattern.LAST_FOUR_COLUMNS_BIT_PATTERN);
    return player;
  }
  
  public static void main(String[] args) {
    // Used to test if the patterns are correct.
    final String testBoardString = "--------\n"
                                 + "--------\n" 
                                 + "--------\n"
                                 + "--------\n"
                                 + "--------\n"
                                 + "--------\n"
                                 + "--------\n"
                                 + "-------X\n";
    final long bp = BitPattern.parsePattern(testBoardString);
    System.out.println(BitPattern.patternToString(extendHorizontal(bp)));
    System.out.println(BitPattern.patternToString(extendVertical(bp)));
    System.out.println(BitPattern.patternToString(extendDiag1(bp)));
    System.out.println(BitPattern.patternToString(extendDiag2(bp)));
  }

  public final static float eval(Board b, float[] evaluators) {
    long player = b.getPlayer();
    long opponent = b.getOpponent();
    return evaluators[getHash(player, opponent)] + evaluators[1048576 + getHash(opponent, player)];
  }
}
