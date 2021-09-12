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
package tmp;

import board.Board;
import board.PossibleMovesFinderImproved;

/**
 *
 * @author michele
 */
public class OptimumAlphaBeta {
  static PossibleMovesFinderImproved possibleMovesFinder = 
    new PossibleMovesFinderImproved();
  static long INFINITY = 1000000000000L;

  // Prove b.eval <= eval
  public static long computeOptimumUpper(Board b, int eval) {
    if (b.getEmptySquares() == 0) {
      if ((b.getPlayerDisks() - b.getOpponentDisks()) * 100 > eval) {
        return INFINITY;
      }
      return 0;
    }
    long moves[] = possibleMovesFinder.possibleMovesAdvanced(b.getPlayer(), b.getOpponent());
    long result = 1;

    if (moves.length == 0) {
      if ((b.getPlayerDisks() - b.getOpponentDisks()) * 100 < eval) {
        return INFINITY;
      }
      return 1;
    }

    for (long move : moves) {
      result += computeOptimumLower(b.move(move), -eval);
    }
    return result;
  }

  public static long computeOptimumLower(Board b, int eval) {
    if (b.getEmptySquares() == 0) {
      if ((b.getPlayerDisks() - b.getOpponentDisks()) * 100 < eval) {
        return INFINITY;
      }
      return 0;
    }
    long moves[] = possibleMovesFinder.possibleMovesAdvanced(b.getPlayer(), b.getOpponent());

    if (moves.length == 0) {
      if ((b.getPlayerDisks() - b.getOpponentDisks()) * 100 < eval) {
        return INFINITY;
      }
      return 1;
    }

    long result = INFINITY;
    for (long move : moves) {
      result = Math.min(result, computeOptimumUpper(b.move(move), -eval));
    }
    return result + 1;
  }

  public static long computeOptimumUpperLast(Board b, int eval) {
    if (b.getEmptySquares() == 0) {
      if ((b.getPlayerDisks() - b.getOpponentDisks()) * 100 > eval) {
        return INFINITY;
      }
      return 0;
    }
    long moves[] = possibleMovesFinder.possibleMovesAdvanced(b.getPlayer(), b.getOpponent());
    long result = 1;

    if (moves.length == 0) {
      if ((b.getPlayerDisks() - b.getOpponentDisks()) * 100 > eval) {
        return INFINITY + 1;
      }
      return 1;
    }

    for (long move : moves) {
      result += computeOptimumLowerLast(b.move(move), -eval);
    }
    return result;
  }

  public static long computeOptimumLowerLast(Board b, int eval) {
    if (b.getEmptySquares() == 0) {
      if ((b.getPlayerDisks() - b.getOpponentDisks()) * 100 < eval) {
        return INFINITY + 1;
      }
      return 0;
    }
    long moves[] = possibleMovesFinder.possibleMovesAdvanced(b.getPlayer(), b.getOpponent());

    if (moves.length == 0) {
      if ((b.getPlayerDisks() - b.getOpponentDisks()) * 100 < eval) {
        return INFINITY + 1;
      }
      return 1;
    }

    long result = INFINITY;
    if (b.getEmptySquares() <= 4) {
      long tmp = 0;
      for (long move : moves) {
        tmp = computeOptimumUpperLast(b.move(move), -eval);
        result += tmp % INFINITY;
        if (tmp < INFINITY) {
          result -= INFINITY;
          break;
        }
      }
      return result + 1;
    }
    for (long move : moves) {
      result = Math.min(result, computeOptimumUpperLast(b.move(move), -eval));
    }
    return result + 1;
  }
  
//  public static long[] computeOptimumLowerEval(Board b, int eval) {
//    if (b.getEmptySquares() == 0) {
//      if (b.getPlayerDisks() - b.getOpponentDisks() < eval) {
//        return new long[]{1000000000000L, b.getPlayerDisks() - b.getOpponentDisks()};
//      }
//      return new long[]{0, b.getPlayerDisks() - b.getOpponentDisks()};
//    }
//    long moves[] = possibleMovesFinder.possibleMoves(b);
//
//    if (moves.length == 0) {
//      Board afterPass = b.move(0);
//      if (possibleMovesFinder.possibleMoves(afterPass).length > 0) {
//        long[] result = computeOptimumUpperEval(afterPass, -eval);
//        return new long[]{result[0], -result[1]};
//      }
//      if (b.getPlayerDisks() - b.getOpponentDisks() < eval) {
//        return new long[]{1000000000000L, b.getPlayerDisks() - b.getOpponentDisks()};
//      }
//      return new long[]{1, b.getPlayerDisks() - b.getOpponentDisks()};
//    }
//
//    long bestEval = -66;
//    long nVisited = 0;
//    for (long move : moves) {
//      long[] newResult = computeOptimumUpperEval(b.move(move), -eval);
//      if (-newResult[1] > bestEval) {
//        bestEval = -newResult[1];
//        nVisited = newResult[0];
//      }
//    }
//    return new long[]{nVisited + 1, bestEval};
//  }
//  
//  public static long[] computeOptimumUpperEval(Board b, int eval) {
//    if (b.getEmptySquares() == 0) {
//      if (b.getPlayerDisks() - b.getOpponentDisks() > eval) {
//        return new long[]{1000000000000000L, b.getPlayerDisks() - b.getOpponentDisks()};
//      }
//      return new long[]{0, b.getPlayerDisks() - b.getOpponentDisks()};
//    }
//    long moves[] = possibleMovesFinder.possibleMoves(b);
//
//    if (moves.length == 0) {
//      Board afterPass = b.move(0);
//      if (possibleMovesFinder.possibleMoves(afterPass).length > 0) {
//        long[] result = computeOptimumLowerEval(afterPass, -eval);
//        return new long[]{result[0], -result[1]};
//      }
//      if (b.getPlayerDisks() - b.getOpponentDisks() > eval) {
//        return new long[]{1000000000000L, b.getPlayerDisks() - b.getOpponentDisks()};
//      }
//      return new long[]{0, b.getPlayerDisks() - b.getOpponentDisks()};
//    }
//    long nVisited = 1;
//    long bestEval = -66;
//
//    for (long move : moves) {
//      long[] curResult = computeOptimumLowerEval(b.move(move), -eval);
//      nVisited += curResult[0];
//      bestEval = Math.max(bestEval, -curResult[1]);
//    }
//    return new long[]{nVisited, bestEval};
//  }
}
