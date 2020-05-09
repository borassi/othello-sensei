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

  public EvaluatorMCTS(long maxNVisited, int maxSize, int arraySize) {
    this(maxNVisited, maxSize, arraySize, PossibleMovesFinderImproved.load());
  }

  public EvaluatorMCTS(long maxNVisited, int maxSize, int arraySize, 
                        PossibleMovesFinderImproved possibleMovesFinder) {
    this(maxNVisited, maxSize, arraySize, possibleMovesFinder, new EvaluatorMidgame());
  }

  public EvaluatorMCTS(long maxNVisited, int maxSize, int arraySize,
                       PossibleMovesFinderImproved possibleMovesFinder,
                       EvaluatorMidgame evaluatorMidgame) {
    super(maxNVisited, maxSize, arraySize, possibleMovesFinder);
//    this.depthOneEval = depthOneEvaluator;
    this.evaluatorMidgame = evaluatorMidgame;
  }
  
  public synchronized float getEval() {
    return get(super.firstPosition.getBoard()).eval;
  }
//
//  public ObjectArrayList<Board> getWeirdPositions(int minTrainingSetSize) {
//    ObjectArrayList<Board> result = new ObjectArrayList<>();
//    for (StoredBoard b = first(); b != null; b = next(b.player, b.opponent)) {
//      if (depthOneEval.getMinTrainingSetSize(b.getBoard()) < minTrainingSetSize) {
//        result.add(b.getBoard());
//      }
//    }
//    return result;
//  }
  
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
  
  private int heuristicToStop() {
    return 
        (super.firstPosition.getUpperBound() - super.firstPosition.getLowerBound()) * 10000
        // bestVariationLower > bestVariationUpper; maximize the difference.
        - (super.firstPosition.bestVariationOpponent - super.firstPosition.bestVariationPlayer);
  }

  int[] deltas = new int[Constants.N_SAMPLES];
  protected int addPositions(StoredBoard father) {
    long player = father.player;
    long opponent = father.opponent;
    long[] moves = possibleMovesFinder.possibleMovesAdvanced(player, opponent);
//     TODO
//    this.depthOneEval.setup(player, opponent);
//    this.depthOneEval.invert();
    int addedPositions = 0;
    if (moves.length == 0) {
      this.updateSolved(player, opponent, BitPattern.getEvaluationGameOver(player, opponent));
    } else {
      StoredBoard[] children = new StoredBoard[moves.length];
      for (int i = 0; i < Constants.N_SAMPLES; i++) {
        deltas[i] = father.samples[i] - father.eval;
      }
      for (int i = 0; i < moves.length; i++) {
        long move = moves[i];
        Board childBoard = father.getBoard().move(move);
//        if (move != 0) {
//          this.depthOneEval.update(Long.numberOfTrailingZeros(move & ~(player | opponent)), move);
//          result = this.depthOneEval.eval();
//          this.depthOneEval.undoUpdate(Long.numberOfTrailingZeros(move & ~(player | opponent)), move);
//        } else {
//          result = this.depthOneEval.eval();
//        }
//        float error = this.depthOneEval.lastError();
        evaluatorMidgame.resetNVisitedPositions();
        int result = evaluatorMidgame.evaluatePosition(childBoard, 2, -6400, 6400);
        int error = 300;
        long visited = evaluatorMidgame.getNVisited() + 1;
        children[i] = new StoredBoard(childBoard, result, error, deltas);
        children[i].descendants += visited;
        addedPositions += visited;
      }
      super.add(children, father);
    }
    return addedPositions;
  }
  
  protected void updateEndgame(HashMapVisitedPositions.PositionToImprove pos, int eval) {
//    System.out.println("\n\n\n" + pos.alpha + " " + pos.beta + pos.board + eval);
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
    StoredBoard currentStored = new StoredBoard(
        current,
        evaluatorMidgame.evaluatePosition(current, 2, -6400, 6400),
        800);

//    StoredBoard currentStored = new StoredBoard(current, this.depthOneEval.eval(current), 800);
    super.addFirstPosition(currentStored);
    this.evaluatorMidgame.resetNVisitedPositions();
    HashMapVisitedPositions.PositionToImprove nextPos;
    float bestHeuristicToStop = Float.POSITIVE_INFINITY;
    int oldStable = Integer.MIN_VALUE;
    long seenPositions = 0;
    while ((nextPos = nextPositionToImprove()) != null) {
      StoredBoard next = nextPos.board;

      if (next.getBoard().getEmptySquares() <= Constants.EMPTIES_FOR_FORCED_MIDGAME && next != this.firstPosition) {
        this.evaluatorMidgame.resetNVisitedPositions();
        int curEval = evaluatorMidgame.evaluatePosition(
            next.getBoard(), Constants.EMPTIES_FOR_FORCED_MIDGAME, nextPos.alpha, nextPos.beta);
        seenPositions = evaluatorMidgame.getNVisited() + 1;
        updateEndgame(nextPos, curEval);
      } else if (this.size < this.maxSize) {
        seenPositions = this.addPositions(next);
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
        }
        seenPositions = evaluatorMidgame.getNVisited() + 1;
        if (d >= next.getBoard().getEmptySquares()) {
          updateEndgame(nextPos, curEval);
        } else {
          nextPos.board.updateEval(curEval, (float) (800 / Math.sqrt(d+1) * Math.sqrt(2)));
          updateFathers(nextPos.board);
        }
      }
      for (StoredBoard b : nextPos.parents) {
        b.descendants += seenPositions;
      }
      if (this.firstPosition.isSolved()) {
//        System.out.println("STOPPING FINISHED");
        break;
      }
//      float heuristicToStop = this.heuristicToStop();
//      bestHeuristicToStop = Math.min(bestHeuristicToStop, heuristicToStop);
//
//      if (heuristicToStop == bestHeuristicToStop && this.firstPosition.descendants > maxSize * 0.8) {
//        System.out.println("STOPPING HEUR");
//        System.out.println(heuristicToStop);
//        System.out.println(bestHeuristicToStop);
//        System.out.println(super.firstPosition.getUpperBound());
//        System.out.println(super.firstPosition.getLowerBound());
//        // bestVariationLower > bestVariationUpper; maximize the difference.
//        System.out.println(super.firstPosition.bestVariationOpponent);
//        System.out.println(super.firstPosition.bestVariationPlayer);
//        System.out.println(size + " " + maxSize);
//        break;
//      }
      if (this.firstPosition.isPartiallySolved() &&
          this.firstPosition.eval != oldStable) {
//        System.out.println("Stabilizing at " + this.firstPosition.eval);
        oldStable = this.firstPosition.eval;
        this.setEvalGoal(this.firstPosition.eval);
//        break;
      }
      if (this.firstPosition.descendants > maxNVisited) {
        System.out.println("STOPPING SIZE" + maxNVisited);
        break;
      }
    }
//    System.out.println(this.size() + this.lastMovesEval.nVisited);
//    System.out.println("W" + weird + "/" + tot);
//    System.out.print("  " + wrong + "/" + tot + "  ");
//    System.out.println(-get(current).getEval());
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