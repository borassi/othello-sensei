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
import board.GetMoves;
import helpers.Gaussian;

/**
 *
 * @author michele
 */
public abstract class EndgameTimeEstimatorInterface {
  public double proofNumber(Board board, int lower, int approxEval) {
    return proofNumber(board.getPlayer(), board.getOpponent(), lower, approxEval);
  }
  public abstract double proofNumber(long player, long opponent, int lower, int approxEval);

  public double disproofNumber(Board board, int lower, int approxEval) {
    return disproofNumber(board.getPlayer(), board.getOpponent(), lower, approxEval);
  }
  public abstract double disproofNumber(long player, long opponent, int lower, int approxEval);
  
}
