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

package evaluateposition;

import bitpattern.BitPattern;
import board.Board;
import evaluatedepthone.DepthOneEvaluator;
import evaluatedepthone.MultilinearRegressionImproved;
import evaluatedepthone.PatternEvaluatorImproved;
import it.unimi.dsi.fastutil.longs.LongArrayList;
import it.unimi.dsi.fastutil.objects.ObjectArrays;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import java.lang.reflect.Field;
import java.util.Arrays;
import java.util.logging.Level;
import java.util.logging.Logger;
import sun.misc.Unsafe;

/**
 *
 * @author michele
 */
public class EvaluatorLastMoves implements Serializable {
  /**
   * Needed to implement Serializable.
   */
  private static final long serialVersionUID = 1L;
  public static final String EVALUATOR_LAST_MOVES_FILEPATTERN = 
          "coefficients/evaluator_last_moves.sar";

  private static final int EMPTIES_FOR_SUPER_FAST = 5;
  private static final int EMPTIES_FOR_FAST = 10;

  public transient Unsafe unsafe;
  long[] flipHorizontal = new long[64 * 256 * 256];
  long[] flipVertical = new long[64 * 256 * 256];
  long[] flipDiagonal = new long[64 * 256 * 256];
  long[] flipReverseDiagonal = new long[64 * 256 * 256];
  long[] flipHorizontalLast = new long[64 * 256 * 256];
  long[] flipVerticalLast = new long[64 * 256];
  long[] flipDiagonalLast = new long[64 * 256];
  long[] flipReverseDiagonalLast = new long[64 * 256];
  int nVisited = 0;
  DepthOneEvaluator depthOneEval;
  int baseOffset;
  Move[][] moves = new Move[128][64];
  
  long tmp[] = new long[0];
  long masksTmp[];
  long firstLastInEdges[];
  
  private long neighbors[];
  private long horizVert[];
  
  public static class Move implements Comparable<Move>, Serializable {
    int move;
    long flip;
    int eval;
    
    @Override
    public int compareTo(Move other) {
      return Integer.compare(eval, other.eval);
    }
  }
  
  public void save() {
    save(EVALUATOR_LAST_MOVES_FILEPATTERN);
  }
  public void save(String file) {
    try {
       FileOutputStream fileOut = new FileOutputStream(file);
       ObjectOutputStream out = new ObjectOutputStream(fileOut);
       out.writeObject(this);
       out.close();
       fileOut.close();
       System.out.println("Saved pattern evaluator.");
    } catch (IOException e) {
       e.printStackTrace();
    }
  }

  public static EvaluatorLastMoves load() {
    try {
      return load(EVALUATOR_LAST_MOVES_FILEPATTERN);
    } catch(Exception e) {
      return new EvaluatorLastMoves();
    }
  }

  /**
   * Loads a PatternEvaluatorImproved.
   * @param filepath the file (obtained by calling save() on another multilinear regression).
   * @return The PatternEvaluatorImproved loaded, or an empty MultilinearRegression if errors occurred.
   */
  public static EvaluatorLastMoves load(String filepath) {
    EvaluatorLastMoves result;
    try (ObjectInputStream in = new ObjectInputStream(new FileInputStream(filepath))) {
      result = (EvaluatorLastMoves) in.readObject();
    } catch (IOException | ClassNotFoundException | ClassCastException e) {
      System.out.println("Error when loading the PatternEvaluator:\n" + 
                         Arrays.toString(e.getStackTrace()));
      return new EvaluatorLastMoves();
    }
    result.setupUnsafe();
    return result;
  }

  public EvaluatorLastMoves() {
    this(PatternEvaluatorImproved.load());
  }

  public EvaluatorLastMoves(DepthOneEvaluator depthOneEval) {
    this.depthOneEval = depthOneEval;
    setupUnsafe();
    initFlips();
    initMoves();
    for (int i = 0; i < moves.length; ++i) {
      for (int j = 0; j < moves[i].length; ++j) {
        moves[i][j] = new Move();
      }
    }
    save();
  }
  
