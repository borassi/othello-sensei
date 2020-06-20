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
import constants.Constants;
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
    return proofNumber(board.getPlayer(), board.getOpponent(), lower, approxEval);
  }
  public static double proofNumber(long player, long opponent, int lower, int approxEval) {
    return Math.max(1, Math.min(1.27E89, Math.exp(logProofNumber(player, opponent, lower, approxEval))));
  }
  public static double logProofNumber(long player, long opponent, int lower, int approxEval) {
    int empties = 64 - Long.bitCount(player | opponent);
    return -1.7147 + 0.6223 * empties + 1.0554 * Math.log(2 + GetMoves.getNMoves(opponent, player))
        +0.000603 * (lower - approxEval) - Math.max(Math.min(Math.log(1 - Gaussian.CDF(lower, approxEval, 400)), 0), -20);
  }
  public static double disproofNumber(Board board, int lower, int approxEval) {
    return disproofNumber(board.getPlayer(), board.getOpponent(), lower, approxEval);
  }
  public static double disproofNumber(long player, long opponent, int lower, int approxEval) {
    return Math.max(1, Math.min(1.27E89, Math.exp(logDisproofNumber(player, opponent, lower, approxEval))));
  }
  public static double logDisproofNumber(long player, long opponent, int lower, int approxEval) {
    int empties = 64 - Long.bitCount(player | opponent);
    return -3.9479 + 0.5727 * empties + 2.7668 * Math.log(1 + GetMoves.getNMoves(player, opponent))
        -0.0005 * (lower - approxEval) - Math.max(Math.min(Math.log(Gaussian.CDF(lower, approxEval, 400)), 0), -20);
  }
  
  public static void buildDataset(int minEmpties, int maxEmpties, double subsample) {
    EvaluatorMCTS evaluator = new EvaluatorMCTS(Constants.MCTS_SIZE, 2 * Constants.MCTS_SIZE);
    EvaluatorMidgame evaluatorMidgame = new EvaluatorMidgame();

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
      int approxEval = evaluatorMidgame.evaluatePosition(b.board, 2, -6400, 6400);
      int eval = evaluator.evaluatePosition(b.board, lower, lower + 1, Long.MAX_VALUE, Long.MAX_VALUE);
      double logNVisited = Math.log(evaluator.getNVisited());
      double predicted = eval > lower ?
          logProofNumber(b.board.getPlayer(), b.board.getOpponent(), lower, approxEval) :
          logDisproofNumber(b.board.getPlayer(), b.board.getOpponent(), lower, approxEval);
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
    buildDataset(14, 26, 0.001);
  }
}
