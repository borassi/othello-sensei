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
import board.GetMovesCache;
import board.PossibleMovesFinderImproved;
import constants.Constants;
import java.util.ArrayList;

public class EvaluatorMCTS extends HashMapVisitedPositions {
  private final EvaluatorMidgame evaluatorMidgame;

  public long nEndgames;
  public long nVisitedEndgames;
  
  public int lower;
  public int upper;
  
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
    return get(super.firstPosition.getBoard()).getEval();
  }

  public long getNVisited() {
    return this.firstPosition.descendants;
  }
  
  public long getNStored() {
    return this.size;
  }

//  int[] deltas = new int[Constants.N_SAMPLES];
//  protected int addPositions(StoredBoard father) {
//    long player = father.getPlayer();
//    long opponent = father.getOpponent();
//    long[] moves = possibleMovesFinder.possibleMovesAdvanced(player, opponent);
////     TODO
////    this.depthOneEval.setup(player, opponent);
////    this.depthOneEval.invert();
//    int addedPositions = 0;
//    if (moves.length == 0) {
//      StoredBoard b = getFull(player, opponent);
//      b.setSolved(BitPattern.getEvaluationGameOver(player, opponent));
//    } else {
//      StoredBoard[] children = new StoredBoard[moves.length];
//      for (int i = 0; i < moves.length; i++) {
//        long move = moves[i];
//        Board childBoard = father.getBoard().move(move);
//        int depth = 2;//childBoard.getEmptySquares() <= Constants.EMPTIES_FOR_FORCED_MIDGAME + 2 ? 2 : 2;
//        evaluatorMidgame.resetNVisitedPositions();
//        int result = evaluatorMidgame.evaluatePosition(childBoard, depth, -6400, 6400);
//        long visited = evaluatorMidgame.getNVisited() + 1;
//        children[i] = new StoredBoard(childBoard, result);
//        children[i].descendants += visited;
//        addedPositions += visited;
//      }
//      super.add(children, father);
//    }
//    return addedPositions;
//  }
  
  public long addChildren(StoredBoard father) { 
    GetMovesCache mover = new GetMovesCache();
    long player = father.getPlayer();
    long opponent = father.getOpponent();
    
    long curMoves = mover.getMoves(father.getPlayer(), father.getOpponent());
    int nMoves = Long.bitCount(curMoves);
    
    if (nMoves == 0) {
      if (mover.getNMoves(opponent, player) == 0) {
        father.setSolved(BitPattern.getEvaluationGameOver(player, opponent));
        return 0;
      } else {
        ++nMoves;
      }
    }
    
    StoredBoard[] children = new StoredBoard[nMoves];
    long addedPositions = 0;

    for (int i = 0; i < nMoves; ++i) {
      int move = Long.numberOfTrailingZeros(curMoves);
      long moveBit = 1L << move;
      curMoves = curMoves & (~moveBit);
      long flip = move == 64 ? 0 : mover.getFlip(move) & (opponent | moveBit);
      long newPlayer = opponent & ~flip;
      long newOpponent = player | flip;
      StoredBoard child = this.get(newPlayer, newOpponent);
      if (child == null) {
        int depth = 2;
        evaluatorMidgame.resetNVisitedPositions();
        int eval = evaluatorMidgame.evaluatePosition(newPlayer, newOpponent, depth, -6400, 6400);
        long visited = evaluatorMidgame.getNVisited() + 1;
        child = StoredBoard.childStoredBoard(newPlayer, newOpponent, father, eval, visited);
        addedPositions += visited;
        super.add(child);
      } else {
        child.addFather(father);
      }
      children[i] = child;
    }
    father.setChildren(children);
    return addedPositions;
  }
  
  protected void updateEndgame(HashMapVisitedPositions.PositionToImprove pos, int eval) {
    StoredBoard b = pos.board;
    if (eval <= pos.alpha) {
      // Tricky but probably correct.
      b.setUpper(eval);
    } else if (eval >= pos.beta) {
      b.setLower(eval);
    } else {
      b.setSolved(eval);
    }
    b.updateFathers();
  }

  private int roundEval(int eval) {
    eval = Math.max(Math.min(eval, upper - 1), lower + 1);
    eval = ((eval + 6500) / 200) * 200 - 6400;
    return eval;
  }
  
  protected void setEvalGoal(int evalGoal) {
//    if (evalGoal <= lower || evalGoal >= upper) {
//      return;
//    }
    assert(evalGoal >= -6400 && evalGoal <= 6400);
    this.nextUpdateEvalGoal = (long) (this.firstPosition.descendants * 1.1);
    this.firstPosition.setEvalGoalRecursive(roundEval(evalGoal));
  }
  
  public void updateEvalGoalIfNeeded() {
    int evalGoal = firstPosition.getEvalGoal();
    if (this.firstPosition.getProofNumberNextEval() == 0) {
      this.setEvalGoal(evalGoal + 200);
//        System.out.println("Eval goal forced " + this.firstPosition.getEvalGoal() + " after " + this.firstPosition.descendants);
    }
    if (this.firstPosition.getDisproofNumberNextEval() == 0) {
      this.setEvalGoal(evalGoal - 200);
//        System.out.println("Eval goal forced " + this.firstPosition.getEvalGoal() + " after " + this.firstPosition.descendants);
    }
    if (this.firstPosition.descendants > nextUpdateEvalGoal) {
      if (this.firstPosition.getEval() >= evalGoal + 200) {
        this.setEvalGoal(evalGoal + 200);
//        System.out.println("Eval goal " + this.firstPosition.getEvalGoal() + " after " + this.firstPosition.descendants);
      }
      if (this.firstPosition.getEval() <= evalGoal - 200) {
        this.setEvalGoal(evalGoal - 200);
//        System.out.println("Eval goal " + this.firstPosition.getEvalGoal() + " after " + this.firstPosition.descendants);
      }
    }
  }
  
  public void reset() {
    nEndgames = 0;
    nVisitedEndgames = 0;
  }
  
  public void setBoard(Board board) {
    setBoard(board, -6400, 6400);
  }
  public void setBoard(Board board, int lower, int upper) {
    setBoard(board, lower, upper, true);
  }
  
  public synchronized void setBoard(Board board, int lower, int upper, boolean reset) {
    assert(lower < upper);
    this.lower = lower;
    this.upper = upper;
    evaluatorMidgame.resetNVisitedPositions();
    int quickEval = evaluatorMidgame.evaluatePosition(board, 2, -6400, 6400);
    StoredBoard currentStored = StoredBoard.initialStoredBoard(
        board.getPlayer(), board.getOpponent(), quickEval, roundEval(quickEval),
        evaluatorMidgame.getNVisited());

    if (reset) {
      reset();
    }
//    StoredBoard currentStored = new StoredBoard(current, this.depthOneEval.eval(current), 800);
    super.addFirstPosition(currentStored);
    this.evaluatorMidgame.resetNVisitedPositions();
    setEvalGoal(quickEval);
  }
  
  public short evaluatePosition() {
    return evaluatePosition(Long.MAX_VALUE, Long.MAX_VALUE);
  }
  public void stop() {
    status = Status.KILLING;
  }
  
  protected synchronized PositionToImprove nextPositionToImprove() {
    ArrayList<StoredBoard> parents = new ArrayList<>();
    StoredBoard positionToEvaluateLocal = this.firstPosition;
    boolean playerVariates;
    if (!positionToEvaluateLocal.isPartiallySolved()) {
      return nextPositionToImproveMidgame(
          positionToEvaluateLocal, this.firstPosition.minLogDerivativePlayerVariates < this.firstPosition.minLogDerivativeOpponentVariates, true, parents);
    }
    if (firstPosition.getEvalGoal() >= upper) {
      playerVariates = false;
    } else if (firstPosition.getEvalGoal() <= lower) {
      playerVariates = true;
    } else {
      playerVariates = 
          Math.max(positionToEvaluateLocal.getProofNumberCurEval(), positionToEvaluateLocal.getDisproofNumberNextEval()) >
          Math.max(positionToEvaluateLocal.getProofNumberNextEval(), positionToEvaluateLocal.getDisproofNumberCurEval());      
    }
    
//    if (firstPosition.getEvalGoal() >= upper || positionToEvaluateLocal.getProofNumberPlayerVariates() == 0) {
//      playerVariates = true;
//    } else if (firstPosition.getEvalGoal() <= lower || positionToEvaluateLocal.getDisproofNumberOpponentVariates() == Double.POSITIVE_INFINITY) {
//      playerVariates = true;
//    } else {
//      playerVariates = Math.random() > 0.5;
//          Math.max(positionToEvaluateLocal.getProofNumberOpponentVariates(), positionToEvaluateLocal.getDisproofNumberOpponentVariates()) >
//          Math.max(positionToEvaluateLocal.getProofNumberPlayerVariates(), positionToEvaluateLocal.getDisproofNumberPlayerVariates());      
//    }
    return nextPositionToImproveEndgame(positionToEvaluateLocal, playerVariates, true, parents);
  }

  public synchronized short evaluatePosition(long maxNVisited, long maxTimeMillis) {
    status = Status.RUNNING;
    long seenPositions = 0;
    long startTime = System.currentTimeMillis();
    HashMapVisitedPositions.PositionToImprove nextPos;
    int i = 0;
    while (true) {
      nextPos = nextPositionToImprove();
      StoredBoard next = nextPos.board;
      int nEmpties = next.getBoard().getEmptySquares();

      // TODO: FIX
      if (next != this.firstPosition && (
          (next.getProofNumberCurEval() < Constants.PROOF_NUMBER_FOR_ENDGAME && next.getEval() > next.getEvalGoal() + 2000) ||
          (next.getDisproofNumberCurEval() < Constants.PROOF_NUMBER_FOR_ENDGAME && next.getEval() < next.getEvalGoal() - 2000) ||
          nEmpties <= Constants.EMPTIES_FOR_FORCED_MIDGAME)
          && nEmpties <= Constants.EMPTIES_FOR_FORCED_MIDGAME + 3) {
        this.evaluatorMidgame.resetNVisitedPositions();
        int curEval = evaluatorMidgame.evaluatePosition(
            next.getBoard(), nEmpties, nextPos.alpha, nextPos.beta);
        seenPositions = evaluatorMidgame.getNVisited() + 1;
        updateEndgame(nextPos, curEval);
        nEndgames++;
        nVisitedEndgames += seenPositions;
      } else if (this.size < this.maxSize) {
        seenPositions = this.addChildren(next);
      } else {
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
          nextPos.board.setEval(curEval);
          nextPos.board.updateFathers();
        }
      }
      for (StoredBoard b : nextPos.parents) {
        b.descendants += seenPositions;
      }
      updateEvalGoalIfNeeded();

      if (this.firstPosition.getUpper() <= lower || this.firstPosition.getLower() >= upper || this.firstPosition.isSolved()) {
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
    return (short) -firstPosition.getEval();
  }
  
  public int getEvalGoal() {
    return firstPosition.getEvalGoal();
  }

  public StoredBoard getFirstPosition() {
    return firstPosition;
  }

  public StoredBoard[] getEvaluations() {
    return firstPosition.getChildren();
  }
}