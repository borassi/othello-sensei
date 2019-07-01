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

import java.io.Serializable;

import board.Board;
import evaluatedepthone.patternhasher.Corner3x3;
import evaluatedepthone.patternhasher.Corner5x2;
import evaluatedepthone.patternhasher.Diagonal;
import evaluatedepthone.patternhasher.Parity;
import evaluatedepthone.patternhasher.PatternHashInterface;
import evaluatedepthone.patternhasher.Radius;
import evaluatedepthone.patternhasher.Side;
import evaluatedepthone.patternhasher.SideImproved;
import evaluatedepthone.patternhasher.StableDisks;
import evaluatedepthone.patternhasher.StableDisksMiddle;


/**
 * @author michele
 * 
 */
public class PatternEvaluator implements Serializable {
  /**
   * Needed to implement Serializable.
   */
  private static final long serialVersionUID = 1L;

  public long pattern;
  public long mask = 0;
  public short[] evaluator;
  public transient int[] numberOfAppearances; 
  PatternHashInterface hashInterface;
  public final int minNumberOfAppearances;
  public transient float missingEval = 0;
  public transient float missingEvalNumberOfAppearances = 0;
  public transient float[] evaluatorFloat;
  
  
  PatternEvaluator(PatternHashInterface hashInterface, int minNumberOfAppearances) {
    this.hashInterface = hashInterface;
    this.evaluator = new short[this.hashInterface.maxSize()];
    this.minNumberOfAppearances = minNumberOfAppearances;
  }

  void resetNumberOfAppearances() {
    this.numberOfAppearances = new int[this.hashInterface.maxSize()];
  }

  public void resetEvaluatorFloat() {
    evaluatorFloat = new float[this.hashInterface.maxSize()];
    missingEval = 0;
    for (int i = 0; i < evaluatorFloat.length; i++) {
      if (numberOfAppearances[i] < minNumberOfAppearances) {
        missingEval = evaluator[i];
        evaluatorFloat[i] = Integer.MIN_VALUE;
      } else {
        evaluatorFloat[i] = evaluator[i];
      }
    }
  }

  public void updateNumberOfAppearences(Board[] allTransp) {
    if (numberOfAppearances == null) {
      numberOfAppearances = new int[this.hashInterface.maxSize()];
    }
    for (int hash : hashInterface.hashes(allTransp)) {
      numberOfAppearances[hash]++;
      if (numberOfAppearances[hash] < minNumberOfAppearances) {
        missingEvalNumberOfAppearances++;
      } else if (numberOfAppearances[hash] == minNumberOfAppearances) {
        missingEvalNumberOfAppearances -= minNumberOfAppearances - 1;
      }
    }
  }
  
  public void setMissingEval() {
    for (int i = 0; i < numberOfAppearances.length; i++) {
      if (numberOfAppearances[i] < minNumberOfAppearances) {
        evaluator[i] = (short) Math.round(missingEval);
      } else {
        evaluator[i] = (short) Math.round(evaluatorFloat[i]);
      }
    }
    evaluatorFloat = null;
    missingEval = Integer.MIN_VALUE;
  }

  public float evalForTraining(Board[] allTransp) {
    return eval(allTransp);
//    float result = 0;
//    for (int hash : hashInterface.hashes(allTransp)) {
//      if (numberOfAppearances[hash] >= minNumberOfAppearances) {
//        assert(evaluatorFloat[hash] != Integer.MIN_VALUE);
//        result += evaluatorFloat[hash];
//      } else {
//        result += missingEval;
//      }
//    }
//    return result;
  }

  public int getMinTrainingSetSize(Board[] allTransp) {
    int result = Integer.MAX_VALUE;
    for (int hash : hashInterface.hashes(allTransp)) {
      result = Math.min(result, this.numberOfAppearances[hash]);
    }
    return result;
  }

  public int eval(Board[] allTransp) {
    int result = 0;
    for (int hash : hashInterface.hashes(allTransp)) {
      result += evaluator[hash];
    }
    return result;
  }

  public int evalVerbose(Board[] allTransp) {
    int result = 0;
    int i = 0;
    for (int hash : hashInterface.hashes(allTransp)) {
      System.out.println(hashInterface.getClass() + " " + Side.sides[i++]);
      if (numberOfAppearances[hash] >= minNumberOfAppearances || evaluator[hash] != Integer.MIN_VALUE) {
        result += evaluator[hash];
        System.out.println("Eval: " + evaluator[hash] + " / " + numberOfAppearances[hash]);
      } else {
        result += missingEval;
        System.out.println("Eval: " + missingEval + "(missing)");
      }
    }
    return result;
  }

  public void update(Board[] allTransp, double error, double speed) {
    for (int hash : hashInterface.hashes(allTransp)) {
      if (numberOfAppearances[hash] >= minNumberOfAppearances) {
        evaluatorFloat[hash] += 2 * error * speed / numberOfAppearances[hash];
      } else {
        assert(evaluatorFloat[hash] == Integer.MIN_VALUE);
        missingEval += 2 * error * speed / missingEvalNumberOfAppearances;
      }
    }
    
  }
  
  public static final PatternEvaluator[][] getEvaluatorsImproved(int minNumberOfAppearances) {
    PatternEvaluator[][] evaluators = new PatternEvaluator[12][];

    for (int i = 0; i < evaluators.length; i++) {
      evaluators[i] = new PatternEvaluator[]{
              new PatternEvaluator(new Corner3x3(), minNumberOfAppearances),
              new PatternEvaluator(new Diagonal(), minNumberOfAppearances),
              new PatternEvaluator(new Parity(), minNumberOfAppearances),
              new PatternEvaluator(new Side(), minNumberOfAppearances),
              new PatternEvaluator(new StableDisksMiddle(), minNumberOfAppearances),
//              new PatternEvaluator(new Radius(), minNumberOfAppearances),
//              new PatternEvaluator(new StableDisks(), minNumberOfAppearances),
              new PatternEvaluator(new Corner5x2(), minNumberOfAppearances)
              };
    }
    return evaluators;
  }
}
