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
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import java.lang.management.ManagementFactory;
import java.lang.management.ThreadMXBean;
import java.util.ArrayList;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author michele
 */
public class GetMovesCache implements Serializable {
  /**
   * Needed to implement Serializable.
   */
  private static final long serialVersionUID = 1L;
//  public final static int EVAL_SHIFT = 32;
  public static final String GET_MOVES_FILEPATTERN = 
      "coefficients/get_moves.sar";
  
  public static abstract class Flipper {
    public abstract long run();
  }
  private static long[] flipHorizontalLast = new long[64 * 256];
  private static long[] flipVerticalLast = new long[64 * 256];
  private static long[] flipDiagonalLast = new long[64 * 256];
  private static long[] flipReverseDiagonalLast = new long[64 * 256];
  
  private static final long CORNERS = BitPattern.parsePattern(
      "X------X" +
      "--------" +
      "--------" +
      "--------" +
      "--------" +
      "--------" +
      "--------" +
      "X------X");
  
  private static final long MASK = BitPattern.parsePattern(
      "-XXXXXX-" +
      "-XXXXXX-" +
      "-XXXXXX-" +
      "-XXXXXX-" +
      "-XXXXXX-" +
      "-XXXXXX-" +
      "-XXXXXX-" +
      "-XXXXXX-");
  
  private static boolean init = false;
  public GetMovesCache() {
    if (!init) {
      try {
        loadFlips();
      } catch (IOException | ClassNotFoundException ex) {
        initMoves();
        try {
          saveFlips();
        } catch (IOException ex1) {
          Logger.getLogger(GetMovesCache.class.getName()).log(Level.WARNING, null, ex1);
        }
      }
      init = true;
    }
  }
  
  
  private static void saveFlips() throws IOException {
    ObjectOutputStream out = new ObjectOutputStream(
        new FileOutputStream(GET_MOVES_FILEPATTERN)
    );
    out.writeObject(flipHorizontalLast);
    out.writeObject(flipVerticalLast);
    out.writeObject(flipDiagonalLast);
    out.writeObject(flipReverseDiagonalLast);
    out.flush();
    out.close();
  }

  private static void loadFlips() throws IOException, ClassNotFoundException {
    ObjectInputStream in = new ObjectInputStream(new FileInputStream(GET_MOVES_FILEPATTERN));
    flipHorizontalLast = (long[]) in.readObject();
    flipVerticalLast = (long[]) in.readObject();
    flipDiagonalLast = (long[]) in.readObject();
    flipReverseDiagonalLast = (long[]) in.readObject();
    in.close();
  }
  
  static int hashDiagonal(int move, long player) {
    long bitPattern = BitPattern.getDiag9(move);
    return BitPattern.anyDiagonalToFirstRow(player & bitPattern);
  }
  static int hashRevDiagonal(int move, long player) {
    long bitPattern = BitPattern.getDiag7(move);
    return BitPattern.anyDiagonalToFirstRow(player & bitPattern);
  }
  static int hashRow(int move, long player) {
    long bitPattern = BitPattern.getRow(move);
    return BitPattern.anyRowToFirstRow(player & bitPattern, move / 8);
  }
  static int hashColumn(int move, long player) {
    long bitPattern = BitPattern.getColumn(move);
    return BitPattern.anyColumnToFirstRow(player & bitPattern, move % 8);
  }

