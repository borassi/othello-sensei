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
import board.GetFlip;
import board.GetMoves;
import board.GetMovesCache;
import constants.Constants;
import evaluatedepthone.DepthOneEvaluator;
import evaluatedepthone.FindStableDisks;
import evaluatedepthone.PatternEvaluatorImproved;
import java.util.Arrays;

public class EvaluatorMidgame {
  static final int SQUARE_VALUE[] = {
    // JCW's score:
    18,  4,  16, 12, 12, 16,  4, 18,
     4,  2,   6,  8,  8,  6,  2,  4,
    16,  6,  14, 10, 10, 14,  6, 16,
    12,  8,  10,  0,  0, 10,  8, 12,
    12,  8,  10,  0,  0, 10,  8, 12,
    16,  6,  14, 10, 10, 14,  6, 16,
     4,  2,   6,  8,  8,  6,  2,  4,
    18,  4,  16, 12, 12, 16,  4, 18
  };
  public class Move implements Comparable<Move> {
    int move;
    long flip;
    int value;
//    int eval;
//    int nextOpponentMoves;
//    int lower;
//    int upper;
//    float error;
//    boolean isInHash;

    @Override
    public int compareTo(Move other) {
////      System.out.println(isInHash + " " + other.isInHash);
//      if (isInHash && !other.isInHash) {
//        return -1;
//      } else if (other.isInHash && !isInHash) {
//        return 1;
//      }
      return Float.compare(other.value, value);
    }
//
//    public float value() {
//      return -nextOpponentMoves * 60000 + eval * 100 + SQUARE_VALUE[move];
//    }
  }
  
  final DepthOneEvaluator depthOneEvaluator;
  private final GetFlip flipper;
//  private final HashMapVisitedPositionsForAlphaBeta hashMapVisitedPositions;
  private long nVisitedPositions = 0;
  private long nComputedMoves = 0;
  private final EvaluatorLastMoves lastMovesEvaluator;
  private final HashMap hashMap = new HashMap();
  private final FindStableDisks findStableDisks = new FindStableDisks();
  Move[][] moves = new Move[64][64];

  public EvaluatorMidgame() {
    this(PatternEvaluatorImproved.load());
  }

  public EvaluatorMidgame(DepthOneEvaluator depthOneEvaluator) {
    this(depthOneEvaluator, GetFlip.load());
  }

  public EvaluatorMidgame(
      DepthOneEvaluator depthOneEvaluator,
      GetFlip getFlip) {
    this.depthOneEvaluator = depthOneEvaluator;
    this.flipper = getFlip;
    this.lastMovesEvaluator = new EvaluatorLastMoves(getFlip, this.findStableDisks);
    for (int i = 0; i < this.moves.length; ++i) {
      for (int j = 0; j < this.moves[i].length; ++j) {
        this.moves[i][j] = new Move();
      }
    }
  }
  
//  public void printHashMapVisitedPositions() {
//    System.out.println(hashMapVisitedPositions);
//  }

  public int evaluatePositionQuick(
      long player, long opponent, int depth, int lower, int upper, boolean passed, int lastMove) {
    long movesBit = GetMoves.getMoves(player, opponent);
    int move;
    long flip;
    boolean pass = true;
    int currentEval;
    int bestEval = Integer.MIN_VALUE;
    nComputedMoves++;

    if (depth <= 1) {
      int depthZeroEval = depthOneEvaluator.eval();
      depthOneEvaluator.invert();
      while (movesBit != 0) {
        flip = Long.lowestOneBit(movesBit);
        move = Long.numberOfTrailingZeros(flip);      
        movesBit = movesBit & (~flip);
        flip = GetMoves.getFlip(move, player, opponent);

        nVisitedPositions++;
        pass = false;

        depthOneEvaluator.update(move, flip);
        currentEval = (depthZeroEval * Constants.WEIGHT_DEPTH_1 - depthOneEvaluator.eval() * Constants.WEIGHT_DEPTH_0) /
            (Constants.WEIGHT_DEPTH_1 + Constants.WEIGHT_DEPTH_0);
        depthOneEvaluator.undoUpdate(move, flip);
        bestEval = bestEval > currentEval ? bestEval : currentEval;
        if (bestEval >= upper) {
          break;
        }
      }
    } else {
      depthOneEvaluator.invert();
      while (movesBit != 0) {
        flip = Long.lowestOneBit(movesBit);
        move = Long.numberOfTrailingZeros(flip);      
        movesBit = movesBit & (~flip);
        flip = GetMoves.getFlip(move, player, opponent);

        nVisitedPositions++;
        pass = false;
        depthOneEvaluator.update(move, flip);
        currentEval = -evaluatePositionQuick(opponent & ~flip, player | flip, depth - 1, -upper, -Math.max(lower, bestEval), false, move);
        depthOneEvaluator.undoUpdate(move, flip);
        bestEval = bestEval > currentEval ? bestEval : currentEval;
        if (bestEval >= upper) {
          break;
        }
      }
    }

    if (pass) {
      if (passed) {
        bestEval = BitPattern.getEvaluationGameOver(player, opponent);
      } else {
        bestEval = -evaluatePositionQuick(opponent, player, depth, -upper, -lower, true, lastMove);
      }
    }
    
    this.depthOneEvaluator.invert();
    return bestEval;    
  }
  

