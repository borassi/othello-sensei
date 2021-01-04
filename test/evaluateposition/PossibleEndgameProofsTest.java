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
public class PossibleEndgameProofsTest {

  @Test
  public void testOperations() {
    PossibleEndgameProofs c1 = new PossibleEndgameProofs(new StoredBoard(2, 1, true));
    PossibleEndgameProofs c2 = new PossibleEndgameProofs(new StoredBoard(4, 1, true));
    PossibleEndgameProofs c3 = new PossibleEndgameProofs(new StoredBoard(8, 1, true));
    PossibleEndgameProofs c4 = new PossibleEndgameProofs(new StoredBoard(16, 1, true));
    PossibleEndgameProofs.EndgameProof p1 = new PossibleEndgameProofs.EndgameProof(100, c1, c2);
    PossibleEndgameProofs.EndgameProof p2 = new PossibleEndgameProofs.EndgameProof(120, c1);
    PossibleEndgameProofs.EndgameProof greaterThanP2 = new PossibleEndgameProofs.EndgameProof(140, c1, c3);
    PossibleEndgameProofs.EndgameProof p3 = new PossibleEndgameProofs.EndgameProof(140, c4);
    PossibleEndgameProofs.EndgameProof greaterThanMany = new PossibleEndgameProofs.EndgameProof(140, c1, c2, c3);
    PossibleEndgameProofs.EndgameProof p4 = new PossibleEndgameProofs.EndgameProof(200);
    
    StoredBoard tmp = new StoredBoard(0, 0, true);
    PossibleEndgameProofs proofs = new PossibleEndgameProofs(tmp, p3, greaterThanP2, p1, p2, p3, greaterThanMany);
    proofs.simplify();
    assertListEqualsIgnoreOrder(Arrays.asList(p3, p1, p2), proofs.orClauses);
    
    PossibleEndgameProofs orTest = new PossibleEndgameProofs(tmp, p3, greaterThanP2, p1);
    orTest.or(new PossibleEndgameProofs(tmp, p4, p2, greaterThanMany));
    assertListEqualsIgnoreOrder(Arrays.asList(p3, p1, p4, p2), orTest.orClauses);
    // [(100 b1 b2), (140 b1 b3), (120 b1)] +
    // [(200), (140 b1 b2 b3)] =
    // [(300 b1 b2), (340 b1 b3), (320 b1), (240 b1 b2 b3), (280 b1 b2 b3), (260 b1 b2 b3)] =
    // [(300 b1 b2), (320 b1), (240 b1 b2 b3)]
    
    PossibleEndgameProofs andTest = new PossibleEndgameProofs(tmp, p1, greaterThanP2, p2);
    andTest.and(new PossibleEndgameProofs(tmp, p4, greaterThanMany));
    
    assertListEqualsIgnoreOrder(
        Arrays.asList(
            new PossibleEndgameProofs.EndgameProof(300, c1, c2),
            new PossibleEndgameProofs.EndgameProof(320, c1),
            new PossibleEndgameProofs.EndgameProof(240, c1, c2, c3)),
        andTest.orClauses);
  }
  
}
