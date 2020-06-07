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
import constants.Constants;
import java.util.ArrayList;
import java.util.HashSet;

public class HashMapVisitedPositions {
  protected final PossibleMovesFinderImproved possibleMovesFinder;

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
    this(1000000, 2000000, PossibleMovesFinderImproved.load());
  }
  
  public HashMapVisitedPositions(int maxSize, int arraySize, PossibleMovesFinderImproved possibleMovesFinder) {
    this.arraySize = arraySize;
    this.maxSize = maxSize;
    this.possibleMovesFinder = possibleMovesFinder;
    empty();
  }
//
//  protected synchronized PositionToImprove nextPositionToImproveRandom(boolean playerIsStartingPlayer) {
//    int i = 0;
//    for (StoredBoard b = first(); b != null; b = next(b.player, b.opponent)) {
//      if (getFull(b.player, b.opponent).isLeaf() && !b.isSolved()) {
//        i++;
//      }
//    }
//    int chosen = 1 + (int) (Math.random() * i);
//    i = 0;
//    for (StoredBoard b = first(); b != null; b = next(b.player, b.opponent)) {
//      if (getFull(b.player, b.opponent).isLeaf() && !b.isSolved()) {
//        i++;
//      }
//
//      if (i == chosen) {
//        ArrayList<StoredBoard> parents = new ArrayList<>();
//        StoredBoard parent = b;
//        while (parent.fathers.size() > 0) {
//          parents.add(parent);
//          parent = parent.fathers.get((int) (Math.random() * parent.fathers.size()));
//        }
//        ArrayList<StoredBoard> parentsReversed = new ArrayList<>();
//        for (int p = parents.size() - 1; p >= 0; --p) {
//          parentsReversed.add(parents.get(p));
//        }
//        return new PositionToImprove(b, playerIsStartingPlayer, Math.random() > 0.5, this.firstPosition, parentsReversed);
//      }
//    }
//    return null;
//  }

  protected synchronized PositionToImprove nextPositionToImproveMidgame(
      StoredBoard father, boolean playerVariates, boolean playerIsStartingPlayer,
      ArrayList<StoredBoard> parents) {
    parents.add(father);
//    System.out.println("ENDGAME");
//    lastEndgame = true;
    if (father.isLeaf()) {
      return new PositionToImprove(father, playerVariates, playerIsStartingPlayer, this.firstPosition, parents);
    }
    if (playerVariates) {
      return nextPositionToImproveMidgame(father.bestChildMidgamePlayerVariates(), !playerVariates, !playerIsStartingPlayer, parents);
    } else {
      return nextPositionToImproveMidgame(father.bestChildMidgameOpponentVariates(), !playerVariates, !playerIsStartingPlayer, parents);
    }
//    return nextPositionToImproveEndgame(best, !playerVariates, !playerIsStartingPlayer, parents);
  }

  protected synchronized PositionToImprove nextPositionToImproveEndgame(
      StoredBoard father, boolean playerVariates, boolean playerIsStartingPlayer,
      ArrayList<StoredBoard> parents) {
    parents.add(father);
//    System.out.println("ENDGAME");
//    lastEndgame = true;
    if (father.isLeaf()) {
      return new PositionToImprove(father, playerVariates, playerIsStartingPlayer, this.firstPosition, parents);
    }
    if (playerVariates) {
      return nextPositionToImproveEndgame(father.bestChildPlayerVariates(), !playerVariates, !playerIsStartingPlayer, parents);
    } else {
      return nextPositionToImproveEndgame(father.bestChildOpponentVariates(), !playerVariates, !playerIsStartingPlayer, parents);
    }
//    return nextPositionToImproveEndgame(best, !playerVariates, !playerIsStartingPlayer, parents);
  }