  int getMoves(long player, long opponent, int lower, int upper, int depth, HashMap.BoardInHash boardInHash, Move[] output) {
    int move;
    long moveBit;
    GetMovesCache mover = new GetMovesCache();
    long curMoves = mover.getMoves(player, opponent);
    long flip;
    int nMoves = 0;
    int value = 0;

    while (curMoves != 0) {
      move = Long.numberOfTrailingZeros(curMoves);
      moveBit = 1L << move;
      curMoves = curMoves & (~moveBit);
      flip = mover.getFlip(move) & (opponent | moveBit);
      nVisitedPositions++;
      Move curMove = output[nMoves++];
      curMove.move = move;
      curMove.flip = flip;
      if (boardInHash != null && move == boardInHash.bestMove) {
        value = 1 << 30;
      } else if (boardInHash != null && move == boardInHash.secondBestMove) {
        value = 1 << 29;
      } else {
        value = -(GetMoves.getWeightedNMoves(opponent & ~flip, player | flip) << 16);
//        if (depth > Constants.EMPTIES_FOR_ENDGAME + 3) {
//          this.depthOneEvaluator.update(move, flip);
//          value += -(this.depthOneEvaluator.eval() << 7);
//          this.depthOneEvaluator.undoUpdate(move, flip);
//        }
        value |= SQUARE_VALUE[move];
      }
      curMove.value = value;
    }
    Arrays.sort(output, 0, nMoves);
    return nMoves;
  }
  public int nEndgames = 0;
  public long nVisitedEndgames = 0;
  public int evaluatePositionSlow(
      long player, long opponent, int depth, int lower, int upper, boolean passed, boolean fast) {
    HashMap.BoardInHash boardInHash = null;
    boardInHash = hashMap.getStoredBoard(player, opponent);
    if (boardInHash != null) {
      if (boardInHash.lower >= upper && boardInHash.depthLower >= depth) {
        return boardInHash.lower;
      }
      if (boardInHash.upper <= lower && boardInHash.depthUpper >= depth) {
        return boardInHash.upper;
      }
      if (boardInHash.lower == boardInHash.upper && boardInHash.depthLower >= depth
          && boardInHash.depthUpper >= depth) {
        return boardInHash.lower;
      }
    }
    int stabilityCutoffUpper = findStableDisks.getUpperBound(player, opponent);
    if (stabilityCutoffUpper <= lower) {
      return stabilityCutoffUpper;
    }
    int nEmpties = Long.bitCount(~(player | opponent)) - 1;
    int move;
    long flip;
    boolean pass = true;
    int currentEval;
    int bestEval = Integer.MIN_VALUE;
    int bestMove = -1;
    int secondBestEval = Integer.MIN_VALUE;
    int secondBestMove = -1;
    int depthZeroEval = depthOneEvaluator.eval();
    long nVisited;
 
    nComputedMoves++;
    this.depthOneEvaluator.invert();
    Move[] moves = this.moves[nEmpties];

    int nMoves = getMoves(player, opponent, lower, upper, depth, boardInHash, moves);
    for (int i = 0; i < nMoves; ++i) {
      Move curMove = moves[i];
      move = curMove.move;
      flip = curMove.flip;

      pass = false;
      int newLower = Math.max(lower, bestEval);
      if (depth <= 1) {
        depthOneEvaluator.update(move, flip);
        currentEval = (depthZeroEval * Constants.WEIGHT_DEPTH_1 - depthOneEvaluator.eval() * Constants.WEIGHT_DEPTH_0) /
            (Constants.WEIGHT_DEPTH_1 + Constants.WEIGHT_DEPTH_0);
        depthOneEvaluator.undoUpdate(move, flip);        
      } else if (depth >= nEmpties && (
          nEmpties <= Constants.EMPTIES_FOR_ENDGAME ||
          (nEmpties == Constants.EMPTIES_FOR_ENDGAME + 1 && upper - lower <= 200 && Math.abs(depthZeroEval - upper) > 600) ||
          (nEmpties == Constants.EMPTIES_FOR_ENDGAME + 2 && upper - lower <= 200 && Math.abs(depthZeroEval - upper) > 1800))) {
        lastMovesEvaluator.resetNVisited();
        currentEval = -lastMovesEvaluator.evaluatePosition(
            opponent & ~flip, player | flip, -upper, -newLower, 64);
        nVisited = lastMovesEvaluator.getNVisited();
        nVisitedPositions += nVisited;
        nVisitedEndgames += nVisited;
        nEndgames++;
      } else {
        depthOneEvaluator.update(move, flip);
        if (depth <= 3 && fast) {
          currentEval = -evaluatePositionQuick(opponent & ~flip, player | flip, depth - 1, -upper, -newLower, false, move);
        } else {
          if (bestEval == Integer.MIN_VALUE || upper - newLower < 200) {
            currentEval = -evaluatePositionSlow(opponent & ~flip, player | flip, depth - 1, -upper, -newLower, false, fast);
          } else {
            currentEval = -evaluatePositionSlow(opponent & ~flip, player | flip, depth - 1, -bestEval-1, -bestEval, false, fast);
            if (currentEval > bestEval && currentEval < upper) {
              currentEval = -evaluatePositionSlow(opponent & ~flip, player | flip, depth - 1, -upper, -newLower, false, fast);
            }
          }
        }
        depthOneEvaluator.undoUpdate(move, flip);
      }
      if (currentEval > bestEval) {
        secondBestEval = bestEval;
        secondBestMove = bestMove;
        bestEval = currentEval;
        bestMove = move;
      } else if (currentEval > secondBestEval) {
        secondBestEval = currentEval;
        secondBestMove = move;
      }
      if (bestEval >= upper) {
        break;
      }
    }

    if (pass) {
      if (passed) {
        bestEval = BitPattern.getEvaluationGameOver(player, opponent);
      } else {
        bestEval = -evaluatePositionSlow(opponent, player, depth, -upper, -lower, true, fast);
      }
    }
    
    this.depthOneEvaluator.invert();
    if (nEmpties >= Constants.EMPTIES_FOR_ENDGAME + 3 || depth >= nEmpties) {
      if (bestEval > lower) {
        hashMap.updateLowerBound(player, opponent, bestEval, depth, bestMove, secondBestMove);
      }
      if (bestEval < upper) {
        hashMap.updateUpperBound(player, opponent, bestEval, depth, bestMove, secondBestMove);
      }
    }
    return bestEval;    
  }
  
