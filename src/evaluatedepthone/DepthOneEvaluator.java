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

package evaluatedepthone;

import board.Board;

public interface DepthOneEvaluator {
  public float lastError();
  public int eval();
  public int eval(long player, long opponent);
  public int evalVerbose(long player, long opponent);
  public void setup(long player, long opponent);
  public void update(int square, long flip);
  public void undoUpdate(int square, long flip);
  public void invert();
}
