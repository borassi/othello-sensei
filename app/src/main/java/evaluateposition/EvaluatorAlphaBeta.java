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
import static constants.Constants.VISITED_ENDGAME_GOAL;
import constants.Stats;
import evaluatedepthone.DepthOneEvaluator;
import evaluatedepthone.DiskDifferenceEvaluatorPlusTwo;
import evaluatedepthone.FindStableDisks;
import evaluatedepthone.PatternEvaluatorImproved;
import helpers.Gaussian;
import java.util.Arrays;

public class EvaluatorAlphaBeta implements EvaluatorInterface {
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
  public static class Constant {
    private double value = VISITED_ENDGAME_GOAL;
    public synchronized double get() {
      return value;
    }
    public synchronized void update(long nVisited) {
      value += 0.01 * (VISITED_ENDGAME_GOAL - nVisited);
    }
  }
  
  final DepthOneEvaluator depthOneEvaluator;
  private long nVisited = 0;
  private final EvaluatorLastMoves lastMovesEvaluator = new EvaluatorLastMoves();
  private final HashMap hashMap;
  final Move[][] moves = new Move[64][64];
  static Constant constant = new Constant();

  public static void resetConstant() {
    constant = new Constant();
  }

  public EvaluatorAlphaBeta() {
    this(new PatternEvaluatorImproved());
  }

  public EvaluatorAlphaBeta(DepthOneEvaluator depthOneEvaluator) {
    this(depthOneEvaluator, new HashMap());
  }

  public EvaluatorAlphaBeta(HashMap hashMap) {
    this(new PatternEvaluatorImproved(), hashMap);
  }

  public EvaluatorAlphaBeta(DepthOneEvaluator depthOneEvaluator, HashMap hashMap) {
    this.depthOneEvaluator = depthOneEvaluator;
//    this.lastMovesEvaluator = new EvaluatorLastMoves();
    for (int i = 0; i < this.moves.length; ++i) {
      for (int j = 0; j < this.moves[i].length; ++j) {
        this.moves[i][j] = new Move();
      }
    }
    this.hashMap = hashMap;
  }
//
//  public double getDisproofNumber(
//      long player, long opponent, int depth, int evalGoal, double bestProofNumber, boolean passed) {
////    assert (evalGoal + 200000) % 200 == 100;
//    long movesBit = GetMoves.getMoves(player, opponent);
//    int move;
//    long flip;
//    boolean pass = true;
//    nComputedMoves++;
//    double result = 0;
//    int currentEval;
//    int depthZeroEval = depthOneEvaluator.eval();
//
//    depthOneEvaluator.invert();
//    while (movesBit != 0) {
//      flip = Long.lowestOneBit(movesBit);
//      move = Long.numberOfTrailingZeros(flip);      
//      movesBit = movesBit & (~flip);
//      flip = GetMoves.getFlip(move, player, opponent);
//
////      nVisitedPositions++;
//      pass = false;
//      depthOneEvaluator.update(move, flip);
//      if (depth > 1) {
//        result += getProofNumber(opponent & ~flip, player | flip, depth - 1, -evalGoal, bestProofNumber, false);
//      } else {
//        currentEval = (depthZeroEval * Constants.WEIGHT_DEPTH_1 - depthOneEvaluator.eval() * Constants.WEIGHT_DEPTH_0) /
//            (Constants.WEIGHT_DEPTH_1 + Constants.WEIGHT_DEPTH_0);
//        result += StoredBoard.endgameTimeEstimator.disproofNumber(player, opponent, evalGoal, currentEval);          
//      }
//      depthOneEvaluator.undoUpdate(move, flip);
//    }
//
//    if (pass) {
//      if (passed) {
//        result = BitPattern.getEvaluationGameOver(player, opponent) > evalGoal ? Double.POSITIVE_INFINITY : 0;
//      } else {
//        result = getProofNumber(opponent, player, depth, -evalGoal, bestProofNumber, true);
//      }
//    }
//    
//    depthOneEvaluator.invert();
//    return result;    
//  }
//
//  public double getProofNumber(
//      long player, long opponent, int depth, int evalGoal, double bestProofNumber, boolean passed) {
//    assert (evalGoal + 200000) % 200 == 100;
//    long movesBit = GetMoves.getMoves(player, opponent);
//    int move;
//    long flip;
//    boolean pass = true;
//    nComputedMoves++;
//    double result = Double.POSITIVE_INFINITY;
//    int currentEval;
//    int depthZeroEval = depthOneEvaluator.eval();
//
//    depthOneEvaluator.invert();
//    while (movesBit != 0) {
//      flip = Long.lowestOneBit(movesBit);
//      move = Long.numberOfTrailingZeros(flip);      
//      movesBit = movesBit & (~flip);
//      flip = GetMoves.getFlip(move, player, opponent);
////      nVisitedPositions++;
//      pass = false;
//      depthOneEvaluator.update(move, flip);
////      depthOneEvaluator.setup(opponent & ~flip, player | flip);
//      if (depth > 1) {
//        result = Math.min(result, getDisproofNumber(opponent & ~flip, player | flip, depth - 1, -evalGoal, Math.min(result, bestProofNumber), false));
//      } else {
//        currentEval = (depthZeroEval * Constants.WEIGHT_DEPTH_1 - depthOneEvaluator.eval() * Constants.WEIGHT_DEPTH_0) /
//            (Constants.WEIGHT_DEPTH_1 + Constants.WEIGHT_DEPTH_0);
//        result = Math.min(result, StoredBoard.endgameTimeEstimator.proofNumber(player, opponent, evalGoal, currentEval));          
//      }
//      depthOneEvaluator.undoUpdate(move, flip);
//    }
//
//    if (pass) {
//      if (passed) {
//        result = BitPattern.getEvaluationGameOver(player, opponent) > evalGoal ? 0 : Double.POSITIVE_INFINITY;
//      } else {
//        result = getDisproofNumber(opponent, player, depth, -evalGoal, Double.POSITIVE_INFINITY, true);
//      }
//    }
//    
//    this.depthOneEvaluator.invert();
//    return result;    
//  }

