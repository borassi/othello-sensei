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
  private final EvaluatorInterface evaluatorMidgame;

  public long nEndgames;
  public long nVisitedEndgames;
  
  int lower;
  int upper;
  
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
                       EvaluatorInterface evaluatorMidgame) {
    super(maxSize, arraySize);
    this.evaluatorMidgame = evaluatorMidgame;
    this.firstPosition = StoredBoard.initialStoredBoard(new Board(), 0, 0, 1);
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

  private int roundEval(int eval) {
    eval = Math.max(Math.min(eval, upper - 1), lower + 1);
    eval = ((eval + 6500) / 200) * 200 - 6400;
    return eval;
  }
  
  protected void setEvalGoal(int evalGoal) {
    assert(evalGoal >= lower && evalGoal <= upper);
    this.nextUpdateEvalGoal = (long) (this.firstPosition.descendants * 1.1);
    this.firstPosition.setEvalGoalRecursive(roundEval(evalGoal));
  }
  
  public void updateEvalGoalIfNeeded() {
    int evalGoal = firstPosition.getEvalGoal();
    if (this.firstPosition.getProofNumberNextEval() == 0 && evalGoal + 200 <= upper) {
      this.setEvalGoal(evalGoal + 200);
//        System.out.println("Eval goal forced " + this.firstPosition.getEvalGoal() + " after " + this.firstPosition.descendants);
    }
    if (this.firstPosition.getDisproofNumberNextEval() == 0 && evalGoal - 200 >= lower) {
      this.setEvalGoal(evalGoal - 200);
//        System.out.println("Eval goal forced " + this.firstPosition.getEvalGoal() + " after " + this.firstPosition.descendants);
    }
    if (this.firstPosition.descendants > nextUpdateEvalGoal) {
      if (this.firstPosition.getEval() >= evalGoal + 200 && evalGoal + 200 <= upper) {
        this.setEvalGoal(evalGoal + 200);
//        System.out.println("Eval goal " + this.firstPosition.getEvalGoal() + " after " + this.firstPosition.descendants);
      }
      if (this.firstPosition.getEval() <= evalGoal - 200 && evalGoal - 200 >= lower) {
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

  protected synchronized PositionToImprove nextPositionToImproveMidgame(
      StoredBoard father, boolean playerVariates,
      ArrayList<StoredBoard> parents) {
    parents.add(father);
    if (father.isLeaf()) {
      return new PositionToImprove(father, playerVariates, parents);
    }
    if (playerVariates) {
      return nextPositionToImproveMidgame(father.bestChildMidgamePlayerVariates(), !playerVariates, parents);
    } else {
      return nextPositionToImproveMidgame(father.bestChildMidgameOpponentVariates(), !playerVariates, parents);
    }
  }

  private boolean positionToImproveAndEvalGoalAreOk(StoredBoard position, boolean playerVariates) {
    int evalGoal = position.evalGoal;
    int posUpper = position.playerIsStartingPlayer ? upper : -lower;
    int posLower = position.playerIsStartingPlayer ? lower : -upper;

    if (playerVariates) {
      if (evalGoal >= posUpper) {
        throw new AssertionError(
            "Player should not try to prove that [value > ] evalGoal (" + evalGoal
            + ") >= upper (" + posUpper + ") in board\n" + position.getBoard());
      }      
    } else {
      if (evalGoal <= posLower) {
        throw new AssertionError(
            "Opponent should not try to prove that [value < ] evalGoal (" + evalGoal
            + ") <= lower (" + posLower + ") in board\n" + position.getBoard());
      }
    }
    return true;
  }
  
  protected synchronized PositionToImprove nextPositionToImproveEndgame(
      StoredBoard father, boolean playerVariates,
      ArrayList<StoredBoard> parents) {
    assert positionToImproveAndEvalGoalAreOk(father, playerVariates);
    parents.add(father);
    if (father.isLeaf()) {
      return new PositionToImprove(father, playerVariates, parents);
    }
    if (playerVariates) {
      return nextPositionToImproveEndgame(father.bestChildPlayerVariates(), !playerVariates, parents);
    } else {
      return nextPositionToImproveEndgame(father.bestChildOpponentVariates(), !playerVariates, parents);
    }
  }

  protected PositionToImprove nextPositionToImprove() {
    ArrayList<StoredBoard> parents = new ArrayList<>();
    StoredBoard firstPositionLocal = this.firstPosition;
    boolean playerVariates;
    
    if (!firstPositionLocal.isPartiallySolved()) {
      return nextPositionToImproveMidgame(
          firstPositionLocal,
          this.firstPosition.minLogDerivativePlayerVariates < this.firstPosition.minLogDerivativeOpponentVariates,
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
    return nextPositionToImproveEndgame(firstPositionLocal, playerVariates, parents);
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

  void setFirstPosition(Board b) {
    setFirstPosition(b.getPlayer(), b.getOpponent());
  }

  void setFirstPosition(long player, long opponent) {
    empty();
    int quickEval = evaluatorMidgame.evaluatePosition(player, opponent, 2, lower, upper);
    StoredBoard boardStored = StoredBoard.initialStoredBoard(
        player, opponent, quickEval, roundEval(quickEval),
        evaluatorMidgame.getNVisited() + 1);
    add(boardStored);
    this.firstPosition = boardStored;
  }
  
  public void addChildren(PositionToImprove fatherPos) {
    StoredBoard father = fatherPos.board;
    GetMovesCache mover = new GetMovesCache();
    long player = father.getPlayer();
    long opponent = father.getOpponent();
    
    long curMoves = mover.getMoves(father.getPlayer(), father.getOpponent());
    int nMoves = Long.bitCount(curMoves);
    
    if (nMoves == 0) {
      if (mover.getNMoves(opponent, player) == 0) {
        father.setSolved(BitPattern.getEvaluationGameOver(player, opponent));
        assert father.isAllOK();
        return;
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
    fatherPos.addVisitedPositions(addedPositions);
    father.setChildren(children);
    assert father.isAllOK();
  }
  
  public void solvePosition(PositionToImprove position, int nEmpties) {
    StoredBoard board = position.board;
    int alpha;
    int beta;
//    if (firstPosition.isPartiallySolved()) {
      alpha = position.getAlpha();
      beta = position.getBeta();
//    } else {
//      alpha = -6400;
//      beta = 6400;
//    }
    int eval = evaluatorMidgame.evaluatePosition(
        board.getPlayer(), board.getOpponent(), nEmpties, alpha, beta);
    if (eval <= alpha) {
      // Tricky but probably correct.
      board.setUpper(eval);
    } else if (eval >= beta) {
      board.setLower(eval);
    } else {
      board.setSolved(eval);
    }
    long seenPositions = evaluatorMidgame.getNVisited();
    nEndgames++;
    nVisitedEndgames += seenPositions;
    position.addVisitedPositions(seenPositions);
  }
  
  public void deepenPosition(PositionToImprove position, int nEmpties) {
    StoredBoard board = position.board;
    int curEval = evaluatorMidgame.evaluatePosition(
          board.getPlayer(), board.getOpponent(), 2, -6400, 6400);
    int d;
    long seenPositions = 0;
    for (d = 4; seenPositions < board.descendants * 2; d += 2) {
      if (nEmpties - d < Constants.EMPTIES_FOR_ENDGAME) {
        solvePosition(position, nEmpties);
        return;
      }
      curEval = evaluatorMidgame.evaluatePosition(
          board.getPlayer(), board.getOpponent(), d, -6400, 6400);
      seenPositions += evaluatorMidgame.getNVisited();
    }
    board.setEval(curEval);
    position.addVisitedPositions(seenPositions);
  }
  
  public boolean isSolved() {
    return this.firstPosition.isSolved() || this.firstPosition.getUpper() <= lower || this.firstPosition.getLower() >= upper;
  }
  
  public short evaluatePosition(
      Board board, int lower, int upper, long maxNVisited, long maxTimeMillis, boolean reset) {
    status = Status.RUNNING;
    this.lower = Math.floorDiv(lower, 200) * 200;
    this.upper = (int) Math.ceil(upper / 200.0) * 200;
    assert(this.lower < this.upper);

    if (firstPosition.getPlayer() != board.getPlayer() ||
        firstPosition.getOpponent() != board.getOpponent()) {
      StoredBoard boardStored = null;
      
      if (!reset) {
        boardStored = get(board.getPlayer(), board.getOpponent());
        firstPosition = boardStored;
      }
      if (boardStored == null) {
        setFirstPosition(board.getPlayer(), board.getOpponent());
      }
      setEvalGoal(Math.max(lower, Math.min(upper, firstPosition.getEval())));
    }
    if (firstPosition.isSolved()) {
      status = Status.SOLVED;
      return (short) firstPosition.getEval();
    }

    long startTime = System.currentTimeMillis();
    HashMapVisitedPositions.PositionToImprove nextPos;
    int i = 0;
    while (true) {
      nextPos = nextPositionToImprove();
      StoredBoard next = nextPos.board;
      int nEmpties = 64 - Long.bitCount(next.getPlayer() | next.getOpponent());

      if (next != this.firstPosition
          && nEmpties <= Constants.EMPTIES_FOR_FORCED_MIDGAME + 3
          && (
          (next.getProofNumberCurEval() < Constants.PROOF_NUMBER_FOR_ENDGAME && next.getEval() > next.getEvalGoal() + 2000) ||
          (next.getDisproofNumberCurEval() < Constants.PROOF_NUMBER_FOR_ENDGAME && next.getEval() < next.getEvalGoal() - 2000) ||
          nEmpties <= Constants.EMPTIES_FOR_FORCED_MIDGAME)) {
        solvePosition(nextPos, nEmpties);
      } else if (this.size < this.maxSize) {
        addChildren(nextPos);
      } else {
        deepenPosition(nextPos, nEmpties);
      }
      updateEvalGoalIfNeeded();

      if (isSolved()) {
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