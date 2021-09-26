// Copyright 2021 Google LLC
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
import static evaluateposition.StoredBoard.LOG_DERIVATIVE_MINUS_INF;

import androidx.annotation.NonNull;

import helpers.Utils;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.HashSet;
import java.util.Objects;
import java.util.PriorityQueue;

public class StoredBoardBestDescendant implements Comparable<StoredBoardBestDescendant> {

  StoredBoard board;
  int evalGoal;
  ArrayList<StoredBoard> parents = new ArrayList<>();
  int derivativeLoss;
  float proofNumberLoss;

  public int getDerivativeLoss() {
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

  @NonNull
  @Override
  public String toString() {
    return "goal: " + evalGoal + " (" + getDerivativeLoss() + "|" + getProofNumberLoss() + ") " + board.toString().replace("\n", "");
  }

  public int getAlpha() {
    return evalGoal;
  }

  public int getBeta() {
    return evalGoal;
  }

  public void updateDescendants(long newDescendants) {
    assert board != null;
    board.addDescendants(newDescendants);
    for (StoredBoard parent : parents) {
      parent.addDescendants(newDescendants);
    }
  }
  
  protected StoredBoardBestDescendant(StoredBoard board, int evalGoal, int derivativeLoss, float proofNumberLoss) {
    this.board = board;
    this.evalGoal = evalGoal;
    this.derivativeLoss = derivativeLoss;
    assert proofNumberLoss <= 0;
    this.proofNumberLoss = proofNumberLoss;
  }
  private StoredBoardBestDescendant(StoredBoardBestDescendant other) {
    board = other.board;
    evalGoal = other.evalGoal;
    parents = new ArrayList<>(other.parents);
    derivativeLoss = other.derivativeLoss;
    proofNumberLoss = other.proofNumberLoss;
  }
  public StoredBoardBestDescendant(StoredBoard board, int evalGoal) {
    this(board, evalGoal, firstDerivativeLoss(board, evalGoal), 0);
  }
  
  private static int firstDerivativeLoss(StoredBoard board, int evalGoal) {
    return Math.max(LOG_DERIVATIVE_MINUS_INF, board.maxLogDerivative(evalGoal));
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
    int childLogDerivative = childLogDerivative(child, evalGoal);
    if (derivativeLoss + childLogDerivative <= LOG_DERIVATIVE_MINUS_INF) {
      derivativeLoss = LOG_DERIVATIVE_MINUS_INF;
      proofNumberLoss = proofNumberLoss + child.getProofNumber(-evalGoal) - board.maxFiniteChildrenProofNumber(evalGoal);
      // No extra proof number loss.
    } else {
      derivativeLoss = derivativeLoss - maxLogDerivative() + childLogDerivative;
    }
    evalGoal = -evalGoal;
    board = child;
    assert hasValidDescendants();
  }
  
  public boolean hasValidDescendants() {
    return hasValidDescendants(board, evalGoal);
  }

  public static boolean hasValidDescendants(StoredBoard b, int evalGoal) {
    return (b.getProofNumber(evalGoal) != 0 && b.getDisproofNumber(evalGoal) != 0);
  }

  public static StoredBoardBestDescendant bestDescendant(
      StoredBoard father, int evalGoal) {
    StoredBoardBestDescendant result = new StoredBoardBestDescendant(father, evalGoal);
    if (!result.hasValidDescendants() || father.isBusy) {
      return null;
    }

    while (!result.board.isLeaf()) {
      StoredBoard bestChild = result.bestChild();
      assert bestChild != null;
      assert !bestChild.isBusy;
      result.toChild(bestChild);
    }
    assert result.board != null;
    return result;
  }

  public static StoredBoardBestDescendant randomDescendant(
      StoredBoard father, int evalGoal) {
    StoredBoardBestDescendant result = new StoredBoardBestDescendant(father, evalGoal);
    if (!result.hasValidDescendants()) {
      return null;
    }

    while (result.board != null && !result.board.isLeaf()) {
      result.toChild(randomChild(result.board, result.evalGoal));
    }
    return result;
  }

  public static StoredBoardBestDescendant fixedDescendant(
      StoredBoard father, int evalGoal, String sequence) {
    StoredBoardBestDescendant result = new StoredBoardBestDescendant(father, evalGoal);

    for (int i = 0; i < sequence.length(); i += 2) {
      result.toChild(fixedChild(result.board, sequence.substring(i, i+2)));
    }
    return result;
  }
  
  private int childLogDerivative(StoredBoard child, int evalGoal) {
    return board.childLogDerivative(child, evalGoal);
  }
  private int maxLogDerivative() {
    return board.maxLogDerivative(evalGoal);
  }

  private static boolean endgame(StoredBoard board, int evalGoal) {
    return board.getProb(evalGoal) == 1 || board.getProb(evalGoal) == 0;
  }


  public static ArrayList<StoredBoardBestDescendant> bestDescendants(StoredBoard father, int n, int evalGoal) {
    PriorityQueue<StoredBoardBestDescendant> toProcess = new PriorityQueue<>();
    ArrayList<StoredBoardBestDescendant> result = new ArrayList<>();
    StoredBoardBestDescendant greaterEqual = new StoredBoardBestDescendant(father, evalGoal);

    if (greaterEqual.hasValidDescendants()) {
      toProcess.add(greaterEqual);
    }
    HashSet<StoredBoard> visited = new HashSet<>();

    while (result.size() < n && !toProcess.isEmpty()) {
      StoredBoardBestDescendant next = toProcess.poll();
      StoredBoard nextBoard = next.board;
//      System.out.println(next.derivativeLoss);
      if (result.size() > 10
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
//      if (next.greaterEqual) {
        if (next.board.getProb(evalGoal) > 0.97 || next.derivativeLoss == Float.NEGATIVE_INFINITY) {
          toProcess.add(next.copyToChild(next.bestChild()));
          continue;
        }
//      }
      for (StoredBoard child : nextBoard.children) {
        if (hasValidDescendants(child, -evalGoal) && (endgame(next.board, evalGoal) == endgame(child, -evalGoal))) {
          toProcess.add(next.copyToChild(child));
        }
      }
    }
    return result;
  }
  
  public StoredBoard bestChild() {
    return board.bestChild(evalGoal);
//    if (greaterEqual) {
//      if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {
//        if (board.getProofNumberGreaterEqual() == 0 || board.getProofNumberGreaterEqual() == Float.POSITIVE_INFINITY) {
//          return board.bestChildProofGreaterEqual();
//        }
//      }
//      return board.bestChildGreaterEqual();
//    } else {
//      if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {
//        if (board.getDisproofNumberStrictlyGreater() <= LOG_DERIVATIVE_MINUS_INF || board.getDisproofNumberStrictlyGreater() == 0) {
//          return board.bestChildProofStrictlyGreater();
//        }
//      }
//      return board.bestChildStrictlyGreater();
//    }
  }

  public static StoredBoard randomChild(StoredBoard father, int evalGoal) {
    assert hasValidDescendants(father, evalGoal);
    ArrayList<StoredBoard> validChildren = new ArrayList<>();
    
    for (StoredBoard child : father.getChildren()) {
      if (hasValidDescendants(child, -evalGoal)) {
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