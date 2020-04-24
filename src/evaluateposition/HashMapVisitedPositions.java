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
import board.PossibleMovesFinderImproved;
import bitpattern.BitPattern;
import java.util.ArrayList;
import java.util.HashSet;

public class HashMapVisitedPositions {
  protected final PossibleMovesFinderImproved possibleMovesFinder;

  public StoredBoard evaluationsHashMap[];
  protected StoredBoard firstPosition;
  private int arraySize;
  long maxSize;
  long size;
  int evalGoal = 7000;
  
  protected class PositionToImprove {
    StoredBoard board;
    boolean playerIsStartingPlayer;
    boolean playerVariates;
    int alpha;
    int beta;
    ArrayList<StoredBoard> parents;
    
    protected PositionToImprove(StoredBoard board, boolean playerIsStartingPlayer,
        ArrayList<StoredBoard> parents, int alpha, int beta) {
      this.board = board;
      this.playerIsStartingPlayer = playerIsStartingPlayer;
      this.parents = parents;
      this.alpha = alpha;
      this.beta = beta;
    }
    
    public PositionToImprove(StoredBoard board, boolean playerVariates,
        boolean playerIsStartingPlayer, StoredBoard firstPosition,
        ArrayList<StoredBoard> parents) {
      this.board = board;
      this.playerIsStartingPlayer = playerIsStartingPlayer;
      this.parents = parents;
      this.setAlphaBeta(firstPosition);
    }
  
    private void setAlphaBeta(StoredBoard firstPosition) {
      if (playerIsStartingPlayer) {
        alpha = firstPosition.eval;
        beta = firstPosition.eval;
      } else {
        alpha = -firstPosition.eval;
        beta = -firstPosition.eval;
      }
      if (alpha == beta) {
        if (playerVariates) {
          beta++;
        } else {
          alpha--;
        }
      }
    }
  }
  
  public HashMapVisitedPositions() {
    this(2000000, 1000000, PossibleMovesFinderImproved.load());
  }
  
  public HashMapVisitedPositions(int arraySize, long maxSize, PossibleMovesFinderImproved possibleMovesFinder) {
    this.arraySize = arraySize;
    this.maxSize = maxSize;
    this.possibleMovesFinder = possibleMovesFinder;
    empty();
  }

  protected synchronized PositionToImprove nextPositionToImproveRandom(boolean playerIsStartingPlayer) {
    int i = 0;
    for (StoredBoard b = first(); b != null; b = next(b.player, b.opponent)) {
      if (getFull(b.player, b.opponent).isLeaf() && !b.isSolved()) {
        i++;
      }
    }
    int chosen = 1 + (int) (Math.random() * i);
    i = 0;
    for (StoredBoard b = first(); b != null; b = next(b.player, b.opponent)) {
      if (getFull(b.player, b.opponent).isLeaf() && !b.isSolved()) {
        i++;
      }

      if (i == chosen) {
        ArrayList<StoredBoard> parents = new ArrayList<>();
        StoredBoard parent = b;
        while (parent.fathers.size() > 0) {
          parents.add(parent);
          parent = parent.fathers.get((int) (Math.random() * parent.fathers.size()));
        }
        ArrayList<StoredBoard> parentsReversed = new ArrayList<>();
        for (int p = parents.size() - 1; p >= 0; --p) {
          parentsReversed.add(parents.get(p));
        }
        return new PositionToImprove(b, Math.random() > 0.5, playerIsStartingPlayer, this.firstPosition, parentsReversed);
      }
    }
    return null;
  }


//  protected synchronized PositionToImprove nextPositionToImproveEndgame(
//      StoredBoard father, boolean playerVariates, boolean playerIsStartingPlayer,
//      ArrayList<StoredBoard> parents) {
//    parents.add(father);
////    System.out.println("ENDGAME");
//    lastEndgame = true;
//    if (father.isLeaf()) {
//      return new PositionToImprove(father, playerVariates, playerIsStartingPlayer, this.firstPosition, parents);
//    }
//    int bestValue = Integer.MIN_VALUE;
//    StoredBoard best = null; 
//    if (playerVariates) {
//      // Player variates: maximize upper bound (lower bound next move).
//      for (StoredBoard child : father.children) {
//        if (bestValue < -child.bestVariationOpponent) {
//          bestValue = -child.bestVariationOpponent;
//          best = child;
//        }
//      }
//    } else {
//      // Opponent variates:
//      best = father.bestChild(this.getEvalGoalForBoard(father));
//    }
//    return nextPositionToImproveEndgame(best, !playerVariates, !playerIsStartingPlayer, parents);
//  }
//  boolean lastEndgame = false;
//  GaussianNumberGenerator tmp = new GaussianNumberGenerator();
  
//  protected synchronized PositionToImprove nextPositionToImproveStandard(
//      StoredBoard position,
////      int sample,
//      boolean playerVariates,
//      boolean playerIsStartingPlayer,
//      ArrayList<StoredBoard> parents) {
//    parents.add(position);
////    System.out.println("STANDARD");
//    lastEndgame = false;
//    if (position.isLeaf()) {
////      return new PositionToImprove(position, playerVariates, playerIsStartingPlayer, this.firstPosition, parents);
//      return new PositionToImprove(position, true, playerIsStartingPlayer, this.firstPosition, parents);
//    }
//    double bestValue = Double.NEGATIVE_INFINITY;
//    StoredBoard best = null; 
////    if (playerVariates) {
//      for (StoredBoard child : position.children) {
////        if (child.isPartiallySolved()) {
////          continue;
////        }
//        double current;
//        if (playerVariates) {
//          current = child.costUntilLeafDefense + StoredBoard.edgeCost(position, child, true);
//        } else {
//          current = child.costUntilLeafAttack + StoredBoard.edgeCost(position, child, false);
//        }//(int) (-child.getLowerBound() - 50 * Math.log(child.descendants)); // TODO: FIX!!!
//        if (current >= bestValue) {
//          bestValue = current;
//          best = child;
//        }
//      }
////    } else {
////      for (StoredBoard child : position.children) {
////        if (child.isPartiallySolved()) {
////          continue;
////        }
////        int current = (int) (-child.getLowerBound());// + (Math.random() - 0.5) * 0); // Suppressing now.
////        if (current >= bestValue) {
////          bestValue = current;
////          best = child;
////        }
////      }
////    }
//    return nextPositionToImproveStandard(best, !playerVariates,// sample, 
//                                         !playerIsStartingPlayer, parents);
//  }

