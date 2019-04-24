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

package tmp;

import board.Board;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import static org.junit.Assert.*;

/**
 *
 * @author michele
 */
public class OptimumAlphaBetaTest {
  
  /**
   * Test of computeOptimum method, of class OptimumAlphaBeta.
   */
  @Test
  public void testComputeOptimum() {
    Board b = new Board(
        "OXXO-XOX" + 
        "XXXXOOOO" + 
        "XXXXOOOO" + 
        "XXXXOOOO" + 
        "XXXXOOOO" + 
        "XXXXOOOO" + 
        "XOXXOOXO" + 
        "-XXXOOO-", true);
    assertEquals(OptimumAlphaBeta.computeOptimumLower(b, 20), 4);
    assertEquals(OptimumAlphaBeta.computeOptimumUpper(b, 20), 7);
  }
  
  @Test
  public void testComputeOptimumPass() {
    Board b = new Board(
        "XXXXXXO-" + 
        "XXXXXXXX" + 
        "XXXXXXXX" + 
        "XXXXXXXX" + 
        "XXXXXXO-" + 
        "XXXXXXXX" + 
        "XXXXXXXX" + 
        "XOOOXXO-", true);
    assertEquals(3, OptimumAlphaBeta.computeOptimumLower(b, 58));
    assertEquals(10, OptimumAlphaBeta.computeOptimumUpper(b, 58));
  }
}
