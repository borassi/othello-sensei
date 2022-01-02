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

public class ProbCombinerExponential extends ProbCombiner {
  private final double lambda;

  public ProbCombinerExponential(double lambda) {
    assert lambda < 0;
    this.lambda = lambda;
  }
  @Override
  public double f(double x) {
    assert x >= 0 && x <= 1;
    return Math.exp(1 - Math.pow(x, lambda));
  }

  @Override
  public double inverse(double y) {
    assert y >= 0 && y <= 1;
    return Math.pow(1 - Math.log(y), 1 / lambda);
  }

  @Override
  public double derivative(double x) {
    assert x >= 0 && x <= 1;
    double exponent = 1 - Math.pow(x, lambda);
    if (exponent < -100) {
      return 0;
    }
    return -lambda * Math.pow(x, lambda-1) * Math.exp(exponent);
  }
}
