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

import org.junit.Test;

import board.Board;
import static org.junit.Assert.assertEquals;

public class MultilinearRegressionImprovedTest {
  PatternEvaluatorImproved eval = new PatternEvaluatorImproved();
  final static Board oneOnEdge =
          new Board("---X----\n"
                  + "--------\n"
                  + "--------\n"
                  + "--------\n"
                  + "--------\n"
                  + "--------\n"
                  + "--------\n"
                  + "--------\n",
                  true);
  final static Board empty =
          new Board("--------\n"
                  + "--------\n"
                  + "--------\n"
                  + "--------\n"
                  + "--------\n"
                  + "--------\n"
                  + "--------\n"
                  + "--------\n",
                  true);
  final static Board twoOnEdge =
          new Board("---XX---\n"
                  + "--------\n"
                  + "--------\n"
                  + "--------\n"
                  + "--------\n"
                  + "--------\n"
                  + "--------\n"
                  + "--------\n",
                  true);
  final static Board twoOppositeEdges =
          new Board("---XX---\n"
                  + "--------\n"
                  + "--------\n"
                  + "--------\n"
                  + "--------\n"
                  + "--------\n"
                  + "--------\n"
                  + "---OO---\n",
                  true);
  final static Board fourCorners =
          new Board("O------O\n"
                  + "--------\n"
                  + "--------\n"
                  + "--------\n"
                  + "--------\n"
                  + "--------\n"
                  + "--------\n"
                  + "O------O\n",
                  true);
  final static Board xSquare =
          new Board("--------\n"
                  + "--------\n"
                  + "--------\n"
                  + "--------\n"
                  + "--------\n"
                  + "--------\n"
                  + "-X------\n"
                  + "--------\n",
                  true);
  
  @Test
  public void testFeatureBasic() {
    for (int i = 0; i < 10000; ++i) {
      long origError = (long) (Math.random() * 10000);
      long origEval = (long) ((Math.random() - 0.5) * 20000);
      long orig = (origEval << 32) + origError;
      MultilinearRegressionImproved.Feature feature = new MultilinearRegressionImproved.Feature(
          eval, orig);
      assertEquals(feature.toLong(), orig);
    }
  }

  @Test
  public void testEditFeature() {
    for (int i = 0; i < 10000; ++i) {
      long origError = (long) (Math.random() * 10000);
      long origEval = (long) ((Math.random() - 0.5) * 20000);
      double errorSum = (Math.random() - 0.5) * origError;
      double evalSum = (Math.random() - 0.5) * 10000;
      long orig = (origEval << 32) + origError;
      MultilinearRegressionImproved.Feature feature = new MultilinearRegressionImproved.Feature(
          eval, orig);
      feature.eval += evalSum;
      feature.squaredError += errorSum;
      long updatedEval = feature.toLong() >> 32;
      long updatedError = feature.toLong() - (updatedEval << 32);

      assertEquals(updatedEval, origEval + evalSum, 0.51);
      assertEquals(updatedError, origError + errorSum, 0.51);
    }
  }

  @Test
  public void testSumFeatures() {
    for (int i = 0; i < 10000; ++i) {
      long origEval1 = (long) ((Math.random() - 0.5) * 20000);
      long origError1 = (long) (Math.random() * 10000);
      long origEval2 = (long) ((Math.random() - 0.5) * 20000);
      long origError2 = (long) (Math.random() * 10000);
      MultilinearRegressionImproved.Feature f1 = new MultilinearRegressionImproved.Feature(eval);
      f1.eval = origEval1;
      f1.squaredError = origError1;
      MultilinearRegressionImproved.Feature f2 = new MultilinearRegressionImproved.Feature(eval);
      f2.eval = origEval2;
      f2.squaredError = origError2;

      long sumEval = (f1.toLong() + f2.toLong()) >> PatternEvaluatorImproved.EVAL_SHIFT;
      long sumErrors = (f1.toLong() + f2.toLong()) - (sumEval << PatternEvaluatorImproved.EVAL_SHIFT);
      assertEquals(sumEval, origEval1 + origEval2);
      assertEquals(sumErrors, origError1 + origError2);
    }
  }
  
  @Test
  public void testTrainBasic() {
    ArrayList<BoardWithEvaluation> trainingSet = new ArrayList<>();
    for (int i = 0; i < 10; i++) {
      trainingSet.add(new BoardWithEvaluation(oneOnEdge, 1000));
      trainingSet.add(new BoardWithEvaluation(twoOnEdge, 2000));
      trainingSet.add(new BoardWithEvaluation(twoOppositeEdges, 0));
      trainingSet.add(new BoardWithEvaluation(fourCorners, -4000));
    }

    MultilinearRegressionImproved mr = new MultilinearRegressionImproved(eval, trainingSet);
    
    MultilinearRegressionImproved.Feature[][][] newEvals = mr.getFeatures();
    mr.train(newEvals, trainingSet, 0, 0.005F, 20, false);
    mr.setFeatures(newEvals);
    
    for (BoardWithEvaluation b : trainingSet) {
      assertEquals(b.evaluation, eval.eval(b.board), 30);
      assertEquals(200, eval.lastError(), 30);  // Minimum error: +-2
    }
  }

