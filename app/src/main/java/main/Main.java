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

import java.awt.Toolkit;
import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.StringSelection;
import java.util.ArrayList;

import bitpattern.PositionIJ;
import board.Board;

import java.util.Set;
import java.util.SortedSet;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.atomic.AtomicInteger;

import evaluateposition.TreeNodeInterface;
import jni.JNI;
import thor.Thor;
import ui_desktop.CaseAnnotations;

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
  private final JNI EVALUATOR;
  private final ExecutorService EXECUTOR = Executors.newSingleThreadExecutor();
  private long startTime;
  private static UI ui;
  private final AtomicInteger waitingTasks = new AtomicInteger(0);
  private final Thor thor;

  /**
   * Creates a new UI and sets the initial position.
   */
  public Main(UI ui) {
    Main.ui = ui;
    thor = new Thor(ui.fileAccessor());
    EVALUATOR = new JNI(ui.fileAccessor());
  }

  public SortedSet<String> getThorTournaments() {
    return thor.getTournaments();
  }

  public SortedSet<String> getThorPlayers() {
    return thor.getPlayers();
  }

  public void thorLookup(Set<String> black, Set<String> white, Set<String> tournaments) {
    thor.lookupPositions(black, white, tournaments);
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
    EVALUATOR.stop();
  }

  public void resetHashMaps() {
    stop();
    Future finished = EXECUTOR.submit(() -> EVALUATOR.empty());
    try {
      finished.get();
    } catch (ExecutionException | InterruptedException e) {
      e.printStackTrace();
    }
    ui.setCases(board, blackTurn);
  }

  public void setEndgameBoard(int n) {
    stop();
    setBoard(JNI.getEndgameBoard(n), true);
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

  public ArrayList<Board> getBoards() {
    ArrayList<Board> boards = new ArrayList<>();
    if (ui.delta() == 0) {
      if (!ui.useBook() || EVALUATOR.getFromBook(board.getPlayer(), board.getOpponent()) == null) {
        boards.add(board);
      }
    } else {
      for (Board child : JNI.uniqueChildren(board)) {
        if (!ui.useBook() || EVALUATOR.getFromBook(child.getPlayer(), child.getOpponent()) == null) {
          boards.add(child);
        }
      }
    }
    return boards;
  }

  @Override
  public void run() {
    waitingTasks.getAndAdd(-1);
    Board board = new Board(this.board.getPlayer(), this.board.getOpponent());
    ArrayList<Board> boards = getBoards();
    if (boards.isEmpty()) {
      showMCTSEvaluations();
      return;
    }
    startTime = System.currentTimeMillis();
    EVALUATOR.evaluate(boards, ui.lower(), ui.upper(), ui.maxVisited(), 100, (float) ui.delta());
    if (ui.wantThorGames()) {
      ui.setThorGames(board, thor.getGames(board));
    }
    while (ui.active() && !EVALUATOR.finished(ui.maxVisited())) {
      showMCTSEvaluations();
      EVALUATOR.evaluate(boards, ui.lower(), ui.upper(), ui.maxVisited(), 1000, (float) ui.delta());
    }
    if (ui.useBook() && ui.delta() > 0) {
      System.out.println("Adding to book" + board);
      EVALUATOR.addToBook(board, oldBoards);
    }
    if (waitingTasks.get() == 0) {
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
      board = JNI.move(board, move);
      if (board.equals(oldBoard)) {
        return;
      }
    } catch (IllegalArgumentException ex) {
      return;
    }
    oldBoards.add(oldBoard);
    oldBlackTurns.add(blackTurn);
    blackTurn = !blackTurn;

    if (JNI.haveToPass(board)) {
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

    for (Board child : JNI.children(board)) {
      TreeNodeInterface b = getStoredBoard(child);
      if (b == null) {
        continue;
      }
      int eval = b.getEval();
      int move = moveFromBoard(board, child);
      if (eval < best) {
        best = eval;
        bestMove = move;
      }
    }
    return bestMove;
  }
  
  private void evaluate() {
    if (!ui.active()) {
      return;
    }
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
    if (waitingTasks.compareAndSet(0, 1)) {
      EXECUTOR.submit(this);
    }
  }

  private TreeNodeInterface getStoredBoard(Board b) {
    return getStoredBoard(b.getPlayer(), b.getOpponent());
  }

  private TreeNodeInterface getStoredBoard(long player, long opponent) {
    TreeNodeInterface board = EVALUATOR.get(player, opponent);
    if (ui.useBook() && board == null) {
      return EVALUATOR.getFromBook(player, opponent);
    }
    return board;
  }

  private long getNVisited() {
    long result = 0;
    for (Board child : JNI.uniqueChildren(board)) {
      TreeNodeInterface childStored = getStoredBoard(child);
      if (childStored != null) {
        result += childStored.getDescendants();
      }
    }
    return result;
  }

  private void showMCTSEvaluations() {
    int bestMove = this.findBestMove();
    boolean hasThor = thor.getNumGames(board) > 0;
    TreeNodeInterface father = getStoredBoard(board);
    for (Board child : JNI.children(board)) {
      TreeNodeInterface childStored = getStoredBoard(child);
      CaseAnnotations annotations;
      int move = moveFromBoard(board, child);
      if (childStored != null) {
        annotations = new CaseAnnotations(father, childStored, hasThor ? thor.getNumGames(child) : -1, move == bestMove);
        ui.setAnnotations(annotations, move);
      }
    }
    CaseAnnotations positionAnnotations = null;
    if (father != null) {
      positionAnnotations = new CaseAnnotations(father, father, false);
    }
    ui.setExtras(
        getNVisited(), System.currentTimeMillis() - startTime,
        positionAnnotations);
    ui.repaint();
  }

  public void copy() {
    Clipboard clipboard = Toolkit.getDefaultToolkit().getSystemClipboard();
    String game = "";
    for (int i = 1; i < oldBoards.size(); ++i) {
      int move = moveFromBoard(oldBoards.get(i-1), oldBoards.get(i));
      game += new PositionIJ(move).toString();
    }
    game += new PositionIJ(moveFromBoard(oldBoards.get(oldBoards.size() - 1), board));
    clipboard.setContents(new StringSelection(game), null);
  }
}
