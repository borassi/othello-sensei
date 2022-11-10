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

package main;

import java.util.ArrayList;

import board.Board;
import helpers.FileAccessor;
import jni.JNI;
import thor.Game;
import ui_desktop.CaseAnnotations;

/**
 *
 * @author michele
 */
public interface UI {
  void setCases(Board board, boolean blackTurn);
  void setAnnotations(CaseAnnotations annotations, int move);
  void setExtras(long nVisited, double milliseconds, CaseAnnotations annotations);
  boolean playBlackMoves();
  boolean playWhiteMoves();
  long maxVisited();
  double delta();
  int lower();
  int upper();
  void repaint();
  boolean wantThorGames();
  void setThorGames(Board b, ArrayList<Game> games);
  FileAccessor fileAccessor();
}
