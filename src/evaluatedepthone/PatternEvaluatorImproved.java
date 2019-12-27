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

import board.Board;
import board.PossibleMovesFinderImproved;
import bitpattern.BitPattern;
//import evaluatedepthone.patternhasher.StableDisksMiddle;
import it.unimi.dsi.fastutil.ints.IntArrayList;
import it.unimi.dsi.fastutil.longs.LongOpenHashSet;
import it.unimi.dsi.fastutil.objects.ObjectArrayList;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import java.util.Arrays;

/**
 *
 * @author michele
 */
public class PatternEvaluatorImproved implements Serializable, DepthOneEvaluator {
  /**
   * Needed to implement Serializable.
   */
  private static final long serialVersionUID = 1L;
  public static final String DEPTH_ONE_EVALUATOR_FILEPATTERN = 
          "coefficients/pattern_evaluator_improved1.sar";
  long features[];
//  long player;
//  long opponent;
  ObjectArrayList<int[]> featuresToSquaresList;
  static final int EMPTIES_SPLITS = 20; // SET TO <= 2 FOR TESTS
  static final long FEATURE_CORNER_4X4 = BitPattern.parsePattern("--------" +
                                                                 "--------" +
                                                                 "--------" +
                                                                 "--------" +
                                                                 "------XX" +
                                                                 "-----XXX" +
                                                                 "----XXXX" +
                                                                 "----XXXX");
  static final long FEATURE_CORNER_SIDED = BitPattern.parsePattern("--------" +
                                                                 "--------" +
                                                                 "--------" +
                                                                 "-------X" +
                                                                 "-------X" +
                                                                 "-------X" +
                                                                 "------XX" +
                                                                 "---XXXXX");
  static final long FEATURE_EDGE_BOH = BitPattern.parsePattern("--------" +
                                                                 "--------" +
                                                                 "--------" +
                                                                 "--------" +
                                                                 "--------" +
                                                                 "--------" +
                                                                 "--XXXX--" +
                                                                 "X-XXXX-X");
  static final long FEATURE_IMPR_DIAGONAL = BitPattern.parsePattern("XX------" +
                                                                    "XX------" +
                                                                    "--X-----" +
                                                                    "---X----" +
                                                                    "----X---" +
                                                                    "-----X--" +
                                                                    "------XX" +
                                                                    "------XX");
  static final long FEATURE_DIAGONAL = BitPattern.parsePattern("X-------" +
                                                               "-X------" +
                                                               "--X-----" +
                                                               "---X----" +
                                                               "----X---" +
                                                               "-----X--" +
                                                               "------X-" +
                                                               "-------X");
//  static final long FEATURE_LAST_ROW = BitPattern.LAST_ROW_BIT_PATTERN;
//  static final long FEATURE_LAST_BUT_ONE_ROW = BitPattern.LAST_ROW_BIT_PATTERN << 8;
//  static final long FEATURE_LAST_BUT_TWO_ROW = BitPattern.LAST_ROW_BIT_PATTERN << 16;
//  static final long FEATURE_LAST_BUT_THREE_ROW = BitPattern.LAST_ROW_BIT_PATTERN << 24;

  final static long PATTERN_CORNER = BitPattern.parsePattern("--------\n" 
                                                           + "--------\n"
                                                           + "--------\n"
                                                           + "--------\n"
                                                           + "--------\n"
                                                           + "--------\n"
                                                           + "-X----X-\n"
                                                           + "X------X\n");

  final static long PATTERN_LAST1 = BitPattern.parsePattern("--------\n" 
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "XXXXXXXX\n");

  final static long PATTERN_LAST2 = BitPattern.parsePattern("--------\n" 
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "XXXXXXXX\n"
                                                          + "--------\n");

  final static long PATTERN_SM_LAST1 = BitPattern.parsePattern("--------\n" 
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "-XXXXXX-\n");

  final static long PATTERN_SM_LAST2 = BitPattern.parsePattern("--------\n" 
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "--XXXX--\n"
                                                          + "--------\n");

