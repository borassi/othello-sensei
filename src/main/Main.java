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
import board.PossibleMovesFinderImproved;
import evaluatedepthone.PatternEvaluatorImproved;
import evaluateposition.EvaluatorLastMoves;
import evaluateposition.EvaluatorMCTS;
import evaluateposition.StoredBoard;
import java.io.FileNotFoundException;
import java.io.IOException;
import ui.DesktopUI;
import ui.UI;

/**
 * @author michele
 * The main function of the program.
 * Connects the board and the evaluation with the UI.
 */
public class Main {
  ArrayList<Boolean> oldBlackTurns = new ArrayList<>();
  ArrayList<Board> oldBoards = new ArrayList<>();
  boolean blackTurn = true;
  private int depth = -1;
  private final PatternEvaluatorImproved DEPTH_ONE_EVALUATOR;
  private final PossibleMovesFinderImproved POSSIBLE_MOVES_FINDER;
  private final EvaluatorLastMoves EVALUATOR_LAST_MOVES;
  /**
   * The board, as a pair of bitpattern.
   */
  Board board;
  /**
   * The user interface.
   */
  UI ui;
  public static FileAccessor fileAccessor = new DesktopFileAccessor();
  EvaluatorMCTS evaluator;
  
  /**
   * Creates a new UI and sets the initial position.
   */
  public Main() {
    DEPTH_ONE_EVALUATOR = PatternEvaluatorImproved.load();
    POSSIBLE_MOVES_FINDER = PossibleMovesFinderImproved.load();
    EVALUATOR_LAST_MOVES = EvaluatorLastMoves.load();
  }
  
  public void setUI(UI ui) {
    this.ui = ui;
    newGame();
    setBoard(new Board("--OOO-------XX-OOOOOOXOO-OOOOXOOX-OOOXXO---OOXOO---OOOXO--OOOO--", true), true); // 43
  }

  public final void changeDepth(int depth) {
    this.depth = depth;
    if (depth > 0) {
      evaluator = new EvaluatorMCTS(2 * depth, depth, POSSIBLE_MOVES_FINDER, DEPTH_ONE_EVALUATOR, EVALUATOR_LAST_MOVES);
    }
  }

  /**
   * Undos a move, if possible.
   */
  public void undo() {
    if (oldBoards.size() > 0) {
      board = oldBoards.remove(oldBoards.size() - 1);
      blackTurn = oldBlackTurns.remove(oldBlackTurns.size() - 1);
      ui.setCases(board, blackTurn);
    }
  }
  