  public int evaluatePositionQuick(
      long player, long opponent, int depth, int lower, int upper, boolean passed, int lastMove) {
    long movesBit = GetMoves.getMoves(player, opponent);
    int move;
    long flip;
    boolean pass = true;
    int currentEval;
    int bestEval = Integer.MIN_VALUE;

    if (depth <= 1) {
      int depthZeroEval = depthOneEvaluator.eval();
      depthOneEvaluator.invert();
      while (movesBit != 0) {
        flip = Long.lowestOneBit(movesBit);
        move = Long.numberOfTrailingZeros(flip);      
        movesBit = movesBit & (~flip);
        flip = GetMoves.getFlip(move, player, opponent);

        nVisited++;
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

        nVisited++;
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
  
int good = 0;
int bad = 0;
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
      nVisited++;
      Move curMove = output[nMoves++];
      curMove.move = move;
      curMove.flip = flip;
//      if (boardInHash != null) {
//        good++;
//        if (move == boardInHash.bestMove) {
//          curMove.value = 1 << 30;
//        } else if (move == boardInHash.secondBestMove) {
//          curMove.value = 1 << 29;
//        } else {
//          value = (1 + GetMoves.getWeightedNMoves(opponent & ~flip, player | flip)) << 20;     
//          value -= SQUARE_VALUE[move];   
//          curMove.value = -value;
//        }
//      } else 
      if (depth > Constants.EMPTIES_FOR_ENDGAME) {
//        bad++;
        this.depthOneEvaluator.update(move, flip);
        int eval = this.depthOneEvaluator.eval();
        curMove.value = -(int) (
            StoredBoard.endgameTimeEstimator.disproofNumber(opponent & ~flip, player | flip, -upper, eval)
            / (Gaussian.CDF(-upper, eval, 1000)));   
        this.depthOneEvaluator.undoUpdate(move, flip);     
      } else {
        value = (1 + GetMoves.getWeightedNMoves(opponent & ~flip, player | flip)) << 20;     
        value -= SQUARE_VALUE[move];   
        curMove.value = -value;
      }
    }
//    System.out.println(good + " " + bad);
    Arrays.sort(output, 0, nMoves);
    return nMoves;
  }

  final static int EMPTIES_HASH_MAP = 0;
  public int evaluatePositionSlow(
      long player, long opponent, int depth, int lower, int upper, boolean passed, boolean fast) {
    int nEmpties = Long.bitCount(~(player | opponent));
    HashMap.BoardInHash boardInHash = null;
    if (nEmpties > EMPTIES_HASH_MAP) {
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
    }
    int stabilityCutoffUpper = FindStableDisks.getUpperBound(player, opponent);
    if (stabilityCutoffUpper <= lower) {
      return stabilityCutoffUpper;
    }
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
          (nEmpties < Constants.EMPTIES_FOR_ENDGAME + 3) &&
          (nEmpties <= Constants.EMPTIES_FOR_ENDGAME ||
           (-curMove.value < constant.get()))) {
//        currentEval = -lastMovesEvaluator.evaluate(
//            opponent & ~flip, player | flip, -upper, -newLower, flip);
//        nVisited = lastMovesEvaluator.getNVisited();
        EvalWithVisited eval = EvaluatorLastMoves.evaluateCPP(
            opponent & ~flip, player | flip, -upper, -newLower);
//        System.out.println(-eval.eval + " " + currentEval);
//        System.out.println(eval.nVisited + " " + nVisited);
//        if ((currentEval <= newLower && -eval.eval > newLower)
//            || (currentEval >= upper && -eval.eval < upper)) {
//          System.out.println("BIG MISTAKE!!");
//          System.out.println(newLower + " " + -eval.eval + " " + upper);
//          System.out.println(currentEval);
//        }
        currentEval = -eval.eval;
        nVisited = eval.nVisited;
        constant.update(nVisited);
        this.nVisited += nVisited;
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
    if (nEmpties > EMPTIES_HASH_MAP) {
      if (bestEval > lower) {
        hashMap.updateLowerBound(player, opponent, bestEval, depth, bestMove, secondBestMove);
      }
      if (bestEval < upper) {
        hashMap.updateUpperBound(player, opponent, bestEval, depth, bestMove, secondBestMove);
      }
    }
    return bestEval;    
  }
  
  @Override
  public long getNVisited() {
    assert this.nVisited >= 0;
    return this.nVisited;
  }

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
  public int evaluate(Board current, int depth, int lower, int upper) {
    return evaluate(current.getPlayer(), current.getOpponent(), depth, lower, upper);
  }

  @Override
  public int evaluate(long player, long opponent, int depth, int lower, int upper) {
    depthOneEvaluator.setup(player, opponent);
    nVisited = 0;
    int result;
    
    depth = Math.min(depth, 64 - Long.bitCount(player | opponent));
    int empties = Long.bitCount(~(player | opponent));
    if (empties <= Constants.EMPTIES_FOR_ENDGAME && depth >= empties) {
      result = evaluatePositionSlow(player, opponent, Constants.EMPTIES_FOR_ENDGAME + 2, lower, upper, false, true);
    } else if (depth == 0) {
      result = depthOneEvaluator.eval();
    } else if (depth <= 3) {
      result = evaluatePositionQuick(player, opponent, depth, lower, upper, false, 64);
    } else {
      result = evaluatePositionSlow(player, opponent, depth, lower, upper, false, true);
    }
    if (depth >= empties) {
      Stats.addToNVisitedAlphaBetaSolve(nVisited);
      Stats.addToNAlphaBetaSolve(1);      
    }
//    evaluatePositionSlow(
//        player, opponent, depth - Constants.EMPTIES_FOR_ENDGAME - 3,
//        lower, upper, false, false);
    return result;
  }
}
