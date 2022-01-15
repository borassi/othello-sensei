// Copyright 2021 Google LLC
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

package main;

import static evaluateposition.StoredBoard.LOG_DERIVATIVE_MULTIPLIER;

import bitpattern.BitPattern;
import bitpattern.PositionIJ;

import java.util.ArrayList;

import board.Board;
import board.GetMovesCache;
import constants.Constants;
import endgametest.EndgameTest;
import evaluateposition.EvaluatorAlphaBeta;
import evaluateposition.EvaluatorMCTS;
import evaluateposition.EvaluatorMCTS.Status;
import evaluateposition.HashMap;
import evaluateposition.StoredBoard;

import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;
import java.util.logging.Level;
import java.util.logging.Logger;

import ui.CaseAnnotations;
//import ui.DesktopUI;
import ui.UI;

/**
 * @author michele
 * The main function of the program.
 * Connects the board and the evaluation with the UI.
 */
public class Main implements Runnable {
  final ArrayList<Boolean> oldBlackTurns = new ArrayList<>();
  final ArrayList<Board> oldBoards = new ArrayList<>();
  boolean blackTurn = true;
  Board board;
  Board[] boardsToEvaluate;
  private final HashMap HASH_MAP;
  private final EvaluatorMCTS EVALUATORS[] = new EvaluatorMCTS[20];
  private final ExecutorService EXECUTOR = Executors.newSingleThreadExecutor();
  Future future = null;
  private long startTime;
  private final int updateTimes[] = {100, 1000};
  private static UI ui;
  private boolean stopping = false;

  private static class EvaluatorWithBoard {
    EvaluatorMCTS evaluator;
    StoredBoard board;
    private EvaluatorWithBoard(EvaluatorMCTS evaluator, StoredBoard board) {
      this.evaluator = evaluator;
      this.board = board;
    }
    private int getEval() {
      return board.getEval(evaluator.getWeakLower(), evaluator.getWeakUpper());
    }
  }

  /**
   * Creates a new UI and sets the initial position.
   */
  public Main(UI ui) {
    this.ui = ui;
    HASH_MAP = new HashMap(Constants.hashMapSize());
    for (int i = 0; i < EVALUATORS.length; ++i) {
      EVALUATORS[i] = new EvaluatorMCTS(Constants.MCTSSize() / 20, 2 * Constants.MCTSSize() / 20, HASH_MAP);
    }
  }

  /**
   * Action to perform when the case (i, j) was clicked.
   * Performs the move.
   * @param move: the row and the column (from 0 to 7)
   */
  public void play(int move) {
    stop();
    playMoveIfPossible(move);
    evaluate();
  }

  public void stop() {
    stopping = true;
    while (true) {
      if (future == null) {
        return;
      }
      for (int i = 0; i < EVALUATORS.length; ++i) {
        EVALUATORS[i].stop();
      }
      try {
        future.get(100, TimeUnit.MILLISECONDS);
        future = null;
        break;
      } catch (InterruptedException | ExecutionException ex) {
        Logger.getLogger(Main.class.getName()).log(Level.SEVERE, null, ex);
        break;
      } catch (TimeoutException ex) {}
    }
  }

  public void resetHashMaps() {
    stop();
    for (int i = 0; i < EVALUATORS.length; ++i) {
      EVALUATORS[i].empty();
    }
    HASH_MAP.reset();
    EvaluatorAlphaBeta.resetConstant();
    ui.setCases(board, blackTurn);
  }

  public void setEndgameBoard(int n) {
    stop();
    setBoard(EndgameTest.readIthBoard(n), true);
  }

  /**
   * Undos a move, if possible.
   */
  public void undo() {
    stop();
    if (oldBoards.size() > 0) {
      board = oldBoards.remove(oldBoards.size() - 1);
      blackTurn = oldBlackTurns.remove(oldBlackTurns.size() - 1);
      ui.setCases(board, blackTurn);
    }
  }
  
  public void newGame() {
    stop();
    setBoard(new Board(), true);
  }
  
  public void setBoard(Board b, boolean blackTurn) {
    stop();
    this.board = b;
    this.blackTurn = blackTurn;
    this.oldBoards.clear();
    this.oldBlackTurns.clear();
    ui.setCases(board, blackTurn);
  }

  public boolean finished() {
    long visited = 0;
    boolean allSolved = true;
    for (int i = 0; i < this.boardsToEvaluate.length; ++i) {
      EvaluatorMCTS evaluator = EVALUATORS[i];
      visited += evaluator.getNVisited();
      switch (evaluator.getStatus()) {
        case NONE:
        case FAILED:
          return true;
        case SOLVED:
          break;
        case KILLING:
        case KILLED:
        case RUNNING:
        case STOPPED_TIME:
        case STOPPED_POSITIONS:
          allSolved = false;
      }
    }
    return visited > ui.maxVisited() || allSolved;
  }

  private void setEvaluators() {
    if (ui.playWhiteMoves() || ui.playBlackMoves() || ui.delta() == 0) {
      this.boardsToEvaluate = new Board[] {this.board};
    } else {
      this.boardsToEvaluate = GetMovesCache.getAllDescendants(this.board);
    }
  }

  private double boardValue(EvaluatorMCTS evaluator) {
    StoredBoard board = evaluator.getFirstPosition();
    int maxLogDerivative = Integer.MIN_VALUE;
    for (int evalGoal = evaluator.getWeakLower(); evalGoal <= evaluator.getWeakUpper(); evalGoal += 200) {
      if (board.getProb(evalGoal) > Constants.PROB_INCREASE_WEAK_EVAL &&
              board.getProb(evalGoal) < 1 - Constants.PROB_INCREASE_WEAK_EVAL) {
        maxLogDerivative = Math.max(maxLogDerivative, board.maxLogDerivative(evalGoal));
      }
    }
    double evalEffect = -board.getEval(evaluator.getWeakLower(), evaluator.getWeakUpper())
                            * LOG_DERIVATIVE_MULTIPLIER / Math.max(1, ui.delta() * 100);
    return evalEffect + maxLogDerivative;
  }

