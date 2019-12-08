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

package bitpattern;

import java.util.ArrayList;
import board.Board;
import it.unimi.dsi.fastutil.longs.LongOpenHashSet;

public class BitPattern {
  public final static long FULL_BIT_PATTERN = parsePattern(
          "XXXXXXXX\n" +
          "XXXXXXXX\n" +
          "XXXXXXXX\n" +
          "XXXXXXXX\n" +
          "XXXXXXXX\n" +
          "XXXXXXXX\n" +
          "XXXXXXXX\n" +
          "XXXXXXXX\n");
  public final static long FIRST_ROW_BIT_PATTERN = parsePattern(
          "XXXXXXXX\n" +
          "--------\n" +
          "--------\n" +
          "--------\n" +
          "--------\n" +
          "--------\n" +
          "--------\n" +
          "--------\n");
  public final static long FIRST_TWO_ROWS_BIT_PATTERN = parsePattern(
          "XXXXXXXX\n" +
          "XXXXXXXX\n" +
          "--------\n" +
          "--------\n" +
          "--------\n" +
          "--------\n" +
          "--------\n" +
          "--------\n");
  public final static long FIRST_FOUR_ROWS_BIT_PATTERN = parsePattern(
          "XXXXXXXX\n" +
          "XXXXXXXX\n" +
          "XXXXXXXX\n" +
          "XXXXXXXX\n" +
          "--------\n" +
          "--------\n" +
          "--------\n" +
          "--------\n");
  public final static long LAST_ROW_BIT_PATTERN = parsePattern(
          "--------\n" +
          "--------\n" +
          "--------\n" +
          "--------\n" +
          "--------\n" +
          "--------\n" +
          "--------\n" +
          "XXXXXXXX\n");
  public final static long LAST_TWO_ROWS_BIT_PATTERN = parsePattern(
          "--------\n" +
          "--------\n" +
          "--------\n" +
          "--------\n" +
          "--------\n" +
          "--------\n" +
          "XXXXXXXX\n" +
          "XXXXXXXX\n");
  public final static long LAST_FOUR_ROWS_BIT_PATTERN = parsePattern(
          "--------\n" +
          "--------\n" +
          "--------\n" +
          "--------\n" +
          "XXXXXXXX\n" +
          "XXXXXXXX\n" +
          "XXXXXXXX\n" +
          "XXXXXXXX\n");
  public final static long FIRST_COLUMN_BIT_PATTERN = parsePattern(
          "X-------\n" +
          "X-------\n" +
          "X-------\n" +
          "X-------\n" +
          "X-------\n" +
          "X-------\n" +
          "X-------\n" +
          "X-------\n");
  public final static long FIRST_TWO_COLUMNS_BIT_PATTERN = parsePattern(
          "XX------\n" +
          "XX------\n" +
          "XX------\n" +
          "XX------\n" +
          "XX------\n" +
          "XX------\n" +
          "XX------\n" +
          "XX------\n");
  public final static long FIRST_FOUR_COLUMNS_BIT_PATTERN = parsePattern(
          "XXXX----\n" +
          "XXXX----\n" +
          "XXXX----\n" +
          "XXXX----\n" +
          "XXXX----\n" +
          "XXXX----\n" +
          "XXXX----\n" +
          "XXXX----\n");
  public final static long LAST_COLUMN_BIT_PATTERN = parsePattern(
          "-------X\n" +
          "-------X\n" +
          "-------X\n" +
          "-------X\n" +
          "-------X\n" +
          "-------X\n" +
          "-------X\n" +
          "-------X\n");
  public final static long LAST_TWO_COLUMNS_BIT_PATTERN = parsePattern(
          "------XX\n" +
          "------XX\n" +
          "------XX\n" +
          "------XX\n" +
          "------XX\n" +
          "------XX\n" +
          "------XX\n" +
          "------XX\n");
  public final static long LAST_FOUR_COLUMNS_BIT_PATTERN = parsePattern(
          "----XXXX\n" +
          "----XXXX\n" +
          "----XXXX\n" +
          "----XXXX\n" +
          "----XXXX\n" +
          "----XXXX\n" +
          "----XXXX\n" +
          "----XXXX\n");
  public final static long MAIN_DIAG_BIT_PATTERN = parsePattern(
            "X-------\n"
          + "-X------\n" 
          + "--X-----\n"
          + "---X----\n"
          + "----X---\n"
          + "-----X--\n"
          + "------X-\n"
          + "-------X\n");
  final static long MAIN_REV_DIAG_BIT_PATTERN = BitPattern.parsePattern(
            "-------X\n"
          + "------X-\n"
          + "-----X--\n"
          + "----X---\n"
          + "---X----\n" 
          + "--X-----\n"
          + "-X------\n"
          + "X-------\n");
  final static long BOTTOM_LEFT_TRIANGLE_BIT_PATTERN = parsePattern(
            "X-------\n"
          + "XX------\n"
          + "XXX-----\n"
          + "XXXX----\n"
          + "XXXXX---\n"
          + "XXXXXX--\n"
          + "XXXXXXX-\n"
          + "XXXXXXXX\n");
  final static long TOP_RIGHT_TRIANGLE_BIT_PATTERN = ~BOTTOM_LEFT_TRIANGLE_BIT_PATTERN;
  final static long BOTTOM_RIGHT_TRIANGLE_BIT_PATTERN = parsePattern(
            "--------\n"
          + "-------X\n" 
          + "------XX\n"
          + "-----XXX\n"
          + "----XXXX\n"
          + "---XXXXX\n"
          + "--XXXXXX\n"
          + "-XXXXXXX\n");
  final static long TOP_LEFT_TRIANGLE_BIT_PATTERN = ~BOTTOM_RIGHT_TRIANGLE_BIT_PATTERN;


