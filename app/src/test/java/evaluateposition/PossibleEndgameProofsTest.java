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
import java.util.Collections;
import java.util.HashSet;
import java.util.List;
import org.junit.Test;
import static org.junit.Assert.*;

/**
 *
 * @author michele
 */
public class PossibleEndgameProofsTest {

  @Test
  public void testOperations() {
    PossibleEndgameProofs c1 = new PossibleEndgameProofs(new StoredBoard(2, 1, (short) 0), 1);
    PossibleEndgameProofs c2 = new PossibleEndgameProofs(new StoredBoard(4, 1, (short) 0), 1);
    PossibleEndgameProofs c3 = new PossibleEndgameProofs(new StoredBoard(8, 1, (short) 0), 1);
    PossibleEndgameProofs c4 = new PossibleEndgameProofs(new StoredBoard(16, 1, (short) 0), 1);
    PossibleEndgameProofs.EndgameProof p1 = new PossibleEndgameProofs.EndgameProof(100, c1, c2);
    PossibleEndgameProofs.EndgameProof p2 = new PossibleEndgameProofs.EndgameProof(120, c1);
    PossibleEndgameProofs.EndgameProof greaterThanP2 = new PossibleEndgameProofs.EndgameProof(140, c1, c3);
    PossibleEndgameProofs.EndgameProof p3 = new PossibleEndgameProofs.EndgameProof(140, c4);
    PossibleEndgameProofs.EndgameProof greaterThanMany = new PossibleEndgameProofs.EndgameProof(140, c1, c2, c3);
    PossibleEndgameProofs.EndgameProof p4 = new PossibleEndgameProofs.EndgameProof(200);
    
    StoredBoard tmp = new StoredBoard(0, 0, (short) 0);
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

    PossibleEndgameProofs andTestEmpty = new PossibleEndgameProofs(tmp, p1, greaterThanP2, p2);
    PossibleEndgameProofs noProof = new PossibleEndgameProofs(tmp);
    noProof.toNoProof();
    andTestEmpty.and(noProof);
    assert andTestEmpty.canProve() == false;

    andTestEmpty = new PossibleEndgameProofs(tmp, p1, greaterThanP2, p2);
    noProof.and(andTestEmpty);
    
    assert noProof.canProve() == false;
  }
  
  /*
  f: (n1, n2+c+g, n3+c) -> (n1, n2+n4+g, n2+n5+g, n3+n4, n3+n5+g) -> (n1, n2+n4+n6, n2+n5+n6, n3+n4, n3+n5+n6)
  c: (n4, n5+g)
  g: (n6)
  */
  @Test
  public void testGetMinNumChildren() {
    for (int i = 0; i < 1000; ++i) {
      long n1 = (int) (Math.random() * 1000);
      long n2 = (int) (Math.random() * 1000);
      long n3 = (int) (Math.random() * 1000);
      long n4 = (int) (Math.random() * 1000);
      long n5 = (int) (Math.random() * 1000);
      long n6 = (int) (Math.random() * 1000);

      PossibleEndgameProofs g = new PossibleEndgameProofs(new StoredBoard(7, 0, (short) 0), n6);
      PossibleEndgameProofs c = new PossibleEndgameProofs(
          new StoredBoard(3, 0, (short) 0),
          new PossibleEndgameProofs.EndgameProof(n4),
          new PossibleEndgameProofs.EndgameProof(n5, g)        
      );
      PossibleEndgameProofs f = new PossibleEndgameProofs(
          new StoredBoard(1, 0, (short) 0),
          new PossibleEndgameProofs.EndgameProof(n1),
          new PossibleEndgameProofs.EndgameProof(n2, c, g),
          new PossibleEndgameProofs.EndgameProof(n3, c)
      );
      assertEquals(
          f.getMinNPositions(),
          (long) Collections.min(Arrays.asList(n1, n2+n4+n6, n2+n5+n6, n3+n4, n3+n5+n6))
      );
    }
  }
}
