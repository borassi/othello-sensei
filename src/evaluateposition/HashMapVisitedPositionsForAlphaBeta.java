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
import java.util.ArrayList;

public class HashMapVisitedPositionsForAlphaBeta {
  
  public class StoredBoardForAlphaBeta {
    public long player = 0;
    public long opponent = 0;
    public int lower;
    public int upper;
    public int depthLower;
    public int depthUpper;
    public ArrayList<StoredBoardForAlphaBeta> myArray;
    public StoredBoardForAlphaBeta prev;
    public StoredBoardForAlphaBeta next;
    public StoredBoardForAlphaBeta prevToRemove;
    public StoredBoardForAlphaBeta nextToRemove;

    public StoredBoardForAlphaBeta(ArrayList<StoredBoardForAlphaBeta> myArray) {
      this.myArray = myArray;
    }
    
    public EvaluatedBoard toEvaluatedBoard() {
      if (player == 0 && opponent == 0) {
        return null;
      }
      return new EvaluatedBoard(lower, depthLower, upper, depthUpper);
    }
  
    public final void reset(long player, long opponent, 
                            ArrayList<StoredBoardForAlphaBeta> myArray) {
      this.player = player;
      this.opponent = opponent;
      this.lower = -6600;
      this.upper = 6600;
      this.depthLower = 0;
      this.depthUpper = 0;
      this.prevToRemove = null;
      this.nextToRemove = null;
      this.myArray = myArray;
    }
  
    public boolean isNull() {
      return player == 0 && opponent == 0;
    }
      @Override
    public String toString() {
      if (isNull()) {
        return "()\n";
      }
      return new Board(player, opponent).toString() + "\n" + lower + "@" + depthLower + " <= eval <= " + upper + "@" + depthUpper + "\n";
    }
  }

  public ArrayList<StoredBoardForAlphaBeta> fathers = new ArrayList<>();
  public StoredBoardForAlphaBeta prevToRemove = null;
  public StoredBoardForAlphaBeta nextToRemove = null;
  public ArrayList<StoredBoardForAlphaBeta> myArray;
  
  public static final int hashBoard(long player, long opponent, int maxElements) {
    int newPlayer = (int) ((player + (player >> 32)) * 541725397157L);
    int newOpponent = (int) ((opponent + (opponent >> 32)) * 541725397157L);
    return Math.abs(3 * newPlayer + 17 * newOpponent) % maxElements;
  }

  public ArrayList<StoredBoardForAlphaBeta> evaluationsHashMap[];

  protected StoredBoardForAlphaBeta firstToRemove;
  protected StoredBoardForAlphaBeta lastToRemove;

  private int arraySize;
  int maxSize;
  int size;
  
  public HashMapVisitedPositionsForAlphaBeta() {
    this(2000001, 1000000);
  }
  
  public HashMapVisitedPositionsForAlphaBeta(int arraySize, int maxSize) {
    this.arraySize = arraySize;
    this.maxSize = maxSize;
    this.size = 0;
    reset();
  }

  public final void reset() {
    evaluationsHashMap = new ArrayList[arraySize];
    firstToRemove = new StoredBoardForAlphaBeta(new ArrayList<>());
    lastToRemove = new StoredBoardForAlphaBeta(new ArrayList<>());
    for (int i = 0; i < arraySize; i++) {
      evaluationsHashMap[i] = new ArrayList<>();
    }
    for (int i = 0; i < maxSize; i++) {
      evaluationsHashMap[i].add(new StoredBoardForAlphaBeta(evaluationsHashMap[i]));
    }
    for (int i = 0; i < maxSize - 1; i++) {
      pairToRemove(evaluationsHashMap[i].get(0), 
                   evaluationsHashMap[i + 1].get(0));
    }
    pairToRemove(firstToRemove, evaluationsHashMap[0].get(0));
    pairToRemove(evaluationsHashMap[maxSize - 1].get(0), lastToRemove);
    size = 0;
  }
  
  public int size() {
    return size;
  }
  
  public StoredBoardForAlphaBeta getOrAddStoredBoard(Board b) {
    return getOrAddStoredBoard(b.getPlayer(), b.getOpponent());
  }
  
  public StoredBoardForAlphaBeta getStoredBoard(Board b) {
    return getStoredBoard(b.getPlayer(), b.getOpponent());
  }
  
  public StoredBoardForAlphaBeta getOrAddStoredBoard(long player, long opponent) {    
    int hash = hashBoard(player, opponent, arraySize);
    ArrayList<StoredBoardForAlphaBeta> boards = evaluationsHashMap[hash];
    for (StoredBoardForAlphaBeta b : boards) {
      if (b.player == player && b.opponent == opponent) {
        this.updateToRemove(b);
        return b;
      }
    }
    StoredBoardForAlphaBeta b = removeOneBoard();
    b.reset(player, opponent, boards);
    size++;
    boards.add(b);
    addBefore(b, lastToRemove);
    return b;
  }
  
  public StoredBoardForAlphaBeta getStoredBoardNoUpdate(Board b) {    
    return getStoredBoardNoUpdate(b.getPlayer(), b.getOpponent());
  }
  
