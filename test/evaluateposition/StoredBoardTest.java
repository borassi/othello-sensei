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
    firstMove.descendants = 5;

    firstMove.addChildren(eval);
    StoredBoard diag = eval.get(new Board("e6f6"));
    diag.eval = 0;
    diag.descendants = 1;

    StoredBoard perp = eval.get(new Board("e6f4"));
    perp.eval = -100;
    perp.descendants = 1;
 
    StoredBoard par = eval.get(new Board("e6d6"));
    par.eval = 800;
    par.descendants = 1;
    firstMove.setEvalGoalRecursive(1000);
    
    // Proves >= -11.
    assertEquals(perp.proofNumberGreaterEqual, StoredBoard.endgameTimeEstimator.proofNumber(new Board("e6f4"), -1100, -100), 1);
    // Proves <= -9.
    assertEquals(perp.disproofNumberStrictlyGreater, StoredBoard.endgameTimeEstimator.disproofNumber(new Board("e6f4"), -900, -100), 1);

    // Diag is much better than par. This means that proof numbers (for the
    // opponent) are harder.
    assert diag.proofNumberGreaterEqual > par.proofNumberGreaterEqual;
    assert diag.disproofNumberStrictlyGreater < par.disproofNumberStrictlyGreater;
  }
  
  @Test
  public void testBoardChildrenAreCorrect() {
    EvaluatorMCTS eval = new EvaluatorMCTS(10, 20);
    eval.setFirstPosition(new Board("e6"));
    StoredBoard firstMove = eval.firstPosition;
    firstMove.eval = 0;
    firstMove.descendants = 34;
    firstMove.lower = -1;
    firstMove.upper = 1;

    firstMove.addChildren(eval);
    StoredBoard diag = eval.get(new Board("e6f6"));
    diag.eval = 0;
    diag.descendants = 10;
    diag.lower = -1;
    diag.upper = 1;

    StoredBoard perp = eval.get(new Board("e6f4"));
    perp.eval = 2;
    perp.descendants = 11;
    perp.lower = 2;
    perp.upper = 2;
 
    StoredBoard par = eval.get(new Board("e6d6"));
    par.eval = 8;
    par.descendants = 12;
    par.lower = 8;
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
    
    firstMove.setEvalGoalRecursive(-6);
    assert(firstMove.isAllOK());
  }

  @Test
  public void testBoardChildrenAreCorrectPass() {
    StoredBoard pass = StoredBoard.initialStoredBoard(Board.pass());
    pass.eval = 200;
    pass.descendants = 2;
    StoredBoard afterPass = StoredBoard.initialStoredBoard(Board.pass().move(0));
    afterPass.eval = -200;
    afterPass.descendants = 1;

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
    bothPass.descendants = 1;
    
    bothPass.lower = -5600;
    bothPass.upper = -5600;
    bothPass.children = new StoredBoard[0];
    bothPass.proofNumberGreaterEqual = Double.POSITIVE_INFINITY;
    bothPass.disproofNumberStrictlyGreater = 0;
    
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
}