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
    firstMove.evaluations[0].addDescendants(5);

    eval.addChildren(firstMove);
    StoredBoard diag = eval.get(new Board("e6f6"));
    diag.evaluations[0].addDescendants(1);

    StoredBoard perp = eval.get(new Board("e6f4"));
    perp.evaluations[0].addDescendants(1);
 
    StoredBoard par = eval.get(new Board("e6d6"));
    par.evaluations[0].addDescendants(1);
//    firstMove.setEvalGoalRecursive(1000);
    // Diag is much better than par. This means that proof numbers (for the
    // opponent) are harder.
    assert diag.getProofNumber(- 100) > par.getProofNumber(- 100);
    assert diag.getDisproofNumber(100) < par.getDisproofNumber(100);
  }
  @Test
  public void testToEvaluationIndex() {
    for (int i = 0; -6300 + 200 * i <= 6300; ++i) {
      assertEquals(i, StoredBoard.toEvaluationIndex(-6300 + 200 * i));
    }
  }
  
  @Test
  public void testBoardChildrenAreCorrect() {
    EvaluatorMCTS eval = new EvaluatorMCTS(10, 20);
    eval.setFirstPosition(new Board("e6"));
    StoredBoard firstMove = eval.firstPosition;
    eval.addChildren(firstMove);
    firstMove.evaluations[0].addDescendants(34);

    StoredBoard diag = eval.get(new Board("e6f6"));
    diag.evaluations[0].addDescendants(10);

    StoredBoard perp = eval.get(new Board("e6f4"));
    perp.evaluations[0].addDescendants(11);
 
    StoredBoard par = eval.get(new Board("e6d6"));
    par.evaluations[0].addDescendants(12);

    assert(firstMove.isAllOK());

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
    pass.evaluations[0].addDescendants(2);
    StoredBoard afterPass = StoredBoard.initialStoredBoard(Board.pass().move(0));
    afterPass.evaluations[0].addDescendants(1);

    pass.children = new StoredBoard[] {afterPass};
    afterPass.fathers.add(pass);
    afterPass.playerIsStartingPlayer = false;

    assert pass.isAllOK();

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

//  @Test
//  public void testBoardChildrenAreCorrectBothPass() {
//    StoredBoard bothPass = StoredBoard.initialStoredBoard(Board.bothPass());
//    bothPass.eval = -5600;
//    bothPass.addDescendants(1);
//
//    bothPass.lower = -5600;
//    bothPass.upper = -5600;
//    bothPass.children = new StoredBoard[0];
//    bothPass.evalGreaterEqual.proofNumber = Float.POSITIVE_INFINITY;
//    bothPass.evalStrictlyGreater.disproofNumber = 0;
//
//    assert bothPass.isAllOK();
//
//    bothPass.eval = 500;
//    try {
//      bothPass.isAllOK();
//      throw new IllegalStateException("Should have raised an AssertionError.");
//    } catch (AssertionError e) {}
//    bothPass.eval = -5600;
//
//    bothPass.children = new StoredBoard[] {bothPass};
//    try {
//      bothPass.isAllOK();
//      throw new IllegalStateException("Should have raised an AssertionError.");
//    } catch (AssertionError e) {}
//
//    bothPass.children = new StoredBoard[0];
//  }
}