  public StoredBoardForAlphaBeta getStoredBoardNoUpdate(long player, long opponent) {    
    int hash = hashBoard(player, opponent, arraySize);
    ArrayList<StoredBoardForAlphaBeta> boards = evaluationsHashMap[hash];
    for (StoredBoardForAlphaBeta b : boards) {
      if (b.player == player && b.opponent == opponent) {
        return b;
      }
    }
    return null;
  }
  
  public StoredBoardForAlphaBeta getStoredBoard(long player, long opponent) {    
    int hash = hashBoard(player, opponent, arraySize);
    ArrayList<StoredBoardForAlphaBeta> boards = evaluationsHashMap[hash];
    for (StoredBoardForAlphaBeta b : boards) {
      if (b.player == player && b.opponent == opponent) {
        this.updateToRemove(b);
        return b;
      }
    }
    return null;
  }
  
  public EvaluatedBoard get(Board b) {
    StoredBoardForAlphaBeta sb = getStoredBoard(b);
    if (sb == null) {
      return null;
    }
    return sb.toEvaluatedBoard();
  }
  
  public final static void pairToRemove(StoredBoardForAlphaBeta prev, StoredBoardForAlphaBeta next) {
    prev.nextToRemove = next;
    next.prevToRemove = prev;
  }
  
  public final static void addBefore(StoredBoardForAlphaBeta b, StoredBoardForAlphaBeta next) {
    pairToRemove(next.prevToRemove, b);
    pairToRemove(b, next);
  }
  
  public StoredBoardForAlphaBeta removeOneBoard() {
    StoredBoardForAlphaBeta toRemove = firstToRemove.nextToRemove;
    pairToRemove(firstToRemove, toRemove.nextToRemove);

    if (!toRemove.isNull()) {
      size--;
    }
    ArrayList<StoredBoardForAlphaBeta> toRemoveArray = toRemove.myArray;
    
    StoredBoardForAlphaBeta other = toRemoveArray.remove(toRemoveArray.size() - 1);
    if (toRemove == other) {
      return toRemove;
    }
    
    for (int i = 0; i < toRemoveArray.size(); i++) {
      if (toRemoveArray.get(i) == toRemove) {
        toRemoveArray.set(i, other);
      }
    }
    return toRemove;
  }
  
  public void updateToRemove(StoredBoardForAlphaBeta b) {
    // Remove b from the list.
    pairToRemove(b.prevToRemove, b.nextToRemove);
    // Add b at the end.
    addBefore(b, lastToRemove);    
  }
  
  @Override
  public String toString() {
    String result = "";
    int i = 0;
    for (ArrayList<StoredBoardForAlphaBeta> a : evaluationsHashMap) {
      result += i++ + "\n";
      result = a.stream().map((b) -> b.toString()).reduce(result, String::concat);
    }
    return result;
  }
  
  public void updateUpperBound(long player, long opponent, int eval, int depth) {
    StoredBoardForAlphaBeta evaluation = getOrAddStoredBoard(player, opponent);

    evaluation.upper = eval;
    evaluation.depthUpper = depth;
  }
  
  public void updateUpperBound(Board b, int eval, int depth) {    
    updateUpperBound(b.getPlayer(), b.getOpponent(), eval, depth);
  }
  
  public void updateLowerBound(long player, long opponent, int eval, int depth) {    
    StoredBoardForAlphaBeta evaluation = getOrAddStoredBoard(player, opponent);

    evaluation.lower = eval;
    evaluation.depthLower = depth;
  }
  
  public void updateLowerBound(Board b, int eval, int depth) {    
    updateLowerBound(b.getPlayer(), b.getOpponent(), eval, depth);
  }
  
  public static void main(String args[]) {
    
    int N = 10000000;
    int nElements = N / 2 + 1;
    HashMapVisitedPositionsForAlphaBeta visitedPositions = 
      new HashMapVisitedPositionsForAlphaBeta(N, nElements);
    long t = System.currentTimeMillis();
    long maxNIter = 100000;
    int intIter = 1000;
    for (int nIter = 0; nIter < maxNIter; nIter++) {
      Board board = new Board((long) (Math.random() * Long.MAX_VALUE), 
                            (long) (Math.random() * Long.MAX_VALUE));
      int eval = (int) ((Math.random() - 0.5) * 12800);
      int depth = (int) (Math.random() * 6000);
      int value = 0;
      for (int i = 0; i < intIter; ++i) {
        visitedPositions.updateLowerBound(board, eval, depth);
        visitedPositions.updateUpperBound(board, eval, depth);
        value += visitedPositions.get(board).depthUpper;
      }
      if (nIter * intIter % 1000000 == 0) {
        System.out.println(nIter + " " + (System.currentTimeMillis() - t));
      }
      

      if (value - intIter * depth != 0) {
        System.out.println("BIG MISTAKE!!");
      }
    }
    System.out.println(intIter * maxNIter + " " + (System.currentTimeMillis() - t));
    System.out.println(intIter * maxNIter * 1000 / (System.currentTimeMillis() - t));
  }
}
