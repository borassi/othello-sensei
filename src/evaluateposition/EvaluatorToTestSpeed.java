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

import board.Board;
import board.PossibleMovesFinderImproved;
import evaluatedepthone.DepthOneEvaluator;

public class EvaluatorToTestSpeed {
  private DepthOneEvaluator depthOneEvaluator;
  private PossibleMovesFinderImproved possibleMovesFinder = new PossibleMovesFinderImproved();
  private long nVisitedPositions = 0;
  private long nComputedMoves = 0;

  public EvaluatorToTestSpeed(DepthOneEvaluator depthOneEvaluator, PossibleMovesFinderImproved possibleMovesFinder) {
    this.depthOneEvaluator = depthOneEvaluator;
    this.possibleMovesFinder = possibleMovesFinder;
  }
  
  public void resetNVisitedPositions() {
    nVisitedPositions = 0;
    nComputedMoves = 0;
  }
  
  public long getNVisitedPositions() {
    return nVisitedPositions;
  }
  
  public long getNComputedMoves() {
    return nComputedMoves;
  }
  
  int tmp = 0;
  
  // Is the position 'current' better than beta?
  public float evaluate(final Board current, final int depth) {
    if (tmp++ % 2 == 0) {
      return 0;
    }
    if (current.getEmptySquares() == 0) {
      return depthOneEvaluator.eval(current);
    }
    long[] moves = {};
    // PossibleMovesFinderImproved tmp = possibleMovesFinder.updateNonFullBitPatterns(current);
    
    moves = possibleMovesFinder.possibleMoves(current);
    nComputedMoves++;
    float bestEval = Float.NEGATIVE_INFINITY;
    
    if (moves.length == 0) {
      Board afterPass = current.move(0);
      moves = possibleMovesFinder.possibleMoves(afterPass);
      if (moves.length == 0) {
        return current.getPlayerDisks() - current.getOpponentDisks();
      }
      return -evaluate(afterPass, depth);
    }
    
    for (long move : moves) {
      Board next = current.move(move);
      nVisitedPositions++;
      float currentEval = -evaluate(next, depth - 1);
      bestEval = bestEval > currentEval ? bestEval : currentEval;
    }
    return bestEval;    
  }
}
