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
import board.PossibleMovesFinderImproved;
import evaluateposition.EvaluatorLastMoves;
import helpers.SelfPlay;
import evaluateposition.EvaluatorMCTS;
import helpers.LoadDataset;

public class MultilinearRegressionImproved {  

  private PatternEvaluatorImproved patternEvaluator;
  private EvaluatorLastMoves eval = new EvaluatorLastMoves();
  private PossibleMovesFinderImproved pmf = new PossibleMovesFinderImproved();
  ArrayList<BoardWithEvaluation> testingSet;
  
  public MultilinearRegressionImproved(PatternEvaluatorImproved patternEvaluator,
      ArrayList<BoardWithEvaluation> testingSet) {
    this.patternEvaluator = patternEvaluator;
    this.testingSet = testingSet;
  }
  
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
  
  public float trainBoard(Board b, int correctEval, float speed, float lambda) {
    float trainError = correctEval - patternEvaluator.eval(b.getPlayer(), b.getOpponent());
//    System.out.println(error);

    int[] curHashes = patternEvaluator.hashes();
    int evalNumber = PatternEvaluatorImproved.getEvalFromEmpties(patternEvaluator.empties);
    float[][] curEval;
//    System.out.println(b);

    for (int shift = 0; shift <= 0; ++shift) {
      if (evalNumber + shift < 0 || evalNumber + shift >= patternEvaluator.evals.length) {
        continue;
      }
      curEval = patternEvaluator.evals[evalNumber + shift];
      float eval = 0;
      for (int i = 0; i < curHashes.length; ++i) {
        eval += curEval[i][curHashes[i]];
      }

      float error = correctEval - eval;
      
//      System.out.println(eval + " " + correctEval + " " + error + " " + 2 * speed * error);
      for (int i = 0; i < curHashes.length; ++i) {
//        System.out.println(error + " " + 2 * speed * error + " " + );
        double ridgeUpdateSize = 2 * speed * (error - lambda * curEval[i][curHashes[i]]); // Math.signum(error) * 600; //
//        System.out.print("  " + curEval[i][curHashes[i]]);
        curEval[i][curHashes[i]] += ridgeUpdateSize;
        curEval[i][curHashes[i]] = Math.max(Math.min(curEval[i][curHashes[i]], 1500), -1500);
//        System.out.println("  " + curEval[i][curHashes[i]]);
//        curEval[i][curHashes[i]] += (short) Math.floor(ridgeUpdateSize);
//        System.out.println(Math.floor(ridgeUpdateSize));
//        if (Math.random() < ridgeUpdateSize - Math.floor(ridgeUpdateSize)) {
//          System.out.println("ADD ONE");
//          curEval[i][curHashes[i]]++;
//        }
//        curEval[i][curHashes[i]] = (short) Math.max(Math.min(curEval[i][curHashes[i]], 1500), -1500);
      }
//      eval = 0;
//      for (int i = 0; i < curHashes.length; ++i) {
//        eval += curEval[i][curHashes[i]];
//      }
//      System.out.println(eval);
    }
    return trainError;
  }
  
  public float trainStep(ArrayList<BoardWithEvaluation> trainingSet, float lambda, float speed) {
    double sumErrorSquared = 0;

    Collections.shuffle(trainingSet);

    for (BoardWithEvaluation be : trainingSet) {
      if (be.board.getEmptySquares() == 0) {
        continue;
      }
      int contB = 0;
      for (Board b : be.board.horizontalSymmetries()) {
        float error = trainBoard(b, be.evaluation, speed, lambda);
//        trainBoard(new Board(b.getOpponent(), b.getPlayer()), -be.evaluation, speed, lambda);
        
        if (contB++ == 0) {
          sumErrorSquared += error * error;
        }
      }
    }
    float curError = (float) Math.sqrt(sumErrorSquared / trainingSet.size());
    return curError;
  }
  
  
  public void selfTrainEndgameBoard(Board b, ArrayList<BoardWithEvaluation> trainingSet) {
    int evalB = this.eval.evaluatePosition(b, -6600, 6600, 0);
    trainingSet.add(new BoardWithEvaluation(b, evalB));
    if (b.getEmptySquares() == 1) {
      return;
    }
    long[] moves = pmf.possibleMovesAdvanced(b.getPlayer(), b.getOpponent());
    for (long move : moves) {
      if (Math.random() < 0.6) {
        continue;
      }
      selfTrainEndgameBoard(b.move(move), trainingSet);
    }
  }
  
