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

import java.util.HashMap;
import java.util.Map;
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
  private class Position {
    Board board;
    boolean blackTurn;
    double error;

    public Position(Board board, boolean blackTurn, double error) {
      this.board = board;
      this.blackTurn = blackTurn;
      this.error = error;
    }
  }
  Board board;
  boolean blackTurn;
  Board firstPosition;
  boolean firstBlackTurn;
  final ArrayList<Position> oldPositions = new ArrayList<>();
  private final JNI EVALUATOR;
  private final ExecutorService EXECUTOR = Executors.newSingleThreadExecutor();
  private long startTime;
  private static UI ui;
  private final AtomicInteger waitingTasks = new AtomicInteger(0);
  private static Thor thor;
  private boolean canComputeError;

  /**
   * Creates a new UI and sets the initial position.
   */
  public Main(UI ui) {
    Main.ui = ui;
    thor = new Thor(ui.fileAccessor());
    EVALUATOR = new JNI(ui.fileAccessor());
    resetFirstPosition();
  }

  public SortedSet<String> getThorTournaments() {
    return thor.getTournaments();
  }

  public static SortedSet<String> getThorPlayers() {
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

  public void setFirstPosition(Board board, boolean blackTurn) {
    firstPosition = board;
    firstBlackTurn = blackTurn;
    newGame();
  }

  public void resetFirstPosition() {
    setFirstPosition(new Board(), true);
  }

  public void setFirstPosition() {
    setFirstPosition(board.deepCopy(), blackTurn);
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

  public boolean isSenseiTurn() {
    return (blackTurn && ui.playBlackMoves()) || (!blackTurn && ui.playWhiteMoves());
  }

  /**
   * Undoes a move, if possible.
   */
  public void undo() {
    stop();
    boolean firstUndo = true;
    while ((firstUndo || isSenseiTurn()) && oldPositions.size() > 0) {
      Position position = oldPositions.remove(oldPositions.size() - 1);
      board = position.board;
      blackTurn = position.blackTurn;
      ui.setCases(board, blackTurn);
      firstUndo = false;
    }
    evaluate();
  }
  
  public void newGame() {
    stop();
    setBoard(firstPosition, firstBlackTurn);
  }
  
  public void setBoard(Board b, boolean blackTurn) {
    stop();
    board = b;
    this.blackTurn = blackTurn;
    oldPositions.clear();
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
    Board board = this.board.deepCopy();
    if (JNI.isGameOver(board)) {
      return;
    }
    boolean mustPlay = (ui.playBlackMoves() && blackTurn) || (ui.playWhiteMoves() && !blackTurn);
    boolean isMatch = ui.playBlackMoves() || ui.playWhiteMoves();
    ArrayList<Board> oldBoards = new ArrayList<>();
    for (Position oldPosition : this.oldPositions) {
      oldBoards.add(oldPosition.board.deepCopy());
    }

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
    canComputeError = ui.delta() > 0;
    while (ui.active() && !EVALUATOR.finished(ui.maxVisited())) {
      if (!isMatch) {
        showMCTSEvaluations();
      }
      EVALUATOR.evaluate(boards, ui.lower(), ui.upper(), ui.maxVisited(), 1000, (float) ui.delta());
    }
    if (ui.useBook() && ui.delta() > 0) {
      EVALUATOR.addToBook(board, oldBoards);
    }
    if (mustPlay) {
      this.play(findMoveToPlay(ui.getError()));
    } else if (waitingTasks.get() == 0 && !isMatch) {
      showMCTSEvaluations();
    }
    setExtras();
  }
  
  /**
   * Plays a move at the coordinates (i, j), if it is possible.
   * 
   * @param move the move (0 is h8, 1 is g8, ..., 63 is a1).
   */
  private void playMoveIfPossible(int move) {
    Board newBoard;
    try {
      newBoard = JNI.move(board, move);
      if (newBoard.equals(board)) {
        return;
      }
    } catch (IllegalArgumentException ex) {
      return;
    }
    double error = getError(move);
    oldPositions.add(new Position(board, blackTurn, error));

    if (JNI.haveToPass(newBoard) && !JNI.isGameOver(newBoard)) {
      board = newBoard.move(0);
    } else {
      board = newBoard;
      blackTurn = !blackTurn;
    }
    ui.setCases(board, blackTurn);
    canComputeError = false;
  }
  
  private int moveFromBoard(Board father, Board child) {
    long move = (child.getPlayer() | child.getOpponent()) & ~(father.getPlayer() | father.getOpponent());
    
    return Long.numberOfTrailingZeros(move);
  }

  private HashMap<Integer, TreeNodeInterface> getNextPositions() {
    HashMap<Integer, TreeNodeInterface> result = new HashMap<>();
    for (Board child : JNI.children(board)) {
      TreeNodeInterface b = getStoredBoard(child);
      if (b == null) {
        continue;
      }
      result.put(moveFromBoard(board, child), b);
    }
    return result;
  }

  public static double generateExponential(double lambda) {
    return -1 / lambda * Math.log(Math.random());
  }

  int findMoveToPlay(double error) {
    double bestScore = Double.POSITIVE_INFINITY;
    int move = -1;
    for (Map.Entry<Integer, TreeNodeInterface> entry : getNextPositions().entrySet()) {
      double eval = -entry.getValue().getEval() / 100.0;
      double score = generateExponential(Math.pow(error, eval));
      if (score < bestScore) {
        bestScore = score;
        move = entry.getKey();
      }
    }
    return move;
  }

  private ArrayList<Integer> findBestMoves() {
    double best = Double.NEGATIVE_INFINITY;
    ArrayList<Integer> bestMove = new ArrayList<>();

    for (Map.Entry<Integer, TreeNodeInterface> entry : getNextPositions().entrySet()) {
      double eval = -entry.getValue().getEval() / 100.0;
      int move = entry.getKey();
      if (eval > best) {
        best = eval;
        bestMove.clear();
        bestMove.add(move);
      } else if (eval == best) {
        bestMove.add(move);
      }
    }
    return bestMove;
  }

  private double getError(int move) {
    if (board.equals(new Board())) {
      return 0;
    }
    if (!canComputeError) {
      return Double.NEGATIVE_INFINITY;
    }
    double bestEval = Double.NEGATIVE_INFINITY;
    HashMap<Integer, TreeNodeInterface> nextPositions = getNextPositions();
    if (!nextPositions.containsKey(move)) {
      return Double.NEGATIVE_INFINITY;
    }

    for (TreeNodeInterface b : nextPositions.values()) {
      bestEval = Math.max(bestEval, -b.getEval() / 100.0);
    }

    return bestEval - (-nextPositions.get(move).getEval() / 100.0);
  }
  
  private void evaluate() {
    if (!ui.active()) {
      return;
    }
    if (ui.maxVisited() <= 0) {
      showMCTSEvaluations();
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
    ArrayList<Integer> bestMove = this.findBestMoves();
    boolean hasThor = thor.getNumGames(board) > 0;
    TreeNodeInterface father = getStoredBoard(board);
    for (Board child : JNI.children(board)) {
      TreeNodeInterface childStored = getStoredBoard(child);
      CaseAnnotations annotations;
      int move = moveFromBoard(board, child);
      if (childStored != null) {
        annotations = new CaseAnnotations(
            father,
            childStored,
            hasThor ? thor.getNumGames(child) : -1,
            bestMove.contains(move));
        ui.setAnnotations(annotations, move);
      }
    }
    setExtras();
    ui.repaint();
  }

  public double getError(boolean black) {
    double result = 0;
    for (Position position : oldPositions) {
      if (position.blackTurn == black) {
        result += position.error;
      }
    }
    return result;
  }

  public void setExtras() {
    TreeNodeInterface father = getStoredBoard(board);
    CaseAnnotations positionAnnotations = null;
    if (father != null) {
      positionAnnotations = new CaseAnnotations(father, father, false);
    }
    ui.setExtras(
        getNVisited(), System.currentTimeMillis() - startTime,
        positionAnnotations, getError(true), getError(false));
  }

  public void copy() {
    Clipboard clipboard = Toolkit.getDefaultToolkit().getSystemClipboard();
    String game = "";
    for (int i = 1; i < oldPositions.size(); ++i) {
      int move = moveFromBoard(oldPositions.get(i-1).board, oldPositions.get(i).board);
      game += new PositionIJ(move).toString();
    }
    clipboard.setContents(new StringSelection(game), null);
  }
}