  final static long PATTERN_LAST3 = BitPattern.parsePattern("--------\n" 
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "-XXXXXX-\n"
                                                          + "--------\n"
                                                          + "--------\n");

  final static long PATTERN_LAST4 = BitPattern.parsePattern("--------\n" 
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "-XXXXXX-\n"
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "--------\n");

  final static long PATTERN_CORNER_5x2 = BitPattern.parsePattern("--------\n" 
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "---XXXXX\n"
                                                          + "---XXXXX\n");
  
  static long[][] FEATURE_GROUPS = {
    {PATTERN_CORNER, PATTERN_SM_LAST1, PATTERN_SM_LAST2, PATTERN_LAST3, PATTERN_LAST4},
    {FEATURE_IMPR_DIAGONAL},
    {FEATURE_CORNER_4X4},
    {PATTERN_LAST1 << 16}, 
    {PATTERN_LAST1 << 24},
    {FEATURE_DIAGONAL << 8},
    {FEATURE_DIAGONAL << 16},
  };
  
  int featureToBaseFeature[][];

  int squaresToFeatureSingle[][] = new int[64][];
  int squaresToFeatureDouble[][] = new int[64][];
  int[] hashes;
  int[] baseHashes;
  int empties;
  int[] maxBaseHashes;
  int[] emptyBaseHashes;

  int[][][] evals;

  static final int[] longToFeature(long pattern) {
    IntArrayList feature = new IntArrayList();
    int square;
    while (true) {
      square = Long.numberOfTrailingZeros(pattern);
      feature.add(square);
      pattern = pattern & ~(1L << square);
      if (pattern == 0) {
        break;
      }
    }
    return feature.toArray(new int[feature.size()]);
  }

  static final long featureToLong(int[] feature) {
    long result = 0;
    for (int i : feature) {
      result |= 1L << i;
    }
    return result;
  }
  
  static final int[] horizMirrorFeature(int[] feature) {
    int[] newFeature = new int[feature.length];
    for (int i = 0; i < feature.length; ++i) {
      int f = feature[i];
      int row = f / 8;
      int column = f % 8;
      int newRow = row;
      int newColumn = 7 - column;
      newFeature[i] = newRow * 8 + newColumn;
    }
    return newFeature;
  }
  
  static final int[] rotateFeature(int[] feature) {
    int[] newFeature = new int[feature.length];
    for (int i = 0; i < feature.length; ++i) {
      int f = feature[i];
      int row = f / 8;
      int column = f % 8;
      int newRow = column;
      int newColumn = 7 - row;
      newFeature[i] = newRow * 8 + newColumn;
    }
    return newFeature;
  }
  
  static final ObjectArrayList<int[]> allFeatures(long pattern) {
    LongOpenHashSet seen = new LongOpenHashSet();
    int[] feature = longToFeature(pattern);
    ObjectArrayList<int[]> result = new ObjectArrayList<>();

    for (int i = 0; i < 1; ++i) {
      for (int j = 0; j < 4; ++j) {
        if (!seen.contains(featureToLong(feature))) {
          result.add(feature);
          seen.add(featureToLong(feature));
        }
        feature = rotateFeature(feature);
      }
    }
    return result;
  }
  
  public int[] hashes() {
    for (int i = 0; i < featureToBaseFeature.length; ++i) {
      hashes[i] = 0;
      int j = 0;
      while (baseHashes[featureToBaseFeature[i][j]] == emptyBaseHashes[featureToBaseFeature[i][j]] &&
             j < featureToBaseFeature[i].length - 2) {
        hashes[i] += (maxBaseHashes[featureToBaseFeature[i][j]] + 1) * (maxBaseHashes[featureToBaseFeature[i][j+1]] + 1);
        ++j;
      }
      if (j == featureToBaseFeature[i].length - 1) {
        hashes[i] = baseHashes[featureToBaseFeature[i][j]];
      } else {
        hashes[i] += baseHashes[featureToBaseFeature[i][j]] +
            (maxBaseHashes[featureToBaseFeature[i][j]] + 1) * baseHashes[featureToBaseFeature[i][j+1]];
      }
    }
//    hashes[featureToBaseFeature.length] = StableDisksMiddle.hash(new Board(player, opponent));
    return hashes;
  }

//  public int[] hashes() {
//    for (int i = 0; i < featureToBaseFeature.length; ++i) {
//      hashes[i] = 0;
//      int j = 0;
//      while (baseHashes[featureToBaseFeature[i][j]] == emptyBaseHashes[featureToBaseFeature[i][j]] &&
//             j < featureToBaseFeature[i].length - 1) {
//        ++j;
//        hashes[i] += (maxBaseHashes[featureToBaseFeature[i][0]] + 1);
//      }
//      hashes[i] += baseHashes[featureToBaseFeature[i][j]];
//    }
//    return hashes;
//  }
  
