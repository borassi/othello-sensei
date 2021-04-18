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

  public void allDescendantsStrictlyGreater(
      StoredBoard start, float costDerivativeParents, float costProofNumberParents, ArrayList<StoredBoardBestDescendant> result) {
    if (start.probStrictlyGreater == 1 || start.disproofNumberStrictlyGreater == 0 || start.disproofNumberStrictlyGreater == Float.POSITIVE_INFINITY) {
      return;
    }
    if (start.isLeaf()) {
      result.add(new StoredBoardBestDescendant(start, false, costDerivativeParents, costProofNumberParents));
      return;
    }
    float maxChildrenProofNumber = start.maxFiniteChildrenProofNumber();
    for (StoredBoard child : start.children) {
      if (costDerivativeParents + start.logDerivativeProbStrictlyGreater(child) == Float.NEGATIVE_INFINITY) {
        allDescendantsGreaterEqual(
            child,
            Float.NEGATIVE_INFINITY,
            costProofNumberParents - maxChildrenProofNumber + child.proofNumberGreaterEqual,
            result);
      } else {
        allDescendantsGreaterEqual(
            child,
            costDerivativeParents + start.logDerivativeProbStrictlyGreater(child) - start.maxLogDerivativeProbStrictlyGreater,
            costProofNumberParents,
            result);
      }
    }
  }
  
  public void allDescendantsGreaterEqual(
      StoredBoard start, float costDerivativeParents, float costProofNumberParents, ArrayList<StoredBoardBestDescendant> result) {
    if (start.proofNumberGreaterEqual == 0 || start.proofNumberGreaterEqual == Float.POSITIVE_INFINITY || start.probGreaterEqual == 0) {
      return;
    }
    if (start.isLeaf()) {
      result.add(new StoredBoardBestDescendant(start, true, costDerivativeParents, costProofNumberParents));
      return;
    }
    
    if (costDerivativeParents == Float.NEGATIVE_INFINITY) {
      StoredBoard bestChild = start.bestChildEndgameGreaterEqual();
      allDescendantsStrictlyGreater(
          bestChild,
          Float.NEGATIVE_INFINITY,
          costProofNumberParents,
          result);
    } else {
      StoredBoard bestChild = start.bestChildMidgameGreaterEqual();
      allDescendantsStrictlyGreater(
          bestChild,
          costDerivativeParents - start.maxLogDerivativeProbGreaterEqual + start.logDerivativeProbGreaterEqual(bestChild),
          costProofNumberParents,
          result);
//      for (StoredBoard child : start.children) {
//        if (child.probStrictlyGreater != 1) {
//          allDescendantsStrictlyGreater(
//              child,
//              costDerivativeParents - start.maxLogDerivativeProbGreaterEqual + start.logDerivativeProbGreaterEqual(child),
//              costProofNumberParents,
//              result);
//        }
//      }
    }
  }

  public void printDescendants(StoredBoard b, String indent, boolean greaterEqual) {
    if (greaterEqual) {
      System.out.print(indent + b.maxLogDerivativeProbGreaterEqual + " " + b.proofNumberGreaterEqual + " " + b.probGreaterEqual);
    } else {
      System.out.print(indent + b.maxLogDerivativeProbStrictlyGreater + " " + b.disproofNumberStrictlyGreater + " " + b.probStrictlyGreater);
    }
    System.out.println(" " + b.getBoard().toString().replace("\n", ""));
    if (!b.isLeaf()) {
      for (StoredBoard child : b.children) {
        printDescendants(child, indent + "  ", !greaterEqual);
      }
    }
  }

  public ArrayList<StoredBoardBestDescendant> removeDuplicates(ArrayList<StoredBoardBestDescendant> descendants) {
    HashSet<StoredBoard> duplicates = new HashSet<>();
    ArrayList<StoredBoardBestDescendant> result = new ArrayList<>();
    for (StoredBoardBestDescendant descendant : descendants) {
      if (duplicates.contains(descendant.board)) {
        continue;
      }
      result.add(descendant);
      duplicates.add(descendant.board);
    }
    return result;
  }
  
  public ArrayList<StoredBoardBestDescendant> allDescendantsGreaterEqual(StoredBoard start) {
    ArrayList<StoredBoardBestDescendant> allDescendants = new ArrayList<>();
    allDescendantsGreaterEqual(start, start.maxLogDerivativeProbGreaterEqual == Float.NEGATIVE_INFINITY ? Float.NEGATIVE_INFINITY : 0, 0, allDescendants);
    Collections.sort(allDescendants);
    return removeDuplicates(allDescendants);
  }
  
  public ArrayList<StoredBoardBestDescendant> allDescendantsStrictlyGreater(StoredBoard start) {
    ArrayList<StoredBoardBestDescendant> allDescendants = new ArrayList<>();
    allDescendantsStrictlyGreater(start, 0, 0, allDescendants);
    Collections.sort(allDescendants);
    return removeDuplicates(allDescendants);
  }
  
  public EvaluatorMCTS randomEvaluatorMCTS(Board start) {
    int nElements = 5 + (int) (Math.random() * 100);
    int totalSize = nElements + (int) (Math.random() * 100);

    EvaluatorMCTS evaluator = new EvaluatorMCTS(totalSize, totalSize,  () -> new DiskDifferenceEvaluatorPlusTwo());

    evaluator.setFirstPosition(start.getPlayer(), start.getOpponent());

    for (int j = 0; j < nElements; j++) {
      StoredBoardBestDescendant nextPos;
      boolean greaterEqual = Math.random() > 0.5;
      if (StoredBoardBestDescendant.hasValidDescendants(evaluator.firstPosition, greaterEqual)) {
        nextPos = StoredBoardBestDescendant.randomDescendant(evaluator.firstPosition, greaterEqual);
      } else if (StoredBoardBestDescendant.hasValidDescendants(evaluator.firstPosition, !greaterEqual)) {
        greaterEqual = !greaterEqual;
        nextPos = StoredBoardBestDescendant.randomDescendant(evaluator.firstPosition, greaterEqual);
      } else {
        break;
      }
      assert nextPos != null;
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
//    if (evaluator.firstPosition.probStrictlyGreater == 1 || evaluator.firstPosition.probGreaterEqual == 0) {
//      return randomEvaluatorMCTS(start);
//    }
    return evaluator;
  }
  
  public void checkCompatible(
      ArrayList<StoredBoardBestDescendant> allDescendants,
      ArrayList<StoredBoardBestDescendant> bestDescendants) {
    if (allDescendants.isEmpty() || bestDescendants.isEmpty()) {
      assert bestDescendants.isEmpty() && allDescendants.isEmpty();
      return;
    }
    assert bestDescendants.size() <= allDescendants.size();
    for (int i = 0; i < bestDescendants.size(); ++i) {
      assertEquals(allDescendants.get(i).derivativeLoss, bestDescendants.get(i).derivativeLoss, Math.abs(allDescendants.get(i).derivativeLoss) * 1E-4);
      assertEquals(allDescendants.get(i).proofNumberLoss, bestDescendants.get(i).proofNumberLoss, Math.abs(allDescendants.get(i).proofNumberLoss) * 1E-4);
    }
    float nthDerivativeLoss = allDescendants.get(bestDescendants.size() - 1).derivativeLoss;
    float nthProofNumberLoss = allDescendants.get(bestDescendants.size() - 1).proofNumberLoss;
    for (StoredBoardBestDescendant bestDescendant : bestDescendants) {
      boolean found = false;
      for (StoredBoardBestDescendant descendant : allDescendants) {
        if (bestDescendant.board == descendant.board) {
          found = true;
          assert descendant.derivativeLoss >= nthDerivativeLoss - Math.abs(nthDerivativeLoss) * 1E-4 || (
              descendant.derivativeLoss == Float.NEGATIVE_INFINITY && nthDerivativeLoss == Float.NEGATIVE_INFINITY);
          assert descendant.proofNumberLoss >= nthProofNumberLoss - Math.abs(nthProofNumberLoss) * 1E-4;
          break;
        }
      }
      assert found;
    }
  }
  
  public void checkCompatible(
      ArrayList<StoredBoardBestDescendant> descendants,
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
//      if (i % 100 == 0) {
        System.out.println("Done " + i);
//      }
//      Board start = new Board("OO-OOOOOXOOO-O-OO--O-O-OOOO-XOOOOXXOOOOXOOOOOOOOOXOOOOOOOOO-O--O", true);
      Board start = Board.randomBoard();
      if (start.getPlayer() == 0 && start.getOpponent() == 0) {
        continue;
      }
      EvaluatorMCTS evaluator = randomEvaluatorMCTS(start);
      StoredBoardBestDescendant greaterEqual = StoredBoardBestDescendant.bestDescendant(evaluator.firstPosition, true);

//      printDescendants(evaluator.firstPosition, "", true);
//      System.out.println();
//      System.out.println(greaterEqual);
//      for (StoredBoardBestDescendant b : allDescendantsGreaterEqual(evaluator.firstPosition)) {
//        System.out.println(b);
//      }
      checkCompatible(allDescendantsGreaterEqual(evaluator.firstPosition), greaterEqual);
      StoredBoardBestDescendant strictlyGreater = StoredBoardBestDescendant.bestDescendant(evaluator.firstPosition, false);

      
//      printDescendants(evaluator.firstPosition, "", false);
//      System.out.println();
//      System.out.println(strictlyGreater);
//      for (StoredBoardBestDescendant b : allDescendantsStrictlyGreater(evaluator.firstPosition)) {
//        System.out.println(b);
//      }
      checkCompatible(allDescendantsStrictlyGreater(evaluator.firstPosition), strictlyGreater);
    }
  }

  @Test
  public void testBestDescendants() {
    for (int i = 0; i < 10000; i++) {
      System.out.println("Done " + i);
//      Board start = new Board("OO-OOOOOXOOO-O-OO--O-O-OOOO-XOOOOXXOOOOXOOOOOOOOOXOOOOOOOOO-O--O", true);
      Board start = Board.randomBoard();
      if (start.getPlayer() == 0 && start.getOpponent() == 0) {
        continue;
      }
      EvaluatorMCTS evaluator = randomEvaluatorMCTS(start);
      int n = 1;//(int) (Math.random() * 100) + 1;
      boolean greaterEqual = Math.random() > 0.5;
      
      ArrayList<StoredBoardBestDescendant> actual =
          StoredBoardBestDescendant.bestDescendants(evaluator.firstPosition, n);
      ArrayList<StoredBoardBestDescendant> expected = allDescendantsGreaterEqual(evaluator.firstPosition);
      expected.addAll(allDescendantsStrictlyGreater(evaluator.firstPosition));
      Collections.sort(expected);
      

//      System.out.println("\n" + (greaterEqual ? "greaterequal" : "strictlygreater"));
//      printDescendants(evaluator.firstPosition, "", greaterEqual);
//      System.out.println(n);
      for (int j = 0; j < Math.min(n + 5, expected.size()); ++j) {
        System.out.println(expected.get(j));
        if (actual.size() <= j) {
          System.out.println("= -");
        } else {
          System.out.println("= " + actual.get(j));
        }
      }
      checkCompatible(expected, actual);

    }
  }
}