  public void selfTrainEndgame() {
    ArrayList<BoardWithEvaluation> startingBoards = LoadDataset.loadTrainingSet();
    ArrayList<BoardWithEvaluation> trainingSet = new ArrayList<BoardWithEvaluation>();
    int iter = 0;
    for (BoardWithEvaluation be : startingBoards) {
      Board b = be.board;
      if (iter++ % 500000 == 0) {
        this.train(trainingSet, 0.01F, 0.001F, 1);
        trainingSet = new ArrayList<BoardWithEvaluation>();
//        System.out.println(iter + " " + startingBoards.size());
      }
      if (b.getEmptySquares() != 12) {
        continue;
      }
//      trainingSet.add(new BoardWithEvaluation(b, this.eval.evaluatePosition(b, -6600, 6600, 0)));
//      System.out.print(be.evaluation);
      this.selfTrainEndgameBoard(b, trainingSet);
    }
//    double[] errors = test();
//    System.out.println("  Testing error total: " + errors[0]);
//    System.out.print("  Testing error part:  ");
//    for (int i = 1; i < errors.length; ++i) {
//      System.out.print(((i-1) * 60 / (errors.length-1)) + ":" + (int) errors[i] + " ");
//    }
//    System.out.println();
    
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

  public void train(ArrayList<BoardWithEvaluation> trainingSet,
                    float lambda, float speed, int nIter) {
    int iter = 0;

    float oldError = Float.MAX_VALUE;
    while (iter++ < nIter) {
      System.out.println("Iteration: " + iter);
      float curError = trainStep(trainingSet, lambda, speed);
      System.out.println("  Training error: " + curError);
      double[] errors = test();
      System.out.println("  Testing error total: " + errors[0]);
      System.out.print("  Testing error part:  ");
      for (int i = 1; i < errors.length; ++i) {
        System.out.print(((i-1) * 60 / (errors.length-1)) + ":" + (int) errors[i] + " ");
      }
      System.out.println("\n  Speed: " + speed);
      if (curError > oldError) {
        speed = speed / 2;
      }
      oldError = curError;
    }
  }
  
  public double[] test() {
    int n = 10;
    double sumErrorForEmpties[] = new double[n+2];
    double nForEmpties[] = new double[n+2];

    for (BoardWithEvaluation be : testingSet) {
      if (be.board.getEmptySquares() == 0) {
        continue;
      }
      double error = be.evaluation - patternEvaluator.eval(be.board.getPlayer(), be.board.getOpponent());
      double errorSquared = error * error;
      int slice = 1 + be.board.getEmptySquares() * n / 60;
      sumErrorForEmpties[0] += errorSquared;
      nForEmpties[0]++;
      sumErrorForEmpties[slice] += errorSquared;
      nForEmpties[slice]++;
    }
    for (int i = 0; i < n+1; ++i) {
      sumErrorForEmpties[i] = Math.sqrt(sumErrorForEmpties[i] / nForEmpties[i]);
    }
    return sumErrorForEmpties;
  }
  
  public static void main(String args[]) {
    ArrayList<BoardWithEvaluation> testingSet = LoadDataset.loadTestingSet();
//    PatternEvaluatorImproved eval = new PatternEvaluatorImproved();
    PatternEvaluatorImproved eval = PatternEvaluatorImproved.load();
//    eval.reset((short) 0);
    MultilinearRegressionImproved mr = new MultilinearRegressionImproved(eval, testingSet);
    ArrayList<BoardWithEvaluation> trainingSet = LoadDataset.loadTrainingSet();
//    trainingSet.addAll(LoadDataset.loadOMGSet(170));

    mr.train(trainingSet, 0.01F, 0.004F, 2);
    mr.train(trainingSet, 0.01F, 0.002F, 2);
    mr.train(trainingSet, 0.01F, 0.001F, 2);
    mr.train(trainingSet, 0.01F, 0.0005F, 2);
    mr.train(trainingSet, 0.01F, 0.0002F, 2);
    mr.train(trainingSet, 0.01F, 0.0001F, 2);
    eval.save();
  }
}
