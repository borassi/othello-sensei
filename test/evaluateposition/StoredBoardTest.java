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
    StoredBoard sb = new StoredBoard(new Board(1, 2), 10, 0);
    sb.updateEval(0, 0);//, new int[StoredBoard.N_SAMPLES]);
    
    for (short s : sb.samples) {
      assertEquals(0, s);
    }
  }
  @Test
  public void testUpdateLowerUpperBound() {
    StoredBoard sb = new StoredBoard(new Board(1, 2), 10, 0);
    sb.updateEval(0, 1000);//, new int[StoredBoard.N_SAMPLES]);
    sb.setLower(-200, 7000);
    assertEquals(0, sb.eval);
    for (short s : sb.samples) {
      assert(s >= -200);
    }
    assertEquals(-200, sb.lower);
    assertEquals(6400, sb.upper);

    sb.setLower(200, 7000);
    assertEquals(200, sb.eval);
    for (short s : sb.samples) {
      assert(s >= 200);
    }
    assertEquals(200, sb.lower);
    assertEquals(6400, sb.upper);

    sb.setUpper(400, 7000);
    assertEquals(200, sb.eval);
    for (short s : sb.samples) {
      assert(s <= 400);
      assert(s >= 200);
    }
    assertEquals(200, sb.lower); // TODO: fix.
    assertEquals(400, sb.upper);
    
    sb.setSolved(200, 7000);
    assertEquals(200, sb.eval);
    for (short s : sb.samples) {
      assertEquals(s, 200);
    }
    assertEquals(200, sb.lower);
    assertEquals(200, sb.upper);
    
    
  }
}