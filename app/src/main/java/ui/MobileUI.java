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

import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TextView;

import java.util.ArrayList;

import board.Board;

import constants.Constants;
import helpers.Utils;
import main.Main;
import thor.Game;

public class MobileUI extends AppCompatActivity implements UI {
  private BoardView boardView;
  private TextView empties;
  private TextView posPerSec;
  public Main main;
  public static Context context;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    context = getBaseContext();
    Constants.MOBILE = true;
    main = new Main(this);
    setContentView(R.layout.activity_main);
    boardView = findViewById(R.id.board);
    empties = findViewById(R.id.empties);
    posPerSec = findViewById(R.id.posPerSec);
    main.newGame();
  }

  public void newGame(View view) {
    main.newGame();
  }

  public void undo(View view) { main.undo(); }

  public void getMove(int move) {
    main.play(move);
  }

  @Override
  public void setCases(Board board, boolean blackTurn) {
    boardView.setCases(board, blackTurn);
    boardView.resetAnnotations();
    runOnUiThread(() -> empties.setText("Empties: " + board.getEmptySquares()));
  }

  @Override
  public void repaint() {
    boardView.invalidate();
  }

  @Override
  public boolean wantThorGames() {
    return false;
  }

  @Override
  public void setThorGames(Board b, ArrayList<Game> games) {}

  @Override
  public void setAnnotations(CaseAnnotations annotations, int move) {
    BoardView boardView = findViewById(R.id.board);
    boardView.setAnnotations(annotations, move);
  }

  @Override
  public void setExtras(long nVisited, double milliseconds, CaseAnnotations annotations) {
    runOnUiThread(() -> posPerSec.setText("Positions: " + Utils.prettyPrintDouble(nVisited) + "\nPos/sec: " + Utils.prettyPrintDouble(nVisited * 1000.0 / milliseconds)));
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
    EditText depth = findViewById(R.id.depthValue);
    return Long.parseLong(depth.getText().toString());
  }

  @Override
  public double delta() {
    TextView delta = findViewById(R.id.deltaValue);
    return Double.parseDouble(delta.getText().toString());
  }

  @Override
  public int lower() {
    return -6300;
  }

  @Override
  public int upper() {
    return 6300;
  }

  public void stop(View view) {
    main.stop();
  }
}
