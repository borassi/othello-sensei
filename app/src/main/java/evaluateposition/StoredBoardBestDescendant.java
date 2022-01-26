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

import static evaluateposition.StoredBoard.LOG_DERIVATIVE_MINUS_INF;

import androidx.annotation.NonNull;

import java.util.ArrayList;
import java.util.Comparator;
import java.util.Objects;

public class StoredBoardBestDescendant implements Comparable<StoredBoardBestDescendant> {

  StoredBoard.Evaluation eval;
  ArrayList<StoredBoard.Evaluation> parents = new ArrayList<>();
  int derivativeLoss;
  float proofNumberLoss;
  int alpha;
  int beta;

  protected StoredBoardBestDescendant(
      StoredBoard.Evaluation eval,
      ArrayList<StoredBoard.Evaluation> parents,
      int alpha,
      int beta) {
    this.eval = eval;
    this.parents = parents;
    this.alpha = alpha;
    this.beta = beta;
    this.derivativeLoss = 0;
    this.proofNumberLoss = 0;
  }

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
    for (StoredBoard.Evaluation parent : parents) {
      parent.addDescendants(newDescendants);
      parent.getStoredBoard().decreaseNThreadsWorking();
    }
  }
  
  private static int firstDerivativeLoss(StoredBoard.Evaluation eval) {
    return Math.max(LOG_DERIVATIVE_MINUS_INF, eval.maxLogDerivative());
  }

}