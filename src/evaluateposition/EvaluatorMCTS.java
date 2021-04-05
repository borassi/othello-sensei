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

public class EvaluatorMCTS extends HashMapVisitedPositions {
  private final EvaluatorMidgame evaluatorMidgame;

  public long nEndgames;
  public long nVisitedEndgames;
  private long maxNVisited;
  private long stopTimeMillis;
  
  int lower = -6300;
  int upper = 6300;
  
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
    this.evaluatorMidgame = evaluatorMidgame;
    this.firstPosition = StoredBoard.initialStoredBoard(new Board());
  }
  
  public float getEval() {
    return firstPosition.getEval();
  }

  public long getNVisited() {
    return this.firstPosition.getDescendants();
  }
  
  public long getNStored() {
    return this.size;
  }

  int roundEval(int eval) {
    assert (lower + 6400) % 200 == 100;
    assert (upper + 6400) % 200 == 100;
    assert lower >= -6300 && lower <= 6300;
    assert upper >= -6300 && upper <= 6300;
    if (eval <= lower) {
      return lower - 100;
    }
    if (eval > upper) {
      return upper + 100;
    }
    return ((eval + 6499) / 200) * 200 - 6400;
  }
  
  protected void setEvalGoal(int evalGoal) {
    evalGoal = roundEval(evalGoal);
    assert(evalGoal >= lower - 100 && evalGoal <= upper + 100);
    assert evalGoal % 200 == 0;
    this.nextUpdateEvalGoal = (long) (this.firstPosition.getDescendants() * 1.1);
    this.firstPosition.setEvalGoalRecursive(roundEval(evalGoal));
  }
  
  public void updateEvalGoalIfNeeded() {
    int evalGoal = firstPosition.getEvalGoal();
    if (this.firstPosition.probStrictlyGreater >= 1-1E-8 && evalGoal <= upper) {
      this.setEvalGoal(evalGoal + 200);
//        System.out.println("Eval goal forced " + this.firstPosition.getEvalGoal() + " after " + this.firstPosition.descendants);
    }
    if (this.firstPosition.probGreaterEqual <= 1E-8 && evalGoal >= lower) {
      this.setEvalGoal(evalGoal - 200);
//        System.out.println("Eval goal forced " + this.firstPosition.getEvalGoal() + " after " + this.firstPosition.descendants);
    }
    if (this.firstPosition.getDescendants() > nextUpdateEvalGoal) {
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
    if (firstPosition.getEvalGoal() > upper || firstPosition.getEvalGoal() >= firstPosition.upper) {
      return true;
    } else if (firstPosition.getEvalGoal() <= lower || firstPosition.getEvalGoal() <= firstPosition.lower) {
      return false;
    }
    if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {
      if (this.firstPosition.isSolved()) {
        return firstPosition.extraInfo.minDisproofStrictlyGreater - firstPosition.extraInfo.minDisproofStrictlyGreaterVar <
            firstPosition.extraInfo.minProofGreaterEqual - firstPosition.extraInfo.minProofGreaterEqualVar;
      }
    }
    if (firstPosition.isLeaf()) {
      return true;
    }
    if (this.firstPosition.probGreaterEqual == 1 && this.firstPosition.probStrictlyGreater == 0) {
      return this.firstPosition.proofNumberGreaterEqual > this.firstPosition.disproofNumberStrictlyGreater;
    }
    return this.firstPosition.probGreaterEqual < 1 - this.firstPosition.probStrictlyGreater;
  }

  protected StoredBoardBestDescendant nextPositionToImprove() {
    if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {
      if (this.firstPosition.isSolved()) {
        this.firstPosition.setIsFinished(true);
      }
    }

    if (Constants.APPROX_ONLY && this.firstPosition.probGreaterEqual > 1 - 0.02 && this.firstPosition.probStrictlyGreater < 0.02) {
      status = Status.SOLVED;
      return null;
    }
    if (status == Status.KILLING) {
      status = Status.KILLED;
      return null;
    }
    if (this.firstPosition.getDescendants() > maxNVisited) {
      status = Status.STOPPED_POSITIONS;
      return null;
    }
    if (System.currentTimeMillis() > stopTimeMillis) {
      status = Status.STOPPED_TIME;
      return null;
    }
    StoredBoardBestDescendant result = StoredBoardBestDescendant.bestDescendant(firstPosition, this.nextPositionGreaterEqual());
    return result;
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
    firstPosition.addDescendants(evaluatorMidgame.getNVisited() + 1);
    firstPosition.setEvalGoalRecursive(roundEval(quickEval));
    
    add(firstPosition);
  }
  
  public void addChildren(StoredBoardBestDescendant position) {
    addChildren(position, position.board.getBoard().getEmptySquares());
  }
  
  public void addChildren(StoredBoardBestDescendant position, int nEmpties) {
    StoredBoard board = position.board;
    board.addChildren(this);
    if (board.children == null) {
      // Game over.
      assert board.isSolved();
      return;
    }
    int depth = nEmpties < 24 ? 2 : 3;
    long addedPositions = 0;
    for (StoredBoard child : board.children) {
      if (child.getDescendants() == 0) {
        int eval = evaluatorMidgame.evaluatePosition(child.getPlayer(), child.getOpponent(), depth, -6400, 6400);
        child.addDescendants(evaluatorMidgame.getNVisited() + 1);
        child.setEval(eval);
        addedPositions += child.getDescendants();
      }
    }
    position.updateDescendants(addedPositions);
  }
  
  public void solvePosition(StoredBoardBestDescendant position, int nEmpties) {
    StoredBoard board = position.board;
    int alpha = position.getAlpha();
    int beta = position.getBeta();
    int eval = evaluatorMidgame.evaluatePosition(
        board.getPlayer(), board.getOpponent(), nEmpties, alpha, beta);
    long seenPositions = evaluatorMidgame.getNVisited() + 1;
    nEndgames++;
    nVisitedEndgames += seenPositions;
    constant = Math.max(0, constant + 0.05 * (10000 - seenPositions));
    position.updateDescendants(seenPositions);
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
    position.updateDescendants(seenPositions);
    board.setEval(curEval);
  }
  
  public boolean isSolved() {
    return this.firstPosition.isSolved() || this.firstPosition.getUpper() <= lower || this.firstPosition.getLower() >= upper;
  }

  private double constant = 0;
  public boolean toBeSolved(StoredBoard b) {
//    System.out.println(constant);
    return b.nEmpties <= Constants.EMPTIES_FOR_FORCED_MIDGAME + 3
        && (
        (b.proofNumberGreaterEqual < constant && b.getEval() > b.getEvalGoal() + 1000) ||
        (b.disproofNumberStrictlyGreater < constant && b.getEval() < b.getEvalGoal() - 1000) ||
        b.nEmpties <= Constants.EMPTIES_FOR_FORCED_MIDGAME);    
  }

  public short evaluatePosition(
      Board board, int lower, int upper, long maxNVisited, long maxTimeMillis, boolean reset) {
    assert Math.abs(lower % 200) == 100;
    assert Math.abs(upper % 200) == 100;
    assert(lower <= upper);
    evaluatorMidgame.constant = 400;
    evaluatorMidgame.nEndgames = 0;
    evaluatorMidgame.nVisitedEndgames = 0;
    constant = 0;
    status = Status.RUNNING;
    this.lower = lower;
    this.upper = upper;
    this.maxNVisited = maxNVisited;
    this.stopTimeMillis = System.currentTimeMillis() + maxTimeMillis;
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
      setEvalGoal(firstPosition.getEval());
    }

    StoredBoardBestDescendant nextPos;
    for (nextPos = StoredBoardBestDescendant.bestDescendant(firstPosition, this.nextPositionGreaterEqual());
         nextPos != null;
         nextPos = nextPositionToImprove()) {
      StoredBoard next = nextPos.board;
      int nEmpties = 64 - Long.bitCount(next.getPlayer() | next.getOpponent());

      if (next != this.firstPosition && toBeSolved(next)) {
        solvePosition(nextPos, nEmpties);
      } else if (this.size < this.maxSize) {
        addChildren(nextPos, nEmpties);
      } else {
        deepenPosition(nextPos, nEmpties);
      }
      next.updateFathers();
      updateEvalGoalIfNeeded();
////      if (this.firstPosition.evalGoal == -1200) {
//        System.out.println(
//            this.firstPosition.evalGoal + " " + firstPosition.getDescendants() + " "
//            + firstPosition.probGreaterEqual + " " + firstPosition.probStrictlyGreater);
////      }
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