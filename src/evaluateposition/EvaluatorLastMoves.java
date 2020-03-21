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
import evaluatedepthone.FindStableDisks;

/**
 *
 * @author michele
 */
public class EvaluatorLastMoves {

  private static final int EMPTIES_FOR_SUPER_FAST = 5;
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
  private int evalOneEmpty(long player, long opponent, boolean passed) {
    int position = Long.numberOfTrailingZeros(~(player | opponent));
    long flip = flipper.getFlipLast(position, opponent);
    if (flip != 0) {
      return BitPattern.getEvaluationBoardFull(player | flip);
    }
    if (passed) {
      return BitPattern.getEvaluationGameOver(player, opponent);
    }
    return -evalOneEmpty(opponent, player, true);
  }
  
  private int evaluateSuperFast(long player, long opponent, int lower, int upper,
                                boolean passed) {
    long empties = ~(player | opponent);
    int nEmpties = Long.bitCount(empties);
    if (nEmpties >= 4) {
      int stabilityCutoffUpper = stableDisks.getUpperBound(player, opponent);
      if (stabilityCutoffUpper <= lower) {
        return stabilityCutoffUpper;
      }
    }
    boolean pass = true;
    int best = Integer.MIN_VALUE;
    int move;
    boolean twoEmpties = nEmpties == 2;

    while (empties != 0) {
      move = Long.numberOfTrailingZeros(empties);      
      empties = empties & (~(1L << move));
      long flip = flipper.getFlip(move, player, opponent);
      if (flip == 0) {
        continue;
      }
      nVisited++;
      if (twoEmpties) {
        best = Math.max(best, -evalOneEmpty(opponent & ~flip, player | flip, false));
      } else {
        best = Math.max(best, 
          -evaluateSuperFast(opponent & ~flip, player | flip, -upper, 
                             -Math.max(lower, best), false));
      }
      pass = false;
      if (best >= upper) {
        return best;
      }
    }
    
    if (pass) {
      if (passed) {
        return BitPattern.getEvaluationGameOver(player, opponent);
      }
      return -evaluateSuperFast(opponent, player, -upper, -lower, true);
    }
    return best;
  }
  
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

  private int evaluateFast(long player, long opponent, int lower, int upper,
                           boolean passed, long lastMove) {
    long empties = ~(player | opponent);
    int nEmpties = Long.bitCount(empties);
    int stabilityCutoffUpper = stableDisks.getUpperBound(player, opponent);
    if (stabilityCutoffUpper <= lower) {
      return stabilityCutoffUpper;
    }
    if (nEmpties <= EMPTIES_FOR_SUPER_FAST) {
      return evaluateSuperFast(player, opponent, lower, upper, false);
    }

    int best = Integer.MIN_VALUE;
    long flip;
//    System.out.println(masksTmp);
//    masksTmp = new long[] {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    long[] firstInEdges = nEmpties > 5 ? firstLastInEdges(empties) : tmp;
    masksTmp[0] = ~GetFlip.neighbors(empties) & GetFlip.neighbors(player);
    masksTmp[1] = nEmpties > 5 ? firstInEdges[0] & firstInEdges[1] & GetFlip.neighbors(player) : 0;
    masksTmp[2] = GetFlip.neighbors(lastMove) & CORNERS;
    masksTmp[4] = nEmpties > 5 ? firstInEdges[0] | firstInEdges[1] : 0;
    masksTmp[5] = ((lastMove & XSQUARES) == 0) ? GetFlip.neighbors(lastMove) : 0;
    int move;
    long moveBit;
    boolean pass = true;
    
    for (long mask : masksTmp) {
      while ((empties & mask) != 0) {
        move = Long.numberOfTrailingZeros(empties & mask);
        moveBit = 1L << move;
        empties = empties & (~moveBit);
        flip = flipper.getFlip(move, player, opponent) & ~player;
        if (flip == 0) {
          continue;
        }
        nVisited++;
        best = Math.max(best, 
          -evaluateFast(opponent & ~flip, player | flip, -upper, 
                             -Math.max(lower, best), false, flip));
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
      return -evaluateFast(opponent, player, -upper, -lower, true, 0);
    }
    return best;
  }
  
  public int evaluatePosition(long player, long opponent, int lower, int upper, long lastMove) {
    if (~(player | opponent) == 0) {
      return BitPattern.getEvaluationBoardFull(player);
    }

    return evaluateFast(player, opponent, lower, upper, false, lastMove);
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
    System.out.println(BitPattern.patternToString(-1L));
    System.out.println(BitPattern.patternToString(-1L << (-9)));
//    Board b = new Board("--XXXXX--OOOXX-O-OOOXXOX-OXOXOXXOXXXOXXX--XOXOXX-XXXOOO--OOOOO--", true);
//    EvaluatorLastMoves eval = new EvaluatorLastMoves();
//    long t = System.currentTimeMillis();
//    eval.evaluatePosition(b, -6400, 6400);
//    System.out.println(eval.getNVisited() + " " + (System.currentTimeMillis() - t));
  }
}
