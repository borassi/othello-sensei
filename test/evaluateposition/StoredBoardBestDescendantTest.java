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
package evaluateposition;

import bitpattern.BitPattern;
import board.Board;
import board.GetMovesCache;
import evaluatedepthone.DiskDifferenceEvaluatorPlusTwo;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashSet;
import org.junit.Test;
import static org.junit.Assert.*;

/**
 *
 * @author michele
 */
public class StoredBoardBestDescendantTest {
  EvaluatorInterface EVALUATOR_BASIC = new DiskDifferenceEvaluatorPlusTwo();

  public class AnyLeafDescendant implements Comparable<AnyLeafDescendant> {
    public StoredBoard board;
    public float value;
    public boolean firstPositionGreaterEqual;
    
    public AnyLeafDescendant(StoredBoard board, float value, boolean greaterEqual) {
      assert board.isLeaf();
      this.board = board;
      this.value = value;
      firstPositionGreaterEqual = this.board.playerIsStartingPlayer ? greaterEqual : !greaterEqual;
    }
    
    @Override
    public int compareTo(AnyLeafDescendant other) {
      return -Float.compare(value, other.value);
    }
  
    @Override
    public String toString() {
      return value + " " + board.toString().replace("\n", "");
    }
  }

  public void allDescendantsStrictlyGreater(
      StoredBoard start, float costParents, ArrayList<AnyLeafDescendant> result) {
    if (costParents == Float.NEGATIVE_INFINITY) {
      return;
    }
    if (start.isLeaf()) {
      if (start.maxLogDerivativeProbStrictlyGreater > Float.NEGATIVE_INFINITY) {
        result.add(new AnyLeafDescendant(start, costParents + start.maxLogDerivativeProbStrictlyGreater, false));
      }
      return;
    }

    for (StoredBoard child : start.children) {
      allDescendantsGreaterEqual(
          child,
          costParents + start.logDerivativeEdgeProbStrictlyGreater(child),
          result);
    }
  }
//  
  public void printDescendants(StoredBoard b, String indent) {
    System.out.println(
        indent + b.maxLogDerivativeProbGreaterEqual + " " + b.maxLogDerivativeProbStrictlyGreater + " " + b.getBoard().toString().replace("\n", ""));
    if (!b.isLeaf()) {
      for (StoredBoard child : b.children) {
        printDescendants(child, indent + "  ");
      }
    }
  }
  
  public void allDescendantsGreaterEqual(
      StoredBoard start, float costParents, ArrayList<AnyLeafDescendant> result) {
    if (costParents == Float.NEGATIVE_INFINITY) {
      return;
    }
    if (start.isLeaf()) {
      if (start.maxLogDerivativeProbGreaterEqual > Float.NEGATIVE_INFINITY) {
        result.add(new AnyLeafDescendant(start, costParents + start.maxLogDerivativeProbGreaterEqual, false));
      }
      return;
    }

    for (StoredBoard child : start.children) {
      allDescendantsStrictlyGreater(
          child,
          costParents + start.logDerivativeEdgeProbGreaterEqual(child),
          result);
    }
  }
  public ArrayList<AnyLeafDescendant> removeDuplicates(ArrayList<AnyLeafDescendant> descendants) {
    HashSet<StoredBoard> duplicates = new HashSet<>();
    ArrayList<AnyLeafDescendant> result = new ArrayList<>();
    for (AnyLeafDescendant descendant : descendants) {
      if (duplicates.contains(descendant.board)) {
        continue;
      }
      result.add(descendant);
      duplicates.add(descendant.board);
    }
    return result;
  }
  
  public ArrayList<AnyLeafDescendant> allDescendantsGreaterEqual(StoredBoard start) {
    ArrayList<AnyLeafDescendant> allDescendants = new ArrayList<>();
    allDescendantsGreaterEqual(start, 0, allDescendants);
    Collections.sort(allDescendants);
    return removeDuplicates(allDescendants);
  }
  
  public ArrayList<AnyLeafDescendant> allDescendantsStrictlyGreater(StoredBoard start) {
    ArrayList<AnyLeafDescendant> allDescendants = new ArrayList<>();
    allDescendantsStrictlyGreater(start, 0, allDescendants);
    Collections.sort(allDescendants);
    return removeDuplicates(allDescendants);
  }
  
