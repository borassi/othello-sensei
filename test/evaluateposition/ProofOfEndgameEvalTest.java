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
    PossibleEndgameProofs.EndgameProof p1 = new PossibleEndgameProofs.EndgameProof(100, b1, b2);
    PossibleEndgameProofs.EndgameProof p2 = new PossibleEndgameProofs.EndgameProof(120, b1);
    PossibleEndgameProofs.EndgameProof greaterThanP2 = new PossibleEndgameProofs.EndgameProof(140, b1, b3);
    PossibleEndgameProofs.EndgameProof p3 = new PossibleEndgameProofs.EndgameProof(140, b4);
    PossibleEndgameProofs.EndgameProof greaterThanMany = new PossibleEndgameProofs.EndgameProof(140, b1, b2, b3);
    PossibleEndgameProofs.EndgameProof p4 = new PossibleEndgameProofs.EndgameProof(200);
    
    PossibleEndgameProofs proofs = new PossibleEndgameProofs(p3, greaterThanP2, p1, p2, p3, greaterThanMany);
    proofs.simplify();
    assertEquals(new PossibleEndgameProofs(p3, p1, p2), proofs);
    assertEquals(
        new PossibleEndgameProofs(p3, p1, p4, p2),
        new PossibleEndgameProofs(p3, greaterThanP2, p1).or(new PossibleEndgameProofs(p4, p2, greaterThanMany)));
    // [(100 b1 b2), (140 b1 b3), (120 b1)] +
    // [(200), (140 b1 b2 b3)] =
    // [(300 b1 b2), (340 b1 b3), (320 b1), (240 b1 b2 b3), (280 b1 b2 b3), (260 b1 b2 b3)] =
    // [(300 b1 b2), (320 b1), (240 b1 b2 b3)]
    assertEquals(
        new PossibleEndgameProofs(
            new PossibleEndgameProofs.EndgameProof(300, b1, b2),
            new PossibleEndgameProofs.EndgameProof(320, b1),
            new PossibleEndgameProofs.EndgameProof(240, b1, b2, b3)),
        new PossibleEndgameProofs(p1, greaterThanP2, p2).and(
            new PossibleEndgameProofs(p4, greaterThanMany)));
  }
  
}
