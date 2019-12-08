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

import bitpattern.BitPattern;
import board.Board;
import board.PossibleMovesFinderImproved;
import it.unimi.dsi.fastutil.ints.Int2ObjectOpenHashMap;
import it.unimi.dsi.fastutil.ints.IntOpenHashSet;
import java.util.ArrayList;
import org.junit.Test;
import static org.junit.Assert.*;

/**
 *
 * @author michele
 */
public class PatternEvaluatorImprovedTest {
  static final PatternEvaluatorImproved eval = new PatternEvaluatorImproved();
  static final PatternEvaluatorImproved evalForTest = new PatternEvaluatorImproved();

  @Test
  public void testHorizMirrorFeature() {
    assertArrayEquals(
        new int[] {7, 4, 21, 63},
        PatternEvaluatorImproved.horizMirrorFeature(new int[] {0, 3, 18, 56}));
    assertArrayEquals(
        new int[] {56, 50, 0},
        PatternEvaluatorImproved.horizMirrorFeature(new int[] {63, 53, 7}));
    for (int i = 0; i < 10000; ++i) {
      int[] f = PatternEvaluatorImproved.longToFeature((long) (Math.random() * Long.MAX_VALUE));
      int[] newF = f;
      for (int j = 0; j < 2; ++j) {
        newF = PatternEvaluatorImproved.horizMirrorFeature(newF);
      }
      assertArrayEquals(f, newF);
    }
  }
  
  @Test
  public void testRotateFeature() {
    assertArrayEquals(
        new int[] {7, 63, 56, 0},
        PatternEvaluatorImproved.rotateFeature(new int[] {0, 7, 63, 56}));
    assertArrayEquals(
        new int[] {15, 55, 40},
        PatternEvaluatorImproved.rotateFeature(new int[] {1, 6, 61}));
    assertArrayEquals(
        new int[] {21, 29},
        PatternEvaluatorImproved.rotateFeature(new int[] {18, 19}));
    for (int i = 0; i < 10000; ++i) {
      int[] f = PatternEvaluatorImproved.longToFeature((long) (Math.random() * Long.MAX_VALUE));
      int[] newF = f;
      for (int j = 0; j < 4; ++j) {
        newF = PatternEvaluatorImproved.rotateFeature(newF);
      }
      assertArrayEquals(f, newF);
    }
  }
  
  @Test
  public void testFeatureToLong() {
    for (int i = 0; i < 100000; ++i) {
      long l = (long) (Math.random() * Long.MAX_VALUE);
      assert(l == PatternEvaluatorImproved.featureToLong(PatternEvaluatorImproved.longToFeature(l)));
    }
  }
  

  /**
   * Test of setup method, of class PatternEvaluatorImproved.
   */
  @Test
  public void testHash() {
    for (int i = 0; i < eval.features.length; ++i) {
      long feature = eval.features[i];
      System.out.println(eval.maxBaseHashes[i]);
      ArrayList<Long> patterns = BitPattern.allSubBitPatterns(feature);
      Int2ObjectOpenHashMap<Board> foundPatterns = new Int2ObjectOpenHashMap<>();
      for (long player : patterns) {
        int k = 0;
        for (long opponent : patterns) {
          if (k++ * patterns.size() > 1000000) {
            break;
          }
          if ((player & opponent) == 0) {
//            player = player | (((long) (Math.random() * Long.MAX_VALUE))
//              & ~feature);
//            opponent = opponent | (((long) (Math.random() * Long.MAX_VALUE))
//              & ~feature & ~player);
            eval.setup(player, opponent);
            int hash = eval.baseHashes()[i];
            assert(hash >= 0);
            assertEquals(eval.empties, new Board(player, opponent).getEmptySquares());
            assert(hash <= eval.maxBaseHashes[i]);
            if (foundPatterns.containsKey(hash)) {
              System.out.println(BitPattern.patternToString(feature));
              System.out.println(new Board(player, opponent));
              System.out.println(foundPatterns.get(hash));
              System.out.println(hash);
              assert(false);
            }
            foundPatterns.put(hash, new Board(player, opponent));
          }
        }
      }
    }
  }
//  @Test
//  public void testAllFeatures() {
//    assertEquals(PatternEvaluatorImproved.allFeatures(eval.FEATURE_CORNER_4X4).size(), 4);
//    assertEquals(PatternEvaluatorImproved.allFeatures(eval.FEATURE_DIAGONAL).size(), 2);
//  }
  
  public String toStringBase3(int num, int max) {
    String result = "";
    while (max > 0) {
      switch (num % 3) {
        case 0: result += "O"; break;
        case 1: result += "-"; break;
        case 2: result += "X"; break;
      }
      
      num /= 3;
      max /= 3;
    }
    return result;
  }
  
  @Test
  public void testUpdate() {
    PossibleMovesFinderImproved pmf = new PossibleMovesFinderImproved();
    for (int i = 0; i < 10000; ++i) {
      Board b = Board.randomBoard();
      long[] moves = pmf.possibleMovesAdvanced(b.getPlayer(), b.getOpponent());
      if (moves.length == 0) {
        continue;
      }
      long flip = moves[(int) (Math.random() * moves.length)];
      int move = Long.numberOfTrailingZeros(flip & ~b.getPlayer() & ~b.getOpponent());
      eval.setup(b);
      eval.invert();
      if (flip != 0) {
        eval.update(move, flip);
      }
      Board after = b.move(flip);
      evalForTest.setup(after);
      int[] actual = eval.baseHashes();
      int[] expected = evalForTest.baseHashes();
      assertEquals(eval.empties, evalForTest.empties);
      for (int j = 0; j < actual.length; ++j) {
        if (actual[j] != expected[j]) {
          assert(false);
        }
      }
      assertArrayEquals(eval.baseHashes(), evalForTest.baseHashes());
    }
  }
  
