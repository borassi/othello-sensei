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
package ui;

import evaluateposition.HashMap;
import evaluateposition.StoredBoard;
import evaluateposition.TreeNodeInterface;

public class CaseAnnotations {
  public final TreeNodeInterface father;
  public final TreeNodeInterface treeNode;
  public final HashMap.BoardInHash boardInHash;
  public final boolean isBestMove;
  public final int thorGames;

  public CaseAnnotations(TreeNodeInterface father, TreeNodeInterface treeNode, boolean isBestMove) {
    this(father, treeNode, null, isBestMove);
  }

  public CaseAnnotations(TreeNodeInterface father, TreeNodeInterface treeNode, int thorGames, boolean isBestMove) {
    this(father, treeNode, null, thorGames, isBestMove);
  }

  public CaseAnnotations(TreeNodeInterface father, HashMap.BoardInHash boardInHash, boolean isBestMove) {
    this(father, null, boardInHash, isBestMove);
  }

  private CaseAnnotations(TreeNodeInterface father, TreeNodeInterface treeNode, HashMap.BoardInHash boardInHash, boolean isBestMove) {
    this(father, treeNode, boardInHash, -1, isBestMove);
  }

  private CaseAnnotations(TreeNodeInterface father, TreeNodeInterface treeNode, HashMap.BoardInHash boardInHash, int thorGames, boolean isBestMove) {
    this.treeNode = treeNode;
    this.boardInHash = boardInHash;
    this.isBestMove = isBestMove;
    this.thorGames = thorGames;
    this.father = father;
  }
}
