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

import board.Board;
import org.junit.Test;
import static org.junit.Assert.*;

/**
 *
 * @author michele
 */
public class StoredBoardTest {

  @Test
  public void testUpdateEval() {
    Board.e6();
    
    StoredBoard father = StoredBoard.initialStoredBoard(Board.e6().getPlayer(), Board.e6().getOpponent(), 10, 20, 1);
    StoredBoard child1 = StoredBoard.childStoredBoard(Board.e6f4().getPlayer(), Board.e6f4().getOpponent(), father, -100, 1);
    StoredBoard child2 = StoredBoard.childStoredBoard(Board.e6f6().getPlayer(), Board.e6f6().getOpponent(), father, 0, 1);
    StoredBoard child3 = StoredBoard.childStoredBoard(Board.e6d6().getPlayer(), Board.e6d6().getOpponent(), father, 800, 1);
    father.setChildren(new StoredBoard[] {child1, child2, child3});
    assertEquals(father.getEval(), 100);
  }

  @Test
  public void testProofNumbers() {
    Board.e6();
    
    StoredBoard father = StoredBoard.initialStoredBoard(Board.e6().getPlayer(), Board.e6().getOpponent(), 10, 20, 1);
    StoredBoard child1 = StoredBoard.childStoredBoard(Board.e6f4().getPlayer(), Board.e6f4().getOpponent(), father, -100, 1);
    StoredBoard child2 = StoredBoard.childStoredBoard(Board.e6f6().getPlayer(), Board.e6f6().getOpponent(), father, 0, 1);
    StoredBoard child3 = StoredBoard.childStoredBoard(Board.e6d6().getPlayer(), Board.e6d6().getOpponent(), father, 800, 1);
    
    child1.proofNumberCurEval = 1000;
    child1.proofNumberNextEval = 2200;
    child1.disproofNumberCurEval = 10000;
    child1.disproofNumberNextEval = 22000;

    child2.proofNumberCurEval = 1100;
    child2.proofNumberNextEval = 2100;
    child2.disproofNumberCurEval = 11000;
    child2.disproofNumberNextEval = 21000;

    child3.proofNumberCurEval = 1200;
    child3.proofNumberNextEval = 2000;
    child3.disproofNumberCurEval = 12000;
    child3.disproofNumberNextEval = 20000;
    
    father.setChildren(new StoredBoard[] {child1, child2, child3});
    assertEquals(father.getEval(), 100);
    // To prove father > x, we have to prove that at least a child is < x
    assertEquals(10000, father.proofNumberCurEval, 1);
    assertEquals(20000, father.proofNumberNextEval, 1);
    // To prove father < x, we have to prove that all children are > x
    assertEquals(1000 + 1100 + 1200, father.disproofNumberCurEval, 1);
    assertEquals(2000 + 2100 + 2200, father.disproofNumberNextEval, 1);
  }

  @Test
  public void testEvalGoalForLeaf() {
    StoredBoard father = StoredBoard.initialStoredBoard(Board.e6().getPlayer(), Board.e6().getOpponent(), 10, 20, 1);
    StoredBoard child1 = StoredBoard.childStoredBoard(Board.e6f4().getPlayer(), Board.e6f4().getOpponent(), father, -100, 1);
    StoredBoard child2 = StoredBoard.childStoredBoard(Board.e6f6().getPlayer(), Board.e6f6().getOpponent(), father, 0, 1);
    StoredBoard child3 = StoredBoard.childStoredBoard(Board.e6d6().getPlayer(), Board.e6d6().getOpponent(), father, 800, 1);
    father.setChildren(new StoredBoard[] {child1, child2, child3});
    father.setEvalGoalRecursive(1000);
    
    // Proves >= -11.
    assertEquals(child1.proofNumberCurEval, EndgameTimeEstimator.proofNumber(Board.e6f4(), -1100, -100), 1);
    // Proves >= -9.
    assertEquals(child1.proofNumberNextEval, EndgameTimeEstimator.proofNumber(Board.e6f4(), -900, -100), 1);
    // Proves <= -9.
    assertEquals(child1.disproofNumberCurEval, EndgameTimeEstimator.disproofNumber(Board.e6f4(), -900, -100), 1);
    // Proves <= -11.
    assertEquals(child1.disproofNumberNextEval, EndgameTimeEstimator.disproofNumber(Board.e6f4(), -1100, -100), 1);

    // Child1 is much better than child3. This means that proof numbers (for the
    // opponent) are harder.
    assert child1.proofNumberCurEval > child3.proofNumberCurEval;
    assert child1.proofNumberNextEval > child3.proofNumberNextEval;
    assert child1.disproofNumberCurEval < child3.disproofNumberCurEval;
    assert child1.disproofNumberNextEval < child3.disproofNumberNextEval;
  }
}