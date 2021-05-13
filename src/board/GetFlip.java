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
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import java.lang.management.ManagementFactory;
import java.lang.management.ThreadMXBean;
import java.util.ArrayList;
import java.util.Arrays;
import main.Main;
//import sun.misc.Unsafe;
/**
 *
 * @author michele
 */
public class GetFlip implements Serializable {
  private final static String SERIALIZE_FILEPATTERN = "coefficients/get_flip.sar";
//  private transient Unsafe unsafe;
  static final long[] flipHorizontalLast = new long[64 * 256];
  static final long[] flipVerticalLast = new long[64 * 256];
  static final long[] flipDiagonalLast = new long[64 * 256];
  static final long[] flipReverseDiagonalLast = new long[64 * 256];
  
  static long neighbors[];
  static long horizVert[];
  
  public static void main(String args[]) throws IOException, ClassNotFoundException {
    GetFlip result = new GetFlip();
//    result.save();
    ObjectInputStream in = Main.fileAccessor.inputFile(SERIALIZE_FILEPATTERN);
//    try (ObjectInputStream in = Main.fileAccessor.inputFile(filepath)) {
    result = (GetFlip) in.readObject();
  }
  
  static {
    initFlips();
    initMoves();
  }
  
  private void save() {
    save(SERIALIZE_FILEPATTERN);
  }
  private void save(String file) {
    try {
       ObjectOutputStream out = Main.fileAccessor.outputFile(file);
       out.writeObject(this);
       out.close();
       System.out.println("Saved GetFlip().");
    } catch (IOException e) {
       e.printStackTrace();
    }
  }

  public static GetFlip load() {
    return load(SERIALIZE_FILEPATTERN);
  }

