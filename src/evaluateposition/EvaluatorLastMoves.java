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
package evaluateposition;

import bitpattern.BitPattern;
import board.Board;
import static board.PossibleMovesFinderImproved.possibleMovesBasic;
import evaluatedepthone.MultilinearRegression;
import it.unimi.dsi.fastutil.ints.IntArrayList;
import it.unimi.dsi.fastutil.longs.LongArrayList;
import java.util.Arrays;

/**
 *
 * @author michele
 */
public class EvaluatorLastMoves {
  static final int POSITION_VALUE[] = new int[] {
      0, 7, 3, 4, 4, 3, 7, 0,
      7, 8, 6, 5, 5, 6, 8, 7,
      3, 6, 2, 1, 1, 2, 6, 3,
      4, 5, 1, 9, 9, 1, 5, 4,
      4, 5, 1, 9, 9, 1, 5, 4,
      3, 6, 2, 1, 1, 2, 6, 3,
      7, 8, 6, 5, 5, 6, 8, 7,
      0, 7, 3, 4, 4, 3, 7, 0
  };
    
  class Empty implements Comparable<Empty> {
    Empty prev = null;
    Empty next = null;
    Space space = null;
    int position = -1;
    
    @Override
    public int compareTo(Empty other) {
      return Integer.compare(POSITION_VALUE[position], POSITION_VALUE[other.position]);
    }
  }

  class Space implements Comparable<Space> {
    Empty firstEmpty = new Empty();
    Empty lastEmpty = new Empty();
    short nEmpties;
    int number;
    
    public Space(int number) {
      reset(number);
    }
    
    public final void reset(int number) {
      firstEmpty.next = lastEmpty;
      firstEmpty.space = this;
      lastEmpty.prev = firstEmpty;
      lastEmpty.space = this;
      this.number = number;
      nEmpties = 0;      
    }
    
    private void addSorted(int position) {
      Empty newEmpty = new Empty();
      newEmpty.space = this;
      newEmpty.position = position;
      Empty e;
      for (e = firstEmpty.next; e != lastEmpty; e = e.next) {
        if (newEmpty.compareTo(e) < 0) {
          break;
        }
      }
      newEmpty.prev = e.prev;
      newEmpty.next = e;
      this.reAddElement(newEmpty);
    }
    
    private void addBeforeLast(int position) {
      Empty newEmpty = new Empty();
      newEmpty.space = this;
      newEmpty.position = position;
      newEmpty.prev = lastEmpty.prev;
      newEmpty.next = lastEmpty;
      this.reAddElement(newEmpty);
    }

    private void reAddElement(Empty empty) {
      empty.prev.next = empty;
      empty.next.prev = empty;
      nEmpties++;      
    }

    private void removeElement(Empty empty) {
      empty.prev.next = empty.next;
      empty.next.prev = empty.prev;
      nEmpties--;
    }
    
    public int[] toArray() {
      int result[] = new int[nEmpties];
      Empty empty = firstEmpty;
      for (int i = 0; i < nEmpties; i++) {
        empty = empty.next;
        result[i] = empty.position;
      }
      return result;
    }

    public int value() {
      return (nEmpties == 0 ? 3000 : 0) +
          (nEmpties % 2 == 0 ? 2000 : 0) +
          (number != lastSpacePlayed.topInt() ? 1000 : 0) +
          nEmpties * 10 + 
          number;
    }
    @Override
    public int compareTo(Space other) {
      return Integer.compare(value(), other.value());
    }
  }

  Space spaces[];
  long[][] flipHorizontal = new long[64][256 * 256];
  long[][] flipVertical = new long[64][256 * 256];
  long[][] flipDiagonal = new long[64][256 * 256];
  long[][] flipReverseDiagonal = new long[64][256 * 256];
  int nVisited = 0;
  MultilinearRegression r = new MultilinearRegression();
  IntArrayList lastSpacePlayed = new IntArrayList(new int[] {-1});
  
