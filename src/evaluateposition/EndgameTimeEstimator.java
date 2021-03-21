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
import evaluatedepthone.FindStableDisks;
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

  @Override
  public double logProofNumber(long player, long opponent, int lower, int approxEval) {
    int empties = 64 - Long.bitCount(player | opponent);
    return -3.29 + 0.5395 * empties + 2.4157 * Math.log(1 + GetMoves.getNMoves(opponent, player))
        -0.000615 * (approxEval - lower);
//    return -1.7147 + 0.6223 * empties + 1.0554 * Math.log(2 + GetMoves.getNMoves(opponent, player))
//        +0.000603 * (lower - approxEval);
  }

  @Override
  public double disproofNumber(long player, long opponent, int lower, int approxEval) {
    if (LOG && Math.random() < Math.pow(1.5, Long.bitCount(~(player | opponent))) / 1000 / 3000) {
      requests.add(new EndgameTimeEstimatorRequest(player, opponent, lower, approxEval, false));
    }
    return Math.max(1, Math.min(1.27E89, Math.exp(logDisproofNumber(player, opponent, lower, approxEval))));
  }

  @Override
  public double logDisproofNumber(long player, long opponent, int lower, int approxEval) {
    int empties = 64 - Long.bitCount(player | opponent);
    return -3.6659 + 0.5501 * empties + 2.7047 * Math.log(1 + GetMoves.getNMoves(player, opponent))
        -0.000590 * (lower - approxEval);
//    return -3.9479 + 0.5727 * empties + 2.7668 * Math.log(1 + GetMoves.getNMoves(player, opponent))
//        -0.0005 * (lower - approxEval);
  }
  
  public static void buildDataset(int minEmpties, int maxEmpties, double subsample) {
    EvaluatorMidgame evaluatorMidgame = new EvaluatorMidgame();
    EvaluatorMCTS evaluator = new EvaluatorMCTS(Constants.MCTS_SIZE, 2 * Constants.MCTS_SIZE, evaluatorMidgame);
    EndgameTimeEstimator endgameTimeEstimator = new EndgameTimeEstimator();
    FindStableDisks stableDisks = FindStableDisks.load();

    ArrayList<BoardWithEvaluation> training = LoadDataset.loadTrainingSet(1990, 1990);
    double sumErrorSquared = 0;
    double nBoards = 0;
    

    System.out.println("requestProof actualProof " +
                       "nVisited logNVisited " +
                       "Empties Lower approxEval playerMoves opponentMoves lowerStable upperStable " +
                       "eval solved proof disproof");
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
        Board current = new Board(request.player, request.opponent);
        evaluator.reset();
        evaluatorMidgame.resetHashMap();
        evaluator.reset();
        int eval = -evaluator.evaluatePosition(
            current, request.lower, request.lower, Long.MAX_VALUE, 20 * 1000, true);
        double nVisited = evaluator.getNVisited();
        double logNVisited = Math.log(evaluator.getNVisited());
        double predicted = eval > request.lower ?
            endgameTimeEstimator.logProofNumber(current.getPlayer(), current.getOpponent(), request.lower, request.approxEval) :
            endgameTimeEstimator.logDisproofNumber(current.getPlayer(), current.getOpponent(), request.lower, request.approxEval);

        if ((eval > request.lower) != request.proof) {
          continue;
        }
        System.out.println(
            request.proof + " " + (eval > request.lower) + " " +
            nVisited + " " + Math.log(nVisited) + " " +
            current.getEmptySquares() + " " +
            request.lower + " " + request.approxEval + " " + 
            GetMoves.getNMoves(current.getPlayer(), current.getOpponent()) + " " +
            GetMoves.getNMoves(current.getOpponent(), current.getPlayer()) + " " +
            stableDisks.getLowerBound(current.getPlayer(), current.getOpponent()) + " " +
            stableDisks.getUpperBound(current.getPlayer(), current.getOpponent()) + " " +
            eval + " " +
            (evaluator.getStatus() == EvaluatorMCTS.Status.SOLVED) + " " +
            evaluator.firstPosition.proofNumberGreaterEqual + " " +
            evaluator.firstPosition.disproofNumberStrictlyGreater
        );
//        System.out.println(predicted + " " + logNVisited);
//        if (request.proof) {
          sumErrorSquared += (predicted - logNVisited) * (predicted - logNVisited);
          nBoards++;
//        }
//        System.out.println("Average squared error: " + Math.sqrt(sumErrorSquared / nBoards));
        
      }
    }
    System.out.println("Average squared error: " + Math.sqrt(sumErrorSquared / nBoards));
  }
  
  public static void main(String args[]) {
    buildDataset(8, 20, 0.03);
  }
}
