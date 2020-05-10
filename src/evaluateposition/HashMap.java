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

public class HashMap {
  
  public class BoardInHash {
    public long player = 0;
    public long opponent = 0;
    public int lower;
    public int upper;
    public int depthLower;
    public int depthUpper;
    public BoardInHash prev;
    public BoardInHash next;
    public BoardInHash prevToRemove;
    public BoardInHash nextToRemove;
    public int bestMove;
    public int secondBestMove;
    
    public EvaluatedBoard toEvaluatedBoard() {
      if (player == 0 && opponent == 0) {
        return null;
      }
      return new EvaluatedBoard(lower, depthLower, upper, depthUpper);
    }
  
    public final void reset(long player, long opponent) {
      this.player = player;
      this.opponent = opponent;
      this.lower = -6600;
      this.upper = 6600;
      this.depthLower = 0;
      this.depthUpper = 0;
      this.prevToRemove = null;
      this.nextToRemove = null;
    }
  
    public boolean isNull() {
      return depthLower == 0 && depthUpper == 0;
    }
      @Override
    public String toString() {
      if (isNull()) {
        return "()\n";
      }
      return new Board(player, opponent).toString() + "\n" + lower + "@" + depthLower + " <= eval <= " + upper + "@" + depthUpper + "\n";
    }
  }

  public BoardInHash prevToRemove = null;
  public BoardInHash nextToRemove = null;
  public ArrayList<BoardInHash> myArray;
  
  public static final int hashBoard(long player, long opponent, int maxElements) {
    int newPlayer = (int) ((player + (player >> 32)) * 541725397157L);
    int newOpponent = (int) ((opponent + (opponent >> 32)) * 541725397157L);
    return Math.abs(3 * newPlayer + 17 * newOpponent) % maxElements;
  }

  public BoardInHash evaluationsHashMap[];

  protected BoardInHash firstToRemove;
  protected BoardInHash lastToRemove;

  private int arraySize;
  int maxSize;
  int size;
  
  public HashMap() {
    this(10000001, 5000000);
  }
  
  public HashMap(int arraySize, int maxSize) {
    this.arraySize = arraySize;
    this.maxSize = maxSize;
    this.size = 0;
    reset();
  }

  public final void reset() {
    evaluationsHashMap = new BoardInHash[arraySize];
    firstToRemove = new BoardInHash();
    lastToRemove = new BoardInHash();
    BoardInHash boards[] = new BoardInHash[maxSize];
    for (int i = 0; i < maxSize; i++) {
      BoardInHash b = new BoardInHash();
      b.player = (long) (Math.random() * Long.MAX_VALUE);
      boards[i] = b;
      this.insertBefore(b, evaluationsHashMap[hashBoard(b.player, b.opponent, arraySize)]);
      evaluationsHashMap[hashBoard(b.player, b.opponent, arraySize)] = b;
    }
    for (int i = 0; i < maxSize - 1; i++) {
      pairToRemove(boards[i], boards[i + 1]);
    }
    pairToRemove(firstToRemove, boards[0]);
    pairToRemove(boards[maxSize - 1], lastToRemove);
    size = 0;
  }
  
  public int size() {
    return size;
  }
  
  public BoardInHash getOrAddStoredBoard(Board b) {
    return getOrAddStoredBoard(b.getPlayer(), b.getOpponent());
  }
  
  public BoardInHash getStoredBoard(Board b) {
    return getStoredBoard(b.getPlayer(), b.getOpponent());
  }
  
  public BoardInHash getOrAddStoredBoard(long player, long opponent) {    
    int hash = hashBoard(player, opponent, arraySize);
    for (BoardInHash b = evaluationsHashMap[hash]; b != null; b = b.next) {
      if (b.player == player && b.opponent == opponent) {
        this.updateToRemove(b);
        assert(allOk());
        return b;
      }
    }
    BoardInHash b = removeOneBoard();
    b.reset(player, opponent);
    size++;
    insertBefore(b, evaluationsHashMap[hash]);
    evaluationsHashMap[hash] = b;
    addBeforeToRemove(b, lastToRemove);
    assert(evaluationsHashMap[hash].prev == null);
    assert(allOk());
    return b;
  }
  
  public BoardInHash getStoredBoardNoUpdate(Board b) {    
    return getStoredBoardNoUpdate(b.getPlayer(), b.getOpponent());
  }
  
  public BoardInHash getStoredBoardNoUpdate(long player, long opponent) {    
    int hash = hashBoard(player, opponent, arraySize);
    for (BoardInHash b = evaluationsHashMap[hash]; b != null; b = b.next) {
      if (b.player == player && b.opponent == opponent) {
        return b;
      }
    }
    return null;
  }
  
  public BoardInHash getStoredBoard(long player, long opponent) {    
    int hash = hashBoard(player, opponent, arraySize);
    for (BoardInHash b = evaluationsHashMap[hash]; b != null; b = b.next) {
      if (b.player == player && b.opponent == opponent) {
        this.updateToRemove(b);
        return b;
      }
    }
    return null;
  }
  
