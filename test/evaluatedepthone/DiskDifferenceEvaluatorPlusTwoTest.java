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

import bitpattern.BitPattern;
import board.Board;
import board.PossibleMovesFinderImproved;
import org.junit.Test;
import static org.junit.Assert.*;

/**
 *
 * @author michele
 */
public class DiskDifferenceEvaluatorPlusTwoTest {
  static final DiskDifferenceEvaluatorPlusTwo eval = new DiskDifferenceEvaluatorPlusTwo();
  static final DiskDifferenceEvaluatorPlusTwo evalForTest = new DiskDifferenceEvaluatorPlusTwo();

  
  @Test
  public void testUpdate() {
    PossibleMovesFinderImproved pmf = new PossibleMovesFinderImproved();
    for (int i = 0; i < 10000; ++i) {
      Board b = Board.randomBoard();
      long[] moves = pmf.possibleMovesAdvanced(b.getPlayer(), b.getOpponent());
      if (moves.length == 0) {
        continue;
      }
      long flip = moves[(int) (Math.random() * moves.length)];
      int move = Long.numberOfTrailingZeros(flip & ~b.getPlayer() & ~b.getOpponent());
      eval.setup(b.getPlayer(), b.getOpponent());
      eval.invert();
      if (flip != 0) {
        eval.update(move, flip);
      }
      Board after = b.move(flip);
      evalForTest.setup(after.getPlayer(), after.getOpponent());
      assertEquals(eval.b, evalForTest.b);
      assertEquals(eval.eval(), evalForTest.eval());
    }
  }
  
  @Test
  public void testUndoUpdate() {
    PossibleMovesFinderImproved pmf = new PossibleMovesFinderImproved();
    for (int i = 0; i < 10000; ++i) {
      Board b = Board.randomBoard();
      long[] moves = pmf.possibleMovesAdvanced(b.getPlayer(), b.getOpponent());
      if (moves.length == 0) {
        continue;
      }
      long flip = moves[(int) (Math.random() * moves.length)];
      int move = Long.numberOfTrailingZeros(flip & ~b.getPlayer() & ~b.getOpponent());
      eval.setup(b.getPlayer(), b.getOpponent());
      eval.invert();
      if (flip != 0) {
        eval.update(move, flip);
        eval.undoUpdate(move, flip);
      }
      eval.invert();
      evalForTest.setup(b.getPlayer(), b.getOpponent());
      assertEquals(eval.b, evalForTest.b);
      assertEquals(eval.eval(), evalForTest.eval());
    }
  }
}
