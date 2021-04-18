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
import board.PossibleMovesFinderImproved;
import evaluatedepthone.DepthOneEvaluator;
import evaluatedepthone.DiskDifferenceEvaluatorPlusTwo;
import java.util.ArrayList;

public class EvaluatorBasic implements EvaluatorInterface {
  private final DepthOneEvaluator depthOneEvaluator;
  private PossibleMovesFinderImproved possibleMovesFinder;
  private int nVisitedPositions = 0;
  private long nComputedMoves = 0;

  public EvaluatorBasic() {
    this(new DiskDifferenceEvaluatorPlusTwo(), PossibleMovesFinderImproved.load());
  }

  public EvaluatorBasic(DepthOneEvaluator depthOneEvaluator, PossibleMovesFinderImproved possibleMovesFinder) {
    this.depthOneEvaluator = depthOneEvaluator;
    this.possibleMovesFinder = possibleMovesFinder;
  }
  
  public void resetNVisited() {
    nVisitedPositions = 0;
    nComputedMoves = 0;
  }
  
  public long getNVisitedPositions() {
    return nVisitedPositions;
  }
  
  public long getNComputedMoves() {
    return nComputedMoves;
  }

  @Override
  public int evaluate(long player, long opponent, int depth, int alpha, int beta) {
    return evaluatePosition(new Board(player, opponent), depth);
  }
  
  public int evaluatePosition(Board current, int depth) {
    resetNVisited();
    if (depth <= 0) {
      return depthOneEvaluator.eval(current.getPlayer(), current.getOpponent());
    } 
    long[] moves = possibleMovesFinder.possibleMoves(current);
    nComputedMoves++;
    
    if (moves.length == 0) {
      Board afterPass = current.move(0);
      moves = possibleMovesFinder.possibleMoves(afterPass);
      if (moves.length == 0) {
        return BitPattern.getEvaluationGameOver(current);
      }
      return -evaluatePosition(afterPass, depth);
    }

    int bestEval = Integer.MIN_VALUE;
    for (long move : moves) {
      nVisitedPositions++;
      Board next = current.move(move);
      int currentEval = -evaluatePosition(next, depth - 1);
      bestEval = bestEval > currentEval ? bestEval : currentEval;
    }
    return bestEval;    
  }

  @Override
  public long getNVisited() {
    return nVisitedPositions;
  }
}
