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

import android.os.Bundle;
import android.widget.ArrayAdapter;
import android.widget.Spinner;

import androidx.appcompat.app.AppCompatActivity;

import bitpattern.BitPattern;
import bitpattern.PositionIJ;

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
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

import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Comparator;
import java.util.PriorityQueue;
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
import ui.DesktopUI;
import ui.R;
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
  private final HashMap HASH_MAP;
  private final EvaluatorMCTS EVALUATOR;
  private final ExecutorService EXECUTOR = Executors.newSingleThreadExecutor();
  Future future = null;
  private long startTime;
  private final int updateTimes[] = {100, 1000};
  private static UI ui;
  
  /**
   * Creates a new UI and sets the initial position.
   */
  public Main(UI ui) {
    this.ui = ui;
    HASH_MAP = new HashMap(Constants.hashMapSize());
    EVALUATOR = new EvaluatorMCTS(Constants.MCTSSize(), 2 * Constants.MCTSSize(), HASH_MAP);
  }

  public void stop() {
    stopping = true;
    while (true) {
      if (future == null) {
        return;
      }
      EVALUATOR.stop();
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
    EVALUATOR.empty();
    HASH_MAP.reset();
    EvaluatorAlphaBeta.resetConstant();
    ui.setCases(board, blackTurn);
  }

  public void setEndgameBoard(int n) {
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
    EVALUATOR.evaluatePosition(b, ui.lower(), ui.upper(), ui.depth(), updateTime, ui.delta() * 100);
  }
  
  private boolean stopping = false;

  @Override
  public synchronized void run() {
    stopping = false;
    boolean finished = false;
    int nUpdate = 0;
    
    while (!stopping && !finished) {
      int updateTime = updateTimes[Math.min(updateTimes.length - 1, nUpdate)];
      evaluatePosition(board, updateTime, nUpdate == 0);
      showMCTSEvaluations();
      finished = EVALUATOR.getStatus() == Status.SOLVED || EVALUATOR.getStatus() == Status.STOPPED_POSITIONS || EVALUATOR.getStatus() == Status.FAILED;
      if (ui.delta() > 0 && finished) {
        for (StoredBoard child : EVALUATOR.get(board).getChildren()) {
          if (!child.isSolved()) {
            finished = false;
          }
        }
      }
      nUpdate++;
    }
    
    if ((ui.playBlackMoves() && blackTurn) || (ui.playWhiteMoves() && !blackTurn)) {
      Board evaluatedBoard = EVALUATOR.getFirstPosition().getBoard();
      if (evaluatedBoard.getPlayer() == this.board.getPlayer()
          && evaluatedBoard.getOpponent() == this.board.getOpponent()) {
        this.playMoveIfPossible(this.findBestMove(EVALUATOR.getFirstPosition().getChildren()));
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

      CaseAnnotations annotations = new CaseAnnotations(child, false);
//      annotations.eval = -child.lower / 100F;
////      annotations.isBestMove = ij.equals(bestIJ);
////      annotations.lower = -child.upper / 100F;
////      annotations.upper = -child.lower / 100F;
////      annotations.proofNumberCurEval = child.getDisproofNumberCurEval();
////      annotations.proofNumberNextEval = child.getDisproofNumberNextEval();
////      annotations.disproofNumberCurEval = child.getProofNumberCurEval();
////      annotations.disproofNumberNextEval = child.getProofNumberNextEval();
//      annotations.otherAnnotations =
//          ">" + Utils.prettyPrintDouble(-child.upper / 100) + "@" + child.depthUpper + "\n" +
//          "<" + Utils.prettyPrintDouble(-child.lower / 100) + "@" + child.depthLower;
      ui.setAnnotations(annotations, new PositionIJ(move));
    }
    ui.repaint();
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
      float eval = evaluation.getEval(EVALUATOR.getWeakLower(), EVALUATOR.getWeakUpper());
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
    startTime = System.currentTimeMillis();
    future = EXECUTOR.submit(this);
  }
  
  
  private void showMCTSEvaluations() {
    StoredBoard current = EVALUATOR.get(board);
    if (current == null) {
      showHashMapEvaluations();
      return;
    }
    ui.setExtras(
        new CaseAnnotations(current, EVALUATOR.getWeakLower(), EVALUATOR.getWeakUpper(), false),
        System.currentTimeMillis() - startTime);
    if (current.getChildren() == null) {
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

      CaseAnnotations annotations = new CaseAnnotations(child, EVALUATOR.getWeakLower(), EVALUATOR.getWeakUpper(), ij.equals(bestIJ));
      ui.setAnnotations(annotations, ij);
    }
    ui.repaint();
  }
}
