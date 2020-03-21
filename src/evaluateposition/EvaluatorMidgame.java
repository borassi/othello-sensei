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
import constants.Constants;
import evaluatedepthone.BoardWithEvaluation;
import evaluatedepthone.DepthOneEvaluator;
import evaluatedepthone.FindStableDisks;
import evaluatedepthone.PatternEvaluatorImproved;
import java.util.ArrayList;
import java.util.Collections;

public class EvaluatorMidgame {
  final DepthOneEvaluator depthOneEvaluator;
  private final GetFlip flipper;
//  private final HashMapVisitedPositionsForAlphaBeta hashMapVisitedPositions;
  private long nVisitedPositions = 0;
  private long nComputedMoves = 0;
  private final EvaluatorLastMoves lastMovesEvaluator;
  private final HashMapVisitedPositionsForAlphaBeta hashMap = new HashMapVisitedPositionsForAlphaBeta();
  private final FindStableDisks findStableDisks = new FindStableDisks();

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
  public static class Move implements Comparable<Move> {
    int move;
    long flip;
    int eval;
    int nextOpponentMoves;
    int lower;
    int upper;
    float error;
    boolean isInHash;
    
    @Override
    public int compareTo(Move other) {
      if (isInHash && !other.isInHash) {
        return -1;
      } else if (other.isInHash && !isInHash) {
        return 1;
      }
      return Float.compare(other.value(), value());
    }
    
    public float value() {
      return -nextOpponentMoves * 60000 + eval * 100 + SQUARE_VALUE[move];
    }
  }

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
  }
  
