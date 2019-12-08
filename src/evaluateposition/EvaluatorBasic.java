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
import bitpattern.PositionIJ;
import board.Board;
import board.PossibleMovesFinderImproved;
import evaluatedepthone.DepthOneEvaluator;
import it.unimi.dsi.fastutil.objects.ObjectArrayList;
import java.util.ArrayList;

public class EvaluatorBasic implements EvaluatorInterface {
  private final DepthOneEvaluator depthOneEvaluator;
  private PossibleMovesFinderImproved possibleMovesFinder = new PossibleMovesFinderImproved();
  private int nVisitedPositions = 0;
  private long nComputedMoves = 0;

  public EvaluatorBasic(DepthOneEvaluator depthOneEvaluator, PossibleMovesFinderImproved possibleMovesFinder) {
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
  
  @Override
  public int evaluatePosition(Board current, int depth, int alpha, int beta) {
    return evaluatePosition(current, depth);
  }
  
  public int evaluatePosition(Board current, int depth) {
    if (current.getEmptySquares() == 0) {
      return BitPattern.getEvaluationBoardFull(current);
    }
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

  public ObjectArrayList<StoredBoard> evaluatePositionAll(Board current, int depth) {
    ObjectArrayList<StoredBoard> evaluations = new ObjectArrayList<>();
    long[] moves = possibleMovesFinder.possibleMoves(current);
 
    for (long move : moves) {
      Board next = current.move(move);
      int eval = -evaluatePosition(next, depth - 1);
      evaluations.add(new StoredBoard(next, eval, 0));
    }
    return evaluations;
  }

  @Override
  public long getNVisited() {
    return nVisitedPositions;
  }
}
