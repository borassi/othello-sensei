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

public abstract class ProbCombiner {
  private final static double LAMBDA = -0.45;

  public abstract double f(double x);

  public double inverse(double y) {
    assert y >= 0 && y <= 1;
    double l = 0;
    double u = 1;
    while (u - l > 1E-12) {
      double mid = (l + u) / 2;
      if (f(mid) == y) {
        return mid;
      } else if (f(mid) < y) {
        l = mid;
      } else {
        u = mid;
      }
    }
    return (l + u) / 2;
  }

  public double derivative(double x) {
    assert x >= 0 && x <= 1;
    double epsilon = 1E-10;
    if (x < epsilon) {
      return (f(x + epsilon) - f(x)) / epsilon;
    } else if (x > 1 - epsilon) {
      return (f(x) - f(x - epsilon)) / epsilon;
    }
    return (f(x+epsilon) - f(x-epsilon)) / (2 * epsilon);
  }
}
