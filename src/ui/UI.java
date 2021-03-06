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
package ui;

import bitpattern.PositionIJ;
import board.Board;
import evaluateposition.StoredBoard;

/**
 *
 * @author michele
 */
public interface UI {
  public void setCases(Board board, boolean blackTurn);
  public void setAnnotations(CaseAnnotations annotations, PositionIJ ij);
  public void setExtras(StoredBoard firstPosition, double milliseconds);
  public boolean playBlackMoves();
  public boolean playWhiteMoves();
  public long depth();
  public int delta();
  public int lower();
  public int upper();
}