  public void setupUnsafe() {
    Field theUnsafe;
    try {
      theUnsafe = Unsafe.class.getDeclaredField("theUnsafe");
      theUnsafe.setAccessible(true);
      unsafe = (Unsafe) theUnsafe.get(null);
      baseOffset = unsafe.arrayBaseOffset(long[].class);
    } catch (Exception ex) {
      Logger.getLogger(EvaluatorLastMoves.class.getName()).log(Level.SEVERE, null, ex);
    }
    this.masksTmp = new long[] {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    masksTmp[3] = CORNERS;
    masksTmp[6] = CENTRAL;
    masksTmp[7] = CENTRAL1;
    masksTmp[8] = CENTRAL2;
    masksTmp[9] = ~0L;
    this.firstLastInEdges = new long[2];
  }
  
  // COMPUTING FLIPS.
  
  private static int hashDiagonal(int move, long player) {
    long bitPattern = BitPattern.getNWSEDiag(move);
    return BitPattern.anyDiagonalToFirstRow(player & bitPattern);
  }
  private static int hashRevDiagonal(int move, long player) {
    long bitPattern = BitPattern.getNESWDiag(move);
    return BitPattern.anyDiagonalToFirstRow(player & bitPattern);
  }
  private static int hashRow(int move, long player) {
    long bitPattern = BitPattern.getRow(move);
    return BitPattern.anyRowToFirstRow(player & bitPattern, move / 8);
  }
  private static int hashColumn(int move, long player) {
    long bitPattern = BitPattern.getColumn(move);
    return BitPattern.anyColumnToFirstRow(player & bitPattern, move % 8);
  }
  
  private static int hashDiagonal(int move, long player, long opponent) {
    long bitPattern = BitPattern.getNWSEDiag(move);
    return BitPattern.anyDiagonalToFirstRow(player & bitPattern) + 
            BitPattern.anyDiagonalToFirstRow(opponent & bitPattern) * 256;
  }
  private static int hashRevDiagonal(int move, long player, long opponent) {
    long bitPattern = BitPattern.getNESWDiag(move);
    return BitPattern.anyDiagonalToFirstRow(player & bitPattern) + 
            BitPattern.anyDiagonalToFirstRow(opponent & bitPattern) * 256;
  }
  private static int hashRow(int move, long player, long opponent) {
    long bitPattern = BitPattern.getRow(move);
    return BitPattern.anyRowToFirstRow(player & bitPattern, move / 8) + 
            BitPattern.anyRowToFirstRow(opponent & bitPattern, move / 8) * 256;
  }
  private static int hashColumn(int move, long player, long opponent) {
    long bitPattern = BitPattern.getColumn(move);
    return BitPattern.anyColumnToFirstRow(player & bitPattern, move % 8) + 
            BitPattern.anyColumnToFirstRow(opponent & bitPattern, move % 8) * 256;
  }
  private static int hashGeneric(int move, long player, long bitPattern) {
    if (bitPattern == BitPattern.getNWSEDiag(move)) {
      return hashDiagonal(move, player);
    }
    if (bitPattern == BitPattern.getNESWDiag(move)) {
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
    if (bitPattern == BitPattern.getNWSEDiag(move)) {
      return hashDiagonal(move, player, opponent);
    }
    if (bitPattern == BitPattern.getNESWDiag(move)) {
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
  public static final LongArrayList possibleMovesBasic(Board b) {
    LongArrayList moves = new LongArrayList();
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
        BitPattern.getColumn(position), BitPattern.getNESWDiag(position),
        BitPattern.getNWSEDiag(position)};
      long[][] flips = {flipHorizontal, flipVertical, flipReverseDiagonal, flipDiagonal};
      long[][] flipsLast = {flipHorizontalLast, flipVerticalLast, flipReverseDiagonalLast, flipDiagonalLast};

      for (int i = 0; i < bitPatterns.length; i++) {
        long bitPattern = bitPatterns[i];
        long[] flip = flips[i];
        long[] flipLast = flipsLast[i];
        for (Board board : Board.existingBoardsInBitPattern(bitPattern)) {
          LongArrayList possibleMoves = possibleMovesBasic(board);
          int hash = hashGeneric(
            position, board.getPlayer(), board.getOpponent(), bitPattern);

          for (int j = 0; j < possibleMoves.size(); j++) {
            long curFlip = possibleMoves.getLong(j);          
            int thisMovePosition = Long.numberOfTrailingZeros(
              curFlip & (~board.getPlayer()) & (~board.getOpponent()));
            if (thisMovePosition == position) {
              flip[position * 65536 + hash] = curFlip;
              if ((curFlip | board.getPlayer() | board.getOpponent()) == bitPattern) {
                int newHash = hashGeneric(
                  position, board.getOpponent(), bitPattern);
                assert(flipLast[position * 256 + newHash] == 0);
                flipLast[position * 256 + newHash] = curFlip;
              }
            }
          }
        }
      }
    }
  }
  
  private void initFlips() {
    neighbors = new long[64];
    horizVert = new long[64];
//    diagFlips = new long[64];
//    deletersFlips = new long[64];
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

  protected long getFlipLast(int move, long opponent) {
    int position = baseOffset + (move * 2048);
    return unsafe.getLongVolatile(flipHorizontalLast, position + hashRow(move, opponent) * 8) |
        unsafe.getLongVolatile(flipVerticalLast, position + hashColumn(move, opponent) * 8) | 
        unsafe.getLongVolatile(flipDiagonalLast, position + hashDiagonal(move, opponent) * 8) | 
        unsafe.getLongVolatile(flipReverseDiagonalLast, position + hashRevDiagonal(move, opponent) * 8);
  }
  
  int i = 0, j = 0;

  protected long getFlip(int move, long player, long opponent) {
    long flip = 0;
    long moveBit = 1L << move;
    long empties = ~(player | opponent | moveBit);
    int position = baseOffset + move * 524288;
    int positionLast = baseOffset + move * 2048;
    long opponentShifted = opponent & this.neighbors[move];
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
      curFlip = unsafe.getLongVolatile(flipHorizontalLast, positionLast + hashRow(move, opponent) * 8);
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
      curFlip = unsafe.getLongVolatile(flipVerticalLast, positionLast + hashColumn(move, opponent) * 8);
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
      curFlip = unsafe.getLongVolatile(flipDiagonalLast, positionLast + hashDiagonal(move, opponent) * 8);
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
      curFlip = unsafe.getLongVolatile(flipReverseDiagonalLast, positionLast + hashRevDiagonal(move, opponent) * 8);
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
    return flip == moveBit ? 0 : flip;
    
//    long tmp = getFlipLast(move, opponent);
//    if ((tmp & ~(player | opponent | (1L << move))) == 0) {
//      return tmp;
//    }
//    return unsafe.getLongVolatile(flipHorizontal, position + hashRow(move, player, opponent) * 8) |
//        unsafe.getLongVolatile(flipVertical, position + hashColumn(move, player, opponent) * 8) | 
//        unsafe.getLongVolatile(flipDiagonal, position + hashDiagonal(move, player, opponent) * 8) | 
//        unsafe.getLongVolatile(flipReverseDiagonal, position + hashRevDiagonal(move, player, opponent) * 8);
  }
  
  // EVALUATE.
  private int evalOneEmpty(long player, long opponent, boolean passed) {
    int position = Long.numberOfTrailingZeros(~(player | opponent));
    long flip = getFlipLast(position, opponent);
    if (flip != 0) {
      return BitPattern.getEvaluationBoardFull(player | flip);
    }
    if (passed) {
      return BitPattern.getEvaluationGameOver(player, opponent);
    }
    return -evalOneEmpty(opponent, player, true);
  }
  
  private int evaluateSuperFast(long player, long opponent, int alpha, int beta,
                                boolean passed, long lastFlip) {
    if (!passed) {
      nVisited++;
    }
    long empties = ~(player | opponent);
    
    if (Long.bitCount(empties) == 1) {
      return evalOneEmpty(player, opponent, passed);
    }
    boolean pass = true;
    int best = Integer.MIN_VALUE;
    int move;
//    long neigh = neighborCases(lastFlip) & empties;
//    while (neigh != 0) {
//      move = Long.numberOfTrailingZeros(neigh & empties);      
//      neigh = neigh & (~(1L << move));     
//      empties = empties & (~(1L << move));
////      if ((this.neighbors[move] & opponent) == 0) {
////        continue;
////      }
//      long flip = getFlip(move, player, opponent);
//      if (flip == 0) {
//        continue;
//      }
//      best = Math.max(best, 
//        -evaluateSuperFast(opponent & ~flip, player | flip, -beta, 
//                           -Math.max(alpha, best), false, flip));
//      pass = false;
//      if (best >= beta) {
//        return best;
//      }
//    }
    while (empties != 0) {
      move = Long.numberOfTrailingZeros(empties);      
      empties = empties & (~(1L << move));
      if ((this.neighbors[move] & opponent) == 0) {
        continue;
      }
      long flip = getFlip(move, player, opponent);
      if (flip == 0) {
        continue;
      }
      best = Math.max(best, 
        -evaluateSuperFast(opponent & ~flip, player | flip, -beta, 
                           -Math.max(alpha, best), false, flip));
      pass = false;
      if (best >= beta) {
        return best;
      }
    }
    
    if (pass) {
      if (passed) {
        return BitPattern.getEvaluationGameOver(player, opponent);
      }
      return -evaluateSuperFast(opponent, player, -beta, -alpha, true, 0);
    }
    return best;
  }
  
  private final static long XSQUARES = BitPattern.parsePattern(
      "--------" +
      "-X----X-" +
      "--------" +
      "--------" +
      "--------" +
      "--------" +
      "-X----X-" +
      "--------");
  private final static long CORNERS = BitPattern.parsePattern(
      "X------X" +
      "--------" +
      "--------" +
      "--------" +
      "--------" +
      "--------" +
      "--------" +
      "X------X");
  
  private final static long HORIZVERT = BitPattern.parsePattern(
      "--------" +
      "--------" +
      "--------" +
      "--------" +
      "--------" +
      "------X-" +
      "-----X-X" +
      "------X-");
  
  private final static long CENTRAL1 = BitPattern.parsePattern(
      "--------" +
      "--XXXX--" +
      "-XXXXXX-" +
      "-XXXXXX-" +
      "-XXXXXX-" +
      "-XXXXXX-" +
      "--XXXX--" +
      "--------");
  
  private final static long CENTRAL = BitPattern.parsePattern(
      "--XXXX--" +
      "--XXXX--" +
      "XXXXXXXX" +
      "XXXXXXXX" +
      "XXXXXXXX" +
      "XXXXXXXX" +
      "--XXXX--" +
      "--XXXX--");
  
  private final static long CENTRAL2 = BitPattern.parsePattern(
      "-XXXXXX-" +
      "X-XXXX-X" +
      "XXXXXXXX" +
      "XXXXXXXX" +
      "XXXXXXXX" +
      "XXXXXXXX" +
      "X-XXXX-X" +
      "-XXXXXX-");
  
  private final static long neighborCases(long bitPattern) {
    return (((bitPattern << 9) | (bitPattern << 1) | (bitPattern >> 7)) & ~BitPattern.LAST_COLUMN_BIT_PATTERN) |
        (((bitPattern >> 9) | (bitPattern >> 1) | (bitPattern << 7)) & ~BitPattern.FIRST_COLUMN_BIT_PATTERN) |
        (bitPattern << 8) | (bitPattern >> 8);
  }
  
  final static long edges[] = new long[] {BitPattern.FIRST_COLUMN_BIT_PATTERN,
                                          BitPattern.LAST_COLUMN_BIT_PATTERN,
                                          BitPattern.FIRST_ROW_BIT_PATTERN,
                                          BitPattern.LAST_ROW_BIT_PATTERN};
  
  private final long[] firstLastInEdges(long bitPattern) {
    long firstBit = 0;
    long lastBit = 0;
    for (long edge : edges) {
      firstBit |= Long.highestOneBit(edge & bitPattern);
      lastBit |= Long.lowestOneBit(edge & bitPattern);
    }
    this.firstLastInEdges[0] = firstBit & ~CORNERS;
    this.firstLastInEdges[1] = lastBit & ~CORNERS;
    return this.firstLastInEdges;
  }
  
  int getMovesAdvanced(long player, long opponent, long lastMove, Move[] result) {
    int nMoves = getMoves(player, opponent, lastMove, result);
    if (result[0].move < 0) {
      this.depthOneEval.invert();
      getMoves(opponent, player, lastMove, result);
      this.depthOneEval.invert();
      if (result[0].move >= 0) {
        result[0].move = 0;
        result[0].flip = 0;
        result[1].move = -1;
        return 1;
      } else {
        result[0].move = -1;
        return 0;
      }
    }
    return nMoves;
  }

  int getMoves(long player, long opponent, long lastMove, Move[] result) {
    int move;
    long moveBit;
    long empties = ~(player | opponent);
    int nMoves = 0;

    while (empties != 0) {
      move = Long.numberOfTrailingZeros(empties);
      moveBit = 1L << move;
      empties = empties & (~moveBit);
      long flip = getFlip(move, player, opponent) & ~player;
      if (flip == 0) {
        continue;
      }
      Move curMove = result[nMoves];
      curMove.move = move;
      curMove.flip = flip;
      this.depthOneEval.update(move, flip);
      curMove.eval = this.depthOneEval.eval();
      this.depthOneEval.undoUpdate(move, flip);
      ++nMoves;
    }
    result[nMoves].move = -1;
    ObjectArrays.quickSort(result, 0, nMoves);
    return nMoves;
  }

  private int evaluateRecursive(long player, long opponent, int alpha, int beta,
                                long lastMove, int depth) {
    long empties = ~(player | opponent);
    int nEmpties = Long.bitCount(empties);
    if (nEmpties <= EMPTIES_FOR_FAST) {
      return evaluateFast(player, opponent, alpha, beta, false, lastMove, depth);
    }
    nVisited++;

    int best = Integer.MIN_VALUE;
    long flip;

    Move curMoves[] = moves[depth];
    this.depthOneEval.invert();
    this.getMovesAdvanced(player, opponent, lastMove, curMoves);
    for (Move move : curMoves) {
      if (move.move < 0) {
        break;
      }
      flip = move.flip;
      if (flip != 0) {
        this.depthOneEval.update(move.move, flip);
      }
      best = Math.max(best, 
        -evaluateRecursive(opponent & ~flip, player | flip, -beta, 
                           -Math.max(alpha, best), flip, depth + 1));
      if (flip != 0) {
        this.depthOneEval.undoUpdate(move.move, flip);
      }
      if (best >= beta) {
        break;
      }
    }
    this.depthOneEval.invert();
    if (best == Integer.MIN_VALUE) {
      return BitPattern.getEvaluationGameOver(player, opponent);
    }
    return best;
  }

  private int evaluateFast(long player, long opponent, int alpha, int beta,
                               boolean passed, long lastMove, int depth) {
    long empties = ~(player | opponent);
    int nEmpties = Long.bitCount(empties);
    if (nEmpties <= EMPTIES_FOR_SUPER_FAST) {
      return evaluateSuperFast(player, opponent, alpha, beta, false, lastMove);
    }
    nVisited++;
    if (nEmpties == 1) {
      return evalOneEmpty(player, opponent, false);
    }

    int best = Integer.MIN_VALUE;
    long flip;
//    System.out.println(masksTmp);
//    masksTmp = new long[] {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    long[] firstInEdges = nEmpties > 5 ? firstLastInEdges(empties) : tmp;
    masksTmp[0] = ~neighborCases(empties) & neighborCases(player);
    masksTmp[1] = nEmpties > 5 ? firstInEdges[0] & firstInEdges[1] & neighborCases(player) : 0;
    masksTmp[2] = neighborCases(lastMove) & CORNERS;
    masksTmp[4] = nEmpties > 5 ? firstInEdges[0] | firstInEdges[1] : 0;
    masksTmp[5] = ((lastMove & XSQUARES) == 0) ? neighborCases(lastMove) : 0;
    int move;
    long moveBit;
    boolean pass = true;
    
    for (long mask : masksTmp) {
      while ((empties & mask) != 0) {
        move = Long.numberOfTrailingZeros(empties & mask);
        moveBit = 1L << move;
        empties = empties & (~moveBit);
        flip = getFlip(move, player, opponent) & ~player;
        if (flip == 0) {
          continue;
        }
        best = Math.max(best, 
          -evaluateFast(opponent & ~flip, player | flip, -beta, 
                             -Math.max(alpha, best), false, flip, depth + 1));
        pass = false;
        if (best >= beta) {
          break;
        }
      }
      if (best >= beta) {
        break;
      }
    }
    if (pass) {
      if (passed) {
        return BitPattern.getEvaluationGameOver(player, opponent);
      }
      return -evaluateFast(opponent, player, -beta, -alpha, true, 0, depth);
    }
    return best;
  }
  
  public int evaluatePosition(Board b, int alpha, int beta, long lastMove) {
    long player = b.getPlayer();
    long opponent = b.getOpponent();
    if (~(player & opponent) == 0) {
      return BitPattern.getEvaluationBoardFull(b);
    }
    nVisited--;
    
    this.depthOneEval.setup(player, opponent);
    return evaluateRecursive(player, opponent, alpha, beta, lastMove, 0);
  }
  
  public void resetNVisited() {
    nVisited = 0;
  }
  
  
  public int getNVisited() {
    return nVisited;
  }
  
  public static void main(String args[]) {
    System.out.println(BitPattern.patternToString(-1L));
    System.out.println(BitPattern.patternToString(-1L << (-9)));
//    Board b = new Board("--XXXXX--OOOXX-O-OOOXXOX-OXOXOXXOXXXOXXX--XOXOXX-XXXOOO--OOOOO--", true);
//    EvaluatorLastMoves eval = new EvaluatorLastMoves();
//    long t = System.currentTimeMillis();
//    eval.evaluatePosition(b, -6400, 6400);
//    System.out.println(eval.getNVisited() + " " + (System.currentTimeMillis() - t));
  }
}
