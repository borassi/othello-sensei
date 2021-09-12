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

import java.io.Serializable;

import bitpattern.BitPattern;
import bitpattern.PositionIJ;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.ArrayList;

import helpers.FileAccessor;

public class PossibleMovesFinderImproved implements Serializable {

  /**
   * Needed to silence a warning.
   */
  private static final long serialVersionUID = 1L;

  public static final String SERIALIZE_FILEPATTERN = 
          "coefficients/possible_moves_finder.sar";

  // BitPatterns corresponding to all the rows, columns, and diagonals of length at least 3.
  protected long bitPatterns[];
  
  private Move[][][] movesForBitPatterns;
  
  private Move[][] movesForBitPatterns0;
  private Move[][] movesForBitPatterns1;
  private Move[][] movesForBitPatterns2;
  private Move[][] movesForBitPatterns3;
  private Move[][] movesForBitPatterns4;
  private Move[][] movesForBitPatterns5;
  private Move[][] movesForBitPatterns6;
  private Move[][] movesForBitPatterns7;
  private Move[][] movesForBitPatterns8;
  private Move[][] movesForBitPatterns9;
  private Move[][] movesForBitPatterns10;
  private Move[][] movesForBitPatterns11;
  private Move[][] movesForBitPatterns12;
  private Move[][] movesForBitPatterns13;
  private Move[][] movesForBitPatterns14;
  private Move[][] movesForBitPatterns15;
  private Move[][] movesForBitPatterns16;
  private Move[][] movesForBitPatterns17;
  private Move[][] movesForBitPatterns18;
  private Move[][] movesForBitPatterns19;
  private Move[][] movesForBitPatterns20;
  private Move[][] movesForBitPatterns21;
  private Move[][] movesForBitPatterns22;
  private Move[][] movesForBitPatterns23;
  private Move[][] movesForBitPatterns24;
  private Move[][] movesForBitPatterns25;
  private Move[][] movesForBitPatterns26;
  private Move[][] movesForBitPatterns27;
  private Move[][] movesForBitPatterns28;
  private Move[][] movesForBitPatterns29;
  private Move[][] movesForBitPatterns30;
  private Move[][] movesForBitPatterns31;
  private Move[][] movesForBitPatterns32;
  private Move[][] movesForBitPatterns33;
  private Move[][] movesForBitPatterns34;
  private Move[][] movesForBitPatterns35;
  private Move[][] movesForBitPatterns36;
  private Move[][] movesForBitPatterns37;

  private long bitPatterns0;
  private long bitPatterns1;
  private long bitPatterns2;
  private long bitPatterns3;
  private long bitPatterns4;
  private long bitPatterns5;
  private long bitPatterns6;
  private long bitPatterns7;
  private long bitPatterns8;
  private long bitPatterns9;
  private long bitPatterns10;
  private long bitPatterns11;
  private long bitPatterns12;
  private long bitPatterns13;
  private long bitPatterns14;
  private long bitPatterns15;
  private long bitPatterns16;
  private long bitPatterns17;
  private long bitPatterns18;
  private long bitPatterns19;
  private long bitPatterns20;
  private long bitPatterns21;
  private long bitPatterns22;
  private long bitPatterns23;
  private long bitPatterns24;
  private long bitPatterns25;
  private long bitPatterns26;
  private long bitPatterns27;
  private long bitPatterns28;
  private long bitPatterns29;
  private long bitPatterns30;
  private long bitPatterns31;
  private long bitPatterns32;
  private long bitPatterns33;
  private long bitPatterns34;
  private long bitPatterns35;
  private long bitPatterns36;
  private long bitPatterns37;

