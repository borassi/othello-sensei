// Copyright 2022 Google LLC
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

import junit.framework.TestCase;

import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;

import java.util.Arrays;
import java.util.Collection;

@RunWith(Parameterized.class)
public class ProbCombinerTest extends TestCase {
  @Parameterized.Parameter()
  public ProbCombiner combiner;

  @Parameterized.Parameters
  public static Collection<ProbCombiner> initParameters() {
    return Arrays.asList(new ProbCombiner[] {
        new ProbCombinerExponential(-0.3),
        new ProbCombinerExponential(-1),
        new ProbCombinerPolynomial(2, 2),
        new ProbCombinerPolynomial(1.2, 2),
        new ProbCombinerPolynomial(0.01, 2),
        new ProbCombinerPolynomial(10/9, 10),
        new ProbCombinerExpPolyLog(1),
        new ProbCombinerExpPolyLog(2),
        new ProbCombinerExpPolyLog(4)
    });
  }

  @Test
  public void testF() {
    assertEquals(0, combiner.f(0), 1E-6);
    assertEquals(0, combiner.f(1E-6), 1E-4);
    assertEquals(1, combiner.f(1), 1E-6);
    assertEquals(1, combiner.f(1 - 1E-6), 1E-4);
    for (double d = 0.01; d <= 1; d += 0.01) {
      assertTrue(combiner.f(d) > combiner.f(d - 0.01));
      assertTrue(combiner.f(d) > 0);
      assertTrue(combiner.f(d) < 1);
    }
  }

  @Test
  public void testInverse() {
    for (double d = 0.01; d <= 1; d += 0.01) {
      assertEquals(d, combiner.inverse(combiner.f(d)), 1E-6);
    }
  }

  @Test
  public void testExponentialFDerivative() {
    double epsilon = 1E-10;
    double tolerance = 1E-5;
    assertEquals(
        (combiner.f(epsilon) - combiner.f(0)) / epsilon,
        combiner.derivative(0),
        tolerance);
    assertEquals(
        (combiner.f(1) - combiner.f(1-epsilon)) / epsilon,
        combiner.derivative(1),
        tolerance);

    for (double d = 0.01; d <= 0.99; d += 0.01) {
      assertEquals(
          (combiner.f(d + epsilon) - combiner.f(d)) / epsilon,
          combiner.derivative(d),
          tolerance);
      assertEquals(
          (combiner.f(d - epsilon) - combiner.f(d)) / (-epsilon),
          combiner.derivative(d),
          tolerance);
    }
  }
}