  protected synchronized PositionToImprove nextPositionToImprove(
      StoredBoard father, boolean playerIsStartingPlayer, int offset,
      ArrayList<StoredBoard> parents) {
    parents.add(father);
    if (father.isLeaf()) {
      return new PositionToImprove(father, playerIsStartingPlayer, parents, -6400, 6400);
    }
    double bestValue = Double.NEGATIVE_INFINITY;
    StoredBoard best = null; 
    for (StoredBoard child : father.children) {
      float curValue = father.childMaxLogDerivative(offset, child);
//      System.out.println(curValue + " " + StoredBoard.offsetToEval(offset));
      if (curValue >= bestValue) {
        bestValue = curValue;
        best = child;
      }
    }
    return nextPositionToImprove(best, !playerIsStartingPlayer, StoredBoard.invertOffset(offset), parents);
  }
  
  protected synchronized PositionToImprove nextPositionToImprove() {
    ArrayList<StoredBoard> parents = new ArrayList<>();
    StoredBoard positionToEvaluateLocal = this.firstPosition;
    if (positionToEvaluateLocal.isSolved()) {
      return null;
    }
    return nextPositionToImprove(positionToEvaluateLocal, true, positionToEvaluateLocal.getBestOffset(), parents);
  }

//  protected synchronized PositionToImprove nextPositionToImprove() {
//    ArrayList<StoredBoard> parents = new ArrayList<>();
//    StoredBoard positionToEvaluateLocal = this.firstPosition;
//    if (positionToEvaluateLocal.isSolved()) {
//      return null;
//    }
//    int evalGoal = this.firstPosition.getEvalGoal();
////    System.out.println("\nOrigEvalGoal: " + evalGoal);
//    return nextPositionToImprovePNS(
//        positionToEvaluateLocal,
//        evalGoal,
//        true, parents);
//
////    boolean playerVariates;
////    if (positionToEvaluateLocal.bestVariationPlayer < -6400) {
////      assert(positionToEvaluateLocal.bestVariationOpponent <= 6400);
////      playerVariates = false;
////    } else if (positionToEvaluateLocal.bestVariationOpponent > 6400) {
////      playerVariates = true;
////    } else {
////      int playerDelta = positionToEvaluateLocal.bestVariationPlayer - positionToEvaluateLocal.eval;
////      int opponentDelta = positionToEvaluateLocal.eval - positionToEvaluateLocal.bestVariationOpponent;
////      playerVariates = playerDelta > opponentDelta;
////    }
////    System.out.println("Endgame: " + playerVariates);
//  }

  private void empty() {
    evaluationsHashMap = new StoredBoard[arraySize];
    size = 0;
  }
  
