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
import java.util.ArrayList;
import org.junit.Test;
import static org.junit.Assert.*;

/**
 *
 * @author michele
 */
public class PatternEvaluatorImprovedTest {
  static final PatternEvaluatorImproved EVAL = new PatternEvaluatorImproved();
  static final PatternEvaluatorImproved EVAL_FOR_TEST = new PatternEvaluatorImproved();

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
    for (int i = 0; i < EVAL.features.length; ++i) {
      long feature = EVAL.features[i];
      ArrayList<Long> patterns = BitPattern.allSubBitPatterns(feature);
      Int2ObjectOpenHashMap<Board> foundPatterns = new Int2ObjectOpenHashMap<>();
      for (long player : patterns) {
        int k = 0;
        for (long opponent : patterns) {
          if (k++ * patterns.size() > 1000000) {
            break;
          }
          if ((player & opponent) == 0) {
            player = player | (((long) (Math.random() * Long.MAX_VALUE))
              & ~feature);
            opponent = opponent | (((long) (Math.random() * Long.MAX_VALUE))
              & ~feature & ~player);
            EVAL.setup(player, opponent);
            int hash = EVAL.baseHashes()[i];
            assert(hash >= 0);
            assertEquals(EVAL.empties, new Board(player, opponent).getEmptySquares());
            assert(hash <= EVAL.maxBaseHashes[i]);
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
  @Test
  public void testAllFeatures() {
    assertEquals(PatternEvaluatorImproved.allFeatures(PatternEvaluatorImproved.FEATURE_CORNER_4X4).size(), 4);
    assertEquals(PatternEvaluatorImproved.allFeatures(PatternEvaluatorImproved.FEATURE_DIAGONAL).size(), 2);
  }
  
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
      EVAL.setup(b);
      EVAL.invert();
//      System.out.println(b);
      if (flip != 0) {
        EVAL.update(move, flip);
      }
      Board after = b.move(flip);
      EVAL_FOR_TEST.setup(after);
//      System.out.println(after);
//      System.out.println(new Board(eval.player, eval.opponent) + "\n\n\n\n");
//      System.out.println(evalForTest);
      assertEquals(EVAL.empties, EVAL_FOR_TEST.empties);
//      assertEquals(eval.player, after.getPlayer());
//      assertEquals(eval.opponent, after.getOpponent());
      int[] actual = EVAL.baseHashes();
      int[] expected = EVAL_FOR_TEST.baseHashes();
      for (int j = 0; j < actual.length; ++j) {
        if (actual[j] != expected[j]) {
          assert(false);
        }
      }
      assertArrayEquals(EVAL.baseHashes(), EVAL_FOR_TEST.baseHashes());
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
      EVAL.setup(b);
      EVAL.invert();
      if (flip != 0) {
        EVAL.update(move, flip);
        EVAL.undoUpdate(move, flip);
      }
      EVAL.invert();
      EVAL_FOR_TEST.setup(b);
      int[] actual = EVAL.baseHashes();
      int[] expected = EVAL_FOR_TEST.baseHashes();
//      assertEquals(eval.player, b.getPlayer());
//      assertEquals(eval.opponent, b.getOpponent());
      assertEquals(EVAL.empties, EVAL_FOR_TEST.empties);
      for (int j = 0; j < actual.length; ++j) {
        if (actual[j] != expected[j]) {
          System.out.println(b);
          System.out.println(move);
          System.out.println(BitPattern.patternToString(flip));
          System.out.println(BitPattern.patternToString(PatternEvaluatorImproved.featureToLong(EVAL.featuresToSquaresList.get(j))));
          System.out.println(j + " " + toStringBase3(actual[j], EVAL.maxBaseHashes[j]) +
              "    " + toStringBase3(expected[j], EVAL.maxBaseHashes[j]));
          assert(false);
        }
      }
      assertArrayEquals(EVAL.baseHashes(), EVAL_FOR_TEST.baseHashes());
    }
  }
  
  @Test
  public void testSetupFeaturesToSquare() {
    for (int i = 0; i < 20; i += 5) {
      assertArrayEquals(new int[] {i, i+1, i+2, i+3, i+4}, EVAL.featureToBaseFeature[i / 5]);
    }
    assertEquals(EVAL.features[20], PatternEvaluatorImproved.FEATURE_IMPR_DIAGONAL);
    assertEquals(EVAL.features[21], BitPattern.horizontalMirror(PatternEvaluatorImproved.FEATURE_IMPR_DIAGONAL));
    assertEquals(EVAL.features[22], PatternEvaluatorImproved.FEATURE_CORNER_4X4);
    // 23 to 25 are rotations of corner 4x4.
    assertEquals(EVAL.features[26], PatternEvaluatorImproved.PATTERN_LAST1 << 16);
//    assertEquals(EVAL.features[27], PatternEvaluatorImproved.PATTERN_LAST2);
//    assertEquals(EVAL.features[28], PatternEvaluatorImproved.PATTERN_LAST3);
//    assertEquals(EVAL.features[29], PatternEvaluatorImproved.PATTERN_LAST4);
    
    for (int i = 20; i < EVAL.features.length; ++i) {
      assertArrayEquals(new int[] {i}, EVAL.featureToBaseFeature[i - 16]);
    }
  }
  
  @Test
  public void testEval() {
    EVAL.randomizeEvals();
    for (int i = 0; i < 100000; ++i) {
      Board b = Board.randomBoard();
      EVAL.setup(b);
//      System.out.println(EVAL.eval() + " " + EVAL.evalSlow());
      int correctEval = EVAL.evalSlow();
      float lastError = EVAL.lastError();
      if (correctEval != EVAL.eval() || Math.abs(lastError - EVAL.lastError()) > 0.01) {
        System.out.println(b);
        System.out.println();
        assert(false);
      }
//      assertEquals(eval.evalSlow(), eval.eval());
    }
  }

  @Test
  public void testSave() {
    EVAL.randomizeEvals();
    EVAL.save("tmp/PatternEvaluatorImprovedTest.ser");
    PatternEvaluatorImproved evalCopy = PatternEvaluatorImproved.load(
            "tmp/PatternEvaluatorImprovedTest.ser");
    for (int i = 0; i < 100; ++i) {
      Board b = Board.randomBoard();
      EVAL.setup(b);
      evalCopy.setup(b);
      assertEquals(EVAL.evalSlow(), evalCopy.evalSlow());
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