  private static int hashGeneric(int move, long player, long bitPattern) {
    if (bitPattern == BitPattern.getDiag9(move)) {
      return hashDiagonal(move, player);
    }
    if (bitPattern == BitPattern.getDiag7(move)) {
      return hashRevDiagonal(move, player);
    }
    if (bitPattern == BitPattern.getRow(move)) {
      return hashRow(move, player);
    }
    if (bitPattern == BitPattern.getColumn(move)) {
      return hashColumn(move, player);
    }
    assert(false);
    return 0;
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
              for (int backSteps = steps; backSteps > 0; backSteps--) {
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
  
  private void initMoves() {
    for (int position = 0; position < 64; position++) {
      long bitPatterns[] = {BitPattern.getRow(position), 
        BitPattern.getColumn(position), BitPattern.getDiag7(position),
        BitPattern.getDiag9(position)};
      long[][] flipsLast = {flipHorizontalLast, flipVerticalLast, flipReverseDiagonalLast, flipDiagonalLast};

      for (int i = 0; i < bitPatterns.length; i++) {
        long bitPattern = bitPatterns[i];
        long[] flipLast = flipsLast[i];
        for (Board board : Board.existingBoardsInBitPattern(bitPattern)) {
          ArrayList<Long> possibleMoves = possibleMovesBasic(board);

          for (int j = 0; j < possibleMoves.size(); j++) {
            long curFlip = possibleMoves.get(j);          
            int thisMovePosition = Long.numberOfTrailingZeros(
              curFlip & (~board.getPlayer()) & (~board.getOpponent()));
            if (thisMovePosition == position) {
              if ((curFlip | board.getPlayer() | board.getOpponent()) == bitPattern) {
                int newHash = hashGeneric(
                  position, board.getOpponent(), bitPattern);
//                assert(flipLast[position * 256 + newHash] == 0);
                flipLast[position * 256 + newHash] = curFlip;
              }
            }
          }
        }
      }
    }
  }
  
  private static long getDisksRightOfCells1(long disks, long cells, int dir) {
    return disks & (cells >>> dir);
  }

  private static long getDisksRightOfCells2(long disks, long cells, int dir) {
    long result = disks & (cells >>> dir);
    return disks & (result | (result >>> dir));
  }

  private static long getDisksRightOfCells3(long disks, long cells, int dir) {
    long result = disks & (cells >>> dir);
    result |= disks & (result >>> dir);
    return result | (disks & (result >>> dir));
  }

  private static long getDisksRightOfCells4(long disks, long cells, int dir) {
    long result = disks & (cells >>> dir);
    result |= disks & (result >>> dir);
    result |= disks & (result >>> dir);
    return result | (disks & (result >>> dir));
  }

  private static long getDisksRightOfCells5(long disks, long cells, int dir) {
    long result = disks & (cells >>> dir);
    result |= disks & (result >>> dir);
    result |= disks & (result >>> dir);
    result |= disks & (result >>> dir);
    return result | (disks & (result >>> dir));
  }
  
  private static long getDisksLeftOfCells6(long disks, long cells, int dir) {
    int dir2 = dir * 2;
    
    long result = disks & (cells << dir);
    result |= disks & (result << dir);
    disks = disks & (disks << dir);
    result |= disks & (result << dir2);
    result |= disks & (result << dir2);
    return result;
  }
  
  private static long getDisksRightOfCells6(long disks, long cells, int dir) {
    int dir2 = dir * 2;
    
    long result = disks & (cells >>> dir);
    result |= disks & (result >>> dir);
    disks = disks & (disks >>> dir);
    result |= disks & (result >>> dir2);
    result |= disks & (result >>> dir2);
    return result;
  }
  
  private long opponentNearPlayer1;
  private long opponentNearPlayer7;
  private long opponentNearPlayer8;
  private long opponentNearPlayer9;
  private long opponentAfterPlayer1;
  private long opponentAfterPlayer7;
  private long opponentAfterPlayer8;
  private long opponentAfterPlayer9;
  private long moves;
  
  public long getMoves(long player, long opponent) {
    long opponentFlippable = opponent & MASK;
    long empties = ~(player | opponent);
    opponentNearPlayer1 = getDisksLeftOfCells6(opponentFlippable, player, 1) | getDisksRightOfCells6(opponentFlippable, player, 1);
    opponentNearPlayer8 = getDisksLeftOfCells6(opponent, player, 8) | getDisksRightOfCells6(opponent, player, 8);
    opponentNearPlayer7 = getDisksLeftOfCells6(opponentFlippable, player, 7) | getDisksRightOfCells6(opponentFlippable, player, 7);
    opponentNearPlayer9 = getDisksLeftOfCells6(opponentFlippable, player, 9) | getDisksRightOfCells6(opponentFlippable, player, 9);
    
//    opponentAfterPlayer1 = getDisksRightOfCells6(opponentFlippable, player, 1) >>> 1;
//    opponentAfterPlayer8 = getDisksRightOfCells6(opponent, player, 8) >>> 8;
//    opponentAfterPlayer7 = getDisksRightOfCells6(opponentFlippable, player, 7) >>> 7;
//    opponentAfterPlayer9 = getDisksRightOfCells6(opponentFlippable, player, 9) >>> 9;
    
    moves = empties & (
        (opponentNearPlayer1 << 1) |
        (opponentNearPlayer8 << 8) |
        (opponentNearPlayer7 << 7) |
        (opponentNearPlayer9 << 9) |
        (opponentNearPlayer1 >>> 1) |
        (opponentNearPlayer8 >>> 8) |
        (opponentNearPlayer7 >>> 7) |
        (opponentNearPlayer9 >>> 9));
//    System.out.println(BitPattern.patternToString(moves));
    return moves;
  }
  
  public int getNMoves(long player, long opponent) {
    return Long.bitCount(getMoves(player, opponent));
  }
  
  public int getWeightedNMoves(long player, long opponent) {
    long moves = getMoves(player, opponent);
    return Long.bitCount(moves) + Long.bitCount(moves & CORNERS);
  }

  public static Board moveIfPossible(Board b, int move) {
    GetMovesCache mover = new GetMovesCache();
    long movesBit = mover.getMoves(b.getPlayer(), b.getOpponent());
    if (((1L << move) & movesBit) == 0) {
      throw new IllegalArgumentException("Invalid move.");
    }
    return b.move(mover.getFlip(move));
  }  

  public static boolean haveToPass(Board b) {
    return haveToPass(b.getPlayer(), b.getOpponent());
  }  
  
  public static boolean haveToPass(long player, long opponent) {
    long[] moves = getAllMoves(player, opponent);
    return moves != null && moves.length == 1 && moves[0] == 0;
  }

  public static long[] getAllMoves(Board b) {
    return getAllMoves(b.getPlayer(), b.getOpponent());
  }  

  public static long[] getAllMoves(long player, long opponent) {
    GetMovesCache mover = new GetMovesCache();
    long movesBit = mover.getMoves(player, opponent);
    long moves[] = new long[Long.bitCount(movesBit)];
    int move;
    int nMoves = Long.bitCount(movesBit);
    for (int i = 0; i < nMoves; ++i) {
      move = Long.numberOfTrailingZeros(movesBit);
      movesBit = movesBit & ~(1L << move);
      moves[i] = mover.getFlip(move) & ~player;
    }
    if (moves.length == 0) {
      if (mover.getMoves(opponent, player) == 0) {
        return null;
      }
      return new long[] {0};
    }
    return moves;
  }
//  public static Board[] getAllNextBoard(Board b) {
//    return getAllNextBoard(b.getPlayer(), b.getOpponent());
//  }
//  
//  public static Board[] getAllNextBoard(long player, long opponent) {
//    long[] moves = getAllMoves(player, opponent);
//    Board[] result = new Board[moves.length];
//    for (int i = 0; i < moves.length; ++i) {
//      result[i] = new Board(player, opponent);
//      result[i].moveInPlace(moves[i]);
//    }
//    return result;
//  }
//  private class Flipper0 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          GetFlip.flipHorizontalLast[GetFlip.hashRow(0, opponentNearPlayer1)] |
//          GetFlip.flipVerticalLast[GetFlip.hashColumn(0, opponentNearPlayer8)] |
//          GetFlip.flipDiagonalLast[GetFlip.hashDiagonal(0, opponentNearPlayer9)] |
//          GetFlip.flipReverseDiagonalLast[GetFlip.hashRevDiagonal(0, opponentNearPlayer7)];
//    }
//  }
////  private class Flipper0 extends Flipper {
////    @Override
////    public long run() {
////      return 
////          0x0000000000000001L
////          | ((0x0000000000000001L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
////          | /* dir = 9 */ (0x8040201008040201L & ((0x0000000000000001L + /* dir = 9 */ (opponentAfterPlayer9 | 0x7FBFDFEFF7FBFDFEL)) ^ /* dir = 9 */ (opponentAfterPlayer9 | 0x7FBFDFEFF7FBFDFEL)))
////          | /* dir = 8 */ (0x0101010101010101L & ((0x0000000000000001L + /* dir = 8 */ (opponentAfterPlayer8 | 0xFEFEFEFEFEFEFEFEL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xFEFEFEFEFEFEFEFEL)));
////    }
////  }
//
//  private class Flipper1 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000000000000002L
//          | ((0x0000000000000002L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = 9 */ (0x0080402010080402L & ((0x0000000000000002L + /* dir = 9 */ (opponentAfterPlayer9 | 0xFF7FBFDFEFF7FBFDL)) ^ /* dir = 9 */ (opponentAfterPlayer9 | 0xFF7FBFDFEFF7FBFDL)))
//          | /* dir = 8 */ (0x0202020202020202L & ((0x0000000000000002L + /* dir = 8 */ (opponentAfterPlayer8 | 0xFDFDFDFDFDFDFDFDL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xFDFDFDFDFDFDFDFDL)));
//    }
//  }
//
//  private class Flipper2 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000000000000004L
//          | ((0x0000000000000004L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = 9 */ (0x0000804020100804L & ((0x0000000000000004L + /* dir = 9 */ (opponentAfterPlayer9 | 0xFFFF7FBFDFEFF7FBL)) ^ /* dir = 9 */ (opponentAfterPlayer9 | 0xFFFF7FBFDFEFF7FBL)))
//          | /* dir = 8 */ (0x0404040404040404L & ((0x0000000000000004L + /* dir = 8 */ (opponentAfterPlayer8 | 0xFBFBFBFBFBFBFBFBL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xFBFBFBFBFBFBFBFBL)))
//          | /* dir = 7 */ (0x0000000000010204L & ((0x0000000000000004L + /* dir = 7 */ (opponentAfterPlayer7 | 0xFFFFFFFFFFFEFDFBL)) ^ /* dir = 7 */ (opponentAfterPlayer7 | 0xFFFFFFFFFFFEFDFBL)))
//          | /* dir = -1 */ getDisksRightOfCells1(opponentNearPlayer1, 0x0000000000000004L, 1);
//    }
//  }
//
//  private class Flipper3 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000000000000008L
//          | ((0x0000000000000008L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = 9 */ (0x0000008040201008L & ((0x0000000000000008L + /* dir = 9 */ (opponentAfterPlayer9 | 0xFFFFFF7FBFDFEFF7L)) ^ /* dir = 9 */ (opponentAfterPlayer9 | 0xFFFFFF7FBFDFEFF7L)))
//          | /* dir = 8 */ (0x0808080808080808L & ((0x0000000000000008L + /* dir = 8 */ (opponentAfterPlayer8 | 0xF7F7F7F7F7F7F7F7L)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xF7F7F7F7F7F7F7F7L)))
//          | /* dir = 7 */ (0x0000000001020408L & ((0x0000000000000008L + /* dir = 7 */ (opponentAfterPlayer7 | 0xFFFFFFFFFEFDFBF7L)) ^ /* dir = 7 */ (opponentAfterPlayer7 | 0xFFFFFFFFFEFDFBF7L)))
//          | /* dir = -1 */ getDisksRightOfCells2(opponentNearPlayer1, 0x0000000000000008L, 1);
//    }
//  }
//
//  private class Flipper4 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000000000000010L
//          | ((0x0000000000000010L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = 9 */ (0x0000000080402010L & ((0x0000000000000010L + /* dir = 9 */ (opponentAfterPlayer9 | 0xFFFFFFFF7FBFDFEFL)) ^ /* dir = 9 */ (opponentAfterPlayer9 | 0xFFFFFFFF7FBFDFEFL)))
//          | /* dir = 8 */ (0x1010101010101010L & ((0x0000000000000010L + /* dir = 8 */ (opponentAfterPlayer8 | 0xEFEFEFEFEFEFEFEFL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xEFEFEFEFEFEFEFEFL)))
//          | /* dir = 7 */ (0x0000000102040810L & ((0x0000000000000010L + /* dir = 7 */ (opponentAfterPlayer7 | 0xFFFFFFFEFDFBF7EFL)) ^ /* dir = 7 */ (opponentAfterPlayer7 | 0xFFFFFFFEFDFBF7EFL)))
//          | /* dir = -1 */ getDisksRightOfCells3(opponentNearPlayer1, 0x0000000000000010L, 1);
//    }
//  }
//
//  private class Flipper5 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000000000000020L
//          | ((0x0000000000000020L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = 9 */ (0x0000000000804020L & ((0x0000000000000020L + /* dir = 9 */ (opponentAfterPlayer9 | 0xFFFFFFFFFF7FBFDFL)) ^ /* dir = 9 */ (opponentAfterPlayer9 | 0xFFFFFFFFFF7FBFDFL)))
//          | /* dir = 8 */ (0x2020202020202020L & ((0x0000000000000020L + /* dir = 8 */ (opponentAfterPlayer8 | 0xDFDFDFDFDFDFDFDFL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xDFDFDFDFDFDFDFDFL)))
//          | /* dir = 7 */ (0x0000010204081020L & ((0x0000000000000020L + /* dir = 7 */ (opponentAfterPlayer7 | 0xFFFFFEFDFBF7EFDFL)) ^ /* dir = 7 */ (opponentAfterPlayer7 | 0xFFFFFEFDFBF7EFDFL)))
//          | /* dir = -1 */ getDisksRightOfCells4(opponentNearPlayer1, 0x0000000000000020L, 1);
//    }
//  }
//
//  private class Flipper6 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000000000000040L
//          | /* dir = 8 */ (0x4040404040404040L & ((0x0000000000000040L + /* dir = 8 */ (opponentAfterPlayer8 | 0xBFBFBFBFBFBFBFBFL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xBFBFBFBFBFBFBFBFL)))
//          | /* dir = 7 */ (0x0001020408102040L & ((0x0000000000000040L + /* dir = 7 */ (opponentAfterPlayer7 | 0xFFFEFDFBF7EFDFBFL)) ^ /* dir = 7 */ (opponentAfterPlayer7 | 0xFFFEFDFBF7EFDFBFL)))
//          | /* dir = -1 */ getDisksRightOfCells5(opponentNearPlayer1, 0x0000000000000040L, 1);
//    }
//  }
//
//  private class Flipper7 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000000000000080L
//          | /* dir = 8 */ (0x8080808080808080L & ((0x0000000000000080L + /* dir = 8 */ (opponentAfterPlayer8 | 0x7F7F7F7F7F7F7F7FL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0x7F7F7F7F7F7F7F7FL)))
//          | /* dir = 7 */ (0x0102040810204080L & ((0x0000000000000080L + /* dir = 7 */ (opponentAfterPlayer7 | 0xFEFDFBF7EFDFBF7FL)) ^ /* dir = 7 */ (opponentAfterPlayer7 | 0xFEFDFBF7EFDFBF7FL)))
//          | /* dir = -1 */ getDisksRightOfCells6(opponentNearPlayer1, 0x0000000000000080L, 1);
//    }
//  }
//
//  private class Flipper8 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000000000000100L
//          | ((0x0000000000000100L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = 9 */ (0x4020100804020100L & ((0x0000000000000100L + /* dir = 9 */ (opponentAfterPlayer9 | 0xBFDFEFF7FBFDFEFFL)) ^ /* dir = 9 */ (opponentAfterPlayer9 | 0xBFDFEFF7FBFDFEFFL)))
//          | /* dir = 8 */ (0x0101010101010101L & ((0x0000000000000100L + /* dir = 8 */ (opponentAfterPlayer8 | 0xFEFEFEFEFEFEFEFEL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xFEFEFEFEFEFEFEFEL)));
//    }
//  }
//
//  private class Flipper9 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000000000000200L
//          | ((0x0000000000000200L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = 9 */ (0x8040201008040201L & ((0x0000000000000200L + /* dir = 9 */ (opponentAfterPlayer9 | 0x7FBFDFEFF7FBFDFEL)) ^ /* dir = 9 */ (opponentAfterPlayer9 | 0x7FBFDFEFF7FBFDFEL)))
//          | /* dir = 8 */ (0x0202020202020202L & ((0x0000000000000200L + /* dir = 8 */ (opponentAfterPlayer8 | 0xFDFDFDFDFDFDFDFDL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xFDFDFDFDFDFDFDFDL)));
//    }
//  }
//
//  private class Flipper10 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000000000000400L
//          | ((0x0000000000000400L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = 9 */ (0x0080402010080402L & ((0x0000000000000400L + /* dir = 9 */ (opponentAfterPlayer9 | 0xFF7FBFDFEFF7FBFDL)) ^ /* dir = 9 */ (opponentAfterPlayer9 | 0xFF7FBFDFEFF7FBFDL)))
//          | /* dir = 8 */ (0x0404040404040404L & ((0x0000000000000400L + /* dir = 8 */ (opponentAfterPlayer8 | 0xFBFBFBFBFBFBFBFBL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xFBFBFBFBFBFBFBFBL)))
//          | /* dir = 7 */ (0x0000000001020408L & ((0x0000000000000400L + /* dir = 7 */ (opponentAfterPlayer7 | 0xFFFFFFFFFEFDFBF7L)) ^ /* dir = 7 */ (opponentAfterPlayer7 | 0xFFFFFFFFFEFDFBF7L)))
//          | /* dir = -1 */ getDisksRightOfCells1(opponentNearPlayer1, 0x0000000000000400L, 1);
//    }
//  }
//
//  private class Flipper11 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000000000000800L
//          | ((0x0000000000000800L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = 9 */ (0x0000804020100804L & ((0x0000000000000800L + /* dir = 9 */ (opponentAfterPlayer9 | 0xFFFF7FBFDFEFF7FBL)) ^ /* dir = 9 */ (opponentAfterPlayer9 | 0xFFFF7FBFDFEFF7FBL)))
//          | /* dir = 8 */ (0x0808080808080808L & ((0x0000000000000800L + /* dir = 8 */ (opponentAfterPlayer8 | 0xF7F7F7F7F7F7F7F7L)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xF7F7F7F7F7F7F7F7L)))
//          | /* dir = 7 */ (0x0000000102040810L & ((0x0000000000000800L + /* dir = 7 */ (opponentAfterPlayer7 | 0xFFFFFFFEFDFBF7EFL)) ^ /* dir = 7 */ (opponentAfterPlayer7 | 0xFFFFFFFEFDFBF7EFL)))
//          | /* dir = -1 */ getDisksRightOfCells2(opponentNearPlayer1, 0x0000000000000800L, 1);
//    }
//  }
//
//  private class Flipper12 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000000000001000L
//          | ((0x0000000000001000L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = 9 */ (0x0000008040201008L & ((0x0000000000001000L + /* dir = 9 */ (opponentAfterPlayer9 | 0xFFFFFF7FBFDFEFF7L)) ^ /* dir = 9 */ (opponentAfterPlayer9 | 0xFFFFFF7FBFDFEFF7L)))
//          | /* dir = 8 */ (0x1010101010101010L & ((0x0000000000001000L + /* dir = 8 */ (opponentAfterPlayer8 | 0xEFEFEFEFEFEFEFEFL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xEFEFEFEFEFEFEFEFL)))
//          | /* dir = 7 */ (0x0000010204081020L & ((0x0000000000001000L + /* dir = 7 */ (opponentAfterPlayer7 | 0xFFFFFEFDFBF7EFDFL)) ^ /* dir = 7 */ (opponentAfterPlayer7 | 0xFFFFFEFDFBF7EFDFL)))
//          | /* dir = -1 */ getDisksRightOfCells3(opponentNearPlayer1, 0x0000000000001000L, 1);
//    }
//  }
//
//  private class Flipper13 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000000000002000L
//          | ((0x0000000000002000L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = 9 */ (0x0000000080402010L & ((0x0000000000002000L + /* dir = 9 */ (opponentAfterPlayer9 | 0xFFFFFFFF7FBFDFEFL)) ^ /* dir = 9 */ (opponentAfterPlayer9 | 0xFFFFFFFF7FBFDFEFL)))
//          | /* dir = 8 */ (0x2020202020202020L & ((0x0000000000002000L + /* dir = 8 */ (opponentAfterPlayer8 | 0xDFDFDFDFDFDFDFDFL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xDFDFDFDFDFDFDFDFL)))
//          | /* dir = 7 */ (0x0001020408102040L & ((0x0000000000002000L + /* dir = 7 */ (opponentAfterPlayer7 | 0xFFFEFDFBF7EFDFBFL)) ^ /* dir = 7 */ (opponentAfterPlayer7 | 0xFFFEFDFBF7EFDFBFL)))
//          | /* dir = -1 */ getDisksRightOfCells4(opponentNearPlayer1, 0x0000000000002000L, 1);
//    }
//  }
//
//  private class Flipper14 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000000000004000L
//          | /* dir = 8 */ (0x4040404040404040L & ((0x0000000000004000L + /* dir = 8 */ (opponentAfterPlayer8 | 0xBFBFBFBFBFBFBFBFL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xBFBFBFBFBFBFBFBFL)))
//          | /* dir = 7 */ (0x0102040810204080L & ((0x0000000000004000L + /* dir = 7 */ (opponentAfterPlayer7 | 0xFEFDFBF7EFDFBF7FL)) ^ /* dir = 7 */ (opponentAfterPlayer7 | 0xFEFDFBF7EFDFBF7FL)))
//          | /* dir = -1 */ getDisksRightOfCells5(opponentNearPlayer1, 0x0000000000004000L, 1);
//    }
//  }
//
//  private class Flipper15 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000000000008000L
//          | /* dir = 8 */ (0x8080808080808080L & ((0x0000000000008000L + /* dir = 8 */ (opponentAfterPlayer8 | 0x7F7F7F7F7F7F7F7FL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0x7F7F7F7F7F7F7F7FL)))
//          | /* dir = 7 */ (0x0204081020408000L & ((0x0000000000008000L + /* dir = 7 */ (opponentAfterPlayer7 | 0xFDFBF7EFDFBF7FFFL)) ^ /* dir = 7 */ (opponentAfterPlayer7 | 0xFDFBF7EFDFBF7FFFL)))
//          | /* dir = -1 */ getDisksRightOfCells6(opponentNearPlayer1, 0x0000000000008000L, 1);
//    }
//  }
//
//  private class Flipper16 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000000000010000L
//          | ((0x0000000000010000L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = 9 */ (0x2010080402010000L & ((0x0000000000010000L + /* dir = 9 */ (opponentAfterPlayer9 | 0xDFEFF7FBFDFEFFFFL)) ^ /* dir = 9 */ (opponentAfterPlayer9 | 0xDFEFF7FBFDFEFFFFL)))
//          | /* dir = 8 */ (0x0101010101010101L & ((0x0000000000010000L + /* dir = 8 */ (opponentAfterPlayer8 | 0xFEFEFEFEFEFEFEFEL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xFEFEFEFEFEFEFEFEL)))
//          | /* dir = -7 */ getDisksRightOfCells1(opponentNearPlayer7, 0x0000000000010000L, 7)
//          | /* dir = -8 */ getDisksRightOfCells1(opponentNearPlayer8, 0x0000000000010000L, 8);
//    }
//  }
//
//  private class Flipper17 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000000000020000L
//          | ((0x0000000000020000L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = 9 */ (0x4020100804020100L & ((0x0000000000020000L + /* dir = 9 */ (opponentAfterPlayer9 | 0xBFDFEFF7FBFDFEFFL)) ^ /* dir = 9 */ (opponentAfterPlayer9 | 0xBFDFEFF7FBFDFEFFL)))
//          | /* dir = 8 */ (0x0202020202020202L & ((0x0000000000020000L + /* dir = 8 */ (opponentAfterPlayer8 | 0xFDFDFDFDFDFDFDFDL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xFDFDFDFDFDFDFDFDL)))
//          | /* dir = -7 */ getDisksRightOfCells1(opponentNearPlayer7, 0x0000000000020000L, 7)
//          | /* dir = -8 */ getDisksRightOfCells1(opponentNearPlayer8, 0x0000000000020000L, 8);
//    }
//  }
//
//  private class Flipper18 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000000000040000L
//          | ((0x0000000000040000L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = 9 */ (0x8040201008040201L & ((0x0000000000040000L + /* dir = 9 */ (opponentAfterPlayer9 | 0x7FBFDFEFF7FBFDFEL)) ^ /* dir = 9 */ (opponentAfterPlayer9 | 0x7FBFDFEFF7FBFDFEL)))
//          | /* dir = 8 */ (0x0404040404040404L & ((0x0000000000040000L + /* dir = 8 */ (opponentAfterPlayer8 | 0xFBFBFBFBFBFBFBFBL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xFBFBFBFBFBFBFBFBL)))
//          | /* dir = 7 */ (0x0000000102040810L & ((0x0000000000040000L + /* dir = 7 */ (opponentAfterPlayer7 | 0xFFFFFFFEFDFBF7EFL)) ^ /* dir = 7 */ (opponentAfterPlayer7 | 0xFFFFFFFEFDFBF7EFL)))
//          | /* dir = -1 */ getDisksRightOfCells1(opponentNearPlayer1, 0x0000000000040000L, 1)
//          | /* dir = -7 */ getDisksRightOfCells1(opponentNearPlayer7, 0x0000000000040000L, 7)
//          | /* dir = -8 */ getDisksRightOfCells1(opponentNearPlayer8, 0x0000000000040000L, 8)
//          | /* dir = -9 */ getDisksRightOfCells1(opponentNearPlayer9, 0x0000000000040000L, 9);
//    }
//  }
//
//  private class Flipper19 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000000000080000L
//          | ((0x0000000000080000L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = 9 */ (0x0080402010080402L & ((0x0000000000080000L + /* dir = 9 */ (opponentAfterPlayer9 | 0xFF7FBFDFEFF7FBFDL)) ^ /* dir = 9 */ (opponentAfterPlayer9 | 0xFF7FBFDFEFF7FBFDL)))
//          | /* dir = 8 */ (0x0808080808080808L & ((0x0000000000080000L + /* dir = 8 */ (opponentAfterPlayer8 | 0xF7F7F7F7F7F7F7F7L)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xF7F7F7F7F7F7F7F7L)))
//          | /* dir = 7 */ (0x0000010204081020L & ((0x0000000000080000L + /* dir = 7 */ (opponentAfterPlayer7 | 0xFFFFFEFDFBF7EFDFL)) ^ /* dir = 7 */ (opponentAfterPlayer7 | 0xFFFFFEFDFBF7EFDFL)))
//          | /* dir = -1 */ getDisksRightOfCells2(opponentNearPlayer1, 0x0000000000080000L, 1)
//          | /* dir = -7 */ getDisksRightOfCells1(opponentNearPlayer7, 0x0000000000080000L, 7)
//          | /* dir = -8 */ getDisksRightOfCells1(opponentNearPlayer8, 0x0000000000080000L, 8)
//          | /* dir = -9 */ getDisksRightOfCells1(opponentNearPlayer9, 0x0000000000080000L, 9);
//    }
//  }
//
//  private class Flipper20 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000000000100000L
//          | ((0x0000000000100000L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = 9 */ (0x0000804020100804L & ((0x0000000000100000L + /* dir = 9 */ (opponentAfterPlayer9 | 0xFFFF7FBFDFEFF7FBL)) ^ /* dir = 9 */ (opponentAfterPlayer9 | 0xFFFF7FBFDFEFF7FBL)))
//          | /* dir = 8 */ (0x1010101010101010L & ((0x0000000000100000L + /* dir = 8 */ (opponentAfterPlayer8 | 0xEFEFEFEFEFEFEFEFL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xEFEFEFEFEFEFEFEFL)))
//          | /* dir = 7 */ (0x0001020408102040L & ((0x0000000000100000L + /* dir = 7 */ (opponentAfterPlayer7 | 0xFFFEFDFBF7EFDFBFL)) ^ /* dir = 7 */ (opponentAfterPlayer7 | 0xFFFEFDFBF7EFDFBFL)))
//          | /* dir = -1 */ getDisksRightOfCells3(opponentNearPlayer1, 0x0000000000100000L, 1)
//          | /* dir = -7 */ getDisksRightOfCells1(opponentNearPlayer7, 0x0000000000100000L, 7)
//          | /* dir = -8 */ getDisksRightOfCells1(opponentNearPlayer8, 0x0000000000100000L, 8)
//          | /* dir = -9 */ getDisksRightOfCells1(opponentNearPlayer9, 0x0000000000100000L, 9);
//    }
//  }
//
//  private class Flipper21 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000000000200000L
//          | ((0x0000000000200000L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = 9 */ (0x0000008040201008L & ((0x0000000000200000L + /* dir = 9 */ (opponentAfterPlayer9 | 0xFFFFFF7FBFDFEFF7L)) ^ /* dir = 9 */ (opponentAfterPlayer9 | 0xFFFFFF7FBFDFEFF7L)))
//          | /* dir = 8 */ (0x2020202020202020L & ((0x0000000000200000L + /* dir = 8 */ (opponentAfterPlayer8 | 0xDFDFDFDFDFDFDFDFL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xDFDFDFDFDFDFDFDFL)))
//          | /* dir = 7 */ (0x0102040810204080L & ((0x0000000000200000L + /* dir = 7 */ (opponentAfterPlayer7 | 0xFEFDFBF7EFDFBF7FL)) ^ /* dir = 7 */ (opponentAfterPlayer7 | 0xFEFDFBF7EFDFBF7FL)))
//          | /* dir = -1 */ getDisksRightOfCells4(opponentNearPlayer1, 0x0000000000200000L, 1)
//          | /* dir = -7 */ getDisksRightOfCells1(opponentNearPlayer7, 0x0000000000200000L, 7)
//          | /* dir = -8 */ getDisksRightOfCells1(opponentNearPlayer8, 0x0000000000200000L, 8)
//          | /* dir = -9 */ getDisksRightOfCells1(opponentNearPlayer9, 0x0000000000200000L, 9);
//    }
//  }
//
//  private class Flipper22 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000000000400000L
//          | /* dir = 8 */ (0x4040404040404040L & ((0x0000000000400000L + /* dir = 8 */ (opponentAfterPlayer8 | 0xBFBFBFBFBFBFBFBFL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xBFBFBFBFBFBFBFBFL)))
//          | /* dir = 7 */ (0x0204081020408000L & ((0x0000000000400000L + /* dir = 7 */ (opponentAfterPlayer7 | 0xFDFBF7EFDFBF7FFFL)) ^ /* dir = 7 */ (opponentAfterPlayer7 | 0xFDFBF7EFDFBF7FFFL)))
//          | /* dir = -1 */ getDisksRightOfCells5(opponentNearPlayer1, 0x0000000000400000L, 1)
//          | /* dir = -8 */ getDisksRightOfCells1(opponentNearPlayer8, 0x0000000000400000L, 8)
//          | /* dir = -9 */ getDisksRightOfCells1(opponentNearPlayer9, 0x0000000000400000L, 9);
//    }
//  }
//
//  private class Flipper23 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000000000800000L
//          | /* dir = 8 */ (0x8080808080808080L & ((0x0000000000800000L + /* dir = 8 */ (opponentAfterPlayer8 | 0x7F7F7F7F7F7F7F7FL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0x7F7F7F7F7F7F7F7FL)))
//          | /* dir = 7 */ (0x0408102040800000L & ((0x0000000000800000L + /* dir = 7 */ (opponentAfterPlayer7 | 0xFBF7EFDFBF7FFFFFL)) ^ /* dir = 7 */ (opponentAfterPlayer7 | 0xFBF7EFDFBF7FFFFFL)))
//          | /* dir = -1 */ getDisksRightOfCells6(opponentNearPlayer1, 0x0000000000800000L, 1)
//          | /* dir = -8 */ getDisksRightOfCells1(opponentNearPlayer8, 0x0000000000800000L, 8)
//          | /* dir = -9 */ getDisksRightOfCells1(opponentNearPlayer9, 0x0000000000800000L, 9);
//    }
//  }
//
//  private class Flipper24 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000000001000000L
//          | ((0x0000000001000000L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = 9 */ (0x1008040201000000L & ((0x0000000001000000L + /* dir = 9 */ (opponentAfterPlayer9 | 0xEFF7FBFDFEFFFFFFL)) ^ /* dir = 9 */ (opponentAfterPlayer9 | 0xEFF7FBFDFEFFFFFFL)))
//          | /* dir = 8 */ (0x0101010101010101L & ((0x0000000001000000L + /* dir = 8 */ (opponentAfterPlayer8 | 0xFEFEFEFEFEFEFEFEL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xFEFEFEFEFEFEFEFEL)))
//          | /* dir = -7 */ getDisksRightOfCells2(opponentNearPlayer7, 0x0000000001000000L, 7)
//          | /* dir = -8 */ getDisksRightOfCells2(opponentNearPlayer8, 0x0000000001000000L, 8);
//    }
//  }
//
//  private class Flipper25 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000000002000000L
//          | ((0x0000000002000000L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = 9 */ (0x2010080402010000L & ((0x0000000002000000L + /* dir = 9 */ (opponentAfterPlayer9 | 0xDFEFF7FBFDFEFFFFL)) ^ /* dir = 9 */ (opponentAfterPlayer9 | 0xDFEFF7FBFDFEFFFFL)))
//          | /* dir = 8 */ (0x0202020202020202L & ((0x0000000002000000L + /* dir = 8 */ (opponentAfterPlayer8 | 0xFDFDFDFDFDFDFDFDL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xFDFDFDFDFDFDFDFDL)))
//          | /* dir = -7 */ getDisksRightOfCells2(opponentNearPlayer7, 0x0000000002000000L, 7)
//          | /* dir = -8 */ getDisksRightOfCells2(opponentNearPlayer8, 0x0000000002000000L, 8);
//    }
//  }
//
//  private class Flipper26 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000000004000000L
//          | ((0x0000000004000000L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = 9 */ (0x4020100804020100L & ((0x0000000004000000L + /* dir = 9 */ (opponentAfterPlayer9 | 0xBFDFEFF7FBFDFEFFL)) ^ /* dir = 9 */ (opponentAfterPlayer9 | 0xBFDFEFF7FBFDFEFFL)))
//          | /* dir = 8 */ (0x0404040404040404L & ((0x0000000004000000L + /* dir = 8 */ (opponentAfterPlayer8 | 0xFBFBFBFBFBFBFBFBL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xFBFBFBFBFBFBFBFBL)))
//          | /* dir = 7 */ (0x0000010204081020L & ((0x0000000004000000L + /* dir = 7 */ (opponentAfterPlayer7 | 0xFFFFFEFDFBF7EFDFL)) ^ /* dir = 7 */ (opponentAfterPlayer7 | 0xFFFFFEFDFBF7EFDFL)))
//          | /* dir = -1 */ getDisksRightOfCells1(opponentNearPlayer1, 0x0000000004000000L, 1)
//          | /* dir = -7 */ getDisksRightOfCells2(opponentNearPlayer7, 0x0000000004000000L, 7)
//          | /* dir = -8 */ getDisksRightOfCells2(opponentNearPlayer8, 0x0000000004000000L, 8)
//          | /* dir = -9 */ getDisksRightOfCells1(opponentNearPlayer9, 0x0000000004000000L, 9);
//    }
//  }
//
//  private class Flipper27 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000000008000000L
//          | ((0x0000000008000000L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = 9 */ (0x8040201008040201L & ((0x0000000008000000L + /* dir = 9 */ (opponentAfterPlayer9 | 0x7FBFDFEFF7FBFDFEL)) ^ /* dir = 9 */ (opponentAfterPlayer9 | 0x7FBFDFEFF7FBFDFEL)))
//          | /* dir = 8 */ (0x0808080808080808L & ((0x0000000008000000L + /* dir = 8 */ (opponentAfterPlayer8 | 0xF7F7F7F7F7F7F7F7L)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xF7F7F7F7F7F7F7F7L)))
//          | /* dir = 7 */ (0x0001020408102040L & ((0x0000000008000000L + /* dir = 7 */ (opponentAfterPlayer7 | 0xFFFEFDFBF7EFDFBFL)) ^ /* dir = 7 */ (opponentAfterPlayer7 | 0xFFFEFDFBF7EFDFBFL)))
//          | /* dir = -1 */ getDisksRightOfCells2(opponentNearPlayer1, 0x0000000008000000L, 1)
//          | /* dir = -7 */ getDisksRightOfCells2(opponentNearPlayer7, 0x0000000008000000L, 7)
//          | /* dir = -8 */ getDisksRightOfCells2(opponentNearPlayer8, 0x0000000008000000L, 8)
//          | /* dir = -9 */ getDisksRightOfCells2(opponentNearPlayer9, 0x0000000008000000L, 9);
//    }
//  }
//
//  private class Flipper28 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000000010000000L
//          | ((0x0000000010000000L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = 9 */ (0x0080402010080402L & ((0x0000000010000000L + /* dir = 9 */ (opponentAfterPlayer9 | 0xFF7FBFDFEFF7FBFDL)) ^ /* dir = 9 */ (opponentAfterPlayer9 | 0xFF7FBFDFEFF7FBFDL)))
//          | /* dir = 8 */ (0x1010101010101010L & ((0x0000000010000000L + /* dir = 8 */ (opponentAfterPlayer8 | 0xEFEFEFEFEFEFEFEFL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xEFEFEFEFEFEFEFEFL)))
//          | /* dir = 7 */ (0x0102040810204080L & ((0x0000000010000000L + /* dir = 7 */ (opponentAfterPlayer7 | 0xFEFDFBF7EFDFBF7FL)) ^ /* dir = 7 */ (opponentAfterPlayer7 | 0xFEFDFBF7EFDFBF7FL)))
//          | /* dir = -1 */ getDisksRightOfCells3(opponentNearPlayer1, 0x0000000010000000L, 1)
//          | /* dir = -7 */ getDisksRightOfCells2(opponentNearPlayer7, 0x0000000010000000L, 7)
//          | /* dir = -8 */ getDisksRightOfCells2(opponentNearPlayer8, 0x0000000010000000L, 8)
//          | /* dir = -9 */ getDisksRightOfCells2(opponentNearPlayer9, 0x0000000010000000L, 9);
//    }
//  }
//
//  private class Flipper29 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000000020000000L
//          | ((0x0000000020000000L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = 9 */ (0x0000804020100804L & ((0x0000000020000000L + /* dir = 9 */ (opponentAfterPlayer9 | 0xFFFF7FBFDFEFF7FBL)) ^ /* dir = 9 */ (opponentAfterPlayer9 | 0xFFFF7FBFDFEFF7FBL)))
//          | /* dir = 8 */ (0x2020202020202020L & ((0x0000000020000000L + /* dir = 8 */ (opponentAfterPlayer8 | 0xDFDFDFDFDFDFDFDFL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xDFDFDFDFDFDFDFDFL)))
//          | /* dir = 7 */ (0x0204081020408000L & ((0x0000000020000000L + /* dir = 7 */ (opponentAfterPlayer7 | 0xFDFBF7EFDFBF7FFFL)) ^ /* dir = 7 */ (opponentAfterPlayer7 | 0xFDFBF7EFDFBF7FFFL)))
//          | /* dir = -1 */ getDisksRightOfCells4(opponentNearPlayer1, 0x0000000020000000L, 1)
//          | /* dir = -7 */ getDisksRightOfCells1(opponentNearPlayer7, 0x0000000020000000L, 7)
//          | /* dir = -8 */ getDisksRightOfCells2(opponentNearPlayer8, 0x0000000020000000L, 8)
//          | /* dir = -9 */ getDisksRightOfCells2(opponentNearPlayer9, 0x0000000020000000L, 9);
//    }
//  }
//
//  private class Flipper30 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000000040000000L
//          | /* dir = 8 */ (0x4040404040404040L & ((0x0000000040000000L + /* dir = 8 */ (opponentAfterPlayer8 | 0xBFBFBFBFBFBFBFBFL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xBFBFBFBFBFBFBFBFL)))
//          | /* dir = 7 */ (0x0408102040800000L & ((0x0000000040000000L + /* dir = 7 */ (opponentAfterPlayer7 | 0xFBF7EFDFBF7FFFFFL)) ^ /* dir = 7 */ (opponentAfterPlayer7 | 0xFBF7EFDFBF7FFFFFL)))
//          | /* dir = -1 */ getDisksRightOfCells5(opponentNearPlayer1, 0x0000000040000000L, 1)
//          | /* dir = -8 */ getDisksRightOfCells2(opponentNearPlayer8, 0x0000000040000000L, 8)
//          | /* dir = -9 */ getDisksRightOfCells2(opponentNearPlayer9, 0x0000000040000000L, 9);
//    }
//  }
//
//  private class Flipper31 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000000080000000L
//          | /* dir = 8 */ (0x8080808080808080L & ((0x0000000080000000L + /* dir = 8 */ (opponentAfterPlayer8 | 0x7F7F7F7F7F7F7F7FL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0x7F7F7F7F7F7F7F7FL)))
//          | /* dir = 7 */ (0x0810204080000000L & ((0x0000000080000000L + /* dir = 7 */ (opponentAfterPlayer7 | 0xF7EFDFBF7FFFFFFFL)) ^ /* dir = 7 */ (opponentAfterPlayer7 | 0xF7EFDFBF7FFFFFFFL)))
//          | /* dir = -1 */ getDisksRightOfCells6(opponentNearPlayer1, 0x0000000080000000L, 1)
//          | /* dir = -8 */ getDisksRightOfCells2(opponentNearPlayer8, 0x0000000080000000L, 8)
//          | /* dir = -9 */ getDisksRightOfCells2(opponentNearPlayer9, 0x0000000080000000L, 9);
//    }
//  }
//
//  private class Flipper32 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000000100000000L
//          | ((0x0000000100000000L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = 9 */ (0x0804020100000000L & ((0x0000000100000000L + /* dir = 9 */ (opponentAfterPlayer9 | 0xF7FBFDFEFFFFFFFFL)) ^ /* dir = 9 */ (opponentAfterPlayer9 | 0xF7FBFDFEFFFFFFFFL)))
//          | /* dir = 8 */ (0x0101010101010101L & ((0x0000000100000000L + /* dir = 8 */ (opponentAfterPlayer8 | 0xFEFEFEFEFEFEFEFEL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xFEFEFEFEFEFEFEFEL)))
//          | /* dir = -7 */ getDisksRightOfCells3(opponentNearPlayer7, 0x0000000100000000L, 7)
//          | /* dir = -8 */ getDisksRightOfCells3(opponentNearPlayer8, 0x0000000100000000L, 8);
//    }
//  }
//
//  private class Flipper33 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000000200000000L
//          | ((0x0000000200000000L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = 9 */ (0x1008040201000000L & ((0x0000000200000000L + /* dir = 9 */ (opponentAfterPlayer9 | 0xEFF7FBFDFEFFFFFFL)) ^ /* dir = 9 */ (opponentAfterPlayer9 | 0xEFF7FBFDFEFFFFFFL)))
//          | /* dir = 8 */ (0x0202020202020202L & ((0x0000000200000000L + /* dir = 8 */ (opponentAfterPlayer8 | 0xFDFDFDFDFDFDFDFDL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xFDFDFDFDFDFDFDFDL)))
//          | /* dir = -7 */ getDisksRightOfCells3(opponentNearPlayer7, 0x0000000200000000L, 7)
//          | /* dir = -8 */ getDisksRightOfCells3(opponentNearPlayer8, 0x0000000200000000L, 8);
//    }
//  }
//
//  private class Flipper34 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000000400000000L
//          | ((0x0000000400000000L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = 9 */ (0x2010080402010000L & ((0x0000000400000000L + /* dir = 9 */ (opponentAfterPlayer9 | 0xDFEFF7FBFDFEFFFFL)) ^ /* dir = 9 */ (opponentAfterPlayer9 | 0xDFEFF7FBFDFEFFFFL)))
//          | /* dir = 8 */ (0x0404040404040404L & ((0x0000000400000000L + /* dir = 8 */ (opponentAfterPlayer8 | 0xFBFBFBFBFBFBFBFBL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xFBFBFBFBFBFBFBFBL)))
//          | /* dir = 7 */ (0x0001020408102040L & ((0x0000000400000000L + /* dir = 7 */ (opponentAfterPlayer7 | 0xFFFEFDFBF7EFDFBFL)) ^ /* dir = 7 */ (opponentAfterPlayer7 | 0xFFFEFDFBF7EFDFBFL)))
//          | /* dir = -1 */ getDisksRightOfCells1(opponentNearPlayer1, 0x0000000400000000L, 1)
//          | /* dir = -7 */ getDisksRightOfCells3(opponentNearPlayer7, 0x0000000400000000L, 7)
//          | /* dir = -8 */ getDisksRightOfCells3(opponentNearPlayer8, 0x0000000400000000L, 8)
//          | /* dir = -9 */ getDisksRightOfCells1(opponentNearPlayer9, 0x0000000400000000L, 9);
//    }
//  }
//
//  private class Flipper35 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000000800000000L
//          | ((0x0000000800000000L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = 9 */ (0x4020100804020100L & ((0x0000000800000000L + /* dir = 9 */ (opponentAfterPlayer9 | 0xBFDFEFF7FBFDFEFFL)) ^ /* dir = 9 */ (opponentAfterPlayer9 | 0xBFDFEFF7FBFDFEFFL)))
//          | /* dir = 8 */ (0x0808080808080808L & ((0x0000000800000000L + /* dir = 8 */ (opponentAfterPlayer8 | 0xF7F7F7F7F7F7F7F7L)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xF7F7F7F7F7F7F7F7L)))
//          | /* dir = 7 */ (0x0102040810204080L & ((0x0000000800000000L + /* dir = 7 */ (opponentAfterPlayer7 | 0xFEFDFBF7EFDFBF7FL)) ^ /* dir = 7 */ (opponentAfterPlayer7 | 0xFEFDFBF7EFDFBF7FL)))
//          | /* dir = -1 */ getDisksRightOfCells2(opponentNearPlayer1, 0x0000000800000000L, 1)
//          | /* dir = -7 */ getDisksRightOfCells3(opponentNearPlayer7, 0x0000000800000000L, 7)
//          | /* dir = -8 */ getDisksRightOfCells3(opponentNearPlayer8, 0x0000000800000000L, 8)
//          | /* dir = -9 */ getDisksRightOfCells2(opponentNearPlayer9, 0x0000000800000000L, 9);
//    }
//  }
//
//  private class Flipper36 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000001000000000L
//          | ((0x0000001000000000L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = 9 */ (0x8040201008040201L & ((0x0000001000000000L + /* dir = 9 */ (opponentAfterPlayer9 | 0x7FBFDFEFF7FBFDFEL)) ^ /* dir = 9 */ (opponentAfterPlayer9 | 0x7FBFDFEFF7FBFDFEL)))
//          | /* dir = 8 */ (0x1010101010101010L & ((0x0000001000000000L + /* dir = 8 */ (opponentAfterPlayer8 | 0xEFEFEFEFEFEFEFEFL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xEFEFEFEFEFEFEFEFL)))
//          | /* dir = 7 */ (0x0204081020408000L & ((0x0000001000000000L + /* dir = 7 */ (opponentAfterPlayer7 | 0xFDFBF7EFDFBF7FFFL)) ^ /* dir = 7 */ (opponentAfterPlayer7 | 0xFDFBF7EFDFBF7FFFL)))
//          | /* dir = -1 */ getDisksRightOfCells3(opponentNearPlayer1, 0x0000001000000000L, 1)
//          | /* dir = -7 */ getDisksRightOfCells2(opponentNearPlayer7, 0x0000001000000000L, 7)
//          | /* dir = -8 */ getDisksRightOfCells3(opponentNearPlayer8, 0x0000001000000000L, 8)
//          | /* dir = -9 */ getDisksRightOfCells3(opponentNearPlayer9, 0x0000001000000000L, 9);
//    }
//  }
//
//  private class Flipper37 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000002000000000L
//          | ((0x0000002000000000L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = 9 */ (0x0080402010080402L & ((0x0000002000000000L + /* dir = 9 */ (opponentAfterPlayer9 | 0xFF7FBFDFEFF7FBFDL)) ^ /* dir = 9 */ (opponentAfterPlayer9 | 0xFF7FBFDFEFF7FBFDL)))
//          | /* dir = 8 */ (0x2020202020202020L & ((0x0000002000000000L + /* dir = 8 */ (opponentAfterPlayer8 | 0xDFDFDFDFDFDFDFDFL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xDFDFDFDFDFDFDFDFL)))
//          | /* dir = 7 */ (0x0408102040800000L & ((0x0000002000000000L + /* dir = 7 */ (opponentAfterPlayer7 | 0xFBF7EFDFBF7FFFFFL)) ^ /* dir = 7 */ (opponentAfterPlayer7 | 0xFBF7EFDFBF7FFFFFL)))
//          | /* dir = -1 */ getDisksRightOfCells4(opponentNearPlayer1, 0x0000002000000000L, 1)
//          | /* dir = -7 */ getDisksRightOfCells1(opponentNearPlayer7, 0x0000002000000000L, 7)
//          | /* dir = -8 */ getDisksRightOfCells3(opponentNearPlayer8, 0x0000002000000000L, 8)
//          | /* dir = -9 */ getDisksRightOfCells3(opponentNearPlayer9, 0x0000002000000000L, 9);
//    }
//  }
//
//  private class Flipper38 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000004000000000L
//          | /* dir = 8 */ (0x4040404040404040L & ((0x0000004000000000L + /* dir = 8 */ (opponentAfterPlayer8 | 0xBFBFBFBFBFBFBFBFL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xBFBFBFBFBFBFBFBFL)))
//          | /* dir = 7 */ (0x0810204080000000L & ((0x0000004000000000L + /* dir = 7 */ (opponentAfterPlayer7 | 0xF7EFDFBF7FFFFFFFL)) ^ /* dir = 7 */ (opponentAfterPlayer7 | 0xF7EFDFBF7FFFFFFFL)))
//          | /* dir = -1 */ getDisksRightOfCells5(opponentNearPlayer1, 0x0000004000000000L, 1)
//          | /* dir = -8 */ getDisksRightOfCells3(opponentNearPlayer8, 0x0000004000000000L, 8)
//          | /* dir = -9 */ getDisksRightOfCells3(opponentNearPlayer9, 0x0000004000000000L, 9);
//    }
//  }
//
//  private class Flipper39 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000008000000000L
//          | /* dir = 8 */ (0x8080808080808080L & ((0x0000008000000000L + /* dir = 8 */ (opponentAfterPlayer8 | 0x7F7F7F7F7F7F7F7FL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0x7F7F7F7F7F7F7F7FL)))
//          | /* dir = 7 */ (0x1020408000000000L & ((0x0000008000000000L + /* dir = 7 */ (opponentAfterPlayer7 | 0xEFDFBF7FFFFFFFFFL)) ^ /* dir = 7 */ (opponentAfterPlayer7 | 0xEFDFBF7FFFFFFFFFL)))
//          | /* dir = -1 */ getDisksRightOfCells6(opponentNearPlayer1, 0x0000008000000000L, 1)
//          | /* dir = -8 */ getDisksRightOfCells3(opponentNearPlayer8, 0x0000008000000000L, 8)
//          | /* dir = -9 */ getDisksRightOfCells3(opponentNearPlayer9, 0x0000008000000000L, 9);
//    }
//  }
//
//  private class Flipper40 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000010000000000L
//          | ((0x0000010000000000L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = 9 */ (0x0402010000000000L & ((0x0000010000000000L + /* dir = 9 */ (opponentAfterPlayer9 | 0xFBFDFEFFFFFFFFFFL)) ^ /* dir = 9 */ (opponentAfterPlayer9 | 0xFBFDFEFFFFFFFFFFL)))
//          | /* dir = 8 */ (0x0101010101010101L & ((0x0000010000000000L + /* dir = 8 */ (opponentAfterPlayer8 | 0xFEFEFEFEFEFEFEFEL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xFEFEFEFEFEFEFEFEL)))
//          | /* dir = -7 */ getDisksRightOfCells4(opponentNearPlayer7, 0x0000010000000000L, 7)
//          | /* dir = -8 */ getDisksRightOfCells4(opponentNearPlayer8, 0x0000010000000000L, 8);
//    }
//  }
//
//  private class Flipper41 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000020000000000L
//          | ((0x0000020000000000L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = 9 */ (0x0804020100000000L & ((0x0000020000000000L + /* dir = 9 */ (opponentAfterPlayer9 | 0xF7FBFDFEFFFFFFFFL)) ^ /* dir = 9 */ (opponentAfterPlayer9 | 0xF7FBFDFEFFFFFFFFL)))
//          | /* dir = 8 */ (0x0202020202020202L & ((0x0000020000000000L + /* dir = 8 */ (opponentAfterPlayer8 | 0xFDFDFDFDFDFDFDFDL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xFDFDFDFDFDFDFDFDL)))
//          | /* dir = -7 */ getDisksRightOfCells4(opponentNearPlayer7, 0x0000020000000000L, 7)
//          | /* dir = -8 */ getDisksRightOfCells4(opponentNearPlayer8, 0x0000020000000000L, 8);
//    }
//  }
//
//  private class Flipper42 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000040000000000L
//          | ((0x0000040000000000L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = 9 */ (0x1008040201000000L & ((0x0000040000000000L + /* dir = 9 */ (opponentAfterPlayer9 | 0xEFF7FBFDFEFFFFFFL)) ^ /* dir = 9 */ (opponentAfterPlayer9 | 0xEFF7FBFDFEFFFFFFL)))
//          | /* dir = 8 */ (0x0404040404040404L & ((0x0000040000000000L + /* dir = 8 */ (opponentAfterPlayer8 | 0xFBFBFBFBFBFBFBFBL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xFBFBFBFBFBFBFBFBL)))
//          | /* dir = 7 */ (0x0102040810204080L & ((0x0000040000000000L + /* dir = 7 */ (opponentAfterPlayer7 | 0xFEFDFBF7EFDFBF7FL)) ^ /* dir = 7 */ (opponentAfterPlayer7 | 0xFEFDFBF7EFDFBF7FL)))
//          | /* dir = -1 */ getDisksRightOfCells1(opponentNearPlayer1, 0x0000040000000000L, 1)
//          | /* dir = -7 */ getDisksRightOfCells4(opponentNearPlayer7, 0x0000040000000000L, 7)
//          | /* dir = -8 */ getDisksRightOfCells4(opponentNearPlayer8, 0x0000040000000000L, 8)
//          | /* dir = -9 */ getDisksRightOfCells1(opponentNearPlayer9, 0x0000040000000000L, 9);
//    }
//  }
//
//  private class Flipper43 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000080000000000L
//          | ((0x0000080000000000L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = 9 */ (0x2010080402010000L & ((0x0000080000000000L + /* dir = 9 */ (opponentAfterPlayer9 | 0xDFEFF7FBFDFEFFFFL)) ^ /* dir = 9 */ (opponentAfterPlayer9 | 0xDFEFF7FBFDFEFFFFL)))
//          | /* dir = 8 */ (0x0808080808080808L & ((0x0000080000000000L + /* dir = 8 */ (opponentAfterPlayer8 | 0xF7F7F7F7F7F7F7F7L)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xF7F7F7F7F7F7F7F7L)))
//          | /* dir = 7 */ (0x0204081020408000L & ((0x0000080000000000L + /* dir = 7 */ (opponentAfterPlayer7 | 0xFDFBF7EFDFBF7FFFL)) ^ /* dir = 7 */ (opponentAfterPlayer7 | 0xFDFBF7EFDFBF7FFFL)))
//          | /* dir = -1 */ getDisksRightOfCells2(opponentNearPlayer1, 0x0000080000000000L, 1)
//          | /* dir = -7 */ getDisksRightOfCells3(opponentNearPlayer7, 0x0000080000000000L, 7)
//          | /* dir = -8 */ getDisksRightOfCells4(opponentNearPlayer8, 0x0000080000000000L, 8)
//          | /* dir = -9 */ getDisksRightOfCells2(opponentNearPlayer9, 0x0000080000000000L, 9);
//    }
//  }
//
//  private class Flipper44 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000100000000000L
//          | ((0x0000100000000000L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = 9 */ (0x4020100804020100L & ((0x0000100000000000L + /* dir = 9 */ (opponentAfterPlayer9 | 0xBFDFEFF7FBFDFEFFL)) ^ /* dir = 9 */ (opponentAfterPlayer9 | 0xBFDFEFF7FBFDFEFFL)))
//          | /* dir = 8 */ (0x1010101010101010L & ((0x0000100000000000L + /* dir = 8 */ (opponentAfterPlayer8 | 0xEFEFEFEFEFEFEFEFL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xEFEFEFEFEFEFEFEFL)))
//          | /* dir = 7 */ (0x0408102040800000L & ((0x0000100000000000L + /* dir = 7 */ (opponentAfterPlayer7 | 0xFBF7EFDFBF7FFFFFL)) ^ /* dir = 7 */ (opponentAfterPlayer7 | 0xFBF7EFDFBF7FFFFFL)))
//          | /* dir = -1 */ getDisksRightOfCells3(opponentNearPlayer1, 0x0000100000000000L, 1)
//          | /* dir = -7 */ getDisksRightOfCells2(opponentNearPlayer7, 0x0000100000000000L, 7)
//          | /* dir = -8 */ getDisksRightOfCells4(opponentNearPlayer8, 0x0000100000000000L, 8)
//          | /* dir = -9 */ getDisksRightOfCells3(opponentNearPlayer9, 0x0000100000000000L, 9);
//    }
//  }
//
//  private class Flipper45 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000200000000000L
//          | ((0x0000200000000000L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = 9 */ (0x8040201008040201L & ((0x0000200000000000L + /* dir = 9 */ (opponentAfterPlayer9 | 0x7FBFDFEFF7FBFDFEL)) ^ /* dir = 9 */ (opponentAfterPlayer9 | 0x7FBFDFEFF7FBFDFEL)))
//          | /* dir = 8 */ (0x2020202020202020L & ((0x0000200000000000L + /* dir = 8 */ (opponentAfterPlayer8 | 0xDFDFDFDFDFDFDFDFL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xDFDFDFDFDFDFDFDFL)))
//          | /* dir = 7 */ (0x0810204080000000L & ((0x0000200000000000L + /* dir = 7 */ (opponentAfterPlayer7 | 0xF7EFDFBF7FFFFFFFL)) ^ /* dir = 7 */ (opponentAfterPlayer7 | 0xF7EFDFBF7FFFFFFFL)))
//          | /* dir = -1 */ getDisksRightOfCells4(opponentNearPlayer1, 0x0000200000000000L, 1)
//          | /* dir = -7 */ getDisksRightOfCells1(opponentNearPlayer7, 0x0000200000000000L, 7)
//          | /* dir = -8 */ getDisksRightOfCells4(opponentNearPlayer8, 0x0000200000000000L, 8)
//          | /* dir = -9 */ getDisksRightOfCells4(opponentNearPlayer9, 0x0000200000000000L, 9);
//    }
//  }
//
//  private class Flipper46 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000400000000000L
//          | /* dir = 8 */ (0x4040404040404040L & ((0x0000400000000000L + /* dir = 8 */ (opponentAfterPlayer8 | 0xBFBFBFBFBFBFBFBFL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0xBFBFBFBFBFBFBFBFL)))
//          | /* dir = 7 */ (0x1020408000000000L & ((0x0000400000000000L + /* dir = 7 */ (opponentAfterPlayer7 | 0xEFDFBF7FFFFFFFFFL)) ^ /* dir = 7 */ (opponentAfterPlayer7 | 0xEFDFBF7FFFFFFFFFL)))
//          | /* dir = -1 */ getDisksRightOfCells5(opponentNearPlayer1, 0x0000400000000000L, 1)
//          | /* dir = -8 */ getDisksRightOfCells4(opponentNearPlayer8, 0x0000400000000000L, 8)
//          | /* dir = -9 */ getDisksRightOfCells4(opponentNearPlayer9, 0x0000400000000000L, 9);
//    }
//  }
//
//  private class Flipper47 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0000800000000000L
//          | /* dir = 8 */ (0x8080808080808080L & ((0x0000800000000000L + /* dir = 8 */ (opponentAfterPlayer8 | 0x7F7F7F7F7F7F7F7FL)) ^ /* dir = 8 */ (opponentAfterPlayer8 | 0x7F7F7F7F7F7F7F7FL)))
//          | /* dir = 7 */ (0x2040800000000000L & ((0x0000800000000000L + /* dir = 7 */ (opponentAfterPlayer7 | 0xDFBF7FFFFFFFFFFFL)) ^ /* dir = 7 */ (opponentAfterPlayer7 | 0xDFBF7FFFFFFFFFFFL)))
//          | /* dir = -1 */ getDisksRightOfCells6(opponentNearPlayer1, 0x0000800000000000L, 1)
//          | /* dir = -8 */ getDisksRightOfCells4(opponentNearPlayer8, 0x0000800000000000L, 8)
//          | /* dir = -9 */ getDisksRightOfCells4(opponentNearPlayer9, 0x0000800000000000L, 9);
//    }
//  }
//
//  private class Flipper48 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0001000000000000L
//          | ((0x0001000000000000L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = -7 */ getDisksRightOfCells5(opponentNearPlayer7, 0x0001000000000000L, 7)
//          | /* dir = -8 */ getDisksRightOfCells5(opponentNearPlayer8, 0x0001000000000000L, 8);
//    }
//  }
//
//  private class Flipper49 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0002000000000000L
//          | ((0x0002000000000000L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = -7 */ getDisksRightOfCells5(opponentNearPlayer7, 0x0002000000000000L, 7)
//          | /* dir = -8 */ getDisksRightOfCells5(opponentNearPlayer8, 0x0002000000000000L, 8);
//    }
//  }
//
//  private class Flipper50 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0004000000000000L
//          | ((0x0004000000000000L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = -1 */ getDisksRightOfCells1(opponentNearPlayer1, 0x0004000000000000L, 1)
//          | /* dir = -7 */ getDisksRightOfCells4(opponentNearPlayer7, 0x0004000000000000L, 7)
//          | /* dir = -8 */ getDisksRightOfCells5(opponentNearPlayer8, 0x0004000000000000L, 8)
//          | /* dir = -9 */ getDisksRightOfCells1(opponentNearPlayer9, 0x0004000000000000L, 9);
//    }
//  }
//
//  private class Flipper51 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0008000000000000L
//          | ((0x0008000000000000L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = -1 */ getDisksRightOfCells2(opponentNearPlayer1, 0x0008000000000000L, 1)
//          | /* dir = -7 */ getDisksRightOfCells3(opponentNearPlayer7, 0x0008000000000000L, 7)
//          | /* dir = -8 */ getDisksRightOfCells5(opponentNearPlayer8, 0x0008000000000000L, 8)
//          | /* dir = -9 */ getDisksRightOfCells2(opponentNearPlayer9, 0x0008000000000000L, 9);
//    }
//  }
//
//  private class Flipper52 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0010000000000000L
//          | ((0x0010000000000000L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = -1 */ getDisksRightOfCells3(opponentNearPlayer1, 0x0010000000000000L, 1)
//          | /* dir = -7 */ getDisksRightOfCells2(opponentNearPlayer7, 0x0010000000000000L, 7)
//          | /* dir = -8 */ getDisksRightOfCells5(opponentNearPlayer8, 0x0010000000000000L, 8)
//          | /* dir = -9 */ getDisksRightOfCells3(opponentNearPlayer9, 0x0010000000000000L, 9);
//    }
//  }
//
//  private class Flipper53 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0020000000000000L
//          | ((0x0020000000000000L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = -1 */ getDisksRightOfCells4(opponentNearPlayer1, 0x0020000000000000L, 1)
//          | /* dir = -7 */ getDisksRightOfCells1(opponentNearPlayer7, 0x0020000000000000L, 7)
//          | /* dir = -8 */ getDisksRightOfCells5(opponentNearPlayer8, 0x0020000000000000L, 8)
//          | /* dir = -9 */ getDisksRightOfCells4(opponentNearPlayer9, 0x0020000000000000L, 9);
//    }
//  }
//
//  private class Flipper54 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0040000000000000L
//          | /* dir = -1 */ getDisksRightOfCells5(opponentNearPlayer1, 0x0040000000000000L, 1)
//          | /* dir = -8 */ getDisksRightOfCells5(opponentNearPlayer8, 0x0040000000000000L, 8)
//          | /* dir = -9 */ getDisksRightOfCells5(opponentNearPlayer9, 0x0040000000000000L, 9);
//    }
//  }
//
//  private class Flipper55 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0080000000000000L
//          | /* dir = -1 */ getDisksRightOfCells6(opponentNearPlayer1, 0x0080000000000000L, 1)
//          | /* dir = -8 */ getDisksRightOfCells5(opponentNearPlayer8, 0x0080000000000000L, 8)
//          | /* dir = -9 */ getDisksRightOfCells5(opponentNearPlayer9, 0x0080000000000000L, 9);
//    }
//  }
//
//  private class Flipper56 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0100000000000000L
//          | ((0x0100000000000000L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = -7 */ getDisksRightOfCells6(opponentNearPlayer7, 0x0100000000000000L, 7)
//          | /* dir = -8 */ getDisksRightOfCells6(opponentNearPlayer8, 0x0100000000000000L, 8);
//    }
//  }
//
//  private class Flipper57 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0200000000000000L
//          | ((0x0200000000000000L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = -7 */ getDisksRightOfCells5(opponentNearPlayer7, 0x0200000000000000L, 7)
//          | /* dir = -8 */ getDisksRightOfCells6(opponentNearPlayer8, 0x0200000000000000L, 8);
//    }
//  }
//
//  private class Flipper58 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0400000000000000L
//          | ((0x0400000000000000L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = -1 */ getDisksRightOfCells1(opponentNearPlayer1, 0x0400000000000000L, 1)
//          | /* dir = -7 */ getDisksRightOfCells4(opponentNearPlayer7, 0x0400000000000000L, 7)
//          | /* dir = -8 */ getDisksRightOfCells6(opponentNearPlayer8, 0x0400000000000000L, 8)
//          | /* dir = -9 */ getDisksRightOfCells1(opponentNearPlayer9, 0x0400000000000000L, 9);
//    }
//  }
//
//  private class Flipper59 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x0800000000000000L
//          | ((0x0800000000000000L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = -1 */ getDisksRightOfCells2(opponentNearPlayer1, 0x0800000000000000L, 1)
//          | /* dir = -7 */ getDisksRightOfCells3(opponentNearPlayer7, 0x0800000000000000L, 7)
//          | /* dir = -8 */ getDisksRightOfCells6(opponentNearPlayer8, 0x0800000000000000L, 8)
//          | /* dir = -9 */ getDisksRightOfCells2(opponentNearPlayer9, 0x0800000000000000L, 9);
//    }
//  }
//
//  private class Flipper60 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x1000000000000000L
//          | ((0x1000000000000000L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = -1 */ getDisksRightOfCells3(opponentNearPlayer1, 0x1000000000000000L, 1)
//          | /* dir = -7 */ getDisksRightOfCells2(opponentNearPlayer7, 0x1000000000000000L, 7)
//          | /* dir = -8 */ getDisksRightOfCells6(opponentNearPlayer8, 0x1000000000000000L, 8)
//          | /* dir = -9 */ getDisksRightOfCells3(opponentNearPlayer9, 0x1000000000000000L, 9);
//    }
//  }
//
//  private class Flipper61 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x2000000000000000L
//          | ((0x2000000000000000L + opponentAfterPlayer1) ^ opponentAfterPlayer1)
//          | /* dir = -1 */ getDisksRightOfCells4(opponentNearPlayer1, 0x2000000000000000L, 1)
//          | /* dir = -7 */ getDisksRightOfCells1(opponentNearPlayer7, 0x2000000000000000L, 7)
//          | /* dir = -8 */ getDisksRightOfCells6(opponentNearPlayer8, 0x2000000000000000L, 8)
//          | /* dir = -9 */ getDisksRightOfCells4(opponentNearPlayer9, 0x2000000000000000L, 9);
//    }
//  }
//
//  private class Flipper62 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x4000000000000000L
//          | /* dir = -1 */ getDisksRightOfCells5(opponentNearPlayer1, 0x4000000000000000L, 1)
//          | /* dir = -8 */ getDisksRightOfCells6(opponentNearPlayer8, 0x4000000000000000L, 8)
//          | /* dir = -9 */ getDisksRightOfCells5(opponentNearPlayer9, 0x4000000000000000L, 9);
//    }
//  }
//
//  private class Flipper63 extends Flipper {
//    @Override
//    public long run() {
//      return 
//          0x8000000000000000L
//          | /* dir = -1 */ getDisksRightOfCells6(opponentNearPlayer1, 0x8000000000000000L, 1)
//          | /* dir = -8 */ getDisksRightOfCells6(opponentNearPlayer8, 0x8000000000000000L, 8)
//          | /* dir = -9 */ getDisksRightOfCells6(opponentNearPlayer9, 0x8000000000000000L, 9);
//    }
//  }

//  private Flipper[] flipper = new Flipper[] {
//    new Flipper0(),
//    new Flipper1(),
//    new Flipper2(),
//    new Flipper3(),
//    new Flipper4(),
//    new Flipper5(),
//    new Flipper6(),
//    new Flipper7(),
//    new Flipper8(),
//    new Flipper9(),
//    new Flipper10(),
//    new Flipper11(),
//    new Flipper12(),
//    new Flipper13(),
//    new Flipper14(),
//    new Flipper15(),
//    new Flipper16(),
//    new Flipper17(),
//    new Flipper18(),
//    new Flipper19(),
//    new Flipper20(),
//    new Flipper21(),
//    new Flipper22(),
//    new Flipper23(),
//    new Flipper24(),
//    new Flipper25(),
//    new Flipper26(),
//    new Flipper27(),
//    new Flipper28(),
//    new Flipper29(),
//    new Flipper30(),
//    new Flipper31(),
//    new Flipper32(),
//    new Flipper33(),
//    new Flipper34(),
//    new Flipper35(),
//    new Flipper36(),
//    new Flipper37(),
//    new Flipper38(),
//    new Flipper39(),
//    new Flipper40(),
//    new Flipper41(),
//    new Flipper42(),
//    new Flipper43(),
//    new Flipper44(),
//    new Flipper45(),
//    new Flipper46(),
//    new Flipper47(),
//    new Flipper48(),
//    new Flipper49(),
//    new Flipper50(),
//    new Flipper51(),
//    new Flipper52(),
//    new Flipper53(),
//    new Flipper54(),
//    new Flipper55(),
//    new Flipper56(),
//    new Flipper57(),
//    new Flipper58(),
//    new Flipper59(),
//    new Flipper60(),
//    new Flipper61(),
//    new Flipper62(),
//    new Flipper63(),
//  };
  
  public long getFlip(int move) {
    int moveShifted = move << 8;
    return
        flipHorizontalLast[moveShifted + hashRow(move, opponentNearPlayer1)] |
        flipVerticalLast[moveShifted + hashColumn(move, opponentNearPlayer8)] |
        flipDiagonalLast[moveShifted + hashDiagonal(move, opponentNearPlayer9)] |
        flipReverseDiagonalLast[moveShifted + hashRevDiagonal(move, opponentNearPlayer7)];
  }


  public static String getFlipCodeInterface() {
    String result = "private static Flipper[] flipper = new Flipper[] {\n";
    for (int i = 0; i < 64; ++i) {
      result += String.format("  new Flipper%d(),\n", i);
    }
    result += "};\n";
    return indent(result, 2) + "\n  public static long getFlip(int move) {\n    return flipper[move].run(player, opponent);\n  }";
  }

  public static String getFlipCode() {
    String result = "switch (move) {\n";
    for (int i = 0; i < 64; ++i) {
      result += String.format("  case %d:\n    return getFlip%d();\n", i, i);
    }
    result += "  default:\n    assert(false);\n}\nreturn 0;";
    return "  public static long getFlip(int move) {\n" + indent(result, 4) + "\n  }";
  }
  
  public static int maxNFlips(int n, int dir) {
    int dirHoriz = (dir + 16) % 8;
    dirHoriz = dirHoriz == 7 ? -1 : dirHoriz;
    for (int i = 0; ; ++i) {
      if (n + i * dir < 0 || n + i * dir > 63 || n % 8 + i * dirHoriz < 0 || n % 8 + i * dirHoriz > 7) {
        return i - 2;
      }
    }
  }
  
  public static String getFlipNCode(int n) {
    long moveBit = 1L << n;

    ArrayList<String> conditions = new ArrayList<>();
    conditions.add(String.format("0x%016XL", moveBit));
    for (int dir : new int[] {1, 9, 8, 7, -1, -7, -8, -9}) {
      int maxNFlips = maxNFlips(n, dir);
      if (maxNFlips <= 0) {
        continue;
      }
      long line = 0;
      switch (Math.abs(dir)) {
        case 1:
          line = BitPattern.getRow(n);
          break;
        case 7:
          line = BitPattern.getDiag7(n);
          break;
        case 8:          
          line = BitPattern.getColumn(n);
          break;
        case 9:
          line = BitPattern.getDiag9(n);
          break;
      }

      if (dir == 1) {
        conditions.add(String.format("((0x%016XL + opponentAfterPlayer1) ^ opponentAfterPlayer1)", moveBit));
      } else if (dir > 0) {
        String mask = String.format("/* dir = %d */ (opponentAfterPlayer%d | 0x%016XL)", dir, dir, ~line);
        conditions.add(String.format("/* dir = %d */ (0x%016XL & ((0x%016XL + %s) ^ %s))", dir, line, moveBit, mask, mask));
      } else {
        conditions.add(String.format("/* dir = %d */ getDisksRightOfCells%d(opponentNearPlayer%d, 0x%016XL, %d)", dir, maxNFlips, -dir, moveBit, -dir));
      }
    }
    
    return "  private class Flipper" + n + " extends Flipper {\n" +
           "    @Override\n" +
           "    public long run() {\n" +
           "      return \n" +
           indent(String.join("\n| ", conditions), 10) + ";\n" +
           "    }\n" +
           "  }\n";
//    return "  private static long getFlip" + n + "(long player, long opponent) {\n" + indent(result, 4) + "\n  }";
  }
  
  public static String getFlipNCodeOld(int n) {
    long firstCaseInEachDir = 0;
    long moveBit = 1L << n;

    ArrayList<String> conditions = new ArrayList<>();
    conditions.add(String.format("0x%016XL", moveBit));
    for (int dir : new int[] {1, 9, 8, 7, -1, -7, -8, -9}) {
      int maxNFlips = maxNFlips(n, dir);
      if (maxNFlips <= 0) {
        continue;
      }
      long line = 0;
      switch (Math.abs(dir)) {
        case 1:
          line = BitPattern.getRow(n);
          break;
        case 7:
          line = BitPattern.getDiag7(n);
          break;
        case 8:          
          line = BitPattern.getColumn(n);
          break;
        case 9:
          line = BitPattern.getDiag9(n);
          break;
      }
      long firstCaseInDir = 1L << (n + dir);
      firstCaseInEachDir |= firstCaseInDir;

      if (dir == 1) {
        conditions.add(String.format("((0x%016XL + opponentAfterPlayer1) ^ opponentAfterPlayer1)", moveBit));
      } else if (dir > 0) {
        String mask = String.format("/* dir = %d */ (opponentAfterPlayer%d | 0x%016XL)", dir, dir, ~line);
        conditions.add(String.format("/* dir = %d */ (0x%016XL & ((0x%016XL + %s) ^ %s))", dir, line, moveBit, mask, mask));
      } else {
        conditions.add(String.format("/* dir = %d */ getDisksRightOfCells%d(opponentNearPlayer%d, 0x%016XL, %d)", dir, maxNFlips, -dir, moveBit, -dir));
      }
    }
    
    return "  private class Flipper" + n + " extends Flipper {\n" +
           "    @Override\n" +
           "    public long run() {\n" +
           "      return \n" +
           indent(String.join("\n| ", conditions), 10) + ";\n" +
           "    }\n" +
           "  }\n";
//    return "  private static long getFlip" + n + "(long player, long opponent) {\n" + indent(result, 4) + "\n  }";
  }
  
  public static String indent(String str, int n) {
    String indent = new String(new char[n]).replace("\0", " ");
    return indent + str.replace("\n", "\n" + indent);
  }
  
  public static void main(String args[]) {
    for (int i = 0; i < 64; ++i) {
      System.out.println(getFlipNCode(i));
    }
    System.out.println(getFlipCodeInterface());
//      System.out.println(getFlipNCode(63));
    
    GetMovesCache getMoves = new GetMovesCache();
    GetFlip getFlip = new GetFlip();
    
    long n = 400000000L;
    ThreadMXBean thread = ManagementFactory.getThreadMXBean();

    long tmp = 0;
    long t = -System.currentTimeMillis();	
    long cpuT = thread.getCurrentThreadCpuTime();
    
    for (long i = 10000; i > 0; --i) {
      Board b = Board.randomBoard(0.5, 0.5);
      long player = b.getPlayer();
      long opponent = b.getOpponent();
      for (long j = n / 10000; j > 0; --j) {
        getMoves.getMoves(player | (i*j), opponent & ~(i*j));
//        tmp = tmp ^ GetMoves.getFlipGeneric((int) (j % 63), player | (i*j), opponent & ~(i*j));// (int) (i % 11));
        tmp = tmp ^ getMoves.getFlip(0);// (int) (i % 11));
//      tmp = tmp ^ getFlip(63, player | (2*i), opponent & ~(2*i));// (int) (i % 11));
//      tmp = tmp ^ getFlip(63, player | (4*i), opponent & ~(4*i));// (int) (i % 11));
//      tmp = tmp ^ getFlip(player | i, opponent & ~i, 1);
//      tmp = tmp ^ getFlip(player | i, opponent & ~i, 2);
//      tmp = tmp ^ getFlip(player | i, opponent & ~i, 8);
//      tmp = tmp ^ getFlip(player | i, opponent & ~i, 9);
      }
    }
    cpuT += thread.getCurrentThreadCpuTime();
    t += System.currentTimeMillis();
    System.out.println(t);
    System.out.println(n * 1000.0 / t);
    System.out.println(cpuT);
    System.out.println(n * 1000000000.0 / cpuT);
    System.out.println(BitPattern.patternToString(tmp));
  }
  
//  public static void main(String args[]) {
//    GetFlip flipper = GetFlip.load();
//    
//    Board b = Board.randomBoard();
//    long player = b.getPlayer();
//    long opponent = b.getOpponent();
//    GetMoves getMoves = new GetMoves();
//    
//    long n = 400000000L;
//    long t = -System.currentTimeMillis();	
//    ThreadMXBean thread = ManagementFactory.getThreadMXBean();
//
//    long cpuT = thread.getCurrentThreadCpuTime();
//    long tmp = 0;
//    
//    for (long i = n; i > 0; --i) {
//      tmp = tmp ^ flipper.getFlip(63, player | i, opponent & ~i);
//    }
//    cpuT += thread.getCurrentThreadCpuTime();
//    t += System.currentTimeMillis();
//    System.out.println(t);
//    System.out.println(n * 1000.0 / t);
//    System.out.println(cpuT);
//    System.out.println(n * 1000000000.0 / cpuT);
//    System.out.println(BitPattern.patternToString(tmp));
//  }
}
