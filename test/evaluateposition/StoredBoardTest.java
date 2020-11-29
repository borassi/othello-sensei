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
  public void testInitialBoard() {
    StoredBoard father = StoredBoard.initialStoredBoard(new Board("e6"), 10, 20, 5);
    StoredBoard child1 = StoredBoard.childStoredBoard(new Board("e6f4"), father, -100, 1);
    StoredBoard child2 = StoredBoard.childStoredBoard(new Board("e6f6"), father, 0, 1);
    StoredBoard child3 = StoredBoard.childStoredBoard(new Board("e6d6"), father, 800, 1);

    father.setChildren(new StoredBoard[] {child1, child2, child3});
    assertEquals(father.getEval(), 100);
    
    child1.proofNumberGreaterEqual = 1000;
    child1.proofNumberStrictlyGreater = 2200;
    child1.disproofNumberStrictlyGreater = 10000;
    child1.disproofNumberGreaterEqual = 22000;

    child2.proofNumberGreaterEqual = 1100;
    child2.proofNumberStrictlyGreater = 2100;
    child2.disproofNumberStrictlyGreater = 11000;
    child2.disproofNumberGreaterEqual = 21000;

    child3.proofNumberGreaterEqual = 1200;
    child3.proofNumberStrictlyGreater = 2000;
    child3.disproofNumberStrictlyGreater = 12000;
    child3.disproofNumberGreaterEqual = 20000;
    
    father.setChildren(new StoredBoard[] {child1, child2, child3});
    assertEquals(father.getEval(), 100);
    // To prove father > x, we have to prove that at least a child is < x
    assertEquals(10000, father.proofNumberGreaterEqual, 1);
    assertEquals(20000, father.proofNumberStrictlyGreater, 1);
    // To prove father < x, we have to prove that all children are > x
    assertEquals(1000 + 1100 + 1200, father.disproofNumberStrictlyGreater, 1);
    assertEquals(2000 + 2100 + 2200, father.disproofNumberGreaterEqual, 1);
  }

  @Test
  public void testEvalGoalForLeaf() {
    StoredBoard father = StoredBoard.initialStoredBoard(new Board("e6"), 10, 20, 5);
    StoredBoard child1 = StoredBoard.childStoredBoard(new Board("e6f4"), father, -100, 1);
    StoredBoard child2 = StoredBoard.childStoredBoard(new Board("e6f6"), father, 0, 1);
    StoredBoard child3 = StoredBoard.childStoredBoard(new Board("e6d6"), father, 800, 1);
    father.setChildren(new StoredBoard[] {child1, child2, child3});
    father.setEvalGoalRecursive(1000);
    
    // Proves >= -11.
    assertEquals(child1.proofNumberGreaterEqual, StoredBoard.endgameTimeEstimator.proofNumber(new Board("e6f4"), -1100, -100), 1);
    // Proves >= -9.
    assertEquals(child1.proofNumberStrictlyGreater, StoredBoard.endgameTimeEstimator.proofNumber(new Board("e6f4"), -900, -100), 1);
    // Proves <= -9.
    assertEquals(child1.disproofNumberStrictlyGreater, StoredBoard.endgameTimeEstimator.disproofNumber(new Board("e6f4"), -900, -100), 1);
    // Proves <= -11.
    assertEquals(child1.disproofNumberGreaterEqual, StoredBoard.endgameTimeEstimator.disproofNumber(new Board("e6f4"), -1100, -100), 1);

    // Child1 is much better than child3. This means that proof numbers (for the
    // opponent) are harder.
    assert child1.proofNumberGreaterEqual > child3.proofNumberGreaterEqual;
    assert child1.proofNumberStrictlyGreater > child3.proofNumberStrictlyGreater;
    assert child1.disproofNumberStrictlyGreater < child3.disproofNumberStrictlyGreater;
    assert child1.disproofNumberGreaterEqual < child3.disproofNumberGreaterEqual;
  }
  
  @Test
  public void testBoardChildrenAreCorrect() {
    StoredBoard firstMove = StoredBoard.initialStoredBoard(new Board("e6"), 0, 0, 34);
    firstMove.lower = -1;
    firstMove.upper = 1;
 
    StoredBoard diag = StoredBoard.childStoredBoard(new Board("e6f6"), firstMove, 0, 10);
    diag.lower = -1;
    diag.upper = 1;
    
    StoredBoard perp = StoredBoard.childStoredBoard(new Board("e6f4"), firstMove, 2, 11);
    perp.lower = 2;
    perp.upper = 2;
 
    StoredBoard par = StoredBoard.childStoredBoard(new Board("e6d6"), firstMove, 8, 12);
    par.lower = 8;
    par.upper = 8;

    firstMove.children = new StoredBoard[]{diag, perp, par};

    assert(firstMove.isAllOK());

    firstMove.eval = 1;
    assert(!firstMove.isAllOK());
    firstMove.eval = 0;
    
    firstMove.lower = 12;
    assert(!firstMove.isAllOK());
    firstMove.lower = -1;
    
    firstMove.upper = 124;
    assert(!firstMove.isAllOK());
    firstMove.upper = 1;
    
    diag.fathers.clear();
    assert(!firstMove.isAllOK());
    diag.fathers.add(firstMove);

    diag.fathers.add(firstMove);
    assert(!firstMove.isAllOK());
    diag.fathers.remove(diag.fathers.size() - 1);
    
    firstMove.children = new StoredBoard[] {par, perp};
    assert(!firstMove.isAllOK());
    firstMove.children = new StoredBoard[] {par, par, perp};
    assert(!firstMove.isAllOK());
    firstMove.children = new StoredBoard[] {par, par, diag, perp};
    assert(!firstMove.isAllOK());
    firstMove.children = new StoredBoard[] {par, diag, perp};

    firstMove.descendants = 3;
    assert(!firstMove.isAllOK());
    firstMove.descendants = 34;
    
    firstMove.setEvalGoalRecursive(-6);
    assert(firstMove.isAllOK());
  }

  @Test
  public void testBoardChildrenAreCorrectPass() {
    StoredBoard pass = StoredBoard.initialStoredBoard(Board.pass(), 200, 0, 2);
    StoredBoard afterPass = StoredBoard.childStoredBoard(Board.pass().move(0), pass, -200, 1);

    pass.children = new StoredBoard[] {afterPass};

    assert pass.isAllOK();

    pass.eval = 100;
    assert(!pass.isAllOK());
    pass.eval = -200;
    
    afterPass.fathers.clear();
    assert(!pass.isAllOK());
    afterPass.fathers.add(pass);
    
    pass.children = new StoredBoard[0];
    assert(!pass.isAllOK());
    pass.children = new StoredBoard[] {pass};
    assert(!pass.isAllOK());
    pass.children = new StoredBoard[] {afterPass};
  }

  @Test
  public void testBoardChildrenAreCorrectBothPass() {
    StoredBoard bothPass = StoredBoard.initialStoredBoard(Board.bothPass(), -5600, 0, 1);
    
    bothPass.lower = -5600;
    bothPass.upper = -5600;
    bothPass.children = new StoredBoard[0];
    bothPass.proofNumberGreaterEqual = Double.POSITIVE_INFINITY;
    bothPass.proofNumberStrictlyGreater = Double.POSITIVE_INFINITY;
    bothPass.disproofNumberStrictlyGreater = 0;
    bothPass.disproofNumberGreaterEqual = 0;
    
    assert bothPass.isAllOK();

    bothPass.eval = 500;
    assert !bothPass.isAllOK();
    bothPass.eval = -5600;
        
    bothPass.children = new StoredBoard[] {bothPass};
    assert !bothPass.isAllOK();
    bothPass.children = new StoredBoard[0];
  }
}