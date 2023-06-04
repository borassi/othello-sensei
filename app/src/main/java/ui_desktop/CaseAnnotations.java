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
package ui_desktop;

import constants.Constants;
import jni.JNI;
import jni.TreeNodeCPP;

public class CaseAnnotations {
  public final TreeNodeCPP father;
  public final TreeNodeCPP treeNode;
  public final boolean isBestMove;
  public final int thorGames;

  public CaseAnnotations(TreeNodeCPP father, TreeNodeCPP treeNode, boolean isBestMove) {
    this(father, treeNode, -1, isBestMove);
  }

  public CaseAnnotations(TreeNodeCPP father, TreeNodeCPP treeNode, int thorGames, boolean isBestMove) {
    this.treeNode = treeNode;
    this.isBestMove = isBestMove;
    this.thorGames = thorGames;
    this.father = father;
  }

  public String evalLine() {
    String evalFormatter;
    if (treeNode.isSolved()) {
      evalFormatter = "%+.0f";
    } else {
      evalFormatter = "%+.2f";
    }
    return String.format(evalFormatter, -treeNode.getEval() / 100.0);
  }

  public String proofDisproofNumberLine() {
    int lower = treeNode.getPercentileLower(Constants.PROB_INCREASE_WEAK_EVAL) - 100;
    int upper = treeNode.getPercentileUpper(Constants.PROB_INCREASE_WEAK_EVAL) + 100;
    return
        JNI.prettyPrintDouble(treeNode.proofNumber(lower - 100)) + " " +
        JNI.prettyPrintDouble(treeNode.disproofNumber(upper + 100));
  }

  public String solveProbabilityLine() {
    return
        JNI.prettyPrintDouble(treeNode.solveProbabilityLower(-6300)) + " " +
        JNI.prettyPrintDouble(treeNode.solveProbabilityUpper(6300));
  }

  public String getLines() {
    int lower = treeNode.getPercentileLower(Constants.PROB_INCREASE_WEAK_EVAL) - 100;
    int upper = treeNode.getPercentileUpper(Constants.PROB_INCREASE_WEAK_EVAL) + 100;
    String rows;
    if (thorGames < 0) {
      rows = evalLine() + "\n"
          + JNI.prettyPrintDouble(treeNode.getDescendants()) + "\n";
      if (lower == upper) {
        rows += proofDisproofNumberLine() + "\n";
        rows += solveProbabilityLine();
      } else {
        rows += "[" + (-upper/100) + ", " + (-lower/100) + "]";
      }
      return rows;
    } else {
      return (thorGames == 0 ? "" : thorGames) + "\n" +
          evalLine() + "\n"
          + JNI.prettyPrintDouble(treeNode.getDescendants());
    }
  }
}
