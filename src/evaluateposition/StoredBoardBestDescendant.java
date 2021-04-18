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
import constants.Constants;
import helpers.Utils;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.HashSet;
import java.util.Objects;
import java.util.PriorityQueue;

/**
 *
 * @author michele
 */
public class StoredBoardBestDescendant implements Comparable<StoredBoardBestDescendant> {

  StoredBoard board;
  boolean greaterEqual;
  ArrayList<StoredBoard> parents = new ArrayList<>();
  float derivativeLoss = 0;
  float proofNumberLoss = 0;

  public float getDerivativeLoss() {
    return derivativeLoss;
  }
  public float getProofNumberLoss() {
    assert proofNumberLoss <= 0;
    return proofNumberLoss;
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
  public boolean equals(Object other) {
    if (!(other instanceof StoredBoardBestDescendant)) {
      return false;
    }
    StoredBoardBestDescendant otherSB = (StoredBoardBestDescendant) other;
    if (getPlayer() != otherSB.getPlayer() || getOpponent() != otherSB.getOpponent() || parents.size() != otherSB.parents.size()) {
      return false;
    }
    for (int i = 0; i < parents.size(); ++i) {
      if (!parents.get(i).equals(otherSB.parents.get(i))) {
        return false;
      }
    }
    return true;
  }

  @Override
  public int hashCode() {
    int hash = 5;
    hash = 11 * hash + Objects.hashCode(this.board);
    hash = 11 * hash + Objects.hashCode(this.parents);
    return hash;
  }

  @Override
  public int compareTo(StoredBoardBestDescendant other) {
    return Comparator
        .comparingDouble(StoredBoardBestDescendant::getDerivativeLoss)
        .thenComparingDouble(StoredBoardBestDescendant::getProofNumberLoss).reversed()
        .thenComparingInt(StoredBoardBestDescendant::getNEmpties)
        .thenComparingLong(StoredBoardBestDescendant::getPlayer)
        .thenComparingLong(StoredBoardBestDescendant::getOpponent)
        .compare(this, other);
  }
  
  @Override
  public String toString() {
    return greaterEqual ? "greaterEqual" : "strictlyGreater" + " (" + getDerivativeLoss() + "|" + getProofNumberLoss() + ") " + board.toString().replace("\n", "");
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
  
  protected StoredBoardBestDescendant(StoredBoard board, boolean greaterEqual, float derivativeLoss, float proofNumberLoss) {
    this.board = board;
    this.greaterEqual = greaterEqual;
    this.derivativeLoss = derivativeLoss;
    assert proofNumberLoss <= 0;
    this.proofNumberLoss = proofNumberLoss;
  }
  private StoredBoardBestDescendant(StoredBoardBestDescendant other) {
    board = other.board;
    greaterEqual = other.greaterEqual;
    parents = new ArrayList<>(other.parents);
    derivativeLoss = other.derivativeLoss;
    proofNumberLoss = other.proofNumberLoss;
  }
  public StoredBoardBestDescendant(StoredBoard board, boolean greaterEqual) {
    this(board, greaterEqual, firstDerivativeLoss(board, greaterEqual), 0);
  }
  
  private static float firstDerivativeLoss(StoredBoard board, boolean greaterEqual) {
    return (greaterEqual ? board.maxLogDerivativeProbGreaterEqual : board.maxLogDerivativeProbStrictlyGreater);
  }
  
  private StoredBoardBestDescendant copyToChild(StoredBoard child) {
    assert child == null || Utils.arrayContains(board.children, child);
    StoredBoardBestDescendant copy = new StoredBoardBestDescendant(this);
    copy.toChild(child);
    return copy;
  }
  
  private void toChild(StoredBoard child) {
    assert Utils.arrayContains(board.children, child);
    parents.add(board);
    Float childLogDerivative = childLogDerivative(child);
    if (greaterEqual) {
      if (derivativeLoss + childLogDerivative == Float.NEGATIVE_INFINITY) {
        derivativeLoss = Float.NEGATIVE_INFINITY;
        // No extra proof number loss.
      } else {
        derivativeLoss = derivativeLoss - maxLogDerivative() + childLogDerivative;
      }
    } else {
      if (derivativeLoss + childLogDerivative == Float.NEGATIVE_INFINITY) {
        derivativeLoss = Float.NEGATIVE_INFINITY;
        proofNumberLoss = proofNumberLoss + child.proofNumberGreaterEqual - board.maxFiniteChildrenProofNumber();  // TODO IMPROVE!!!
      } else {
        derivativeLoss = derivativeLoss - maxLogDerivative() + childLogDerivative;
        // proofNumberLoss unchanged.
      }
    }
    greaterEqual = !greaterEqual;
    board = child;
    assert hasValidDescendants();
  }
  
  public boolean hasValidDescendants() {
    return hasValidDescendants(board, greaterEqual);
  }

  public static boolean hasValidDescendants(StoredBoard b, boolean greaterEqual) {
    if (greaterEqual) {
      if (b.probGreaterEqual == 0 || b.proofNumberGreaterEqual == 0 || b.proofNumberGreaterEqual == Float.POSITIVE_INFINITY) {
        return false;
      }
    } else {
      if (b.disproofNumberStrictlyGreater == 0 || b.disproofNumberStrictlyGreater == Float.POSITIVE_INFINITY || b.probStrictlyGreater == 1) {
        return false;
      } 
    }
    return true;
  } 

  public static StoredBoardBestDescendant bestDescendant(
      StoredBoard father, boolean greaterEqual) {
    StoredBoardBestDescendant result = new StoredBoardBestDescendant(father, greaterEqual);
    if (!result.hasValidDescendants()) {
      return null;
    }

    while (result.board != null && !result.board.isLeaf()) {
      StoredBoard bestChild = result.bestChild();
      result.toChild(bestChild);
      assert result.derivativeLoss == 0 || result.derivativeLoss == Float.NEGATIVE_INFINITY;
    }
    return result;
  }

  public static StoredBoardBestDescendant randomDescendant(
      StoredBoard father, boolean greaterEqual) {
    StoredBoardBestDescendant result = new StoredBoardBestDescendant(father, greaterEqual);

    while (result.board != null && !result.board.isLeaf()) {
      result.toChild(randomChild(result.board, result.greaterEqual));
    }
    return result;
  }

  public static StoredBoardBestDescendant fixedDescendant(
      StoredBoard father, boolean greaterEqual, String sequence) {
    StoredBoardBestDescendant result = new StoredBoardBestDescendant(father, greaterEqual);

    for (int i = 0; i < sequence.length(); i += 2) {
      result.toChild(fixedChild(result.board, sequence.substring(i, i+2)));
    }
    return result;
  }
  
  private float childLogDerivative(StoredBoard child) {
    return greaterEqual ? board.logDerivativeProbGreaterEqual(child) : board.logDerivativeProbStrictlyGreater(child);
  }
  private float maxLogDerivative() {
    return greaterEqual ? board.maxLogDerivativeProbGreaterEqual : board.maxLogDerivativeProbStrictlyGreater;
  }
  private static float prob(StoredBoard board, boolean greaterEqual) {
    return greaterEqual ? board.probGreaterEqual : board.probStrictlyGreater;
  }

  private static boolean endgame(StoredBoard board, boolean greaterEqual) {
    return greaterEqual ? board.probGreaterEqual == 1 : board.probStrictlyGreater == 0;
  }


  public static ArrayList<StoredBoardBestDescendant> bestDescendants(StoredBoard father, int n) {
    PriorityQueue<StoredBoardBestDescendant> toProcess = new PriorityQueue<>();
    ArrayList<StoredBoardBestDescendant> result = new ArrayList<>();
    StoredBoardBestDescendant first = new StoredBoardBestDescendant(father, true);
    StoredBoardBestDescendant second = new StoredBoardBestDescendant(father, false);

    if (first.hasValidDescendants()) {
      toProcess.add(first);
    }
    if (second.hasValidDescendants()) {
      toProcess.add(second);
    }
    HashSet<StoredBoard> visited = new HashSet<>();

    while (result.size() < n && !toProcess.isEmpty()) {
      StoredBoardBestDescendant next = toProcess.poll();
      StoredBoard nextBoard = next.board;
//      System.out.println(next.derivativeLoss);
      if (result.size() > 10 //&& father.probGreaterEqual > 0.1 && father.probStrictlyGreater < 0.1
          && result.get(0).derivativeLoss != Float.NEGATIVE_INFINITY && next.derivativeLoss - result.get(0).derivativeLoss < -3) {
//        System.out.println("HI" + result.size() + " " + result.get(0).derivativeLoss + " " + next.derivativeLoss);
        break;
      }
//      nextBoard.fathers.get(0);
      if (visited.contains(nextBoard)) {
        continue;
      }
      visited.add(nextBoard);
      if (nextBoard.isLeaf()) {
        result.add(next);
        continue;
      }
      if (next.greaterEqual) {
        if (next.board.probGreaterEqual > 0.97 || next.derivativeLoss == Float.NEGATIVE_INFINITY) {
          toProcess.add(next.copyToChild(next.bestChild()));
          continue;
        }
      }
      for (StoredBoard child : nextBoard.children) {
        if (hasValidDescendants(child, !next.greaterEqual) && (endgame(next.board, next.greaterEqual) == endgame(child, !next.greaterEqual))) {
          toProcess.add(next.copyToChild(child));
        }
      }
    }
    return result;
  }
  
  public StoredBoard bestChild() {
    if (greaterEqual) {
      if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {
        if (board.proofNumberGreaterEqual == 0 || board.proofNumberGreaterEqual == Float.POSITIVE_INFINITY) {
          return board.bestChildProofGreaterEqual();
        }
      }
      if (maxLogDerivative() + derivativeLoss == Float.NEGATIVE_INFINITY) {
        return board.bestChildEndgameGreaterEqual();
      } else {
        return board.bestChildMidgameGreaterEqual();        
      }
    } else {
      if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {
        if (board.disproofNumberStrictlyGreater == Float.POSITIVE_INFINITY || board.disproofNumberStrictlyGreater == 0) {
          return board.bestChildProofStrictlyGreater();
        }
      }
      if (maxLogDerivative() + derivativeLoss == Float.NEGATIVE_INFINITY) {
        return board.bestChildEndgameStrictlyGreater();
      } else {
        return board.bestChildMidgameStrictlyGreater();        
      }
    }
  }

  public static StoredBoard randomChild(StoredBoard father, boolean greaterEqual) {
    assert hasValidDescendants(father, greaterEqual);
    ArrayList<StoredBoard> validChildren = new ArrayList<>();
    
    for (StoredBoard child : father.getChildren()) {
      if (hasValidDescendants(child, !greaterEqual)) {
        validChildren.add(child);
      }
    }
    return validChildren.get((int) (Math.random() * validChildren.size()));
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