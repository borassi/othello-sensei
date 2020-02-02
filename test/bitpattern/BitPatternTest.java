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
  public void testHorizontalMirror() {
    String patternString = "X-------\n"
                         + "-------X\n" 
                         + "XX------\n"
                         + "-XXXX---\n"
                         + "--------\n"
                         + "--------\n"
                         + "--------\n"
                         + "XXXXXXXX\n";
    String resultString = "-------X\n"
                        + "X-------\n" 
                        + "------XX\n"
                        + "---XXXX-\n"
                        + "--------\n"
                        + "--------\n"
                        + "--------\n"
                        + "XXXXXXXX\n";
    long pattern = BitPattern.parsePattern(patternString);
    String reversed = BitPattern.patternToString(BitPattern.horizontalMirror(pattern));
    
    assertEquals(reversed, resultString);
  }

  @Test
  public void randomHorizontalMirror() {
    for (int i = 0; i < 1000; i++) {
      String patternString = randomPatternString();
      long pattern = BitPattern.parsePattern(patternString);
      assertEquals(patternString, BitPattern.patternToString(
              BitPattern.horizontalMirror(BitPattern.horizontalMirror(pattern))));
    }
  }
  

  @Test
  public void randomVerticalMirror() {
    for (int i = 0; i < 1000; i++) {
      String patternString = randomPatternString();
      long pattern = BitPattern.parsePattern(patternString);
      assertEquals(patternString, BitPattern.patternToString(
              BitPattern.verticalMirror(BitPattern.verticalMirror(pattern))));
    }
  }
  
  @Test
  public void testTranspose() {
    String patternString = "X-------\n"
                         + "-------X\n" 
                         + "XX------\n"
                         + "-XXXX---\n"
                         + "--------\n"
                         + "--------\n"
                         + "--------\n"
                         + "XXXXXXXX\n";
    String resultString = "X-X----X\n"
                        + "--XX---X\n" 
                        + "---X---X\n"
                        + "---X---X\n"
                        + "---X---X\n"
                        + "-------X\n"
                        + "-------X\n"
                        + "-X-----X\n";
    long pattern = BitPattern.parsePattern(patternString);
    String reversed = BitPattern.patternToString(BitPattern.transpose(pattern));
    
    assertEquals(resultString, reversed);
  }

  @Test
  public void randomTranspose() {
    for (int i = 0; i < 1000; i++) {
      String patternString = randomPatternString();
      long pattern = BitPattern.parsePattern(patternString);
      assertEquals(patternString, BitPattern.patternToString(
              BitPattern.transpose(BitPattern.transpose(pattern))));
    }
  }
  
  @Test
  public void testVerticalMirror() {
    String patternString = "X-X----X\n"
                         + "--XX---X\n" 
                         + "---X---X\n"
                         + "---X---X\n"
                         + "---X---X\n"
                         + "-------X\n"
                         + "-------X\n"
                         + "-X-----X\n";
    String resultString = "-X-----X\n"
                        + "-------X\n" 
                        + "-------X\n"
                        + "---X---X\n"
                        + "---X---X\n"
                        + "---X---X\n"
                        + "--XX---X\n"
                        + "X-X----X\n";
    long pattern = BitPattern.parsePattern(patternString);
    String reversed = BitPattern.patternToString(BitPattern.verticalMirror(pattern));
    
    assertEquals(reversed, resultString);
  }
  
  @Test
  public void testAllRotations() {
    String patternString = randomPatternString();
    long pattern = BitPattern.parsePattern(patternString);
    HashSet<Long> transpositions = BitPattern.allTranspositions(pattern);
    assertEquals(transpositions.size(), 8);
    
    for (int i = 0; i < 30; ++i) {
      if (Math.random() < 0.5) {
        pattern = BitPattern.verticalMirror(pattern);
      }
      if (Math.random() < 0.5) {
        pattern = BitPattern.horizontalMirror(pattern);
      }
      if (Math.random() < 0.5) {
        pattern = BitPattern.transpose(pattern);
      }
      if (!transpositions.contains(pattern)) {
        System.out.println(BitPattern.patternToString(pattern));
        for (long transposition : transpositions) {
          System.out.println(BitPattern.patternToString(transposition));
        }
        assert(false);
      }
    }
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
  
  @Test
  public void interestingBitPatterns() {
    long bitPatterns[] = BitPattern.interestingBitPatterns();
    
    assert(contains(bitPatterns, FIRST_ROW));
    assert(contains(bitPatterns, ANY_ROW));
    assert(contains(bitPatterns, LAST_ROW));
    assert(contains(bitPatterns, FIRST_COLUMN));
    assert(contains(bitPatterns, ANY_COLUMN));
    assert(contains(bitPatterns, LAST_COLUMN));
    assert(contains(bitPatterns, FIRST_DIAG));
    assert(contains(bitPatterns, MAIN_DIAG));
    assert(contains(bitPatterns, LAST_DIAG));
    assert(contains(bitPatterns, BitPattern.horizontalMirror(FIRST_DIAG)));
    assert(contains(bitPatterns, BitPattern.horizontalMirror(MAIN_DIAG)));
    assert(contains(bitPatterns, BitPattern.horizontalMirror(LAST_DIAG)));
    assert(contains(bitPatterns, BitPattern.horizontalMirror(LAST_DIAG)));

    assert(!contains(bitPatterns, 0));
    assertEquals(8 /*rows*/ + 8 /*rows*/ + 11 /*diag1*/ + 11 /*diag2*/, bitPatterns.length);
  }
  

  @Test
  public void numberExistingSubBitPatternsTest() {
    long bitPatterns[] = BitPattern.interestingBitPatterns();
    
    for (long bitPattern : bitPatterns) {
      ArrayList<Long> subBitPatterns = BitPattern.allSubBitPatterns(bitPattern);
      assertEquals((int) Math.pow(2., Long.bitCount(bitPattern)), subBitPatterns.size());
    }
  }

  @Test
  public void existingBoardsInBitPatternTest() {
    long bitPatterns[] = BitPattern.interestingBitPatterns();
    
    for (long bitPattern : bitPatterns) {
      ArrayList<Long> subBitPatterns = BitPattern.allSubBitPatterns(bitPattern);
      for (int i = 0; i < 1000; i++) {
        long pattern = BitPattern.parsePattern(randomPatternString()) & bitPattern;
        assert(subBitPatterns.contains(pattern));
      }
    }
  }
  
  @Test
  public void testAnyDiagonalToFirstRow() {
    long bitPatterns[] = BitPattern.interestingBitPatterns();
    
    for (int i = 16; i < 38; i++) {
      long bitPattern = bitPatterns[i];
      HashSet<Long> hashes = new HashSet<>();
      ArrayList<Long> subBitPatterns = BitPattern.allSubBitPatterns(bitPattern);
      for (long subBitPattern : subBitPatterns) {
        long firstRow = BitPattern.anyDiagonalToFirstRow(subBitPattern);
        assertEquals(0, firstRow & ~BitPattern.LAST_ROW_BIT_PATTERN);
        assert(!hashes.contains(firstRow));
        hashes.add(firstRow);
      }
    }
  }

  @Test
  public void testAnyColumnToFirstRow() {
    long bitPatterns[] = BitPattern.interestingBitPatterns();
    
    for (int i = 8; i < 16; i++) {
      long bitPattern = bitPatterns[i];
      HashSet<Long> hashes = new HashSet<>();
      ArrayList<Long> subBitPatterns = BitPattern.allSubBitPatterns(bitPattern);
      for (long subBitPattern : subBitPatterns) {
        long firstRow = BitPattern.anyColumnToFirstRow(subBitPattern, 15-i);
        assertEquals(0, firstRow & ~BitPattern.LAST_ROW_BIT_PATTERN);
        assert(!hashes.contains(firstRow));
        hashes.add(firstRow);
      }
    }
  }

  @Test
  public void testAnyRowToFirstRow() {
    long bitPatterns[] = BitPattern.interestingBitPatterns();
    
    for (int i = 0; i < 8; i++) {
      long bitPattern = bitPatterns[i];
      HashSet<Long> hashes = new HashSet<>();
      ArrayList<Long> subBitPatterns = BitPattern.allSubBitPatterns(bitPattern);
      for (long subBitPattern : subBitPatterns) {
        long firstRow = BitPattern.anyRowToFirstRow(subBitPattern, 7-i);
        assertEquals(0, firstRow & ~BitPattern.LAST_ROW_BIT_PATTERN);
        assert(!hashes.contains(firstRow));
        hashes.add(firstRow);
      }
    }
  }
  
  @Test
  public void testGetRow() {
    assertEquals(LAST_ROW, BitPattern.getRow(5));
    assertEquals(FIRST_ROW, BitPattern.getRow(62));
    assertEquals(ANY_ROW, BitPattern.getRow(47));
    assertEquals(ANY_ROW, BitPattern.getRow(40));
    assertEquals(FIRST_ROW, BitPattern.getRow(63));
    assertEquals(LAST_ROW, BitPattern.getRow(0));
  }
  @Test
  public void testGetColumn() {
    assertEquals(ANY_COLUMN, BitPattern.getColumn(4));
    assertEquals(FIRST_COLUMN, BitPattern.getColumn(55));
    assertEquals(LAST_COLUMN, BitPattern.getColumn(48));
    assertEquals(FIRST_COLUMN, BitPattern.getColumn(63));
    assertEquals(LAST_COLUMN, BitPattern.getColumn(0));
  }
  @Test
  public void testGetNWSEDiag() {
    assertEquals(FIRST_DIAG, BitPattern.getNWSEDiag(5));
    assertEquals(FIRST_DIAG, BitPattern.getNWSEDiag(14));
    assertEquals(FIRST_DIAG, BitPattern.getNWSEDiag(23));
    assertEquals(MAIN_DIAG, BitPattern.getNWSEDiag(0));
    assertEquals(MAIN_DIAG, BitPattern.getNWSEDiag(27));
    assertEquals(MAIN_DIAG, BitPattern.getNWSEDiag(63));
    assertEquals(LAST_DIAG, BitPattern.getNWSEDiag(40));
    assertEquals(1L << 7, BitPattern.getNWSEDiag(7));
    assertEquals(1L << 56, BitPattern.getNWSEDiag(56));
  }
  @Test
  public void testGetNESWDiag() {
    assertEquals(LAST_REV_DIAG, BitPattern.getNESWDiag(2));
    assertEquals(LAST_REV_DIAG, BitPattern.getNESWDiag(9));
    assertEquals(LAST_REV_DIAG, BitPattern.getNESWDiag(16));
    assertEquals(FIRST_REV_DIAG, BitPattern.getNESWDiag(61));
    assertEquals(FIRST_REV_DIAG, BitPattern.getNESWDiag(54));
    assertEquals(FIRST_REV_DIAG, BitPattern.getNESWDiag(47));
    assertEquals(MAIN_REV_DIAG, BitPattern.getNESWDiag(7));
    assertEquals(MAIN_REV_DIAG, BitPattern.getNESWDiag(56));
    assertEquals(1L, BitPattern.getNESWDiag(0));
    assertEquals(1L << 63, BitPattern.getNESWDiag(63));
  }
  
}
