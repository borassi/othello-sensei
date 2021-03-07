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

import bitpattern.PositionIJ;
import board.Board;
import constants.Constants;
import helpers.Utils;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.HashSet;
import java.util.PriorityQueue;

/**
 *
 * @author michele
 */
public class StoredBoardBestDescendant implements Comparable<StoredBoardBestDescendant> {

  StoredBoard board;
  boolean greaterEqual;
  boolean endgame;
  ArrayList<StoredBoard> parents = new ArrayList<>();
  double totalLogDerivative = 0;

  public double getTotalLogDerivative() {
    return totalLogDerivative;
  }
  public int getNEmpties() {
    return board.nEmpties;
  }
  public long getPlayer() {
    return board.getPlayer();
  }
  public long getOpponent() {
    return board.getOpponent();
  }

  @Override
  public int compareTo(StoredBoardBestDescendant other) {
    return Comparator
        .comparingDouble(StoredBoardBestDescendant::getTotalLogDerivative).reversed()
        .thenComparingInt(StoredBoardBestDescendant::getNEmpties)
        .thenComparingLong(StoredBoardBestDescendant::getPlayer)
        .thenComparingLong(StoredBoardBestDescendant::getOpponent)
        .compare(this, other);
  }
  
  @Override
  public String toString() {
    return getTotalLogDerivative() + " " + board.toString().replace("\n", "");
  }

  public int getAlpha() {
    if (greaterEqual) {
      return board.getEvalGoal() - 100;
    } else {
      return board.getEvalGoal() + 100;
    }
  }

  public int getBeta() {
    if (greaterEqual) {
      return board.getEvalGoal() - 100;
    } else {
      return board.getEvalGoal() + 100;
    }
  }

  public void updateDescendants(long newDescendants) {
    assert board != null;
    board.addDescendants(newDescendants);
    for (StoredBoard parent : parents) {
      parent.addDescendants(newDescendants);
    }
  }
  
  private StoredBoardBestDescendant(StoredBoard board, boolean greaterEqual, boolean endgame, double totalLogDerivative) {
    this.board = board;
    this.greaterEqual = greaterEqual;
    this.endgame = endgame;
    this.totalLogDerivative = totalLogDerivative;
  }
  private StoredBoardBestDescendant(StoredBoardBestDescendant other) {
    board = other.board;
    greaterEqual = other.greaterEqual;
    endgame = other.endgame;
    parents = new ArrayList<>(other.parents);
    totalLogDerivative = other.totalLogDerivative;
  }
  private StoredBoardBestDescendant(StoredBoard board, boolean greaterEqual, boolean endgame) {
    this(board, greaterEqual, endgame, 0);
    this.totalLogDerivative = maxLogDerivative();
  }
  
  private StoredBoardBestDescendant copyToChild(StoredBoard child) {
    assert child == null || Utils.arrayContains(board.children, child);
    StoredBoardBestDescendant copy = new StoredBoardBestDescendant(this);
    copy.toChild(child);
    copy.totalLogDerivative = totalLogDerivative - maxLogDerivative() + childLogDerivative(child);
    return copy;
  }
  
  private void toChild(StoredBoard child) {
    assert child == null || Utils.arrayContains(board.children, child);
    parents.add(board);
    greaterEqual = !greaterEqual;
    board = child;
  }

  public static StoredBoardBestDescendant bestDescendant(
      StoredBoard father, boolean greaterEqual, boolean endgame) {
    StoredBoardBestDescendant result = new StoredBoardBestDescendant(father, greaterEqual, endgame);

    while (result.board != null && !result.board.isLeaf()) {
      result.toChild(bestChild(
          result.board,
          result.greaterEqual,
          result.endgame,
          false));
    }
    if (result.board == null
        || (result.greaterEqual && result.board.maxLogDerivativeProbGreaterEqual == Double.NEGATIVE_INFINITY)
        || (!result.greaterEqual && result.board.maxLogDerivativeProbStrictlyGreater == Double.NEGATIVE_INFINITY)) {
      return null;
    }
    return result;
  }

  public static StoredBoardBestDescendant randomDescendant(
      StoredBoard father, boolean greaterEqual) {
    StoredBoardBestDescendant result = new StoredBoardBestDescendant(father, greaterEqual, false);

    while (result.board != null && !result.board.isLeaf()) {
      result.toChild(randomChild(result.board));
    }
    return result;
  }

