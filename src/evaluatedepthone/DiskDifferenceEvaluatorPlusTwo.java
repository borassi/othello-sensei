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

/**
 * @author michele
 * Trivial evaluator that returns the disk difference. Used for tests.
 */
public class DiskDifferenceEvaluatorPlusTwo implements DepthOneEvaluator {

  Board b;

  @Override
  public int eval() {
    return 100 * (b.getPlayerDisks() - b.getOpponentDisks() + 2);
  }

  @Override
  public int evalVerbose(Board b) {
    return eval(b);
  }

  @Override
  public void setup(Board b) {
    this.b = b;
  }

  @Override
  public void update(int square, long flip) {
    b = new Board(b.getPlayer() & ~flip, b.getOpponent() | flip);
  }

  @Override
  public void undoUpdate(int square, long flip) {
    b = new Board((b.getPlayer() | flip) & ~(1L << square), b.getOpponent() & ~flip);
  }

  @Override
  public void invert() {
    b = new Board(b.getOpponent(), b.getPlayer());
  }

  @Override
  public int eval(Board b) {
    Board oldB = b;
    this.setup(b);
    int result = this.eval();
    this.setup(oldB);
    return result;
  }
}