  public static final int hashBoard(long player, long opponent, int maxElements) {
    int newPlayer = (int) ((player + (player >>> 32)) * 541725397157L);
    int newOpponent = (int) ((opponent + (opponent >>> 32)) * 541725397157L);
    return ((3 * newPlayer + 17 * newOpponent) % maxElements + maxElements) % maxElements;
  }
  
  public void addFirstPosition(StoredBoard b) {
    empty();
    add(b);
//    b.playerIsStartingPlayer = true;
    this.firstPosition = b;
  }
  
  protected void add(StoredBoard b) {
    long player = b.player;
    long opponent = b.opponent;
    int hash = hashBoard(player, opponent, arraySize);

    StoredBoard first = evaluationsHashMap[hash];
    b.next = first;
    if (first != null) {
      first.prev = b;
    }
    evaluationsHashMap[hash] = b;
    size++;
  }
  
  public void add(StoredBoard[] children, StoredBoard father) {
    StoredBoard fatherInHash = getFull(father.player, father.opponent);
    assert(children.length > 0);
    for (int i = 0; i < children.length; i++) {
      StoredBoard child = children[i];
//      child.playerIsStartingPlayer = !father.playerIsStartingPlayer;
      StoredBoard childInHash = getFull(child.player, child.opponent);
      if (childInHash != null) {
        boolean found = false;
        for (StoredBoard otherFather : childInHash.fathers) {
          if (otherFather.player == fatherInHash.player &&
              otherFather.opponent == fatherInHash.opponent) {
            found = true;
            break;
          }
        }
        if (!found) {
          childInHash.fathers.add(fatherInHash);
        }
        children[i] = childInHash;
        continue;
      }
      child.fathers.add(fatherInHash);
      add(child);
//      this.updateEvalGoalForLeaf(child);
    }
    fatherInHash.addChildren(children);
    children[0].updateFathers();
  }
  
  public StoredBoard get(Board b) {
    return get(b.getPlayer(), b.getOpponent());
  }
  
  private StoredBoard getFull(long player, long opponent) {
    int hash = hashBoard(player, opponent, arraySize);

    StoredBoard first = evaluationsHashMap[hash];
    StoredBoard b = first;

    while (b != null) {
      if (b.player == player && b.opponent == opponent) {
        return b;
      }
      b = b.next;
    }
    return null;
  }
  public StoredBoard get(long player, long opponent) {
    StoredBoard result = getFull(player, opponent);
    if (result == null) {
      return null;
    }
    return result;
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
  
  public StoredBoard first() {
    return next(-1);
  }
  public StoredBoard next(long player, long opponent) {
    StoredBoard b = getFull(player, opponent);
    if (b.next != null) {
      return b.next;
    }
    return next(hashBoard(player, opponent, arraySize));
  }
//  
//  public EvaluatedBoard getEvaluatedBoard(Board b) {
//    StoredBoard sb = get(b);
//    if (sb == null) {
//      return null;
//    }
//    return sb.toEvaluatedBoard();
//  }
  
  protected boolean hasEvalGoal() {
    return this.evalGoal <= 6400 && this.evalGoal >= -6400;
  }
  
//  protected synchronized void updateAllDescendants() {
//    updateAllDescendantsRecursive(this.firstPosition, new HashSet<>());
//  }
//  
//  protected synchronized void updateAllDescendantsRecursive(StoredBoard start,
//      HashSet<StoredBoard> alreadyDone) {
//    if (alreadyDone.contains(start)) {
//      return;
//    }
//    if (start.isLeaf()) {
//      this.updateEvalGoalForLeaf(start);
//      return;
//    }
//    for (StoredBoard child : start.children) {
//      updateAllDescendantsRecursive(child, alreadyDone);
//    }
//    start.updateFather(this.getEvalGoalForBoard(start));
//    alreadyDone.add(start);
//  }

  /**
   * Checks that this board is correct:
   * 1) If it has a father, the father is truly a father
   * @param b the board
   * @return 
   */
  public boolean boardIsCorrect(StoredBoard b) {
    Board board = b.getBoard();
    for (StoredBoard fatherHash : b.fathers) {
      StoredBoard father = fatherHash;
      boolean found = false;
      Board fatherBoard = father.getBoard();
      long moves[] = possibleMovesFinder.possibleMovesAdvanced(father.player, father.opponent);
      for (long move : moves) {
        Board result = fatherBoard.move(move);
        if (result.getPlayer() == board.getPlayer() && 
            result.getOpponent() == board.getOpponent()) {
          found = true;
          break;
        }
      }
      if (!found) {
        System.out.println("The board\n" + fatherBoard + " should not be one " +
          "of the fathers of the board\n" + board);
        return false;
      }
    }
    return true;
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
