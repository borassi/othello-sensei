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
package helpers;

import board.Board;
import board.PossibleMovesFinderImproved;
import evaluateposition.EvaluatorMCTS;
import evaluateposition.StoredBoard;
import java.util.ArrayList;

public class SelfPlay extends Thread {
  
  EvaluatorMCTS player1, player2;
  final PossibleMovesFinderImproved pmf;
  final int nRandMoves;
  ArrayList<Board> game;
  
  public SelfPlay(EvaluatorMCTS player1, EvaluatorMCTS player2,
                  PossibleMovesFinderImproved pmf, int nRandMoves) {
    this.player1 = player1;
    this.player2 = player2;
    this.pmf = pmf;
    this.nRandMoves = nRandMoves;
  }
  
  public synchronized Board bestMove(Board b, EvaluatorMCTS player) {
    player.evaluatePosition(b, 100000000, 1000);
    StoredBoard[] evaluations = player.getFirstPosition().getChildren();
    double best = Double.POSITIVE_INFINITY;
//    double bestUpper = Double.POSITIVE_INFINITY;
//    double bestPlayerVariates = Double.POSITIVE_INFINITY;
    Board next = null;

    for (StoredBoard evaluation : evaluations) {
      if (evaluation == null) {
        continue;
      }
      int eval = evaluation.getEval();
      if (eval < best) {// ||
//          (eval == best && evaluation.getUpperBound() < bestUpper) ||
//          (eval == best && evaluation.getUpperBound() == bestUpper &&
//           evaluation.lower < bestPlayerVariates)) {
        best = eval;
//        bestUpper = evaluation.getUpperBound();
//        bestPlayerVariates = evaluation.lower;
        next = evaluation.getBoard();
      }
    }
    return next;
  }
  
  public synchronized ArrayList<Board> get() {
    return this.game;
  }
  
  @Override
  public synchronized void run() {
    Board b = new Board();
    game = new ArrayList<>();
    
    for (int i = 0; i < nRandMoves; ++i) {
      long[] moves = pmf.possibleMovesAdvanced(b.getPlayer(), b.getOpponent());
      if (moves.length == 0) {
        b = new Board();
        continue;
      }
      b.moveInPlace(moves[(int) (Math.random() * moves.length)]);
    }
    
    while (b != null) {
      game.add(b);
      b = bestMove(b, player1);
      EvaluatorMCTS tmp = player1;
      player1 = player2;
      player2 = tmp;
    }
  }

//  public static void main(String args[]) {
//    PossibleMovesFinderImproved pmf = new PossibleMovesFinderImproved();
//    EvaluatorLastMoves lastMoves = new EvaluatorLastMoves();
//    MultilinearRegressionImproe oldMR = MultilinearRegression.load("coefficients/multilinear_regression_improved_OK.sar");
//    MultilinearRegression newMR = MultilinearRegression.load("coefficients/multilinear_regression_improved.sar");
//    EvaluatorMCTS oldEval = new EvaluatorMCTS(50000, 100000, pmf, oldMR, lastMoves);
//    EvaluatorMCTS newEval = new EvaluatorMCTS(50000, 100000, pmf, newMR, lastMoves);
//
//    long t = System.currentTimeMillis();
//    for (int i = 0; i < 1000; ++i) {
//      SelfPlay p = new SelfPlay(
//          i % 2 == 0 ? oldEval : newEval,
//          i % 2 == 0 ? newEval : oldEval, pmf, 0);
//      p.start();
//      try {
//        p.join();
//      } catch (InterruptedException ex) {
//        Logger.getLogger(SelfPlay.class.getName()).log(Level.SEVERE, null, ex);
//      }
//      ObjectArrayList<Board> result = p.get();
//      System.out.println(((result.size() + i) % 2 == 0 ? 1 : -1) * 
//          BitPattern.getEvaluationGameOver(result.top()));
////      System.out.println(i + " " + (System.currentTimeMillis() - t));
////      for (Board b : result) {
////        System.out.println(b);
////      }
////      break;
//    }
//  }
}
