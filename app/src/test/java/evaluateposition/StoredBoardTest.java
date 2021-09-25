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
  public void testEvalGoalForLeaf() {
    EvaluatorMCTS eval = new EvaluatorMCTS(10, 20);
    eval.setFirstPosition(new Board("e6"));
    StoredBoard firstMove = eval.firstPosition;
    firstMove.eval = 0;
    firstMove.addDescendants(5);

    eval.addChildren(firstMove);
    StoredBoard diag = eval.get(new Board("e6f6"));
    diag.eval = 0;
    diag.addDescendants(1);

    StoredBoard perp = eval.get(new Board("e6f4"));
    perp.eval = -100;
    perp.addDescendants(1);
 
    StoredBoard par = eval.get(new Board("e6d6"));
    par.eval = 800;
    par.addDescendants(1);
    firstMove.setEvalGoalRecursive(1000);
    // Diag is much better than par. This means that proof numbers (for the
    // opponent) are harder.
    assert diag.getProofNumberGreaterEqual() > par.getProofNumberGreaterEqual();
    assert diag.getDisproofNumberStrictlyGreater() < par.getDisproofNumberStrictlyGreater();
  }
  
  @Test
  public void testBoardChildrenAreCorrect() {
    EvaluatorMCTS eval = new EvaluatorMCTS(10, 20);
    eval.setFirstPosition(new Board("e6"));
    StoredBoard firstMove = eval.firstPosition;
    eval.addChildren(firstMove);
    firstMove.eval = 0;
    firstMove.addDescendants(34);
    firstMove.lower = -1;
    firstMove.upper = 1;

    StoredBoard diag = eval.get(new Board("e6f6"));
    diag.eval = 0;
    diag.addDescendants(10);
    diag.lower = -1;
    diag.upper = 1;

    StoredBoard perp = eval.get(new Board("e6f4"));
    perp.eval = 2;
    perp.addDescendants(11);
    perp.lower = 1;
    perp.upper = 2;
 
    StoredBoard par = eval.get(new Board("e6d6"));
    par.eval = 8;
    par.addDescendants(12);
    par.lower = 6;
    par.upper = 8;

    assert(firstMove.isAllOK());

    firstMove.eval = 1;
    try {
      firstMove.isAllOK();
      throw new IllegalStateException("Should have raised an AssertionError.");
    } catch (AssertionError e) {}
    firstMove.eval = 0;
    
    firstMove.lower = 12;
    try {
      firstMove.isAllOK();
      throw new IllegalStateException("Should have raised an AssertionError.");
    } catch (AssertionError e) {}
    firstMove.lower = -1;
    
    firstMove.upper = 124;
    try {
      firstMove.isAllOK();
      throw new IllegalStateException("Should have raised an AssertionError.");
    } catch (AssertionError e) {}
    firstMove.upper = 1;
    
    diag.fathers.clear();
    try {
      firstMove.isAllOK();
      throw new IllegalStateException("Should have raised an AssertionError.");
    } catch (AssertionError e) {}
    diag.fathers.add(firstMove);

    diag.fathers.add(firstMove);
    try {
      firstMove.isAllOK();
      throw new IllegalStateException("Should have raised an AssertionError.");
    } catch (AssertionError e) {}
    diag.fathers.remove(diag.fathers.size() - 1);
    
    firstMove.children = new StoredBoard[] {par, perp};
    try {
      firstMove.isAllOK();
      throw new IllegalStateException("Should have raised an AssertionError.");
    } catch (AssertionError e) {}
    firstMove.children = new StoredBoard[] {par, par, perp};
    try {
      firstMove.isAllOK();
      throw new IllegalStateException("Should have raised an AssertionError.");
    } catch (AssertionError e) {}
    firstMove.children = new StoredBoard[] {par, par, diag, perp};
    try {
      firstMove.isAllOK();
      throw new IllegalStateException("Should have raised an AssertionError.");
    } catch (AssertionError e) {}
    firstMove.children = new StoredBoard[] {par, diag, perp};
    
    assert(firstMove.isAllOK());
  }

  @Test
  public void testBoardChildrenAreCorrectPass() {
    StoredBoard pass = StoredBoard.initialStoredBoard(Board.pass());
    pass.eval = 200;
    pass.addDescendants(2);
    StoredBoard afterPass = StoredBoard.initialStoredBoard(Board.pass().move(0));
    afterPass.eval = -200;
    afterPass.addDescendants(1);

    pass.children = new StoredBoard[] {afterPass};
    afterPass.fathers.add(pass);
    afterPass.playerIsStartingPlayer = false;

    assert pass.isAllOK();

    pass.eval = 100;
    try {
      pass.isAllOK();
      throw new IllegalStateException("Should have raised an AssertionError.");
    } catch (AssertionError e) {}
    pass.eval = -200;
    
    afterPass.fathers.clear();
    try {
      pass.isAllOK();
      throw new IllegalStateException("Should have raised an AssertionError.");
    } catch (AssertionError e) {}
    afterPass.fathers.add(pass);
    
    pass.children = new StoredBoard[0];
    try {
      pass.isAllOK();
      throw new IllegalStateException("Should have raised an AssertionError.");
    } catch (AssertionError e) {}
    pass.children = new StoredBoard[] {pass};
    try {
      pass.isAllOK();
      throw new IllegalStateException("Should have raised an AssertionError.");
    } catch (AssertionError e) {}
    pass.children = new StoredBoard[] {afterPass};
  }

  @Test
  public void testBoardChildrenAreCorrectBothPass() {
    StoredBoard bothPass = StoredBoard.initialStoredBoard(Board.bothPass());
    bothPass.eval = -5600;
    bothPass.addDescendants(1);
    
    bothPass.lower = -5600;
    bothPass.upper = -5600;
    bothPass.children = new StoredBoard[0];
    bothPass.evalGreaterEqual.proofNumber = Float.POSITIVE_INFINITY;
    bothPass.evalStrictlyGreater.disproofNumber = 0;
    
    assert bothPass.isAllOK();

    bothPass.eval = 500;
    try {
      bothPass.isAllOK();
      throw new IllegalStateException("Should have raised an AssertionError.");
    } catch (AssertionError e) {} 
    bothPass.eval = -5600;
        
    bothPass.children = new StoredBoard[] {bothPass};
    try {
      bothPass.isAllOK();
      throw new IllegalStateException("Should have raised an AssertionError.");
    } catch (AssertionError e) {} 
      
    bothPass.children = new StoredBoard[0];
  }

  /*
  b - b1 - b11 - b111
    \    /     /
      b2 - b21 - b211
         \     /
           b22 
  
  */
  @Test
  public void testLeastCommonAncestorNoPass() {
    StoredBoard b = new StoredBoard(0, 0, true);
    StoredBoard b1 = new StoredBoard(0, 1, false);
    StoredBoard b2 = new StoredBoard(0, 1, false);
    StoredBoard b11 = new StoredBoard(0, 3, true);
    StoredBoard b21 = new StoredBoard(0, 3, true);
    StoredBoard b22 = new StoredBoard(0, 3, true);
    StoredBoard b111 = new StoredBoard(0, 7, false);
    StoredBoard b211 = new StoredBoard(0, 7, false);

    b.children = new StoredBoard[] {b1, b2};
    b1.children = new StoredBoard[] {b11};
    b2.children = new StoredBoard[] {b11, b21, b22};
    b11.children = new StoredBoard[] {b111};
    b21.children = new StoredBoard[] {b111, b211};
    b22.children = new StoredBoard[] {b211};
    
    for (StoredBoard board : new StoredBoard[] {b, b1, b2, b11, b21, b22, b111, b211}) {
      if (board.children == null) {
        continue;
      }
      for (StoredBoard child : board.children) {
        child.fathers.add(board);
      }
    }
    
    assertNull(b.findLeastCommonAncestor());
    assertEquals(b, b1.findLeastCommonAncestor());
    assertEquals(b, b11.findLeastCommonAncestor());
    assertEquals(b, b111.findLeastCommonAncestor());
    assertEquals(b2, b211.findLeastCommonAncestor());
  }

  /*
      b1 PS b1_ -  b11 PS b111
    /           PS       /
  b - b2 -- b21         /
    \                  /
      b3 -- b31 ------/    
  */
  @Test
  public void testLeastCommonAncestorPass() {
    StoredBoard b = new StoredBoard(0, 0, true);
    StoredBoard b1 = new StoredBoard(0, 1, false);
    StoredBoard b2 = new StoredBoard(0, 1, false);
    StoredBoard b3 = new StoredBoard(0, 1, false);
    StoredBoard b1_ = new StoredBoard(1, 0, true);
    StoredBoard b21 = new StoredBoard(3, 0, true);
    StoredBoard b31 = new StoredBoard(3, 0, true);
    StoredBoard b11 = new StoredBoard(0, 3, false);
    StoredBoard b111 = new StoredBoard(0, 7, false);

    b.children = new StoredBoard[] {b1, b2, b3};
    b1.children = new StoredBoard[] {b1_};
    b2.children = new StoredBoard[] {b21};
    b1_.children = new StoredBoard[] {b11};
    b21.children = new StoredBoard[] {b11};
    b11.children = new StoredBoard[] {b111};
    b3.children = new StoredBoard[] {b31};
    b31.children = new StoredBoard[] {b111};
    
    for (StoredBoard board : new StoredBoard[] {b, b1, b2, b3, b11, b21, b31, b1_, b111}) {
      if (board.children == null) {
        continue;
      }
      for (StoredBoard child : board.children) {
        child.fathers.add(board);
      }
    }
    
    assertNull(b.findLeastCommonAncestor());
    assertEquals(b, b1.findLeastCommonAncestor());
    assertEquals(b1, b1_.findLeastCommonAncestor());
    assertEquals(b2, b21.findLeastCommonAncestor());
    assertEquals(b, b11.findLeastCommonAncestor());
    assertEquals(b, b111.findLeastCommonAncestor());
    assertEquals(b3, b31.findLeastCommonAncestor());
  }
}