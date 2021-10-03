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

import org.junit.Test;

import board.Board;
import board.PossibleMovesFinderImproved;
import junit.framework.TestCase;

public class HashMapVisitedPositionsTest extends TestCase {
  PossibleMovesFinderImproved POSSIBLE_MOVES_FINDER = new PossibleMovesFinderImproved();

  @Test
  public void testAddGet() {
    EvaluatorMCTS eval = new EvaluatorMCTS(20, 10);
    StoredBoard s = StoredBoard.initialStoredBoard(new Board());
    s.evaluations[0].addDescendants(1);
    eval.addChildren(s);
    StoredBoard t = eval.get(new Board("e6"));
    HashMapVisitedPositions visitedPositions = new HashMapVisitedPositions(11, 5);

    assertEquals(0, visitedPositions.size);
    
    assertEquals(visitedPositions.get(s.getPlayer(), s.getOpponent()), null);
    assertEquals(visitedPositions.get(s.getPlayer(), s.getOpponent()), null);
    
    visitedPositions.add(s); 
    s.eval = 123;
    assertEquals(123, visitedPositions.get(s.getPlayer(), s.getOpponent()).getEval());
    
    s.eval = 789;
    assertEquals(789, visitedPositions.get(s.getPlayer(), s.getOpponent()).getEval());

    t.eval = 456;
    visitedPositions.add(t); 
    assertEquals(456, visitedPositions.get(t.getPlayer(), t.getOpponent()).getEval());
  }
}