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

import board.Board;
import java.util.Arrays;

public class HashMapVisitedPositions {
  public StoredBoard[] evaluationsHashMap;
  protected StoredBoard firstPosition;
  private int arraySize;
  long maxSize;
  long size;
  
  public HashMapVisitedPositions() {
    this(1000000, 2000000);
  }
  
  public HashMapVisitedPositions(int maxSize, int arraySize) {
    this.arraySize = arraySize;
    this.maxSize = maxSize;
    evaluationsHashMap = new StoredBoard[arraySize];
    empty();
  }

  public final void empty() {
    Arrays.fill(evaluationsHashMap, null);
//    System.gc();
    firstPosition = StoredBoard.initialStoredBoard(new Board());
    size = 0;
  }
  
  public void add(StoredBoard b) {
    int hash = hash(b);

    StoredBoard first = evaluationsHashMap[hash];
    assert b != first;
    evaluationsHashMap[hash] = b;
    size++;
    if (first != null) {
      assert hash(first) == hash;
      first.prev = b;
      b.next = first;
      
      assert first.isPrevNextOK();
    }
    assert b.isPrevNextOK();
  }
  
  public StoredBoard get(Board b) {
    return get(b.getPlayer(), b.getOpponent());
  }
  
  private int hash(StoredBoard b) {
    return hash(b.getPlayer(), b.getOpponent());
  }
  
  private int hash(long player, long opponent) {
    return Board.hash(player, opponent) % arraySize;
  }
  
  public StoredBoard get(long player, long opponent) {
    for (StoredBoard b = evaluationsHashMap[hash(player, opponent)]; b != null; b = b.next) {
      assert(b != b.next);
      if (b.getPlayer() == player && b.getOpponent() == opponent) {
        return b;
      }
    }
    return null;
  }
  
  private StoredBoard next(int hash) {
    StoredBoard b;
    for (int i = hash + 1; i < arraySize; i++) {
      b = evaluationsHashMap[i];
      if (b != null) {
        return b;
      }
    }
    return null;
  }
  
  @Override
  public String toString() {
    String result = "";
    int i = 0;
    for (StoredBoard a : evaluationsHashMap) {
      result += i++ + "\n";
      StoredBoard b = a;
      while (b != null) {
        result = result.concat(b.toString());
        b = b.next;
      }
    }
    return result;
  }
}