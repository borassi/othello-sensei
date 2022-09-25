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
import helpers.FileAccessor;
import helpers.Utils;
import main.Main;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;
//import java.lang.management.ManagementFactory;
//import java.lang.management.ThreadMXBean;
import java.util.ArrayList;
import java.util.Scanner;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;
import java.util.function.Function;
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
  
  private static final long MASK = BitPattern.parsePattern(
      "-XXXXXX-" +
      "-XXXXXX-" +
      "-XXXXXX-" +
      "-XXXXXX-" +
      "-XXXXXX-" +
      "-XXXXXX-" +
      "-XXXXXX-" +
      "-XXXXXX-");
  
  static {
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
  }
  
  
  private static void saveFlips() throws IOException {
    ObjectOutputStream out = FileAccessor.objectOutputStream(GET_MOVES_FILEPATTERN);
    out.writeObject(flipHorizontalLast);
    out.writeObject(flipVerticalLast);
    out.writeObject(flipDiagonalLast);
    out.writeObject(flipReverseDiagonalLast);
    out.flush();
    out.close();
  }

  private static void loadFlips() throws IOException, ClassNotFoundException {
    ObjectInputStream in = FileAccessor.objectInputStream(GET_MOVES_FILEPATTERN);
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
  
  private static void initMoves() {
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

  public static Board moveIfPossible(Board b, int move) {
    GetMovesCache mover = new GetMovesCache();
    long movesBit = mover.getMoves(b.getPlayer(), b.getOpponent());
    if (((1L << move) & movesBit) == 0) {
      throw new IllegalArgumentException("Invalid move " + move);
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

  public static Board[] getAllDescendants(Board b) {
    return getAllDescendants(b.getPlayer(), b.getOpponent());
  }
  public static Board[] getAllDescendants(long player, long opponent) {
    long[] moves = getAllMoves(player, opponent);
    Board[] result = new Board[moves.length];
    for (int i = 0; i < result.length; ++i) {
      result[i] = new Board(player, opponent).move(moves[i]);
    }
    return result;
  }
  
  public long getFlip(int move) {
    int moveShifted = move << 8;
    return
        flipHorizontalLast[moveShifted + hashRow(move, opponentNearPlayer1)] |
        flipVerticalLast[moveShifted + hashColumn(move, opponentNearPlayer8)] |
        flipDiagonalLast[moveShifted + hashDiagonal(move, opponentNearPlayer9)] |
        flipReverseDiagonalLast[moveShifted + hashRevDiagonal(move, opponentNearPlayer7)];
  }

  public static String indent(String str, int n) {
    String indent = new String(new char[n]).replace("\0", " ");
    return indent + str.replace("\n", "\n" + indent);
  }
}
