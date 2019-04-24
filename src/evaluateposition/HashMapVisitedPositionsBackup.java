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

public class HashMapVisitedPositionsBackup {
  public static final int hashBoard(long player, long opponent, int maxElements) {
    int newPlayer = (int) ((player + (player >>> 32)) * 541725397157L);
    int newOpponent = (int) ((opponent + (opponent >>> 32)) * 541725397157L);
    return ((3 * newPlayer + 17 * newOpponent) % maxElements + maxElements) % maxElements;
  }

  public StoredBoardBackup evaluationsHashMap[];

  protected StoredBoardBackup firstToRemove;
  protected StoredBoardBackup lastToRemove;

  private int arraySize;
  int maxSize;
  int size;
  
  public HashMapVisitedPositionsBackup() {
    this(2000000, 1000000);
  }
  
  public HashMapVisitedPositionsBackup(int arraySize, int maxSize) {
    this.arraySize = arraySize;
    this.maxSize = maxSize;
    this.size = 0;
    evaluationsHashMap = new StoredBoardBackup[arraySize];
    for (int i = 0; i < maxSize; i++) {
      evaluationsHashMap[i] = new StoredBoardBackup(new Board(0, 0));
      evaluationsHashMap[i].positionInHashMap = i;
    }
    firstToRemove = new StoredBoardBackup(new Board(0, 0));
    lastToRemove = new StoredBoardBackup(new Board(0, 0));
    this.pairToRemoveReset();
  }

  public final void pairToRemoveReset() {
    for (int i = 0; i < maxSize - 1; i++) {
      pairToRemove(evaluationsHashMap[i], evaluationsHashMap[i + 1]);
    }
    pairToRemove(firstToRemove, evaluationsHashMap[0]);
    pairToRemove(evaluationsHashMap[maxSize - 1], lastToRemove);
  }
  
  public final void empty() {
    StoredBoardBackup current = firstToRemove.nextToRemove;
    StoredBoardBackup next;
    for (int i = 0; i < maxSize; i++) {
      next = current.nextToRemove;
      current.reset(0, 0);
      current.positionInHashMap = i;
      evaluationsHashMap[i] = current;
      current = next;
    }
    for (int i = maxSize; i < arraySize; i++) {
      evaluationsHashMap[i] = null;      
    }
    pairToRemoveReset();
    size = 0;
  }
  
  public int size() {
    return size;
  }
  
  public StoredBoardBackup getOrAddStoredBoard(Board b) {
    return getOrAddStoredBoard(b.getPlayer(), b.getOpponent());
  }
  
  public StoredBoardBackup getStoredBoard(Board b) {
    return getStoredBoard(b.getPlayer(), b.getOpponent());
  }
  
  
  public void updateEval(Board b, float newEval) {    
    StoredBoardBackup evaluation = getOrAddStoredBoard(b);
    evaluation.eval = (short) (newEval * 100);
  }
  
  public StoredBoardBackup getOrAddStoredBoard(long player, long opponent) {    
    int hash = hashBoard(player, opponent, arraySize);

    StoredBoardBackup first = evaluationsHashMap[hash];
    StoredBoardBackup b = first;

    while (b != null) {
      if (b.player == player && b.opponent == opponent) {
        this.updateToRemove(b);
        return b;
      }
      b = b.nextInHashMap;
    }
    StoredBoardBackup removed = removeOneBoard();
    removed.reset(player, opponent);
    removed.positionInHashMap = hash;
    StoredBoardBackup next = evaluationsHashMap[hash];
    removed.nextInHashMap = next;
    if (next != null) {
      next.prevInHashMap = removed;
    }
    evaluationsHashMap[hash] = removed;
    size++;
    
    addBefore(removed, lastToRemove);
    return removed;
  }
  
  public StoredBoardBackup getStoredBoardNoUpdate(Board b) {    
    return getStoredBoardNoUpdate(b.getPlayer(), b.getOpponent());
  }
  
  public StoredBoardBackup getStoredBoardNoUpdate(long player, long opponent) {    
    int hash = hashBoard(player, opponent, arraySize);

    StoredBoardBackup first = evaluationsHashMap[hash];
    StoredBoardBackup b = first;

    while (b != null) {
      if (b.player == player && b.opponent == opponent) {
        return b;
      }
      b = b.nextInHashMap;
    }
    return null;
  }
  
  public StoredBoardBackup getStoredBoard(long player, long opponent) {    
    int hash = hashBoard(player, opponent, arraySize);

    StoredBoardBackup first = evaluationsHashMap[hash];
    StoredBoardBackup b = first;

    while (b != null) {
      if (b.player == player && b.opponent == opponent) {
        return b;
      }
      b = b.nextInHashMap;
    }
    return null;
  }
  
  public EvaluatedBoard get(Board b) {
    StoredBoardBackup sb = getStoredBoard(b);
    if (sb == null) {
      return null;
    }
    return sb.toEvaluatedBoard();
  }
  
  public final static void pairToRemove(StoredBoardBackup prev, StoredBoardBackup next) {
    prev.nextToRemove = next;
    next.prevToRemove = prev;
  }
  
  public final static void addBefore(StoredBoardBackup b, StoredBoardBackup next) {
    pairToRemove(next.prevToRemove, b);
    pairToRemove(b, next);
  }
  
  public StoredBoardBackup removeOneBoard() {
    StoredBoardBackup toRemove = firstToRemove.nextToRemove;
    pairToRemove(firstToRemove, toRemove.nextToRemove);

    if (!toRemove.isNull()) {
      size--;
    }

    if (toRemove.prevInHashMap != null) {
      toRemove.prevInHashMap.nextInHashMap = toRemove.nextInHashMap;
    } else {
//      System.out.println(toRemove);
//      System.out.println(toRemove.positionInHashMap);
      evaluationsHashMap[toRemove.positionInHashMap] = null;
    }
    if (toRemove.nextInHashMap != null) {
      toRemove.nextInHashMap.prevInHashMap = toRemove.prevInHashMap;
    }
    return toRemove;
  }
  
  public void updateToRemove(StoredBoardBackup b) {
    // Remove b from the list.
    pairToRemove(b.prevToRemove, b.nextToRemove);
    // Add b at the end.
    addBefore(b, lastToRemove);    
  }
  
  @Override
  public String toString() {
    String result = "";
    int i = 0;
    for (StoredBoardBackup a : evaluationsHashMap) {
      result += i++ + "\n";
      StoredBoardBackup b = a;
      while (b != null) {
        result = result.concat(b.toString());
        b = b.nextInHashMap;
      }
    }
    return result;
  }
}