  public long getNComputedMoves() {
    return this.nComputedMoves;
  }
  
  public long getNVisited() {
    return this.nVisitedPositions;
  }
  
//  public void resetHashMapVisitedPositions() {
//    this.hashMapVisitedPositions.reset();
//  }
  /**
   * - Returns value and samples.
   * - Uses hash map.
   * - Removes positions that are < lower or > upper.
   * - V Very fast at small depth.
   * - V Goes up to depth when evaluating.
   * - V At depth 8, uses EvaluatorLastMoves.
   * @param current
   * @param depth
   * @param lower
   * @param upper 
   */
  public int evaluatePosition(Board current, int depth, int lower, int upper) {
//    assert(current.isLeaf());
//    evaluatePositionWithHashMap(current.getBoard(), depth - 4, lower, upper);
    depthOneEvaluator.setup(current.getPlayer(), current.getOpponent());
    if (depth == 0) {
      return depthOneEvaluator.eval();
    }
    depth = Math.min(depth, current.getEmptySquares());
//    this.resetNVisitedPositions();
    if (depth <= 3) {
      return evaluatePositionQuick(current.getPlayer(), current.getOpponent(), depth,
        lower, upper, false, 64);
    }
    evaluatePositionSlow(
        current.getPlayer(), current.getOpponent(), depth - Constants.EMPTIES_FOR_ENDGAME - 1,
        lower, upper, false, false);
    return evaluatePositionSlow(current.getPlayer(), current.getOpponent(), depth, lower, upper, false, true);
//    approxEval = approxEval + 100;
//    int attemptLower = Math.max(lower, approxEval - (approxEval + 6400) % 200 - 100);
////    System.out.print(tmp + " " + attemptLower + " ");
//    int eval;
//    int i = 0;
//    int step = 200;
//    while (true) {
//      ++i;
//      int attemptUpper = upper;//attemptLower + step;
//      eval = evaluatePositionSlow(current.getPlayer(), current.getOpponent(), depth, attemptLower, attemptUpper, false);
//      if (eval <= lower || eval >= upper) {
//        return eval;
//      }
//      if (eval < attemptLower) {
//        attemptLower -= step;
//      } else if (eval > attemptLower + step) {
//        attemptLower += step;
//      } else {
//        return eval;
//      }
////      step *= 2;
//    }
  }
  
//  public StoredBoardForAlphaBeta getStoredBoard(Board board) {
//    return this.hashMapVisitedPositions.getStoredBoard(board);
//  }

  public void resetNVisitedPositions() {
    this.nVisitedPositions = 0;
    this.nComputedMoves = 0;
  }
  
  public static void main(String args[]) {
    
  }
}