  @Test
  public void testTrainError() {
    ArrayList<BoardWithEvaluation> trainingSet = new ArrayList<>();
    for (int i = 0; i < 10; i++) {
      trainingSet.add(new BoardWithEvaluation(oneOnEdge, 500));
      trainingSet.add(new BoardWithEvaluation(oneOnEdge, 1500));
      trainingSet.add(new BoardWithEvaluation(twoOnEdge, 2000));
      trainingSet.add(new BoardWithEvaluation(twoOppositeEdges, 0));
      trainingSet.add(new BoardWithEvaluation(fourCorners, -4000));
    }

    MultilinearRegressionImproved mr = new MultilinearRegressionImproved(eval, trainingSet);
    
    MultilinearRegressionImproved.Feature[][][] newEvals = mr.getFeatures();
    mr.train(newEvals, trainingSet, 0, 0.005F, 2, false);
    mr.train(newEvals, trainingSet, 0, 0.002F, 2, false);
    mr.train(newEvals, trainingSet, 0, 0.001F, 2, false);
    mr.train(newEvals, trainingSet, 0, 0.0005F, 2, false);
    mr.train(newEvals, trainingSet, 0, 0.0002F, 2, false);
    mr.train(newEvals, trainingSet, 0, 0.0001F, 2, false);
    mr.setFeatures(newEvals);
    
    for (BoardWithEvaluation b : trainingSet) {
      if (b.board != oneOnEdge) {
        System.out.println(b.board);
        assertEquals(b.evaluation, eval.eval(b.board), 400);
        assertEquals(200, eval.lastError(), 400);  // Minimum error: +-2
      } else {
        System.out.println(b.board);
        assertEquals(1000, eval.eval(b.board), 400);
        assertEquals(500, eval.lastError(), 400);
      }
    }
  }

//  @Test
//  public void testMissingConfiguration() {
//    ArrayList<BoardWithEvaluation> trainingSet = new ArrayList<>();
//    trainingSet.add(new BoardWithEvaluation(oneOnEdge, 100));
//    trainingSet.add(new BoardWithEvaluation(empty, 0));
//    
//    // Both are trained based on 1 "empty" side configuration and the rest.
//
//    PatternEvaluator[][] evaluators = new PatternEvaluator[12][];
//      for (int i = 0; i < evaluators.length; i++) {
//        evaluators[i] = new PatternEvaluator[] {new PatternEvaluator(new Side(), 5)};
//      }
//    MultilinearRegression m = new MultilinearRegression(evaluators);
//    m.train(trainingSet, 0.5F, 20);
//    
//    // Empty edge is worth 0, everything else is 100.
//    assertEquals(0, m.eval(empty), 5);
//    assertEquals(100, m.eval(oneOnEdge), 5);
//    assertEquals(100, m.eval(twoOnEdge), 5);
//  }
//  
//  @Test
//  public void testXSquare() {
//    ArrayList<BoardWithEvaluation> trainingSet = new ArrayList<BoardWithEvaluation>();
//    for (int i = 0; i < 10; i++) {
//      trainingSet.add(new BoardWithEvaluation(xSquare, -200));
//      trainingSet.add(new BoardWithEvaluation(fourCorners, 200));
//    }
//    MultilinearRegression m = new MultilinearRegression(0);
//    m.train(trainingSet, 0.5F, 20);
//    for (BoardWithEvaluation b : trainingSet) {
//      assertEquals(0, m.eval(b.board) - b.evaluation);
//    }
//  }
//  
//  @Test
//  public void testSymmetries() {
//    ArrayList<BoardWithEvaluation> trainingSet = new ArrayList<BoardWithEvaluation>();
//    for (int i = 0; i < 10; i++) {
//      trainingSet.add(new BoardWithEvaluation(oneOnEdge, 1));
//      trainingSet.add(new BoardWithEvaluation(twoOnEdge, 2));
//      trainingSet.add(new BoardWithEvaluation(twoOppositeEdges, 0));
//      trainingSet.add(new BoardWithEvaluation(fourCorners, -4));
//    }
//    MultilinearRegression m = new MultilinearRegression();
//    m.train(trainingSet, 0.001F, 20);
//    for (int i = 0; i < 100; i++) {
//      Board b = BoardTest.randomBoard();
//      double d = m.eval(b);
//      for (Board p : Board.allTranspositions(b)) {
//        assertEquals(d, m.eval(p), 0.01);
//      }
//    }
//  }
}
