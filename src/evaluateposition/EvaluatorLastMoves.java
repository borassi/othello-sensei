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
import constants.Constants;
import evaluatedepthone.FindStableDisks;
import java.lang.management.ManagementFactory;
import java.lang.management.ThreadMXBean;
import java.text.DecimalFormat;

/**
 *
 * @author michele
 */
public class EvaluatorLastMoves {
  private final FindStableDisks stableDisks;

  int nVisited = 0;
  int baseOffset;
  
  long tmp[] = new long[0];
  long masksTmp[];
  long firstLastInEdges[];

  private final GetFlip flipper;
  
  public EvaluatorLastMoves(GetFlip flipper, FindStableDisks stableDisks) {
    this.flipper = flipper;
    this.stableDisks = stableDisks;
    setup();
  }
  public EvaluatorLastMoves() {
    this(GetFlip.load(), new FindStableDisks());
  }
  
  private void setup() {
    this.masksTmp = new long[] {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    masksTmp[3] = CORNERS;
    masksTmp[6] = CENTRAL;
    masksTmp[7] = CENTRAL1;
    masksTmp[8] = CENTRAL2;
    masksTmp[9] = ~0L;
    this.firstLastInEdges = new long[2];
  }
  
  // EVALUATE.
  private static int evalOneEmpty(int x, long player, long opponent) {
    long flip = 0;
    flip = GetMoves.getFlip(x, player, opponent);
    if (flip != 0) {
      return 200 * Long.bitCount(player | flip) - 6400;
    }

    flip = GetMoves.getFlip(x, opponent, player);
    if (flip != 0) {
      return 6400 - 200 * Long.bitCount(opponent | flip);
    }
    int playerDisks = 200 * Long.bitCount(player);
    return playerDisks + (playerDisks >= 6400 ? 200 : 0) - 6400;
  }
  
  private int evalTwoEmptiesPassed(int x1, int x2, long player, long opponent, int lower, int upper) {
    int best = -6600;
    long flip = GetMoves.getFlip(x1, player, opponent);
    if (flip != 0) {
      nVisited++;
      best = -evalOneEmpty(x2, opponent & ~flip, player | flip);
      if (best >= upper) {
        return best;
      }
    }
    flip = GetMoves.getFlip(x2, player, opponent);
    if (flip != 0) {
      nVisited++;
      return Math.max(best, -evalOneEmpty(x1, opponent & ~flip, player | flip));
    }

    if (best > -6600) {
      return best;
    }
    return BitPattern.getEvaluationGameOver(player, opponent);
  }
  
  private int evalTwoEmpties(int x1, int x2, long player, long opponent, int lower, int upper) {
    int best = -6600;
    long flip = GetMoves.getFlip(x1, player, opponent);
    if (flip != 0) {
      nVisited++;
      best = -evalOneEmpty(x2, opponent & ~flip, player | flip);
      if (best >= upper) {
        return best;
      }
    }
    flip = GetMoves.getFlip(x2, player, opponent);
    if (flip != 0) {
      nVisited++;
      return Math.max(best, -evalOneEmpty(x1, opponent & ~flip, player | flip));
    }

    if (best > -6600) {
      return best;
    }
    return -evalTwoEmptiesPassed(x1, x2, opponent, player, -upper, -lower);
  }
  
//  private int evalTwoEmpties(long player, long opponent, int lower, int upper) {
//    long empties = ~(player | opponent);
//    int x1 = Long.numberOfTrailingZeros(empties);
//    int x2 = 63 - Long.numberOfLeadingZeros(empties);
//    return evalTwoEmpties(x1, x2, player, opponent, lower, upper);
//  }
//  
  private int evalThreeEmptiesPassed(int x1, int x2, int x3, long player, long opponent, int lower, int upper) {
    int best = -6600;
    int cur;
    long flip = GetMoves.getFlip(x1, player, opponent);
    if (flip != 0) {
      nVisited++;
      best = -evalTwoEmpties(x2, x3, opponent & ~flip, player | flip, -upper, -lower);
      if (best >= upper) {
        return best;
      }
    }
    flip = GetMoves.getFlip(x2, player, opponent);
    if (flip != 0) {
      nVisited++;
      cur = -evalTwoEmpties(x1, x3, opponent & ~flip, player | flip, -upper, -lower);
      if (cur >= upper) {
        return cur;
      }
      best = Math.max(best, cur);
    }
    flip = GetMoves.getFlip(x3, player, opponent);
    if (flip != 0) {
      nVisited++;
      return Math.max(best, -evalTwoEmpties(x1, x2, opponent & ~flip, player | flip, -upper, -lower));
    }

    if (best > -6600) {
      return best;
    }
    return BitPattern.getEvaluationGameOver(player, opponent);
  }
  
  
  private int evalThreeEmpties(
      int x1, int x2, int x3, long player, long opponent, int lower, int upper,
      int scLastSamePlayerDelta, boolean lastMoveCorner) {
    
    if (scLastSamePlayerDelta <= Constants.TRY_SC_LAST_3_NO_CORNER ||
        (lastMoveCorner && scLastSamePlayerDelta <= Constants.TRY_SC_LAST_3_CORNER)) {
      int scUpper = stableDisks.getUpperBound(player, opponent);
      if (scUpper <= lower) {
        return scUpper;
      }
    }
    int best = -6600;
    long flip = GetMoves.getFlip(x1, player, opponent);
    if (flip != 0) {
      nVisited++;
      best = -evalTwoEmpties(x2, x3, opponent & ~flip, player | flip, -upper, -lower);
      if (best >= upper) {
        return best;
      }
    }
    flip = GetMoves.getFlip(x2, player, opponent);
    if (flip != 0) {
      nVisited++;
      int cur = -evalTwoEmpties(x1, x3, opponent & ~flip, player | flip, -upper, -lower);
      if (cur >= upper) {
        return cur;
      }
      best = Math.max(best, cur);
    }
    flip = GetMoves.getFlip(x3, player, opponent);
    if (flip != 0) {
      nVisited++;
      return Math.max(best, -evalTwoEmpties(x1, x2, opponent & ~flip, player | flip, -upper, -lower));
    }

    if (best > -6600) {
      return best;
    }
    return -evalThreeEmptiesPassed(x1, x2, x3, opponent, player, -upper, -lower);
  }
  
  
  private int evalFourEmptiesPassed(
      int x1, int x2, int x3, int x4, long player, long opponent, int lower,
      int upper, int scLastSamePlayerDelta) {
    int best = -6600;
    long flip = GetMoves.getFlip(x1, player, opponent);
    if (flip != 0) {
      nVisited++;
      best = -evalThreeEmpties(x2, x3, x4, opponent & ~flip, player | flip, -upper, -lower, scLastSamePlayerDelta, (flip & CORNERS) != 0);
      if (best >= upper) {
        return best;
      }
    }
    flip = GetMoves.getFlip(x2, player, opponent);
    if (flip != 0) {
      nVisited++;
      int cur = -evalThreeEmpties(x1, x3, x4, opponent & ~flip, player | flip, -upper, -lower, scLastSamePlayerDelta, (flip & CORNERS) != 0);
      if (cur >= upper) {
        return cur;
      }
      best = Math.max(best, cur);
    }
    flip = GetMoves.getFlip(x3, player, opponent);
    if (flip != 0) {
      nVisited++;
      int cur = -evalThreeEmpties(x1, x2, x4, opponent & ~flip, player | flip, -upper, -lower, scLastSamePlayerDelta, (flip & CORNERS) != 0);
      if (cur >= upper) {
        return cur;
      }
      best = Math.max(best, cur);
    }
    flip = GetMoves.getFlip(x4, player, opponent);
    if (flip != 0) {
      nVisited++;
      return Math.max(best, -evalThreeEmpties(x1, x2, x3, opponent & ~flip, player | flip, -upper, -lower, scLastSamePlayerDelta, (flip & CORNERS) != 0));
    }

    if (best > -6600) {
      return best;
    }
    return BitPattern.getEvaluationGameOver(player, opponent);
  }
  
  
  private int evalFourEmptiesSwapLast2(
      int x1, int x2, int x3, int x4, long player, long opponent, int lower,
      int upper, int scLastDiffPlayerDelta, int scLastSamePlayerDelta,
      boolean lastMoveCorner) {
    int scCurDelta;
    if (scLastSamePlayerDelta <= Constants.TRY_SC_LAST_4_NO_CORNER ||
        (lastMoveCorner && scLastSamePlayerDelta <= Constants.TRY_SC_LAST_4_CORNER)) {
      int scUpper = stableDisks.getUpperBound(player, opponent);
      scCurDelta = scUpper - lower;
      if (scCurDelta <= 0) {
        return scUpper;
      }
    } else {
      scCurDelta = 12800;
    }
    int best = -6600;
    long flip = GetMoves.getFlip(x1, player, opponent);
    if (flip != 0) {
      nVisited++;
      best = -evalThreeEmpties(x2, x3, x4, opponent & ~flip, player | flip, -upper, -lower, scLastDiffPlayerDelta, (flip & CORNERS) != 0);
      if (best >= upper) {
        return best;
      }
    }
    flip = GetMoves.getFlip(x2, player, opponent);
    if (flip != 0) {
      nVisited++;
      int cur = -evalThreeEmpties(x1, x3, x4, opponent & ~flip, player | flip, -upper, -lower, scLastDiffPlayerDelta, (flip & CORNERS) != 0);
      if (cur >= upper) {
        return cur;
      }
      best = Math.max(best, cur);
    }
    flip = GetMoves.getFlip(x3, player, opponent);
    if (flip != 0) {
      nVisited++;
      int cur = -evalThreeEmpties(x4, x1, x2, opponent & ~flip, player | flip, -upper, -lower, scLastDiffPlayerDelta, (flip & CORNERS) != 0);
      if (cur >= upper) {
        return cur;
      }
      best = Math.max(best, cur);
    }
    flip = GetMoves.getFlip(x4, player, opponent);
    if (flip != 0) {
      nVisited++;
      return Math.max(best, -evalThreeEmpties(x3, x1, x2, opponent & ~flip, player | flip, -upper, -lower, scLastDiffPlayerDelta, (flip & CORNERS) != 0));
    }

    if (best > -6600) {
      return best;
    }
    return -evalFourEmptiesPassed(x1, x2, x3, x4, opponent, player, -upper, -lower, scCurDelta);
  }
  
  
  private int evalFourEmpties(
      int x1, int x2, int x3, int x4, long player, long opponent, int lower,
      int upper, int scLastDiffPlayerDelta, int scLastSamePlayerDelta,
      boolean lastMoveCorner) {
    int scCurDelta;
    if (scLastSamePlayerDelta <= Constants.TRY_SC_LAST_4_NO_CORNER ||
        (lastMoveCorner && scLastSamePlayerDelta <= Constants.TRY_SC_LAST_4_CORNER)) {
      int scUpper = stableDisks.getUpperBound(player, opponent);
      scCurDelta = scUpper - lower;
      if (scCurDelta <= 0) {
        return scUpper;
      }
    } else {
      scCurDelta = 12800;
    }
    int best = -6600;
    long flip = GetMoves.getFlip(x1, player, opponent);
    if (flip != 0) {
      nVisited++;
      best = -evalThreeEmpties(x2, x3, x4, opponent & ~flip, player | flip, -upper, -lower, scLastDiffPlayerDelta, (flip & CORNERS) != 0);
      if (best >= upper) {
        return best;
      }
    }
    flip = GetMoves.getFlip(x2, player, opponent);
    if (flip != 0) {
      nVisited++;
      int cur = -evalThreeEmpties(x1, x3, x4, opponent & ~flip, player | flip, -upper, -lower, scLastDiffPlayerDelta, (flip & CORNERS) != 0);
      if (cur >= upper) {
        return cur;
      }
      best = Math.max(best, cur);
    }
    flip = GetMoves.getFlip(x3, player, opponent);
    if (flip != 0) {
      nVisited++;
      int cur = -evalThreeEmpties(x1, x2, x4, opponent & ~flip, player | flip, -upper, -lower, scLastDiffPlayerDelta, (flip & CORNERS) != 0);
      if (cur >= upper) {
        return cur;
      }
      best = Math.max(best, cur);
    }
    flip = GetMoves.getFlip(x4, player, opponent);
    if (flip != 0) {
      nVisited++;
      return Math.max(best, -evalThreeEmpties(x1, x2, x3, opponent & ~flip, player | flip, -upper, -lower, scLastDiffPlayerDelta, (flip & CORNERS) != 0));
    }

    if (best > -6600) {
      return best;
    }
    return -evalFourEmptiesPassed(x1, x2, x3, x4, opponent, player, -upper, -lower, scCurDelta);
  }
  
  private int evalFiveEmptiesPassed(
      int x1, int x2, int x3, int x4, int x5, long player, long opponent,
      int lower, int upper, int scLastDiffPlayerDelta, int scLastSamePlayerDelta) {
    int best = -6600;
    long flip = GetMoves.getFlip(x1, player, opponent);
    if (flip != 0) {
      nVisited++;
      best = -evalFourEmpties(
          x2, x3, x4, x5, opponent & ~flip, player | flip, -upper, -lower,
          scLastSamePlayerDelta, scLastDiffPlayerDelta, (flip & CORNERS) != 0);
      if (best >= upper) {
        return best;
      }
    }
    flip = GetMoves.getFlip(x2, player, opponent);
    if (flip != 0) {
      nVisited++;
      int cur = -evalFourEmpties(
          x1, x3, x4, x5, opponent & ~flip, player | flip, -upper, -lower,
          scLastSamePlayerDelta, scLastDiffPlayerDelta, (flip & CORNERS) != 0);
      if (cur >= upper) {
        return cur;
      }
      best = Math.max(best, cur);
    }
    flip = GetMoves.getFlip(x3, player, opponent);
    if (flip != 0) {
      nVisited++;
      int cur = -evalFourEmpties(
          x1, x2, x4, x5, opponent & ~flip, player | flip, -upper, -lower,
          scLastSamePlayerDelta, scLastDiffPlayerDelta, (flip & CORNERS) != 0);
      if (cur >= upper) {
        return cur;
      }
      best = Math.max(best, cur);
    }
    flip = GetMoves.getFlip(x4, player, opponent);
    if (flip != 0) {
      nVisited++;
      int cur = -evalFourEmpties(
          x1, x2, x3, x5, opponent & ~flip, player | flip, -upper, -lower,
          scLastSamePlayerDelta, scLastDiffPlayerDelta, (flip & CORNERS) != 0);
      if (cur >= upper) {
        return cur;
      }
      best = Math.max(best, cur);
    }
    flip = GetMoves.getFlip(x5, player, opponent);
    if (flip != 0) {
      nVisited++;
      return Math.max(best, -evalFourEmpties(
          x1, x2, x3, x4, opponent & ~flip, player | flip, -upper, -lower,
          scLastSamePlayerDelta, scLastDiffPlayerDelta, (flip & CORNERS) != 0));
    }

    if (best > -6600) {
      return best;
    }
    return BitPattern.getEvaluationGameOver(player, opponent);
  }
    
  private int evalFiveEmptiesSwapLast2(
      int x1, int x2, int x3, int x4, int x5, long player,
      long opponent, int lower, int upper,
      int scLastDiffPlayerDelta, int scLastSamePlayerDelta) {
    assert(SQUARE_VALUE[x1] >= SQUARE_VALUE[x2]);
    assert(SQUARE_VALUE[x2] >= SQUARE_VALUE[x3]);
    assert(SQUARE_VALUE[x3] >= SQUARE_VALUE[x4]);
    assert(SQUARE_VALUE[x4] >= SQUARE_VALUE[x5]);
    assert(x1 != x2 && x1 != x3 && x1 != x4 && x1 != x5);
    assert(x2 != x3 && x2 != x4 && x2 != x5);
    assert(x3 != x4 && x3 != x5);
    assert(x4 != x5);
    
    int best = -6600;
    long flip = GetMoves.getFlip(x1, player, opponent);
    if (flip != 0) {
      nVisited++;
      best = -evalFourEmptiesSwapLast2(
          x2, x3, x4, x5, opponent & ~flip, player | flip, -upper, -lower,
          scLastSamePlayerDelta, scLastDiffPlayerDelta, (flip & CORNERS) != 0);
      if (best >= upper) {
        return best;
      }
    }
    flip = GetMoves.getFlip(x2, player, opponent);
    if (flip != 0) {
      nVisited++;
      int cur = -evalFourEmptiesSwapLast2(
          x1, x3, x4, x5, opponent & ~flip, player | flip, -upper, -lower,
          scLastSamePlayerDelta, scLastDiffPlayerDelta, (flip & CORNERS) != 0);
      if (cur >= upper) {
        return cur;
      }
      best = Math.max(best, cur);
    }
    flip = GetMoves.getFlip(x3, player, opponent);
    if (flip != 0) {
      nVisited++;
      int cur = -evalFourEmptiesSwapLast2(
          x1, x2, x4, x5, opponent & ~flip, player | flip, -upper, -lower,
          scLastSamePlayerDelta, scLastDiffPlayerDelta, (flip & CORNERS) != 0);
      if (cur >= upper) {
        return cur;
      }
      best = Math.max(best, cur);
    }
    flip = GetMoves.getFlip(x4, player, opponent);
    if (flip != 0) {
      nVisited++;
      int cur = -evalFourEmptiesSwapLast2(
          x5, x1, x2, x3, opponent & ~flip, player | flip, -upper, -lower,
          scLastSamePlayerDelta, scLastDiffPlayerDelta, (flip & CORNERS) != 0);
      if (cur >= upper) {
        return cur;
      }
      best = Math.max(best, cur);
    }
    flip = GetMoves.getFlip(x5, player, opponent);
    if (flip != 0) {
      nVisited++;
      return Math.max(best, -evalFourEmptiesSwapLast2(
          x4, x1, x2, x3, opponent & ~flip, player | flip, -upper, -lower,
          scLastSamePlayerDelta, scLastDiffPlayerDelta, (flip & CORNERS) != 0));
    }

    if (best > -6600) {
      return best;
    }
    return -evalFiveEmptiesPassed(x1, x2, x3, x4, x5, opponent, player, -upper, -lower, scLastSamePlayerDelta, scLastDiffPlayerDelta);
  }
    
  private int evalFiveEmpties(
      int x1, int x2, int x3, int x4, int x5, long player,
      long opponent, int lower, int upper,
      int scLastDiffPlayerDelta, int scLastSamePlayerDelta) {
    assert(SQUARE_VALUE[x1] >= SQUARE_VALUE[x2]);
    assert(SQUARE_VALUE[x2] >= SQUARE_VALUE[x3]);
    assert(SQUARE_VALUE[x3] >= SQUARE_VALUE[x4]);
    assert(SQUARE_VALUE[x4] >= SQUARE_VALUE[x5]);
    assert(x1 != x2 && x1 != x3 && x1 != x4 && x1 != x5);
    assert(x2 != x3 && x2 != x4 && x2 != x5);
    assert(x3 != x4 && x3 != x5);
    assert(x4 != x5);
    
    int best = -6600;
    long flip = GetMoves.getFlip(x1, player, opponent);
    if (flip != 0) {
      nVisited++;
      best = -evalFourEmpties(
          x2, x3, x4, x5, opponent & ~flip, player | flip, -upper, -lower,
          scLastSamePlayerDelta, scLastDiffPlayerDelta, (flip & CORNERS) != 0);
      if (best >= upper) {
        return best;
      }
    }
    flip = GetMoves.getFlip(x2, player, opponent);
    if (flip != 0) {
      nVisited++;
      int cur = -evalFourEmpties(
          x1, x3, x4, x5, opponent & ~flip, player | flip, -upper, -lower,
          scLastSamePlayerDelta, scLastDiffPlayerDelta, (flip & CORNERS) != 0);
      if (cur >= upper) {
        return cur;
      }
      best = Math.max(best, cur);
    }
    flip = GetMoves.getFlip(x3, player, opponent);
    if (flip != 0) {
      nVisited++;
      int cur = -evalFourEmpties(
          x1, x2, x4, x5, opponent & ~flip, player | flip, -upper, -lower,
          scLastSamePlayerDelta, scLastDiffPlayerDelta, (flip & CORNERS) != 0);
      if (cur >= upper) {
        return cur;
      }
      best = Math.max(best, cur);
    }
    flip = GetMoves.getFlip(x4, player, opponent);
    if (flip != 0) {
      nVisited++;
      int cur = -evalFourEmpties(
          x1, x2, x3, x5, opponent & ~flip, player | flip, -upper, -lower,
          scLastSamePlayerDelta, scLastDiffPlayerDelta, (flip & CORNERS) != 0);
      if (cur >= upper) {
        return cur;
      }
      best = Math.max(best, cur);
    }
    flip = GetMoves.getFlip(x5, player, opponent);
    if (flip != 0) {
      nVisited++;
      return Math.max(best, -evalFourEmpties(
          x1, x2, x3, x4, opponent & ~flip, player | flip, -upper, -lower,
          scLastSamePlayerDelta, scLastDiffPlayerDelta, (flip & CORNERS) != 0));
    }

    if (best > -6600) {
      return best;
    }
    return -evalFiveEmptiesPassed(x1, x2, x3, x4, x5, opponent, player, -upper, -lower, scLastSamePlayerDelta, scLastDiffPlayerDelta);
  }
  
  final static int SPACE[] = {
    0, 0, 0, 0, 1, 1, 1, 1,
    0, 0, 0, 0, 1, 1, 1, 1,
    0, 0, 0, 0, 1, 1, 1, 1,
    0, 0, 0, 0, 1, 1, 1, 1,
    2, 2, 2, 2, 3, 3, 3, 3,
    2, 2, 2, 2, 3, 3, 3, 3,
    2, 2, 2, 2, 3, 3, 3, 3,
    2, 2, 2, 2, 3, 3, 3, 3,
  };
  
  final static int SQUARE_VALUE[] = {
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
  
  private static int evalEmpty(int x, int spaceSize) {
    return ((spaceSize & 1) << 24) | ((6 - spaceSize) << 16) | (SPACE[x] << 8) | SQUARE_VALUE[x];
  }
  
  private int evalFiveEmpties(
      long player, long opponent, int lower, int upper,
      int scLastDiffPlayerDelta, int scLastSamePlayerDelta,
      boolean lastMoveCorner) {

    int scCurDelta;
    if (scLastSamePlayerDelta <= Constants.TRY_SC_LAST_5_NO_CORNER ||
        (lastMoveCorner && scLastSamePlayerDelta <= Constants.TRY_SC_LAST_5_CORNER)) {
      int scUpper = stableDisks.getUpperBound(player, opponent);
      scCurDelta = scUpper - lower;
      if (scCurDelta <= 0) {
        return scUpper;
      }
    } else {
      scCurDelta = 12800;
    }
    long empties = ~(player | opponent);
    int spaceSizes[] = {Long.bitCount(empties & BOTTOMRIGHT),
                        Long.bitCount(empties & BOTTOMLEFT),
                        Long.bitCount(empties & TOPRIGHT),
                        Long.bitCount(empties & TOPLEFT)};
    int x1, x2, x3, x4, x5, xSwap;
    x1 = Long.numberOfTrailingZeros(empties);
    empties = empties & ~(1L << x1);

    x2 = Long.numberOfTrailingZeros(empties);
    empties = empties & ~(1L << x2);

    x3 = Long.numberOfTrailingZeros(empties);
    empties = empties & ~(1L << x3);

    x4 = Long.numberOfTrailingZeros(empties);
    empties = empties & ~(1L << x4);

    x5 = Long.numberOfTrailingZeros(empties);
    int spaceSizeX1 = spaceSizes[SPACE[x1]];
    int spaceSizeX2 = spaceSizes[SPACE[x2]];
    int spaceSizeX3 = spaceSizes[SPACE[x3]];
    int spaceSizeX4 = spaceSizes[SPACE[x4]];
    int spaceSizeX5 = spaceSizes[SPACE[x5]];
    int valueX1 = evalEmpty(x1, spaceSizeX1);
    int valueX2 = evalEmpty(x2, spaceSizeX2);
    int valueX3 = evalEmpty(x3, spaceSizeX3);
    int valueX4 = evalEmpty(x4, spaceSizeX4);
    int valueX5 = evalEmpty(x5, spaceSizeX5);
//    System.out.println(BitPattern.patternToString(empties));
//    System.out.println(spaceSizes[0] + " " + spaceSizes[1] + " " + spaceSizes[2] + " " + spaceSizes[3]);
//    System.out.println(spaceSizeX1 + " " + spaceSizeX2 + " " + spaceSizeX3 + " " + spaceSizeX4 + " " + spaceSizeX5);
    
    if (valueX1 < valueX2) {
      xSwap = x1;
      x1 = x2;
      x2 = xSwap;
      xSwap = valueX1;
      valueX1 = valueX2;
      valueX2 = xSwap;
    }
    // x1 >= x2.
    if (valueX3 < valueX4) {
      xSwap = x3;
      x3 = x4;
      x4 = xSwap;
      xSwap = valueX3;
      valueX3 = valueX4;
      valueX4 = xSwap;
    }
    // x1 >= x2, x3 >= x4.
    if (valueX1 < valueX3) {
      xSwap = x1;
      x1 = x3;
      x3 = xSwap;
      xSwap = valueX1;
      valueX1 = valueX3;
      valueX3 = xSwap;
      xSwap = x2;
      x2 = x4;
      x4 = xSwap;
      xSwap = valueX2;
      valueX2 = valueX4;
      valueX4 = xSwap;
    }
    // x1 >= x2, x1 >= x3 >= x4
    if (valueX5 <= valueX3) {
      if (valueX5 > valueX4) {
        xSwap = x5;
        x5 = x4;
        x4 = xSwap;
        xSwap = valueX5;
        valueX5 = valueX4;
        valueX4 = xSwap;
      }
      // x1 >= x2, x1 >= x3 >= x4 >= x5
      if (valueX2 < valueX4) {
        if (valueX2 < valueX5) {
          if (spaceSizeX1 == 2 || spaceSizeX2 == 2 || spaceSizeX3 == 2 || spaceSizeX4 == 2) {
            return evalFiveEmptiesSwapLast2(x1, x3, x4, x5, x2, player, opponent, lower, upper, scLastDiffPlayerDelta, scCurDelta);
          } else {
            return evalFiveEmpties(x1, x3, x4, x5, x2, player, opponent, lower, upper, scLastDiffPlayerDelta, scCurDelta);            
          }
        } else {
          if (spaceSizeX1 == 2 || spaceSizeX2 == 2 || spaceSizeX3 == 2 || spaceSizeX4 == 2) {
            return evalFiveEmptiesSwapLast2(x1, x3, x4, x2, x5, player, opponent, lower, upper, scLastDiffPlayerDelta, scCurDelta);   
          } else {
            return evalFiveEmpties(x1, x3, x4, x2, x5, player, opponent, lower, upper, scLastDiffPlayerDelta, scCurDelta);             
          }       
        }
      } else {
        if (valueX2 < valueX3) {
          if (spaceSizeX1 == 2 || spaceSizeX2 == 2 || spaceSizeX3 == 2 || spaceSizeX4 == 2) {
            return evalFiveEmptiesSwapLast2(x1, x3, x2, x4, x5, player, opponent, lower, upper, scLastDiffPlayerDelta, scCurDelta);
          } else {
            return evalFiveEmpties(x1, x3, x2, x4, x5, player, opponent, lower, upper, scLastDiffPlayerDelta, scCurDelta);
          }
        } else {
          if (spaceSizeX1 == 2 || spaceSizeX2 == 2 || spaceSizeX3 == 2 || spaceSizeX4 == 2) {
            return evalFiveEmptiesSwapLast2(x1, x2, x3, x4, x5, player, opponent, lower, upper, scLastDiffPlayerDelta, scCurDelta);   
          } else {
            return evalFiveEmpties(x1, x2, x3, x4, x5, player, opponent, lower, upper, scLastDiffPlayerDelta, scCurDelta);
          }   
        }        
      }
    } else {
      // x1 >= x2, x1 >= x3 >= x4, x5 >= x3
      if (valueX5 > valueX1) {
        xSwap = x5;
        x5 = x1;
        x1 = xSwap;
//        xSwap = valueX5;
        valueX5 = valueX1;
//        valueX1 = xSwap;
      }
      // x1 >= x2, x1 >= x5 >= x3 >= x4
      if (valueX2 < valueX3) {
        if (valueX2 < valueX4) {
          if (spaceSizeX1 == 2 || spaceSizeX2 == 2 || spaceSizeX3 == 2 || spaceSizeX4 == 2) {
            return evalFiveEmptiesSwapLast2(x1, x5, x3, x4, x2, player, opponent, lower, upper, scLastDiffPlayerDelta, scCurDelta);
          } else {
            return evalFiveEmpties(x1, x5, x3, x4, x2, player, opponent, lower, upper, scLastDiffPlayerDelta, scCurDelta);            
          }
        } else {
          if (spaceSizeX1 == 2 || spaceSizeX2 == 2 || spaceSizeX3 == 2 || spaceSizeX4 == 2) {
            return evalFiveEmptiesSwapLast2(x1, x5, x3, x2, x4, player, opponent, lower, upper, scLastDiffPlayerDelta, scCurDelta);
          } else {
            return evalFiveEmpties(x1, x5, x3, x2, x4, player, opponent, lower, upper, scLastDiffPlayerDelta, scCurDelta);            
          }
        }
      } else {
        if (valueX2 < valueX5) {
          if (spaceSizeX1 == 2 || spaceSizeX2 == 2 || spaceSizeX3 == 2 || spaceSizeX4 == 2) {
            return evalFiveEmptiesSwapLast2(x1, x5, x2, x3, x4, player, opponent, lower, upper, scLastDiffPlayerDelta, scCurDelta);
          } else {
            return evalFiveEmpties(x1, x5, x2, x3, x4, player, opponent, lower, upper, scLastDiffPlayerDelta, scCurDelta);            
          }
        } else {
          if (spaceSizeX1 == 2 || spaceSizeX2 == 2 || spaceSizeX3 == 2 || spaceSizeX4 == 2) {
            return evalFiveEmptiesSwapLast2(x1, x2, x5, x3, x4, player, opponent, lower, upper, scLastDiffPlayerDelta, scCurDelta);
          } else {
            return evalFiveEmpties(x1, x2, x5, x3, x4, player, opponent, lower, upper, scLastDiffPlayerDelta, scCurDelta);            
          }      
        }        
      }
    }
  }
//  private int evaluateSuperFast(
//      long player, long opponent, int lower, int upper,
//      boolean passed, int lastSCFail, int last2SCFail) {
//    long empties = ~(player | opponent);
//    int nEmpties = Long.bitCount(empties);
//    int curSCFail;
//    if (nEmpties > 6 || last2SCFail > 2000) {
//      int scUpper = stableDisks.getUpperBound(player, opponent);
//      if (scUpper <= lower) {
//        return scUpper;
//      }
//      curSCFail = scUpper - lower;
////      if (nEmpties == 6) System.out.println(last2SCFail + " Fail " + curSCFail);
//    } else {
//      curSCFail = 12800;
//    }
//    boolean pass = true;
//    int best = Integer.MIN_VALUE;
//    int move;
//    boolean superFast = nEmpties == 6;
//    empties = empties & GetFlip.neighbors(opponent);
//    while (empties != 0) {
//      move = Long.numberOfTrailingZeros(empties);      
//      empties = empties & (~(1L << move));
//      long flip = GetMoves.getFlip(move, player, opponent);
//      if (flip == 0) {
//        continue;
//      }
//      nVisited++;
//      if (superFast) {
////        int x = Long.numberOfTrailingZeros(~(player | opponent | flip));
////        best = Math.max(best, -evalOneEmpty(x, opponent & ~flip, player | flip));
//
//        best = Math.max(best, -evalFiveEmpties(opponent & ~flip, player | flip, -upper, -Math.max(lower, best)));
//      } else {
//        best = Math.max(best, 
//          -evaluateSuperFast(
//              opponent & ~flip, player | flip, -upper, 
//              -Math.max(lower, best), false, curSCFail, lastSCFail));
//      }
//      pass = false;
//      if (best >= upper) {
//        return best;
//      }
//    }
//    
//    if (pass) {
//      if (passed) {
//        return BitPattern.getEvaluationGameOver(player, opponent);
//      }
//      return -evaluateSuperFast(opponent, player, -upper, -lower, true, curSCFail, lastSCFail);
//    }
//    return best;
//  }
  final static long TOPLEFT = BitPattern.parsePattern(
      "XXXX----" +
      "XXXX----" +
      "XXXX----" +
      "XXXX----" +
      "--------" +
      "--------" +
      "--------" +
      "--------");
  final static long TOPRIGHT = TOPLEFT >>> 4;
  final static long BOTTOMLEFT = TOPLEFT >>> 32;
  final static long BOTTOMRIGHT = TOPRIGHT >>> 32;
  
  final static long XSQUARES = BitPattern.parsePattern(
      "--------" +
      "-X----X-" +
      "--------" +
      "--------" +
      "--------" +
      "--------" +
      "-X----X-" +
      "--------");
  final static long CORNERS = BitPattern.parsePattern(
      "X------X" +
      "--------" +
      "--------" +
      "--------" +
      "--------" +
      "--------" +
      "--------" +
      "X------X");
  
  final static long HORIZVERT = BitPattern.parsePattern(
      "--------" +
      "--------" +
      "--------" +
      "--------" +
      "--------" +
      "------X-" +
      "-----X-X" +
      "------X-");
  
  final static long CENTRAL1 = BitPattern.parsePattern(
      "--------" +
      "--XXXX--" +
      "-XXXXXX-" +
      "-XXXXXX-" +
      "-XXXXXX-" +
      "-XXXXXX-" +
      "--XXXX--" +
      "--------");
  
  final static long CENTRAL = BitPattern.parsePattern(
      "--XXXX--" +
      "--XXXX--" +
      "XXXXXXXX" +
      "XXXXXXXX" +
      "XXXXXXXX" +
      "XXXXXXXX" +
      "--XXXX--" +
      "--XXXX--");
  
  final static long CENTRAL2 = BitPattern.parsePattern(
      "-XXXXXX-" +
      "X-XXXX-X" +
      "XXXXXXXX" +
      "XXXXXXXX" +
      "XXXXXXXX" +
      "XXXXXXXX" +
      "X-XXXX-X" +
      "-XXXXXX-");

  final static long edges[] = new long[] {BitPattern.FIRST_COLUMN_BIT_PATTERN,
                                          BitPattern.LAST_COLUMN_BIT_PATTERN,
                                          BitPattern.FIRST_ROW_BIT_PATTERN,
                                          BitPattern.LAST_ROW_BIT_PATTERN};
  
  private long[] firstLastInEdges(long bitPattern) {
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

  private int evaluateFast(
      long player, long opponent, int lower, int upper, boolean passed,
      long lastMove, int scLastDiffPlayerDelta, int scLastSamePlayerDelta) {
    long empties = ~(player | opponent);
    int nEmpties = Long.bitCount(empties);
//    if (nEmpties <= Constants.EMPTIES_FOR_SUPER_FAST_ENDGAME) {
//      return evaluateSuperFast(player, opponent, lower, upper, false, 0, 0);
//    }
    int scUpper = stableDisks.getUpperBound(player, opponent);
    if (scUpper <= lower) {
      return scUpper;
    }

    int best = Integer.MIN_VALUE;
    long flip;
    long[] firstInEdges = firstLastInEdges(empties);
    masksTmp[0] = ~GetFlip.neighbors(empties) & GetFlip.neighbors(player);
    masksTmp[1] = firstInEdges[0] & firstInEdges[1] & GetFlip.neighbors(player);
    masksTmp[2] = GetFlip.neighbors(lastMove) & CORNERS;
    masksTmp[4] = firstInEdges[0] | firstInEdges[1];
    masksTmp[5] = ((lastMove & XSQUARES) == 0) ? GetFlip.neighbors(lastMove) : 0;
    int move;
    long moveBit;
    boolean pass = true;
    
    for (long mask : masksTmp) {
      while ((empties & mask) != 0) {
        move = Long.numberOfTrailingZeros(empties & mask);
        moveBit = 1L << move;
        empties = empties & (~moveBit);
        flip = GetMoves.getFlip(move, player, opponent) & ~player;
        if (flip == 0) {
          continue;
        }
        nVisited++;
        if (nEmpties == 6) {
          best = Math.max(best, 
            -evalFiveEmpties(
                opponent & ~flip, player | flip, -upper, -Math.max(lower, best),
                scUpper - lower, scLastDiffPlayerDelta, (moveBit & CORNERS) != 0));  
        } else {
          best = Math.max(best, 
            -evaluateFast(
                opponent & ~flip, player | flip, -upper, -Math.max(lower, best),
                false, flip, scUpper - lower, scLastDiffPlayerDelta));        
        }
        pass = false;
        if (best >= upper) {
          break;
        }
      }
      if (best >= upper) {
        break;
      }
    }
    if (pass) {
      if (passed) {
        return BitPattern.getEvaluationGameOver(player, opponent);
      }
      return -evaluateFast(opponent, player, -upper, -lower, true, 0, scUpper - lower, scLastDiffPlayerDelta);
    }
    return best;
  }
  
  public int evaluatePosition(long player, long opponent, int lower, int upper, long lastMove) {
    if (~(player | opponent) == 0) {
      return BitPattern.getEvaluationBoardFull(player);
    }

    return evaluateFast(player, opponent, lower, upper, false, lastMove, 0, 0);
  }
  
  public int evaluatePosition(Board b, int lower, int upper, long lastMove) {
    return evaluatePosition(b.getPlayer(), b.getOpponent(), lower, upper, lastMove);
  }
  
  public void resetNVisited() {
    nVisited = 0;
  }
  
  
  public int getNVisited() {
    return nVisited;
  }
  
  public static void main(String args[]) {
    
    int x[] = new int[100000];
    for (int i = 0; i < x.length; ++i) {
      x[i] = (int) (Math.random() * 64);
    }
    EvaluatorLastMoves eval = new EvaluatorLastMoves();
    
    ThreadMXBean thread = ManagementFactory.getThreadMXBean();
    double t = -thread.getCurrentThreadCpuTime();
    for (int iter = 0; iter < 1000000; ++iter) {
      double tmp = Math.random();
      long player = (long) (tmp * Long.MAX_VALUE);
      long opponent = ~player;
      
      int x1 = 0;
      int x2 = 63;
      int x3 = 12;
      int x4 = 13;
     
      player = player & ~(1L << x1);
      opponent = opponent & ~(1L << x1);
      player = player & ~(1L << x2);
      opponent = opponent & ~(1L << x2);
      player = player & ~(1L << x3);
      opponent = opponent & ~(1L << x3);
      player = player & ~(1L << x4);
      opponent = opponent & ~(1L << x4);
      int low = (int) (tmp * 120) - 60;
      eval.evalFiveEmpties(player, opponent, low, low+1, 0, 0, true);
    }
    t += thread.getCurrentThreadCpuTime();
    System.out.println(new DecimalFormat("#,###.00").format(t / 1000000000.0));
    System.out.println(new DecimalFormat("#,###").format(eval.getNVisited() * 1000000000.0 / t));
  }
}
