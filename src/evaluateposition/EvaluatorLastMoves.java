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
import static board.PossibleMovesFinderImproved.possibleMovesBasic;
import evaluatedepthone.MultilinearRegression;
import it.unimi.dsi.fastutil.longs.LongArrayList;
import java.lang.reflect.Field;
import java.util.Arrays;
import java.util.logging.Level;
import java.util.logging.Logger;
import sun.misc.Unsafe;

/**
 *
 * @author michele
 */
public class EvaluatorLastMoves {
//  static final int POSITION_VALUE[] = new int[] {
//      0, 7, 3, 4, 4, 3, 7, 0,
//      7, 8, 6, 5, 5, 6, 8, 7,
//      3, 6, 2, 1, 1, 2, 6, 3,
//      4, 5, 1, 9, 9, 1, 5, 4,
//      4, 5, 1, 9, 9, 1, 5, 4,
//      3, 6, 2, 1, 1, 2, 6, 3,
//      7, 8, 6, 5, 5, 6, 8, 7,
//      0, 7, 3, 4, 4, 3, 7, 0
//  };
//    
//  class Empty implements Comparable<Empty> {
//    Empty prev = null;
//    Empty next = null;
//    Space space = null;
//    int position = -1;
//    
//    @Override
//    public int compareTo(Empty other) {
//      return Integer.compare(POSITION_VALUE[position], POSITION_VALUE[other.position]);
//    }
//  }
//  class EmptyWithEvaluation implements Comparable<EmptyWithEvaluation> {
//    Empty empty;
//    int eval;
//    @Override
//    public int compareTo(EmptyWithEvaluation other) {
//      return Integer.compare(eval, other.eval);
//    }
//    public EmptyWithEvaluation(Empty empty, int eval) {
//      this.empty = empty;
//      this.eval = eval;
//    }
//  }
//
//  class Space implements Comparable<Space> {
//    Empty firstEmpty = new Empty();
//    Empty lastEmpty = new Empty();
//    short nEmpties;
//    int number;
//    
//    public Space(int number) {
//      reset(number);
//    }
//    
//    public final void reset(int number) {
//      firstEmpty.next = lastEmpty;
//      firstEmpty.space = this;
//      lastEmpty.prev = firstEmpty;
//      lastEmpty.space = this;
//      this.number = number;
//      nEmpties = 0;      
//    }
//    
//    private void addSorted(int position) {
//      Empty newEmpty = new Empty();
//      newEmpty.space = this;
//      newEmpty.position = position;
//      Empty e;
//      for (e = firstEmpty.next; e != lastEmpty; e = e.next) {
//        if (newEmpty.compareTo(e) < 0) {
//          break;
//        }
//      }
//      newEmpty.prev = e.prev;
//      newEmpty.next = e;
//      this.reAddElement(newEmpty);
//    }
//    
//    private void addBeforeLast(int position) {
//      Empty newEmpty = new Empty();
//      newEmpty.space = this;
//      newEmpty.position = position;
//      newEmpty.prev = lastEmpty.prev;
//      newEmpty.next = lastEmpty;
//      this.reAddElement(newEmpty);
//    }
//
//    private void reAddElement(Empty empty) {
//      empty.prev.next = empty;
//      empty.next.prev = empty;
//      nEmpties++;      
//    }
//
//    private void removeElement(Empty empty) {
//      empty.prev.next = empty.next;
//      empty.next.prev = empty.prev;
//      nEmpties--;
//    }
//    
//    public int[] toArray() {
//      int result[] = new int[nEmpties];
//      Empty empty = firstEmpty;
//      for (int i = 0; i < nEmpties; i++) {
//        empty = empty.next;
//        result[i] = empty.position;
//      }
//      return result;
//    }
//
//    public int value() {
//      return 
//          (nEmpties == 0 ? 1000000 : 0) +
//          (nEmpties != 1 ? 200000 : 0) +
//          (nEmpties % 2 == 0 ? 100000 : 0) +
////          (number != lastSpacePlayed.topInt() ? 10000 : 0) +
//          nEmpties * 10 + 
//          number;
//    }
//    @Override
//    public int compareTo(Space other) {
//      return Integer.compare(value(), other.value());
//    }
//  }

