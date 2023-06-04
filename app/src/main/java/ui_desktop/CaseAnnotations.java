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

  public String getLines() {
    int lower = treeNode.getPercentileLower(Constants.PROB_INCREASE_WEAK_EVAL);
    int upper = treeNode.getPercentileUpper(Constants.PROB_INCREASE_WEAK_EVAL);
    String rows;
    String evalFormatter;
    if (treeNode.isSolved()) {
      evalFormatter = "%+.0f";
    } else {
      evalFormatter = "%+.2f";
    }
    if (thorGames < 0) {
     return
          String.format(evalFormatter, -treeNode.getEval() / 100.0) + "\n" +
          JNI.prettyPrintDouble(treeNode.getDescendants()) + "\n" + (
              lower == upper ?
                  JNI.prettyPrintDouble(treeNode.proofNumber(lower)) + " " +
                      JNI.prettyPrintDouble(treeNode.disproofNumber(lower)) :
                  ("[" + (-upper/100) + ", " + (-lower/100) + "]")
          );
    } else {
      return (thorGames == 0 ? "" : thorGames) + "\n" +
          String.format(evalFormatter, -treeNode.getEval() / 100.0) + "\n"
          + JNI.prettyPrintDouble(treeNode.getDescendants());
    }
  }
}
