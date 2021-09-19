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
package endgametest;

import board.Board;
import board.GetMovesCache;
import constants.Constants;
import constants.Stats;
import evaluatedepthone.BoardWithEvaluation;
import evaluatedepthone.PatternEvaluatorImproved;
import evaluateposition.EvalWithVisited;
import evaluateposition.EvaluatorMCTS;
import evaluateposition.EvaluatorAlphaBeta;
import evaluateposition.EvaluatorLastMoves;
import evaluateposition.HashMap;
import helpers.LoadDataset;
import helpers.Utils;
import java.text.NumberFormat;
import java.util.ArrayList;
import java.util.Locale;
import java.util.Random;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author michele
 */
public class EndgameThor {
  public final ArrayList<BoardWithEvaluation> boards = new ArrayList<>();
  long t = 0;

//  EvaluatorMCTS evalMCTS = new EvaluatorMCTS(2000000, 4000000);
//  EvaluatorLastMoves evalLast = new EvaluatorLastMoves();
//  EvaluatorAlphaBeta evalMidgame = new EvaluatorAlphaBeta();
//  Random generator = new Random(1234);
  final NumberFormat numberFormat = NumberFormat.getNumberInstance(Locale.US);
  
  public EndgameThor() {
    ArrayList<BoardWithEvaluation> allBoards = LoadDataset.loadTrainingSet(1977, 1997);
    for (BoardWithEvaluation b : allBoards) {
      long[] moves = GetMovesCache.getAllMoves(b.board);
//      return moves != null && moves.length == 1 && moves[0] == 0;
      if (b.board.getEmptySquares() == 14 &&
          (moves != null) &&
          (moves.length > 1 || (moves.length == 1 && moves[0] != 0))) {
        boards.add(b);
      }
    }
  }
  
  public boolean runSingleBoard(BoardWithEvaluation be) {
    Board b = be.board;
    int alpha = (int) -6400;// (be.evaluation + (generator.nextDouble() - 0.5) * 3000);
    int beta = (int) 6400; // (alpha + generator.nextDouble() * 1000);

    t -= System.currentTimeMillis();
    EvalWithVisited eval = EvaluatorLastMoves.evaluateCPP(b.getPlayer(), b.getOpponent(), alpha, beta);
//    int result = evalMidgame.evaluate(b.getPlayer(), b.getOpponent(), b.getEmptySquares(), alpha, beta);
//    int result = evalLast.evaluate(b, alpha, beta, 0);
    t += System.currentTimeMillis();
    int result = eval.eval;
    if (alpha < be.evaluation && be.evaluation < beta) {
      if (result - be.evaluation > 0) {
        System.out.println(b);
        System.out.println(b.toString().replace("\n", ""));
        System.out.println(result);
        System.out.println(be.evaluation);
        return false;
      }
    }
    if (be.evaluation > alpha && result <= alpha) {
      System.out.println(b);
      System.out.println(result + " " + be.evaluation + " " + alpha + " " + beta);
      return false;
    }
    if (be.evaluation < beta && result >= beta) {
      System.out.println(b);
      System.out.println(result + " " + be.evaluation + " " + alpha + " " + beta);
      return false;
    }
    return true;
  }

  public void run() {
    int i = 0;
    Stats.reset();
    long nVisited;
    for (BoardWithEvaluation be : boards) {
      if (++i % 100 == 0) {
        nVisited = Stats.getNVisitedAlphaBetaSolve() != 0 ? Stats.getNVisitedAlphaBetaSolve() : Stats.getNVisitedLastMoves();
        System.out.println(i + " / " + boards.size());
//        System.out.println("Visited1 / pos: " + numberFormat.format((int) (eval1.getNVisited() / i)));
        System.out.println("Visited / pos: " + numberFormat.format((int) (nVisited / i)));
        System.out.println("Visited / s: " + numberFormat.format((int) (nVisited * 1000. / t)));
        System.out.println("Visited / endgame: " + numberFormat.format((int) (nVisited / Stats.getNLastMoves())));
      }
      int d = be.board.getEmptySquares();
      if (!runSingleBoard(be)) {
        System.out.println("FAIL");
        break;
      }
    }
    nVisited = Stats.getNVisitedAlphaBetaSolve() != 0 ? Stats.getNVisitedAlphaBetaSolve() : Stats.getNVisitedLastMoves();
    System.out.println(t + " " + numberFormat.format(nVisited) + " " + i);
    System.out.println("Visited / pos: " + numberFormat.format((int) (nVisited / i)));
    System.out.println("Visited / s: " + numberFormat.format((int) (nVisited * 1000. / t)));
    System.out.println("Visited / endgame: " + numberFormat.format((int) (nVisited / Stats.getNLastMoves())));
//    System.out.println(GetFlip.getCPUTime() + " / " + (cpuT / 1000000000.) + " = " + GetFlip.getCPUTime() * 100000000000. / cpuT + "%");
  }
  
  public void parallelTest() {
    ExecutorService es = Executors.newFixedThreadPool(12);
    HashMap h = new HashMap();
    long t = -System.currentTimeMillis();
//    for (int i = 0; i < 5; ++i) {
      for (BoardWithEvaluation be : boards) {
        es.submit(() -> {
//          (new EvaluatorAlphaBeta(new PatternEvaluatorImproved(), h)).evaluate(be.board.getPlayer(), be.board.getOpponent(), be.board.getEmptySquares(), 100, 100);
          EvaluatorLastMoves.evaluateCPP(be.board.getPlayer(), be.board.getOpponent(), 100, 100);
//          new EvaluatorLastMoves().evaluate(be.board, -6400, 6400, 0);
        });
      }
//    }
    es.shutdown();
    try {
      es.awaitTermination(1000, TimeUnit.HOURS);
    } catch (InterruptedException ex) {
      Logger.getLogger(Utils.class.getName()).log(Level.SEVERE, null, ex);
    }
    t += System.currentTimeMillis();
    long nVisited = Stats.getNVisitedAlphaBetaSolve() == 0 ? Stats.getNVisitedLastMoves() : Stats.getNVisitedAlphaBetaSolve();
    System.out.println(t + " " + numberFormat.format(nVisited) + " " + boards.size());
    System.out.println("Visited / pos: " + numberFormat.format((int) (nVisited / boards.size())));
    System.out.println("Visited / s: " + numberFormat.format((int) (nVisited * 1000. / t)));
    System.out.println("Visited / endgame: " + numberFormat.format((int) (Stats.getNVisitedLastMoves() / Stats.getNLastMoves())));
  }
  
  public static void main(String args[]) { 
//    new EndgameThor().run();
    new EndgameThor().parallelTest();
  }
}
