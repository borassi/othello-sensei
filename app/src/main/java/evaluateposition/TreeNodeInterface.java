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

import board.Board;

public abstract class TreeNodeInterface {
  public abstract long getDescendants();
  public abstract int getEval();
  public abstract int getPercentileLower(float prob);
  public abstract int getPercentileUpper(float prob);
  public abstract int getLower();
  public abstract int getUpper();
  public abstract int getWeakLower();
  public abstract int getWeakUpper();
  public abstract float proofNumber(int evalGoal);
  public abstract float disproofNumber(int evalGoal);
  public abstract float getProb(int evalGoal);
  public abstract int maxLogDerivative(int evalGoal);
  public abstract int childLogDerivative(TreeNodeInterface child, int evalGoal);
  public abstract Board getBoard();

  public boolean isSolved() {
    return getLower() == getUpper() - 200;
  }

  public boolean isPartiallySolved() {
    return getWeakLower() == getWeakUpper() - 200;
  }
}