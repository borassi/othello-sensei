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
import java.util.ArrayList;

import board.Board;
import board.PossibleMovesFinderImproved;
import evaluatedepthone.BoardWithEvaluation;
import evaluatedepthone.DepthOneEvaluator;
import evaluatedepthone.MultilinearRegression;
import evaluateposition.HashMapVisitedPositionsForAlphaBeta.StoredBoardForAlphaBeta;
import it.unimi.dsi.fastutil.objects.ObjectArrayList;
import it.unimi.dsi.fastutil.objects.ObjectArrays;

public class EvaluatorAlphaBeta implements EvaluatorInterface {
  private final DepthOneEvaluator depthOneEvaluator;
  private PossibleMovesFinderImproved possibleMovesFinder;
  private final HashMapVisitedPositionsForAlphaBeta hashMapVisitedPositions;
  private long nVisitedPositions = 0;
  private long nComputedMoves = 0;

  private BoardWithEvaluation[][] nextPositions; 

  public static final String DEPTH_ONE_EVALUATOR_FILEPATTERN = 
          "coefficients/multilinear_regression_improved.sar";

  public EvaluatorAlphaBeta() {
    this(MultilinearRegression.load(EvaluatorAlphaBeta.DEPTH_ONE_EVALUATOR_FILEPATTERN), 
         new PossibleMovesFinderImproved(),
         new HashMapVisitedPositionsForAlphaBeta());
  }

  public EvaluatorAlphaBeta(DepthOneEvaluator depthOneEvaluator, 
                   PossibleMovesFinderImproved possibleMovesFinder,
                   HashMapVisitedPositionsForAlphaBeta hashMapVisitedPositions) {
    this.depthOneEvaluator = depthOneEvaluator;
    this.possibleMovesFinder = possibleMovesFinder;
    this.nextPositions = new BoardWithEvaluation[64][64];
    this.hashMapVisitedPositions = hashMapVisitedPositions;
    for (int i = 0; i < nextPositions.length; i++) {
      BoardWithEvaluation[] curEvaluations = nextPositions[i];
      for (int j = 0; j < curEvaluations.length; j++) {
        curEvaluations[j] = new BoardWithEvaluation(new Board(0, 0), 0);
      }
    }
  }
  
  public void printHashMapVisitedPositions() {
    System.out.println(hashMapVisitedPositions);
  }

  @Override
  public int evaluatePosition(Board current, int depth, int alpha, int beta) {
    int nEmpties = current.getEmptySquares();
    if (nEmpties == 0) {
      return BitPattern.getEvaluationBoardFull(current);
    }
    long[] moves = possibleMovesFinder.possibleMoves(current);

    nComputedMoves++;
    if (moves.length == 0) {
      Board afterPass = current.move(0);
      moves = possibleMovesFinder.possibleMoves(afterPass);
      if (moves.length == 0) {
        return BitPattern.getEvaluationGameOver(current);
      }
      return -evaluatePosition(afterPass, depth, -beta, -alpha);
    }

    if (nEmpties == 1) {
      return BitPattern.getEvaluationBoardFull(current.getPlayer() | moves[0]);
    }

    int bestEval = Integer.MIN_VALUE;
    
    for (long move : moves) {
      nVisitedPositions++;
      Board next = current.move(move);
      int currentEval = depth <= 1 ?
        -depthOneEvaluator.eval(next) :
        -evaluatePosition(next, depth - 1, -beta, -Math.max(alpha, bestEval));
      bestEval = bestEval > currentEval ? bestEval : currentEval;
      if (bestEval >= beta) {
        break;
      }
    }
    return bestEval;    
  }
  
  protected static final int getSortedMoves(
          Board b,
          PossibleMovesFinderImproved possibleMovesFinder,
          HashMapVisitedPositionsForAlphaBeta hashMapVisitedPositions,
          int depth,
          int alpha,
          int beta,
          DepthOneEvaluator depthOneEvaluator,
          BoardWithEvaluation[] output) {
    long[] moves = possibleMovesFinder.possibleMoves(b);
    int bestEval = Integer.MIN_VALUE;
    int nOutput = 0;
 
    for (long move : moves) {
      Board next = b.move(move);
      EvaluatedBoard evaluation = hashMapVisitedPositions.get(next);
      if (evaluation != null) {
        Evaluation lower = evaluation.getUpper().addOneDepth();
        if (lower.depth >= depth && lower.eval >= beta) {
          return lower.eval;
        }
        Evaluation upper = evaluation.getLower().addOneDepth();
        if (upper.depth >= depth && upper.eval <= alpha) {
          bestEval = upper.eval;
          continue;
        }
        output[nOutput++].update(next, -100 * lower.eval - upper.eval);
      } else {
        output[nOutput++].update(next, depthOneEvaluator.eval(next) + 1000);
      }
    }
    ObjectArrays.quickSort(output, 0, nOutput);
    output[nOutput].update(new Board(0, 0), Integer.MIN_VALUE);

    return bestEval;
  }
  
