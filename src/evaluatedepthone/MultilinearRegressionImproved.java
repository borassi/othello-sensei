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

package evaluatedepthone;

import java.util.ArrayList;
import java.util.Collections;

import board.Board;
import evaluateposition.EvaluatorAlphaBeta;
import helpers.LoadDataset;

public class MultilinearRegressionImproved {  

  private PatternEvaluatorImproved patternEvaluator;
  
  /**
   * Creates an empty MultilinearRegression.
   * If you need to read a MultilinearRegression from file, this is not
   * the function you are looking for (see load() below).
   * If you need to create and train a new model, after this you must
   * call setTrainingSet() and train().
   */
//  public MultilinearRegressionImproved(PatternEvaluatorImproved patternEvaluator, int[][] evals) {
//    this(30);
//  }
//  public int evaluatorsFromBoard(Board b) {
//    int emptySquares = Math.min(b.getEmptySquares(), 60);
//    int result = (emptySquares - 1) / 6;
//
//    return result;
//  }
  
  public static float trainStep(PatternEvaluatorImproved evaluator,
      ArrayList<BoardWithEvaluation> trainingSet, float speed) {
    double sumErrorSquared = 0;
    double lambda = 0.1;

    Collections.shuffle(trainingSet);

    for (BoardWithEvaluation be : trainingSet) {
      int contB = 0;
//      Board b = be.board;
      for (Board b : be.board.horizontalSymmetries()) {
        float error = be.evaluation - evaluator.eval(b);
        
        if (contB++ == 0) {
          sumErrorSquared += error * error;
        }
        int[] curHashes = evaluator.hashes();
        short[][] curEval = evaluator.evals[PatternEvaluatorImproved.getEvalFromEmpties(evaluator.empties)];
        for (int i = 0; i < curHashes.length; ++i) {
          double ridgeUpdateSize = 2 * speed * (error - lambda * curEval[i][curHashes[i]]);
          curEval[i][curHashes[i]] += (short) Math.floor(ridgeUpdateSize);
          if (Math.random() < ridgeUpdateSize - Math.floor(ridgeUpdateSize)) {
            curEval[i][curHashes[i]]++;
          }
          curEval[i][curHashes[i]] = (short) Math.max(Math.min(curEval[i][curHashes[i]], 1500), -1500);
        }
      }
    }
    float curError = (float) Math.sqrt(sumErrorSquared / trainingSet.size());
    return curError;
  }
  
//  public void selfTrain() {
//    int nThreads = 1;
//    PossibleMovesFinderImproved pmf = new PossibleMovesFinderImproved();
//    EvaluatorLastMoves lastMoves = new EvaluatorLastMoves();
//    
//    while (true) {
//      System.out.println("DONE.");
//      for (int i = 0; i < 100; ++i) {
//        SelfPlay selfPlay[] = new SelfPlay[nThreads];
//        for (int j = 0; j < nThreads; ++j) {
//          EvaluatorMCTS eval = new EvaluatorMCTS(50000, 100000, pmf, this, lastMoves);
//          selfPlay[j] = new SelfPlay(eval, eval, pmf, (int) (Math.random() * 10));
//          selfPlay[j].start();
//        }
//        for (SelfPlay s : selfPlay) {
//          try {
//            s.join();
//          } catch (InterruptedException ex) {
//            Logger.getLogger(MultilinearRegressionImproved.class.getName()).log(Level.SEVERE, null, ex);
//          }
//          ObjectArrayList<Board> result = s.get();
//          int finalResult = BitPattern.getEvaluationGameOver(result.top());
//          for (int j = 0; j < result.size(); ++j) {
//            Board b = result.get(j);
//            int error = (((result.size() - j) % 2) == 0 ? -finalResult : finalResult) - eval(b);
//            Board[] allTranspositions = Board.allTranspositions(b);
//            int group = evaluatorsFromBoard(b);
//            PatternEvaluator[] curEvaluators = this.evaluators[group];
//            for (PatternEvaluator evaluator : curEvaluators) {
//              evaluator.update(allTranspositions, error, 0.2);
//            }
//          }
//        }
//      }
//    }
//  }
  
  /**
   * Trains the model.
   * @param trainingSet
   * @param speed
   * @param nIter The number of iterations.
   */
  public static void train(PatternEvaluatorImproved evaluator,
                           ArrayList<BoardWithEvaluation> trainingSet,
                           ArrayList<BoardWithEvaluation> testingSet,
                           float speed, int nIter) {
    int iter = 0;

    float oldError = Float.MAX_VALUE;
    while (iter++ < nIter) {
      System.out.println("Iteration: " + iter);
      float curError = trainStep(evaluator, trainingSet, speed);
      System.out.println("  Training error: " + curError);
      System.out.println("  Testing error:  " + test(evaluator, testingSet));
      System.out.println("  Speed: " + speed);
      if (curError > oldError) {
        speed = speed / 2;
      }
      oldError = curError;
    }
  }
  
  public static double test(PatternEvaluatorImproved evaluator,
       ArrayList<BoardWithEvaluation> testingSet) {
    double sumErrorSquared = 0;

    for (BoardWithEvaluation be : testingSet) {
      double error = be.evaluation - evaluator.eval(be.board);
      sumErrorSquared += error * error;
//      if (Math.abs(error) > 1600 && be.board.getEmptySquares() > 10) {
//        System.out.println(be.board);
//        int eval = evaluator.evalVerbose();
//        System.out.println("TOTAL: " + be.evaluation + " " + eval);
//      }
    }
    return Math.sqrt(sumErrorSquared / testingSet.size());
  }
  
  public static void main(String args[]) {
    PatternEvaluatorImproved eval = new PatternEvaluatorImproved(); //PatternEvaluatorImproved.load();// 
//    depthOneEvaluator = new MultilinearRegression();
    ArrayList<BoardWithEvaluation> trainingSet = LoadDataset.loadTrainingSet();
    ArrayList<BoardWithEvaluation> testingSet = LoadDataset.loadTestingSet();
    trainingSet.addAll(LoadDataset.loadOMGSet(170));
//    eval.reset((short) 0);
    eval.train(trainingSet, testingSet, 0.02F, 2);
    eval.train(trainingSet, testingSet, 0.01F, 5);
    eval.train(trainingSet, testingSet, 0.005F, 5);
    eval.train(trainingSet, testingSet, 0.002F, 5);
//    eval.train(trainingSet, testingSet, 0.005F, 5);
    eval.save();
  }
}
