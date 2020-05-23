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
import endgametest.EndgameTest;
import evaluatedepthone.PatternEvaluatorImproved;
import evaluateposition.EvaluatorMCTS;
import evaluateposition.EvaluatorMidgame;
import evaluateposition.HashMap;
import evaluateposition.StoredBoard;
import java.io.FileNotFoundException;
import java.io.IOException;
import ui.CaseAnnotations;
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
  private final EvaluatorMidgame EVALUATOR_MIDGAME;
  private EvaluatorMCTS EVALUATOR;
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
    EVALUATOR = new EvaluatorMCTS(1000, 1000000, 2000000, POSSIBLE_MOVES_FINDER, EVALUATOR_MIDGAME);
//    EVALUATOR = new EvaluatorMCTS(1000000000L, 4000000, 1000000000L);
  }
  
  public void setUI(UI ui) {
    this.ui = ui;
    newGame();
    setBoard(EndgameTest.readIthBoard(44), true); // 37
  }

  public final void changeDepth(int depth) {
    this.depth = depth;
    if (depth > 0) {
      EVALUATOR = new EvaluatorMCTS(depth, 1000000, 2000000, POSSIBLE_MOVES_FINDER, EVALUATOR_MIDGAME);
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
  
  public static String prettyPrintDouble(double l) {
    if (l < 1.E3) {
      return String.format("%.0f", l);
    } else if (l == Double.POSITIVE_INFINITY) {
      return "+Inf";
    } else if (l == Double.NEGATIVE_INFINITY) {
      return "-Inf";
    } else if (l < 1.E4) {
      return String.format("%.1fK", l / 1.E3);
    } else if (l < 1.E6) {
      return String.format("%.0fK", l / 1.E3);
    } else if (l < 1.E7) {
      return String.format("%.1fM", l / 1.E6);
    } else if (l < 1.E9) {
      return String.format("%.0fM", l / 1.E6);
    } else if (l < 1.E10) {
      return String.format("%.1fG", l / 1.E9); 
    } else if (l < 1.E12) {
      return String.format("%.0fG", l / 1.E9);
    } else if (l < 1.E13) {
      return String.format("%.1fT", l / 1.E12); 
    } else {
      return String.format("%.0fT", l / 1.E12);
    }
  }
  
  public void showHashMapEvaluations() {
    long[] moves = POSSIBLE_MOVES_FINDER.possibleMoves(board);
    ArrayList<StoredBoard> evaluations = new ArrayList<>();
    
    for (long move : moves) {
      Board next = board.move(move);
      StoredBoard evaluation = EVALUATOR.get(next);
      evaluations.add(evaluation);
    }
    PositionIJ bestIJ = this.findBestMove(evaluations);
    for (StoredBoard evaluation : evaluations) {
      if (evaluation != null) {
        PositionIJ ij = moveFromBoard(board, evaluation);

        CaseAnnotations annotations = new CaseAnnotations();
        annotations.eval = -evaluation.getEval() / 100F;
        annotations.isBestMove = ij.equals(bestIJ);
//        annotations.safeEval = -evaluation.getSafeEval() / 100F;
//        annotations.costUntilLeafAttack = StoredBoard.edgeCost(EVALUATOR.get(board), evaluation, true) + evaluation.costUntilLeafDefense;
//        annotations.costUntilLeafDefense = StoredBoard.edgeCost(EVALUATOR.get(board), evaluation, false) + evaluation.costUntilLeafAttack;
//        annotations.edgeCostAttack = StoredBoard.edgeCost(EVALUATOR.get(board), evaluation, true);
//        annotations.edgeCostDefense = StoredBoard.edgeCost(EVALUATOR.get(board), evaluation, false);
        annotations.lower = -evaluation.upper / 100F;
        annotations.upper = -evaluation.lower / 100F;
//        annotations.bestVariationPlayer = -evaluation.bestVariationOpponent / 100F;
//        annotations.bestVariationOpponent = -evaluation.bestVariationPlayer / 100F;
        annotations.nVisited = evaluation.descendants;
//        annotations.otherAnnotations = evaluation.getInterestingProbabilities((Math.random() > 0.5 ? -1 : 1) * EVALUATOR.firstPosition.eval);
//            String.format(
//            "%d %d\n", EVALUATOR.getEvalGoal() / 100, evaluation.expectedToSolve);
        annotations.otherAnnotations =
            prettyPrintDouble(evaluation.proofNumberCurEval) + " " + prettyPrintDouble(evaluation.proofNumberNextEval) + "\n" +
            prettyPrintDouble(evaluation.disproofNumberNextEval) + " " + prettyPrintDouble(evaluation.disproofNumberCurEval) + "\n";
        
//        for (int i = 0; i < evaluation.samples.length / 3 * 3; i += 3) {
//          annotations.otherAnnotations += String.format("%.1f  %.1f  %.1f\n", -evaluation.samples[i] / 100F,
//              -evaluation.samples[i+1] / 100F, -evaluation.samples[i+2] / 100F);
//        }
        ui.setAnnotations(annotations, ij);
      }
    }
    StoredBoard cur = EVALUATOR.get(board);
//    System.out.println(cur.eval + " (" + cur.lower + ", " + cur.upper + ")");
//    for (int i = 63; i >= 0; --i) {
////      String.format("%d:%.2g (%.2g)\n", -offsetToEval(i) / 100, 1 - probEvalGreaterThan[i],
////            fathers.size() > 0 ? maxLogDerivativeDescendants(i, fathers.get(0)) : 0);
//      System.out.println(-StoredBoard.offsetToEval(i) / 100 + " " + (1 - cur.probEvalGreaterThan[i]) + " " +
//          (cur.fathers.size() > 0 ? cur.maxLogDerivativeDescendants(i, cur.fathers.get(0)) : 0) + " " + cur.maxChildrenLogDerivative[i]);
//    }
//    
//    if (bestIJ.i >= 0) {
//      ui.setBestMove(bestIJ);
//    }
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
      if (eval < best) {
//        || TODO: FIX!!!
//          (eval == best && evaluation.getUpperBound() < bestUpper) ||
//          (eval == best && evaluation.getUpperBound() == bestUpper &&
//           evaluation.upper < bestOpponentVariates)) {
        best = eval;
//        bestUpper = evaluation.getUpperBound();
//        bestOpponentVariates = evaluation.upper;
        bestIJ = ij;
      }
    }
    return bestIJ;
  }
  
  public void evaluate() {
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
    if (ui.depth() != depth) {
      this.changeDepth(ui.depth());
    }
    ArrayList<StoredBoard> evaluations = EVALUATOR.evaluatePositionAll(
      board, ui.depth());

    PositionIJ bestIJ = findBestMove(evaluations);
    
    if (bestIJ.i >= 0) {      
      if (ui.playBlackMoves() || ui.playWhiteMoves()) {
        playMoveIfPossible(bestIJ);
        return;
      }
    }
 
    for (StoredBoard evaluation : evaluations) {
      PositionIJ ij = moveFromBoard(board, evaluation);
      CaseAnnotations annotations = new CaseAnnotations();
      annotations.eval = -evaluation.getEval() / 100F;
//      annotations.safeEval = -evaluation.getSafeEval() / 100F;
//      annotations.costUntilLeafAttack = evaluation.disproofNumberCurEval;
//      annotations.costUntilLeafDefense = evaluation.proofNumberCurEval;
//      annotations.edgeCostAttack = StoredBoard.edgeCost(EVALUATOR.get(board), evaluation, true);
//      annotations.edgeCostDefense = StoredBoard.edgeCost(EVALUATOR.get(board), evaluation, true);
      annotations.isBestMove = ij.equals(bestIJ);
//      annotations.lower = -evaluation.upper / 100F;
//      annotations.upper = -evaluation.lower / 100F;
//      annotations.bestVariationPlayer = -evaluation.bestVariationOpponent / 100F;
//      annotations.bestVariationOpponent = -evaluation.bestVariationPlayer / 100F;
      annotations.nVisited = evaluation.descendants;
      annotations.otherAnnotations =
          prettyPrintDouble(evaluation.proofNumberCurEval) + " " + prettyPrintDouble(evaluation.proofNumberNextEval) + "\n" +
          prettyPrintDouble(evaluation.disproofNumberNextEval) + " " + prettyPrintDouble(evaluation.disproofNumberCurEval) + "\n";

      ui.setAnnotations(annotations, ij);
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