  public final static int getEvaluationGameOver(Board b) {
    return getEvaluationGameOver(b.getPlayer(), b.getOpponent());
  }
  
  public final static int getEvaluationBoardFull(long player) {
    return 100 * (2 * Long.bitCount(player) - 64);
  }
  
  public final static int getEvaluationBoardFull(Board b) {
    return 100 * (2 * Long.bitCount(b.getPlayer()) - 64);
  }
  
  
  public final static int getEvaluationGameOver(long player, long opponent) {
    int playerDisks = Long.bitCount(player);
    int opponentDisks = Long.bitCount(opponent);

    int diff = playerDisks - opponentDisks;
    return 100 * (diff + (int) Math.signum(diff) * (64 - playerDisks - opponentDisks));
  }
  
  /**
   * Parses a string pattern into a 64-bit long, as follows: 
   * - it removes any newline. 
   * - it sets '1' to the bits corresponding to the input
   *   variable 'letter', '0' to all the other bits.
   * 
   * @param pattern
   *          the string pattern
   * @param letter
   *          the letter to be used
   * @return the converted string
   */
  public final static long parsePattern(String pattern, char letter) {
    long result = 0;
    pattern = pattern.replace("\n", "");
    for (int i = 0; i < 64; i++) {
      result = result << 1;
      if (pattern.charAt(i) == letter) {
        result = result | 1;
      }
    }
    return result;
  }

  /**
   * See parsePattern(String pattern, char letter).
   * 
   * @param pattern
   * @return
   */
  public final static long parsePattern(String pattern) {
    return parsePattern(pattern, 'X');
  }

  /**
   * Converts a pattern to a string, by putting 'X' where the corresponding
   * bit is 1, '-' where the corresponding bit is 0
   * @param pattern the bit pattern
   * @return the string version of the pattern.
   */
  public final static String patternToString(long pattern) {
    String result = "";
    for (int i = 0; i < 64; i++) {
      if (pattern >>> 63 == 1) {
        result += "X";
      } else {
        result += "-";
      }
      pattern = pattern << 1;
      if (i % 8 == 7) {
        result += "\n";
      }
    }
    return result;
  }

