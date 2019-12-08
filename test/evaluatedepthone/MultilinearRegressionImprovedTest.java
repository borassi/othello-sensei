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
import board.BoardTest;
import evaluatedepthone.patternhasher.Side;
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
  public void testTrainNumberOfDisks() {
    ArrayList<BoardWithEvaluation> trainingSet = new ArrayList<>();
    for (int i = 0; i < 10; i++) {
      trainingSet.add(new BoardWithEvaluation(oneOnEdge, 100));
      trainingSet.add(new BoardWithEvaluation(twoOnEdge, 200));
      trainingSet.add(new BoardWithEvaluation(twoOppositeEdges, 0));
      trainingSet.add(new BoardWithEvaluation(fourCorners, -400));
    }

    eval.train(trainingSet, trainingSet, 0.05F, 20);
    for (BoardWithEvaluation b : trainingSet) {
      assertEquals(0, eval.eval(b.board) - b.evaluation, 10);
    }
    eval.train(trainingSet, trainingSet, 0.01F, 20);
    for (BoardWithEvaluation b : trainingSet) {
      assertEquals(eval.eval(b.board), b.evaluation, 5);
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
