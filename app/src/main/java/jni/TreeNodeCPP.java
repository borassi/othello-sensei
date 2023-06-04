// Copyright 2023 Michele Borassi
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
import constants.Constants;

public class TreeNodeCPP implements AutoCloseable {
  private final long nodeAddress;
  public TreeNodeCPP(long nodeAddress) {
    this.nodeAddress = nodeAddress;
  }

  public long getNodeAddress() { return nodeAddress; }
  public native long getDescendants();

  public native boolean isSolved();

  public native int getEval();

  public native int getPercentileLower(float prob);

  public native int getPercentileUpper(float prob);

  public native int getLower();

  public native int getUpper();

  public native int getWeakLower();

  public native int getWeakUpper();

  public native float proofNumber(int evalGoal);

  public native float disproofNumber(int evalGoal);
  public native double solveProbabilityLower(int lower);

  public native double solveProbabilityUpper(int upper);

  public native float getProb(int evalGoal);

  public native int maxLogDerivative(int evalGoal);

  public native int childLogDerivative(TreeNodeCPP child, int evalGoal);

  public native long getPlayer();
  public native long getOpponent();

  public Board getBoard() {
    return new Board(getPlayer(), getOpponent());
  }

  public native void close();

  public String getLines() {
    int lower = getPercentileLower(Constants.PROB_INCREASE_WEAK_EVAL);
    int upper = getPercentileUpper(Constants.PROB_INCREASE_WEAK_EVAL);
    String lines;
    String evalFormatter;
    if (isSolved()) {
      evalFormatter = "%+.0f";
    } else {
      evalFormatter = "%+.2f";
    }
    return
        String.format(evalFormatter, -getEval() / 100.0) + "\n" +
        JNI.prettyPrintDouble(getDescendants()) + "\n" + (
            lower == upper ?
                JNI.prettyPrintDouble(proofNumber(lower)) + " " +
                    JNI.prettyPrintDouble(disproofNumber(lower)) :
                ("[" + (-upper/100) + ", " + (-lower/100) + "]")
        );
  }
}
