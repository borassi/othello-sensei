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

package evaluatedepthone.patternhasher;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;

import org.junit.Test;

import bitpattern.BitPattern;
import board.Board;

public class PatternHashTest {


  final static long PATTERN_ALL = BitPattern.parsePattern(
            "XXXXXXXX\n" 
          + "XXXXXXXX\n"
          + "XXXXXXXX\n"
          + "XXXXXXXX\n"
          + "XXXXXXXX\n"
          + "XXXXXXXX\n"
          + "XXXXXXXX\n"
          + "XXXXXXXX\n");
  
  @Test
  public void testCorner5x2() {
    ArrayList<Long> patterns = BitPattern.allSubBitPatterns(Corner5x2.pattern);
    HashSet<Integer> foundPatterns = new HashSet<>();
    Corner5x2 hasher = new Corner5x2();
    
    patterns.stream().forEach((player) -> {
      for (long opponent : patterns) {
        if ((player & opponent) == 0) {
          player = player | (((long) (Math.random() * Long.MAX_VALUE))
            & ~Corner5x2.pattern);
          opponent = opponent | (((long) (Math.random() * Long.MAX_VALUE))
            & ~Corner5x2.pattern);
          int hash = Corner5x2.hash(player, opponent);
          assert(hash >= 0);
          assert(hash < hasher.maxSize());
          assert(!foundPatterns.contains(hash));
          foundPatterns.add(hash);
        }
      }
    });
  }
  @Test
  public void testCorner3x3() {
    ArrayList<Long> patterns = BitPattern.allSubBitPatterns(Corner3x3.pattern);
    HashSet<Integer> foundPatterns = new HashSet<Integer>();
    Corner3x3 hasher = new Corner3x3();
    
    for (long player : patterns) {
      for (long opponent : patterns) {
        if ((player & opponent) == 0) {
          player = player | (((long) (Math.random() * Long.MAX_VALUE))
                  & ~Corner3x3.pattern);
          opponent = opponent | (((long) (Math.random() * Long.MAX_VALUE))
                  & ~Corner3x3.pattern);
          int hash = Corner3x3.hash(player, opponent);
          assert(hash < hasher.maxSize());
          assert(!foundPatterns.contains(hash));
          foundPatterns.add(hash);
        }
      }
    }
  }
  @Test
  public void testDiagonal() {
    ArrayList<Long> patterns = BitPattern.allSubBitPatterns(Diagonal.pattern);
    HashSet<Integer> foundPatterns = new HashSet<Integer>();
    Diagonal hasher = new Diagonal();
    
    for (long player : patterns) {
      for (long opponent : patterns) {
        if ((player & opponent) == 0) {
          player = player | (((long) (Math.random() * Long.MAX_VALUE))
                  & ~Diagonal.pattern);
          opponent = opponent | (((long) (Math.random() * Long.MAX_VALUE))
                  & ~Diagonal.pattern);
          int hash = Diagonal.hash(player, opponent);
          assert(hash < hasher.maxSize());
          assert(!foundPatterns.contains(hash));
          foundPatterns.add(hash);
        }
      }
    }
  }
  @Test
  public void testSide() {
    ArrayList<Long> patternsSweet = BitPattern.allSubBitPatterns(Side.patternSweet);
    ArrayList<Long> patternCenter = BitPattern.allSubBitPatterns(Side.patternCenter);
    ArrayList<Long> patternEdge = BitPattern.allSubBitPatterns(Side.patternEdge);
    ArrayList<Long> patternSide = BitPattern.allSubBitPatterns(Side.patternSide);
 
    long bitPatterns[] = {Side.patternSweet, Side.patternCenter, Side.patternEdge, Side.patternSide};
    ArrayList<ArrayList<Long>> allPatterns = new ArrayList<>();
 
    long[] masks = new long[] {PATTERN_ALL, Side.maskSweet, Side.maskCenter, Side.maskEdge};
    allPatterns.add(patternsSweet);
    allPatterns.add(patternCenter);
    allPatterns.add(patternEdge);
    allPatterns.add(patternSide);
    HashSet<Integer> foundPatterns = new HashSet<>();
    Side hasher = new Side();
    
    for (int i = 0; i < allPatterns.size(); i++) {
      ArrayList<Long> patterns = allPatterns.get(i);
      for (long player : patterns) {
        for (long opponent : patterns) {
          if ((player & opponent) == 0 && ((player | opponent) & masks[i]) != 0) {
            int hash = Side.hash(player, opponent);
            assert(hash < hasher.maxSize());
            assert(!foundPatterns.contains(hash));
            foundPatterns.add(hash);
          }
        }
      }
    }
  }
  @Test
  public void testSideImproved() {    
    ArrayList<Long> patternsSweet = BitPattern.allSubBitPatterns(SideImproved.patternSweet);
    ArrayList<Long> patternCenter = BitPattern.allSubBitPatterns(SideImproved.patternCenter);
    ArrayList<Long> patternEdge = BitPattern.allSubBitPatterns(SideImproved.patternEdge);
    ArrayList<Long> patternSide = BitPattern.allSubBitPatterns(SideImproved.patternSide);
 
    long bitPatterns[] = {SideImproved.patternSweet, SideImproved.patternCenter, SideImproved.patternEdge, SideImproved.patternSide};
    ArrayList<ArrayList<Long>> allHalfPatterns = new ArrayList<>();
 
    long[] masks = new long[] {PATTERN_ALL, SideImproved.maskSweet, SideImproved.maskCenter, SideImproved.maskEdge};
    allHalfPatterns.add(patternsSweet);
    allHalfPatterns.add(patternCenter);
    allHalfPatterns.add(patternEdge);
    allHalfPatterns.add(patternSide);
    ArrayList<ArrayList<Long>> allPatterns = new ArrayList<>();
    
    int i = 0, j = 0;
    for (ArrayList<Long> halfPatterns1 : allHalfPatterns) {
      j = 0;
      for (ArrayList<Long> halfPatterns2 : allHalfPatterns) {
        // We combine halfPatterns1 and halfPatterns2 into allPatterns.
        ArrayList<Long> combined = new ArrayList<>();
        for (long halfPattern1 : halfPatterns1) {
          if ((halfPattern1 & masks[i]) == 0) { continue; }
          for (long halfPattern2 : halfPatterns2) {
            if ((halfPattern2 & masks[j]) == 0) { continue; }
            combined.add(halfPattern1 | BitPattern.horizontalMirror(halfPattern2));
          }
        }
        allPatterns.add(combined);
        j++;
      }
      i++;
    }
    HashSet<Integer> foundPatterns = new HashSet<>();
    HashMap<Integer, Board> foundPatternsMap = new HashMap<>();
    SideImproved hasher = new SideImproved();
    
    for (ArrayList<Long> patterns : allPatterns) {
      System.out.println("YEAH!");
      i = 0;
      for (long player : patterns) {
        for (long opponent : patterns) {
          i++;
          if ((player & opponent) == 0) {
//            player = player | (((long) (Math.random() * Long.MAX_VALUE))
//                    & ~bitPatterns[i] & ~masks[i]);
//            opponent = opponent | (((long) (Math.random() * Long.MAX_VALUE))
//                    & ~bitPatterns[i] & ~masks[i]);
            int hash = SideImproved.hash(player, opponent, BitPattern.horizontalMirror(player), BitPattern.horizontalMirror(opponent));
//            System.out.println(i + "/" + patterns.size() * patterns.size());
          
            assert(hash < hasher.maxSize());
            if (foundPatterns.contains(hash)) {
              System.out.println("Error:\n" + 
                new Board(player, opponent) + "\n\n" +
                foundPatternsMap.get(hash) + "\n" + 
                BitPattern.patternToString(SideImproved.halfHash(
                  foundPatternsMap.get(hash).getPlayer(), 
                  foundPatternsMap.get(hash).getOpponent())) + "\n" +
                BitPattern.patternToString(SideImproved.halfHash(
                  player, opponent)));
            }
            assert(!foundPatterns.contains(hash));
            foundPatterns.add(hash);
            foundPatternsMap.put(hash, new Board(player, opponent));
          }
        }
      }
    }
  }
}
