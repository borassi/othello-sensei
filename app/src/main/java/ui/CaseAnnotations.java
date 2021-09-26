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

public class CaseAnnotations {
  public StoredBoard storedBoard;
  public HashMap.BoardInHash boardInHash;
  public boolean isBestMove;
  public int evalGoal;

  public CaseAnnotations(StoredBoard storedBoard, int evalGoal, boolean isBestMove) {
    this(storedBoard, evalGoal, null, isBestMove);
  }

  public CaseAnnotations(HashMap.BoardInHash boardInHash, boolean isBestMove) {
    this(null, 0, boardInHash, isBestMove);
  }

  private CaseAnnotations(StoredBoard storedBoard, int evalGoal, HashMap.BoardInHash boardInHash, boolean isBestMove) {
    this.storedBoard = storedBoard;
    this.boardInHash = boardInHash;
    this.isBestMove = isBestMove;
    this.evalGoal = evalGoal;
  }
}
