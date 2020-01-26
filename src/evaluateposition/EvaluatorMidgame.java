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
import evaluatedepthone.PatternEvaluatorImproved;
import evaluateposition.HashMapVisitedPositionsForAlphaBeta.StoredBoardForAlphaBeta;
import it.unimi.dsi.fastutil.objects.ObjectArrayList;
import java.util.Collections;

public class EvaluatorMidgame {
  final DepthOneEvaluator depthOneEvaluator;
  private final GetFlip flipper;
//  private final HashMapVisitedPositionsForAlphaBeta hashMapVisitedPositions;
  private long nVisitedPositions = 0;
  private long nComputedMoves = 0;
  private final EvaluatorLastMoves lastMovesEvaluator;

  private BoardWithEvaluation[][] nextPositions;

  public static class Move implements Comparable<Move> {
    int move;
    long flip;
    int eval;
    int nextOpponentMoves;
    int lower;
    int upper;
    float error;
    
    @Override
    public int compareTo(Move other) {
      return Float.compare(other.value(), value());
    }
    
    public float value() {
      return eval;
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
    this.lastMovesEvaluator = new EvaluatorLastMoves(getFlip);
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
      if (nEmpties <= Constants.EMPTIES_FOR_ENDGAME) {
        lastMovesEvaluator.resetNVisited();
        currentEval = -lastMovesEvaluator.evaluatePosition(opponent & ~flip, player | flip, -upper, -lower, empties);
        nVisitedPositions += lastMovesEvaluator.getNVisited();
      } else if (depth <= 1) {
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

  ObjectArrayList<Move> getMoves(long player, long opponent, int lower, int upper) {
    int move;
    long moveBit;
    long empties = ~(player | opponent);
    ObjectArrayList<Move> moves = new ObjectArrayList<>();

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
      curMove.move = move;
      curMove.flip = flip;
      curMove.eval = -this.depthOneEvaluator.eval();
      curMove.error = this.depthOneEvaluator.lastError();
      curMove.nextOpponentMoves = Long.bitCount(
          (empties & ~moveBit) & GetFlip.neighbors(flip | player));
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
    int nEmpties = Long.bitCount(~(player | opponent)) - 1;
    int move;
    long flip;
    boolean pass = true;
    int currentEval;
    int bestEval = Integer.MIN_VALUE;
    nComputedMoves++;
    this.depthOneEvaluator.invert();

    ObjectArrayList<Move> moves = getMoves(player, opponent, lower, upper);
    for (Move curMove : moves) {
      move = curMove.move;
      flip = curMove.flip;

      pass = false;
      if (nEmpties <= Constants.EMPTIES_FOR_ENDGAME) {
        lastMovesEvaluator.resetNVisited();
        currentEval = -lastMovesEvaluator.evaluatePosition(opponent & ~flip, player | flip, -upper, -Math.max(lower, bestEval), 64);
        nVisitedPositions += lastMovesEvaluator.getNVisited();
      } else if (depth <= 1) {
        currentEval = -curMove.eval;
      } else {
        depthOneEvaluator.update(move, flip);
        if (depth <= 3) {
          currentEval = -evaluatePositionQuick(opponent & ~flip, player | flip, depth - 1, -upper, -Math.max(lower, bestEval), false, move);
        } else {
          currentEval = -evaluatePositionSlow(opponent & ~flip, player | flip, depth - 1, -upper, -Math.max(lower, bestEval), false);
        }
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
        bestEval = -evaluatePositionSlow(opponent, player, depth, -upper, -lower, true);
      }
    }
    
    this.depthOneEvaluator.invert();
    return bestEval;    
  }
  
  public long getNComputedMoves() {
    return this.nComputedMoves;
  }
  
  public long getNVisitedPositions() {
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
    return evaluatePositionSlow(current.getPlayer(), current.getOpponent(), depth, lower, upper, false);
//    return evaluatePositionQuick(current.getPlayer(), current.getOpponent(), depth, lower, upper, false, 64);
  }
  
//  public StoredBoardForAlphaBeta getStoredBoard(Board board) {
//    return this.hashMapVisitedPositions.getStoredBoard(board);
//  }

  public void resetNVisitedPositions() {
    this.nVisitedPositions = 0;
    this.nComputedMoves = 0;
  }

  public long getNVisited() {
    return this.nVisitedPositions;
  }
  
  public static void main(String args[]) {
    
  }
}
