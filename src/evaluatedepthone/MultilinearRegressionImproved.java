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
import helpers.LoadDataset;

public class MultilinearRegressionImproved {

  private final PatternEvaluatorImproved patternEvaluator;
  private final PossibleMovesFinderImproved pmf = new PossibleMovesFinderImproved();
  private final EvaluatorLastMoves evaluatorLastMoves = new EvaluatorLastMoves();
  ArrayList<BoardWithEvaluation> testingSet;
  
  public MultilinearRegressionImproved(
      ArrayList<BoardWithEvaluation> testingSet) {
    this.patternEvaluator = new PatternEvaluatorImproved();
    this.testingSet = testingSet;
  }
  
  static class Feature {
    public float eval; // Between -1500 and 1500
    public int appearences = 0;
    public float squaredError = 1200F * 1200F / 42F; // Between 2000 * 2000, 200 * 200 / 42.0

    public Feature(long eval) {
      this.eval = eval;// >> PatternEvaluatorImproved.EVAL_SHIFT;
//      if (eval == 0) {
      this.squaredError = (1200F * 1200F) / PatternEvaluatorImproved.EVALS[0].length;
//      } else {
//        this.squaredError = eval - ((eval >> PatternEvaluatorImproved.EVAL_SHIFT) << PatternEvaluatorImproved.EVAL_SHIFT);
//      }
    }

    public int toInt() {
      return ((int) Math.round(eval));
//      long evalPart = ((long) Math.round(eval)) << PatternEvaluatorImproved.EVAL_SHIFT;
//      assert(Long.MAX_VALUE / patternEvaluator.EVALS[0].length > evalPart);
//      int errorPart = (int) (Math.round(squaredError));
//      assert((double) errorPart * patternEvaluator.EVALS[0].length < (1L << PatternEvaluatorImproved.EVAL_SHIFT));
//      return evalPart + errorPart;
    }
  }
  
  
  protected Feature[][][] getFeatures() {
    int[][][] intFeatures = PatternEvaluatorImproved.EVALS;
    Feature[][][] features = new Feature[intFeatures.length][][];
    
    for (int i = 0; i < features.length; ++i) {
      features[i] = new Feature[intFeatures[i].length][];
      for (int j = 0; j < features[i].length; ++j) {
        if (j > 0 && intFeatures[i][j] == intFeatures[i][j-1]) {
          features[i][j] = features[i][j-1];
          continue;
        }
        features[i][j] = new Feature[intFeatures[i][j].length];
        int[] evalsIJ = intFeatures[i][j];
        Feature[] featuresIJ = features[i][j];
        for (int k = 0; k < featuresIJ.length; ++k) {
          featuresIJ[k] = new Feature(evalsIJ[k]);
        }
      }
    }
    return features;
  }
  
  void exportFeatures(Feature[][][] newEvals) {
    for (int i = 0; i < newEvals.length; ++i) {
      for (int j = 0; j < newEvals[i].length; ++j) {
        if (j > 0 && newEvals[i][j] == newEvals[i][j-1]) {
          PatternEvaluatorImproved.EVALS[i][j] = PatternEvaluatorImproved.EVALS[i][j-1];
          continue;
        }
        Feature[] newEvalsIJ = newEvals[i][j];
        int[] evalsIJ = PatternEvaluatorImproved.EVALS[i][j];
        for (int k = 0; k < evalsIJ.length; ++k) {
          evalsIJ[k] = newEvalsIJ[k].toInt();
        }
      }
    }
  }

  float eval(Feature[][] evals, int hashes[]) {
    float result = 0;
    for (int i = 0; i < hashes.length; ++i) {
      result += evals[i][hashes[i]].eval;
    }
    return result;
  }
  
  float trainBoard(Feature[][][] evals, Board b, int correctEval, float speed, float lambda,
      boolean updateAppearences) {
    patternEvaluator.setup(b);
    float trainError = 0;
    int[] curHashes = patternEvaluator.hashes();
    int evalNumber = PatternEvaluatorImproved.getEvalFromEmpties(b.getEmptySquares());
    Feature[][] curEval;

    for (int shift = -2; shift <= 2; ++shift) {
      if (evalNumber + shift < 0 || evalNumber + shift >= evals.length) {
        continue;
      }
      curEval = evals[evalNumber + shift];
      float eval = eval(curEval, curHashes);

      float error = correctEval - eval;
      if (shift == 0) {
        trainError = error;
      }
//      System.out.println(correctEval);
//      System.out.println(eval);
      double expectedSquaredError = 0;
      for (int i = 0; i < curHashes.length; ++i) {
        expectedSquaredError += curEval[i][curHashes[i]].squaredError;
      }
//      if (speed > 0 && Math.random() < 0.001) {
//        System.out.println(Math.sqrt(expectedSquaredError) + " " + error);
//      }
      for (int i = 0; i < curHashes.length; ++i) {
        Feature feature = curEval[i][curHashes[i]];
        if (updateAppearences) {
          feature.appearences = Math.min(100000, feature.appearences + 1);
        } else {
          float expectedErrorUpdateSize = (float) (2 * speed * (error * error - expectedSquaredError));
          feature.squaredError += expectedErrorUpdateSize;
          feature.squaredError = (float) Math.max(
              Math.min(feature.squaredError, 2000 * 2000),
              200.0 * 200.0 / this.patternEvaluator.EVALS[0].length);

          double ridgeUpdateSize = 2 * speed * (error - lambda * feature.eval);// * 800 / Math.sqrt(expectedSquaredError);
//              / Math.min(1000, Math.max(feature.appearences, 5)); // Math.signum(error) * 600; //
          feature.eval += ridgeUpdateSize;
          feature.eval = Math.max(Math.min(feature.eval, 1500), -1500);
        }
      }
//      System.out.println(eval(curEval, curHashes) + "\n");
    }
    return trainError;
  }
  
