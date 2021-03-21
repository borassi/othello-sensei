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
import board.GetMoves;
import board.GetMovesCache;
import constants.Constants;
import evaluatedepthone.DepthOneEvaluator;
import evaluatedepthone.FindStableDisks;
import evaluatedepthone.PatternEvaluatorImproved;
import java.util.Arrays;

public class EvaluatorMidgame implements EvaluatorInterface {
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

    @Override
    public int compareTo(Move other) {
      return Float.compare(other.value, value);
    }
  }
  
  final DepthOneEvaluator depthOneEvaluator;
  private long nVisitedPositions = 0;
  private long nComputedMoves = 0;
  private final EvaluatorLastMoves lastMovesEvaluator;
  private final HashMap hashMap;
  private final FindStableDisks findStableDisks = FindStableDisks.load();
  private final int logNPlusOne[];
  Move[][] moves = new Move[64][64];

  public EvaluatorMidgame() {
    this(PatternEvaluatorImproved.load());
  }

  public EvaluatorMidgame(DepthOneEvaluator depthOneEvaluator) {
    this(depthOneEvaluator, new HashMap());
  }

  public EvaluatorMidgame(DepthOneEvaluator depthOneEvaluator, HashMap hashMap) {
    this.depthOneEvaluator = depthOneEvaluator;
    this.lastMovesEvaluator = new EvaluatorLastMoves(this.findStableDisks);
    for (int i = 0; i < this.moves.length; ++i) {
      for (int j = 0; j < this.moves[i].length; ++j) {
        this.moves[i][j] = new Move();
      }
    }
    this.hashMap = hashMap;
    logNPlusOne = new int[64];
    for (int i = 0; i < logNPlusOne.length; ++i) {
      logNPlusOne[i] = (int) (Math.log(i+1) * 100);
    }
    
  }

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

  public double getDisproofNumber(
      long player, long opponent, int depth, int evalGoal, double bestProofNumber, boolean passed) {
//    assert (evalGoal + 200000) % 200 == 100;
    long movesBit = GetMoves.getMoves(player, opponent);
    int move;
    long flip;
    boolean pass = true;
    nComputedMoves++;
    double result = 0;
    int currentEval;
    int depthZeroEval = depthOneEvaluator.eval();

    depthOneEvaluator.invert();
    while (movesBit != 0) {
      flip = Long.lowestOneBit(movesBit);
      move = Long.numberOfTrailingZeros(flip);      
      movesBit = movesBit & (~flip);
      flip = GetMoves.getFlip(move, player, opponent);

//      nVisitedPositions++;
      pass = false;
      depthOneEvaluator.update(move, flip);
      if (depth > 1) {
        result += getProofNumber(opponent & ~flip, player | flip, depth - 1, -evalGoal, bestProofNumber, false);
      } else {
        currentEval = (depthZeroEval * Constants.WEIGHT_DEPTH_1 - depthOneEvaluator.eval() * Constants.WEIGHT_DEPTH_0) /
            (Constants.WEIGHT_DEPTH_1 + Constants.WEIGHT_DEPTH_0);
        result += StoredBoard.endgameTimeEstimator.disproofNumber(player, opponent, evalGoal, currentEval);          
      }
      depthOneEvaluator.undoUpdate(move, flip);
    }

    if (pass) {
      if (passed) {
        result = BitPattern.getEvaluationGameOver(player, opponent) > evalGoal ? Double.POSITIVE_INFINITY : 0;
      } else {
        result = getProofNumber(opponent, player, depth, -evalGoal, bestProofNumber, true);
      }
    }
    
    depthOneEvaluator.invert();
    return result;    
  }

  public double getProofNumber(
      long player, long opponent, int depth, int evalGoal, double bestProofNumber, boolean passed) {
    assert (evalGoal + 200000) % 200 == 100;
    long movesBit = GetMoves.getMoves(player, opponent);
    int move;
    long flip;
    boolean pass = true;
    nComputedMoves++;
    double result = Double.POSITIVE_INFINITY;
    int currentEval;
    int depthZeroEval = depthOneEvaluator.eval();

    depthOneEvaluator.invert();
    while (movesBit != 0) {
      flip = Long.lowestOneBit(movesBit);
      move = Long.numberOfTrailingZeros(flip);      
      movesBit = movesBit & (~flip);
      flip = GetMoves.getFlip(move, player, opponent);
//      nVisitedPositions++;
      pass = false;
      depthOneEvaluator.update(move, flip);
//      depthOneEvaluator.setup(opponent & ~flip, player | flip);
      if (depth > 1) {
        result = Math.min(result, getDisproofNumber(opponent & ~flip, player | flip, depth - 1, -evalGoal, Math.min(result, bestProofNumber), false));
      } else {
        currentEval = (depthZeroEval * Constants.WEIGHT_DEPTH_1 - depthOneEvaluator.eval() * Constants.WEIGHT_DEPTH_0) /
            (Constants.WEIGHT_DEPTH_1 + Constants.WEIGHT_DEPTH_0);
        result = Math.min(result, StoredBoard.endgameTimeEstimator.proofNumber(player, opponent, evalGoal, currentEval));          
      }
      depthOneEvaluator.undoUpdate(move, flip);
    }

    if (pass) {
      if (passed) {
        result = BitPattern.getEvaluationGameOver(player, opponent) > evalGoal ? 0 : Double.POSITIVE_INFINITY;
      } else {
        result = getDisproofNumber(opponent, player, depth, -evalGoal, Double.POSITIVE_INFINITY, true);
      }
    }
    
    this.depthOneEvaluator.invert();
    return result;    
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
      if (depth > Constants.EMPTIES_FOR_ENDGAME + 2) {
        this.depthOneEvaluator.update(move, flip);
        curMove.value = -(int) StoredBoard.endgameTimeEstimator.disproofNumber(opponent & ~flip, player | flip, -upper, this.depthOneEvaluator.eval());   
        this.depthOneEvaluator.undoUpdate(move, flip);     
      } else {
        value = 1 + GetMoves.getWeightedNMoves(opponent & ~flip, player | flip);        
        curMove.value = -(value * value * value) * 100000;
//        value |= SQUARE_VALUE[move];
      }
    }
    Arrays.sort(output, 0, nMoves);
    return nMoves;
  }
  public int nEndgames = 0;
  public long nVisitedEndgames = 0;
  private EndgameTimeEstimator estimator = new EndgameTimeEstimator();
  double constant = 1;

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
    int nEmpties = Long.bitCount(~(player | opponent));
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

    long newPlayer;
    long newOpponent;
    for (int i = 0; i < nMoves; ++i) {
      Move curMove = moves[i];
      move = curMove.move;
      flip = curMove.flip;
      pass = false;
      newPlayer = opponent & ~flip;
      newOpponent = player | flip;

      int newLower = Math.max(lower, bestEval);
      depthOneEvaluator.update(move, flip);
      if (depth <= 1) {
        currentEval = (depthZeroEval * Constants.WEIGHT_DEPTH_1 - depthOneEvaluator.eval() * Constants.WEIGHT_DEPTH_0) /
            (Constants.WEIGHT_DEPTH_1 + Constants.WEIGHT_DEPTH_0);
      } else if (
          depth >= nEmpties &&
          (nEmpties < Constants.EMPTIES_FOR_ENDGAME + 7) &&
          (nEmpties == Constants.EMPTIES_FOR_ENDGAME + 2 ||
           (-curMove.value < constant))) {
//        System.out.print(StoredBoard.endgameTimeEstimator.disproofNumber(newPlayer, newOpponent, -upper, depthOneEvaluator.eval()));
        lastMovesEvaluator.resetNVisited();
        currentEval = -lastMovesEvaluator.evaluatePosition(
            opponent & ~flip, player | flip, -upper, -newLower, flip);
        nVisited = lastMovesEvaluator.getNVisited();
        constant = constant + 0.01 * (150 - nVisited);
//        System.out.println(constant);
        nVisitedPositions += nVisited;
        nVisitedEndgames += nVisited;
        nEndgames++;
      } else {
        if (depth <= 3 && fast) {
          currentEval = -evaluatePositionQuick(newPlayer, newOpponent, depth - 1, -upper, -newLower, false, move);
        } else {
          if (bestEval == Integer.MIN_VALUE || upper - newLower < 200) {
            currentEval = -evaluatePositionSlow(newPlayer, newOpponent, depth - 1, -upper, -newLower, false, fast);
          } else {
            currentEval = -evaluatePositionSlow(newPlayer, newOpponent, depth - 1, -bestEval-1, -bestEval, false, fast);
            if (currentEval > bestEval && currentEval < upper) {
              currentEval = -evaluatePositionSlow(newPlayer, newOpponent, depth - 1, -upper, -newLower, false, fast);
            }
          }
        }
      }
      depthOneEvaluator.undoUpdate(move, flip);
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
  
  @Override
  public long getNVisited() {
    if (this.nVisitedPositions < 0) {
      throw new RuntimeException("WEIRD, VERY WEIRD!!!");
    }
    return this.nVisitedPositions;
  }
  
//  public void resetHashMapVisitedPositions() {
//    this.hashMapVisitedPositions.reset();
//  }
  /**
   * - Returns value and samples.- Uses hash map.
   * - Removes positions that are < lower or > upper.
   * - V Very fast at small depth.
   * - V Goes up to depth when evaluating.
   * - V At depth 8, uses EvaluatorLastMoves.
   * @param current
   * @param depth
   * @param lower
   * @param upper 
   * @return The evaluation
   */
  public int evaluatePosition(Board current, int depth, int lower, int upper) {
    return evaluatePosition(current.getPlayer(), current.getOpponent(), depth, lower, upper);
  }

  @Override
  public int evaluatePosition(long player, long opponent, int depth, int lower, int upper) {
    depthOneEvaluator.setup(player, opponent);
    this.resetNVisited();
    
    depth = Math.min(depth, 64 - Long.bitCount(player | opponent));
    int empties = Long.bitCount(~(player | opponent));
    if (empties <= Constants.EMPTIES_FOR_ENDGAME && depth >= empties) {
      return evaluatePositionSlow(player, opponent, Constants.EMPTIES_FOR_ENDGAME + 2, lower, upper, false, true);
    } else if (depth == 0) {
      return depthOneEvaluator.eval();
    } else if (depth <= 3) {
      return evaluatePositionQuick(player, opponent, depth,
        lower, upper, false, 64);
    }
//    evaluatePositionSlow(
//        player, opponent, depth - Constants.EMPTIES_FOR_ENDGAME - 1,
//        lower, upper, false, false);
    return evaluatePositionSlow(player, opponent, depth, lower, upper, false, true);
  }

  public void resetNVisited() {
    this.nVisitedPositions = 0;
    this.nComputedMoves = 0;
  }
  
  @Override
  public void resetHashMap() {
    this.hashMap.reset();
  }
}