  public EvaluatorLastMoves() {
    initMoves();
  }
  
  // HANDLING EMPTY CASES
  protected int[] getEmpties() {
    int[] result = new int[getNEmpties()];
    int i = 0;
    
    for (Space space : spaces) {
      for (int empty : space.toArray()) {
        result[i++] = empty;
      }
    }
    return result;
  }
  public int getNEmpties() {
    int result = 0;
    for (Space space : spaces) {
      result += space.nEmpties;
    }
    return result;
  }
  
  protected void setEmpties(Board b) {
    setEmpties(b.getPlayer(), b.getOpponent());
  }

  protected void setEmpties(long player, long opponent) {
    long emptySquares = ~(player | opponent);
    spaces = new Space[] {new Space(0), new Space(1), new Space(2), new Space(3)};
    
    while (emptySquares != 0) {
      int empty = Long.numberOfTrailingZeros(emptySquares);
      emptySquares = emptySquares & ~(1L << empty);
      spaces[empty % 8 >= 4 ? 1 : 0 + 2 * (empty / 32)].addSorted(empty);
    }
    Arrays.sort(spaces);
  }
  
  private Space bestSpace() {
    Space toReturn = null;
    for (Space s : spaces) {
      toReturn = (toReturn == null || s.compareTo(toReturn) < 0) ? s : toReturn;
    }
    return toReturn;
  }
  private Space bestSpaceAfterSpace(Space space) {
    Space toReturn = null;
    for (Space s : spaces) {
      if (s.compareTo(space) <= 0) {
        continue;
      }
      toReturn = toReturn == null || s.compareTo(toReturn) < 0 ? s : toReturn;
    }
    return toReturn;
  }
  
  Empty firstEmpty() {
    return nextEmpty(bestSpace().firstEmpty);
  }

  Empty nextEmpty(Empty empty) {
    Empty toReturn = empty.next;
    Space space = empty.space;

    if (toReturn.position == -1) {
      space = bestSpaceAfterSpace(space);
      if (space == null || space.nEmpties == 0) {
        return null;
      }
      toReturn = space.firstEmpty.next;
    } 
    toReturn.space.removeElement(toReturn);
    lastSpacePlayed.push(toReturn.space.number);
    return toReturn;
  }
  
  void reAddEmpty(Empty empty) {
    empty.space.reAddElement(empty);
    lastSpacePlayed.popInt();
  }
  
  // COMPUTING FLIPS.
  
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

  private void initMoves() {
    for (int position = 0; position < 64; position++) {
      long bitPatterns[] = {BitPattern.getRow(position), 
        BitPattern.getColumn(position), BitPattern.getNESWDiag(position),
        BitPattern.getNWSEDiag(position)};
      long[][][] flips = {flipHorizontal, flipVertical, flipReverseDiagonal, flipDiagonal};

      for (int i = 0; i < bitPatterns.length; i++) {
        long bitPattern = bitPatterns[i];
        long[][] flip = flips[i];
        for (Board board : Board.existingBoardsInBitPattern(bitPattern)) {
          LongArrayList possibleMoves = possibleMovesBasic(board);
          int hash = hashGeneric(
            position, board.getPlayer(), board.getOpponent(), bitPattern);

          for (int j = 0; j < possibleMoves.size(); j++) {
            long curFlip = possibleMoves.getLong(j);          
            int thisMovePosition = Long.numberOfTrailingZeros(
              curFlip & (~board.getPlayer()) & (~board.getOpponent()));
            if (thisMovePosition == position) {
              flip[position][hash] = curFlip;
            }
          }
        }
      }
    }
  }
  
//  protected long getFlip1(int move, long player, long opponent) {
//    long moveBit = 1L << move;
//    int rightShift = move % 8;
//    int leftShift = 7 - rightShift;
//    int bottomShift = move / 8;
//    int topShift = 7 - bottomShift;
//    int dirs[] = new int[] {-9, -8, -7, -1, 1, 7, 8, 9};
//    int maxShift[] = new int[] {Math.min(leftShift, topShift), topShift, Math.min(rightShift, topShift),
//                                leftShift, rightShift,
//                                Math.min(leftShift, bottomShift), bottomShift, Math.min(rightShift, bottomShift)};
//                                
//    long flip = 0;
//    for (int i = 0; i < 8; i++) {
//      long curFlip = 0;
//      long c;
//      int dir = dirs[i];
//      for (int j = 1; j <= maxShift[i]; j++) {
//        c = 1L << (move - j * dir);
//        if ((c & opponent) != 0) {
//          curFlip |= c;
//        } else if ((c & player) != 0) {
//          flip |= curFlip;
//          break;
//        } else {
//          break;
//        }
//      }
//    }
//    flip = flip == 0 ? 0 : flip | moveBit;
//    return flip;
//  }