  public int evaluatePositionWithHashMap(Board current, int depth, int alpha, int beta) {
    int nEmpties = current.getEmptySquares();
    if (nEmpties == 0) {
      return BitPattern.getEvaluationBoardFull(current);
    }
    BoardWithEvaluation[] nextPositions = this.nextPositions[nEmpties];
    
    int bestEval = getSortedMoves(
      current, possibleMovesFinder, hashMapVisitedPositions, depth, 
      alpha, beta, depthOneEvaluator, nextPositions);
    nComputedMoves++;

    if (nextPositions[0].evaluation == Integer.MIN_VALUE && 
        bestEval == Integer.MIN_VALUE) {
      Board afterPass = current.move(0);
      long[] moves = possibleMovesFinder.possibleMoves(afterPass);
      if (moves.length == 0) {
        return BitPattern.getEvaluationGameOver(current);
      }
      return -evaluatePositionWithHashMap(afterPass, depth, -beta, -alpha);
    }

    if (nEmpties == 1) {
      return -BitPattern.getEvaluationBoardFull(nextPositions[0].board);
    }
    int currentEval;
    BoardWithEvaluation next;
    if (bestEval < beta) {
      for (int i = 0; ; i++) {
        next = nextPositions[i];
        if (next.evaluation == Integer.MIN_VALUE) {
          break;
        }
        nVisitedPositions++;
        
        if (depth <= 1) {
          currentEval = -depthOneEvaluator.eval(next.board);
        } else if (depth <= 3) {
          currentEval = -evaluatePosition(next.board, depth - 1, -beta, -Math.max(alpha, bestEval));
        } else {
          currentEval = -evaluatePositionWithHashMap(next.board, depth - 1, -beta, -Math.max(alpha, bestEval));
        }
        bestEval = Math.max(bestEval, currentEval);
        if (bestEval >= beta) {
          break;
        }
      }
      if (bestEval > alpha) {
        this.hashMapVisitedPositions.updateLowerBound(current, new Evaluation(bestEval, depth));
      }
      if (bestEval < beta) {
        this.hashMapVisitedPositions.updateUpperBound(current, new Evaluation(bestEval, depth));
      }
    }
    return bestEval;
  }
  
  public long getNComputedMoves() {
    return this.nComputedMoves;
  }
  
  
  public long getNVisitedPositions() {
    return this.nVisitedPositions;
  }
  
  public void resetHashMapVisitedPositions() {
    this.hashMapVisitedPositions.reset();
  }
  
  public int evaluatePosition(Board current, int depth) {
    evaluatePositionWithHashMap(current, depth - 4, -6400, 6400);
    return evaluatePositionWithHashMap(current, depth, -6400, 6400);
  }
  
  public StoredBoardForAlphaBeta getStoredBoard(Board board) {
    return this.hashMapVisitedPositions.getStoredBoard(board);
  }
  
  public ObjectArrayList<StoredBoard> evaluatePositionAll(Board current, int depth) {
    ObjectArrayList<StoredBoard> evaluations = new ObjectArrayList<>();
    long[] moves = possibleMovesFinder.possibleMoves(current);
 
    for (long move : moves) {
      Board next = current.move(move);
      int eval = -EvaluatorAlphaBeta.this.evaluatePosition(next, depth - 1);
      evaluations.add(new StoredBoard(next, eval, 0));
    }
    return evaluations;
  }

  public void evalVerbose(Board board) {
    depthOneEvaluator.evalVerbose(board);
  }

  public void resetNVisitedPositions() {
    this.nVisitedPositions = 0;
    this.nComputedMoves = 0;
  }

  @Override
  public long getNVisited() {
    return this.nVisitedPositions;
  }
}
