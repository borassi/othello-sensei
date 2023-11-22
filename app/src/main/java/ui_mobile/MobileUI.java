// Copyright 2023 Michele Borassi
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
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.res.AssetManager;
import android.graphics.Rect;
import android.os.Bundle;
import android.text.TextPaint;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.TextView;

import com.google.firebase.crashlytics.buildtools.reloc.com.google.common.io.ByteStreams;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.Locale;

import bitpattern.PositionIJ;
import board.Board;

import constants.Constants;
import jni.ThorGame;
import jni.ThorGameWithMove;
import main.Main;
import jni.JNI;
import ui_desktop.CaseAnnotations;
import main.UI;

import ui.R;

public class MobileUI extends AppCompatActivity implements UI {
  private enum Task {
    INACTIVE,
    SHOW_EVALS,
    PLAY_BLACK,
    PLAY_WHITE
  }
  private BoardView boardView;
  private TextView extraInfo;
  public Main main;
  private Board board;
  private boolean blackTurn;
  private boolean wantThor = false;
  private Menu menu;
  private Task task = Task.SHOW_EVALS;
  private String notes = "";
  private final static String VERSION = "3";
  private final static String VERSION_FILE = "/version.txt";
  private String localFolderPath;
  private static ArrayList<String> thorPlayers;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    try {
      Context context = getApplicationContext();
      localFolderPath = context.getFilesDir().getAbsolutePath();
      copyAssetsIfNotPresent();
    } catch (IOException e) {
      throw new RuntimeException(e);
    }
    Constants.MOBILE = true;
    setContentView(R.layout.activity_main);
    boardView = findViewById(R.id.board);
    extraInfo = findViewById(R.id.empties);
    main = new Main(this);
    main.newGame();
    thorPlayers = main.getThorPlayers();
  }

  protected static ArrayList<String> getThorPlayers() {
    return thorPlayers;
  }

  private static void deleteDirectoryRecursively(File dir) {
    for (File file: dir.listFiles()) {
      if (file.isDirectory())
        deleteDirectoryRecursively(file);
      file.delete();
    }
  }
  private void copyAssetsIfNotPresent() throws IOException {
    String versionFilepath = localFolderPath + VERSION_FILE;
    String version = "";
    try {
      BufferedReader versionBR = new BufferedReader(new FileReader(versionFilepath));
      version = versionBR.readLine();
      versionBR.close();
    } catch (IOException e) {}
    if (version.equals(VERSION)) {
      return;
    }
    deleteDirectoryRecursively(new File(localFolderPath));
    AssetManager manager = getApplicationContext().getAssets();
    copyAssets(manager, "", localFolderPath);
    BufferedWriter versionBW = new BufferedWriter(new FileWriter(versionFilepath));
    versionBW.write(VERSION);
    versionBW.close();
  }

  private void copyAssets(AssetManager assetManager, String source, String destination) throws IOException {
    String[] filenames = assetManager.list(source);
    if (filenames.length == 0) {
      InputStream in = assetManager.open(source);
      OutputStream out = new FileOutputStream(destination);
      ByteStreams.copy(in, out);
      in.close();
      out.close();
      return;
    }
    new File(destination).mkdirs();
    for (String filename : filenames) {
      String childSource = source + (source == "" ? "" : "/") + filename;
      String childDestination = destination + "/" + filename;
      copyAssets(assetManager, childSource, childDestination);
    }
  }

  @Override
  public boolean onCreateOptionsMenu(Menu menu) {
    getMenuInflater().inflate(R.menu.menu, menu);
    this.menu = menu;
    return true;
  }

  private void rotateTask() {
    MenuItem item = menu.findItem(R.id.active);
    switch (task) {
      case SHOW_EVALS:
        task = task.INACTIVE;
        item.setIcon(R.drawable.notifications_off_20px);
        break;
      case INACTIVE:
        task = Task.PLAY_BLACK;
        item.setIcon(R.drawable.circle_full_20px);
        break;
      case PLAY_BLACK:
        task = Task.PLAY_WHITE;
        item.setIcon(R.drawable.circle_empty_20px);
        break;
      case PLAY_WHITE:
        task = Task.SHOW_EVALS;
        item.setIcon(R.drawable.notifications_20px);
        break;
    }
  }

  @Override
  public boolean onOptionsItemSelected(MenuItem item) {
    // Handle item selection
    int itemId = item.getItemId();
    if (itemId == R.id.newGame) {
      main.newGame();
      return true;
    } else if (itemId == R.id.undo) {
      main.undo();
      return true;
    } else if (itemId == R.id.stop) {
      main.stop();
      return true;
    } else if (itemId == R.id.active) {
      rotateTask();
      return true;
    } else if (itemId == R.id.thor) {
      main.stop();
      loadThor();
      return true;
    } else if (itemId == R.id.set_first_position) {
      main.setFirstPosition();
      return true;
    } else if (itemId == R.id.reset_first_position) {
      main.resetFirstPosition();
      return true;
    } else if (itemId == R.id.copy_notes) {
      copyNotes();
      return true;
    } else if (itemId == R.id.add_to_notes) {
      addToNotes();
      return true;
    } else if (itemId == R.id.action_settings) {
      main.stop();
      startActivity(new Intent(this, SettingsActivity.class));
      return true;
    }
    return super.onOptionsItemSelected(item);
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
          main.thorLookup(Arrays.asList(player), new ArrayList<>(), new ArrayList<>());
        } else {
          main.thorLookup(new ArrayList<>(), Arrays.asList(player), new ArrayList<>());
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

  private String getLongestLine(final String text) {
    String result = "";
    for (String line : text.split("\n")) {
      if (line.length() > result.length()) {
        result = line;
      }
    }
    return result;
  }
  
  public void setExtraInfoText(final String text) {
    runOnUiThread(() -> {
      double widthLimit = (extraInfo.getWidth() - extraInfo.getPaddingLeft() - extraInfo.getPaddingRight()) * 0.9;
      String longestLine = getLongestLine(text);
      for (int i = 50; i > 1; --i) {
        extraInfo.setTextSize(i);
        TextPaint paint = extraInfo.getPaint();
        Rect bounds = new Rect();
        paint.getTextBounds(longestLine, 0, longestLine.length(), bounds);
        if (bounds.right - bounds.left < widthLimit) {
          break;
        }
      }
      extraInfo.setText(text);
      extraInfo.invalidate();
    });
  }

  public void setThorGames(ArrayList<ThorGame> games, int square) {
    boardView.setThorGames(games, square);
  }

  @Override
  public void updateThorGamesWindow(ArrayList<ThorGameWithMove> games) {
    int numGames = Math.min(30, games.size());
    String content = "Showing " + numGames + " of " + games.size() + " games:\n";
    for (int i = 0; i < numGames; ++i) {
      ThorGameWithMove gameMove = games.get(i);
      ThorGame game = gameMove.game();

      content += String.format(
          "%2s %d %20s %2d - %2d %-20s\n",
          new PositionIJ(gameMove.move()).toString(), game.year(), game.black(),
          game.blackScore(), 64 - game.blackScore(), game.white(),
          game.tournament());
    }
//    this.thorGamesText.setText(content);
//    String content = "";
//    for (ThorGame game : games) {
//      content += String.format(
//          "%d %20s %2d-%2d %-20s %s", game.year(), game.black(),
//          game.blackScore(), 64 - game.blackScore(), game.white(),
//          game.tournament());
//    }
    setExtraInfoText(content);
  }

  @Override
  public String evalFile() {
    return Paths.get(localFolderPath, "pattern_evaluator.dat").toString();
  }

  @Override
  public String thorFolder() {
    return Paths.get(localFolderPath, "thor").toString();
  }

  @Override
  public String bookFolder() {
    return Paths.get(localFolderPath, "book").toString();
  }

  @Override
  public boolean active() {
    return task != Task.INACTIVE;
  }

  @Override
  public UseBook useBook() {
    SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(this);
    if (preferences.getBoolean("use_book", false)) {
      return UseBook.READ_ONLY;
    } else {
      return UseBook.DO_NOT_USE;
    }
  }

  @Override
  public double getError() {
    SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(this);
    return Double.parseDouble(preferences.getString("error_when_playing", getResources().getString(R.string.error_when_playing_default)));
  }

  @Override
  public boolean approx() {
    return false;
  }

  @Override
  public int nThreads() {
    return Runtime.getRuntime().availableProcessors();
  }

  @Override
  public void setAnnotations(CaseAnnotations annotations, int move) {
    BoardView boardView = findViewById(R.id.board);
    boardView.setAnnotations(annotations, move);
  }

  @Override
  public void setExtras(long nVisited, double milliseconds, CaseAnnotations annotations, double errorBlack, double errorWhite) {
    if (wantThor) {
      return;
    }
    String text = String.format(
        Locale.US,
        "Black:       %5d\n" +
        "White:       %5d\n" +
        "Error Black: %5.2f\n" +
        "Error White: %5.2f\n" +
        "Empties:     %5d\n" +
        "Positions:   %5s\n" +
        "Pos/sec:     %5s\n",
        blackTurn ? board.getPlayerDisks() : board.getOpponentDisks(),
        blackTurn ? board.getOpponentDisks() : board.getPlayerDisks(),
        errorBlack,
        errorWhite,
        board.getEmptySquares(),
        JNI.prettyPrintDouble(nVisited),
        JNI.prettyPrintDouble(nVisited * 1000.0 / milliseconds));
    setExtraInfoText(text);
  }

  @Override
  public boolean playBlackMoves() {
    return task == Task.PLAY_BLACK;
  }

  @Override
  public boolean playWhiteMoves() {
    return task == Task.PLAY_WHITE;
  }

  @Override
  public long maxVisited() {
    SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(this);
    if (task == Task.PLAY_BLACK || task == Task.PLAY_WHITE) {
      return Long.parseLong(preferences.getString(
          "stop_after_play",
          getResources().getString(R.string.stop_after_play_default)));
    } else {
      return Long.parseLong(preferences.getString(
          "stop_after_eval",
          getResources().getString(R.string.stop_after_eval_default)));
    }
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

  private void copyNotes() {
    ClipboardManager clipboard = (ClipboardManager)
        getSystemService(Context.CLIPBOARD_SERVICE);
    ClipData clip = ClipData.newPlainText("Games", notes);
    clipboard.setPrimaryClip(clip);
  }

  private void addToNotes() {
    notes += main.getGame() + "\n";
  }
}