  protected long getFlip(int move, long player, long opponent) {
    long row = flipHorizontal[move][hashRow(move, player, opponent)];
    long column = flipVertical[move][hashColumn(move, player, opponent)];
    long diag = flipDiagonal[move][hashDiagonal(move, player, opponent)];
    long revDiag = flipReverseDiagonal[move][hashRevDiagonal(move, player, opponent)];

    return row | column | diag | revDiag;
  }
  
  // EVALUATE.
  private int evalOneEmpty(long player, long opponent, boolean passed) {
    long flip = getFlip(bestSpace().firstEmpty.next.position, player, opponent);
    return flip != 0
        ? BitPattern.getEvaluationBoardFull(player | flip)
        : passed
            ? BitPattern.getEvaluationGameOver(player, opponent)
            : -evaluateRecursive(opponent, player, -6400, 6400, true);
  }
  
  private int evaluateRecursive(long player, long opponent, int alpha, int beta,
                                  boolean passed) {
    if (!passed) {
      nVisited++;
    }
    
    if (this.getNEmpties() == 1) {
      return evalOneEmpty(player, opponent, passed);
    }
    boolean pass = true;
    int best = Integer.MIN_VALUE;
    int move;
    
    for (Empty empty = firstEmpty(); empty != null; empty = nextEmpty(empty)) {
      move = empty.position;
      long flip = getFlip(move, player, opponent);
      if (flip == 0) {
        reAddEmpty(empty);
        continue;
      }
      best = Math.max(best, 
        -evaluateRecursive(opponent & ~flip, player | flip, -beta, 
                           -Math.max(alpha, best), false));
      pass = false;
      reAddEmpty(empty);
      if (best >= beta) {
        break;
      }
    }
    if (pass) {
      if (passed) {
        return BitPattern.getEvaluationGameOver(player, opponent);
      }
      return -evaluateRecursive(opponent, player, -beta, -alpha, true);
    }
    return best;
  }
  
  public int evaluatePosition(Board b, int alpha, int beta) {
    long player = b.getPlayer();
    long opponent = b.getOpponent();
    setEmpties(player, opponent);
    if (this.getNEmpties() == 0) {
      return BitPattern.getEvaluationBoardFull(b);
    }
    nVisited--;
    return evaluateRecursive(player, opponent, alpha, beta, false);
  }
  
  public void resetNVisited() {
    nVisited = 0;
  }
  
  
  public int getNVisited() {
    return nVisited;
  }
  
  public static void main(String args[]) {
    Board b = new Board("--XXXXX--OOOXX-O-OOOXXOX-OXOXOXXOXXXOXXX--XOXOXX-XXXOOO--OOOOO--", true);
    EvaluatorLastMoves eval = new EvaluatorLastMoves();
    long t = System.currentTimeMillis();
    eval.evaluatePosition(b, -6400, 6400);
    System.out.println(eval.getNVisited() + " " + (System.currentTimeMillis() - t));
  }
}