  private void evaluateFirst() {
    for (int i = 0; i < 1; ++i) {
      EvaluatorMCTS evaluator = EVALUATORS[i];
      Board board = boardsToEvaluate[i];
      evaluator.evaluatePosition(board, ui.lower(), ui.upper(), ui.maxVisited(), 20);
    }
  }

  private void evaluate(int time) {
    for (int step = 0; step < 10; ++step) {
      EvaluatorMCTS nextEvaluator = EVALUATORS[0];
      double best = Double.NEGATIVE_INFINITY;

      for (int i = 0; i < 1; ++i) {
        EvaluatorMCTS evaluator = EVALUATORS[i];
        double value = boardValue(evaluator);
        if (value > best) {
          best = value;
          nextEvaluator = evaluator;
        }
      }
      nextEvaluator.evaluatePosition(nextEvaluator.getFirstPosition().getBoard(), ui.lower(), ui.upper(), ui.maxVisited(), time / 10);
    }
  }

  @Override
  public synchronized void run() {
    stopping = false;
    setEvaluators();
    evaluateFirst();
    showMCTSEvaluations();
    while (!stopping && !finished()) {
      evaluate(1000);
      showMCTSEvaluations();
    }

    if ((ui.playBlackMoves() && blackTurn) || (ui.playWhiteMoves() && !blackTurn)) {
      this.playMoveIfPossible(this.findBestMove());
    }
  }
  
  /**
   * Plays a move at the coordinates (i, j), if it is possible.
   * 
   * @param move the move (0 is h8, 1 is g8, ..., 63 is a1).
   */
  private void playMoveIfPossible(int move) {
    Board oldBoard = board.deepCopy();
    try {
      board = GetMovesCache.moveIfPossible(board, move);
    } catch (IllegalArgumentException ex) {
      return;
    }
    oldBoards.add(oldBoard);
    oldBlackTurns.add(blackTurn);
    blackTurn = !blackTurn;

    if (GetMovesCache.haveToPass(board)) {
      board = board.move(0);
      blackTurn = !blackTurn;
    }
    ui.setCases(board, blackTurn);
  }
  
  private int moveFromBoard(Board father, Board child) {
    long move = (child.getPlayer() | child.getOpponent()) & ~(father.getPlayer() | father.getOpponent());
    
    return Long.numberOfTrailingZeros(move);
  }

  private int findBestMove() {
    double best = Double.POSITIVE_INFINITY;
    int bestMove = -1;

    for (Board child : GetMovesCache.getAllDescendants(board)) {
      EvaluatorWithBoard evaluatorBoard = getEvaluatorWithBoard(child);
      if (evaluatorBoard.board == null) {
        continue;
      }
      int eval = evaluatorBoard.getEval();
      int move = moveFromBoard(board, child);
      if (eval < best) {
        best = eval;
        bestMove = move;
      }
    }
    return bestMove;
  }
  
  private void evaluate() {
    if (ui.maxVisited() <= 0) {
      showMCTSEvaluations();
      return;
    }
    if (ui.playBlackMoves() && !blackTurn) {
      return;
    }
    
    if (ui.playWhiteMoves() && blackTurn) {
      return;
    }
    startTime = System.currentTimeMillis();
    future = EXECUTOR.submit(this);
  }

  private EvaluatorWithBoard getEvaluatorWithBoard(Board b) {
    return getEvaluatorWithBoard(b.getPlayer(), b.getOpponent());
  }

  private EvaluatorWithBoard getEvaluatorWithBoard(long player, long opponent) {
    EvaluatorMCTS bestEvaluator = null;
    StoredBoard best = null;
    for (int i = 0; i < this.boardsToEvaluate.length; ++i) {
      EvaluatorMCTS curEvaluator = EVALUATORS[i];
      StoredBoard current = curEvaluator.get(player, opponent);
      if (current == null) {
        continue;
      }
      if (best == null || current.getDescendants() > best.getDescendants()) {
        best = current;
        bestEvaluator = curEvaluator;
      }
    }
    return new EvaluatorWithBoard(bestEvaluator, best);
  }
  
  private void showMCTSEvaluations() {
    int bestMove = this.findBestMove();

    for (Board child : GetMovesCache.getAllDescendants(board)) {
      EvaluatorWithBoard evaluatorBoard = getEvaluatorWithBoard(child);
      StoredBoard childStored = evaluatorBoard.board;
      EvaluatorMCTS evaluator = evaluatorBoard.evaluator;
      CaseAnnotations annotations;
      int move = moveFromBoard(board, child);
      if (childStored != null) {
        annotations = new CaseAnnotations(childStored, evaluator.getWeakLower(),
            evaluator.getWeakUpper(), move == bestMove);
      } else {
        HashMap.BoardInHash childHash = this.HASH_MAP.getStoredBoardNoUpdate(child);
        if (childHash == null) {
          continue;
        }
        annotations = new CaseAnnotations(childHash, move == bestMove);
      }
      ui.setAnnotations(annotations, move);
    }
    EvaluatorWithBoard evaluatorBoard = getEvaluatorWithBoard(board);
    if (evaluatorBoard.board != null) {
      ui.setExtras(
          new CaseAnnotations(evaluatorBoard.board, evaluatorBoard.evaluator.getWeakLower(), evaluatorBoard.evaluator.getWeakUpper(), false),
          System.currentTimeMillis() - startTime);
    }
    ui.repaint();
  }
}