  /**
   * Mirrors the unsigned long long (exchange the line 1 - 8, 2 - 7, 3 - 6 & 4 - 5).
   * @param b An unsigned long long.
   * @return The mirrored unsigned long long.
   */
  public final static long horizontalMirror(long b) {
    b = ((b >> 1) & 0x5555555555555555L) | ((b << 1) & 0xAAAAAAAAAAAAAAAAL);
    b = ((b >> 2) & 0x3333333333333333L) | ((b << 2) & 0xCCCCCCCCCCCCCCCCL);
    b = ((b >> 4) & 0x0F0F0F0F0F0F0F0FL) | ((b << 4) & 0xF0F0F0F0F0F0F0F0L);

    return b;
  }

  /**
   * Mirrors the unsigned long long (exchange the columns 1 - 8, 2 - 7, 3 - 6 & 4 - 5).
   * @param b An unsigned long long.
   * @return The mirrored unsigned long long.
   */
  public final static long verticalMirror(long b) {
    b = ((b >>  8) & 0x00FF00FF00FF00FFL) | ((b <<  8) & 0xFF00FF00FF00FF00L);
    b = ((b >> 16) & 0x0000FFFF0000FFFFL) | ((b << 16) & 0xFFFF0000FFFF0000L);
    b = ((b >> 32) & 0x00000000FFFFFFFFL) | ((b << 32) & 0xFFFFFFFF00000000L);
    return b;
  }

  /**
   * Transposes the unsigned long long (symmetry % A1-H8 diagonal).
   * @param b An unsigned long long.
   * @return The mirrored unsigned long long.
   */
  public final static long transpose(long b) {
    long t;
  
    t = (b ^ (b >> 7)) & 0x00aa00aa00aa00aaL;
    b = b ^ t ^ (t << 7);
    t = (b ^ (b >> 14)) & 0x0000cccc0000ccccL;
    b = b ^ t ^ (t << 14);
    t = (b ^ (b >> 28)) & 0x00000000f0f0f0f0L;
    b = b ^ t ^ (t << 28);
  
    return b;
  }
  
  public final static LongOpenHashSet allTranspositions(long b) {
    LongOpenHashSet result = new LongOpenHashSet();
    for (int i = 0; i < 2; ++i) {
      for (int j = 0; j < 4; ++j) {
        result.add(b);
        b = transpose(horizontalMirror(b));
      }
      b = horizontalMirror(b);
    }
    return result;
  }
  
  public final static long moveToBit(PositionIJ ij) {
    return moveToBit(ij.i, ij.j);
  }
  
  public final static long moveToBit(int i, int j) {
    return 1L << (63 - (i * 8 + j));
  }
  
  public final static PositionIJ bitToMove(long bitPattern) {
    int pos = 63 - Long.numberOfTrailingZeros(bitPattern);
    return new PositionIJ(pos / 8, pos % 8);
  }
  

  /**
   * Computes all the bitpatterns corresponding to rows (0-7), columns (8-15),
   * NW-SE diagonals of length at least 3 (16-26), NE-SW diagonals (27-37).
   * @return the computed bitpatterns, as an array.
   */
  public final static long[] interestingBitPatterns() {
    ArrayList<Long> bitPatterns = new ArrayList<Long>();

    // Rows are obtained by shifting the first row.
    for (int i = 0; i < 8; i++) {
      bitPatterns.add(FIRST_ROW_BIT_PATTERN >>> (8 * i));
    }
    // Columns are obtained by transposing rows.
    for (int i = 0; i < 8; i++) {
      bitPatterns.add(BitPattern.transpose(FIRST_ROW_BIT_PATTERN >>> (8 * i)));
    }
    // NW-SE diagonals are obtained by shifting and adding a bit, apart from the longest
    long curDiag = BitPattern.parsePattern(
              "--------\n"
            + "--------\n" 
            + "--------\n"
            + "--------\n"
            + "--------\n"
            + "X-------\n"
            + "-X------\n"
            + "--X-----\n");
    for (int i = 1; i < 6; i++) {
      bitPatterns.add(curDiag);
      bitPatterns.add(BitPattern.horizontalMirror(curDiag));
      bitPatterns.add(BitPattern.transpose(curDiag));
      bitPatterns.add(BitPattern.horizontalMirror(BitPattern.transpose(curDiag)));
      curDiag = curDiag >>> 1 | (1L << (8 * i + 23));
    }
    // We add the longest diagonals
    bitPatterns.add(curDiag);
    bitPatterns.add(BitPattern.horizontalMirror(curDiag));
    
    // We copy the ArrayList into the array.
    long[] result = new long[bitPatterns.size()];
    for (int i = 0; i < bitPatterns.size(); i++) {
      result[i] = bitPatterns.get(i);
    }
    return result;
  }
  
