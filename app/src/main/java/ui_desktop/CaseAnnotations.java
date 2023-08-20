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

import jni.JNI;
import jni.Node;

public class CaseAnnotations {
  public final Node father;
  public final Node node;
  public final boolean isBestMove;
  public final int thorGames;

  public CaseAnnotations(Node father, Node treeNode, boolean isBestMove) {
    this(father, treeNode, -1, isBestMove);
  }

  public CaseAnnotations(Node father, Node treeNode, int thorGames, boolean isBestMove) {
    this.node = treeNode;
    this.isBestMove = isBestMove;
    this.thorGames = thorGames;
    this.father = father;
  }

  public String evalLine() {
    String evalFormatter;
    if (node.isSolved()) {
      evalFormatter = "%+.0f";
    } else {
      evalFormatter = "%+.2f";
    }
    return String.format(evalFormatter, -node.getEval())
               + "\n" + String.format(evalFormatter, -node.getLeafEval());
  }

  public String proofDisproofNumberLine() {
    int lower = node.getPercentileLower() - 2;
    int upper = node.getPercentileUpper() + 2;
    return
        (lower >= node.getWeakLower() ? JNI.prettyPrintDouble(node.proofNumber(lower)) : "-") + " " +
        (upper <= node.getWeakUpper() ? JNI.prettyPrintDouble(node.disproofNumber(upper)) : "-");
  }

//  public String solveProbabilityLine() {
//    return
//        JNI.prettyPrintDouble(treeNode.solveProbabilityLower(-6300)) + " " +
//        JNI.prettyPrintDouble(treeNode.solveProbabilityUpper(6300));
//  }

  public String getLines() {
    int lower = node.getPercentileLower() - 1;
    int upper = node.getPercentileUpper() + 1;
    String rows;
    if (thorGames < 0) {
      rows = evalLine() + "\n"
          + JNI.prettyPrintDouble(node.getDescendants()) + "\n";
      if (lower == upper) {
        rows += proofDisproofNumberLine() + "\n";
//        rows += solveProbabilityLine();
      } else {
        rows += "[" + (-upper) + ", " + (-lower) + "]";
      }
      return rows;
    } else {
      return (thorGames == 0 ? "" : thorGames) + "\n" +
          evalLine() + "\n"
          + JNI.prettyPrintDouble(node.getDescendants());
    }
  }
}
