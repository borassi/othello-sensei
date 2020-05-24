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

public class EvaluatorMCTS extends HashMapVisitedPositions {
  private final EvaluatorMidgame evaluatorMidgame;

  public EvaluatorMCTS(int maxSize, int arraySize) {
    this(maxSize, arraySize, PossibleMovesFinderImproved.load());
  }

  public EvaluatorMCTS(int maxSize, int arraySize, 
                        PossibleMovesFinderImproved possibleMovesFinder) {
    this(maxSize, arraySize, possibleMovesFinder, new EvaluatorMidgame());
  }

  public EvaluatorMCTS(int maxSize, int arraySize,
                       PossibleMovesFinderImproved possibleMovesFinder,
                       EvaluatorMidgame evaluatorMidgame) {
    super(maxSize, arraySize, possibleMovesFinder);
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

  public long getNVisited() {
    return this.firstPosition.descendants;
  }
  
  public long getNStored() {
    return this.size;
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
      StoredBoard b = getFull(player, opponent);
      b.setSolved(BitPattern.getEvaluationGameOver(player, opponent), this.getEvalGoalForBoard(b));
    } else {
      StoredBoard[] children = new StoredBoard[moves.length];
      int evalGoal = this.getEvalGoalForBoard(father);
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
        int depth = 2;//childBoard.getEmptySquares() <= Constants.EMPTIES_FOR_FORCED_MIDGAME + 2 ? 2 : 2;
        evaluatorMidgame.resetNVisitedPositions();
        int result = evaluatorMidgame.evaluatePosition(childBoard, depth, -6400, 6400);
        int error = 600;
        long visited = evaluatorMidgame.getNVisited() + 1;
        children[i] = new StoredBoard(childBoard, result, evalGoal);
        children[i].descendants += visited;
        addedPositions += visited;
      }
      super.add(children, father);
    }
    return addedPositions;
  }
  
  protected void updateEndgame(HashMapVisitedPositions.PositionToImprove pos, int eval) {
//    System.out.println("\n\n\n" + pos.alpha + " " + pos.beta + pos.board + eval);
    StoredBoard b = getFull(pos.board.player, pos.board.opponent);
    if (eval <= pos.alpha) {
      // Tricky but probably correct.
      b.setUpper(eval, this.getEvalGoalForBoard(b));
    } else if (eval >= pos.beta) {
      b.setLower(eval, this.getEvalGoalForBoard(b));
    } else {
      b.setSolved(eval, this.getEvalGoalForBoard(b));
    }
    b.updateFathers();
  }

  public long nEndgames;
  public long nWeirdEndgames;
  public long nVisitedEndgames;
  
  public enum Status {
    NONE,
    RUNNING,
    STOPPED_TIME,
    STOPPED_POSITIONS,
    SOLVED,
    KILLING,
    KILLED,
  }
  public Status status = Status.NONE;
  private long nextUpdateEvalGoal;

  
  public void updateEvalGoalIfNeeded() {
    if (this.firstPosition.proofNumberNextEval == 0) {
      this.setEvalGoal(evalGoal + 200);
      nextUpdateEvalGoal = (long) (this.firstPosition.descendants * 1.2);
//        System.out.println("Eval goal forced " + evalGoal + " after " + this.firstPosition.descendants);
    }
    if (this.firstPosition.disproofNumberNextEval == 0) {
      this.setEvalGoal(evalGoal - 200);
      nextUpdateEvalGoal = (long) (this.firstPosition.descendants * 1.2);    
//        System.out.println("Eval goal forced " + evalGoal + " after " + this.firstPosition.descendants);
    }
    if (this.firstPosition.descendants > nextUpdateEvalGoal) {
      if (
//            this.firstPosition.proofNumberNextEval < this.firstPosition.disproofNumberCurEval * 1.1 &&
          this.firstPosition.eval >= evalGoal + 200) {
        this.setEvalGoal(evalGoal + 200);
        nextUpdateEvalGoal = (long) (this.firstPosition.descendants * 1.2);
//          System.out.println(firstPosition.eval);
//          System.out.println("Eval goal " + evalGoal + " after " + this.firstPosition.descendants);
//          break;
      }
      if (
//            this.firstPosition.proofNumberCurEval > this.firstPosition.disproofNumberNextEval * 1.1 &&
          this.firstPosition.eval <= evalGoal - 200) {
        this.setEvalGoal(evalGoal - 200);
        nextUpdateEvalGoal = (long) (this.firstPosition.descendants * 1.2);
//          System.out.println(firstPosition.eval);
//          System.out.println("Eval goal " + evalGoal + " after " + this.firstPosition.descendants);
//          break;
      }
    }
  }
  
  public void setBoard(Board board) {
    int quickEval = evaluatorMidgame.evaluatePosition(board, 6, -6400, 6400);
    StoredBoard currentStored = new StoredBoard(
        board,
        quickEval,
        800);

//    StoredBoard currentStored = new StoredBoard(current, this.depthOneEval.eval(current), 800);
    super.addFirstPosition(currentStored);
    currentStored.descendants = evaluatorMidgame.getNVisited();
    this.evaluatorMidgame.resetNVisitedPositions();
    nextUpdateEvalGoal = (long) (currentStored.descendants * 1.1);
    setEvalGoal(((quickEval + 200100) / 200) * 200 - 200000);
  }
  
  public short evaluatePosition() {
    return evaluatePosition(Long.MAX_VALUE, Long.MAX_VALUE);
  }
  public void stop() {
    status = Status.KILLING;
  }
  
  public short evaluatePosition(long maxNVisited, long maxTimeMillis) {
    status = Status.RUNNING;
    long seenPositions = 0;
    long startTime = System.currentTimeMillis();
    HashMapVisitedPositions.PositionToImprove nextPos;
    int i = 0;
    while ((nextPos = nextPositionToImprove()) != null) {
      StoredBoard next = nextPos.board;
//      if (i++ % 1 == 0) {
//        System.out.println(this.firstPosition.proofNumberCurEval + " " + this.firstPosition.disproofNumberCurEval + " " + firstPosition.descendants);
//      }
      int nEmpties = next.getBoard().getEmptySquares();

      if (next != this.firstPosition && (
          next.proofNumberCurEval < Constants.PROOF_NUMBER_FOR_ENDGAME || next.disproofNumberNextEval < Constants.PROOF_NUMBER_FOR_ENDGAME ||
          nEmpties <= Constants.EMPTIES_FOR_FORCED_MIDGAME)) {
        this.evaluatorMidgame.resetNVisitedPositions();
        evaluatorMidgame.evaluatePosition(
            next.getBoard(), nEmpties - Constants.EMPTIES_FOR_ENDGAME - 1, nextPos.alpha, nextPos.beta);
        int curEval = evaluatorMidgame.evaluatePosition(
            next.getBoard(), nEmpties, nextPos.alpha, nextPos.beta);
        seenPositions = evaluatorMidgame.getNVisited() + 1;
        updateEndgame(nextPos, curEval);
        nEndgames++;
        nVisitedEndgames += seenPositions;
//        System.out.println(seenPositions);
      } else if (this.size < this.maxSize) {
        seenPositions = this.addPositions(next);
      } else {
//        System.out.println("HELLO!" + next.getBoard());
        this.evaluatorMidgame.resetNVisitedPositions();
        int curEval = evaluatorMidgame.evaluatePosition(
            next.getBoard(), 2, nextPos.alpha, nextPos.beta);
        int d;
        for (d = 4; evaluatorMidgame.getNVisited() < next.descendants * 2 && d < nEmpties; d += 2) {
          if (nEmpties - d < Constants.EMPTIES_FOR_ENDGAME) {
            d = nEmpties;
          }
          curEval = evaluatorMidgame.evaluatePosition(
            next.getBoard(), d, nextPos.alpha, nextPos.beta);
        }
        seenPositions = evaluatorMidgame.getNVisited() + 1;
        if (d >= next.getBoard().getEmptySquares()) {
          updateEndgame(nextPos, curEval);
        } else {
          nextPos.board.updateEval(curEval, this.getEvalGoalForBoard(nextPos.board));
          nextPos.board.updateFathers();
        }
      }
      for (StoredBoard b : nextPos.parents) {
        b.descendants += seenPositions;
      }
      updateEvalGoalIfNeeded();
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
    
//      System.out.println(next);

      if (this.firstPosition.isSolved()) {
        status = Status.SOLVED;
        break;
      }
      if (status == Status.KILLING) {
        status = Status.KILLED;
        break;
      }
      if (this.firstPosition.descendants > maxNVisited) {
        status = Status.STOPPED_POSITIONS;
        break;
      }
      if (System.currentTimeMillis() - startTime > maxTimeMillis) {
        status = Status.STOPPED_TIME;
        break;
      }
    }
//    System.out.println(this.size() + this.lastMovesEval.nVisited);
//    System.out.println("W" + weird + "/" + tot);
//    System.out.print("  " + wrong + "/" + tot + "  ");
//    System.out.println(-get(current).getEval());
    return (short) -firstPosition.getEval();
  }
  
  public int getEvalGoal() {
    return evalGoal;
  }

  public StoredBoard getFirstPosition() {
    return firstPosition;
  }

  public StoredBoard[] getEvaluations() {
    return firstPosition.children;
  }
}