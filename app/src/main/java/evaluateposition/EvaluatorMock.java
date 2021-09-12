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

import board.Board;

public class EvaluatorMock implements EvaluatorInterface {
  long player;
  long opponent;
  long depth;
  long alpha;
  long beta;

  @Override
  public int evaluate(long player, long opponent, int depth, int alpha, int beta) {
    this.player = player;
    this.opponent = opponent;
    this.depth = depth;
    this.alpha = alpha;
    this.beta = beta;
    return 0;
  }

  @Override
  public long getNVisited() {
    return Board.hash(player, opponent) & 0b11111111 * (long) Math.pow(2, depth) * (alpha - beta);
  }
}