  public int evalVerbose() {
//    float[][] curEvals = evals[getEvalFromEmpties(empties)];
//    System.out.println("NWSE Diag: " + curEvals[0][baseHashes[0]]);
//    System.out.println("NESW Diag: " + curEvals[0][baseHashes[1]]);
//    System.out.println("SE Corner: " + curEvals[2][baseHashes[2]]);
//    System.out.println("NE Corner: " + curEvals[2][baseHashes[3]]);
//    System.out.println("NW Corner: " + curEvals[2][baseHashes[4]]);
//    System.out.println("SW Corner: " + curEvals[2][baseHashes[5]]);
//    
//    int eval = 0;
//    if (baseHashes[6] != 3280) {
//      eval += curEvals[6][baseHashes[6] + baseHashes[7] * 6561];
//    } else if (baseHashes[7] != 3280) {
//      eval += curEvals[6][43046721 + baseHashes[7] + baseHashes[8] * 6561]; 
//    } else {
//      eval += curEvals[6][86093442 + baseHashes[8] + baseHashes[9] * 6561]; 
//    }
//    System.out.println("S Edge:    " + eval);
//    eval = 0;
//    
//    if (baseHashes[10] != 3280) {
//      eval += curEvals[6][baseHashes[10] + baseHashes[11] * 6561];
//    } else if (baseHashes[11] != 3280) {
//      eval += curEvals[6][43046721 + baseHashes[11] + baseHashes[12] * 6561]; 
//    } else {
//      eval += curEvals[6][86093442 + baseHashes[12] + baseHashes[13] * 6561]; 
//    }
//    System.out.println("E Edge:    " + eval);
//    eval = 0;
//  
//    if (baseHashes[14] != 3280) {
//      eval += curEvals[6][baseHashes[14] + baseHashes[15] * 6561];
//    } else if (baseHashes[15] != 3280) {
//      eval += curEvals[6][43046721 + baseHashes[15] + baseHashes[16] * 6561]; 
//    } else {
//      eval += curEvals[6][86093442 + baseHashes[16] + baseHashes[17] * 6561]; 
//    }
//    System.out.println("N Edge:    " + eval);
//    eval = 0;
////    eval += baseHashes[18] != 3280 ?
////        eval9[baseHashes[18] + baseHashes[19] * 6561] :
////        baseHashes[19] != 3280 ?
////            eval9[43046721 + baseHashes[19] + baseHashes[20] * 6561] :
////            eval9[86093442 + baseHashes[20] + baseHashes[21] * 6561];
//    
//    if (baseHashes[18] != 3280) {
//      eval += curEvals[6][baseHashes[18] + baseHashes[19] * 6561];
//    } else if (baseHashes[19] != 3280) {
//      eval += curEvals[6][43046721 + baseHashes[19] + baseHashes[20] * 6561]; 
//    } else {
//      eval += curEvals[6][86093442 + baseHashes[20] + baseHashes[21] * 6561]; 
//    }
//    System.out.println("W Edge:    " + eval);
////    eval = 0;
    
    return eval();
  }
  
//    {PATTERN_CORNER, PATTERN_SM_LAST1, PATTERN_SM_LAST2, PATTERN_LAST3, PATTERN_LAST4},
//    {FEATURE_IMPR_DIAGONAL},
//    {FEATURE_CORNER_4X4},
//    {PATTERN_LAST1 << 16}, 
//    {PATTERN_LAST1 << 24},
//    {FEATURE_DIAGONAL << 8},
//    {FEATURE_DIAGONAL << 16},
  public int eval() {
//    return this.evalSlow();
    int[][] curEvals = evals[getEvalFromEmpties(empties)];
    int eval = 0;
    
    if (baseHashes[0] != 40) {
      eval += curEvals[0][baseHashes[0] + baseHashes[1] * 81];
    } else if (baseHashes[1] != 364) {
      eval += curEvals[0][59049 + baseHashes[1] + baseHashes[2] * 729]; 
    } else if (baseHashes[2] != 40) {
      eval += curEvals[0][118098 + baseHashes[2] + baseHashes[3] * 81]; 
    } else {
      eval += curEvals[0][124659 + baseHashes[3] + baseHashes[4] * 81]; 
    }
    
    if (baseHashes[5] != 40) {
      eval += curEvals[0][baseHashes[5] + baseHashes[6] * 81];
    } else if (baseHashes[6] != 364) {
      eval += curEvals[0][59049 + baseHashes[6] + baseHashes[7] * 729]; 
    } else if (baseHashes[7] != 40) {
      eval += curEvals[0][118098 + baseHashes[7] + baseHashes[8] * 81]; 
    } else {
      eval += curEvals[0][124659 + baseHashes[8] + baseHashes[9] * 81]; 
    }
    
    if (baseHashes[10] != 40) {
      eval += curEvals[0][baseHashes[10] + baseHashes[11] * 81];
    } else if (baseHashes[11] != 364) {
      eval += curEvals[0][59049 + baseHashes[11] + baseHashes[12] * 729]; 
    } else if (baseHashes[12] != 40) {
      eval += curEvals[0][118098 + baseHashes[12] + baseHashes[13] * 81]; 
    } else {
      eval += curEvals[0][124659 + baseHashes[13] + baseHashes[14] * 81]; 
    }
    
    if (baseHashes[15] != 40) {
      eval += curEvals[0][baseHashes[15] + baseHashes[16] * 81];
    } else if (baseHashes[16] != 364) {
      eval += curEvals[0][59049 + baseHashes[16] + baseHashes[17] * 729]; 
    } else if (baseHashes[17] != 40) {
      eval += curEvals[0][118098 + baseHashes[17] + baseHashes[18] * 81]; 
    } else {
      eval += curEvals[0][124659 + baseHashes[18] + baseHashes[19] * 81]; 
    }
    // TODO: Replace eval1 with eval0, eval3 with eval2, etc.
    return eval + curEvals[4][baseHashes[20]] + curEvals[4][baseHashes[21]] + // Diagonals
        curEvals[6][baseHashes[22]] + curEvals[6][baseHashes[23]] +           // Corner
        curEvals[6][baseHashes[24]] + curEvals[6][baseHashes[25]] +           // Corner
        curEvals[10][baseHashes[26]] + curEvals[10][baseHashes[27]] +         // Central column
        curEvals[10][baseHashes[28]] + curEvals[10][baseHashes[29]] +         // Central column
        curEvals[14][baseHashes[30]] + curEvals[14][baseHashes[31]] +         // Central-1 column
        curEvals[14][baseHashes[32]] + curEvals[14][baseHashes[33]] +         // Central-1 column
        curEvals[18][baseHashes[34]] + curEvals[18][baseHashes[35]] +         // Diag-1
        curEvals[18][baseHashes[36]] + curEvals[18][baseHashes[37]] +         // Diag-1
        curEvals[22][baseHashes[38]] + curEvals[22][baseHashes[39]] +         // Diag-2
        curEvals[22][baseHashes[40]] + curEvals[22][baseHashes[41]];          // Diag-2
  }
  
