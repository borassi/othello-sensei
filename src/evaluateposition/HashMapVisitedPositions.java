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
import java.util.Arrays;

public class HashMapVisitedPositions {
  public StoredBoard evaluationsHashMap[];
  protected StoredBoard firstPosition;
  private int arraySize;
  long maxSize;
  long size;
  
  class PositionToImprove {
    StoredBoard board;
    boolean playerIsStartingPlayer;
    boolean playerVariates;
    int alpha;
    int beta;
    ArrayList<StoredBoard> parents;
    
    public PositionToImprove(StoredBoard board, boolean playerVariates,
        boolean playerIsStartingPlayer, StoredBoard firstPosition,
        ArrayList<StoredBoard> parents) {
      this.board = board;
      this.playerIsStartingPlayer = playerIsStartingPlayer;
      this.parents = parents;
      this.setAlphaBeta(firstPosition, playerVariates);
    }
  
    private void setAlphaBeta(StoredBoard firstPosition, boolean playerVariates) {
      if (playerIsStartingPlayer) {
        alpha = firstPosition.getEvalGoal();
        beta = firstPosition.getEvalGoal();
      } else {
        alpha = -firstPosition.getEvalGoal();
        beta = -firstPosition.getEvalGoal();
      }
      if (alpha == beta) {
        if (playerVariates) {
          beta++;
        } else {
          alpha--;
        }
      }
      if (!firstPosition.isPartiallySolved()) {
        alpha = -6400;
        beta = 6400;
      }
    }
  }
  
  public HashMapVisitedPositions() {
    this(1000000, 2000000);
  }
  
  public HashMapVisitedPositions(int maxSize, int arraySize) {
    this.arraySize = arraySize;
    this.maxSize = maxSize;
    evaluationsHashMap = new StoredBoard[arraySize];
    empty();
  }

  protected synchronized PositionToImprove nextPositionToImproveMidgame(
      StoredBoard father, boolean playerVariates, boolean playerIsStartingPlayer,
      ArrayList<StoredBoard> parents) {
    parents.add(father);
    if (father.isLeaf()) {
      return new PositionToImprove(father, playerVariates, playerIsStartingPlayer, this.firstPosition, parents);
    }
    if (playerVariates) {
      return nextPositionToImproveMidgame(father.bestChildMidgamePlayerVariates(), !playerVariates, !playerIsStartingPlayer, parents);
    } else {
      return nextPositionToImproveMidgame(father.bestChildMidgameOpponentVariates(), !playerVariates, !playerIsStartingPlayer, parents);
    }
  }

  protected synchronized PositionToImprove nextPositionToImproveEndgame(
      StoredBoard father, boolean playerVariates, boolean playerIsStartingPlayer,
      ArrayList<StoredBoard> parents) {
    parents.add(father);
    if (father.isLeaf()) {
      return new PositionToImprove(father, playerVariates, playerIsStartingPlayer, this.firstPosition, parents);
    }
    if (playerVariates) {
      return nextPositionToImproveEndgame(father.bestChildPlayerVariates(), !playerVariates, !playerIsStartingPlayer, parents);
    } else {
      return nextPositionToImproveEndgame(father.bestChildOpponentVariates(), !playerVariates, !playerIsStartingPlayer, parents);
    }
  }

  protected void empty() {
    Arrays.fill(evaluationsHashMap, null);
    System.gc();
    size = 0;
  }
  
  protected void add(StoredBoard b) {
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
  
  protected StoredBoard get(long player, long opponent) {
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
  
  protected StoredBoard first() {
    return next(-1);
  }
  protected StoredBoard next(long player, long opponent) {
    StoredBoard b = get(player, opponent);
    if (b.next != null) {
      return b.next;
    }
    return next(hash(player, opponent));
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