  /**
   * Loads a GetFlip.
   * @param filepath the file (obtained by calling save() on another multilinear regression).
   * @return The GetFlip loaded, or an empty GetFlip if errors occurred.
   */
  public static GetFlip load(String filepath) {
    GetFlip result;
    try (ObjectInputStream in = Main.fileAccessor.inputFile(filepath)) {
      result = (GetFlip) in.readObject();
    } catch (IOException | ClassNotFoundException | ClassCastException e) {
      System.out.println("Error when loading the GetFlip:\n" + 
                         Arrays.toString(e.getStackTrace()));
      return new GetFlip();
    }
    return result;
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
  
  static int hashDiagonal(int move, long player, long opponent) {
    long bitPattern = BitPattern.getDiag9(move);
    return BitPattern.anyDiagonalToFirstRow(player & bitPattern) + 
            BitPattern.anyDiagonalToFirstRow(opponent & bitPattern) * 256;
  }
  static int hashRevDiagonal(int move, long player, long opponent) {
    long bitPattern = BitPattern.getDiag7(move);
    return BitPattern.anyDiagonalToFirstRow(player & bitPattern) + 
            BitPattern.anyDiagonalToFirstRow(opponent & bitPattern) * 256;
  }
  static int hashRow(int move, long player, long opponent) {
    long bitPattern = BitPattern.getRow(move);
    return BitPattern.anyRowToFirstRow(player & bitPattern, move / 8) + 
            BitPattern.anyRowToFirstRow(opponent & bitPattern, move / 8) * 256;
  }
  static int hashColumn(int move, long player, long opponent) {
    long bitPattern = BitPattern.getColumn(move);
    return BitPattern.anyColumnToFirstRow(player & bitPattern, move % 8) + 
            BitPattern.anyColumnToFirstRow(opponent & bitPattern, move % 8) * 256;
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
  private static int hashGeneric(int move, long player, long opponent, long bitPattern) {
    if (bitPattern == BitPattern.getDiag9(move)) {
      return hashDiagonal(move, player, opponent);
    }
    if (bitPattern == BitPattern.getDiag7(move)) {
      return hashRevDiagonal(move, player, opponent);
    }
    if (bitPattern == BitPattern.getRow(move)) {
      return hashRow(move, player, opponent);
    }
    if (bitPattern == BitPattern.getColumn(move)) {
      return hashColumn(move, player, opponent);
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
  
  private static void initFlips() {
    neighbors = new long[65];
    horizVert = new long[65];
    int dirs[] = new int[] {-9, -8, -7, -1, 1, 7, 8, 9};
    for (int i = 0; i < 64; ++i) {
      int rightShift = i % 8;
      int leftShift = 7 - rightShift;
      int bottomShift = i / 8;
      int topShift = 7 - bottomShift;
      int maxShift[] = new int[] {Math.min(leftShift, topShift),
                                  topShift,
                                  Math.min(rightShift, topShift),
                                  leftShift, rightShift,
                                  Math.min(leftShift, bottomShift),
                                  bottomShift,
                                  Math.min(rightShift, bottomShift)};
      for (int j = 0; j < 8; j++) {
        int dir = dirs[j];
        if (maxShift[j] > 0) {
          if (dir == -8 || dir == -1 || dir == 1 || dir == 8) {
            horizVert[i] |= 1L << (i - 1 * dir);
          }
          neighbors[i] |= 1L << (i - dir);
        }
      }
    }
  }

  public static long getFlipLast(int move, long opponent) {
    if ((neighbors[move] & opponent) == 0) {
      return 0;
    }
    int position = move * 256;
    long flip = flipHorizontalLast[position + hashRow(move, opponent)] |
        flipVerticalLast[position + hashColumn(move, opponent)] |
        flipDiagonalLast[position + hashDiagonal(move, opponent)] |
        flipReverseDiagonalLast[position + hashRevDiagonal(move, opponent)];
    return flip;
  }

  public static long getFlip(int move, long player, long opponent) {
    if ((neighbors[move] & opponent) == 0) {
      return 0;
    }
    long flip = 0;
    long moveBit = 1L << move;
    long empties = ~(player | opponent);
//    if ((moveBit & empties) == 0) {
//      return 0;
//    }
    empties = empties & ~moveBit;
    int positionLast = move * 256;
    long opponentShifted = opponent & neighbors[move];
    long larger = ((-1L) << move);
    long emptiesFlip;
    long smaller = (~larger) | moveBit;
    if (move > 9) {
      opponentShifted = opponentShifted >>> (move - 9);
    } else {
      opponentShifted = opponentShifted << (9 - move);      
    }
    long curFlip;
    if ((opponentShifted & 1280L) != 0) {
      curFlip = flipHorizontalLast[positionLast + hashRow(move, opponent)];
      emptiesFlip = curFlip & empties;
      if (emptiesFlip != 0) {
        if ((emptiesFlip & larger) != 0) {
          curFlip &= smaller;
        }
        if ((emptiesFlip & smaller) != 0) {
          curFlip &= larger;
        }
      }
      flip |= curFlip;
    }
    if ((opponentShifted & 131074L) != 0) {
      curFlip = flipVerticalLast[positionLast + hashColumn(move, opponent)];
      emptiesFlip = curFlip & empties;
      if (emptiesFlip != 0) {
        if ((emptiesFlip & larger) != 0) {
          curFlip &= smaller;
        }
        if ((emptiesFlip & smaller) != 0) {
          curFlip &= larger;
        }
      }
      flip |= curFlip;
    }
    if ((opponentShifted & 262145L) != 0) {
      curFlip = flipDiagonalLast[positionLast + hashDiagonal(move, opponent)];
      emptiesFlip = curFlip & empties;
      if (emptiesFlip != 0) {
        if ((emptiesFlip & larger) != 0) {
          curFlip &= smaller;
        }
        if ((emptiesFlip & smaller) != 0) {
          curFlip &= larger;
        }
      }
      flip |= curFlip;
    }
    if ((opponentShifted & 65540L) != 0) {
      curFlip = flipReverseDiagonalLast[positionLast + hashRevDiagonal(move, opponent)];
      emptiesFlip = curFlip & empties;
      if (emptiesFlip != 0) {
        if ((emptiesFlip & larger) != 0) {
          curFlip &= smaller;
        }
        if ((emptiesFlip & smaller) != 0) {
          curFlip &= larger;
        }
      }
      flip |= curFlip;
    }
    flip = flip & ~player;
    return flip == moveBit ? 0 : flip;
  }
  
  public static long neighborsMove(int move) {
    return neighbors[move];
  }
  
  public final static long neighbors(long bitPattern) {
    return (((bitPattern << 9) | (bitPattern << 1) | (bitPattern >>> 7)) & ~BitPattern.LAST_COLUMN_BIT_PATTERN) |
        (((bitPattern >>> 9) | (bitPattern >>> 1) | (bitPattern << 7)) & ~BitPattern.FIRST_COLUMN_BIT_PATTERN) |
        (bitPattern << 8) | (bitPattern >>> 8);
  }
}