//  boolean lastEndgame = false;
//  protected synchronized PositionToImprove nextPositionToImproveStandard(
//      StoredBoard position, int sample, boolean playerVariates, boolean playerIsStartingPlayer,
//      ArrayList<StoredBoard> parents) {
//    parents.add(position);
////    System.out.println("STANDARD");
//    lastEndgame = false;
//    if (position.isLeaf()) {
//      return new PositionToImprove(position, playerVariates, playerIsStartingPlayer, this.firstPosition, parents);
//    }
//    int bestValue = 6600;
//    StoredBoard best = null; 
//    if (playerVariates) {
//      for (StoredBoard child : position.children) {
//        int current = (int) (child.samples[sample]); // + (Math.random() - 0.5) * 0); // 400
//        if (current < bestValue) {
//          bestValue = current;
//          best = child;
//        }
//      }
//    } else {
//      for (StoredBoard child : position.children) {
//        int current = (int) (child.eval);// + (Math.random() - 0.5) * 0); // Suppressing now.
//        if (current <= bestValue) {
//          bestValue = current;
//          best = child;
//        }
//      }
//    }
//    return nextPositionToImproveStandard(best, sample, !playerVariates, !playerIsStartingPlayer, parents);
//  }
//
//  protected synchronized PositionToImprove nextPositionToImproveTemp(
//      StoredBoard position, int evalGoal, boolean playerIsStartingPlayer,
//      ArrayList<StoredBoard> parents) {
//    parents.add(position);
////    System.out.println("STANDARD");
//    lastEndgame = false;
//    if (position.isLeaf()) {
//      return new PositionToImprove(position, false, playerIsStartingPlayer, this.firstPosition, parents);
//    }
//    int bestValue = -6600;
//    StoredBoard best = null; 
//    for (StoredBoard child : position.children) {
//      int current = 0;
//      for (int sample : child.samples) {
//        if (-sample >= evalGoal) {
//          current++;
//        }
//      }
//      if (current > bestValue || (current == bestValue && child.eval < best.eval)) {
//        bestValue = current;
//        best = child;
//      }
//    }
//    return nextPositionToImproveTemp(best, -evalGoal, !playerIsStartingPlayer, parents);
//  }

  private void empty() {
    evaluationsHashMap = new StoredBoard[arraySize];
    size = 0;
  }
  
  public void addFirstPosition(StoredBoard b) {
    empty();
    add(b);
    this.firstPosition = b;
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
  
//  /**
//   * Checks that this board is correct:
//   * 1) If it has a father, the father is truly a father
//   * @param b the board
//   * @return 
//   */
//  public boolean boardIsCorrect(StoredBoard b) {
//    Board board = b.getBoard();
//    for (StoredBoard fatherHash : b.fathers) {
//      StoredBoard father = fatherHash;
//      boolean found = false;
//      Board fatherBoard = father.getBoard();
//      long moves[] = possibleMovesFinder.possibleMovesAdvanced(father.player, father.opponent);
//      for (long move : moves) {
//        Board result = fatherBoard.move(move);
//        if (result.getPlayer() == board.getPlayer() && 
//            result.getOpponent() == board.getOpponent()) {
//          found = true;
//          break;
//        }
//      }
//      if (!found) {
//        System.out.println("The board\n" + fatherBoard + " should not be one " +
//          "of the fathers of the board\n" + board);
//        return false;
//      }
//    }
//    return true;
//  }
//  
//  /**
//   * Checks that the children of this board are correct: (only if it is not a
//   * leaf):
//   * 1) All children have it among their fathers;
//   * 2) Its lower / upper / eval is the same as the max(-children_eval), and for
//   *    the upper bound, we consider also otherChildrenUpperBound.
//   * 3) Its bestDescendant is one of the children's bestDescendant
//   * 
//   * In case of a board that passes, if the opponent has moves, we do the same
//   * checks as before, where the only move is a pass. Else, we only check that
//   * we set the final game score.
//   * 
//   * @param b the board to check
//   * @return true if everything is correct, false otherwise
//   */
//  public boolean boardChildrenAreCorrect(StoredBoard b) {
//    if (b.isLeaf()) {
////      if (b.descendants != 1) {
////        System.out.println("The board " + b + " should have 1 descendant "
////          + "because it is a leaf. However, it has " + b.descendants + " descendants.");
////        return false;
////      }
//      return true;
//    }
//
//    Board board = b.getBoard();
//    long moves[] = possibleMovesFinder.possibleMoves(board);
//    if (moves.length == 0) {
//      Board afterPass = board.move(0);
//      if (possibleMovesFinder.possibleMoves(afterPass).length == 0) {
//        // Double pass, game is over.
//        if (b.children.length != 0) {
//          System.out.println("The board\n" + board + "should have no children.");
//          return false;          
//        }
//        if (b.eval != BitPattern.getEvaluationGameOver(board)) {
//          System.out.println("The board\n" + board + 
//            "should contain a gg result, but it does not, as it has eval = " +
//            b.eval);
//          return false;
//        } else if (b.isLeaf()) {
//          System.out.println("The board\n" + board + 
//            "should contain a gg result, but it does not, as it has isLeaf=TRUE");
//          return false;
//        }
//        int i = 0;
////        if (b.bestVariationPlayer >= -6400 || b.bestVariationOpponent <= 6400) {
////          System.out.println("The board\n" + board + 
////            "should contain a gg result, but bestVariationLower, bestVariationUpper = " + 
////            b.bestVariationPlayer + ", " + b.bestVariationOpponent + ". Expected: -6600, 6600");
////          return false;
////        }
////        for (short s : b.samples) {
////          i++;
////          if (s != Math.round(b.eval)) {
////            System.out.println("The board\n" + board + 
////              "should contain a gg result, but the score of sample " + i + 
////              " is wrong. Expected: " + b.eval + ". Actual: " + s);
////            return false;
////          }
////        }
//        return true;
//      }
//      moves = new long[] {0};
//    }
//
//    int eval = -6600;
//    ArrayList<StoredBoard> allChildren = new ArrayList<>();
//    int lower = Integer.MIN_VALUE;
//    int upper = Integer.MIN_VALUE;
//    int bestVariationPlayer = -6800;
//    short samples[] = new short[Constants.N_SAMPLES];
//    for (int i = 0; i < samples.length; ++i) {
//      samples[i] = -6600;
//    }
//
//    if (moves.length != b.children.length) {
//      System.out.println("The board\n" + b.getBoard() + 
//        "\n has a wrong number of fathers.\n" + board);
//      return false;
//    }
//    int descendants = 0;
//    for (long move : moves) {
//      Board next = board.move(move);
//      StoredBoard child = this.getFull(next.getPlayer(), next.getOpponent());
//      allChildren.add(child);
//      boolean found = false;
//      for (StoredBoard childStored : b.children) {
//        if (childStored == child) {
//          found = true;
//          break;
//        }
//      }
//      if (child.playerIsStartingPlayer == b.playerIsStartingPlayer) {
//        System.out.println("Wrong starting player for \n" + b.getBoard() + b.playerIsStartingPlayer +
//          "\n and \n" + child.getBoard() + child.playerIsStartingPlayer);
//        return false;        
//      }
//      descendants += child.descendants;
//      if (!found) {
//        System.out.println("The board\n" + b.getBoard() + 
//          "\n should have as a children the board\n" + next);
//        return false;         
//      }
//      if (!child.fathers.contains(b)) {
//        System.out.println("The board\n" + child.getBoard() + 
//          "\n does not have as a father the following board\n" + board);
//        return false; 
//      }
//      eval = Math.max(eval, -child.eval);
//
//      upper = Math.max(upper, -child.lower);
//      lower = Math.max(lower, -child.upper);
////      bestVariationPlayer = Math.max(bestVariationPlayer, -child.bestVariationOpponent);
////      for (int i = 0; i < Constants.N_SAMPLES; ++i) {
////        samples[i] = (short) Math.max(samples[i], -child.samples[i]);
////      }
//    }
//    if (descendants != b.descendants) {
//      System.out.println("The board " + b + " has " + b.descendants + 
//                         " descendants. Expected " + descendants);
//      return false;
//    }
//    
//    if (Math.abs(b.eval - eval) > 1.E-8) {
//      System.out.println(
//        "Wrong eval bound for the board\n" + b.getBoard() + 
//        "Expected: " + eval + ". Actual: " + b.eval);
//      return false;
//    }
////    if (this.hasEvalGoal() && b.bestVariationPlayer != bestVariationPlayer) {// || b.bestVariationLower != bestVariationLower) {
////      System.out.println("Wrong bestVariationPlayer for the board\n" + b.getBoard() + 
////        "Expected: " + bestVariationPlayer + ". Actual: " + b.bestVariationPlayer + ".");
////      return false;
////    }
////    if (this.hasEvalGoal()) {
////      boolean found = false;
////      for (StoredBoard child : b.children) {
////        if (b.bestVariationOpponent == -child.bestVariationPlayer) {
////          found = true;
////          break;
////        }
////      }
////      if (!found) {
////        System.out.println("Wrong bestVariationOpponent=" + b.bestVariationOpponent + " for board\n" + b +
////            ". Alternatives:\n" + b.children + ". All:\n" + allChildren);
////        return false;
////      }
////    }
//    if (b.lower != lower) {// || b.bestVariationLower != bestVariationLower) {
//      System.out.println("Wrong lower for the board\n" + b.getBoard() + 
//        "Expected: " + lower + ". Actual: " + b.lower + ".");
//      return false;
//    }
//    if (b.upper != upper) {// || b.bestVariationLower != bestVariationLower) {
//      System.out.println("Wrong upper for the board\n" + b.getBoard() + 
//        ". Actual: " + b.upper + ". Expected: " + upper + ".");
//      return false;
//    }
//    for (int i = 0; i < Constants.N_SAMPLES; ++i) {
////      if (b.samples[i] != samples[i]) {
////        System.out.println(
////          "Wrong sample or solved " + i + " for the board\n" + b.getBoard() + 
////          "Expected: " + samples[i] + ". Actual: " + b.samples[i]);
////        return false;
////      }
//    }
//    return true;
//  }
//  
//  public boolean allCorrect() {
//    for (StoredBoard b = first(); b != null; b = next(b.player, b.opponent)) {
//      if (!boardIsCorrect(getFull(b.player, b.opponent)) || !boardChildrenAreCorrect(getFull(b.player, b.opponent))) {
//        return false;
//      }
//    }
//    return true;
//  }
  
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