  public static final int getEvalFromEmpties(int empties) {
    return Math.min((empties - 1) * EMPTIES_SPLITS / 60, EMPTIES_SPLITS - 1);
  }

  public int evalSlow() {
    int eval = 0;
    int[] curHashes = hashes();
    int[][] curEval = evals[getEvalFromEmpties(empties)];
    for (int i = 0; i < curEval.length; ++i) {
      eval += curEval[i][curHashes[i]];
    }
    return eval;
  }
  
  public int eval(Board b) {
    return this.eval(b.getPlayer(), b.getOpponent());
  }
  
  public int eval(long player, long opponent) {
    this.setup(player, opponent);
    return this.eval();
  }
  
  public int getPositions(long feature) {
    for (int i = 0; i < features.length; ++i) {
      int[] f = featuresToSquaresList.get(i);
      if (featureToLong(f) == feature) {
        return i;
      }
    }
    return -1;
  }
  
  public void invert() {
    for (int i = 0; i < baseHashes.length; ++i) {
      baseHashes[i] = maxBaseHashes[i] - baseHashes[i];
    }
//    long tmp = player;
//    player = opponent;
//    opponent = tmp;
  }
  
  public void undoUpdate(int square, long flip) {
//    this.opponent = opponent & ~flip;
    flip = flip & ~(1L << square);
//    this.player = player | flip;
    empties++;
    for (int update : squaresToFeatureSingle[square]) {
      baseHashes[update & 127] += update >>> 7;
    }
    while (flip != 0) {
      square = Long.numberOfTrailingZeros(flip);
      for (int update : squaresToFeatureDouble[square]) {
        baseHashes[update & 127] += update >>> 7;
      }
      flip = flip & ~(1L << square);
    }
  }
  