  /**
   * Plays a move at the coordinates (i, j), if it is possible.
   * 
   * @param ij the row and the column (from 0 to 7)
   */
  public void playMoveIfPossible(PositionIJ ij) {
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
  
  public void newGame() {
    setBoard(new Board(), true);
  }
  
  public void setBoard(Board b, boolean blackTurn) {
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
  public void getClick(PositionIJ ij, MouseEvent e) {
    if (SwingUtilities.isLeftMouseButton(e)) {
      playMoveIfPossible(ij);
      evaluate();
    } else if (SwingUtilities.isRightMouseButton(e)) {
      undo();
    }
  }
  
  
  public void showHashMapEvaluations() {
    long[] moves = POSSIBLE_MOVES_FINDER.possibleMoves(board);
    ArrayList<StoredBoard> evaluations = new ArrayList<>();
    
    for (long move : moves) {
      Board next = board.move(move);
      StoredBoard evaluation = evaluator.get(next);
      evaluations.add(evaluation);
      if (evaluation != null) {
        PositionIJ ij = BitPattern.bitToMove(board.moveToBit(move));

        String str = String.format("%.2f\n%d\n", -evaluation.getEval() / 100F, evaluation.descendants);
        str += String.format("%.2f %.2f\n", -evaluation.bestVariationOpponent / 100F,
            -evaluation.bestVariationPlayer / 100F);
        str += String.format("%.2f   %.2f\n", -evaluation.getUpperBound() / 100F, -evaluation.getLowerBound() / 100F);
        
        for (int i = 0; i < evaluation.samples.length / 3 * 3; i += 3) {
          str += String.format("%.1f  %.1f  %.1f\n", -evaluation.samples[i] / 100F,
              -evaluation.samples[i+1] / 100F, -evaluation.samples[i+2] / 100F);
        }
        ui.setAnnotations(str, ij);
      }
    }
    PositionIJ bestIJ = this.findBestMove(evaluations);
    
    if (bestIJ.i >= 0) {
      ui.setBestMove(bestIJ);
    }
  }
  
  public PositionIJ moveFromBoard(Board father, StoredBoard child) {
    long move = (child.player | child.opponent) & ~(father.getPlayer() | father.getOpponent());
    
    return BitPattern.bitToMove(move);
  }

  public PositionIJ findBestMove(ArrayList<StoredBoard> evaluations) {
    double best = Double.POSITIVE_INFINITY;
    double bestUpper = Double.POSITIVE_INFINITY;
    double bestOpponentVariates = Double.POSITIVE_INFINITY;
    PositionIJ bestIJ = new PositionIJ(-1, -1);

    if (evaluations == null || evaluations.isEmpty() || evaluations.get(0) == null) {
      return bestIJ;
    }
    for (StoredBoard evaluation : evaluations) {
      if (evaluation == null) {
        continue;
      }
      float eval = evaluation.getEval();// -(evaluation.getLowerBound() + evaluation.getUpperBound()) / 2.0F;
      PositionIJ ij = moveFromBoard(board, evaluation);
      if (eval < best ||
          (eval == best && evaluation.getUpperBound() < bestUpper) ||
          (eval == best && evaluation.getUpperBound() == bestUpper &&
           evaluation.upper < bestOpponentVariates)) {
        best = eval;
        bestUpper = evaluation.getUpperBound();
        bestOpponentVariates = evaluation.upper;
        bestIJ = ij;
      }
    }
    return bestIJ;
  }
  
  public void evaluate() {
    if (ui.playBlackMoves() && !blackTurn) {
      return;
    }
    
    if (ui.playWhiteMoves() && blackTurn) {
      return;
    }
    if (ui.depth() <= 0) {
      showHashMapEvaluations();
      return;
    }
    if (ui.depth() != depth) {
      this.changeDepth(ui.depth());
    }
    ArrayList<StoredBoard> evaluations = evaluator.evaluatePositionAll(
      board, ui.depth());

    for (StoredBoard evaluation : evaluations) {
      int eval = -evaluation.getEval();// -(evaluation.getLowerBound() + evaluation.getUpperBound()) / 2.0F;
      PositionIJ ij = moveFromBoard(board, evaluation);
      ui.setAnnotations(String.format("%.2f\n%.2f   %.2f", eval / 100F, -evaluation.getUpperBound() / 100F,
          -evaluation.getLowerBound() / 100F), ij);
    }
    PositionIJ bestIJ = findBestMove(evaluations);
//    System.out.println(evaluator.getNVisitedPositionsByEvaluator());

    if (bestIJ.i >= 0) {      
      if (ui.playBlackMoves() || ui.playWhiteMoves()) {
        playMoveIfPossible(bestIJ);
      }
      ui.setBestMove(bestIJ);
    }
  }
  
  // The entry main() method
  public static void main(String[] args) throws FileNotFoundException, IOException {
    Main main = new Main();
    UI ui = new DesktopUI(main);
    main.setUI(ui);
  }

//  public void improveDataset() {
//    for (int i = 0; i < 1000; i++) {
//      try {
//        String file = "tmp/weird_positions_round_6_" + i + ".tmp";
//        DataOutputStream stream = new DataOutputStream(new FileOutputStream(file));
//        for (int j = 0; j < 60; j++) {
//          PositionIJ move = findBestMove(evaluator.evaluatePositionAll(board, ui.depth()));
//          if (move.i >= 0) {
//            playMoveIfPossible(move);
//          } else {
//            newGame();
//          }
//        }
//        stream.flush();
//        stream.close();
////        break;
//      } catch (IOException ex) {
//        Logger.getLogger(Main.class.getName()).log(Level.SEVERE, null, ex);
//      }
//    }
//  }
}
