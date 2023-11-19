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

import android.content.Intent;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.widget.ArrayAdapter;
import android.widget.Spinner;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

import java.util.ArrayList;

import main.Main;
import ui.R;

public class ThorGamesWindowMobile extends AppCompatActivity {

  private Spinner listOfPlayers;
  private TextView player;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.thor_window);
    listOfPlayers = findViewById(R.id.spinner2);
    findViewById(R.id.selectAsBlack).setOnClickListener(view -> select(true));
    findViewById(R.id.selectAsWhite).setOnClickListener(view -> select(false));

    player = findViewById(R.id.player);
    player.addTextChangedListener(new TextWatcher() {
      @Override
      public void afterTextChanged(Editable s) {}

      @Override
      public void beforeTextChanged(CharSequence s, int start, int count, int after) {}

      @Override
      public void onTextChanged(CharSequence s, int start, int before, int count) {
        updateListOfPlayers();
      }
    });
  }

  void updateListOfPlayers() {
    String text = player.getText().toString();
    ArrayList<String> currentPlayers = new ArrayList<>();
    for (String player : MobileUI.getThorPlayers()) {
      if (player.startsWith(text)) {
        currentPlayers.add(player);
      }
      if (currentPlayers.size() > 20) {
        break;
      }
    }
    ArrayAdapter aa = new ArrayAdapter(this, android.R.layout.simple_spinner_item, currentPlayers);
    listOfPlayers.setAdapter(aa);
  }

  void select(boolean black) {
    String player = listOfPlayers.getItemAtPosition(listOfPlayers.getSelectedItemPosition()).toString();
    Intent data = new Intent();
    data.putExtra("player", player);
    data.putExtra("color", black ? "Black" : "White");
    setResult(RESULT_OK, data);
    finish();
  }
}
