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
import jni.ThorGame;
import jni.ThorGameWithMove;
import ui_desktop.CaseAnnotations;

/**
 *
 * @author michele
 */
public interface UI {
  void setCases(Board board, boolean blackTurn);
  void setAnnotations(CaseAnnotations annotations, int move);
  void setExtras(long nVisited, double milliseconds, CaseAnnotations annotations,
                 double errorBlack, double errorWhite);
  void repaint();
  void setThorGames(ArrayList<ThorGame> games, int square);
  void updateThorGamesWindow(ArrayList<ThorGameWithMove> games);
  String evalFile();
  String thorFolder();
  String bookFolder();
}
