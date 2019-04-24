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

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package evaluatedepthone.patternhasher;

import bitpattern.BitPattern;
import board.Board;

/**
 *
 * @author michele
 */
public class Parity implements PatternHashInterface {
  final static long ALL_BUT_SQUARE_CORNER = BitPattern.parsePattern(
          "XXXXXXXX\n" +
          "XXXXXXXX\n" +
          "XXXXXXXX\n" +
          "XXXXXXXX\n" +
          "XXXX----\n" +
          "XXXX----\n" +
          "XXXX----\n" +
          "XXXX----\n");
  
  /**
   * Needed to silence a warning.
   */
  private static final long serialVersionUID = 1L;
  
  private static final int hashes[] = new int[1];
  
  protected final static int hash(long player, long opponent) {
    long filteredPlayer = player | ALL_BUT_SQUARE_CORNER;
    long filteredOpponent = opponent | ALL_BUT_SQUARE_CORNER;
    long empties = ~(filteredPlayer | filteredOpponent);
    
    int numEmpties = Long.bitCount(empties);
    int result = numEmpties > 6 ? 7 : numEmpties;
    
    long emptiesShiftOne = empties << 1;
    long emptiesShiftEight = empties << 8;
    long emptiesShiftNine = empties << 9;

    if ((emptiesShiftOne & filteredOpponent) == 0 &&
        (emptiesShiftEight & filteredOpponent) == 0 &&
        (emptiesShiftNine & filteredOpponent) == 0) {
      result += 8;
    }
    if ((emptiesShiftOne & filteredPlayer) == 0 &&
        (emptiesShiftEight & filteredPlayer) == 0 &&
        (emptiesShiftNine & filteredPlayer) == 0) {
      result += 16;
    }
    
    return result;
  }

  public final static int hash(Board r0, Board r1, Board r2, Board r3) {
    int result0 = hash(r0.getPlayer(), r0.getOpponent());
    int result1 = hash(r1.getPlayer(), r1.getOpponent());
    int result2 = hash(r2.getPlayer(), r2.getOpponent());
    int result3 = hash(r3.getPlayer(), r3.getOpponent());
    int tmp;

    // We sort the results, using as few comparisons as possible.
    if (result1 > result2) {
      tmp = result1;
      result1 = result2;
      result2 = tmp;
    }    
    if (result3 > result0) {
      tmp = result3;
      result3 = result0;
      result0 = tmp;
    }
    if (result1 <= result3) {
      // result1 is the minimum.
      if (result2 <= result3) {
        return combineHashes(result1, result2, result3, result0);
      } else if (result2 <= result0) {
        return combineHashes(result1, result3, result2, result0);        
      } else {
        return combineHashes(result1, result3, result0, result2);            
      }
    } else {
      // result3 is the minimum.
      if (result0 <= result1) {
        return combineHashes(result3, result0, result1, result2);
      } else if (result2 <= result0) {
        return combineHashes(result3, result1, result2, result0);        
      } else {
        return combineHashes(result3, result1, result0, result2);            
      }
    }
  }

  public final static float eval(Board r1, Board r2, Board r3, Board r4, float[] evaluators) {
    return evaluators[hash(r1, r2, r3, r4)];
  }
  
  @Override
  public int[] hashes(Board[] transp) {
    hashes[0] = hash(transp[0], transp[1], transp[2], transp[3]);
    return hashes;
  }
  
  @Override
  public int maxSize() {
    return 1048576; // 2^20
  }
  
  public static final int combineHashes(int hash1, int hash2, int hash3, int hash4) {
    return hash1 + 32 * hash2 + 1024 * hash3 + 32768 * hash4;
  }
}
