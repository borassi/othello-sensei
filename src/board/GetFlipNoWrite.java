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
package board;

import bitpattern.BitPattern;
import static bitpattern.BitPattern.LAST_ROW_BIT_PATTERN;
import bitpattern.BitPatternInt;
import static board.GetFlip.neighbors;

/**
 *
 * @author michele
 */
public class GetFlipNoWrite {
  private static int sideMask = BitPatternInt.parsePattern(
      "-XXXXXX-" +
      "-XXXXXX-" +
      "-XXXXXX-" +
      "-XXXXXX-");
  private static int getDisksRightOfCells6(int disks, int cells) {
    int result = disks & (cells >>> 1);
    result |= disks & (result >>> 1);
    disks = disks & (disks >>> 1);
    result |= disks & (result >>> 2);
    result |= disks & (result >>> 2);
    return result;
  }
  private static int getDisksLeftOfCells6(int disks, int cells) {
    int result = disks & (cells << 1);
    result |= disks & (result << 1);
    disks = disks & (disks << 1);
    result |= disks & (result << 2);
    result |= disks & (result << 2);
    return result;
  }
  
  private static int transform(long bitPattern, long row, long column, long diag7, long diag9, int rowShift, int colShift) {
    int transformed = 0;
    transformed |= ((int) ((bitPattern & row) >>> rowShift));
    transformed |= 
        (((bitPattern & diag7) * BitPattern.LAST_COLUMN_BIT_PATTERN) & BitPattern.FIRST_ROW_BIT_PATTERN) >>> 48; // 255 * 256
    transformed |= 
        (((bitPattern & diag9) * BitPattern.LAST_COLUMN_BIT_PATTERN) & BitPattern.FIRST_ROW_BIT_PATTERN) >>> 40; // 255 * 256 * 256
    transformed |= 
        (((((bitPattern & column) >>> colShift)
        * BitPattern.MAIN_DIAG9_BIT_PATTERN)
        & BitPattern.FIRST_ROW_BIT_PATTERN)
        >>> 32);
    return transformed;
  }
  
  public static long getFlip(int move, long player, long opponent) {
    if ((((1L << move) & (player | opponent)) != 0) || ((GetFlip.neighbors[move] & opponent) == 0)) {
      return 0;
    }
    long result = 0;
    
    int rowShift = (move & -8);
    int colShift = (move & 7);
//    long rowNumber = move & 7;
    long row = BitPattern.LAST_ROW_BIT_PATTERN << rowShift;
    long column = BitPattern.LAST_COLUMN_BIT_PATTERN << colShift;
    long diag7 = BitPattern.getDiag7(move);
    long diag9 = BitPattern.getDiag9(move);
    int transformedMove = (65793 << colShift) + (16777216 << (7 - (rowShift >>> 3)));
    
    int transformedOpponent = transform(opponent, row, column, diag7, diag9, rowShift, colShift) & sideMask;
    int transformedPlayer = transform(player, row, column, diag7, diag9, rowShift, colShift);

    int flipped =
        (getDisksRightOfCells6(transformedOpponent, transformedPlayer) & getDisksLeftOfCells6(transformedOpponent, transformedMove))
        | (getDisksLeftOfCells6(transformedOpponent, transformedPlayer) & getDisksRightOfCells6(transformedOpponent, transformedMove));
//    }
//    System.out.println("Move: " + move + "\n" + new Board(player, opponent));
//    System.out.println(new Board(transformedPlayer, transformedOpponent));
//    System.out.println("Flipped\n" + BitPatternInt.patternToString(flipped));
//    flipped |= getDisksLeftOfCells6(transformedOpponent, transformedPlayer) & getDisksRightOfCells6(transformedOpponent, transformedMove);
    // TURN FLIPPER INTO REALLY FLIPPED.

    result |= ((long) (flipped & 255)) << rowShift;
    result |= (((flipped & 65280) >> 8) * BitPattern.LAST_COLUMN_BIT_PATTERN) & diag7;
    result |= (((flipped & 16711680) >> 16) * BitPattern.LAST_COLUMN_BIT_PATTERN) & diag9;
    result |= ((((flipped & -16777216) >> 24) * BitPattern.MAIN_DIAG9_BIT_PATTERN) & BitPattern.FIRST_COLUMN_BIT_PATTERN) >>> (7-colShift);
//    System.out.println(rowShift);
//    System.out.println("Moved\n" + BitPattern.patternToString((flipped & 255) << rowShift));
//    System.out.println("Result\n" + BitPattern.patternToString(result));
    
    return result == 0 ? result : (result | (1L << move));
  }
  
  public static long getFlipLast(int move, long player, long opponent) {
    if ((GetFlip.neighbors[move] & opponent) == 0) {
      return 0;
    }
    long result = 0;
    
    int rowShift = (move & -8);
    int colShift = (move & 7);
//    long rowNumber = move & 7;
    long row = BitPattern.getRow(move);
    long column = BitPattern.getColumn(move);
    long diag7 = BitPattern.getDiag7(move);
    long diag9 = BitPattern.getDiag9(move);
    int transformedMove = colShift * 65793 + (rowShift << 24);

    int transformedOpponent = transform(opponent, row, column, diag7, diag9, rowShift, colShift);

    int flipped = getDisksRightOfCells6(transformedOpponent, transformedMove) | getDisksLeftOfCells6(transformedOpponent, transformedMove);

    result |= (flipped & 255) << rowShift;
    result |= (((flipped & 65280) >> 8) * BitPattern.FIRST_COLUMN_BIT_PATTERN) & diag7;
    result |= (((flipped & 16711680) >> 16) * BitPattern.FIRST_COLUMN_BIT_PATTERN) & diag9;
    result |= ((((flipped & -2130706432) >> 24) * BitPattern.MAIN_DIAG9_BIT_PATTERN) & BitPattern.LAST_COLUMN_BIT_PATTERN) >>> (8-colShift);
    return result;
  }
}