  public Unsafe unsafe;
//  Space spaces[];
  long[] flipHorizontal = new long[64 * 256 * 256];
  long[] flipVertical = new long[64 * 256 * 256];
  long[] flipDiagonal = new long[64 * 256 * 256];
  long[] flipReverseDiagonal = new long[64 * 256 * 256];
  long[] flipHorizontalLast = new long[64 * 256 * 256];
  long[] flipVerticalLast = new long[64 * 256];
  long[] flipDiagonalLast = new long[64 * 256];
  long[] flipReverseDiagonalLast = new long[64 * 256];
  int nVisited = 0;
//  MultilinearRegression r = new MultilinearRegression();
  int baseOffset;
//  IntArrayList lastSpacePlayed = new IntArrayList(new int[] {-1});
  
  public EvaluatorLastMoves() {
    Field theUnsafe;
    try {
      theUnsafe = Unsafe.class.getDeclaredField("theUnsafe");
      theUnsafe.setAccessible(true);
      unsafe = (Unsafe) theUnsafe.get(null);
      baseOffset = unsafe.arrayBaseOffset(long[].class);
    } catch (Exception ex) {
      Logger.getLogger(EvaluatorLastMoves.class.getName()).log(Level.SEVERE, null, ex);
    }
    initFlips();
    initMoves();
  }
  
  // HANDLING EMPTY CASES
//  protected int[] getEmpties() {
//    int[] result = new int[getNEmpties()];
//    int i = 0;
//    
//    for (Space space : spaces) {
//      for (int empty : space.toArray()) {
//        result[i++] = empty;
//      }
//    }
//    return result;
//  }
//  public int getNEmpties() {
//    int result = 0;
//    for (Space space : spaces) {
//      result += space.nEmpties;
//    }
//    return result;
//  }
//  
//  protected void setEmpties(Board b) {
//    setEmpties(b.getPlayer(), b.getOpponent());
//  }
//
//  protected void setEmpties(long player, long opponent) {
//    long emptySquares = ~(player | opponent);
//    spaces = new Space[] {new Space(0), new Space(1), new Space(2), new Space(3)};
////    spaces = new Space[] {new Space(0)};
//    
//    while (emptySquares != 0) {
//      int empty = Long.numberOfTrailingZeros(emptySquares);
//      emptySquares = emptySquares & ~(1L << empty);
//      spaces[empty % 8 >= 4 ? 1 : 0 + 2 * (empty / 32)].addSorted(empty);
////      spaces[0].addSorted(empty);
//    }
//    Arrays.sort(spaces);
//  }
//  
//  private Space bestSpace() {
//    Space toReturn = null;
//    for (Space s : spaces) {
//      toReturn = (toReturn == null || s.compareTo(toReturn) < 0) ? s : toReturn;
//    }
//    return toReturn;
//  }
//  private Space bestSpaceAfterSpace(Space space) {
//    Space toReturn = null;
//    for (Space s : spaces) {
//      if (s.compareTo(space) <= 0) {
//        continue;
//      }
//      toReturn = toReturn == null || s.compareTo(toReturn) < 0 ? s : toReturn;
//    }
//    return toReturn;
//  }
//  
//  Empty firstEmpty() {
//    return nextEmpty(bestSpace().firstEmpty);
//  }
//
//  Empty nextEmpty(Empty empty) {
//    Empty toReturn = empty.next;
//    Space space = empty.space;
//
//    if (toReturn.position == -1) {
//      space = bestSpaceAfterSpace(space);
//      if (space == null || space.nEmpties == 0) {
//        return null;
//      }
//      toReturn = space.firstEmpty.next;
//    } 
//    toReturn.space.removeElement(toReturn);
////    lastSpacePlayed.push(toReturn.space.number);
//    return toReturn;
//  }
//  
//  void reAddEmpty(Empty empty) {
//    empty.space.reAddElement(empty);
////    lastSpacePlayed.popInt();
//  }
  
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
  

