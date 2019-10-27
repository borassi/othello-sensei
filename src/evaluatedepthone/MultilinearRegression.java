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

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;

import bitpattern.BitPattern;
import board.Board;
import evaluateposition.EvaluatorLastMoves;
import helpers.SelfPlay;
import evaluateposition.EvaluatorMCTS;
import evaluateposition.EvaluatorAlphaBeta;
import board.PossibleMovesFinderImproved;
import it.unimi.dsi.fastutil.objects.ObjectArrayList;
import java.util.logging.Level;
import java.util.logging.Logger;

public class MultilinearRegression implements java.io.Serializable, DepthOneEvaluator {  
  /**
   * Needed to make the class serializable.
   */
  private static final long serialVersionUID = 1L;

  /* The PatternEvaluators used in this class. */
  private PatternEvaluator[][] evaluators;
  
  /**
   * The training set.
   */
  transient ArrayList<BoardWithEvaluation> trainingSet;

  /* 
   * These boards are used to store the rotations of the current board.
   * They can be reused to save the time to instantiate these objects.
   */
//  Board r2 = new Board();
//  Board r3 = new Board();
//  Board r4 = new Board();
  
  /**
   * Creates an empty MultilinearRegression.
   * If you need to read a MultilinearRegression from file, this is not
   * the function you are looking for (see load() below).
   * If you need to create and train a new model, after this you must
   * call setTrainingSet() and train().
   */
  public MultilinearRegression() {
    this(30);
  }
  
  public MultilinearRegression(int minNSamples) {
    this(PatternEvaluator.getEvaluatorsImproved(minNSamples));
  }
  
  public MultilinearRegression(PatternEvaluator[][] evaluators) {
    this.evaluators = evaluators;
  }
  
  public int evaluatorsFromBoard(Board b) {
    int emptySquares = Math.min(b.getEmptySquares(), 60);
    int result = (emptySquares - 1) / 6;

    return result;
  }
  
