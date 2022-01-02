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

public class ProbCombinerPolynomial extends ProbCombiner {
  private final double lambda;
  private final double n;

  public ProbCombinerPolynomial(double lambda, double n) {
    assert lambda > 0;
    assert lambda <= n / (n-1);
    this.lambda = lambda;
    this.n = n;
  }
  @Override
  public double f(double x) {
    assert x >= 0 && x <= 1;
    return lambda * x - (lambda - 1) * Math.pow(x, n);
  }
}
