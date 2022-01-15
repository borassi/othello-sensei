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
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Spinner;
import android.widget.TextView;

import board.Board;

import constants.Constants;
import helpers.Utils;
import main.Main;

public class MobileUI extends AppCompatActivity implements UI {
  private BoardView boardView;
  private TextView posPerSec;
  public Main main;
  public ArrayAdapter<CharSequence> adapter;
  public static Context context;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    context = getBaseContext();
    Constants.MOBILE = true;
    main = new Main(this);
    setContentView(R.layout.activity_main);
    Spinner spinner = findViewById(R.id.depth_spinner);
    adapter = ArrayAdapter.createFromResource(this, R.array.depth, android.R.layout.simple_spinner_item);
    adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
    spinner.setAdapter(adapter);
    boardView = findViewById(R.id.board);
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
    ((TextView) findViewById(R.id.empties)).setText("Empties: " + board.getEmptySquares());
  }

  @Override
  public void repaint() {
    boardView.invalidate();
  }

  @Override
  public void setAnnotations(CaseAnnotations annotations, int move) {
    BoardView boardView = findViewById(R.id.board);
    boardView.setAnnotations(annotations, move);
  }

  @Override
  public void setExtras(CaseAnnotations firstPosition, double milliseconds) {
    posPerSec.setText("Pos/sec: " + Utils.prettyPrintDouble(firstPosition.storedBoard.getDescendants() * 1000.0 / milliseconds));
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
    Spinner depth = findViewById(R.id.depth_spinner);
    return Long.parseLong(depth.getSelectedItem().toString());
  }

  @Override
  public int delta() {
    Spinner delta = findViewById(R.id.delta_spinner);
    return Integer.parseInt(delta.getSelectedItem().toString());
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
