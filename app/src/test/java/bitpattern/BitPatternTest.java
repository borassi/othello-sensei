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

package bitpattern;
import java.util.ArrayList;
import java.util.HashSet;

import org.junit.Test;
import static org.junit.Assert.assertEquals;

public class BitPatternTest {
  final static long FIRST_ROW = BitPattern.parsePattern(
            "XXXXXXXX\n"
          + "--------\n" 
          + "--------\n"
          + "--------\n"
          + "--------\n"
          + "--------\n"
          + "--------\n"
          + "--------\n");

  final static long ANY_ROW = BitPattern.parsePattern(
            "--------\n"
          + "--------\n" 
          + "XXXXXXXX\n"
          + "--------\n"
          + "--------\n"
          + "--------\n"
          + "--------\n"
          + "--------\n");

  final static long LAST_ROW = BitPattern.parsePattern(
            "--------\n"
          + "--------\n" 
          + "--------\n"
          + "--------\n"
          + "--------\n"
          + "--------\n"
          + "--------\n"
          + "XXXXXXXX\n");

  final static long FIRST_COLUMN = BitPattern.parsePattern(
            "X-------\n"
          + "X-------\n" 
          + "X-------\n"
          + "X-------\n"
          + "X-------\n"
          + "X-------\n"
          + "X-------\n"
          + "X-------\n");

  final static long ANY_COLUMN = BitPattern.parsePattern(
            "---X----\n"
          + "---X----\n" 
          + "---X----\n"
          + "---X----\n"
          + "---X----\n"
          + "---X----\n"
          + "---X----\n"
          + "---X----\n");

  final static long LAST_COLUMN = BitPattern.parsePattern(
            "-------X\n"
          + "-------X\n" 
          + "-------X\n"
          + "-------X\n"
          + "-------X\n"
          + "-------X\n"
          + "-------X\n"
          + "-------X\n");

  final static long FIRST_DIAG = BitPattern.parsePattern(
            "--------\n"
          + "--------\n" 
          + "--------\n"
          + "--------\n"
          + "--------\n"
          + "X-------\n"
          + "-X------\n"
          + "--X-----\n");

  final static long MAIN_DIAG = BitPattern.parsePattern(
            "X-------\n"
          + "-X------\n" 
          + "--X-----\n"
          + "---X----\n"
          + "----X---\n"
          + "-----X--\n"
          + "------X-\n"
          + "-------X\n");

  final static long LAST_DIAG = BitPattern.parsePattern(
            "-----X--\n"
          + "------X-\n" 
          + "-------X\n"
          + "--------\n"
          + "--------\n"
          + "--------\n"
          + "--------\n"
          + "--------\n");

  final static long FIRST_REV_DIAG = BitPattern.parsePattern(
            "--X-----\n"
          + "-X------\n" 
          + "X-------\n"
          + "--------\n"
          + "--------\n"
          + "--------\n"
          + "--------\n"
          + "--------\n");

  final static long LAST_REV_DIAG = BitPattern.parsePattern(
            "--------\n"
          + "--------\n" 
          + "--------\n"
          + "--------\n"
          + "--------\n"
          + "-------X\n"
          + "------X-\n"
          + "-----X--\n");

  final static long MAIN_REV_DIAG = BitPattern.parsePattern(
            "-------X\n"
          + "------X-\n"
          + "-----X--\n"
          + "----X---\n"
          + "---X----\n" 
          + "--X-----\n"
          + "-X------\n"
          + "X-------\n");

  public static boolean contains(long[] list, long element) {
    for (long listElement : list) {
      if (listElement == element) {
        return true;
      }
    }
    return false;
  }
  
  public String randomPatternString() {
    String patternString = "";
    for (int j = 0; j < 64; j++) {
      int rand = (int) (Math.random() * 2);
      if (rand == 0) {
        patternString += "-";
      } else {
        patternString += "X";
      }
      if (j % 8 == 7) {
        patternString += "\n";
      }
    }
    return patternString;
  }
  
  @Test
  public void testMoveToBit() {
    String patternString = "--X-----\n"
                         + "--------\n" 
                         + "--------\n"
                         + "--------\n"
                         + "--------\n"
                         + "--------\n"
                         + "--------\n"
                         + "--------\n";
    long pattern = BitPattern.parsePattern(patternString);
    
    assertEquals(pattern, BitPattern.moveToBit(0, 2));
  }
  
  @Test
  public void testBitToMove() {
    String patternString = "--X-----\n"
                         + "--------\n" 
                         + "--------\n"
                         + "--------\n"
                         + "--------\n"
                         + "--------\n"
                         + "--------\n"
                         + "--------\n";
    long pattern = BitPattern.parsePattern(patternString);

    assertEquals(BitPattern.bitToMove(pattern), new PositionIJ(0, 2));
  }
  
  @Test
  public void testBitToMoveAndViceversa() {
    for (int t = 0; t < 1000; t++) {
      int i = (int) (Math.random() * 8);
      int j = (int) (Math.random() * 8);
      PositionIJ move = BitPattern.bitToMove(BitPattern.moveToBit(i, j));
      assertEquals(i, move.i);
      assertEquals(j, move.j);      
    }
  }
}