  public void update(int square, long flip) {
//    System.out.println(BitPattern.patternToString(flip));
//    this.opponent = opponent | flip;
//    this.player = player & ~flip;
    flip = flip & ~(1L << square);
    empties--;
    for (int update : squaresToFeatureSingle[square]) {
      baseHashes[update & 127] -= update >>> 7;
    }
    while (flip != 0) {
      square = Long.numberOfTrailingZeros(flip);
      for (int update : squaresToFeatureDouble[square]) {
        baseHashes[update & 127] -= update >>> 7;
      }
      flip = flip & ~(1L << square);
    }
  }

  public int[] baseHashes() {
    return baseHashes;
  }
  
  @Override
  public void setup(long player, long opponent) {
    assert((player & opponent) == 0);
    Arrays.fill(baseHashes, 0);
//    this.player = player;
//    this.opponent = opponent;
    long emptiesLong = ~(player | opponent);
    empties = Long.bitCount(emptiesLong);
    int square;
    while (player != 0) {
      square = Long.numberOfTrailingZeros(player);
      for (int update : squaresToFeatureDouble[square]) {
        baseHashes[update & 127] += update >>> 7;
      }
      player = player & ~(1L << square);
    }
    while (emptiesLong != 0) {
      square = Long.numberOfTrailingZeros(emptiesLong);
      for (int update : squaresToFeatureSingle[square]) {
        baseHashes[update & 127] += update >>> 7;
      }
      emptiesLong = emptiesLong & ~(1L << square);
    }
  }
  
  public void setup(Board b) {
    setup(b.getPlayer(), b.getOpponent());
  }
  
  public final void setupFeaturesToSquares() {
    ObjectArrayList<IntArrayList> featureToBaseFeaturesList = new ObjectArrayList<>();
    this.featuresToSquaresList = new ObjectArrayList<>();
    for (long[] featureGroup : FEATURE_GROUPS) {
      ObjectArrayList<ObjectArrayList<int[]>> baseFeatures = new ObjectArrayList<>();
      for (long featureLong : featureGroup) {
        baseFeatures.add(allFeatures(featureLong));
      }
      for (int i = 0; i < baseFeatures.get(0).size(); ++i) {
        IntArrayList baseFeature = new IntArrayList();
        for (int j = 0; j < baseFeatures.size(); ++j) {
          assert(baseFeatures.get(0).size() == baseFeatures.get(j).size());
          this.featuresToSquaresList.add(baseFeatures.get(j).get(i));
          baseFeature.add(this.featuresToSquaresList.size() - 1);
        }
        featureToBaseFeaturesList.add(baseFeature);
      }
    }
    featureToBaseFeature = new int[featureToBaseFeaturesList.size()][];
    for (int i = 0; i < featureToBaseFeaturesList.size(); ++i) {
      featureToBaseFeature[i] = featureToBaseFeaturesList.get(i).toArray(new int[featureToBaseFeaturesList.get(i).size()]);
    }
  }
  
