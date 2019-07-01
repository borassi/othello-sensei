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
import java.awt.Color;
import java.awt.event.MouseEvent;
import java.util.ArrayList;

import javax.swing.SwingUtilities;

import board.Board;
import board.PossibleMovesFinderImproved;
import evaluatedepthone.BoardWithEvaluation;
import evaluatedepthone.MultilinearRegression;
import evaluateposition.EvaluatorAlphaBeta;
import evaluateposition.EvaluatorLastMoves;
import evaluateposition.EvaluatorMCTS;
import evaluateposition.StoredBoard;
import helpers.LoadDataset;
import it.unimi.dsi.fastutil.objects.ObjectArrayList;
import java.io.DataOutputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;
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
  private final MultilinearRegression DEPTH_ONE_EVALUATOR = MultilinearRegression.load(EvaluatorAlphaBeta.DEPTH_ONE_EVALUATOR_FILEPATTERN);
  private final PossibleMovesFinderImproved POSSIBLE_MOVES_FINDER = PossibleMovesFinderImproved.load();
  private final EvaluatorLastMoves EVALUATOR_LAST_MOVES = new EvaluatorLastMoves();
  /**
   * The board, as a pair of bitpattern.
   */
  Board board;
  /**
   * The user interface.
   */
  UI ui;
  
  EvaluatorMCTS evaluator;
  
  /**
   * Creates a new UI and sets the initial position.
   */
  public Main() {
    ui = new UI(this);
    changeDepth(ui.depth());
    board = new Board();
//    board = new Board("XXXXXXXOOOOOXX-O-OOOXXOX-OXOXOXXOXXXOXXX--XOXOXX-XXXOOO--OOOOO--", true);
//    board = new Board("XXXXXXXOOOOOXX-O-OOOXXOX-OXOXOXXOXXXOXXX--XOXOXX-XXXOOO--OOOOO--", true);
    board = new Board("--XXXXX--OOOXX-O-OOOXXOX-OXOXOXXOXXXOXXX--XOXOXX-XXXOOO--OOOOO--", true);
    board = new Board("-XXXXX--O-XXOOOXOOOOOOXXOOOOOXXXOOOOXXX-OOOXO-X--OOOO--X-XXXXXX-", true);
    board = new Board("----OX----OOXX---OOOXX-XOOXXOOOOOXXOXXOOOXXXOOOOOXXXXOXO--OOOOOX", true);
    board = new Board("-XXXXX--O-XXOOOXOOOOOOXXOOOOOXXXOOOOXXX-OOOXO-X--OOOO--X-XXXXXX-", true);
    board = new Board("----X------XXXO--OOOXXXXXOOOOXXO-XXOOXXOOOXOXXXXOOOXX---X-XXXX--", false);
    board = new Board("--XXXXX---XXXX---OOOXX---OOXXXX--OOXXXO-OOOOXOO----XOX----XXXXX-", true);
    ui.setCases(board, blackTurn);
    ui.update(ui.getGraphics());
  }

  public final void changeDepth(int depth) {
    if (depth != 0) {
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
    this.board = new Board();
    this.blackTurn = true;
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
      if (ui.depth() > 0) {
        evaluate();
      } else {
        showHashMapEvaluations();
      }
    } else if (SwingUtilities.isRightMouseButton(e)) {
      undo();
    }
//    System.out.println(DEPTH_ONE_EVALUATOR.evalVerbose(board));
  }
  
  public void retrain() {
//    depthOneEvaluator = new MultilinearRegression();
    ArrayList<BoardWithEvaluation> trainingSet = LoadDataset.loadOMGSet(184);
    trainingSet.addAll(LoadDataset.loadTrainingSet());
//    DEPTH_ONE_EVALUATOR.train(trainingSet, 0.5F, 2);
    DEPTH_ONE_EVALUATOR.train(trainingSet, 0.2F, 5);
    DEPTH_ONE_EVALUATOR.train(trainingSet, 0.1F, 2);
//    DEPTH_ONE_EVALUATOR.train(trainingSet, 0.05F, 2);
    DEPTH_ONE_EVALUATOR.save(EvaluatorAlphaBeta.DEPTH_ONE_EVALUATOR_FILEPATTERN);
  }
  
  
  public void showHashMapEvaluations() {
    long[] moves = POSSIBLE_MOVES_FINDER.possibleMoves(board);
    ObjectArrayList<StoredBoard> evaluations = new ObjectArrayList<>();
    
    for (long move : moves) {
      Board next = board.move(move);
      StoredBoard evaluation = evaluator.get(next);
      evaluations.add(evaluation);
      if (evaluation != null) {
        PositionIJ ij = BitPattern.bitToMove(board.moveToBit(move));

        ui.setAnnotationsColor(Color.BLACK, ij);
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
      ui.setAnnotationsColor(Color.RED, bestIJ);
    }
  }
  
  public PositionIJ moveFromBoard(Board father, StoredBoard child) {
    long move = (child.player | child.opponent) & ~(father.getPlayer() | father.getOpponent());
    
    return BitPattern.bitToMove(move);
  }

  public PositionIJ findBestMove(ObjectArrayList<StoredBoard> evaluations) {
    double best = Double.POSITIVE_INFINITY;
    double bestUpper = Double.POSITIVE_INFINITY;
    double bestPlayerVariates = Double.POSITIVE_INFINITY;
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
           evaluation.lower < bestPlayerVariates)) {
        best = eval;
        bestUpper = evaluation.getUpperBound();
        bestPlayerVariates = evaluation.lower;
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
    ObjectArrayList<StoredBoard> evaluations = evaluator.evaluatePositionAll(
      board, ui.depth());

    for (StoredBoard evaluation : evaluations) {
      int eval = -evaluation.getEval();// -(evaluation.getLowerBound() + evaluation.getUpperBound()) / 2.0F;
      PositionIJ ij = moveFromBoard(board, evaluation);
      ui.setAnnotationsColor(Color.BLACK, ij);
      ui.setAnnotations(String.format("%.2f\n%.2f   %.2f", eval / 100F, -evaluation.getUpperBound() / 100F,
          -evaluation.getLowerBound() / 100F), ij);
    }
    PositionIJ bestIJ = findBestMove(evaluations);
//    System.out.println(evaluator.getNVisitedPositionsByEvaluator());

    if (bestIJ.i >= 0) {      
      if (ui.playBlackMoves() || ui.playWhiteMoves()) {
        playMoveIfPossible(bestIJ);
      }
      ui.setAnnotationsColor(Color.RED, bestIJ);
    }
  }
  
  // The entry main() method
  public static void main(String[] args) throws FileNotFoundException, IOException {
    Main main = new Main();
  }

  public void improveDataset() {
    for (int i = 0; i < 1000; i++) {
      try {
        String file = "tmp/weird_positions_round_6_" + i + ".tmp";
        DataOutputStream stream = new DataOutputStream(new FileOutputStream(file));
        for (int j = 0; j < 60; j++) {
          PositionIJ move = findBestMove(evaluator.evaluatePositionAll(board, ui.depth()));
          if (move.i >= 0) {
            playMoveIfPossible(move);
          } else {
            newGame();
          }
          ObjectArrayList<Board> wrongPositions = this.evaluator.getWeirdPositions(20);
          for (Board wrongPosition : wrongPositions) {
            stream.writeLong(LoadDataset.littleToBigEndianLong(wrongPosition.getPlayer()));
            stream.writeLong(LoadDataset.littleToBigEndianLong(wrongPosition.getOpponent()));
          }
        }
        stream.flush();
        stream.close();
//        break;
      } catch (IOException ex) {
        Logger.getLogger(Main.class.getName()).log(Level.SEVERE, null, ex);
      }
    }
  }
}
