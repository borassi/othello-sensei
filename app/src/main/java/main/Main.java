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

import java.util.ArrayList;

import board.Board;
import board.GetMovesCache;
import constants.Constants;
import endgametest.EndgameTest;
import evaluateposition.EvaluatorAlphaBeta;
import evaluateposition.EvaluatorMCTS;
import evaluateposition.HashMap;
import evaluateposition.StoredBoard;

import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;

import ui.CaseAnnotations;
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
  Board[] boardsToEvaluate = new Board[] {};
  private final HashMap HASH_MAP;
  private final EvaluatorMCTS[] EVALUATORS = new EvaluatorMCTS[20];
  private final ExecutorService EXECUTOR = Executors.newSingleThreadExecutor();
  private long startTime;
  private static UI ui;
  private final AtomicBoolean waitingTasks = new AtomicBoolean(false);

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
    Main.ui = ui;
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
    for (int i = 0; i < EVALUATORS.length; ++i) {
      EVALUATORS[i].stop();
    }
  }

  public void resetHashMaps() {
    stop();
    Future finished = EXECUTOR.submit(() -> {
      for (EvaluatorMCTS evaluator : EVALUATORS) {
        evaluator.empty();
      }
      HASH_MAP.reset();
      EvaluatorAlphaBeta.resetConstant();
    });
    try {
      finished.get();
    } catch (ExecutionException | InterruptedException e) {
      e.printStackTrace();
    }
    ui.setCases(board, blackTurn);
  }

  public void setEndgameBoard(int n) {
    stop();
    setBoard(EndgameTest.readIthBoard(n), true);
  }

  /**
   * Undoes a move, if possible.
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
        case RUNNING:
          assert false;
        case FAILED:
        case KILLING:
        case KILLED:
          return true;
        case SOLVED:
          break;
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
    if (evaluator.getStatus() == EvaluatorMCTS.Status.SOLVED) {
      return Double.NEGATIVE_INFINITY;
    }
    double evalEffect = -board.getEval(evaluator.getWeakLower(), evaluator.getWeakUpper()) / Math.max(1, ui.delta() * 100);
    return evalEffect - Math.log(board.getDescendants()) / Math.log(2);
  }

  private void evaluateFirst() {
    for (int i = 0; i < boardsToEvaluate.length; ++i) {
      EvaluatorMCTS evaluator = EVALUATORS[i];
      Board board = boardsToEvaluate[i];
      evaluator.evaluatePosition(board, ui.lower(), ui.upper(), ui.maxVisited(), 20);
    }
  }

  private void evaluate(int time) {
    for (int step = 0; step < 5 && !finished(); ++step) {
      EvaluatorMCTS nextEvaluator = EVALUATORS[0];
      double best = Double.NEGATIVE_INFINITY;

      for (int i = 0; i < boardsToEvaluate.length; ++i) {
        EvaluatorMCTS evaluator = EVALUATORS[i];
        double value = boardValue(evaluator);
        if (value > best) {
          best = value;
          nextEvaluator = evaluator;
        }
      }
      nextEvaluator.evaluatePosition(nextEvaluator.getFirstPosition().getBoard(), ui.lower(), ui.upper(), ui.maxVisited(), time / 5);
    }
  }

  @Override
  public void run() {
    startTime = System.currentTimeMillis();
    waitingTasks.set(false);
    setEvaluators();
    evaluateFirst();
    showMCTSEvaluations();
    while (!finished()) {
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
    if (waitingTasks.compareAndSet(false, true)) {
      EXECUTOR.submit(this);
    }
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
    long nVisited = 0;
    for (int i = 0; i < this.boardsToEvaluate.length; ++i) {
      nVisited += this.EVALUATORS[i].getNVisited();
    }
    CaseAnnotations positionAnnotations = null;
    if (evaluatorBoard.board != null) {
      positionAnnotations = new CaseAnnotations(
          evaluatorBoard.board, evaluatorBoard.evaluator.getWeakLower(),
          evaluatorBoard.evaluator.getWeakUpper(), false);
    }
    ui.setExtras(
        nVisited, System.currentTimeMillis() - startTime,
        positionAnnotations);
    ui.repaint();
  }
}