  public PatternEvaluatorImproved() {
    IntArrayList[] squaresToFeatureSingleList = new IntArrayList[64];
    IntArrayList[] squaresToFeatureDoubleList = new IntArrayList[64];

    setupFeaturesToSquares();

//    System.out.println(featuresToSquaresList.size());
    assert(featuresToSquaresList.size() < 127);
    features = new long[featuresToSquaresList.size()];
    maxBaseHashes = new int[featuresToSquaresList.size()];
    emptyBaseHashes = new int[featuresToSquaresList.size()];
    baseHashes = new int[featuresToSquaresList.size()];
    hashes = new int[featureToBaseFeature.length];
    evals = new int[EMPTIES_SPLITS][hashes.length][];

    for (int i = 0; i < 64; ++i) {
      squaresToFeatureSingleList[i] = new IntArrayList();
      squaresToFeatureDoubleList[i] = new IntArrayList();
    }
    for (int j = 0; j < featuresToSquaresList.size(); ++j) {
      int squareList[] = featuresToSquaresList.get(j);
      features[j] = featureToLong(squareList);
      for (int k = 0; k < squareList.length; ++k) {
        squaresToFeatureSingleList[squareList[k]].add((int) Math.pow(3, k) * 128 + j);
        squaresToFeatureDoubleList[squareList[k]].add((int) (2 * Math.pow(3, k)) * 128 + j);
        maxBaseHashes[j] += 2 * Math.pow(3, k);
        emptyBaseHashes[j] += Math.pow(3, k);
      }
    }
    for (int i = 0; i < 64; ++i) {
      squaresToFeatureSingle[i] = squaresToFeatureSingleList[i].toArray(squaresToFeatureSingle[i]);
      squaresToFeatureDouble[i] = squaresToFeatureDoubleList[i].toArray(squaresToFeatureDouble[i]);
    }
    for (int i = 0; i < featureToBaseFeature.length; ++i) {
      boolean copied = true;
      // Check if featureToBaseFeature[i][j] == featureToBaseFeature[i-1][j] rotated for all j.
      if (i > 0 && featureToBaseFeature[i].length == featureToBaseFeature[i-1].length) {
        for (int j = 0; j < featureToBaseFeature[i].length; ++j) {
          LongOpenHashSet allTransp = BitPattern.allTranspositions(features[featureToBaseFeature[i][j]]);
          if (!allTransp.contains(features[featureToBaseFeature[i-1][j]])) {
            copied = false;
            break;
          }
        }
      } else {
        copied = false;
      }
      for (int emptiesGroup = 0; emptiesGroup < EMPTIES_SPLITS; ++emptiesGroup) {
        if (copied) {
          evals[emptiesGroup][i] = evals[emptiesGroup][i-1];
          continue;
        }
        if (featureToBaseFeature[i].length == 1) {
          evals[emptiesGroup][i] = new int[maxBaseHashes[featureToBaseFeature[i][0]] + 1];
        } else {
          int size = 0;
          for (int j = 0; j < featureToBaseFeature[i].length - 1; ++j) {
            size += (maxBaseHashes[featureToBaseFeature[i][j]] + 1) *  (maxBaseHashes[featureToBaseFeature[i][j+1]] + 1);
          }
          evals[emptiesGroup][i] = new int[size];
        }
//        evals[emptiesGroup][evals[emptiesGroup].length - 1] = new int[65*64];
      }
    }
  }
  
  public void randomizeEvals() {
    for (int emptiesGroup = 0; emptiesGroup < EMPTIES_SPLITS; ++emptiesGroup) {
    
      for (int i = 0; i < evals.length; ++i) {
        int mod1 = (int) (Math.random() * 100) + 3;
        int mod2 = (int) (Math.random() * 100) + 3;
        for (int j = 0; j < evals[i].length; ++j) {
          evals[emptiesGroup][i][j] += emptiesGroup + i % mod1 - j % mod2; // (int) (Math.random() * 6400);
        }
      }
    }
  }
  
