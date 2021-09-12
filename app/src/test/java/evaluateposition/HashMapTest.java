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

import static org.junit.Assert.*;

import org.junit.Test;

import board.Board;
import evaluateposition.HashMap.BoardInHash;
import java.util.ArrayList;

public class HashMapTest {

  @Test
  public void testStartingPosition() {
    Board b1 = new Board();
    Board b2 = new Board(12423, 45);
    
    HashMap visitedPositions = new HashMap(5);
    
    assertEquals(0, visitedPositions.size());
    
    assertEquals(visitedPositions.get(b1), null);
    assertEquals(visitedPositions.get(b1), null);
    
    visitedPositions.updateLowerBound(b1, -10, 1, 12, 13); 
    assertEquals(visitedPositions.get(b1), new EvaluatedBoard(-10, 1, 6600, 0));
    visitedPositions.updateLowerBound(b1, -20, 1, 12, 13);    
    assertEquals(visitedPositions.get(b1), new EvaluatedBoard(-20, 1, 6600, 0));
    visitedPositions.updateLowerBound(b1, -2, 1, 12, 13);    
    assertEquals(visitedPositions.get(b1), new EvaluatedBoard(-2, 1, 6600, 0));
    visitedPositions.updateLowerBound(b1, -10, 2, 12, 13);    
    assertEquals(visitedPositions.get(b1), new EvaluatedBoard(-10, 2, 6600, 0));
    assertEquals(1, visitedPositions.size());
    
    visitedPositions.updateUpperBound(b1, 10, 1, 12, 13);
    assertEquals(visitedPositions.get(b1), new EvaluatedBoard(-10, 2, 10, 1));
    visitedPositions.updateUpperBound(b1, 20, 1, 12, 13);
    assertEquals(visitedPositions.get(b1), new EvaluatedBoard(-10, 2, 20, 1));
    visitedPositions.updateUpperBound(b1, 2, 1, 12, 13);
    assertEquals(visitedPositions.get(b1), new EvaluatedBoard(-10, 2, 2, 1));
    visitedPositions.updateUpperBound(b1, 10, 2, 12, 13);
    assertEquals(visitedPositions.get(b1), new EvaluatedBoard(-10, 2, 10, 2));
    
    visitedPositions.updateLowerBound(b2, 12, 5, 12, 13);
    assertEquals(visitedPositions.get(b2), new EvaluatedBoard(12, 5, 6600, 0));
    visitedPositions.updateUpperBound(b2, 10, 1, 12, 13);
    assertEquals(visitedPositions.get(b2), new EvaluatedBoard(12, 5, 10, 1));
  }

  @Test
  public void testRandom() {
    for (int nIter = 0; nIter < 1000; nIter++) {
      int nElements = (int) (Math.random() * 100) + 1;
      HashMap visitedPositions = new HashMap(nElements);
      Board boards[] = new Board[nElements];
      int eval[] = new int[nElements];
      int depth[] = new int[nElements];
      boolean isUpper[] = new boolean[nElements];
     
      for (int i = 0; i < nElements; i++) {
        boards[i] = new Board((long) (Math.random() * Long.MAX_VALUE), 
                              (long) (Math.random() * Long.MAX_VALUE));
        eval[i] = (int) ((Math.random() - 0.5) * 12800);
        depth[i] = (int) (Math.random() * 6000) + 1;
        isUpper[i] = Math.random() > 0.5;
        if (boards[i].getPlayer() == 0 && boards[i].getOpponent() == 0) {
          i--;
          continue;
        }
        if (isUpper[i]) {
          visitedPositions.updateUpperBound(boards[i], eval[i], depth[i], 12, 13);
        } else {
          visitedPositions.updateLowerBound(boards[i], eval[i], depth[i], 12, 13);   
        }
      }
      {
        int j = 0;
        for (BoardInHash a : visitedPositions.boards) {
          for (BoardInHash b = a; b != null; b = b.next) {
            if (!b.isNull()) {
              j++;
            }
          }
        }
        assertEquals(nElements, j);
      }
      
      for (int i = 0; i < nElements; i++) {
        EvaluatedBoard b = visitedPositions.get(boards[i]);
        if (isUpper[i]) {
          assertEquals(b.upper, eval[i], 1.E-10);
          assertEquals(b.depthUpper, depth[i], 1.E-10);
        } else {
          assertEquals(b.lower, eval[i], 1.E-10);
          assertEquals(b.depthLower, depth[i], 1.E-10);          
        }
      }
    }
  }
  
  private void addToHashMap(HashMap h) {
    Board board = new Board((long) (Math.random() * Long.MAX_VALUE), (long) (Math.random() * Long.MAX_VALUE));
    int eval = (int) ((Math.random() - 0.5) * 12800);
    int depth = (int) (Math.random() * 6000) + 1;
    if (Math.random() > 0.5) {
      h.updateUpperBound(board, eval, depth, 12, 13);
    } else {
      h.updateLowerBound(board, eval, depth, 12, 13);   
    }
  }
  
  @Test
  public void testThreadSafe() {
    for (int i = 0; i < 1000; ++i) {
      int hashMapSize = (int) (Math.random() * 30 + 1);
      int nElements = (int) (Math.random() * 100 + 1);

      HashMap hashMap = new HashMap(hashMapSize);
      ArrayList<HashMap> hashMapStream = new ArrayList<>();
      for (int j = 0; j < nElements; ++j) {
        hashMapStream.add(hashMap);
      }
      hashMapStream.parallelStream().forEach((h) -> {addToHashMap(h);});
      assert hashMap.allOk();
    }
  }
}
