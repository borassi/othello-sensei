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

import static board.BoardTest.assertListEqualsIgnoreOrder;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import org.junit.Test;
import static org.junit.Assert.*;

/**
 *
 * @author michele
 */
public class ProofOfEndgameEvalTest {

  @Test
  public void testOperations() {
    StoredBoard b1 = new StoredBoard(2, 1, true);
    StoredBoard b2 = new StoredBoard(4, 1, true);
    StoredBoard b3 = new StoredBoard(8, 1, true);
    StoredBoard b4 = new StoredBoard(16, 1, true);
    ProofOfEndgameEval p1 = new ProofOfEndgameEval(100, new HashSet<>(Arrays.asList(b1, b2)));
    ProofOfEndgameEval p2 = new ProofOfEndgameEval(120, new HashSet<>(Arrays.asList(b1)));
    ProofOfEndgameEval greaterThanP2 = new ProofOfEndgameEval(140, new HashSet<>(Arrays.asList(b1, b3)));
    ProofOfEndgameEval p3 = new ProofOfEndgameEval(140, new HashSet<>(Arrays.asList(b4)));
    ProofOfEndgameEval greaterThanMany = new ProofOfEndgameEval(140, new HashSet<>(Arrays.asList(b1, b2, b3)));
    ProofOfEndgameEval p4 = new ProofOfEndgameEval(200);
    assertListEqualsIgnoreOrder(
        new ArrayList<>(Arrays.asList(p3, p1, p2)),
        ProofOfEndgameEval.simplify(new ArrayList<>(Arrays.asList(p3, greaterThanP2, p1, p2, p3, greaterThanMany))));
    assertListEqualsIgnoreOrder(
        new ArrayList<>(Arrays.asList(p3, p1, p4, p2)),
        ProofOfEndgameEval.or(
            new ArrayList<>(Arrays.asList(p3, greaterThanP2, p1)),
            new ArrayList<>(Arrays.asList(p4, p2, greaterThanMany))));
    // [(100 b1 b2), (140 b1 b3), (120 b1)] +
    // [(200), (140 b1 b2 b3)] =
    // [(300 b1 b2), (340 b1 b3), (320 b1), (240 b1 b2 b3), (280 b1 b2 b3), (260 b1 b2 b3)] =
    // [(300 b1 b2), (320 b1), (240 b1 b2 b3)]
    assertListEqualsIgnoreOrder(
        new ArrayList<>(Arrays.asList(
            new ProofOfEndgameEval(300, new HashSet<>(Arrays.asList(b1, b2))),
            new ProofOfEndgameEval(320, new HashSet<>(Arrays.asList(b1))),
            new ProofOfEndgameEval(240, new HashSet<>(Arrays.asList(b1, b2, b3))))),
        ProofOfEndgameEval.and(
            new ArrayList<>(Arrays.asList(p1, greaterThanP2, p2)),
            new ArrayList<>(Arrays.asList(p4, greaterThanMany))));
  }
  
}
