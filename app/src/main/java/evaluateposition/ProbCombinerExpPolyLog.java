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

public class ProbCombinerExpPolyLog extends ProbCombiner {
  private final double lambda;

  public ProbCombinerExpPolyLog(double lambda) {
    assert lambda >= 1;
    assert lambda <= 4;
    this.lambda = lambda;
  }
  @Override
  public double f(double x) {
    assert x >= 0 && x <= 1;
    return Math.exp(-Math.pow(-Math.log(x), lambda));
  }
}