// Copyright 2023 Michele Borassi
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

import java.util.ArrayList;

import bitpattern.PositionIJ;
import board.Board;

import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.atomic.AtomicInteger;

import jni.JNI;
import jni.Node;
import jni.ThorGame;
import jni.ThorGameWithMove;
import ui_desktop.CaseAnnotations;

/**
 * @author michele
 * The main function of the program.
 * Connects the board and the evaluation with the UI.
 */
public class Main implements Runnable {
  public enum UseBook {
    DO_NOT_USE,
    READ_ONLY,
    READ_WRITE
  }
  public static class RunParameters {
    final UseBook useBook;
    final double delta;
    final int nThreads;
    final boolean approx;
    final double error;
    final int lower;
    final int upper;
    final long maxNVisited;
    final boolean playBlackMoves;
    final boolean playWhiteMoves;
    final boolean wantThorGames;
    public RunParameters() {
      this.useBook = UseBook.DO_NOT_USE;
      this.delta = 0;
      this.nThreads = 1;
      this.approx = false;
      this.error = 0;
      this.lower = -6300;
      this.upper = 6300;
      this.maxNVisited = 1;
      this.playBlackMoves = false;
      this.playWhiteMoves = false;
      this.wantThorGames = false;
    }

    public RunParameters(UseBook useBook, double delta, int nThreads, boolean approx, double error, int lower, int upper, long maxNVisited, boolean playBlackMoves, boolean playWhiteMoves, boolean wantThorGames) {
      this.useBook = useBook;
      this.delta = delta;
      this.nThreads = nThreads;
      this.approx = approx;
      this.error = error;
      this.lower = lower;
      this.upper = upper;
      this.maxNVisited = maxNVisited;
      this.playBlackMoves = playBlackMoves;
      this.playWhiteMoves = playWhiteMoves;
      this.wantThorGames = wantThorGames;
    }

    private boolean writeBook() {
      return useBook == UseBook.READ_WRITE;
    }

    private boolean readBook() {
    return useBook != UseBook.DO_NOT_USE;
  }

    private boolean isSenseiTurn(boolean blackTurn) {
      return (blackTurn && playBlackMoves) || (!blackTurn && playWhiteMoves);
    }
  }
  private class Position {
    Board board;
    boolean blackTurn;
    double error;
    boolean senseiPlayed;

