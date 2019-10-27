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
import evaluatedepthone.PatternEvaluatorImproved;
import it.unimi.dsi.fastutil.objects.ObjectArrayList;

public class EvaluatorMCTS extends HashMapVisitedPositions implements EvaluatorInterface {
  private final EvaluatorLastMoves lastMovesEval;
  private final PatternEvaluatorImproved depthOneEval;

  public EvaluatorMCTS(int arraySize, int nElements) {
    this(arraySize, nElements, PossibleMovesFinderImproved.load(), 
        PatternEvaluatorImproved.load(),
        new EvaluatorLastMoves());
  }

  public EvaluatorMCTS(int arraySize, int nElements, 
                         PossibleMovesFinderImproved possibleMovesFinder,
                         PatternEvaluatorImproved depthOneEvaluator,
                         EvaluatorLastMoves evaluatorLastMoves) {
    super(arraySize, nElements, possibleMovesFinder);
    this.depthOneEval = depthOneEvaluator;
    this.lastMovesEval = evaluatorLastMoves;
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
        // bestVariationLower > bestVariationUpper; maximize the difference.
        - (super.firstPosition.bestVariationOpponent - super.firstPosition.bestVariationPlayer);
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
  protected int getEvalGoalForBoard(StoredBoard b) {
    if (!hasEvalGoal()) {
      return 7000;
    }
    if (b.playerIsStartingPlayer) {
      return this.evalGoal;
    }
    return -this.evalGoal;
  }
  
  int[] deltas = new int[StoredBoard.N_SAMPLES];
  protected void addPositions(StoredBoard father) {
    long player = father.player;
    long opponent = father.opponent;
    long[] moves = possibleMovesFinder.possibleMovesAdvanced(player, opponent);
    // TODO
    this.depthOneEval.setup(player, opponent);
    this.depthOneEval.invert();
    if (moves.length == 0) {
      this.updateSolved(player, opponent, BitPattern.getEvaluationGameOver(player, opponent));
    } else {
      StoredBoard[] children = new StoredBoard[moves.length];
      for (int i = 0; i < StoredBoard.N_SAMPLES; i++) {
        deltas[i] = father.samples[i] - father.eval;
      }
      for (int i = 0; i < moves.length; i++) {
        long move = moves[i];
        Board childBoard = father.getBoard().move(move);
        int result;
        if (move != 0) {
          this.depthOneEval.update(Long.numberOfTrailingZeros(move & ~(player | opponent)), move);
          result = this.depthOneEval.eval();
          this.depthOneEval.undoUpdate(Long.numberOfTrailingZeros(move & ~(player | opponent)), move);
        } else {
          result = this.depthOneEval.eval();
        }
        children[i] = new StoredBoard(childBoard, result, 800, deltas);
      }
      super.add(children, father);
    }
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
    StoredBoard currentStored = new StoredBoard(current, this.depthOneEval.eval(current), 800, deltas);
//    StoredBoard currentStored = new StoredBoard(current, this.depthOneEval.eval(current), 800);
    super.addFirstPosition(currentStored);
    this.lastMovesEval.nVisited = 0;
    HashMapVisitedPositions.PositionToImprove nextPos;
    float bestHeuristicToStop = Float.POSITIVE_INFINITY;
    int oldStable = Integer.MIN_VALUE;
    int weird = 0;
    int tot = 0;
    while ((nextPos = nextPositionToImprove()) != null) {
      StoredBoard next = nextPos.board;

      if (next.getBoard().getEmptySquares() <= 12 && next != this.firstPosition) {
//        nextPos.alpha = -6600;
//        nextPos.beta = 6600;
        tot++;
        if (next.lower != -6400 || next.upper != 6400) {
          weird++;
//          System.out.println("WEIRD");
//          System.out.println(next);
//          System.out.println(next.fathers.get(0));
//          System.out.println(nextPos.playerVariates);
//          System.out.println(nextPos.alpha);
//          System.out.println(nextPos.beta);
//          break;
        }
//        System.out.println(next);
        long lastMove = 0;
        if (next.fathers.size() > 0) {
          Board father = next.fathers.get(0).getBoard();
          lastMove = (father.getPlayer() | father.getOpponent());
        }
        int curEval = lastMovesEval.evaluatePosition(next.getBoard(), nextPos.alpha, nextPos.beta, lastMove);
//        if (Math.random() < 0.01) {
//          System.out.println(next.getBoard().toStringOneLine() + " " + nextPos.alpha + " " + nextPos.beta);
//        }
        updateEndgame(nextPos, curEval);
      } else {
        this.addPositions(next);
      }
      float heuristicToStop = this.heuristicToStop();
      bestHeuristicToStop = Math.min(bestHeuristicToStop, heuristicToStop);

      if (this.firstPosition.isSolved()) {
        break;
      }
      if (heuristicToStop == bestHeuristicToStop && size > maxSize * 0.8) {
//        System.out.println("STOPPING HEUR");
//        System.out.println(heuristicToStop);
//        System.out.println(bestHeuristicToStop);
//        System.out.println(super.firstPosition.getUpperBound());
//        System.out.println(super.firstPosition.getLowerBound());
//        // bestVariationLower > bestVariationUpper; maximize the difference.
//        System.out.println(super.firstPosition.bestVariationOpponent);
//        System.out.println(super.firstPosition.bestVariationPlayer);
//        System.out.println(size + " " + maxSize);
        break;
      }
      if (this.firstPosition.isPartiallySolved() &&
          this.firstPosition.eval != oldStable) {
//        System.out.println("Stabilizing at " + this.firstPosition.eval);
        oldStable = this.firstPosition.eval;
        this.setEvalGoal(this.firstPosition.eval);
//        break;
      } 
//      System.out.println(this.firstPosition.getLowerBound() + " " + this.firstPosition.getUpperBound());
//      if (this.firstPosition.evalGoal != this.firstPosition.eval) {
////        super.setEvalGoal(Short.MAX_VALUE);
//      }
      if (size > maxSize) {
//        System.out.println("STOPPING SIZE");
        break;
      }
    }
//    System.out.println(this.size() + this.lastMovesEval.nVisited);
//    System.out.println("W" + weird + "/" + tot);
//    System.out.print("  " + wrong + "/" + tot + "  ");
    System.out.println(-get(current).getEval());
    return (short) -get(current).getEval();
  }
  
  public ObjectArrayList<StoredBoard> evaluatePositionAll(Board current, int depth) {
    evaluatePosition(current, depth);
    long[] moves = possibleMovesFinder.possibleMovesAdvanced(current.getPlayer(), current.getOpponent());
    ObjectArrayList<StoredBoard> evaluations = new ObjectArrayList<>();
    for (long move : moves) {
      Board next = current.move(move);
      evaluations.add(get(next));
    }
    return evaluations;
  }
}
