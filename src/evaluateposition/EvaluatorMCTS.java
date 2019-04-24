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
import evaluatedepthone.MultilinearRegression;
import it.unimi.dsi.fastutil.objects.ObjectArrayList;

public class EvaluatorMCTS extends HashMapVisitedPositions implements EvaluatorInterface {
  private final EvaluatorLastMoves lastMovesEval;
  private final DepthOneEvaluator depthOneEval;

  public EvaluatorMCTS(int arraySize, int nElements) {
    this(arraySize, nElements, PossibleMovesFinderImproved.load(), 
        MultilinearRegression.load(EvaluatorAlphaBeta.DEPTH_ONE_EVALUATOR_FILEPATTERN),
        new EvaluatorLastMoves());
  }

  public EvaluatorMCTS(int arraySize, int nElements, 
                         PossibleMovesFinderImproved possibleMovesFinder,
                         DepthOneEvaluator depthOneEvaluator,
                         EvaluatorLastMoves evaluatorLastMoves) {
    super(arraySize, nElements, possibleMovesFinder);
    this.depthOneEval = depthOneEvaluator;
    this.lastMovesEval = evaluatorLastMoves;
  }
  
  public synchronized float getEval() {
    return get(super.firstPosition.getBoard()).eval;
  }

  public ObjectArrayList<Board> getWeirdPositions(int minTrainingSetSize) {
    ObjectArrayList<Board> result = new ObjectArrayList<>();
    for (StoredBoard b = first(); b != null; b = next(b.player, b.opponent)) {
      if (depthOneEval.getMinTrainingSetSize(b.getBoard()) < minTrainingSetSize) {
        result.add(b.getBoard());
      }
    }
    return result;
  }
  
  @Override  
  public long getNVisited() {
    return size;
  }
  
  public int getNVisitedPositionsByEvaluator() {
    return lastMovesEval.nVisited;
  }
  
  @Override
  public int evaluatePosition(Board current, int depth, int alpha, int beta) {
    return evaluatePosition(current, depth);
  }
  
  private int heuristicToStop() {
    return 
        (super.firstPosition.getUpperBound() - super.firstPosition.getLowerBound()) * 10000
        // opponentVariates > playerVariates; maximize the difference.
        + (super.firstPosition.playerVariates - super.firstPosition.opponentVariates);
  }
  
//  protected int eval(Board b) {
//    long player = b.getPlayer();
//    long opponent = b.getOpponent();
//    long[] moves = possibleMovesFinder.possibleMovesAdvanced(player, opponent);
//    int eval = -Integer.MIN_VALUE;
//    if (moves.length == 0) {
//      return BitPattern.getEvaluationGameOver(player, opponent);
//    } else {
//      for (int i = 0; i < moves.length; i++) {
//        long move = moves[i];
//        Board childBoard = b.move(move);
//        eval = Math.max(eval, -this.depthOneEval.eval(childBoard));
//      }
//    }
//    return eval + this.depthOneEval.eval(b);
//  }
//  
  int[] deltas = new int[StoredBoard.N_SAMPLES];
  protected void addPositions(StoredBoard father) {
    long player = father.player;
    long opponent = father.opponent;
    long[] moves = possibleMovesFinder.possibleMovesAdvanced(player, opponent);
    if (moves.length == 0) {
      super.updateSolved(player, opponent,
          BitPattern.getEvaluationGameOver(player, opponent));
    } else {
      StoredBoard[] children = new StoredBoard[moves.length];
      for (int i = 0; i < StoredBoard.N_SAMPLES; i++) {
        deltas[i] = father.samples[i] - father.eval;
      }
      for (int i = 0; i < moves.length; i++) {
        long move = moves[i];
        Board childBoard = father.getBoard().move(move);
        int result = this.depthOneEval.eval(childBoard);
        StoredBoard child = new StoredBoard(childBoard, result, 800, deltas);
//        StoredBoard child = new StoredBoard(childBoard, result, 1000);
        if (childBoard.getEmptySquares() == 0) {
          child.setSolved(BitPattern.getEvaluationGameOver(childBoard));
        }
        children[i] = child;
      }
      super.add(children, father);
    }
  }
  