  private boolean allOk() {
    for (int hash = 0; hash < evaluationsHashMap.length; ++hash) {
      if (evaluationsHashMap[hash] != null &&
          evaluationsHashMap[hash].prev != null) {
        System.out.println("Wrong prev for board " + evaluationsHashMap[hash] +
            " at evaluationsHashMap[" + hash + "].");
        return false;
      }
      for (BoardInHash b = evaluationsHashMap[hash]; b != null; b = b.next) {
        if (hashBoard(b.player, b.opponent, arraySize) != hash) {
          System.out.println("Wrong hash for board " + b);
          return false;
        }
      }
    }
    return true;
  }
  
  public EvaluatedBoard get(Board b) {
    BoardInHash sb = getStoredBoard(b);
    if (sb == null) {
      return null;
    }
    return sb.toEvaluatedBoard();
  }
  
  public final static void pairToRemove(BoardInHash prev, BoardInHash next) {
    prev.nextToRemove = next;
    next.prevToRemove = prev;
  }
  
  private void insertBefore(BoardInHash b, BoardInHash next) {
    b.next = next;
    if (next != null) {
      b.prev = next.prev;
      next.prev = b;
    }
    b.prev = null;
    assert(b.next != b);
    assert(b.prev == null || b.prev.next == b);
    assert(b.next == null || b.next.prev == b);
  }
  public final static void addBeforeToRemove(BoardInHash b, BoardInHash next) {
    pairToRemove(next.prevToRemove, b);
    pairToRemove(b, next);
  }
  
  public BoardInHash removeOneBoard() {
    BoardInHash toRemove = firstToRemove.nextToRemove;
    pairToRemove(firstToRemove, toRemove.nextToRemove);

    if (!toRemove.isNull()) {
      size--;
    }
    if (toRemove.prev == null) {
      assert(evaluationsHashMap[hashBoard(toRemove.player, toRemove.opponent, arraySize)] == toRemove);
      evaluationsHashMap[hashBoard(toRemove.player, toRemove.opponent, arraySize)] = toRemove.next;
    } else {
      assert(evaluationsHashMap[hashBoard(toRemove.player, toRemove.opponent, arraySize)] != toRemove);
      toRemove.prev.next = toRemove.next;
    }
    if (toRemove.next != null) {
      toRemove.next.prev = toRemove.prev;
    }
    assert(evaluationsHashMap[hashBoard(toRemove.player, toRemove.opponent, arraySize)] == null ||
        evaluationsHashMap[hashBoard(toRemove.player, toRemove.opponent, arraySize)].prev == null);
    assert(allOk());
    return toRemove;
  }
  
  public void updateToRemove(BoardInHash b) {
    // Remove b from the list.
    pairToRemove(b.prevToRemove, b.nextToRemove);
    // Add b at the end.
    addBeforeToRemove(b, lastToRemove);   
    assert(allOk()); 
  }
  
  @Override
  public String toString() {
    String result = "";
    int i = 0;
    for (BoardInHash a : evaluationsHashMap) {
      result += i++ + "\n";
      for (BoardInHash b = a; b != null; b = b.next) {
        result += b.toString();
      }
    }
    return result;
  }
  
  public void updateUpperBound(long player, long opponent, int eval, int depth, int bestMove, int secondBestMove) {
    BoardInHash evaluation = getOrAddStoredBoard(player, opponent);

    evaluation.upper = eval;
    evaluation.depthUpper = depth;
    evaluation.bestMove = bestMove;
    evaluation.secondBestMove = secondBestMove;    
  }
  
  public void updateUpperBound(Board b, int eval, int depth, int bestMove, int secondBestMove) {    
    updateUpperBound(b.getPlayer(), b.getOpponent(), eval, depth, bestMove, secondBestMove);
  }
  
  public void updateLowerBound(long player, long opponent, int eval, int depth, int bestMove, int secondBestMove) {    
    BoardInHash evaluation = getOrAddStoredBoard(player, opponent);

    evaluation.lower = eval;
    evaluation.depthLower = depth;
    evaluation.bestMove = bestMove;
    evaluation.secondBestMove = secondBestMove;    
  }
  
  public void updateLowerBound(Board b, int eval, int depth, int bestMove, int secondBestMove) {    
    updateLowerBound(b.getPlayer(), b.getOpponent(), eval, depth, bestMove, secondBestMove);
  }
  
  public static void main(String args[]) {
    
    int N = 10000000;
    int nElements = N / 2 + 1;
    HashMap visitedPositions = 
      new HashMap(N, nElements);
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
        visitedPositions.updateLowerBound(board, eval, depth, -1, -1);
        visitedPositions.updateUpperBound(board, eval, depth, -1, -1);
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