  public static long[] allBitPatterns() {
    long firstRow = BitPattern.parsePattern(
              "XXXXXXXX\n"
            + "--------\n" 
            + "--------\n"
            + "--------\n"
            + "--------\n"
            + "--------\n"
            + "--------\n"
            + "--------\n");
    long bitPatterns[] = new long[38];
    // Rows are obtained by shifting the first row.
    for (int i = 0; i < 8; i++) {
      bitPatterns[i] = firstRow >>> (8 * i);
    }
    // Columns are obtained by transposing rows.
    for (int i = 0; i < 8; i++) {
      bitPatterns[8 + i] = BitPattern.transpose(firstRow >>> (8 * i));
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
    for (int i = 0; i < 5; i++) {
      bitPatterns[16 + 4 * i] = curDiag;
      bitPatterns[16 + 4 * i + 1] = BitPattern.horizontalMirror(curDiag);
      bitPatterns[16 + 4 * i + 2] = BitPattern.transpose(curDiag);
      bitPatterns[16 + 4 * i + 3] = BitPattern.horizontalMirror(BitPattern.transpose(curDiag));
      curDiag = curDiag >>> 1 | (1L << (8 * (i + 1) + 23));
    }
    // We add the longest diagonals
    bitPatterns[36] = curDiag;
    bitPatterns[37] = BitPattern.horizontalMirror(curDiag);
    return bitPatterns;
  }
  
  protected void initBitPatterns() {
    bitPatterns = allBitPatterns();
    // We copy the ArrayList into the array.
    movesForBitPatterns = new Move[bitPatterns.length][][];
    
    for (int i = 0; i < movesForBitPatterns.length; i++) {
      movesForBitPatterns[i] = new Move[65536][]; // 2^16 possible configurations.
    }
  }
  
//  public PossibleMovesFinderImproved updateNonFullBitPatterns(Board board) {
//    long filledCases = board.getPlayer() | board.getOpponent();
//    IntArrayList newBitPatterns = new IntArrayList();
//
//    for (int i = 0; i < bitPatterns.length; i++) {
//      long bitPattern = bitPatterns[i];
//      if ((bitPattern & filledCases) != bitPattern) {
//        newBitPatterns.add(i);
//      }
//    }
//    
//    int newBitPatternsArray[] = newBitPatterns.toArray(
//      new int[newBitPatterns.size()]);
//    return new PossibleMovesFinderImproved(bitPatterns, movesForBitPatterns);
//  }
  
//  public static int hashDiagonal(Board board) {
//    return BitPattern.anyDiagonalToFirstRow(board.getPlayer()) + 
//            BitPattern.anyDiagonalToFirstRow(board.getOpponent()) * 256;
//  }  

  public static final int hashBoard(long bitPattern, Board board, int i) {
    long player = board.getPlayer() & bitPattern;
    long opponent = board.getOpponent() & bitPattern;
    if (i >= 16) {
      return BitPattern.anyDiagonalToFirstRow(player) + 
              BitPattern.anyDiagonalToFirstRow(opponent) * 256;      
    }
    if (i >= 8) {
      return BitPattern.anyColumnToFirstRow(player, 15-i) + 
              BitPattern.anyColumnToFirstRow(opponent, 15-i) * 256;
    }
    return BitPattern.anyRowToFirstRow(player, 7-i) + 
            BitPattern.anyRowToFirstRow(opponent, 7-i) * 256;
  }
  
  private void initMoves() {
    for (int i = 0; i < bitPatterns.length; i++) {
      long bitPattern = bitPatterns[i];
      Move[][] movesForCurrentBitPattern = this.movesForBitPatterns[i];

      for (Board board : Board.existingBoardsInBitPattern(bitPattern)) {
        ArrayList<Long> possibleMoves = possibleMovesBasic(board);
        int hash = hashBoard(bitPattern, board, i);
        movesForCurrentBitPattern[hash] = new Move[possibleMoves.size()];

        for (int j = 0; j < possibleMoves.size(); j++) {
          long move = possibleMoves.get(j);
          movesForCurrentBitPattern[hash][j] = new Move(board, move);
        }
      }
    }
  }
  
  private void init() {
    initBitPatterns();
    initMoves();
  }

  long[] moves = new long[64];
  int[] positions = new int[60];
  /**
   * Returns the set of possible moves, as an ArrayList\<Long\>.
   * Each of the move is a bitpattern that contains 1 on all the disks flipped,
   * plus the disk where the player plays.
   * 
   * From this representation, one can find the board after the move with
   * player = opponent & ~move, opponent = player | move. It is also possible
   * to find the case of the move as move & ~opponent.
   * 
   * @param b the board
   * @return the list of moves.
   */
  public long[] possibleMovesOld(Board b) {
    int nPositions = 0;

    for (int d = 0; d < 1000; d++) {
      for (int i = 0; i < 38; i++) {
        for (Move m : this.movesForBitPatterns[i][hashBoard(bitPatterns[i], b, i)]) {
          int position = m.position;
          if (moves[position] == 0) {
            positions[nPositions++] = position;
          }
          moves[position] = moves[position] | m.flip;
        }
      }
    }
    long[] result = new long[nPositions];
    
    for (int i = 0; i < nPositions; i++) {
      result[i] = moves[positions[i]];
      moves[positions[i]] = 0;
    }
    return result;
  }

  public long[] possibleMoves(Board b) {
    return possibleMoves(b.getPlayer(), b.getOpponent());
  }
  public int possibleMovesInternal(long player, long opponent) {
    int nPositions = 0;
    int position;
    int h;
    long bPlayer = player;
    long bOpponent = opponent;

    player = bPlayer & bitPatterns0;
    opponent = bOpponent & bitPatterns0;
    h = BitPattern.anyRowToFirstRow(player, 7) + BitPattern.anyRowToFirstRow(opponent, 7) * 256;
    for (Move m : this.movesForBitPatterns0[h]) {
      position = m.position;

      if (moves[position] == 0) {
        positions[nPositions++] = position;
        moves[position] = m.flip;
      } else {
        moves[position] |= m.flip;
      }
    }

    player = bPlayer & bitPatterns1;
    opponent = bOpponent & bitPatterns1;
    h = BitPattern.anyRowToFirstRow(player, 6) + BitPattern.anyRowToFirstRow(opponent, 6) * 256;
    for (Move m : this.movesForBitPatterns1[h]) {
      position = m.position;

      if (moves[position] == 0) {
        positions[nPositions++] = position;
        moves[position] = m.flip;
      } else {
        moves[position] |= m.flip;
      }
    }

    player = bPlayer & bitPatterns2;
    opponent = bOpponent & bitPatterns2;
    h = BitPattern.anyRowToFirstRow(player, 5) + BitPattern.anyRowToFirstRow(opponent, 5) * 256;
    for (Move m : this.movesForBitPatterns2[h]) {
      position = m.position;

      if (moves[position] == 0) {
        positions[nPositions++] = position;
        moves[position] = m.flip;
      } else {
        moves[position] |= m.flip;
      }
    }

    player = bPlayer & bitPatterns3;
    opponent = bOpponent & bitPatterns3;
    h = BitPattern.anyRowToFirstRow(player, 4) + BitPattern.anyRowToFirstRow(opponent, 4) * 256;
    for (Move m : this.movesForBitPatterns3[h]) {
      position = m.position;

      if (moves[position] == 0) {
        positions[nPositions++] = position;
        moves[position] = m.flip;
      } else {
        moves[position] |= m.flip;
      }
    }

    player = bPlayer & bitPatterns4;
    opponent = bOpponent & bitPatterns4;
    h = BitPattern.anyRowToFirstRow(player, 3) + BitPattern.anyRowToFirstRow(opponent, 3) * 256;
    for (Move m : this.movesForBitPatterns4[h]) {
      position = m.position;

      if (moves[position] == 0) {
        positions[nPositions++] = position;
        moves[position] = m.flip;
      } else {
        moves[position] |= m.flip;
      }
    }

    player = bPlayer & bitPatterns5;
    opponent = bOpponent & bitPatterns5;
    h = BitPattern.anyRowToFirstRow(player, 2) + BitPattern.anyRowToFirstRow(opponent, 2) * 256;
    for (Move m : this.movesForBitPatterns5[h]) {
      position = m.position;

      if (moves[position] == 0) {
        positions[nPositions++] = position;
        moves[position] = m.flip;
      } else {
        moves[position] |= m.flip;
      }
    }

    player = bPlayer & bitPatterns6;
    opponent = bOpponent & bitPatterns6;
    h = BitPattern.anyRowToFirstRow(player, 1) + BitPattern.anyRowToFirstRow(opponent, 1) * 256;
    for (Move m : this.movesForBitPatterns6[h]) {
      position = m.position;

      if (moves[position] == 0) {
        positions[nPositions++] = position;
        moves[position] = m.flip;
      } else {
        moves[position] |= m.flip;
      }
    }

    player = bPlayer & bitPatterns7;
    opponent = bOpponent & bitPatterns7;
    h = BitPattern.anyRowToFirstRow(player, 0) + BitPattern.anyRowToFirstRow(opponent, 0) * 256;
    for (Move m : this.movesForBitPatterns7[h]) {
      position = m.position;

      if (moves[position] == 0) {
        positions[nPositions++] = position;
        moves[position] = m.flip;
      } else {
        moves[position] |= m.flip;
      }
    }

    player = bPlayer & bitPatterns8;
    opponent = bOpponent & bitPatterns8;
    h = BitPattern.anyColumnToFirstRow(player, 7) + BitPattern.anyColumnToFirstRow(opponent, 7) * 256;
    for (Move m : this.movesForBitPatterns8[h]) {
      position = m.position;

      if (moves[position] == 0) {
        positions[nPositions++] = position;
        moves[position] = m.flip;
      } else {
        moves[position] |= m.flip;
      }
    }

    player = bPlayer & bitPatterns9;
    opponent = bOpponent & bitPatterns9;
    h = BitPattern.anyColumnToFirstRow(player, 6) + BitPattern.anyColumnToFirstRow(opponent, 6) * 256;
    for (Move m : this.movesForBitPatterns9[h]) {
      position = m.position;

      if (moves[position] == 0) {
        positions[nPositions++] = position;
        moves[position] = m.flip;
      } else {
        moves[position] |= m.flip;
      }
    }

    player = bPlayer & bitPatterns10;
    opponent = bOpponent & bitPatterns10;
    h = BitPattern.anyColumnToFirstRow(player, 5) + BitPattern.anyColumnToFirstRow(opponent, 5) * 256;
    for (Move m : this.movesForBitPatterns10[h]) {
      position = m.position;

      if (moves[position] == 0) {
        positions[nPositions++] = position;
        moves[position] = m.flip;
      } else {
        moves[position] |= m.flip;
      }
    }

    player = bPlayer & bitPatterns11;
    opponent = bOpponent & bitPatterns11;
    h = BitPattern.anyColumnToFirstRow(player, 4) + BitPattern.anyColumnToFirstRow(opponent, 4) * 256;
    for (Move m : this.movesForBitPatterns11[h]) {
      position = m.position;

      if (moves[position] == 0) {
        positions[nPositions++] = position;
        moves[position] = m.flip;
      } else {
        moves[position] |= m.flip;
      }
    }

    player = bPlayer & bitPatterns12;
    opponent = bOpponent & bitPatterns12;
    h = BitPattern.anyColumnToFirstRow(player, 3) + BitPattern.anyColumnToFirstRow(opponent, 3) * 256;
    for (Move m : this.movesForBitPatterns12[h]) {
      position = m.position;

      if (moves[position] == 0) {
        positions[nPositions++] = position;
        moves[position] = m.flip;
      } else {
        moves[position] |= m.flip;
      }
    }

    player = bPlayer & bitPatterns13;
    opponent = bOpponent & bitPatterns13;
    h = BitPattern.anyColumnToFirstRow(player, 2) + BitPattern.anyColumnToFirstRow(opponent, 2) * 256;
    for (Move m : this.movesForBitPatterns13[h]) {
      position = m.position;

      if (moves[position] == 0) {
        positions[nPositions++] = position;
        moves[position] = m.flip;
      } else {
        moves[position] |= m.flip;
      }
    }

    player = bPlayer & bitPatterns14;
    opponent = bOpponent & bitPatterns14;
    h = BitPattern.anyColumnToFirstRow(player, 1) + BitPattern.anyColumnToFirstRow(opponent, 1) * 256;
    for (Move m : this.movesForBitPatterns14[h]) {
      position = m.position;

      if (moves[position] == 0) {
        positions[nPositions++] = position;
        moves[position] = m.flip;
      } else {
        moves[position] |= m.flip;
      }
    }

    player = bPlayer & bitPatterns15;
    opponent = bOpponent & bitPatterns15;
    h = BitPattern.anyColumnToFirstRow(player, 0) + BitPattern.anyColumnToFirstRow(opponent, 0) * 256;
    for (Move m : this.movesForBitPatterns15[h]) {
      position = m.position;

      if (moves[position] == 0) {
        positions[nPositions++] = position;
        moves[position] = m.flip;
      } else {
        moves[position] |= m.flip;
      }
    }

    player = bPlayer & bitPatterns16;
    opponent = bOpponent & bitPatterns16;
    h = BitPattern.anyDiagonalToFirstRow(player) + BitPattern.anyDiagonalToFirstRow(opponent) * 256;
    for (Move m : this.movesForBitPatterns16[h]) {
      position = m.position;

      if (moves[position] == 0) {
        positions[nPositions++] = position;
        moves[position] = m.flip;
      } else {
        moves[position] |= m.flip;
      }
    }

    player = bPlayer & bitPatterns17;
    opponent = bOpponent & bitPatterns17;
    h = BitPattern.anyDiagonalToFirstRow(player) + BitPattern.anyDiagonalToFirstRow(opponent) * 256;
    for (Move m : this.movesForBitPatterns17[h]) {
      position = m.position;

      if (moves[position] == 0) {
        positions[nPositions++] = position;
        moves[position] = m.flip;
      } else {
        moves[position] |= m.flip;
      }
    }

    player = bPlayer & bitPatterns18;
    opponent = bOpponent & bitPatterns18;
    h = BitPattern.anyDiagonalToFirstRow(player) + BitPattern.anyDiagonalToFirstRow(opponent) * 256;
    for (Move m : this.movesForBitPatterns18[h]) {
      position = m.position;

      if (moves[position] == 0) {
        positions[nPositions++] = position;
        moves[position] = m.flip;
      } else {
        moves[position] |= m.flip;
      }
    }

    player = bPlayer & bitPatterns19;
    opponent = bOpponent & bitPatterns19;
    h = BitPattern.anyDiagonalToFirstRow(player) + BitPattern.anyDiagonalToFirstRow(opponent) * 256;
    for (Move m : this.movesForBitPatterns19[h]) {
      position = m.position;

      if (moves[position] == 0) {
        positions[nPositions++] = position;
        moves[position] = m.flip;
      } else {
        moves[position] |= m.flip;
      }
    }

    player = bPlayer & bitPatterns20;
    opponent = bOpponent & bitPatterns20;
    h = BitPattern.anyDiagonalToFirstRow(player) + BitPattern.anyDiagonalToFirstRow(opponent) * 256;
    for (Move m : this.movesForBitPatterns20[h]) {
      position = m.position;

      if (moves[position] == 0) {
        positions[nPositions++] = position;
        moves[position] = m.flip;
      } else {
        moves[position] |= m.flip;
      }
    }

    player = bPlayer & bitPatterns21;
    opponent = bOpponent & bitPatterns21;
    h = BitPattern.anyDiagonalToFirstRow(player) + BitPattern.anyDiagonalToFirstRow(opponent) * 256;
    for (Move m : this.movesForBitPatterns21[h]) {
      position = m.position;

      if (moves[position] == 0) {
        positions[nPositions++] = position;
        moves[position] = m.flip;
      } else {
        moves[position] |= m.flip;
      }
    }

    player = bPlayer & bitPatterns22;
    opponent = bOpponent & bitPatterns22;
    h = BitPattern.anyDiagonalToFirstRow(player) + BitPattern.anyDiagonalToFirstRow(opponent) * 256;
    for (Move m : this.movesForBitPatterns22[h]) {
      position = m.position;

      if (moves[position] == 0) {
        positions[nPositions++] = position;
        moves[position] = m.flip;
      } else {
        moves[position] |= m.flip;
      }
    }

    player = bPlayer & bitPatterns23;
    opponent = bOpponent & bitPatterns23;
    h = BitPattern.anyDiagonalToFirstRow(player) + BitPattern.anyDiagonalToFirstRow(opponent) * 256;
    for (Move m : this.movesForBitPatterns23[h]) {
      position = m.position;

      if (moves[position] == 0) {
        positions[nPositions++] = position;
        moves[position] = m.flip;
      } else {
        moves[position] |= m.flip;
      }
    }

    player = bPlayer & bitPatterns24;
    opponent = bOpponent & bitPatterns24;
    h = BitPattern.anyDiagonalToFirstRow(player) + BitPattern.anyDiagonalToFirstRow(opponent) * 256;
    for (Move m : this.movesForBitPatterns24[h]) {
      position = m.position;

      if (moves[position] == 0) {
        positions[nPositions++] = position;
        moves[position] = m.flip;
      } else {
        moves[position] |= m.flip;
      }
    }

    player = bPlayer & bitPatterns25;
    opponent = bOpponent & bitPatterns25;
    h = BitPattern.anyDiagonalToFirstRow(player) + BitPattern.anyDiagonalToFirstRow(opponent) * 256;
    for (Move m : this.movesForBitPatterns25[h]) {
      position = m.position;

      if (moves[position] == 0) {
        positions[nPositions++] = position;
        moves[position] = m.flip;
      } else {
        moves[position] |= m.flip;
      }
    }

    player = bPlayer & bitPatterns26;
    opponent = bOpponent & bitPatterns26;
    h = BitPattern.anyDiagonalToFirstRow(player) + BitPattern.anyDiagonalToFirstRow(opponent) * 256;
    for (Move m : this.movesForBitPatterns26[h]) {
      position = m.position;

      if (moves[position] == 0) {
        positions[nPositions++] = position;
        moves[position] = m.flip;
      } else {
        moves[position] |= m.flip;
      }
    }

    player = bPlayer & bitPatterns27;
    opponent = bOpponent & bitPatterns27;
    h = BitPattern.anyDiagonalToFirstRow(player) + BitPattern.anyDiagonalToFirstRow(opponent) * 256;
    for (Move m : this.movesForBitPatterns27[h]) {
      position = m.position;

      if (moves[position] == 0) {
        positions[nPositions++] = position;
        moves[position] = m.flip;
      } else {
        moves[position] |= m.flip;
      }
    }

    player = bPlayer & bitPatterns28;
    opponent = bOpponent & bitPatterns28;
    h = BitPattern.anyDiagonalToFirstRow(player) + BitPattern.anyDiagonalToFirstRow(opponent) * 256;
    for (Move m : this.movesForBitPatterns28[h]) {
      position = m.position;

      if (moves[position] == 0) {
        positions[nPositions++] = position;
        moves[position] = m.flip;
      } else {
        moves[position] |= m.flip;
      }
    }

    player = bPlayer & bitPatterns29;
    opponent = bOpponent & bitPatterns29;
    h = BitPattern.anyDiagonalToFirstRow(player) + BitPattern.anyDiagonalToFirstRow(opponent) * 256;
    for (Move m : this.movesForBitPatterns29[h]) {
      position = m.position;

      if (moves[position] == 0) {
        positions[nPositions++] = position;
        moves[position] = m.flip;
      } else {
        moves[position] |= m.flip;
      }
    }

    player = bPlayer & bitPatterns30;
    opponent = bOpponent & bitPatterns30;
    h = BitPattern.anyDiagonalToFirstRow(player) + BitPattern.anyDiagonalToFirstRow(opponent) * 256;
    for (Move m : this.movesForBitPatterns30[h]) {
      position = m.position;

      if (moves[position] == 0) {
        positions[nPositions++] = position;
        moves[position] = m.flip;
      } else {
        moves[position] |= m.flip;
      }
    }

    player = bPlayer & bitPatterns31;
    opponent = bOpponent & bitPatterns31;
    h = BitPattern.anyDiagonalToFirstRow(player) + BitPattern.anyDiagonalToFirstRow(opponent) * 256;
    for (Move m : this.movesForBitPatterns31[h]) {
      position = m.position;

      if (moves[position] == 0) {
        positions[nPositions++] = position;
        moves[position] = m.flip;
      } else {
        moves[position] |= m.flip;
      }
    }

    player = bPlayer & bitPatterns32;
    opponent = bOpponent & bitPatterns32;
    h = BitPattern.anyDiagonalToFirstRow(player) + BitPattern.anyDiagonalToFirstRow(opponent) * 256;
    for (Move m : this.movesForBitPatterns32[h]) {
      position = m.position;

      if (moves[position] == 0) {
        positions[nPositions++] = position;
        moves[position] = m.flip;
      } else {
        moves[position] |= m.flip;
      }
    }

    player = bPlayer & bitPatterns33;
    opponent = bOpponent & bitPatterns33;
    h = BitPattern.anyDiagonalToFirstRow(player) + BitPattern.anyDiagonalToFirstRow(opponent) * 256;
    for (Move m : this.movesForBitPatterns33[h]) {
      position = m.position;

      if (moves[position] == 0) {
        positions[nPositions++] = position;
        moves[position] = m.flip;
      } else {
        moves[position] |= m.flip;
      }
    }

    player = bPlayer & bitPatterns34;
    opponent = bOpponent & bitPatterns34;
    h = BitPattern.anyDiagonalToFirstRow(player) + BitPattern.anyDiagonalToFirstRow(opponent) * 256;
    for (Move m : this.movesForBitPatterns34[h]) {
      position = m.position;

      if (moves[position] == 0) {
        positions[nPositions++] = position;
        moves[position] = m.flip;
      } else {
        moves[position] |= m.flip;
      }
    }

    player = bPlayer & bitPatterns35;
    opponent = bOpponent & bitPatterns35;
    h = BitPattern.anyDiagonalToFirstRow(player) + BitPattern.anyDiagonalToFirstRow(opponent) * 256;
    for (Move m : this.movesForBitPatterns35[h]) {
      position = m.position;

      if (moves[position] == 0) {
        positions[nPositions++] = position;
        moves[position] = m.flip;
      } else {
        moves[position] |= m.flip;
      }
    }

    player = bPlayer & bitPatterns36;
    opponent = bOpponent & bitPatterns36;
    h = BitPattern.anyDiagonalToFirstRow(player) + BitPattern.anyDiagonalToFirstRow(opponent) * 256;
    for (Move m : this.movesForBitPatterns36[h]) {
      position = m.position;

      if (moves[position] == 0) {
        positions[nPositions++] = position;
        moves[position] = m.flip;
      } else {
        moves[position] |= m.flip;
      }
    }

    player = bPlayer & bitPatterns37;
    opponent = bOpponent & bitPatterns37;
    h = BitPattern.anyDiagonalToFirstRow(player) + BitPattern.anyDiagonalToFirstRow(opponent) * 256;
    for (Move m : this.movesForBitPatterns37[h]) {
      position = m.position;
      if (moves[position] == 0) {
        positions[nPositions++] = position;
        moves[position] = m.flip;
      } else {
        moves[position] |= m.flip;
      }
    }
    return nPositions;
  }
  public long[] possibleMoves(long player, long opponent) {
    int nPositions = possibleMovesInternal(player, opponent);
    long[] result = new long[nPositions];
    
    for (int i = 0; i < nPositions; i++) {
      result[i] = moves[positions[i]];
      moves[positions[i]] = 0;
    }
    return result;
  }

  public long[] possibleMovesAdvanced(long player, long opponent) {
    int nPositions = possibleMovesInternal(player, opponent);
    if (nPositions == 0) {
      int nMovesAfterPass = possibleMovesInternal(opponent, player);
      if (nMovesAfterPass == 0) {
        return new long[0];
      }
      for (int i = 0; i < nMovesAfterPass; i++) {
        moves[positions[i]] = 0;
      }
      return new long[] {0};
    }
    long[] result = new long[nPositions];
    
    for (int i = 0; i < nPositions; i++) {
      result[i] = moves[positions[i]];
      moves[positions[i]] = 0;
    }
    return result;
  }
  
  
  
  /**
   * The same as possibleMoves, but with a basic algorithm (used for testing).
   * @param b the board.
   * @return the list of moves.
   */
  public static final ArrayList<Long> possibleMovesBasic(Board b) {
    ArrayList<Long> moves = new ArrayList<>();
    int[][] directions = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, 
                           {0, 1}, {1, -1}, {1, 0}, {1, 1}};
    char player = 'X';
    char opponent = 'O';

    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 8; j++) {
        if (b.getCase(i, j, true) != '-') {
          continue;
        }
        long currentMove = 0;
        for (int[] direction : directions) {
          if (b.getCase(i + direction[0], j + direction[1], true) != opponent) {
            continue;
          }
          for (int steps = 2; steps < 8; steps++) {
            if (b.getCase(i + steps * direction[0], j + steps * direction[1], true) == player) {
              for (int backSteps = steps - 1; backSteps > 0; backSteps--) {
                currentMove = currentMove | BitPattern.moveToBit(
                        i + backSteps * direction[0], j + backSteps * direction[1]);
              }
            }
            if (b.getCase(i + steps * direction[0], j + steps * direction[1], true) != opponent) {
              break;
            }              
          }
        }
        if (currentMove != 0) {
          currentMove = currentMove | BitPattern.moveToBit(i, j);
          moves.add(currentMove);
        }
      }
    }
    return moves;
  }
  
  /**
   * @param b the board
   * @return true if the player has to pass, false otherwise.
   */
  public boolean haveToPass(Board b) {
    return possibleMoves(b).length == 0;
  }
  
  /**
   * Plays a move in the case specified (e.g., e6).
   * @param b the board
   * @param move the move
   * @return a Board after the move (does not modify the current board).
   */
  public Board moveIfPossible(Board b, String move) {
    return moveIfPossible(b, new PositionIJ(move.charAt(1) - '1',
                          move.charAt(0) - 'a'));
  }
  
  /**
   * Plays a move in the case (i,j).
   * @param b the board
   * @param ij the row and the column (from 0 to 7)
   * @return a Board after the move (does not modify the current board).
   */
  public Board moveIfPossible(Board b, PositionIJ ij) {
    long bitMove = BitPattern.moveToBit(ij);
    for (long m : possibleMoves(b)) {
      if (b.moveToBit(m) == bitMove) {
        return b.move(m);
      }
    }
    throw new IllegalArgumentException("The move is not valid.");
  }
  
  private PossibleMovesFinderImproved(long bitPatterns[], 
          Move[][][] movesForBitPatterns) {
    this.bitPatterns = bitPatterns;
    this.movesForBitPatterns = movesForBitPatterns;
  }

  public PossibleMovesFinderImproved() {
    init();
    initNumberedVariables();
    save(SERIALIZE_FILEPATTERN);
  }

  public final void save(String filepath) {
    try (ObjectOutputStream out = FileAccessor.objectOutputStream(filepath)) {
      out.writeObject(this);
    } catch (IOException e) {
    }
//    System.out.println("Serialized data is saved.");
  }
  
  public static PossibleMovesFinderImproved load() {
    return load(SERIALIZE_FILEPATTERN);
  }

  public static PossibleMovesFinderImproved load(String filepath) {
    PossibleMovesFinderImproved result;
    try (ObjectInputStream in = FileAccessor.objectInputStream(filepath)) {
      result = (PossibleMovesFinderImproved) in.readObject();
    } catch (IOException | ClassNotFoundException e) {
      e.printStackTrace();
      return new PossibleMovesFinderImproved();
    }
    return result;
  }
  
  private void initNumberedVariables() {
    movesForBitPatterns0 = movesForBitPatterns[0];
    movesForBitPatterns1 = movesForBitPatterns[1];
    movesForBitPatterns2 = movesForBitPatterns[2];
    movesForBitPatterns3 = movesForBitPatterns[3];
    movesForBitPatterns4 = movesForBitPatterns[4];
    movesForBitPatterns5 = movesForBitPatterns[5];
    movesForBitPatterns6 = movesForBitPatterns[6];
    movesForBitPatterns7 = movesForBitPatterns[7];
    movesForBitPatterns8 = movesForBitPatterns[8];
    movesForBitPatterns9 = movesForBitPatterns[9];
    movesForBitPatterns10 = movesForBitPatterns[10];
    movesForBitPatterns11 = movesForBitPatterns[11];
    movesForBitPatterns12 = movesForBitPatterns[12];
    movesForBitPatterns13 = movesForBitPatterns[13];
    movesForBitPatterns14 = movesForBitPatterns[14];
    movesForBitPatterns15 = movesForBitPatterns[15];
    movesForBitPatterns16 = movesForBitPatterns[16];
    movesForBitPatterns17 = movesForBitPatterns[17];
    movesForBitPatterns18 = movesForBitPatterns[18];
    movesForBitPatterns19 = movesForBitPatterns[19];
    movesForBitPatterns20 = movesForBitPatterns[20];
    movesForBitPatterns21 = movesForBitPatterns[21];
    movesForBitPatterns22 = movesForBitPatterns[22];
    movesForBitPatterns23 = movesForBitPatterns[23];
    movesForBitPatterns24 = movesForBitPatterns[24];
    movesForBitPatterns25 = movesForBitPatterns[25];
    movesForBitPatterns26 = movesForBitPatterns[26];
    movesForBitPatterns27 = movesForBitPatterns[27];
    movesForBitPatterns28 = movesForBitPatterns[28];
    movesForBitPatterns29 = movesForBitPatterns[29];
    movesForBitPatterns30 = movesForBitPatterns[30];
    movesForBitPatterns31 = movesForBitPatterns[31];
    movesForBitPatterns32 = movesForBitPatterns[32];
    movesForBitPatterns33 = movesForBitPatterns[33];
    movesForBitPatterns34 = movesForBitPatterns[34];
    movesForBitPatterns35 = movesForBitPatterns[35];
    movesForBitPatterns36 = movesForBitPatterns[36];
    movesForBitPatterns37 = movesForBitPatterns[37];
    bitPatterns0 = bitPatterns[0];
    bitPatterns1 = bitPatterns[1];
    bitPatterns2 = bitPatterns[2];
    bitPatterns3 = bitPatterns[3];
    bitPatterns4 = bitPatterns[4];
    bitPatterns5 = bitPatterns[5];
    bitPatterns6 = bitPatterns[6];
    bitPatterns7 = bitPatterns[7];
    bitPatterns8 = bitPatterns[8];
    bitPatterns9 = bitPatterns[9];
    bitPatterns10 = bitPatterns[10];
    bitPatterns11 = bitPatterns[11];
    bitPatterns12 = bitPatterns[12];
    bitPatterns13 = bitPatterns[13];
    bitPatterns14 = bitPatterns[14];
    bitPatterns15 = bitPatterns[15];
    bitPatterns16 = bitPatterns[16];
    bitPatterns17 = bitPatterns[17];
    bitPatterns18 = bitPatterns[18];
    bitPatterns19 = bitPatterns[19];
    bitPatterns20 = bitPatterns[20];
    bitPatterns21 = bitPatterns[21];
    bitPatterns22 = bitPatterns[22];
    bitPatterns23 = bitPatterns[23];
    bitPatterns24 = bitPatterns[24];
    bitPatterns25 = bitPatterns[25];
    bitPatterns26 = bitPatterns[26];
    bitPatterns27 = bitPatterns[27];
    bitPatterns28 = bitPatterns[28];
    bitPatterns29 = bitPatterns[29];
    bitPatterns30 = bitPatterns[30];
    bitPatterns31 = bitPatterns[31];
    bitPatterns32 = bitPatterns[32];
    bitPatterns33 = bitPatterns[33];
    bitPatterns34 = bitPatterns[34];
    bitPatterns35 = bitPatterns[35];
    bitPatterns36 = bitPatterns[36];
    bitPatterns37 = bitPatterns[37];
  }
  
  public static void main(String args[]) {  
    for (int i = 0; i < 38; i++) {
      System.out.println("private Move[][] movesForBitPatterns" + i + ";");
    }
    for (int i = 0; i < 38; i++) {
      System.out.println("private long bitPatterns" + i + ";");
    }  
    for (int i = 0; i < 38; i++) {
      System.out.println("movesForBitPatterns" + i + " = movesForBitPatterns[" + i + "];");
    }
    for (int i = 0; i < 38; i++) {
      System.out.println("bitPatterns" + i + " = bitPatterns[" + i + "];");
    }
    
    for (int i = 0; i < 38; i++) {
      System.out.println("player = bPlayer & bitPatterns" + i + ";");
      System.out.println("opponent = bOpponent & bitPatterns" + i + ";");
      if (i >= 16) {
        System.out.println("h = BitPattern.anyDiagonalToFirstRow(player) + BitPattern.anyDiagonalToFirstRow(opponent) * 256;");      
      } else if (i >= 8) {
        System.out.println("h = BitPattern.anyColumnToFirstRow(player, " + (15-i) + ") " +
          "+ BitPattern.anyColumnToFirstRow(opponent, " + (15-i) + ") * 256;");   
      } else {
        System.out.println("h = BitPattern.anyRowToFirstRow(player, " + (7-i) + ") " +
          "+ BitPattern.anyRowToFirstRow(opponent, " + (7-i) + ") * 256;");           
      }
      System.out.println("for (Move m : this.movesForBitPatterns" + i + "[h]) {");
      System.out.println("  position = m.position;");
      System.out.println("  if (moves[position] == 0) {");
      System.out.println("    positions[nPositions++] = position;");
      System.out.println("    moves[position] = m.flip;");
      System.out.println("  } else {");
      System.out.println("    moves[position] |= m.flip;");
      System.out.println("  }");
      System.out.println("}");
      System.out.println("");
    }
  }
}
