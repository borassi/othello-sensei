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

package ui;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;

import bitpattern.PositionIJ;
import board.Board;
import evaluateposition.StoredBoard;

/**
 *
 * @author michele
 */
public interface UI {
  void setCases(Board board, boolean blackTurn);
  void setAnnotations(CaseAnnotations annotations, PositionIJ ij);
  void setExtras(StoredBoard firstPosition, int evalGoal, double milliseconds);
  boolean playBlackMoves();
  boolean playWhiteMoves();
  long depth();
  int delta();
  int lower();
  int upper();
}