  public void reset() {
    reset(0);
  }

  public void reset(int eval) {
    for (int i = 0; i < evals.length; ++i) {
      for (int j = 0; j < evals[i].length; ++j) {
        Arrays.fill(evals[i][j], eval);
      }
    }
  }
  
  public void save() {
    save(DEPTH_ONE_EVALUATOR_FILEPATTERN);
  }
  public void save(String file) {
    try {
       FileOutputStream fileOut = new FileOutputStream(file);
       ObjectOutputStream out = new ObjectOutputStream(fileOut);
       out.writeObject(this);
       out.close();
       fileOut.close();
       System.out.println("Saved pattern evaluator.");
    } catch (IOException e) {
       e.printStackTrace();
    }
  }

  public static PatternEvaluatorImproved load() {
    return load(DEPTH_ONE_EVALUATOR_FILEPATTERN);
  }

  /**
   * Loads a PatternEvaluatorImproved.
   * @param filepath the file (obtained by calling save() on another multilinear regression).
   * @return The PatternEvaluatorImproved loaded, or an empty MultilinearRegression if errors occurred.
   */
  public static PatternEvaluatorImproved load(String filepath) {
    PatternEvaluatorImproved result;
    try (ObjectInputStream in = new ObjectInputStream(new FileInputStream(filepath))) {
      result = (PatternEvaluatorImproved) in.readObject();
    } catch (IOException | ClassNotFoundException | ClassCastException e) {
      System.out.println("Error when loading the PatternEvaluator:\n" + 
                         Arrays.toString(e.getStackTrace()));
      return new PatternEvaluatorImproved();
    }
    return result;
  }
  
//  public static void main(String args[]) {
//    PatternEvaluatorImproved eval = new PatternEvaluatorImproved();
//    long[] combinedFeatures = {FEATURE_LAST_ROW, FEATURE_LAST_BUT_ONE_ROW, FEATURE_LAST_BUT_TWO_ROW, FEATURE_LAST_BUT_THREE_ROW};
//    
//    for (int i = 0; i < 2; ++i) {
//      for (int j = 0; j < 4; ++j) {
//        System.out.println("\n\n\n");
//        for (int k = 0; k < combinedFeatures.length; ++k) {
//          System.out.println(eval.getPositions(combinedFeatures[k]));
//          combinedFeatures[k] = BitPattern.transpose(BitPattern.verticalMirror(combinedFeatures[k]));
//        }
//      }
//      for (int k = 0; k < combinedFeatures.length; ++k) {
//        combinedFeatures[k] = BitPattern.horizontalMirror(combinedFeatures[k]);
//      }
//    }
//    System.out.println(eval.getPositions(BitPattern.horizontalMirror(eval.FEATURE_DIAGONAL)));
//  }
  public static void main(String args[]) {
    PatternEvaluatorImproved eval = new PatternEvaluatorImproved();
    PossibleMovesFinderImproved pmf = new PossibleMovesFinderImproved();
    long t = 0;
    int num = 0;
    for (int i = 0; i < 200000; ++i) {
//      System.out.println(i);
      Board b = Board.randomBoard();
//      while (true) {
        long[] moves = pmf.possibleMovesAdvanced(b.getPlayer(), b.getOpponent());
        if (moves.length == 0) {
          continue;
        }
        long flip = moves[(int) (Math.random() * moves.length)];
        if (flip == 0) {
          continue;
        }
        int move = Long.numberOfTrailingZeros(flip & ~b.getPlayer() & ~b.getOpponent());
        num += 100;
        t -= System.currentTimeMillis();
          eval.setup(b);
          eval.invert();
        for (int g = 0; g < 100; ++g) {
          eval.update(move, flip);
          eval.eval();
          eval.undoUpdate(move, flip);
        }
        t += System.currentTimeMillis();
//      }
    }
    System.out.println(num + " " + t + " " + num * 1000L / t);
  }

  @Override
  public int evalVerbose(long player, long opponent) {
    this.setup(player, opponent);
    return this.eval();
  }
}
