// Copyright 2021 Google LLC
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
package tmp;

import board.Board;
import jni.JNI;
import evaluatedepthone.BoardWithEvaluation;
import helpers.LoadDataset;
import board.PossibleMovesFinderImproved;
import java.util.ArrayList;
/**
 *
 * @author michele
 */
public class CaseOptimalEndgame {
  final PossibleMovesFinderImproved pmf = new PossibleMovesFinderImproved();
  long[] losses = new long[64];
  long[] num = new long[64];
  
  public void evalOneBoard(Board board, int lastMove) {
    long[] flips = pmf.possibleMoves(board);
    Board best;
    int bestEval;
    
    for (long flip : flips) {
      int move = Long.numberOfTrailingZeros(flip & ~board.getOpponent() & ~board.getPlayer());
      Board next = board.move(flip);
      System.out.print(JNI.evaluateCPP(next.getPlayer(), next.getOpponent(), -6400, 6400) + " ");
    }
    System.out.println();
  }
  
  public void run() {
    ArrayList<BoardWithEvaluation> trainingSet = LoadDataset.loadTrainingSet(1977, 1979);
    
    for (BoardWithEvaluation boardWithEval : trainingSet) {
      Board board = boardWithEval.board;
      if (board.getEmptySquares() != 12) {
        continue;
      }
      evalOneBoard(board, 0);
    }
  }
  
  public static void main(String args[]) {
    new CaseOptimalEndgame().run();
  }
}
