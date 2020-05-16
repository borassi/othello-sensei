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
package evaluateposition;

import board.Board;
import board.GetMoves;
import evaluatedepthone.BoardWithEvaluation;
import helpers.Gaussian;
import helpers.LoadDataset;
import java.util.ArrayList;

/**
 *
 * @author michele
 */
public class EndgameTimeEstimator {
  public static double proofNumber(Board board, int lower, int approxEval) {
    return Math.max(1, Math.min(1.27E89, Math.exp(logProofNumber(board, lower, approxEval))));
  }
  public static double logProofNumber(Board board, int lower, int approxEval) {
    int empties = board.getEmptySquares();
    return 0.3096 + 0.4138 * empties + 0.0993 * empties * Math.log(2 + GetMoves.getNMoves(board.getOpponent(), board.getPlayer()))
        +0.000597 * (lower - approxEval) - Math.max(Math.min(Math.log(1 - Gaussian.CDF(lower, approxEval, 400)), 10), -10);
  }
  public static double disproofNumber(Board board, int lower, int approxEval) {
    return Math.max(1, Math.min(1.27E89, Math.exp(logDisproofNumber(board, lower, approxEval))));
  }
  public static double logDisproofNumber(Board board, int lower, int approxEval) {
    int empties = board.getEmptySquares();
    
    return 1.5183 + 0.1152 * empties + 0.2137 * empties * Math.log(2 + GetMoves.getNMoves(board.getPlayer(), board.getOpponent()))
        -0.000496 * (lower - approxEval) - Math.max(Math.min(Math.log(Gaussian.CDF(lower, approxEval, 400)), 10), -10);
  }
  
  public static void buildDataset(int minEmpties, int maxEmpties, double subsample) {
    EvaluatorMidgame evaluator = new EvaluatorMidgame();
    ArrayList<BoardWithEvaluation> training = LoadDataset.loadTrainingSet(1990, 2000);
    double sumErrorSquared = 0;
    double nBoards = 0;
    
    System.out.println("Empties Lower approxEval nVisited logNVisited playerMoves opponentMoves isProof");
    for (BoardWithEvaluation b : training) {
      int empties = b.board.getEmptySquares();
      if (empties < minEmpties || empties > maxEmpties || Math.random() > subsample) {
        continue;
      }
      int lower = (int) ((Math.random() - 0.5) * 12800);
      int approxEval = evaluator.evaluatePosition(b.board, 4, -6400, 6400);
      evaluator.resetNVisitedPositions();
      int eval = evaluator.evaluatePosition(b.board, empties, lower, lower+1);
      double logNVisited = Math.log(evaluator.getNVisited());
      double predicted = eval > lower ? logProofNumber(b.board, lower, approxEval) : logDisproofNumber(b.board, lower, approxEval);
      if (logNVisited > 0) {
        System.out.println(
            empties + " " + lower + " " + approxEval + " " + evaluator.getNVisited() +
            " " + logNVisited + " " + GetMoves.getNMoves(b.board.getPlayer(), b.board.getOpponent()) + " " +
            GetMoves.getNMoves(b.board.getOpponent(), b.board.getPlayer()) + " " + (eval > lower));
        sumErrorSquared += (predicted - logNVisited) * (predicted - logNVisited);
      }
      nBoards++;
    }
    System.out.println("Average squared error: " + Math.sqrt(sumErrorSquared / nBoards));
  }
  
  public static void main(String args[]) {
    buildDataset(11, 20, 0.01);
  }
}