  public EvaluatorMCTS randomEvaluatorMCTS(Board start) {
    int nElements = 5 + (int) (Math.random() * 100);
    int totalSize = nElements + (int) (Math.random() * 100);

    EvaluatorMCTS evaluator = new EvaluatorMCTS(totalSize, totalSize, EVALUATOR_BASIC);

    evaluator.setFirstPosition(start.getPlayer(), start.getOpponent());

    for (int j = 0; j < nElements; j++) {
      StoredBoardBestDescendant nextPos = StoredBoardBestDescendant.randomDescendant(
          evaluator.firstPosition, Math.random() > 0.5);

      if (nextPos == null) {
        break;
      }
      StoredBoard next = nextPos.board;
      double d = Math.random();

      long moves = (new GetMovesCache()).getMoves(next.getPlayer(), next.getOpponent());
      if (moves == 0 && (new GetMovesCache()).getMoves(next.getOpponent(), next.getPlayer()) == 0) {
        int correctEval = BitPattern.getEvaluationGameOver(next.getPlayer(), next.getOpponent());
        next.setSolved(correctEval);
        next.updateFathers();
        continue;
      }

      int eval = (int) ((Math.random() - 0.5) * 6400);
      eval = Math.max(Math.min(eval, next.getUpper()), next.getLower());
      if (d <= 0.05) {
        next.setLower(eval);
      } else if (d <= 0.01) {
        next.setUpper(eval);
      } else if (d <= 0.015 || nextPos.board.lower > -6400 || nextPos.board.upper < 6400) {
        next.setSolved(eval);
      } else {
        evaluator.addChildren(nextPos);
      }
      next.updateFathers();
    }
    return evaluator;
  }
  
  public void checkCompatible(
      ArrayList<AnyLeafDescendant> allDescendants,
      ArrayList<StoredBoardBestDescendant> bestDescendants) {
    if (allDescendants.isEmpty() || bestDescendants.isEmpty()) {
      assert bestDescendants.isEmpty() && allDescendants.isEmpty();
      return;
    }
    for (int i = 0; i < bestDescendants.size(); ++i) {
      assertEquals(allDescendants.get(i).value, bestDescendants.get(i).totalLogDerivative, Math.abs(allDescendants.get(i).value) * 1E-4);
    }
    float nthValue = allDescendants.get(bestDescendants.size() - 1).value;
    for (StoredBoardBestDescendant bestDescendant : bestDescendants) {
      boolean found = false;
      for (AnyLeafDescendant descendant : allDescendants) {
        if (bestDescendant.board == descendant.board) {
          found = true;
          assert descendant.value >= nthValue - Math.abs(nthValue) * 1E-4;
          break;
        }
      }
      assert found;
    }
  }
  
  public void checkCompatible(
      ArrayList<AnyLeafDescendant> descendants,
      StoredBoardBestDescendant actual) {
    if (actual == null) {
      checkCompatible(descendants, new ArrayList<>());
    } else {
      checkCompatible(descendants, new ArrayList<>(Arrays.asList(actual)));
    }
  }
  
  @Test
  public void testBestDescendant() {
    for (int i = 0; i < 10000; i++) {
      if (i % 100 == 0) {
        System.out.println("Done " + i);
      }
      Board start = Board.randomBoard();
      if (start.getPlayer() == 0 && start.getOpponent() == 0) {
        continue;
      }
      EvaluatorMCTS evaluator = randomEvaluatorMCTS(start);
      StoredBoardBestDescendant greaterEqual = evaluator.firstPosition.probGreaterEqual == 0 || evaluator.firstPosition.probGreaterEqual == 1 ?
          null :
          StoredBoardBestDescendant.bestDescendant(evaluator.firstPosition, true);
      checkCompatible(allDescendantsGreaterEqual(evaluator.firstPosition), greaterEqual);

      StoredBoardBestDescendant strictlyGreater = evaluator.firstPosition.probStrictlyGreater == 0 || evaluator.firstPosition.probStrictlyGreater == 1 ?
          null :
          StoredBoardBestDescendant.bestDescendant(evaluator.firstPosition, false);
      checkCompatible(allDescendantsStrictlyGreater(evaluator.firstPosition), strictlyGreater);
    }
  }

  @Test
  public void testBestDescendants() {
    for (int i = 0; i < 10000; i++) {
      if (i % 100 == 0) {
        System.out.println("Done " + i);
      }
      Board start = new Board("OO-OOOOOXOOO-O-OO--O-O-OOOO-XOOOOXXOOOOXOOOOOOOOOXOOOOOOOOO-O--O", true);
      // Board start = Board.randomBoard();
      if (start.getPlayer() == 0 && start.getOpponent() == 0) {
        continue;
      }
      EvaluatorMCTS evaluator = randomEvaluatorMCTS(start);
      int n = (int) (Math.random() * 100) + 1;
      
      ArrayList<StoredBoardBestDescendant> actual =
          StoredBoardBestDescendant.bestDescendants(evaluator.firstPosition, n);
      ArrayList<AnyLeafDescendant> expected = allDescendantsGreaterEqual(evaluator.firstPosition);
      expected.addAll(allDescendantsStrictlyGreater(evaluator.firstPosition));

      Collections.sort(expected);
//      System.out.println();
//      printDescendants(evaluator.firstPosition, "");
//      System.out.println(n);
//      if (actual != null && actual.size() > 1) {
//        for (int j = 0; j < actual.size(); ++j) {
//          System.out.println(actual.get(j));
//          System.out.println(expected.get(j));
//        }
//      }
      checkCompatible(expected, actual);

    }
  }
}
