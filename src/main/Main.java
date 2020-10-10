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

package main;

import bitpattern.BitPattern;
import bitpattern.PositionIJ;
import java.util.ArrayList;

import board.Board;
import board.GetMoves;
import board.GetMovesCache;
import constants.Constants;
import endgametest.EndgameTest;
import evaluatedepthone.PatternEvaluatorImproved;
import evaluateposition.EndgameTimeEstimator;
import evaluateposition.EvaluatorMCTS;
import evaluateposition.EvaluatorMidgame;
import evaluateposition.HashMap;
import evaluateposition.StoredBoard;
import helpers.Utils;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Comparator;
import java.util.PriorityQueue;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;
import java.util.logging.Level;
import java.util.logging.Logger;
import ui.CaseAnnotations;
import ui.DesktopUI;
import ui.UI;

/**
 * @author michele
 * The main function of the program.
 * Connects the board and the evaluation with the UI.
 */
public class Main implements Runnable {
  ArrayList<Boolean> oldBlackTurns = new ArrayList<>();
  ArrayList<Board> oldBoards = new ArrayList<>();
  boolean blackTurn = true;
  private final PatternEvaluatorImproved DEPTH_ONE_EVALUATOR;
  private final HashMap HASH_MAP;
  private final EvaluatorMidgame EVALUATOR_MIDGAME;
  private EvaluatorMCTS EVALUATOR;
  private final ExecutorService executor = Executors.newSingleThreadExecutor();
  Future future = null;
  private long startTime;
  /**
   * The board, as a pair of bitpattern.
   */
  Board board;
  /**
   * The user interface.
   */
  UI ui;
  public static FileAccessor fileAccessor = new DesktopFileAccessor();
  
  /**
   * Creates a new UI and sets the initial position.
   */
  public Main() {
    this(Constants.HASH_MAP_SIZE);
  }
  public Main(int hashMapSize) {
    DEPTH_ONE_EVALUATOR = PatternEvaluatorImproved.load();
    HASH_MAP = new HashMap(hashMapSize * 2, hashMapSize);
    EVALUATOR_MIDGAME = new EvaluatorMidgame(DEPTH_ONE_EVALUATOR, HASH_MAP);
    EVALUATOR = new EvaluatorMCTS(Constants.MCTS_SIZE, 2 * Constants.MCTS_SIZE, EVALUATOR_MIDGAME);
  }
  
  public void stop() {
    stopping = true;
    while (true) {
      if (future == null) {
        return;
      }
      EVALUATOR.stop();
      try {
        System.out.println(future);
        future.get(100, TimeUnit.MILLISECONDS);
        future = null;
        break;
      } catch (InterruptedException | ExecutionException ex) {
        Logger.getLogger(Main.class.getName()).log(Level.SEVERE, null, ex);
        break;
      } catch (TimeoutException ex) {}
    }
  }
  