  @Test
  public void testUndoUpdate() {
    PossibleMovesFinderImproved pmf = new PossibleMovesFinderImproved();
    for (int i = 0; i < 10000; ++i) {
      Board b = Board.randomBoard();
      long[] moves = pmf.possibleMovesAdvanced(b.getPlayer(), b.getOpponent());
      if (moves.length == 0) {
        continue;
      }
      long flip = moves[(int) (Math.random() * moves.length)];
      int move = Long.numberOfTrailingZeros(flip & ~b.getPlayer() & ~b.getOpponent());
      eval.setup(b);
      eval.invert();
      if (flip != 0) {
        eval.update(move, flip);
        eval.undoUpdate(move, flip);
      }
      eval.invert();
      evalForTest.setup(b);
      int[] actual = eval.baseHashes();
      int[] expected = evalForTest.baseHashes();
      assertEquals(eval.empties, evalForTest.empties);
      for (int j = 0; j < actual.length; ++j) {
        if (actual[j] != expected[j]) {
          System.out.println(b);
          System.out.println(move);
          System.out.println(BitPattern.patternToString(flip));
          System.out.println(BitPattern.patternToString(
              PatternEvaluatorImproved.featureToLong(eval.featuresToSquaresList.get(j))));
          System.out.println(j + " " + toStringBase3(actual[j], eval.maxBaseHashes[j]) +
              "    " + toStringBase3(expected[j], eval.maxBaseHashes[j]));
          assert(false);
        }
      }
      assertArrayEquals(eval.baseHashes(), evalForTest.baseHashes());
    }
  }
  
//  @Test
//  public void testSetupFeaturesToSquare1() {
//    assertEquals(eval.features[0], PatternEvaluatorImproved.FEATURE_DIAGONAL);
//    assertEquals(eval.features[1], BitPattern.horizontalMirror(PatternEvaluatorImproved.FEATURE_DIAGONAL));
//    assertEquals(eval.features[2], PatternEvaluatorImproved.FEATURE_CORNER_4X4);
//    // 3 to 5 are rotations of corner.
//    assertEquals(eval.features[6], PatternEvaluatorImproved.PATTERN_CORNER);
//    assertEquals(eval.features[7], PatternEvaluatorImproved.PATTERN_LAST1);
//    assertEquals(eval.features[8], PatternEvaluatorImproved.PATTERN_LAST2);
//    assertEquals(eval.features[9], PatternEvaluatorImproved.PATTERN_LAST3);
//    assertEquals(eval.features[10], PatternEvaluatorImproved.PATTERN_LAST4);
//    
//    for (int i = 0; i < 6; ++i) {
//      assertArrayEquals(new int[] {i}, eval.featureToBaseFeature[i]);
//    }
//    for (int i = 6; i < eval.features.length; i += 5) {
//      assertArrayEquals(new int[] {i, i+1, i+2, i+3, i+4}, eval.featureToBaseFeature[6 + (i - 6) / 5]);
//    }
//  }
  
//  @Test
//  public void testSetupFeaturesToSquare() {
//    assertEquals(eval.features[0], PatternEvaluatorImproved.FEATURE_DIAGONAL);
//    assertEquals(eval.features[1], BitPattern.horizontalMirror(PatternEvaluatorImproved.FEATURE_DIAGONAL));
//    assertEquals(eval.features[2], PatternEvaluatorImproved.FEATURE_CORNER_4X4);
//    // 3 to 5 are rotations of corner 3x3.
//    assertEquals(eval.features[6], PatternEvaluatorImproved.PATTERN_LAST1);
//    assertEquals(eval.features[7], PatternEvaluatorImproved.PATTERN_LAST2);
//    assertEquals(eval.features[8], PatternEvaluatorImproved.PATTERN_LAST3);
//    assertEquals(eval.features[9], PatternEvaluatorImproved.PATTERN_LAST4);
//    
//    for (int i = 0; i < 6; ++i) {
//      assertArrayEquals(new int[] {i}, eval.featureToBaseFeature[i]);
//    }
//    for (int i = 6; i < eval.features.length; i += 4) {
//      assertArrayEquals(new int[] {i, i+1, i+2, i+3}, eval.featureToBaseFeature[6 + (i - 6) / 4]);
//    }
//  }
  
  @Test
  public void testEval() {
    eval.randomizeEvals();
    for (int i = 0; i < 100000; ++i) {
      Board b = Board.randomBoard();
      eval.setup(b);
      if (eval.evalSlow() != eval.eval()) {
        System.out.println(b);
        System.out.println(eval.evalVerbose());
        assert(false);
      }
//      assertEquals(eval.evalSlow(), eval.eval());
    }
  }

  @Test
  public void testSave() {
    eval.randomizeEvals();
    eval.save("tmp/PatternEvaluatorImprovedTest.ser");
    PatternEvaluatorImproved evalCopy = PatternEvaluatorImproved.load(
            "tmp/PatternEvaluatorImprovedTest.ser");
    for (int i = 0; i < 100; ++i) {
      Board b = Board.randomBoard();
      eval.setup(b);
      evalCopy.setup(b);
      assertEquals(eval.eval(), evalCopy.eval());
    }
  }
  
//  @Test
//  public void testEvalRotate() {
//    for (int i = 0; i < 100000; ++i) {
//      Board b = Board.randomBoard();
//      eval.setup(b);
//      for (Board transp : b.allTranspositions()) {
//        evalForTest.setup(transp);
//        assertEquals(eval.eval(), evalForTest.eval());
//        assertEquals(eval.evalSlow(), evalForTest.evalSlow());
//      }
//    }
//  }
}
