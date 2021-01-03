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

import board.Board;
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
    this.firstPosition = StoredBoard.initialStoredBoard(new Board());
  }
  
  public float getEval() {
    return get(firstPosition.getBoard()).getEval();
  }

  public long getNVisited() {
    return this.firstPosition.getDescendants();
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
    this.nextUpdateEvalGoal = (long) (this.firstPosition.getDescendants() * 1.1);
    this.firstPosition.setEvalGoalRecursive(roundEval(evalGoal));
  }
  
  public void updateEvalGoalIfNeeded() {
    int evalGoal = firstPosition.getEvalGoal();
    while (this.firstPosition.probStrictlyGreater >= 1-1E-8 && evalGoal + 200 <= upper) {
      this.setEvalGoal(evalGoal + 200);
      evalGoal = firstPosition.getEvalGoal();
//        System.out.println("Eval goal forced " + this.firstPosition.getEvalGoal() + " after " + this.firstPosition.descendants);
    }
    while (this.firstPosition.probGreaterEqual <= 1E-8 && evalGoal - 200 >= lower) {
      this.setEvalGoal(evalGoal - 200);
      evalGoal = firstPosition.getEvalGoal();
//        System.out.println("Eval goal forced " + this.firstPosition.getEvalGoal() + " after " + this.firstPosition.descendants);
    }
    if (this.firstPosition.getDescendants() > nextUpdateEvalGoal) {
      while (this.firstPosition.probStrictlyGreater >= 0.9 && evalGoal + 200 <= upper) {
        this.setEvalGoal(evalGoal + 200);
        evalGoal = firstPosition.getEvalGoal();
//        System.out.println("Eval goal " + this.firstPosition.getEvalGoal() + " after " + this.firstPosition.descendants);
      }
      while (this.firstPosition.probGreaterEqual <= 0.1 && evalGoal - 200 >= lower) {
        this.setEvalGoal(evalGoal - 200);
        evalGoal = firstPosition.getEvalGoal();
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

  public boolean nextPositionGreaterEqual() {
    if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {
      if (this.firstPosition.isSolved()) {
        return firstPosition.extraInfo.minDeltaDisproofStrictlyGreaterBasic > firstPosition.extraInfo.minDeltaProofGreaterEqualBasic;
      }
    }
    if (firstPosition.getEvalGoal() >= upper) {
      return true;
    } else if (firstPosition.getEvalGoal() <= lower) {
      return false;
    } else if (!this.firstPosition.isPartiallySolved()) {
      return this.firstPosition.maxLogDerivativeProbStrictlyGreater < this.firstPosition.maxLogDerivativeProbGreaterEqual;
    }
    return this.firstPosition.disproofNumberStrictlyGreater < this.firstPosition.proofNumberGreaterEqual;      
  }

  protected StoredBoardBestDescendant nextPositionToImprove() {
    return StoredBoardBestDescendant.bestDescendant(firstPosition, nextPositionGreaterEqual());
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
    firstPosition = StoredBoard.initialStoredBoard(player, opponent);
    firstPosition.eval = quickEval;
    firstPosition.descendants = evaluatorMidgame.getNVisited() + 1;
    firstPosition.setEvalGoalRecursive(roundEval(quickEval));
    
    add(firstPosition);
  }
  
  public void addChildren(StoredBoardBestDescendant fatherPos) {
    StoredBoard father = fatherPos.board;
    father.addChildren(this);
    if (father.children == null) {
      // Game over.
      assert father.isSolved();
      return;
    }
    int depth = 2;
    long addedPositions = 0;
    for (StoredBoard child : father.children) {
      if (child.descendants == 0) {
        int eval = evaluatorMidgame.evaluatePosition(child.getPlayer(), child.getOpponent(), depth, -6400, 6400);
        child.descendants = evaluatorMidgame.getNVisited() + 1;
        child.setEval(eval);
        addedPositions += child.descendants;
      }
    }
    fatherPos.addVisitedPositions(addedPositions);
    father.updateFathers();
    assert father.isAllOK();
  }
  
  public void solvePosition(StoredBoardBestDescendant position, int nEmpties) {
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
    long seenPositions = evaluatorMidgame.getNVisited() + 1;
    nEndgames++;
    nVisitedEndgames += seenPositions;
    position.addVisitedPositions(seenPositions);
    if (eval <= alpha) {
      // Tricky but probably correct.
      board.setUpper(eval);
    } else if (eval >= beta) {
      board.setLower(eval);
    } else {
      board.setSolved(eval);
    }
  }
  
  public void deepenPosition(StoredBoardBestDescendant position, int nEmpties) {
    StoredBoard board = position.board;
    int curEval = evaluatorMidgame.evaluatePosition(
          board.getPlayer(), board.getOpponent(), 2, -6400, 6400);
    int d;
    long seenPositions = 0;
    for (d = 4; seenPositions < board.getDescendants() * 2; d += 2) {
      if (nEmpties - d < Constants.EMPTIES_FOR_ENDGAME) {
        solvePosition(position, nEmpties);
        return;
      }
      curEval = evaluatorMidgame.evaluatePosition(
          board.getPlayer(), board.getOpponent(), d, -6400, 6400);
      seenPositions += evaluatorMidgame.getNVisited();
    }
    position.addVisitedPositions(seenPositions);
    board.setEval(curEval);
    board.updateFathers();
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
    if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {
      this.firstPosition.setIsFinished(false);
    }

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

    long startTime = System.currentTimeMillis();
    int i = 0;
    StoredBoardBestDescendant nextPos;
    for (nextPos = nextPositionToImprove();
         !nextPos.isNull();
         nextPos = nextPositionToImprove()) {
      StoredBoard next = nextPos.board;
      int nEmpties = 64 - Long.bitCount(next.getPlayer() | next.getOpponent());

      if (next != this.firstPosition && next.toBeSolved()) {
        solvePosition(nextPos, nEmpties);
      } else if (this.size < this.maxSize) {
        addChildren(nextPos);
      } else {
        deepenPosition(nextPos, nEmpties);
      }
      updateEvalGoalIfNeeded();
      if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {
        if (this.firstPosition.isSolved()) {
          this.firstPosition.setIsFinished(true);
        }
      }

      if (status == Status.KILLING) {
        status = Status.KILLED;
        break;
      }
      if (this.firstPosition.getDescendants() > maxNVisited) {
        status = Status.STOPPED_POSITIONS;
        break;
      }
      if (System.currentTimeMillis() - startTime > maxTimeMillis) {
        status = Status.STOPPED_TIME;
        break;
      }
    }
    if (nextPos.isNull()) {
      status = Status.SOLVED;
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