  public static StoredBoardBestDescendant fixedDescendant(
      StoredBoard father, boolean greaterEqual, String sequence) {
    StoredBoardBestDescendant result = new StoredBoardBestDescendant(father, greaterEqual, false);

    for (int i = 0; i < sequence.length(); i += 2) {
      result.toChild(fixedChild(result.board, sequence.substring(i, i+2)));
    }
    return result;
  }
  
  private double childLogDerivative(StoredBoard child) {
    return greaterEqual ? board.logDerivativeProbGreaterEqual(child) : board.logDerivativeProbStrictlyGreater(child);
  }
  private double maxLogDerivative() {
    return greaterEqual ? board.maxLogDerivativeProbGreaterEqual : board.maxLogDerivativeProbStrictlyGreater;
  }

  public static ArrayList<StoredBoardBestDescendant> bestDescendants(StoredBoard father, int n) {
    PriorityQueue<StoredBoardBestDescendant> toProcess = new PriorityQueue<>();
    ArrayList<StoredBoardBestDescendant> result = new ArrayList<>();
    toProcess.add(new StoredBoardBestDescendant(father, true, false));
    toProcess.add(new StoredBoardBestDescendant(father, false, false));
    HashSet<StoredBoard> visitedGreaterEqual = new HashSet<>();
    HashSet<StoredBoard> visitedStrictlyGreater = new HashSet<>();

    while (result.size() < n && !toProcess.isEmpty()) {
      StoredBoardBestDescendant next = toProcess.poll();
      StoredBoard nextBoard = next.board;
      if (next.maxLogDerivative() == Double.NEGATIVE_INFINITY) {
        return result;
      }
      if ((next.greaterEqual && visitedGreaterEqual.contains(nextBoard)) ||
          (!next.greaterEqual && visitedStrictlyGreater.contains(nextBoard))) {
        continue;
      }
      if (nextBoard.isLeaf()) {
        result.add(next);
        visitedGreaterEqual.add(nextBoard);
        visitedStrictlyGreater.add(nextBoard);
        continue;
      }
      (next.greaterEqual ? visitedGreaterEqual : visitedStrictlyGreater).add(nextBoard);
      for (StoredBoard child : next.board.children) {
        double childLoss = next.totalLogDerivative - next.maxLogDerivative() + next.childLogDerivative(child);
        assert childLoss <= next.totalLogDerivative + 1E-12;
        toProcess.add(next.copyToChild(child));
      }
    }
    return result;
  }
  
  public static StoredBoard bestChild(StoredBoard father, boolean greaterEqual, boolean endgame, boolean verbose) {
    if (greaterEqual) {
      if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {
        if (father.proofNumberGreaterEqual == 0 || father.proofNumberGreaterEqual == Double.POSITIVE_INFINITY) {
          return father.bestChildProofGreaterEqual();
        }
      }
      if (endgame) {
        if (verbose) System.out.println("Endgame!" + father.probGreaterEqual);
        return father.bestChildEndgameGreaterEqual();
      } else {
        if (verbose) System.out.println("Midgame!" + father.probGreaterEqual);
        return father.bestChildMidgameGreaterEqual();
      }
    } else {
      if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {
        if (father.disproofNumberStrictlyGreater == Double.POSITIVE_INFINITY || father.disproofNumberStrictlyGreater == 0) {
          return father.bestChildProofStrictlyGreater();
        }
      }
      if (endgame) {
        if (verbose) System.out.println("Endgame!");
        return father.bestChildEndgameStrictlyGreater();
      } else {
        if (verbose) System.out.println("Midgame!");
        return father.bestChildMidgameStrictlyGreater();
      }
    }
  }

  public static StoredBoard randomChild(StoredBoard father) {
    StoredBoard[] children = father.getChildren();
    return children[(int) (Math.random() * children.length)];
  }

  public static StoredBoard fixedChild(StoredBoard father, String move) {
    Board childBoard = father.getBoard().move(move);
    for (StoredBoard child : father.getChildren()) {
      if (child.getPlayer() == childBoard.getPlayer() && child.getOpponent() == childBoard.getOpponent()) {
        return child;
      }
    }
    throw new AssertionError("Move " + move + " cannot be done in board " + father);
  }
}