  public float trainStep(ArrayList<BoardWithEvaluation> trainingSet, float speed) {
    double sumErrorSquared = 0;

    Collections.shuffle(trainingSet);

    for (BoardWithEvaluation be : trainingSet) {
      Board b = be.board;
      float error = be.evaluation - eval(b);
      sumErrorSquared += error * error;
      Board[] allTranspositions = Board.allTranspositions(b);
      int group = evaluatorsFromBoard(b);
      PatternEvaluator[] curEvaluators = this.evaluators[group];

      for (PatternEvaluator evaluator : curEvaluators) {
        evaluator.update(allTranspositions, error, speed);
      }
    }
    float curError = (float) Math.sqrt(sumErrorSquared / trainingSet.size());
    System.out.println("  Average deviation: " + curError);
    System.out.println("  Speed: " + speed);
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
//            Logger.getLogger(MultilinearRegression.class.getName()).log(Level.SEVERE, null, ex);
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
//      this.save(EvaluatorAlphaBeta.DEPTH_ONE_EVALUATOR_FILEPATTERN);
//    }
//  }
//  
  /**
   * Trains the model.
   * @param trainingSet
   * @param speed
   * @param nIter The number of iterations.
   */
  public void train(ArrayList<BoardWithEvaluation> trainingSet,
                    float speed, int nIter) {
    int iter = 0;

//    for (PatternEvaluator[] patterns : evaluators) {
//      for (PatternEvaluator pattern : patterns) {
//        pattern.resetNumberOfAppearances();
//      }
//    }
    
//    for (BoardWithEvaluation be : trainingSet) {
//      Board b = be.board;
//      Board[] allTranspositions = Board.allTranspositions(b);
//      PatternEvaluator[] curEvaluator = evaluators[evaluatorsFromBoard(b)];
//      for (PatternEvaluator patternEvaluator : curEvaluator) {
//        patternEvaluator.updateNumberOfAppearences(allTranspositions);
//      }
//    }

//    for (PatternEvaluator[] patterns : evaluators) {
//      for (PatternEvaluator pattern : patterns) {
//        pattern.resetEvaluatorFloat();
//      }
//    }

    float oldError = Float.MAX_VALUE;
    while (iter++ < nIter) {
      System.out.println("Iteration: " + iter);
      float curError = trainStep(trainingSet, speed);
      if (curError > oldError) {
        speed = speed / 2;
      }
      oldError = curError;
    }
//    for (PatternEvaluator[] patterns : evaluators) {
//      for (PatternEvaluator pattern : patterns) {
//        pattern.setMissingEval();
//      }
//    }
  }
  /** 
   * Evaluates a position.
   * @param b the position
   * @return the evaluation
   */
//  public int evalForTraining(Board b) {
//    int result = 0;
//    int group = evaluatorsFromBoard(b);
//    PatternEvaluator[] evaluators = this.evaluators[group];
//    Board[] allTranspositions = b.allTranspositions();
//    for (PatternEvaluator evaluator : evaluators) {
//      result += evaluator.evalForTraining(allTranspositions);
//    }
//    return result;
//  }
  /** 
   * Evaluates a position.
   * @param b the position
   * @return the evaluation
   */
  public int eval(Board b) {
    int result = 0;
    int group = evaluatorsFromBoard(b);
    PatternEvaluator[] evaluators = this.evaluators[group];
    Board[] allTranspositions = b.allTranspositions();
    for (PatternEvaluator evaluator : evaluators) {
      result += evaluator.eval(allTranspositions);
    }
    return result;
  }

  @Override
  public int evalVerbose(Board b) {
    int result = 0;
    int group = evaluatorsFromBoard(b);
    PatternEvaluator[] evaluators = this.evaluators[group];
    Board[] allTranspositions = b.allTranspositions();

    System.out.println("\nEvaluating\n" + b);
    for (int i = 0; i < evaluators.length; i++) {
      result += evaluators[i].evalVerbose(allTranspositions);
    }
    return result;
  }
  
  public void save(String file) {
    try {
       FileOutputStream fileOut =
       new FileOutputStream(file);
       ObjectOutputStream out = new ObjectOutputStream(fileOut);
       out.writeObject(this);
       out.close();
       fileOut.close();
       System.out.println("Serialized data is saved.");
    } catch (IOException e) {
       e.printStackTrace();
    }
  }


  /**
   * Creates a multilinear regression class reading the parameters from the
   * file provided.
   * @param filepath the file (obtained by calling save() on another multilinear regression).
   * @return The multilinear regression loaded, or an empty MultilinearRegression if errors occurred.
   */
  public static MultilinearRegression load(String filepath) {
    MultilinearRegression result;
    try (ObjectInputStream in = new ObjectInputStream(new FileInputStream(filepath))) {
      result = (MultilinearRegression) in.readObject();
    } catch (IOException | ClassNotFoundException | ClassCastException e) {
      System.out.println("Error when loading the MultilinearRegression:\n" + 
                         Arrays.toString(e.getStackTrace()));
      return new MultilinearRegression();
    }
    return result;
  }

  @Override
  public int getMinTrainingSetSize(Board b) {
    int result = Integer.MAX_VALUE;
    int group = evaluatorsFromBoard(b);
    PatternEvaluator[] groupEvaluators = this.evaluators[group];
    Board[] allTranspositions = b.allTranspositions();
    for (PatternEvaluator evaluator : groupEvaluators) {
      result = Math.min(result, evaluator.getMinTrainingSetSize(allTranspositions));
    }
    return result;
  }
  
  public static void main(String args[]) {
    MultilinearRegression r = MultilinearRegression.load(EvaluatorAlphaBeta.DEPTH_ONE_EVALUATOR_FILEPATTERN);
    System.out.println(r.evalVerbose(new Board("OXXXXXXX\n" +
                                               "OXOOXOXX\n" +
                                               "OXXXOXOX\n" +
                                               "OXXXXOXX\n" +
                                               "OXXXXXOX\n" +
                                               "OOXOXOOX\n" +
                                               "OOOOOOXX\n" +
                                               "XXXXXXXX", true)));
  }
}
