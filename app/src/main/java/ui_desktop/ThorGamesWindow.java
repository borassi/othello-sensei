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
package ui_desktop;

import java.awt.Dimension;
import java.awt.Font;
import java.util.ArrayList;
import java.util.Locale;

import javax.swing.JFrame;
import javax.swing.JTextArea;

import board.Board;
import thor.Game;

public class ThorGamesWindow extends JFrame {
  private final JTextArea thorGamesText;

  ThorGamesWindow() {
    this.setVisible(false);
    this.thorGamesText = new JTextArea();
    thorGamesText.setEditable(false);
    this.thorGamesText.setFont(new Font(Font.MONOSPACED, Font.PLAIN, 20));
    this.add(thorGamesText);
    this.setSize(800, 800);
  }
  @Override
  public Dimension getPreferredSize() {
    return new Dimension(800, 800);
  }

  public void setGames(Board board, ArrayList<Game> games) {
    String text = "Found " + games.size() + " games:\n";
    games.sort((g1, g2) -> {
      if (g1.year() < g2.year()) {
        return 1;
      } else if (g1.year() > g2.year()) {
        return -1;
      }
      return g1.toString().compareTo(g2.toString());
    });
    for (Game g : games) {
      text += String.format(Locale.US, "%-20s %-20s %-26s %4d %-2s\n", g.black(), g.white(),
          g.tournament(),
          g.year(),
          g.moves().substring((60 - board.getEmptySquares()) * 2, (60 - board.getEmptySquares()) * 2 + 2));
    }
    this.thorGamesText.setText(text);
  }
}
