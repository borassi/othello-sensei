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

public class EvaluatedBoard {
  public int lower = -6400;
  public int upper = 6400;
  public int depthLower = 0;
  public int depthUpper = 0;
  
  public EvaluatedBoard() {}

  public EvaluatedBoard(int lower, int depthLower, int upper, int depthUpper) {
    this.lower = lower;
    this.depthLower = depthLower;
    this.upper = upper;
    this.depthUpper = depthUpper;
  }
  
  public Evaluation getLower() {
    return new Evaluation(lower, depthLower);
  }
  
  public Evaluation getUpper() {
    return new Evaluation(upper, depthUpper);
  }
  
  @Override
  public boolean equals(Object other) {
    if (this.getClass() != other.getClass()) {
      return false;
    }
    EvaluatedBoard otherBoard = (EvaluatedBoard) other;
    return lower == otherBoard.lower && upper == otherBoard.upper && 
            depthLower == otherBoard.depthLower && depthUpper == otherBoard.depthUpper;
  }
  
  @Override
  public String toString() {
    return lower + " <@" + depthLower + " eval <@" + depthUpper + " " + upper;
  }
}