  private long neighbors[];
  private long horizVert[];
//  private long diagFlips[];
//  private long deletersFlips[];
  
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
//        for (int k = 1; k <= maxShift[j]; k++) {
        if (maxShift[j] > 0) {
          if (dir == -8 || dir == -1 || dir == 1 || dir == 8) {
            horizVert[i] |= 1L << (i - 1 * dir);
          }
//          deletersFlips[i] |= 1L << (i - maxShift[j] * dir);
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
    long tmp = getFlipLast(move, opponent);
    if ((tmp & ~(player | opponent | (1L << move))) == 0) {
      return tmp;
    }
//    System.out.println(i + " " + j);
////    System.out.println("NO");
//    if ((unsafe.getLong(neighbors, baseOffset + (move * 8)) & opponent) == 0) {
//      System.out.println(new Board(player, opponent));
//      System.out.println(BitPattern.patternToString(tmp));
//      return 0;
//    }
//    return tmp & (opponent | (1L << move));
    int position = baseOffset + move * 524288;
    return unsafe.getLongVolatile(flipHorizontal, position + hashRow(move, player, opponent) * 8) |
        unsafe.getLongVolatile(flipVertical, position + hashColumn(move, player, opponent) * 8) | 
        unsafe.getLongVolatile(flipDiagonal, position + hashDiagonal(move, player, opponent) * 8) | 
        unsafe.getLongVolatile(flipReverseDiagonal, position + hashRevDiagonal(move, player, opponent) * 8);
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
                                boolean passed) {
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

    while (empties != 0) {
      move = Long.numberOfTrailingZeros(empties);
      empties = empties & (~(1L << move));
      long flip = getFlip(move, player, opponent);
      if (flip == 0) {
        continue;
      }
      best = Math.max(best, 
        -evaluateSuperFast(opponent & ~flip, player | flip, -beta, 
                           -Math.max(alpha, best), false));
      pass = false;
      if (best >= beta) {
        break;
      }
    }
    if (pass) {
      if (passed) {
        return BitPattern.getEvaluationGameOver(player, opponent);
      }
      return -evaluateSuperFast(opponent, player, -beta, -alpha, true);
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
  
  private final static long[] firstLastInEdges(long bitPattern) {
    long firstBit = 0;
    long lastBit = 0;
//    System.out.println(BitPattern.patternToString(bitPattern));
    for (long edge : edges) {
      firstBit |= Long.highestOneBit(edge & bitPattern);
      lastBit |= Long.lowestOneBit(edge & bitPattern);
    }
    return new long[] {firstBit & ~CORNERS, lastBit & ~CORNERS};
  }


  private int evaluateRecursive(long player, long opponent, int alpha, int beta,
                                long lastMove, boolean passed) {
    long empties = ~(player | opponent);
    int nEmpties = Long.bitCount(empties);
    if (nEmpties <= 4) {
      return evaluateSuperFast(player, opponent, alpha, beta, passed);
    }
    if (!passed) {
      nVisited++;
    }
    if (nEmpties == 1) {
      return evalOneEmpty(player, opponent, passed);
    }
    
    boolean pass = true;
    int best = Integer.MIN_VALUE;
    int move;
    int lastMovePosition = Long.numberOfTrailingZeros(lastMove);
    lastMovePosition = lastMovePosition == 64 ? 32 : lastMovePosition;

    long[] firstInEdges = nEmpties > 5 ? firstLastInEdges(empties) : new long[] {};
    long[] masks = new long[] {
        ~neighborCases(empties) & neighborCases(player),
        nEmpties > 5 ? firstInEdges[0] & firstInEdges[1] & neighborCases(player) : 0,
        neighbors[lastMovePosition] & CORNERS,
        ((lastMove & XSQUARES) == 0) ? horizVert[lastMovePosition] : 0,
        CORNERS,
        nEmpties > 5 ? firstInEdges[0] | firstInEdges[1] : 0,
        ((lastMove & XSQUARES) == 0) ? neighbors[lastMovePosition] : 0,
        CENTRAL1,
        CENTRAL,
        CENTRAL2,
        ~0L
    };
    for (long mask : masks) {
//      System.out.println(BitPattern.patternToString(mask));
      while ((empties & mask) != 0) {
        move = Long.numberOfTrailingZeros(empties & mask);
        lastMove = 1L << move;
        empties = empties & (~lastMove);
        long flip = getFlip(move, player, opponent);
        if (flip == 0) {
          continue;
        }
        best = Math.max(best, 
          -evaluateRecursive(opponent & ~flip, player | flip, -beta, 
                             -Math.max(alpha, best), lastMove, false));
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
      return -evaluateRecursive(opponent, player, -beta, -alpha, lastMove, true);
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
    return evaluateRecursive(player, opponent, alpha, beta, lastMove, false);
  }
  
  public void resetNVisited() {
    nVisited = 0;
  }
  
  
  public int getNVisited() {
    return nVisited;
  }
  
//  public static void main(String args[]) {
//    Board b = new Board("--XXXXX--OOOXX-O-OOOXXOX-OXOXOXXOXXXOXXX--XOXOXX-XXXOOO--OOOOO--", true);
//    EvaluatorLastMoves eval = new EvaluatorLastMoves();
//    long t = System.currentTimeMillis();
//    eval.evaluatePosition(b, -6400, 6400);
//    System.out.println(eval.getNVisited() + " " + (System.currentTimeMillis() - t));
//  }
}
