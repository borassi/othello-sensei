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
import board.GetFlip;
import board.PossibleMovesFinderImproved;
import evaluatedepthone.BoardWithEvaluation;
import evaluateposition.EvaluatorMCTS;
import evaluateposition.EvaluatorMidgame;
import helpers.LoadDataset;
import java.lang.management.ManagementFactory;
import java.lang.management.ThreadMXBean;
import java.text.NumberFormat;
import java.util.ArrayList;
import java.util.Locale;
import java.util.Random;

/**
 *
 * @author michele
 */
public class EndgameThor {
  public ArrayList<BoardWithEvaluation> boards;
  EvaluatorMCTS eval = new EvaluatorMCTS(2000000, 4000000);
//  EvaluatorLastMoves eval = new EvaluatorLastMoves();
//  EvaluatorMidgame eval = new EvaluatorMidgame();
  PossibleMovesFinderImproved pmf = new PossibleMovesFinderImproved();
	ThreadMXBean thread = ManagementFactory.getThreadMXBean();
  
  public EndgameThor() {
    boards = LoadDataset.loadTrainingSet(1977, 1987);
  }
  
  long t = 0;
  long cpuT = 0;
  public boolean runSingleBoard(BoardWithEvaluation be) {
    Board b = be.board;
    Random generator = new Random(1234);
    int alpha = (int) (generator.nextDouble() * 120 - 60);
    int beta = (int) (alpha + generator.nextDouble() * 6);
//    int result = eval1.evaluatePosition(b, 60, alpha, beta);
    t -= System.currentTimeMillis();
    cpuT -= thread.getCurrentThreadCpuTime();
    eval.setBoard(b, alpha, beta);
    int result = -eval.evaluatePosition();
//    int result = eval.evaluatePosition(b.getPlayer(), b.getOpponent(), alpha, beta, 64);
    cpuT += thread.getCurrentThreadCpuTime();
    t += System.currentTimeMillis();
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
      return false;
    }
    if (be.evaluation < beta && result >= beta) {
      return false;
    }
    return true;
  }
  
  public void run() {
    NumberFormat numberFormat = NumberFormat.getNumberInstance(Locale.US);
    int i = 0;
    int j = 0;
    long nVisited = 0;
    for (BoardWithEvaluation be : boards) {
      if (j++ % 100 == 0 && i > 0) {
        System.out.println(j + " / " + boards.size());
//        System.out.println("Visited1 / pos: " + numberFormat.format((int) (eval1.getNVisited() / i)));
        System.out.println("Visited / pos: " + numberFormat.format((int) (nVisited / i)));
        System.out.println("Visited / CPUs: " + numberFormat.format((int) (nVisited * 1000000000. / cpuT)));
        System.out.println("Visited / s: " + numberFormat.format((int) (nVisited * 1000. / t)));
        System.out.println("Visited / endgame: " + numberFormat.format((int) (nVisited / eval.nEndgames)));
      }
      int d = be.board.getEmptySquares();
      if (d != 12 || pmf.haveToPass(be.board)) {
        continue;
      }
      i++;
      if (!runSingleBoard(be)) {
        System.out.println("FAIL");
        break;
      }
      nVisited += eval.getNVisited();
    }
    System.out.println(t + " " + numberFormat.format(eval.getNVisited()) + " " + i);
//    System.out.println("Visited1 / s: " + numberFormat.format((int) (eval1.getNVisited() * 1000. / t)));
//    System.out.println("Visited1 / pos: " + numberFormat.format((int) (eval1.getNVisited() / i)));
    System.out.println("Visited / pos: " + numberFormat.format((int) (eval.getNVisited() / i)));
    System.out.println("Visited / CPUs: " + numberFormat.format((int) (eval.getNVisited() * 1000000000. / cpuT)));
    System.out.println("Visited / s: " + numberFormat.format((int) (eval.getNVisited() * 1000. / t)));
    System.out.println("Visited / endgame: " + numberFormat.format((int) (eval.getNVisited() / eval.nEndgames)));
    System.out.println(GetFlip.getCPUTime() + " / " + (cpuT / 1000000000.) + " = " + GetFlip.getCPUTime() * 100000000000. / cpuT + "%");
  }
  
  public static void main(String args[]) {    
    new EndgameThor().run();
  }
}
