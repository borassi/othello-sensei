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
import constants.Constants;
import java.util.ArrayList;

public class EvaluatorMCTS extends HashMapVisitedPositions {
  private final EvaluatorMidgame evaluatorMidgame;

  public long nEndgames;
  public long nVisitedEndgames;
  
  private int lower;
  private int upper;
  
  public enum Status {
    NONE,
    RUNNING,
    STOPPED_TIME,
    STOPPED_POSITIONS,
    SOLVED,
    KILLING,
    KILLED,
  }
  private Status status = Status.KILLED;
  private long nextUpdateEvalGoal;

  public EvaluatorMCTS(int maxSize, int arraySize) {
    this(maxSize, arraySize, new EvaluatorMidgame());
  }

  public EvaluatorMCTS(int maxSize, int arraySize,
                       EvaluatorMidgame evaluatorMidgame) {
    super(maxSize, arraySize);
//    this.depthOneEval = depthOneEvaluator;
    this.evaluatorMidgame = evaluatorMidgame;
    this.firstPosition = StoredBoard.initialStoredBoard(0, 0, 0, 0, 0);
  }
  
  public float getEval() {
    return get(firstPosition.getBoard()).getEval();
  }

  public long getNVisited() {
    return this.firstPosition.descendants;
  }
  
  public long getNStored() {
    return this.size;
  }
  
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
        if (father.getEvalGoal() != -child.getEvalGoal()) {
          child.setEvalGoalRecursive(-father.getEvalGoal());
        }
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
  
  public void stop() {
    status = Status.KILLING;
  }

  protected PositionToImprove nextPositionToImprove() {
    ArrayList<StoredBoard> parents = new ArrayList<>();
    StoredBoard firstPositionLocal = this.firstPosition;
    boolean playerVariates;
    
    if (!firstPositionLocal.isPartiallySolved()) {
      return nextPositionToImproveMidgame(
          firstPositionLocal,
          this.firstPosition.minLogDerivativePlayerVariates < this.firstPosition.minLogDerivativeOpponentVariates,
          true, 
          parents);
    }
    if (firstPosition.getEvalGoal() >= upper) {
      playerVariates = false;
    } else if (firstPosition.getEvalGoal() <= lower) {
      playerVariates = true;
    } else {
      playerVariates = 
          Math.max(firstPositionLocal.getProofNumberCurEval(), firstPositionLocal.getDisproofNumberNextEval()) >
          Math.max(firstPositionLocal.getProofNumberNextEval(), firstPositionLocal.getDisproofNumberCurEval());      
    }
    return nextPositionToImproveEndgame(firstPositionLocal, playerVariates, true, parents);
  }

  public short evaluatePosition(Board board) {
    return evaluatePosition(board, -6400, 6400, Long.MAX_VALUE, Long.MAX_VALUE, true);
  }

  public short evaluatePosition(Board board, long maxNVisited, long maxTimeMillis) {
    return evaluatePosition(board, -6400, 6400, maxNVisited, maxTimeMillis, true);
  }
  
  public Status getStatus() {
    return status;
  }

  public short evaluatePosition(
      Board board, int lower, int upper, long maxNVisited, long maxTimeMillis, boolean reset) {
    status = Status.RUNNING;
    assert(lower < upper);
    this.lower = lower;
    this.upper = upper;
    evaluatorMidgame.resetNVisitedPositions();

    if (firstPosition.getPlayer() != board.getPlayer() ||
        firstPosition.getOpponent() != board.getOpponent()) {
      StoredBoard boardStored = get(board.getPlayer(), board.getOpponent());
      
      if (reset || boardStored == null) {
        empty();
        int quickEval = evaluatorMidgame.evaluatePosition(board, 2, -6400, 6400);
        evaluatorMidgame.resetNVisitedPositions();
        boardStored = StoredBoard.initialStoredBoard(
            board.getPlayer(), board.getOpponent(), quickEval, roundEval(quickEval),
            evaluatorMidgame.getNVisited());
        add(boardStored);
      }
      firstPosition = boardStored;
      setEvalGoal(boardStored.getEval());
    }
//    System.out.println("Evaluating\n" + this.firstPosition.getBoard());
//    System.out.println(this.firstPosition.getEval() + " " + this.firstPosition.getLower() + " " + this.firstPosition.getUpper());
    if (firstPosition.isSolved()) {
//      System.out.println("Skipping because it is solved.");
      status = Status.SOLVED;
      return (short) firstPosition.getEval();
    }

    long seenPositions = 0;
    long startTime = System.currentTimeMillis();
    HashMapVisitedPositions.PositionToImprove nextPos;
    int i = 0;
    while (true) {
      nextPos = nextPositionToImprove();
      StoredBoard next = nextPos.board;
      int nEmpties = 64 - Long.bitCount(next.getPlayer() | next.getOpponent());

      // TODO: FIX
      if (next != this.firstPosition && (
          (next.getProofNumberCurEval() < Constants.PROOF_NUMBER_FOR_ENDGAME && next.getEval() > next.getEvalGoal() + 2000) ||
          (next.getDisproofNumberCurEval() < Constants.PROOF_NUMBER_FOR_ENDGAME && next.getEval() < next.getEvalGoal() - 2000) ||
          nEmpties <= Constants.EMPTIES_FOR_FORCED_MIDGAME)
          && nEmpties <= Constants.EMPTIES_FOR_FORCED_MIDGAME + 3) {
        this.evaluatorMidgame.resetNVisitedPositions();
        int curEval = evaluatorMidgame.evaluatePosition(
            next.getPlayer(), next.getOpponent(), nEmpties, nextPos.alpha, nextPos.beta);
        seenPositions = evaluatorMidgame.getNVisited() + 1;
        updateEndgame(nextPos, curEval);
        nEndgames++;
        nVisitedEndgames += seenPositions;
      } else if (this.size < this.maxSize) {
        seenPositions = this.addChildren(next);
      } else {
        this.evaluatorMidgame.resetNVisitedPositions();
        int curEval = evaluatorMidgame.evaluatePosition(
            next.getPlayer(), next.getOpponent(), 2, nextPos.alpha, nextPos.beta);
        int d; // = nEmpties;
        for (d = 4; evaluatorMidgame.getNVisited() < next.descendants * 2 && d < nEmpties; d += 2) {
          if (nEmpties - d < Constants.EMPTIES_FOR_ENDGAME) {
            d = nEmpties;
          }
          curEval = evaluatorMidgame.evaluatePosition(
            next.getPlayer(), next.getOpponent(), d, nextPos.alpha, nextPos.beta);
        }
        seenPositions = evaluatorMidgame.getNVisited() + 1;
        if (d >= nEmpties) {
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