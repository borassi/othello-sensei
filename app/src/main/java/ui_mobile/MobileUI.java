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

package ui_mobile;

import androidx.appcompat.app.AppCompatActivity;
import androidx.preference.PreferenceManager;

import android.app.Activity;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.Locale;
import java.util.SortedSet;

import board.Board;

import constants.Constants;
import helpers.FileAccessor;
import main.Main;
import thor.Game;
import jni.JNI;
import ui_desktop.CaseAnnotations;
import main.UI;

import ui.R;

public class MobileUI extends AppCompatActivity implements UI {
  private BoardView boardView;
  private TextView extraInfo;
  public Main main;
  private Board board;
  private boolean blackTurn;
  private boolean wantThor = false;
  private boolean active = true;
  private Menu menu;
  @Override
  public boolean onCreateOptionsMenu(Menu menu) {
    getMenuInflater().inflate(R.menu.menu, menu);
    this.menu = menu;
    return true;
  }

  private void setActive(boolean newActive) {
    active = newActive;
    MenuItem item = menu.findItem(R.id.active);
    if (active) {
      item.setIcon(R.drawable.notifications_20px);
      item.setTitle("Disable evaluations");
    } else {
      item.setIcon(R.drawable.notifications_off_20px);
      item.setTitle("Enable evaluations");
    }
  }

  @Override
  public boolean onOptionsItemSelected(MenuItem item) {
    // Handle item selection
    switch (item.getItemId()) {
      case R.id.newGame:
        main.newGame();
        return true;
      case R.id.undo:
        main.undo();
        return true;
      case R.id.stop:
        main.stop();
        return true;
      case R.id.active:
        setActive(!active);
        return true;
      case R.id.thor:
        main.stop();
        loadThor();
        return true;
      case R.id.action_settings:
        main.stop();
        startActivity(new Intent(this, SettingsActivity.class));
        return true;
      default:
        return super.onOptionsItemSelected(item);
    }
  }

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    Constants.MOBILE = true;
    main = new Main(this);
    setContentView(R.layout.activity_main);
    boardView = findViewById(R.id.board);
    extraInfo = findViewById(R.id.empties);
    main.newGame();
  }

  public void loadThor() {
    Intent intent = new Intent(this, ThorGamesWindowMobile.class);
    startActivityForResult(intent, 1);
  }

  @Override
  public void onActivityResult(int requestCode, int resultCode, Intent data) {
    super.onActivityResult(requestCode, resultCode, data);
    if (requestCode == 1) {
      if (resultCode == Activity.RESULT_OK) {
        String player = data.getStringExtra("player");
        String color = data.getStringExtra("color");
        if (color.equals("Black")) {
          main.thorLookup(new HashSet<>(Arrays.asList(player)), new HashSet<>(), new HashSet<>());
        } else {
          main.thorLookup(new HashSet<>(), new HashSet<>(Arrays.asList(player)), new HashSet<>());
        }
        wantThor = true;
      } else {
        wantThor = false;
      }
    }
  }

  public void getMove(int move) {
    main.play(move);
  }

  @Override
  public void setCases(Board board, boolean blackTurn) {
    boardView.setCases(board, blackTurn, wantThor);
    boardView.resetAnnotations();
    this.board = board.deepCopy();
    this.blackTurn = blackTurn;
  }

  @Override
  public void repaint() {
    boardView.invalidate();
  }

  @Override
  public boolean wantThorGames() {
    return wantThor;
  }

  public void setExtraInfoText(final String text, final int size) {
    runOnUiThread(() -> {
      extraInfo.setTextSize(size);
      extraInfo.setText(text);
      extraInfo.invalidate();
    });
  }

  @Override
  public void setThorGames(Board b, ArrayList<Game> games) {
    String text = "Found " + games.size() + " games:\n";
    int i = 0;
    for (Game g : games) {
      text += String.format(Locale.US, "%-20s %-20s %4d %-2s\n", g.black(), g.white(),
          g.year(),
          g.moves().substring((60 - board.getEmptySquares()) * 2,
              (60 - board.getEmptySquares()) * 2 + 2));
      if (i++ > 30) {
        break;
      }
    }
    setExtraInfoText(text, 14);
  }

  @Override
  public FileAccessor fileAccessor() {
    return new FileAccessorMobile(getBaseContext().getAssets());
  }

  @Override
  public boolean active() {
    return active;
  }

  @Override
  public boolean useBook() { return false; }

  @Override
  public void setAnnotations(CaseAnnotations annotations, int move) {
    BoardView boardView = findViewById(R.id.board);
    boardView.setAnnotations(annotations, move);
  }

  @Override
  public void setExtras(long nVisited, double milliseconds, CaseAnnotations annotations) {
    if (wantThor) {
      return;
    }
    String text = String.format(
        Locale.US,
        "Black:     %5d\n" +
        "White:     %5d\n" +
        "Empties:   %5d\n" +
        "Positions: %5s\n" +
        "Pos/sec:   %5s\n",
        blackTurn ? board.getPlayerDisks() : board.getOpponentDisks(),
        blackTurn ? board.getOpponentDisks() : board.getPlayerDisks(),
        board.getEmptySquares(),
        JNI.prettyPrintDouble(nVisited),
        JNI.prettyPrintDouble(nVisited * 1000.0 / milliseconds));
    setExtraInfoText(text, 26);
  }

  @Override
  public boolean playBlackMoves() {
    return false;
  }

  @Override
  public boolean playWhiteMoves() {
    return false;
  }

  @Override
  public long maxVisited() {
    SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(this);
    return Long.parseLong(preferences.getString("stop_after", getResources().getString(R.string.stop_after_default)));
  }

  @Override
  public double delta() {
    SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(this);
    return Double.parseDouble(preferences.getString("delta_moves", getResources().getString(R.string.delta_moves_default)));
  }

  @Override
  public int lower() {
    return -6300;
  }

  @Override
  public int upper() {
    return 6300;
  }
}
