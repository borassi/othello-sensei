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
import bitpattern.BitPattern;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.logging.Level;
import java.util.logging.Logger;

import helpers.FileAccessor;

/**
 *
 * @author michele
 */
public class PatternEvaluatorImproved implements DepthOneEvaluator {
  public final static String DEPTH_ONE_EVALUATOR_FILEPATTERN = "coefficients/pattern_evaluator_coefficients.dat";
  final static int EMPTIES_SPLITS = 20; // SET TO <= 2 FOR TESTS
  final static long FEATURE_CORNER_4X4 = BitPattern.parsePattern("--------" +
                                                                 "--------" +
                                                                 "--------" +
                                                                 "--------" +
                                                                 "------XX" +
                                                                 "-----XXX" +
                                                                 "----XXXX" +
                                                                 "----XXXX");
  final static long FEATURE_CORNER_SIDED = BitPattern.parsePattern("--------" +
                                                                 "--------" +
                                                                 "--------" +
                                                                 "-------X" +
                                                                 "-------X" +
                                                                 "-------X" +
                                                                 "------XX" +
                                                                 "---XXXXX");
  final static long FEATURE_IMPR_DIAGONAL = BitPattern.parsePattern("XX------" +
                                                                    "XX------" +
                                                                    "--X-----" +
                                                                    "---X----" +
                                                                    "----X---" +
                                                                    "-----X--" +
                                                                    "------XX" +
                                                                    "------XX");
  final static long FEATURE_DIAGONAL = BitPattern.parsePattern("X-------" +
                                                               "-X------" +
                                                               "--X-----" +
                                                               "---X----" +
                                                               "----X---" +
                                                               "-----X--" +
                                                               "------X-" +
                                                               "-------X");

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
                                                          + "--XXXX--\n"
                                                          + "--------\n"
                                                          + "--------\n");

  final static long PATTERN_LAST4 = BitPattern.parsePattern("--------\n" 
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "--XXXX--\n"
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "--------\n");

  final static long PATTERN_CORNER_5X2 = BitPattern.parsePattern("--------\n" 
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "--------\n"
                                                          + "---XXXXX\n"
                                                          + "---XXXXX\n");
  
  final static long[][] FEATURE_GROUPS = {
    {PATTERN_CORNER, PATTERN_SM_LAST1, PATTERN_SM_LAST2, PATTERN_LAST3, PATTERN_LAST4},
    {FEATURE_IMPR_DIAGONAL},
    {FEATURE_CORNER_4X4},
    {PATTERN_LAST1 << 16}, 
    {PATTERN_LAST1 << 24},
    {FEATURE_DIAGONAL << 8},
    {FEATURE_DIAGONAL << 16},
  };

  final static int squaresToFeatureSingle[][] = new int[64][];
  final static int squaresToFeatureDouble[][] = new int[64][];
  final static int[] maxBaseHashes;
  final static int[] emptyBaseHashes;

  final static int[][][] EVALS;
  final static long features[];
  final static ArrayList<int[]> featuresToSquaresList;

  int[] hashes;
  int[] baseHashes;
  int empties;

  final static ArrayList<int[]> featureToBaseFeature() {
    featuresToSquaresList.clear();
    ArrayList<int[]> result = new ArrayList<>();
    for (long[] featureGroup : FEATURE_GROUPS) {
      ArrayList<ArrayList<int[]>> baseFeatures = new ArrayList<>();
      for (long featureLong : featureGroup) {
        baseFeatures.add(allFeatures(featureLong));
      }
      for (int i = 0; i < baseFeatures.get(0).size(); ++i) {
        int[] baseFeature = new int[baseFeatures.size()];
        int nBaseFeature = 0;
        for (int j = 0; j < baseFeatures.size(); ++j) {
          assert(baseFeatures.get(0).size() == baseFeatures.get(j).size());
          featuresToSquaresList.add(baseFeatures.get(j).get(i));
          baseFeature[nBaseFeature++] = featuresToSquaresList.size() - 1;
        }
        result.add(baseFeature);
      }
    }
    return result;
  }
  
  static {
    featuresToSquaresList = new ArrayList<>();
    featureToBaseFeature();
    ArrayList<ArrayList<Integer>> squaresToFeatureSingleList = new ArrayList<>();
    ArrayList<ArrayList<Integer>> squaresToFeatureDoubleList = new ArrayList<>();

    assert(featuresToSquaresList.size() < 127);
    features = new long[featuresToSquaresList.size()];
    maxBaseHashes = new int[featuresToSquaresList.size()];
    emptyBaseHashes = new int[featuresToSquaresList.size()];

    for (int i = 0; i < 64; ++i) {
      squaresToFeatureSingleList.add(new ArrayList<>());
      squaresToFeatureDoubleList.add(new ArrayList<>());
    }
    for (int j = 0; j < featuresToSquaresList.size(); ++j) {
      int squareList[] = featuresToSquaresList.get(j);
      features[j] = featureToLong(squareList);
      for (int k = 0; k < squareList.length; ++k) {
        squaresToFeatureSingleList.get(squareList[k]).add((int) Math.pow(3, k) * 128 + j);
        squaresToFeatureDoubleList.get(squareList[k]).add((int) (2 * Math.pow(3, k)) * 128 + j);
        maxBaseHashes[j] += 2 * Math.pow(3, k);
        emptyBaseHashes[j] += Math.pow(3, k);
      }
    }
    for (int i = 0; i < 64; ++i) {
      squaresToFeatureSingle[i] = new int[squaresToFeatureSingleList.get(i).size()];
      squaresToFeatureDouble[i] = new int[squaresToFeatureDoubleList.get(i).size()];
      for (int j = 0; j < squaresToFeatureSingleList.get(i).size(); ++j) {
        squaresToFeatureSingle[i][j] = squaresToFeatureSingleList.get(i).get(j);
        squaresToFeatureDouble[i][j] = squaresToFeatureDoubleList.get(i).get(j);
        
      }
    }
    EVALS = readEvals();
  }

  final static int[] rotateFeature(int[] feature) {
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

  final static ArrayList<int[]> allFeatures(long pattern) {
    HashSet<Long> seen = new HashSet<>();
    int[] feature = longToFeature(pattern);
    ArrayList<int[]> result = new ArrayList<>();

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

  public PatternEvaluatorImproved() {
    baseHashes = new int[featuresToSquaresList.size()];
    hashes = new int[EVALS[0].length];
  }
  
  public static int[][][] emptyEvals() {
    ArrayList<int[]> featureToBaseFeature = featureToBaseFeature();
    int[][][] result = new int[EMPTIES_SPLITS][featureToBaseFeature.size()][];
    for (int i = 0; i < featureToBaseFeature.size(); ++i) {
      boolean copied = true;
      // Check if featureToBaseFeature[i][j] == featureToBaseFeature[i-1][j] rotated for all j.
      if (i > 0 && featureToBaseFeature.get(i).length == featureToBaseFeature.get(i-1).length) {
        for (int j = 0; j < featureToBaseFeature.get(i).length; ++j) {
          HashSet<Long> allTransp = BitPattern.allTranspositions(features[featureToBaseFeature.get(i)[j]]);
          if (!allTransp.contains(features[featureToBaseFeature.get(i-1)[j]])) {
            copied = false;
            break;
          }
        }
      } else {
        copied = false;
      }
      for (int emptiesGroup = 0; emptiesGroup < EMPTIES_SPLITS; ++emptiesGroup) {
        if (copied) {
          result[emptiesGroup][i] = result[emptiesGroup][i-1];
          continue;
        }
        if (featureToBaseFeature.get(i).length == 1) {
          result[emptiesGroup][i] = new int[maxBaseHashes[featureToBaseFeature.get(i)[0]] + 1];
        } else {
          int size = 0;
          for (int j = 0; j < featureToBaseFeature.get(i).length - 1; ++j) {
            size += (maxBaseHashes[featureToBaseFeature.get(i)[j]] + 1) * (maxBaseHashes[featureToBaseFeature.get(i)[j+1]] + 1);
          }
          result[emptiesGroup][i] = new int[size];
        }
      }
    }
    return result;
  }
  
  public static void saveEvals() {
    saveEvals(DEPTH_ONE_EVALUATOR_FILEPATTERN);
  }

  public static void saveEvals(String filepath) {
    try {
      ObjectOutputStream objOut = FileAccessor.objectOutputStream(filepath);
      objOut.writeObject(EVALS);
      objOut.close();
    } catch (IOException ex) {
      Logger.getLogger(MultilinearRegressionImproved.class.getName()).log(Level.SEVERE, null, ex);
    }
  }
  
  public static int[][][] readEvals() {
    return readEvals(DEPTH_ONE_EVALUATOR_FILEPATTERN);
  }

  public static int[][][] readEvals(String filepath) {
    try {
      ObjectInputStream in = FileAccessor.objectInputStream(filepath);
      int[][][] result = (int[][][]) in.readObject();
      in.close();
      return result;
    } catch (IOException | ClassNotFoundException ex) {
      System.out.println("Missing pattern evaluator!");
      Logger.getLogger(PatternEvaluatorImproved.class.getName()).log(Level.SEVERE, null, ex);
      return emptyEvals();
    }
  }
  
  final static int[] longToFeature(long pattern) {
    int[] feature = new int[Long.bitCount(pattern)];
    int currentBit = 0;
    int square;
    while (true) {
      square = Long.numberOfTrailingZeros(pattern);
      feature[currentBit++] = square;
      pattern = pattern & ~(1L << square);
      if (pattern == 0) {
        break;
      }
    }
    return feature;
  }

  final static long featureToLong(int[] feature) {
    long result = 0;
    for (int i : feature) {
      result |= 1L << i;
    }
    return result;
  }
  
  final static int[] horizMirrorFeature(int[] feature) {
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
  
  public int[] hashes() {
    ArrayList<int[]> featureToBaseFeature = featureToBaseFeature();
    for (int i = 0; i < hashes.length; ++i) {
      hashes[i] = 0;
      int j = 0;
      while (baseHashes[featureToBaseFeature.get(i)[j]] == emptyBaseHashes[featureToBaseFeature.get(i)[j]] &&
             j < featureToBaseFeature.get(i).length - 2) {
        hashes[i] += (maxBaseHashes[featureToBaseFeature.get(i)[j]] + 1) * (maxBaseHashes[featureToBaseFeature.get(i)[j+1]] + 1);
        ++j;
      }
      if (j == featureToBaseFeature.get(i).length - 1) {
        hashes[i] = baseHashes[featureToBaseFeature.get(i)[j]];
      } else {
        hashes[i] += baseHashes[featureToBaseFeature.get(i)[j]] +
            (maxBaseHashes[featureToBaseFeature.get(i)[j]] + 1) * baseHashes[featureToBaseFeature.get(i)[j+1]];
      }
    }
//    hashes[featureToBaseFeature.length] = StableDisksMiddle.hash(new Board(player, opponent));
    return hashes;
  }

  @Override
  public int eval() {
//    return this.evalSlow();
    int[][] curEvals = EVALS[getEvalFromEmpties(empties)];
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
    eval += curEvals[4][baseHashes[20]] + curEvals[4][baseHashes[21]] + // Diagonals
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
    return eval;//getEvalAndSetupLastError(eval);
  }
  
//  public int getEvalAndSetupLastError(long eval) {
//    long evalValue = eval >> 32;
//    this.lastError = (float) Math.sqrt(eval - (evalValue << 32));
//    return (int) evalValue;
//  }
//  
//  @Override
//  public float lastError() {
//    return this.lastError;
//  }
  
  public final static int getEvalFromEmpties(int empties) {
    return Math.min((empties - 1) * EMPTIES_SPLITS / 60, EMPTIES_SPLITS - 1);
  }

  // WARNING: RETURNS EVAL * 2^15 + ERROR
  public int evalSlow() {
    int eval = 0;
    int[] curHashes = hashes();
    int[][] curEval = EVALS[getEvalFromEmpties(empties)];
    for (int i = 0; i < curEval.length; ++i) {
      eval += curEval[i][curHashes[i]];
    }
    return eval;//getEvalAndSetupLastError(eval);
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
  
  @Override
  public void invert() {
    for (int i = 0; i < baseHashes.length; ++i) {
      baseHashes[i] = maxBaseHashes[i] - baseHashes[i];
    }
//    long tmp = player;
//    player = opponent;
//    opponent = tmp;
  }
  
  @Override
  public void undoUpdate(int square, long flip) {
    flip = flip & ~(1L << square);
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
  
  @Override
  public void update(int square, long flip) {
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
  
  public void randomizeEvals() {
    for (int emptiesGroup = 0; emptiesGroup < EVALS.length; ++emptiesGroup) {
      for (int i = 0; i < EVALS[emptiesGroup].length; ++i) {
        int mod1 = (int) (Math.random() * 100) + 3;
        int mod2 = (int) (Math.random() * 100) + 3;
        for (int j = 0; j < EVALS[emptiesGroup][i].length; ++j) {
          EVALS[emptiesGroup][i][j] += emptiesGroup + i % mod1 - j % mod2; // (int) (Math.random() * 6400);
        }
      }
    }
  }
  
  public void reset() {
    reset(0);
  }

  public void reset(int eval) {
    for (int i = 0; i < EVALS.length; ++i) {
      for (int j = 0; j < EVALS[i].length; ++j) {
        Arrays.fill(EVALS[i][j], eval);
      }
    }
  }
  
  public void save() {
    save(DEPTH_ONE_EVALUATOR_FILEPATTERN);
  }
  public void save(String filepath) {
    try {
      ObjectOutputStream out = FileAccessor.objectOutputStream(filepath);
      out.writeObject(this);
      out.close();
      System.out.println("Saved pattern evaluator.");
    } catch (IOException e) {
      e.printStackTrace();
    }
  }

//  public static PatternEvaluatorImproved load() {
//    return load(DEPTH_ONE_EVALUATOR_FILEPATTERN);
//  }

//  /**
//   * Loads a PatternEvaluatorImproved.
//   * @param filepath the file (obtained by calling save() on another multilinear regression).
//   * @return The PatternEvaluatorImproved loaded, or an empty MultilinearRegression if errors occurred.
//   */
//  public static PatternEvaluatorImproved load(String filepath) {
//    PatternEvaluatorImproved result;
//    try (ObjectInputStream in = Main.fileAccessor.inputFile(filepath)) {
//      result = (PatternEvaluatorImproved) in.readObject();
//    } catch (IOException | ClassNotFoundException | ClassCastException e) {
//      System.out.println("Error when loading the PatternEvaluator:\n" + 
//                         Arrays.toString(e.getStackTrace()));
//      return new PatternEvaluatorImproved();
//    }
//    return result;
//  }

//  public static void main(String args[]) {
//    PatternEvaluatorImproved eval = new PatternEvaluatorImproved();
//    PossibleMovesFinderImproved pmf = new PossibleMovesFinderImproved();
//    long t = 0;
//    int num = 0;
//    for (int i = 0; i < 200000; ++i) {
//      Board b = Board.randomBoard();
////      while (true) {
//        long[] moves = pmf.possibleMovesAdvanced(b.getPlayer(), b.getOpponent());
//        if (moves.length == 0) {
//          continue;
//        }
//        long flip = moves[(int) (Math.random() * moves.length)];
//        if (flip == 0) {
//          continue;
//        }
//        int move = Long.numberOfTrailingZeros(flip & ~b.getPlayer() & ~b.getOpponent());
//        num += 100;
//        t -= System.currentTimeMillis();
//          eval.setup(b);
//          eval.invert();
//        for (int g = 0; g < 100; ++g) {
//          eval.update(move, flip);
//          eval.eval();
//          eval.undoUpdate(move, flip);
//        }
//        t += System.currentTimeMillis();
////      }
//    }
//    System.out.println(num + " " + t + " " + num * 1000L / t);
//  }

  @Override
  public int evalVerbose(long player, long opponent) {
    this.setup(player, opponent);
    return this.eval();
  }
}
