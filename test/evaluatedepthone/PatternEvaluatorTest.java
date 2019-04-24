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

import static org.junit.Assert.*;

import org.junit.Test;

import bitpattern.BitPattern;
import board.Board;
import evaluatedepthone.patternhasher.Side;

public class PatternEvaluatorTest {
  
  
  final static Board oneOnEdge = new Board(
          "---X----\n"
        + "--------\n"
        + "--------\n"
        + "--------\n"
        + "--------\n"
        + "--------\n"
        + "--------\n"
        + "--------\n",
        true);

  final static Board twoOnEdge = new Board(
          "---XX---\n"
        + "--------\n"
        + "--------\n"
        + "--------\n"
        + "--------\n"
        + "--------\n"
        + "--------\n"
        + "--------\n",
        true);

  long edgeBitPattern = BitPattern.parsePattern(
          "XXXXXXXX\n"
        + "--------\n" 
        + "--------\n"
        + "--------\n"
        + "--------\n"
        + "--------\n"
        + "--------\n"
        + "--------\n");

  @Test
  public void testPatternEvaluator() {
    PatternEvaluator evaluator = new PatternEvaluator(new Side(), 0);
    assertEquals(0, evaluator.evalVerbose(oneOnEdge.allRotations()), 1.E-6F);
    assertEquals(0, evaluator.evalVerbose(twoOnEdge.allRotations()), 1.E-6F);
    
    evaluator.updateNumberOfAppearences(oneOnEdge.allRotations());
    evaluator.updateNumberOfAppearences(twoOnEdge.allRotations());

    // The oneOnEdge board is worth 100 and not 0. Hence, we update
    // the value of all the corresponding edges by adding 2 * 10.
    // The empty edge appears 6 times and 3 in this board, so it values 10. 
    // The edge with one element values 20. 
    evaluator.resetEvaluatorFloat();
    evaluator.update(oneOnEdge.allRotations(), 100, 0.1);
    evaluator.setMissingEval();
    
    assertEquals(50, evaluator.evalVerbose(oneOnEdge.allRotations()), 1.E-6);
    assertEquals(30, evaluator.evalVerbose(twoOnEdge.allRotations()), 1.E-6);

    // The twoOnEdge board is worth -30 and not 30. Hence, we update
    // the value of all the corresponding edges by adding
    // 2 * (-60) * 0.1 = -12.
    // The empty edge appears 6 times and 3 in this board, so it values
    // 10 - 12 * 1/2 = 6.
    // The edge with 2 elements values -12.
    evaluator.resetEvaluatorFloat();
    evaluator.update(twoOnEdge.allRotations(), -60, 0.1);
    evaluator.setMissingEval();
    
    assertEquals(4 * 3 + 20, evaluator.evalVerbose(oneOnEdge.allRotations()), 1.E-6);
    assertEquals(4 * 3 - 12, evaluator.evalVerbose(twoOnEdge.allRotations()), 1.E-6);
  }
  
  @Test
  public void testMinNumberOfAppearences() {
    PatternEvaluator evaluator = new PatternEvaluator(new Side(), 2);
    evaluator.updateNumberOfAppearences(oneOnEdge.allRotations());
    // The oneOnEdge board is worth 1 and not 0. Hence, we update
    // the value of all the corresponding edges by adding 2 * 0.1.
    // The empty edge values 0.2, as the missing edges.
    evaluator.resetEvaluatorFloat();
    evaluator.update(oneOnEdge.allRotations(), 100, 0.1);
    evaluator.setMissingEval();
    assertEquals(20 * 3 + 20, evaluator.eval(oneOnEdge.allRotations()), 1.E-6);
    assertEquals(20 * 3 + 20, evaluator.eval(twoOnEdge.allRotations()), 1.E-6);
    
    evaluator.resetEvaluatorFloat();
    evaluator.evalVerbose(oneOnEdge.allRotations());
    evaluator.update(oneOnEdge.allRotations(), 100, 0.1);
    evaluator.setMissingEval();
    assertEquals(2 * (20 * 3 + 20), evaluator.evalVerbose(oneOnEdge.allRotations()), 1.E-6);
    assertEquals(2 * (20 * 3 + 20), evaluator.evalVerbose(twoOnEdge.allRotations()), 1.E-6);
  }
}