    public Position(Board board, boolean blackTurn, double error, boolean senseiPlayed) {
      this.board = board;
      this.blackTurn = blackTurn;
      this.error = error;
      this.senseiPlayed = senseiPlayed;
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
  private boolean canComputeError;
  private final int MAX_THOR_GAMES = 100;
  RunParameters runParameters = new RunParameters();
  RunParameters newRunParameters;

  /**
   * Creates a new UI and sets the initial position.
   */
  public Main(UI ui) {
    Main.ui = ui;
    EVALUATOR = new JNI(ui.evalFile(), ui.bookFolder(), ui.thorFolder());
    resetFirstPosition();
    setFirstPosition(new Board(), true);
  }

  public ArrayList<String> getThorTournaments() {
    return EVALUATOR.getThorTournaments();
  }

  public ArrayList<String> getThorPlayers() {
    return EVALUATOR.getThorPlayers();
  }

  public void thorLookup(List<String> black, List<String> white, List<String> tournaments) {
    EVALUATOR.lookupThorPositions(black, white, tournaments);
  }

  /**
   * Action to perform when the case (i, j) was clicked.
   * Performs the move.
   * @param move: the row and the column (from 0 to 7)
   */
  public void play(int move) {
    stop();
    playMoveIfPossible(move);
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

  public void analyzeSelfGames() {
    double error = 1.2;
    System.out.println(error);
    for (int i = 0; i < 20; ++i) {
      while (!JNI.isGameOver(board)) {
        run();
        playMoveIfPossible(this.findMoveToPlay());
      }
      for (Position oldPosition : oldPositions) {
        System.out.print(oldPosition.error + " ");
      }
      System.out.println();
      newGame();
    }
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
    if (oldPositions.size() == 0) {
      return;
    }
    Position position = oldPositions.remove(oldPositions.size() - 1);
    while (position.senseiPlayed && oldPositions.size() > 0) {
      position = oldPositions.remove(oldPositions.size() - 1);
    }
    board = position.board;
    blackTurn = position.blackTurn;
    ui.setCases(board, blackTurn);
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
    if (runParameters.delta == 0) {
      if (!runParameters.readBook() || EVALUATOR.getFromBook(board.getPlayer(), board.getOpponent()) == null) {
        boards.add(board);
      }
    } else {
      for (Board child : JNI.uniqueChildren(board)) {
        if (!runParameters.readBook() || EVALUATOR.getFromBook(child.getPlayer(), child.getOpponent()) == null) {
          boards.add(child);
        }
      }
    }
    return boards;
  }

  public String thorGamesToString(List<ThorGameWithMove> games, int numGames) {
    String result = "Showing " + games.size() + " of " + numGames + " games:\n";
    for (ThorGameWithMove game : games) {
      result += game.toString() + "\n";
    }
    return result;
  }

  public void setThorGames(Board board) {
    ArrayList<ThorGameWithMove> allThorGames = new ArrayList<>();
    int numGames = 0;
    for (Board child : JNI.children(board)) {
      ArrayList<ThorGame> currentGames = EVALUATOR.getThorGames(child);
      int move = moveFromBoard(board, child);
      ui.setThorGames(currentGames, move);
      numGames += currentGames.size();
      int i = 0;
      for (ThorGame game : currentGames) {
        allThorGames.add(new ThorGameWithMove(game, (byte) move));
        if (i++ >= MAX_THOR_GAMES) {
          break;
        }
      }
    }
    allThorGames.sort(Comparator
        .comparing((ThorGameWithMove g)->g.game().year()).reversed()
        .thenComparing((ThorGameWithMove g)->g.game().tournament()));
    ui.updateThorGamesWindow(allThorGames);
  }

  public void evaluate(ArrayList<Board> boards) {
    startTime = System.currentTimeMillis();
    EVALUATOR.evaluate(boards, runParameters.lower, runParameters.upper, runParameters.maxNVisited, 100, (float) runParameters.delta, runParameters.nThreads, runParameters.approx);
    while (!EVALUATOR.finished(runParameters.maxNVisited)) {
      if (!(runParameters.playBlackMoves || runParameters.playWhiteMoves)) {
        showMCTSEvaluations();
      }
      EVALUATOR.evaluate(boards, runParameters.lower, runParameters.upper, runParameters.maxNVisited, 1000, (float) runParameters.delta, runParameters.nThreads, runParameters.approx);
    }
    if (runParameters.writeBook() && runParameters.delta > 0 && firstPosition.equals(new Board())) {
      ArrayList<Board> oldBoards = new ArrayList<>();
      for (Position oldPosition : this.oldPositions) {
        oldBoards.add(oldPosition.board.deepCopy());
      }
      oldBoards.add(board.deepCopy());
      EVALUATOR.addToBook(board, oldBoards);
    }
  }

  @Override
  public void run() {
    runParameters = newRunParameters;
    waitingTasks.getAndAdd(-1);
    Board board = this.board.deepCopy();
    if (JNI.isGameOver(board)) {
      return;
    }
    boolean isMatch = runParameters.playBlackMoves || runParameters.playWhiteMoves;

    if (runParameters.wantThorGames) {
      setThorGames(board);
    }

    boolean needToEvaluate = true;
    while (needToEvaluate) {
      ArrayList<Board> boards = getBoards();
      if (!boards.isEmpty()) {
        evaluate(boards);
      }

      if (runParameters.isSenseiTurn(blackTurn)) {
        this.playMoveIfPossible(findMoveToPlay());
      } else if (waitingTasks.get() == 0 && !isMatch) {
        showMCTSEvaluations();
      }
      needToEvaluate = runParameters.isSenseiTurn(blackTurn);
      setExtras();
    }
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
    oldPositions.add(new Position(board, blackTurn, error, runParameters.isSenseiTurn(blackTurn)));

    if (JNI.haveToPass(newBoard) && !JNI.isGameOver(newBoard)) {
      board = newBoard.move(0);
    } else {
      board = newBoard;
      blackTurn = !blackTurn;
    }
    ui.setCases(board, blackTurn);
  }
  
  private int moveFromBoard(Board father, Board child) {
    long move = (child.getPlayer() | child.getOpponent()) & ~(father.getPlayer() | father.getOpponent());
    
    return Long.numberOfTrailingZeros(move);
  }

  private HashMap<Integer, Node> getNextPositions() {
    HashMap<Integer, Node> result = new HashMap<>();
    for (Board child : JNI.children(board)) {
      Node b = getStoredBoard(child);
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

  int findMoveToPlay() {
    double error = runParameters.error;
    assert(error > 5 && error < 80);
    int currentMove = 60 - board.getEmptySquares();
    double moveMultiplier = 0.06 * currentMove / 1.235;
    if (currentMove >= 48) {
      moveMultiplier = 0.77 / 1.235;
    }
    double bestScore = Double.POSITIVE_INFINITY;
    double base = 1 + 8.12 * Math.pow(error * moveMultiplier, -0.825);
    int move = -1;
    for (Map.Entry<Integer, Node> entry : getNextPositions().entrySet()) {
      double eval = -entry.getValue().getEval();
      double score = generateExponential(Math.pow(base, eval));
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

    Map<Integer, Node> nextPositions = getNextPositions();
    for (Map.Entry<Integer, Node> entry : nextPositions.entrySet()) {
      double eval = -entry.getValue().getEval();
      if (eval > best) {
        best = eval;
      }
    }
    for (Map.Entry<Integer, Node> entry : nextPositions.entrySet()) {
      if (-entry.getValue().getEval() > best - 0.1) {
        bestMove.add(entry.getKey());
      }
    }
    return bestMove;
  }

  private double getError(int move) {
    if (board.equals(new Board())) {
      return 0;
    }
    double bestEval = Double.NEGATIVE_INFINITY;
    HashMap<Integer, Node> nextPositions = getNextPositions();
    if (!nextPositions.containsKey(move)) {
      return Double.NEGATIVE_INFINITY;
    }

    for (Node b : nextPositions.values()) {
      bestEval = Math.max(bestEval, -b.getEval());
    }

    return bestEval - (-nextPositions.get(move).getEval());
  }
  
  public void evaluate(RunParameters parameters) {
    if (parameters.maxNVisited <= 0) {
      showMCTSEvaluations();
      return;
    }
    if (waitingTasks.compareAndSet(0, 1)) {
      this.newRunParameters = parameters;
      EXECUTOR.submit(this);
    }
  }

  private Node getStoredBoard(Board b) {
    return getStoredBoard(b.getPlayer(), b.getOpponent());
  }

  private Node getStoredBoard(long player, long opponent) {
    if (runParameters.readBook()) {
      Node node = EVALUATOR.getFromBook(player, opponent);
      if (node != null) {
        return node;
      }
    }
    return EVALUATOR.get(player, opponent);
  }

  private long getNVisited() {
    long result = 0;
    for (Board child : JNI.uniqueChildren(board)) {
      Node childStored = getStoredBoard(child);
      if (childStored != null) {
        result += childStored.getDescendants();
      }
    }
    return result;
  }

  private void showMCTSEvaluations() {
    ArrayList<Integer> bestMove = this.findBestMoves();
    Node father = getStoredBoard(board);
    for (Board child : JNI.children(board)) {
      Node childStored = getStoredBoard(child);
      CaseAnnotations annotations;
      int move = moveFromBoard(board, child);
      if (childStored != null) {
        annotations = new CaseAnnotations(
            father,
            childStored,
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
    Node father = getStoredBoard(board);
    CaseAnnotations positionAnnotations = null;
    if (father != null) {
      positionAnnotations = new CaseAnnotations(father, father, false);
    }
    ui.setExtras(
        getNVisited(), System.currentTimeMillis() - startTime,
        positionAnnotations, getError(true), getError(false));
  }

  public String getGame() {
    String game = "";
    for (int i = 1; i < oldPositions.size(); ++i) {
      int move = moveFromBoard(oldPositions.get(i-1).board, oldPositions.get(i).board);
      game += new PositionIJ(move).toString();
    }
    int move = moveFromBoard(oldPositions.get(oldPositions.size() - 1).board, board);
    game += new PositionIJ(move).toString();
    return game;
  }
}
