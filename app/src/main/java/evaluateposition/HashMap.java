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

import com.sun.xml.internal.bind.v2.runtime.reflect.opt.Const;

import board.Board;
import constants.Constants;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class HashMap {
  
  public class BoardInHash {
    public long player = 0;
    public long opponent = 0;
    public int lower;
    public int upper;
    public int bestMove;
    public int secondBestMove;
    public int depthLower;
    public int depthUpper;
    public BoardInHash prev;
    public BoardInHash next;
    
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
      this.prev = null;
      this.next = null;
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
  
  public static final int hashBoard(long player, long opponent, int maxElements) {
    int newPlayer = (int) ((player + (player >> 32)) * 541725397157L);
    int newOpponent = (int) ((opponent + (opponent >> 32)) * 541725397157L);
    return Math.abs((3 * newPlayer + 17 * newOpponent) % maxElements);
  }

  public final BoardInHash[] boards;

  protected final BoardInHash firstToRemove;
  protected final BoardInHash lastToRemove;

  private final int arraySize;
  private final AtomicInteger size;
  
  public HashMap() {
    this(Constants.hashMapSize());
  }
  
  public HashMap(int arraySize) {
    this.arraySize = arraySize;
    this.size = new AtomicInteger(0);
    boards = new BoardInHash[arraySize];
    firstToRemove = new BoardInHash();
    lastToRemove = new BoardInHash();
    for (int i = 0; i < boards.length; ++i) {
      boards[i] = new BoardInHash();
    }
    reset();
  }

  public final void reset() {
    for (int i = 0; i < boards.length; ++i) {
      boards[i].reset(0, 0);
    }
    this.size.set(0);
  }
  
  public int size() {
    return size.get();
  }
  
  public BoardInHash getOrAddStoredBoard(Board b) {
    return getOrAddStoredBoard(b.getPlayer(), b.getOpponent());
  }
  
  public BoardInHash getStoredBoard(Board b) {
    return getStoredBoard(b.getPlayer(), b.getOpponent());
  }
  
  public BoardInHash getOrAddStoredBoard(long player, long opponent) {    
    int hash = hashBoard(player, opponent, arraySize);
    BoardInHash first = boards[hash];
    synchronized(first) {
      for (BoardInHash b = first; b != null; b = b.next) {
        if (b.player == player && b.opponent == opponent) {
          if (Constants.ASSERT_EXTENDED) {
            assert(allOk());
          }
          return b;
        }
      }
      BoardInHash b;
      if (first.isNull()) {
        b = first;
        b.reset(player, opponent);
      } else {
        b = new BoardInHash();
        b.reset(player, opponent);
        insertAfter(first, b);
      }
      size.incrementAndGet();
      if (Constants.ASSERT_EXTENDED) {
        assert allOk();
      }
      return b;
    }
  }
  
  public BoardInHash getStoredBoardNoUpdate(Board b) {    
    return getStoredBoardNoUpdate(b.getPlayer(), b.getOpponent());
  }
  
  public BoardInHash getStoredBoardNoUpdate(long player, long opponent) {    
    int hash = hashBoard(player, opponent, arraySize);
    synchronized(boards[hash]) {
      for (BoardInHash b = boards[hash]; b != null; b = b.next) {
        if (b.player == player && b.opponent == opponent) {
          return b;
        }
      }
    }
    return null;
  }
  
  public BoardInHash getStoredBoard(long player, long opponent) {    
    int hash = hashBoard(player, opponent, arraySize);
    synchronized(boards[hash]) {
      for (BoardInHash b = boards[hash]; b != null; b = b.next) {
        if (b.player == player && b.opponent == opponent) {
          return b;
        }
      }
    }
    return null;
  }
  
  
  public EvaluatedBoard get(Board b) {
    BoardInHash sb = getStoredBoard(b);
    if (sb == null) {
      return null;
    }
    return sb.toEvaluatedBoard();
  }
  
  private void insertAfter(BoardInHash b, BoardInHash newB) {
    assert hashBoard(b.player, b.opponent, arraySize) == hashBoard(newB.player, newB.opponent, arraySize);
    assert Thread.holdsLock(boards[hashBoard(b.player, b.opponent, arraySize)]);
    newB.next = b.next;
    if (newB.next != null) {
      newB.next.prev = newB;
    }
    newB.prev = b;
    b.next = newB;
    assert(newB.next != newB);
    assert(newB.prev == null || newB.prev.next == newB);
    assert(newB.next == null || newB.next.prev == newB);
  }
  
  @Override
  public String toString() {
    String result = "";
//    int i = 0;
//    for (BoardInHash a : boards) {
//      result += i++ + "\n";
//      for (BoardInHash b = a; b != null; b = b.next) {
//        result += b.toString();
//      }
//    }
    return result;
  }
  
  public void updateUpperBound(long player, long opponent, int eval, int depth, int bestMove, int secondBestMove) {
    BoardInHash evaluation = getOrAddStoredBoard(player, opponent);

    synchronized(evaluation) {
      evaluation.upper = eval;
      evaluation.depthUpper = depth;
      evaluation.bestMove = bestMove;
      evaluation.secondBestMove = secondBestMove;
    }
  }
  
  public void updateUpperBound(Board b, int eval, int depth, int bestMove, int secondBestMove) {    
    updateUpperBound(b.getPlayer(), b.getOpponent(), eval, depth, bestMove, secondBestMove);
  }
  
  public void updateLowerBound(long player, long opponent, int eval, int depth, int bestMove, int secondBestMove) {    
    BoardInHash evaluation = getOrAddStoredBoard(player, opponent);

    synchronized(evaluation) {
      evaluation.lower = eval;
      evaluation.depthLower = depth;
      evaluation.bestMove = bestMove;
      evaluation.secondBestMove = secondBestMove;
    }
  }
  
  public void updateLowerBound(Board b, int eval, int depth, int bestMove, int secondBestMove) {    
    updateLowerBound(b.getPlayer(), b.getOpponent(), eval, depth, bestMove, secondBestMove);
  }
  
  boolean allOk() {
    long testSize = 0;
    for (int hash = 0; hash < boards.length; ++hash) {
      if (boards[hash] != null && boards[hash].prev != null) {
        System.out.println("Wrong prev for board " + boards[hash] +
            " at evaluationsHashMap[" + hash + "].");
        return false;
      }
      for (BoardInHash b = boards[hash]; b != null; b = b.next) {
        if (b.isNull()) {
          continue;
        }
        ++testSize;
        if (hashBoard(b.player, b.opponent, arraySize) != hash) {
          System.out.println("Wrong hash for board " + b);
          return false;
        }
      }
    }
    if (Constants.MAX_PARALLEL_TASKS == 1 && testSize != this.size.get()) {
      System.out.println(testSize + " " + this.size.get());
    }
    return Constants.MAX_PARALLEL_TASKS > 1 || testSize == this.size.get();
  }
  
//  public static void main(String args[]) {
//    
//    int N = 10000000;
//    int nElements = N / 2 + 1;
//    HashMap visitedPositions = 
//      new HashMap(N, nElements);
//    long t = System.currentTimeMillis();
//    long maxNIter = 100000;
//    int intIter = 1000;
//    for (int nIter = 0; nIter < maxNIter; nIter++) {
//      Board board = new Board((long) (Math.random() * Long.MAX_VALUE), 
//                            (long) (Math.random() * Long.MAX_VALUE));
//      int eval = (int) ((Math.random() - 0.5) * 12800);
//      int depth = (int) (Math.random() * 6000);
//      int value = 0;
//      for (int i = 0; i < intIter; ++i) {
//        visitedPositions.updateLowerBound(board, eval, depth, -1, -1);
//        visitedPositions.updateUpperBound(board, eval, depth, -1, -1);
//        value += visitedPositions.get(board).depthUpper;
//      }
//      if (nIter * intIter % 1000000 == 0) {
//        System.out.println(nIter + " " + (System.currentTimeMillis() - t));
//      }
//      
//
//      if (value - intIter * depth != 0) {
//        System.out.println("BIG MISTAKE!!");
//      }
//    }
//    System.out.println(intIter * maxNIter + " " + (System.currentTimeMillis() - t));
//    System.out.println(intIter * maxNIter * 1000 / (System.currentTimeMillis() - t));
//  }
}