  public float trainStep(Feature[][][] evals, ArrayList<BoardWithEvaluation> trainingSet, float lambda, float speed,
      boolean updateAppearences) {
    double sumErrorSquared = 0;

    Collections.shuffle(trainingSet);

//    int i = 0;
    for (BoardWithEvaluation be : trainingSet) {
//      if (i++ % 100000 == 0) {
//        System.out.println(i + " " + trainingSet.size());
//      }
      if (be.board.getEmptySquares() == 0) {
        continue;
      }
      int contB = 0;
      for (Board b : be.board.horizontalSymmetries()) {
        float error = trainBoard(evals, b, be.evaluation, speed, lambda, updateAppearences);
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
    int evalB = this.evaluatorLastMoves.evaluate(b, -6600, 6600, 0);
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
  
  public void selfTrainEndgame(Feature[][][] evals) {
    ArrayList<BoardWithEvaluation> startingBoards = LoadDataset.loadTrainingSet();
    ArrayList<BoardWithEvaluation> trainingSet = new ArrayList<>();
    int iter = 0;
    for (BoardWithEvaluation be : startingBoards) {
      Board b = be.board;
      if (iter++ % 500000 == 0) {
        this.train(evals, trainingSet, 0.01F, 0.001F, 1, false);
        trainingSet = new ArrayList<>();
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
  

  public void train(
      Feature[][][] evals,
      ArrayList<BoardWithEvaluation> trainingSet,
      float lambda, float speed, int nIter, boolean updateAppearences) {
    int iter = 0;

    float oldError = Float.MAX_VALUE;
    while (iter++ < nIter) {
      System.out.println("Iteration: " + iter);
      float curError = trainStep(evals, trainingSet, lambda, speed, updateAppearences);
      System.out.println("  Training error: " + curError);
      double[] errors = test(evals);
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
  
  public double[] test(Feature[][][] evals) {
    int n = 10;
    double sumErrorForEmpties[] = new double[n+2];
    double nForEmpties[] = new double[n+2];

    for (BoardWithEvaluation be : testingSet) {
      if (be.board.getEmptySquares() == 0) {
        continue;
      }
      patternEvaluator.setup(be.board);
      double error = be.evaluation - eval(
          evals[PatternEvaluatorImproved.getEvalFromEmpties(be.board.getEmptySquares())],
          patternEvaluator.hashes());
//      System.out.println(error);
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
  
  // TODO: RUN ONCE!!
  public static void main(String args[]) {
    ArrayList<BoardWithEvaluation> trainingSet = LoadDataset.loadTrainingSet();
    trainingSet.addAll(LoadDataset.loadOMGSet(170));
    ArrayList<BoardWithEvaluation> testingSet = LoadDataset.loadTestingSet();
    MultilinearRegressionImproved mr = new MultilinearRegressionImproved(testingSet);

    Feature features[][][];
    features = mr.getFeatures();
    mr.train(features, trainingSet, 0.01F, 0.002F, 1, false);
    mr.train(features, trainingSet, 0.01F, 0.001F, 1, false);
    mr.train(features, trainingSet, 0.01F, 0.0005F, 1, false);
    mr.train(features, trainingSet, 0.01F, 0.0002F, 1, false);
//    mr.train(features, trainingSet, 0.01F, 0.0001F, 1, false);
    mr.exportFeatures(features);
    PatternEvaluatorImproved.saveEvals("coefficients/pattern_evaluator_coefficients_v1.dat");
 
    trainingSet = LoadDataset.loadTrainingSet();

    mr.train(features, trainingSet, 0.01F, 0.002F, 1, false);
    mr.train(features, trainingSet, 0.01F, 0.001F, 1, false);
    mr.train(features, trainingSet, 0.01F, 0.0005F, 1, false);
    mr.train(features, trainingSet, 0.01F, 0.0002F, 1, false);
    mr.train(features, trainingSet, 0.01F, 0.0001F, 1, false);
    mr.exportFeatures(features);
    PatternEvaluatorImproved.saveEvals();
  }
}
