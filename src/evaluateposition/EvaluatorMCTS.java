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
import constants.Constants;
import java.util.ArrayList;

public class EvaluatorMCTS extends HashMapVisitedPositions implements EvaluatorInterface {
  private final EvaluatorMidgame evaluatorMidgame;
  private long maxNVisited;

  public EvaluatorMCTS(long maxNVisited, int arraySize, long maxSize) {
    this(maxNVisited, arraySize, maxSize, PossibleMovesFinderImproved.load(), 
        new EvaluatorMidgame());
  }

  public EvaluatorMCTS(long maxNVisited, int arraySize, long maxSize, 
                       PossibleMovesFinderImproved possibleMovesFinder) {
    this(maxNVisited, arraySize, maxSize, possibleMovesFinder, new EvaluatorMidgame());
  }

  public EvaluatorMCTS(long maxNVisited, int arraySize, long maxSize,
                       PossibleMovesFinderImproved possibleMovesFinder,
                       EvaluatorMidgame evaluatorMidgame) {
    super(arraySize, maxSize, possibleMovesFinder);
//    this.depthOneEval = depthOneEvaluator;
    this.evaluatorMidgame = evaluatorMidgame;
    this.maxNVisited = maxNVisited;
  }
  
  public synchronized float getEval() {
    return get(super.firstPosition.getBoard()).eval;
  }
  
  @Override  
  public long getNVisited() {
    return this.firstPosition.descendants;
  }
  
  public long getNStored() {
    return this.size;
  }
  
  @Override
  public int evaluatePosition(Board current, int depth, int alpha, int beta) {
    return evaluatePosition(current, depth);
  }
  
  private double heuristicToStop() {
    return 0;
  }

  protected int addPositions(PositionToImprove fatherPos) {
    StoredBoard father = fatherPos.board;
    long player = father.player;
    long opponent = father.opponent;
    long[] moves = possibleMovesFinder.possibleMovesAdvanced(player, opponent);
//     TODO
    int addedPositions = 0;
    if (moves.length == 0) {
      father.setSolved(BitPattern.getEvaluationGameOver(player, opponent));
      father.updateFathers();
    } else {
      StoredBoard[] children = new StoredBoard[moves.length];
//      for (int i = 0; i < StoredBoard.N_SAMPLES; i++) {
//        deltas[i] = father.samples[i] - father.eval;
//      }
      for (int i = 0; i < moves.length; i++) {
        long move = moves[i];
        Board childBoard = father.getBoard().move(move);
        evaluatorMidgame.resetNVisitedPositions();
        int result = 0;
//        if (this.hasEvalGoal()) {
          result = evaluatorMidgame.evaluatePosition(childBoard, 2, -6400, 6400);          
//        } else {
//          result = evaluatorMidgame.evaluatePosition(childBoard, 2, -fatherPos.beta, -fatherPos.alpha);
//        }
//        System.out.println(result + " " + childBoard.getEmptySquares());
        int error = 400;
        long visited = evaluatorMidgame.getNVisited() + 1;
        children[i] = new StoredBoard(childBoard, result, error, visited);
        addedPositions += visited;
      }
      super.add(children, father);
    }
    return addedPositions;
  }
  
  protected void updateEndgame(PositionToImprove pos, int eval) {
//    System.out.println("\n\n\n" + pos.alpha + " " + pos.beta + pos.board + eval);
    StoredBoard b = pos.board;
    if (eval <= pos.alpha) {
      b.setUpper(eval);
    } else if (eval >= pos.beta) {
      b.setLower(eval);
    } else {
      b.setSolved(eval);
    }
    b.updateFathers();
  }

  public short evaluatePosition(Board current, int depth) {
    this.evaluatorMidgame.resetNVisitedPositions();
    StoredBoard currentStored = new StoredBoard(
        current,
        evaluatorMidgame.evaluatePosition(current, 2, -6400, 6400),
        600,
        evaluatorMidgame.getNVisited());

    super.addFirstPosition(currentStored);
    HashMapVisitedPositions.PositionToImprove nextPos;
    double bestHeuristicToStop = Double.POSITIVE_INFINITY;
    long seenPositions = 0;
    this.evalGoal = 0;

    while ((nextPos = nextPositionToImprove()) != null) {
      StoredBoard next = nextPos.board;

      if (next.getBoard().getEmptySquares() <= Constants.EMPTIES_FOR_FORCED_MIDGAME && next != this.firstPosition) {
        this.evaluatorMidgame.resetNVisitedPositions();
        int curEval = evaluatorMidgame.evaluatePosition(
            next.getBoard(), Constants.EMPTIES_FOR_FORCED_MIDGAME, nextPos.alpha, nextPos.beta);
        seenPositions = evaluatorMidgame.getNVisited() + 1;
        updateEndgame(nextPos, curEval);
      } else if (this.size < this.maxSize) {
        seenPositions = this.addPositions(nextPos);
        System.out.println("Add " + seenPositions);
      } else {
        this.evaluatorMidgame.resetNVisitedPositions();
        int curEval = evaluatorMidgame.evaluatePosition(
            next.getBoard(), 2, nextPos.alpha, nextPos.beta);
        int d;
        for (d = 4; evaluatorMidgame.getNVisited() < next.descendants; d += 2) {
          if (next.getBoard().getEmptySquares() - d < 8) {
            d = next.getBoard().getEmptySquares();
          }
          curEval = evaluatorMidgame.evaluatePosition(
            next.getBoard(), d, nextPos.alpha, nextPos.beta);
//          System.out.println(d + " " + evaluatorMidgame.getNVisitedPositions());
        }
        seenPositions = evaluatorMidgame.getNVisited() + 1;
        System.out.println("Midgame " + seenPositions);
        if (d >= next.getBoard().getEmptySquares()) {
          updateEndgame(nextPos, curEval);
        } else {
          nextPos.board.setEval(curEval, (float) (800 / Math.sqrt(d+1) * Math.sqrt(2)));
          nextPos.board.updateFathers();
        }
      }
      System.out.println(this.firstPosition.descendants);
      for (StoredBoard b : nextPos.parents) {
        b.descendants += seenPositions;
      }
      if (this.firstPosition.isSolved()) {
//        System.out.println("STOPPING FINISHED");
        break;
      }
//      double heuristicToStop = this.heuristicToStop();
//      bestHeuristicToStop = Math.min(bestHeuristicToStop, heuristicToStop);
//
//      if (heuristicToStop == bestHeuristicToStop && this.firstPosition.descendants > maxNVisited * 0.8) {
//        break;
//      }
      if (this.firstPosition.descendants > maxNVisited) {
//        System.out.println("STOPPING SIZE");
        break;
      }
    }
//    System.out.println(this.size);
    return (short) -get(current).getEval();
  }
  
  public int getEvalGoal() {
    return evalGoal;
  }

  public ArrayList<StoredBoard> evaluatePositionAll(Board current, int depth) {
    evaluatePosition(current, depth);
    long[] moves = possibleMovesFinder.possibleMovesAdvanced(current.getPlayer(), current.getOpponent());
    ArrayList<StoredBoard> evaluations = new ArrayList<>();
    for (long move : moves) {
      Board next = current.move(move);
      evaluations.add(get(next));
    }
    return evaluations;
  }
}