  /**
   * Returns all the possible bitpatterns that are empty outside the bitPattern
   * provided.
   * @param bitPattern the bit pattern.
   * @return an ArrayList with all the possible patterns.
   */
  public final static ArrayList<Long> allSubBitPatterns(long bitPattern) {
    return allSubBitPatterns(bitPattern, 0, 0);
  }
  
  /**
   * Recursive version of existingBoardsInBitPattern(long bitPattern).
   * @param bitPattern the bit pattern.
   * @param startingBoard the cases already filled.
   * @param currentCase the next case to be filled.
   * @return an ArrayList with all the possible boards.
   */
  private final static ArrayList<Long> allSubBitPatterns(
          long bitPattern, long startingPattern, int currentCase) {
    if (currentCase >= 64) {
      ArrayList<Long> b = new ArrayList<>();
      b.add(startingPattern);
      return b;
    }

    ArrayList<Long> result = allSubBitPatterns(bitPattern, startingPattern, currentCase + 1);
    
    if ((bitPattern & (1L << (63 - currentCase))) == 0) {
      return result;
    }

    long currentPattern = startingPattern |
            BitPattern.moveToBit(currentCase / 8, currentCase % 8);
    result.addAll(allSubBitPatterns(bitPattern, currentPattern, currentCase + 1));
    return result;
  }
  
  public final static int anyDiagonalToFirstRow(long bitPattern) {
    return (int) (((bitPattern * LAST_COLUMN_BIT_PATTERN) & FIRST_ROW_BIT_PATTERN) >>> 56);
  }
  
  public final static int anyColumnToFirstRow(long bitPattern, int i) {
    return (int) ((((bitPattern >>> i) * MAIN_DIAG_BIT_PATTERN) & FIRST_ROW_BIT_PATTERN) >>> 56);
  }
  
  public final static int anyRowToFirstRow(long bitPattern, int i) {
    return (int) (bitPattern >>> (i * 8));
  }
  
  public final static long getRow(int position) {
    return (1L << ((position / 8) * 8)) * LAST_ROW_BIT_PATTERN;
  }
  
  public final static long getColumn(int position) {
    return (1L << (position % 8)) * LAST_COLUMN_BIT_PATTERN;
  }
  public final static long getNWSEDiag(int position) {
    int mainDiagShift = (position - 9 * (position % 8));
    return mainDiagShift > 0 ? 
      MAIN_DIAG_BIT_PATTERN << mainDiagShift : 
      MAIN_DIAG_BIT_PATTERN >>> -mainDiagShift;
    
//    long mask = (position / 8 == position / 9) ? BOTTOM_LEFT_TRIANGLE_BIT_PATTERN : TOP_RIGHT_TRIANGLE_BIT_PATTERN;
//    return ((1L << (position % 9)) * MAIN_DIAG_BIT_PATTERN) & mask;
  }
  public final static long getNESWDiag(int position) {
    int mainDiagShift = (position + 7 * (position % 8) - 56);
    return mainDiagShift > 0 ? 
      MAIN_REV_DIAG_BIT_PATTERN << mainDiagShift : 
      MAIN_REV_DIAG_BIT_PATTERN >>> -mainDiagShift;
  }
}
