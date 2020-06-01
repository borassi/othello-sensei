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
import java.awt.event.MouseEvent;
import java.util.ArrayList;

import javax.swing.SwingUtilities;

import board.Board;
import board.GetFlip;
import board.PossibleMovesFinderImproved;
import constants.Constants;
import endgametest.EndgameTest;
import evaluatedepthone.PatternEvaluatorImproved;
import evaluateposition.EvaluatorMCTS;
import evaluateposition.EvaluatorMidgame;
import evaluateposition.HashMap;
import evaluateposition.StoredBoard;
import helpers.Utils;
import java.io.FileNotFoundException;
import java.io.IOException;
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
  private int depth = -1;
  private final PatternEvaluatorImproved DEPTH_ONE_EVALUATOR;
  private final PossibleMovesFinderImproved POSSIBLE_MOVES_FINDER;
  private final EvaluatorMidgame EVALUATOR_MIDGAME;
  private EvaluatorMCTS EVALUATOR;
  private Thread evaluatorThread = null;
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
    this(1000000);
  }
  public Main(int hashMapSize) {
    DEPTH_ONE_EVALUATOR = PatternEvaluatorImproved.load();
    POSSIBLE_MOVES_FINDER = PossibleMovesFinderImproved.load();
    EVALUATOR_MIDGAME = new EvaluatorMidgame(DEPTH_ONE_EVALUATOR, new HashMap(hashMapSize * 2, hashMapSize));
    EVALUATOR = new EvaluatorMCTS(Constants.MCTS_SIZE, 2 * Constants.MCTS_SIZE, POSSIBLE_MOVES_FINDER, EVALUATOR_MIDGAME);
  }
  
  public synchronized void stop() {
    EVALUATOR.stop();
    if (evaluatorThread != null) {
      try {
        evaluatorThread.join();
        evaluatorThread = null;
      } catch (InterruptedException ex) {
        Logger.getLogger(Main.class.getName()).log(Level.SEVERE, null, ex);
      }
    }
  }
  
  public synchronized void setUI(UI ui) {
    this.ui = ui;
    newGame();
    setBoard(EndgameTest.readIthBoard(45), true); // 37
  }

  /**
   * Undos a move, if possible.
   */
  public synchronized void undo() {
    if (oldBoards.size() > 0) {
      board = oldBoards.remove(oldBoards.size() - 1);
      blackTurn = oldBlackTurns.remove(oldBlackTurns.size() - 1);
      ui.setCases(board, blackTurn);
    }
  }
  
  public synchronized void newGame() {
    stop();
    setBoard(new Board(), true);
  }
  
  public synchronized void setBoard(Board b, boolean blackTurn) {
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
   * @param e the MouseEvent
   */
  public synchronized void getClick(PositionIJ ij, MouseEvent e) {
    stop();
    if (SwingUtilities.isLeftMouseButton(e)) {
      playMoveIfPossible(ij);
      evaluate();
    } else if (SwingUtilities.isRightMouseButton(e)) {
      undo();
    }
  }
  
  @Override
  public void run() {
    int nUpdate = 0;
    while (true) {
      EVALUATOR.evaluatePosition(ui.depth(), updateTimes[Math.min(updateTimes.length-1, nUpdate++)]);
      updateEvals();
      if (EVALUATOR.status != EvaluatorMCTS.Status.STOPPED_TIME) {
        break;
      }
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
  private synchronized void playMoveIfPossible(PositionIJ ij) {
    Board oldBoard = board.deepCopy();
    try {
      board = POSSIBLE_MOVES_FINDER.moveIfPossible(board, ij);
    } catch (IllegalArgumentException ex) {
      return;
    }
    oldBoards.add(oldBoard);
    oldBlackTurns.add(blackTurn);
    blackTurn = !blackTurn;

    if (POSSIBLE_MOVES_FINDER.haveToPass(board)) {
      board = board.move(0);
      blackTurn = !blackTurn;
    }
    ui.setCases(board, blackTurn);
  }
  
  private void showHashMapEvaluations() {
    long[] moves = POSSIBLE_MOVES_FINDER.possibleMoves(board);
    StoredBoard current = EVALUATOR.get(board);
    if (current == null) {
      return;
    }
    StoredBoard[] evaluations = current.getChildren();
    PositionIJ bestIJ = this.findBestMove(evaluations);
    for (StoredBoard evaluation : evaluations) {
      if (evaluation != null) {
        PositionIJ ij = moveFromBoard(board, evaluation);

        CaseAnnotations annotations = new CaseAnnotations();
        annotations.eval = -evaluation.getEval() / 100F;
        annotations.isBestMove = ij.equals(bestIJ);
        annotations.lower = -evaluation.getUpper() / 100F;
        annotations.upper = -evaluation.getLower() / 100F;
        annotations.nVisited = evaluation.getDescendants();
        annotations.otherAnnotations =
            Utils.prettyPrintDouble(evaluation.getProofNumberCurEval()) + " " + Utils.prettyPrintDouble(evaluation.getProofNumberNextEval()) + "\n" +
            Utils.prettyPrintDouble(evaluation.getDisproofNumberNextEval()) + " " + Utils.prettyPrintDouble(evaluation.getDisproofNumberCurEval()) + "\n";
        ui.setAnnotations(annotations, ij);
      }
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
  
  private final int updateTimes[] = {50, 100, 500, 1000};
  
  private void evaluate() {
    if (ui.depth() <= 0) {
      showHashMapEvaluations();
      return;
    }
    if (ui.playBlackMoves() && !blackTurn) {
      return;
    }
    
    if (ui.playWhiteMoves() && blackTurn) {
      return;
    }

    EVALUATOR.setBoard(board);
    evaluatorThread = new Thread(this);
    evaluatorThread.start();
  }
  
  private void updateEvals() {
    StoredBoard[] evaluations = EVALUATOR.getEvaluations();

    PositionIJ bestIJ = findBestMove(evaluations);
     
    for (StoredBoard evaluation : evaluations) {
      PositionIJ ij = moveFromBoard(board, evaluation);
      CaseAnnotations annotations = new CaseAnnotations();
      annotations.eval = -evaluation.getEval() / 100F;
      annotations.isBestMove = ij.equals(bestIJ);
      annotations.nVisited = evaluation.getDescendants();
      annotations.otherAnnotations =
         Utils.prettyPrintDouble(evaluation.getProofNumberCurEval()) + " " + Utils.prettyPrintDouble(evaluation.getProofNumberNextEval()) + "\n" +
         Utils.prettyPrintDouble(evaluation.getDisproofNumberNextEval()) + " " + Utils.prettyPrintDouble(evaluation.getDisproofNumberCurEval()) + "\n";
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
