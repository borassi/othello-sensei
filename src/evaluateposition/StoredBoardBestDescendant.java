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
package evaluateposition;

import bitpattern.PositionIJ;
import board.Board;
import constants.Constants;
import helpers.Utils;
import java.util.ArrayList;

/**
 *
 * @author michele
 */
public class StoredBoardBestDescendant {
  StoredBoard board;
  boolean greaterEqual;
  boolean endgame;
  ArrayList<StoredBoard> parents = new ArrayList<>();

  public int getAlpha() {
    if (greaterEqual) {
      return board.getEvalGoal() - 100;
    } else {
      return board.getEvalGoal() + 100;
    }
  }

  public int getBeta() {
    if (greaterEqual) {
      return board.getEvalGoal() - 100;
    } else {
      return board.getEvalGoal() + 100;
    }
  }

  public void updateDescendants(long newDescendants) {
    assert board != null;
    board.addDescendants(newDescendants);
    for (StoredBoard parent : parents) {
      parent.addDescendants(newDescendants);
    }
  }
  
  public boolean isNull() {
    return board == null;
  }
  
  private StoredBoardBestDescendant(StoredBoard board, boolean greaterEqual, boolean endgame) {
    this.board = board;
    this.greaterEqual = greaterEqual;
    this.endgame = endgame;
  }
  
  private void toChild(StoredBoard child) {
    assert child == null || Utils.arrayContains(board.children, child);
    parents.add(board);
    greaterEqual = !greaterEqual;
    board = child;
  }

  public static StoredBoardBestDescendant bestDescendant(
      StoredBoard father, boolean greaterEqual, boolean endgame) {
    StoredBoardBestDescendant result = new StoredBoardBestDescendant(father, greaterEqual, endgame);

//    System.out.println();
    int i = 0;
    while (result.board != null && !result.board.isLeaf()) {
      result.toChild(bestChild(
          result.board,
          result.greaterEqual,
          result.endgame,
          false));
    }
    return result;
  }

  public static StoredBoardBestDescendant randomDescendant(
      StoredBoard father, boolean greaterEqual) {
    StoredBoardBestDescendant result = new StoredBoardBestDescendant(father, greaterEqual, false);

    while (result.board != null && !result.board.isLeaf()) {
      result.toChild(randomChild(result.board));
    }
    return result;
  }

  public static StoredBoardBestDescendant fixedDescendant(
      StoredBoard father, boolean greaterEqual, String sequence) {
    StoredBoardBestDescendant result = new StoredBoardBestDescendant(father, greaterEqual, false);

    for (int i = 0; i < sequence.length(); i += 2) {
      result.toChild(fixedChild(result.board, sequence.substring(i, i+2)));
    }
    return result;
  }
  
  public static StoredBoard bestChild(StoredBoard father, boolean greaterEqual, boolean endgame, boolean verbose) {
    if (greaterEqual) {
      if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {
        if (father.proofNumberGreaterEqual == 0 || father.proofNumberGreaterEqual == Double.POSITIVE_INFINITY) {
          return father.bestChildProofGreaterEqual();
        }
      }
      if (endgame || father.probGreaterEqual > 1 - 0.01) {
        if (verbose) System.out.println("Endgame!" + father.probGreaterEqual);
        return father.bestChildEndgameGreaterEqual();
      } else {
        if (verbose) System.out.println("Midgame!" + father.probGreaterEqual);
        return father.bestChildMidgameGreaterEqual();
      }
    } else {
      if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {
        if (father.disproofNumberStrictlyGreater == Double.POSITIVE_INFINITY || father.disproofNumberStrictlyGreater == 0) {
          return father.bestChildProofStrictlyGreater();
        }
      }
      if (endgame || father.probStrictlyGreater < 0.01) {
        if (verbose) System.out.println("Endgame!");
        return father.bestChildEndgameStrictlyGreater();
      } else {
        if (verbose) System.out.println("Midgame!");
        return father.bestChildMidgameStrictlyGreater();
      }
    }
  }

  public static StoredBoard randomChild(StoredBoard father) {
    StoredBoard[] children = father.getChildren();
    return children[(int) (Math.random() * children.length)];
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