//  public void printHashMapVisitedPositions() {
//    System.out.println(hashMapVisitedPositions);
//  }

  public int evaluatePositionQuick(
      long player, long opponent, int depth, int lower, int upper, boolean passed, int lastMove) {
    long empties = (~(player | opponent));
    int nEmpties = Long.bitCount(empties) - 1;
    empties = (~(player | opponent)) & GetFlip.neighbors(opponent);
    int move;
    long flip;
    boolean pass = true;
    int currentEval;
    int bestEval = Integer.MIN_VALUE;
    nComputedMoves++;
    this.depthOneEvaluator.invert();

    while (empties != 0) {
      move = Long.numberOfTrailingZeros(empties);      
      empties = empties & (~(1L << move));
      flip = flipper.getFlip(move, player, opponent);

      if (flip == 0) {
        continue;
      }
      nVisitedPositions++;
      pass = false;
//      if (depth >= nEmpties && nEmpties <= Constants.EMPTIES_FOR_ENDGAME) {
//        lastMovesEvaluator.resetNVisited();
//        currentEval = -lastMovesEvaluator.evaluatePosition(opponent & ~flip, player | flip, -upper, -lower, empties);
//        nVisitedPositions += lastMovesEvaluator.getNVisited();
//      } else
      if (depth <= 1) {
        depthOneEvaluator.update(move, flip);
        currentEval = -depthOneEvaluator.eval();
        depthOneEvaluator.undoUpdate(move, flip);
      } else {
        depthOneEvaluator.update(move, flip);
        currentEval = -evaluatePositionQuick(opponent & ~flip, player | flip, depth - 1, -upper, -Math.max(lower, bestEval), false, move);
        depthOneEvaluator.undoUpdate(move, flip);
      }
      bestEval = bestEval > currentEval ? bestEval : currentEval;
      if (bestEval >= upper) {
        break;
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

  ArrayList<Move> getMoves(long player, long opponent, int lower, int upper, int depth) {
    int move;
    long moveBit;
    long empties = ~(player | opponent);
    ArrayList<Move> moves = new ArrayList<>();

    while (empties != 0) {
      move = Long.numberOfTrailingZeros(empties);
      moveBit = 1L << move;
      empties = empties & (~moveBit);
      long flip = flipper.getFlip(move, player, opponent) & ~player;
      if (flip == 0) {
        continue;
      }
      nVisitedPositions++;
      Move curMove = new Move();
      this.depthOneEvaluator.update(move, flip);
      HashMapVisitedPositionsForAlphaBeta.StoredBoardForAlphaBeta boardInHash =
          hashMap.getStoredBoard(opponent & ~flip, player | flip);
      curMove.isInHash = boardInHash != null && (boardInHash.depthLower >= depth || boardInHash.depthUpper >= depth);
      curMove.move = move;
      curMove.flip = flip;
      curMove.eval = -this.depthOneEvaluator.eval();
      curMove.error = this.depthOneEvaluator.lastError();
      
      curMove.nextOpponentMoves = 0;
      long newPlayer = opponent & ~flip;
      long newOpponent = player | flip;
      long possibleMoves = ~(newPlayer | newOpponent) & GetFlip.neighbors(newOpponent);
      while (possibleMoves != 0) {
        int possibleMove = Long.numberOfTrailingZeros(possibleMoves);
        long possibleMoveBit = 1L << possibleMove;
        possibleMoves = possibleMoves & (~possibleMoveBit);
        long possibleFlip = flipper.getFlip(possibleMove, newPlayer, newOpponent);
        if (possibleFlip != 0) {
          curMove.nextOpponentMoves++;
          if (possibleMove == 0 || possibleMove == 7 || possibleMove == 56 || possibleMove == 63) {
            curMove.nextOpponentMoves++;
          }
        }
      }
//      curMove.nextOpponentMoves = Long.bitCount(
//          (empties & ~moveBit) & GetFlip.neighbors(flip | player));
      curMove.lower = lower;
      curMove.upper = upper;
      this.depthOneEvaluator.undoUpdate(move, flip);
      moves.add(curMove);
    }
    Collections.sort(moves);
    return moves;
  }
  
  public int evaluatePositionSlow(
      long player, long opponent, int depth, int lower, int upper, boolean passed) {
    HashMapVisitedPositionsForAlphaBeta.StoredBoardForAlphaBeta boardInHash = hashMap.getStoredBoard(player, opponent);
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
    nComputedMoves++;
    this.depthOneEvaluator.invert();

    ArrayList<Move> moves = getMoves(player, opponent, lower, upper, depth);
    for (Move curMove : moves) {
      move = curMove.move;
      flip = curMove.flip;

      pass = false;
      int newLower = Math.max(lower, bestEval);
      if (depth <= 1) {
        currentEval = curMove.eval;
      } else if (depth >= nEmpties && nEmpties <= Constants.EMPTIES_FOR_ENDGAME) {
        lastMovesEvaluator.resetNVisited();
        currentEval = -lastMovesEvaluator.evaluatePosition(
            opponent & ~flip, player | flip, -upper, -newLower, 64);
        nVisitedPositions += lastMovesEvaluator.getNVisited();
      } else {
        depthOneEvaluator.update(move, flip);
        if (depth <= 3) {
          currentEval = -evaluatePositionQuick(opponent & ~flip, player | flip, depth - 1, -upper, -newLower, false, move);
        } else {
          if (bestEval == Integer.MIN_VALUE || upper - newLower < 200) {
            currentEval = -evaluatePositionSlow(opponent & ~flip, player | flip, depth - 1, -upper, -newLower, false);
          } else {
            currentEval = -evaluatePositionSlow(opponent & ~flip, player | flip, depth - 1, -bestEval-1, -bestEval, false);
            if (currentEval > bestEval && currentEval < upper) {
              currentEval = -evaluatePositionSlow(opponent & ~flip, player | flip, depth - 1, -upper, -newLower, false);
            }
          }
        }
        depthOneEvaluator.undoUpdate(move, flip);
      }
      bestEval = Math.max(bestEval, currentEval);
      if (bestEval >= upper) {
        break;
      }
    }

    if (pass) {
      if (passed) {
        bestEval = BitPattern.getEvaluationGameOver(player, opponent);
      } else {
        bestEval = -evaluatePositionSlow(opponent, player, depth, -upper, -lower, true);
      }
    }
    
    this.depthOneEvaluator.invert();
    if (bestEval > lower) {
      hashMap.updateLowerBound(player, opponent, bestEval, depth);
    }
    if (bestEval < upper) {
      hashMap.updateUpperBound(player, opponent, bestEval, depth);
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
    int approxEval = evaluatePositionSlow(current.getPlayer(), current.getOpponent(), depth - 8, lower, upper, false);
    return evaluatePositionSlow(current.getPlayer(), current.getOpponent(), depth, lower, upper, false);
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