  protected void updateEndgame(PositionToImprove pos, int eval) {
//    System.out.println(eval + " " + pos.alpha + " " + pos.beta);

    if (eval <= pos.alpha) {
      // Tricky but probably correct.
      super.updateUpper(pos.board.player, pos.board.opponent, eval);
    } else if (eval >= pos.beta) {
      super.updateLower(pos.board.player, pos.board.opponent, eval);
    } else {
      super.updateSolved(pos.board.player, pos.board.opponent, eval);
    }
  }

  public short evaluatePosition(Board current, int depth) {
//    StoredBoard currentStored = new StoredBoard(current, this.depthOneEval.eval(current), 600, deltas);
    StoredBoard currentStored = new StoredBoard(current, this.depthOneEval.eval(current), 600);
    super.addFirstPosition(currentStored);
    this.lastMovesEval.nVisited = 0;
    PositionToImprove nextPos;
    float bestHeuristicToStop = Float.POSITIVE_INFINITY;
    int tot = 0;
    int wrong = 0;

    while ((nextPos = nextPositionToImprove()) != null) {
      StoredBoard next = nextPos.board;

      if (next.getBoard().getEmptySquares() <= 6 && next != this.firstPosition) {
//        if (nextPos.playerIsStartingPlayer) {
//          alpha = -this.firstPosition.getLowerBound() - 100;
//          beta = -this.firstPosition.getUpperBound() + 100;
//          eval = firstPosition.eval;
//        } else {
//          alpha = this.firstPosition.getUpperBound() - 100;
//          beta = this.firstPosition.getLowerBound() + 100;
//          eval = -firstPosition.eval;
//        }
//        alpha = -6600;
//        beta = 6600;
//        System.out.println(nextPos.playerIsStart10ingPlayer);
//        System.out.println(firstPosition);
//        System.out.println(next);
//        System.out.println(alpha);
//        System.out.println(beta);
        if (next.playerVariates == -6600 || next.opponentVariates == 6600) {
          wrong++;
//          break;
        }
        tot++;
//        nextPos.alpha = -6600;
//        nextPos.beta = 6600;
        int curEval = lastMovesEval.evaluatePosition(next.getBoard(), nextPos.alpha, nextPos.beta);
        updateEndgame(nextPos, curEval);
      } else {
        this.addPositions(next);
      }
      float heuristicToStop = this.heuristicToStop();
      bestHeuristicToStop = Math.min(bestHeuristicToStop, heuristicToStop);

      if (heuristicToStop == bestHeuristicToStop && size > maxSize * 0.8) {
        System.out.println("STOPPING HEUR");
//        System.out.println(size + " " + maxSize);
        break;
      }
      if (this.firstPosition.isSolved()) {
//        System.out.println("STOPPING SOLVED");
        break;
      }
//      if (this.firstPosition.isPartiallySolved() &&
//          this.firstPosition.eval - this.firstPosition.playerVariates > 2 &&
//          this.firstPosition.opponentVariates - this.firstPosition.eval > 2) {
////        System.out.println("STOPPING SOLVED");
//        break;
//      }
      if (size > maxSize) {
//        System.out.println("STOPPING SIZE");
        break;
      }
    }
//    System.out.print("  " + wrong + "/" + tot + "  ");
    return (short) -get(current).getEval();
  }
  
  public ObjectArrayList<StoredBoard> evaluatePositionAll(Board current, int depth) {
    evaluatePosition(current, depth);
    long[] moves = possibleMovesFinder.possibleMoves(current);
    ObjectArrayList<StoredBoard> evaluations = new ObjectArrayList<>();
    for (long move : moves) {
      Board next = current.move(move);
      evaluations.add(get(next));
    }
    return evaluations;
  }
}
