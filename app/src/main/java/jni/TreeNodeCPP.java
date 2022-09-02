// Copyright 2022 Google LLC
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
package jni;

import board.Board;
import evaluateposition.TreeNodeInterface;

public class TreeNodeCPP extends TreeNodeInterface {
  private final long nodeAddress;
  public TreeNodeCPP(long nodeAddress) {
    this.nodeAddress = nodeAddress;
  }

  public long getNodeAddress() { return nodeAddress; }
  @Override
  public native long getDescendants();

  @Override
  public native int getEval();

  @Override
  public native int getPercentileLower(float prob);

  @Override
  public native int getPercentileUpper(float prob);

  @Override
  public native int getLower();

  @Override
  public native int getUpper();

  @Override
  public native int getWeakLower();

  @Override
  public native int getWeakUpper();

  @Override
  public native float proofNumber(int evalGoal);

  @Override
  public native float disproofNumber(int evalGoal);

  @Override
  public native float getProb(int evalGoal);

  @Override
  public native int maxLogDerivative(int evalGoal);

  @Override
  public int childLogDerivative(TreeNodeInterface child, int evalGoal) {
    return childLogDerivative((TreeNodeCPP) child, evalGoal);
  }

  public native int childLogDerivative(TreeNodeCPP child, int evalGoal);

  public native long getPlayer();
  public native long getOpponent();

  @Override
  public Board getBoard() {
    return new Board(getPlayer(), getOpponent());
  }
}
