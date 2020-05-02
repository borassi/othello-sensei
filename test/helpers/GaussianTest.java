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
package helpers;

import org.junit.Test;
import static org.junit.Assert.*;

/**
 *
 * @author michele
 */
public class GaussianTest {
  @Test
  public void GaussianTest() {
    assertEquals(0.5, Gaussian.CDF(0, 0, 1), 1.E-5);
    // 68-95-99.7 rule.
    assertEquals((1-0.6827) / 2, Gaussian.CDF(-1, 0, 1), 1.E-4);
    assertEquals((1-0.9545) / 2, Gaussian.CDF(-2, 0, 1), 1.E-4);
    assertEquals((1-0.9974) / 2, Gaussian.CDF(-3, 0, 1), 1.E-4);
    assertEquals((1-0.99997) / 2, Gaussian.CDF(-4, 0, 1), 1.E-4);
    assertEquals(0, Gaussian.CDF(-5, 0, 1), 1.E-5);
    assertEquals(0, Gaussian.CDF(-10, 0, 1), 1.E-5);
    assertEquals(0, Gaussian.CDF(-20, 0, 1), 1.E-5);

    assertEquals(1-(1-0.6827) / 2, Gaussian.CDF(1, 0, 1), 1.E-4);
    assertEquals(1-(1-0.9545) / 2, Gaussian.CDF(2, 0, 1), 1.E-4);
    assertEquals(1-(1-0.9974) / 2, Gaussian.CDF(3, 0, 1), 1.E-4);
    assertEquals(1-(1-0.99997) / 2, Gaussian.CDF(4, 0, 1), 1.E-4);
    assertEquals(1, Gaussian.CDF(5, 0, 1), 1.E-5);
    assertEquals(1, Gaussian.CDF(10, 0, 1), 1.E-5);
    assertEquals(1, Gaussian.CDF(20, 0, 1), 1.E-5);
    
    assertEquals(1, Gaussian.CDF(1, 0, 0.00001), 1.E-5);
    assertEquals((1-0.6827) / 2, Gaussian.CDF(4, 6, 2), 1.E-5);
  }
}
