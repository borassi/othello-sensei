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
public class EndgameTimeEstimator extends EndgameTimeEstimatorInterface {
  public class EndgameTimeEstimatorRequest {
    long player;
    long opponent;
    int lower;
    int approxEval;
    boolean proof;
    
    public EndgameTimeEstimatorRequest(long player, long opponent, int lower, int approxEval, boolean proof) {
      this.player = player;
      this.opponent = opponent;
      this.lower = lower;
      this.approxEval = approxEval;
      this.proof = proof;
    }
  }
  public static ArrayList<EndgameTimeEstimatorRequest> requests = new ArrayList<>();
  public static boolean LOG = false;
  public static boolean LOG_PROBABILITY;

  @Override
  public double proofNumber(long player, long opponent, int lower, int approxEval) {
    if (LOG && Math.random() < Math.pow(1.5, Long.bitCount(~(player | opponent))) / 1000 / 30000) {
      requests.add(new EndgameTimeEstimatorRequest(player, opponent, lower, approxEval, true));
    }
    return Math.max(1, Math.min(1.27E89, Math.exp(logProofNumber(player, opponent, lower, approxEval))));
  }

  public double logProofNumber(long player, long opponent, int lower, int approxEval) {
    int empties = 64 - Long.bitCount(player | opponent);
//    return 0.0084 + 0.7193 * empties -0.0861 * Math.log(2 + GetMoves.getNMoves(player, opponent))
//        -0.2998 * Math.log(2 + GetMoves.getNMoves(opponent, player))
//        + 0.000786 * (lower - approxEval);
    return -1.7147 + 0.6223 * empties + 1.0554 * Math.log(2 + GetMoves.getNMoves(opponent, player))
        +0.000603 * (lower - approxEval);
  }

  @Override
  public double disproofNumber(long player, long opponent, int lower, int approxEval) {
    if (LOG && Math.random() < Math.pow(1.5, Long.bitCount(~(player | opponent))) / 1000 / 30000) {
      requests.add(new EndgameTimeEstimatorRequest(player, opponent, lower, approxEval, false));
    }
    return Math.max(1, Math.min(1.27E89, Math.exp(logDisproofNumber(player, opponent, lower, approxEval))));
  }

  public double logDisproofNumber(long player, long opponent, int lower, int approxEval) {
    int empties = 64 - Long.bitCount(player | opponent);
//    return -0.7814 + 0.7461 * empties + 0.0588 * Math.log(2 + GetMoves.getNMoves(player, opponent))
//        -0.0773 * Math.log(2 + GetMoves.getNMoves(opponent, player))
//        -0.000840 * (lower - approxEval);
    return -3.9479 + 0.5727 * empties + 2.7668 * Math.log(1 + GetMoves.getNMoves(player, opponent))
        -0.0005 * (lower - approxEval);
  }
  
  public static void buildDataset(int minEmpties, int maxEmpties, double subsample) {
    EvaluatorMidgame evaluatorMidgame = new EvaluatorMidgame();
    EvaluatorMCTS evaluator = new EvaluatorMCTS(Constants.MCTS_SIZE, 2 * Constants.MCTS_SIZE, evaluatorMidgame);
    EndgameTimeEstimator endgameTimeEstimator = new EndgameTimeEstimator();

    ArrayList<BoardWithEvaluation> training = LoadDataset.loadTrainingSet(1990, 1990);
    double sumErrorSquared = 0;
    double nBoards = 0;
    
    System.out.println("OrigEmpties Empties Lower approxEval nVisited logNVisited playerMoves opponentMoves requestProof eval solved proof disproof");
    for (BoardWithEvaluation b : training) {
      int empties = b.board.getEmptySquares();
      if (empties < minEmpties || empties > maxEmpties || Math.random() > subsample) {
        continue;
      }
      requests.clear();
      LOG = true;
      evaluator.evaluatePosition(b.board, -6300, 6300, Long.MAX_VALUE, 20 * 1000, true);
      LOG = false;
      for (EndgameTimeEstimatorRequest request : requests) {
        evaluator.reset();
        evaluatorMidgame.resetHashMap();
        int eval = evaluator.evaluatePosition(
            new Board(request.player, request.opponent), request.lower, request.lower, Long.MAX_VALUE, 20 * 1000, true);
        double nVisited = evaluator.getNVisited();
//        double logNVisited = Math.log(evaluator.getNVisited() + );
//        double predicted = eval > request.lower ?
//            endgameTimeEstimator.logProofNumber(b.board.getPlayer(), b.board.getOpponent(), request.lower, request.approxEval) :
//            endgameTimeEstimator.logDisproofNumber(b.board.getPlayer(), b.board.getOpponent(), request.lower, request.approxEval);
//        if (logNVisited > 0) {
          System.out.println(
              empties + " " + new Board(request.player, request.opponent).getEmptySquares()
              + " " + request.lower + " " + request.approxEval + " " + nVisited +
              " " + Math.log(nVisited) + " " + GetMoves.getNMoves(b.board.getPlayer(), b.board.getOpponent()) + " " +
              GetMoves.getNMoves(b.board.getOpponent(), b.board.getPlayer()) + " " + request.proof + " " + eval
              + " " + (evaluator.getStatus() == EvaluatorMCTS.Status.SOLVED) + " " + evaluator.firstPosition.proofNumberGreaterEqual + " " + evaluator.firstPosition.disproofNumberStrictlyGreater
          );
//          sumErrorSquared += (predicted - logNVisited) * (predicted - logNVisited);
//        }
        
      }
      nBoards++;
    }
    System.out.println("Average squared error: " + Math.sqrt(sumErrorSquared / nBoards));
  }
  
  public static void main(String args[]) {
    buildDataset(8, 28, 0.03);
  }
}
