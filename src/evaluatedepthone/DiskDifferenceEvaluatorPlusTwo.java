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
import evaluateposition.EvaluatorInterface;

/**
 * @author michele
 * Trivial evaluator that returns the disk difference. Used for tests.
 */
public class DiskDifferenceEvaluatorPlusTwo implements DepthOneEvaluator, EvaluatorInterface {

  Board b = new Board();

//  @Override
//  public float lastError() {
//    return 800;
//  }

  @Override
  public int eval() {
    return 100 * (b.getPlayerDisks() - b.getOpponentDisks() + 2);
  }

  @Override
  public int evalVerbose(long player, long opponent) {
    return eval(player, opponent);
  }

  @Override
  public void setup(long player, long opponent) {
    this.b = new Board(player, opponent);
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

  public long eval(Board b) {
    return eval(b.getPlayer(), b.getOpponent());
  }

  @Override
  public int eval(long player, long opponent) {
    Board oldB = b;
    this.setup(player, opponent);
    int result = this.eval();
    this.setup(oldB.getPlayer(), oldB.getOpponent());
    return result;
  }

  @Override
  public int evaluate(long player, long opponent, int depth, int lower, int upper) {
    return Math.max(lower, Math.min(upper, eval(player, opponent)));
  }

  @Override
  public long getNVisited() {
    return 1;
  }
}