  public void setUI(UI ui) {
    this.ui = ui;
    newGame();
    setBoard(EndgameTest.readIthBoard(46), true);
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
  
  /**
   * Action to perform when the case (i, j) was clicked.
   * Performs the move.
   * @param ij: the row and the column (from 0 to 7)
   */
  public void play(PositionIJ ij) {
    stop();
    playMoveIfPossible(ij);
    evaluate();
  }
  
  public void evaluatePosition(Board b, int updateTime, boolean reset) {
    if (stopping) {
      return;
    }
    EVALUATOR.evaluatePosition(
        b, -6400, 6400, ui.depth(), updateTime, reset);
//    EVALUATOR.evaluatePosition(
//        b, 0, 1, ui.depth(), updateTime, reset);
  }
  
  private boolean stopping = false;

  private PriorityQueue<StoredBoard> childrenToEvaluate(int delta) {
    PriorityQueue<StoredBoard> toEvaluate = new PriorityQueue<>(
        new Comparator<StoredBoard>() {
            @Override
            public int compare(StoredBoard t, StoredBoard t1) {
              return Integer.compare(t.getEval(), t1.getEval());
            }
          });
    StoredBoard firstPosition = EVALUATOR.getFirstPosition();
    if (firstPosition.getChildren() == null) {
      return toEvaluate;
    }
    for (StoredBoard child : firstPosition.getChildren()) {
      if (!child.isSolved()
          && -child.getEval() >= firstPosition.getEval() - delta) {
        toEvaluate.add(child);
      }
    }
    if (toEvaluate.isEmpty()) {
      StoredBoard bestUnsolved = null;
      for (StoredBoard child : firstPosition.getChildren()) {
        if (!child.isSolved()
            && (bestUnsolved == null || -child.getEval() >= -bestUnsolved.getEval())) {
          bestUnsolved = child;
        }
      }
      if (bestUnsolved != null) {
        toEvaluate.add(bestUnsolved);
      }
    }
    return toEvaluate;
  }
  
  private final int updateTimes[] = {100, 1000};
  
  @Override
  public synchronized void run() {
    stopping = false;
    int nUpdate = 0;
    startTime = System.currentTimeMillis();
    
//    evaluatePosition(board, (int) (updateTimes[0] * 0.5), true);
    while (!stopping) {
      int updateTime = updateTimes[Math.min(updateTimes.length-1, nUpdate++)];

      evaluatePosition(board, (int) (updateTime), false);
      showMCTSEvaluations();
      ui.setMovesPerSecond(EVALUATOR.getNVisited() * 1000. / (System.currentTimeMillis() - startTime));
      switch (EVALUATOR.getStatus()) {
        case NONE:
        case RUNNING:
          throw new RuntimeException("Bad state " + EVALUATOR.getStatus() + " for EvaluatorMCTS.");
        case KILLING:
        case KILLED:
        case STOPPED_POSITIONS:
        case SOLVED:
          stopping = true;
          break;
        case STOPPED_TIME:
          break;
      }
//      PriorityQueue<StoredBoard> toEvaluate = childrenToEvaluate(100 * ui.delta());
//      int timeEachPosition = (int) (0.9 * updateTime / toEvaluate.size());
//      if (toEvaluate.isEmpty()) {
//        return;
//      }
//
//      while (!toEvaluate.isEmpty()) {
//        StoredBoard child = toEvaluate.poll();
//        evaluatePosition(child.getBoard(), timeEachPosition, false);
//      }
      nUpdate++;
    }
    
    if ((ui.playBlackMoves() && blackTurn) || (ui.playWhiteMoves() && !blackTurn)) {
      Board evaluatedBoard = EVALUATOR.getFirstPosition().getBoard();
      if (evaluatedBoard.getPlayer() == this.board.getPlayer()
          && evaluatedBoard.getOpponent() == this.board.getOpponent()) {
        this.playMoveIfPossible(this.findBestMove(EVALUATOR.getEvaluations()));
      }
    }
  }
  
  /**
   * Plays a move at the coordinates (i, j), if it is possible.
   * 
   * @param ij the row and the column (from 0 to 7)
   */
  private void playMoveIfPossible(PositionIJ ij) {
    Board oldBoard = board.deepCopy();
    try {
      board = GetMovesCache.moveIfPossible(board, ij.toMove());
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
  
  private void showHashMapEvaluations() {
    GetMovesCache mover = new GetMovesCache();
    long player = board.getPlayer();
    long opponent = board.getOpponent();
    long movesBit = mover.getMoves(board.getPlayer(), board.getOpponent());
    long flip;
    int move;
    
    while (movesBit != 0) {
      flip = Long.lowestOneBit(movesBit);
      move = Long.numberOfTrailingZeros(flip);      
      movesBit = movesBit & (~flip);
      flip = mover.getFlip(move);
      HashMap.BoardInHash child = this.HASH_MAP.getStoredBoardNoUpdate(opponent & ~flip, player | flip);
      if (child == null) {
        continue;
      }

      CaseAnnotations annotations = new CaseAnnotations();
      annotations.eval = -child.lower / 100F;
//      annotations.isBestMove = ij.equals(bestIJ);
//      annotations.lower = -child.upper / 100F;
//      annotations.upper = -child.lower / 100F;
//      annotations.proofNumberCurEval = child.getDisproofNumberCurEval();
//      annotations.proofNumberNextEval = child.getDisproofNumberNextEval();
//      annotations.disproofNumberCurEval = child.getProofNumberCurEval();
//      annotations.disproofNumberNextEval = child.getProofNumberNextEval();
      annotations.otherAnnotations =
          ">" + Utils.prettyPrintDouble(-child.upper / 100) + "@" + child.depthUpper + "\n" +
          "<" + Utils.prettyPrintDouble(-child.lower / 100) + "@" + child.depthLower;
      ui.setAnnotations(annotations, new PositionIJ(move));
    }
  }
  
  private PositionIJ moveFromBoard(Board father, StoredBoard child) {
    long move = (child.getPlayer() | child.getOpponent()) & ~(father.getPlayer() | father.getOpponent());
    
    return BitPattern.bitToMove(move);
  }

  private PositionIJ findBestMove(StoredBoard[] evaluations) {
    double best = Double.POSITIVE_INFINITY;
    PositionIJ bestIJ = new PositionIJ(-1, -1);

    if (evaluations == null || evaluations.length == 0|| evaluations[0] == null) {
      return bestIJ;
    }
    for (StoredBoard evaluation : evaluations) {
      if (evaluation == null) {
        continue;
      }
      float eval = evaluation.getEval();// -(evaluation.getLowerBound() + evaluation.getUpperBound()) / 2.0F;
      PositionIJ ij = moveFromBoard(board, evaluation);
      if (eval < best) {
        best = eval;
        bestIJ = ij;
      }
    }
    return bestIJ;
  }
  
  private void evaluate() {
    if (ui.depth() <= 0) {
      showMCTSEvaluations();
      return;
    }
    if (ui.playBlackMoves() && !blackTurn) {
      return;
    }
    
    if (ui.playWhiteMoves() && blackTurn) {
      return;
    }
    future = executor.submit(this);
  }
  
  
  private void showMCTSEvaluations() {
    StoredBoard current = EVALUATOR.get(board);
    if (current == null || current.getChildren() == null) {
      showHashMapEvaluations();
      return;
    }
    StoredBoard[] children = current.getChildren();
    PositionIJ bestIJ = this.findBestMove(children);
    for (StoredBoard child : children) {
      if (child == null) {
        continue;
      }
      PositionIJ ij = moveFromBoard(board, child);

      CaseAnnotations annotations = new CaseAnnotations();
      annotations.eval = -child.getEval() / 100F;
      annotations.isBestMove = ij.equals(bestIJ);
//      annotations.lower = -child.getUpper() / 100F;
//      annotations.upper = -child.getLower() / 100F;
      annotations.nVisited = child.getDescendants();
//      annotations.proofNumberCurEval = child.getDisproofNumberCurEval();
//      annotations.proofNumberNextEval = child.getDisproofNumberNextEval();
//      annotations.disproofNumberCurEval = child.getProofNumberCurEval();
//      annotations.disproofNumberNextEval = child.getProofNumberNextEval();
      annotations.otherAnnotations =
        Utils.prettyPrintDouble(child.getEvalGoal() / 100) + "\n"
        + Utils.prettyPrintDouble(1 - child.probGreaterEqualEvalGoal) + " " + Utils.prettyPrintDouble(1 - child.probStrictlyGreaterEvalGoal) + "\n"
        + Utils.prettyPrintDouble(current.logDerivativePlayerVariates(child) + child.minLogDerivativeOpponentVariates) + " "
        + Utils.prettyPrintDouble(current.logDerivativeOpponentVariates(child) + child.minLogDerivativePlayerVariates) + "\n"
        + Utils.prettyPrintDouble(Math.exp(current.logDerivativePlayerVariates(child))) + " "
        + Utils.prettyPrintDouble(Math.exp(current.logDerivativeOpponentVariates(child)));
      ui.setAnnotations(annotations, ij);
    }
  }
  
  // The entry main() method
  public static void main(String[] args) throws FileNotFoundException, IOException {
    Main main = new Main();
    UI ui = new DesktopUI(main);
    main.setUI(ui);
  }
}
