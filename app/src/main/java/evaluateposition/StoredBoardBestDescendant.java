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

  StoredBoard.Evaluation eval;
  ArrayList<StoredBoard.Evaluation> parents = new ArrayList<>();
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
    return eval.getStoredBoard().nEmpties;
  }
  public long getPlayer() {
    return eval.getStoredBoard().getPlayer();
  }
  public long getOpponent() {
    return eval.getStoredBoard().getOpponent();
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
    hash = 11 * hash + Objects.hashCode(this.eval);
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
    return " (" + getDerivativeLoss() + "|" + getProofNumberLoss() + ") " + eval.toString().replace("\n", "");
  }

  public void updateDescendants(long newDescendants) {
    assert eval != null;
    eval.addDescendants(newDescendants);
    for (StoredBoard.Evaluation parent : parents) {
      parent.addDescendants(newDescendants);
    }
  }
  
  protected StoredBoardBestDescendant(StoredBoard.Evaluation eval, int derivativeLoss, float proofNumberLoss) {
    this.eval = eval;
    this.derivativeLoss = derivativeLoss;
    assert proofNumberLoss <= 0;
    this.proofNumberLoss = proofNumberLoss;
  }
  private StoredBoardBestDescendant(StoredBoardBestDescendant other) {
    eval = other.eval;
    parents = new ArrayList<>(other.parents);
    derivativeLoss = other.derivativeLoss;
    proofNumberLoss = other.proofNumberLoss;
  }
  public StoredBoardBestDescendant(StoredBoard.Evaluation eval) {
    this(eval, firstDerivativeLoss(eval), 0);
  }
  
  private static int firstDerivativeLoss(StoredBoard.Evaluation eval) {
    return Math.max(LOG_DERIVATIVE_MINUS_INF, eval.maxLogDerivative());
  }
  
  private StoredBoardBestDescendant copyToChild(StoredBoard.Evaluation child) {
    assert child == null || Utils.arrayContains(eval.getStoredBoard().children, child.getStoredBoard());
    StoredBoardBestDescendant copy = new StoredBoardBestDescendant(this);
    copy.toChild(child);
    return copy;
  }
  
  private void toChild(StoredBoard.Evaluation child) {
    assert Utils.arrayContains(eval.getStoredBoard().children, child.getStoredBoard());
    parents.add(eval);
    int childLogDerivative = child.maxLogDerivative();
    if (derivativeLoss + childLogDerivative <= LOG_DERIVATIVE_MINUS_INF) {
      derivativeLoss = LOG_DERIVATIVE_MINUS_INF;
      proofNumberLoss = proofNumberLoss + child.proofNumber;// - eval.maxFiniteChildrenProofNumber(evalGoal);
      // No extra proof number loss.
    } else {
      derivativeLoss = derivativeLoss - maxLogDerivative() + childLogDerivative;
    }
    eval = child;
    assert hasValidDescendants();
  }
  
  public boolean hasValidDescendants() {
    return hasValidDescendants(eval);
  }

  public static boolean hasValidDescendants(StoredBoard.Evaluation eval) {
    return (eval.proofNumber != 0 && eval.disproofNumber != 0);
  }

  public static StoredBoardBestDescendant bestDescendant(
      StoredBoard.Evaluation father) {
    StoredBoardBestDescendant result = new StoredBoardBestDescendant(father);
    if (!result.hasValidDescendants() || father.isBusy) {
      return null;
    }

    while (!result.eval.isLeaf) {
      StoredBoard.Evaluation bestChild = result.bestChild();
      assert bestChild != null;
      assert !bestChild.isBusy;
      result.toChild(bestChild);
    }
    assert result.eval != null;
    return result;
  }

  public static StoredBoardBestDescendant randomDescendant(
      StoredBoard.Evaluation father) {
    StoredBoardBestDescendant result = new StoredBoardBestDescendant(father);
    if (!result.hasValidDescendants()) {
      return null;
    }

    while (result.eval != null && !result.eval.isLeaf) {
      result.toChild(randomChild(result.eval));
    }
    return result;
  }

  public static StoredBoardBestDescendant fixedDescendant(
      StoredBoard.Evaluation eval, String sequence) {
    StoredBoardBestDescendant result = new StoredBoardBestDescendant(eval);

    for (int i = 0; i < sequence.length(); i += 2) {
      result.toChild(fixedChild(eval.getStoredBoard(), sequence.substring(i, i+2)).getEvaluation(-eval.evalGoal));
    }
    return result;
  }
  
  private int childLogDerivative(StoredBoard.Evaluation child) {
    return eval.childLogDerivative(child);
  }
  private int maxLogDerivative() {
    return eval.maxLogDerivative();
  }

  private static boolean endgame(StoredBoard board, int evalGoal) {
    return board.getProb(evalGoal) == 1 || board.getProb(evalGoal) == 0;
  }


//  public static ArrayList<StoredBoardBestDescendant> bestDescendants(StoredBoard.Evaluation father, int n) {
//    PriorityQueue<StoredBoardBestDescendant> toProcess = new PriorityQueue<>();
//    ArrayList<StoredBoardBestDescendant> result = new ArrayList<>();
//    StoredBoardBestDescendant greaterEqual = new StoredBoardBestDescendant(father);
//
//    if (greaterEqual.hasValidDescendants()) {
//      toProcess.add(greaterEqual);
//    }
//    HashSet<StoredBoard.Evaluation> visited = new HashSet<>();
//
//    while (result.size() < n && !toProcess.isEmpty()) {
//      StoredBoardBestDescendant next = toProcess.poll();
//      StoredBoard.Evaluation nextBoard = next.eval;
////      System.out.println(next.derivativeLoss);
//      if (result.size() > 10
//          && result.get(0).derivativeLoss != Float.NEGATIVE_INFINITY && next.derivativeLoss - result.get(0).derivativeLoss < -3) {
////        System.out.println("HI" + result.size() + " " + result.get(0).derivativeLoss + " " + next.derivativeLoss);
//        break;
//      }
////      nextBoard.fathers.get(0);
//      if (visited.contains(nextBoard)) {
//        continue;
//      }
//      visited.add(nextBoard);
//      if (nextBoard.isLeaf) {
//        result.add(next);
//        continue;
//      }
////      if (next.greaterEqual) {
//        if (next.eval.getProb() > 0.97 || next.derivativeLoss == Float.NEGATIVE_INFINITY) {
//          toProcess.add(next.copyToChild(next.bestChild()));
//          continue;
//        }
////      }
//      for (StoredBoard child : nextBoard.getStoredBoard().children) {
//        int evalGoal = 0; // TODO.
//        if (hasValidDescendants(child, -evalGoal) && (endgame(next.eval, evalGoal) == endgame(child, -evalGoal))) {
//          toProcess.add(next.copyToChild(child));
//        }
//      }
//    }
//    return result;
//  }
  
  public StoredBoard.Evaluation bestChild() {
    return eval.bestChild();
  }

  public static StoredBoard.Evaluation randomChild(StoredBoard.Evaluation father) {
    assert hasValidDescendants(father);
    ArrayList<StoredBoard.Evaluation> validChildren = new ArrayList<>();
    
    for (StoredBoard child : father.getStoredBoard().getChildren()) {
      if (hasValidDescendants(child.getEvaluation(-father.evalGoal))) {
        validChildren.add(child.getEvaluation(-father.evalGoal));
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