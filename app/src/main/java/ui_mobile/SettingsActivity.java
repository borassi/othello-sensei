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

import android.content.SharedPreferences;
import android.os.Bundle;
import android.text.InputType;

import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.AppCompatActivity;
import androidx.preference.EditTextPreference;
import androidx.preference.Preference;
import androidx.preference.PreferenceFragmentCompat;

import ui.R;

public class SettingsActivity extends AppCompatActivity {
  @Override
  public boolean onSupportNavigateUp(){
    finish();
    return true;
  }

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.settings_activity);
    if (savedInstanceState == null) {
      getSupportFragmentManager()
          .beginTransaction()
          .replace(R.id.settings, new SettingsFragment())
          .commit();
    }
    ActionBar actionBar = getSupportActionBar();
    if (actionBar != null) {
      actionBar.setDisplayHomeAsUpEnabled(true);
    }
  }

  public static class SettingsFragment extends PreferenceFragmentCompat {
    private SharedPreferences.OnSharedPreferenceChangeListener changeListener;

    @Override
    public void onCreatePreferences(Bundle savedInstanceState, String rootKey) {
      setPreferencesFromResource(R.xml.root_preferences, rootKey);
      changeListener = new SharedPreferences.OnSharedPreferenceChangeListener() {
        @Override
        public void onSharedPreferenceChanged(SharedPreferences sharedPreferences, String key) {
          if (key.equals("delta_moves")) {
            Preference preference = findPreference(key);
            preference.setSummary(String.format("Spend 2x time on a move with score X than a move with score X-%s",
                sharedPreferences.getString(key, "NA")));
          }
          if (key.equals("stop_after")) {
            Preference preference = findPreference(key);
            preference.setSummary(String.format("Stop after visiting %s position",
                sharedPreferences.getString(key, "NA")));
          }
        }
      };
      EditTextPreference pref = findPreference("delta_moves");
      if (pref != null) {
        pref.setOnBindEditTextListener(editText -> editText.setInputType(InputType.TYPE_CLASS_NUMBER | InputType.TYPE_NUMBER_FLAG_DECIMAL));
      }
      pref = findPreference("stop_after");
      if (pref != null) {
        pref.setOnBindEditTextListener(editText -> editText.setInputType(InputType.TYPE_CLASS_NUMBER));
      }
    }

    @Override
    public void onResume() {
      super.onResume();

      getPreferenceScreen().getSharedPreferences().registerOnSharedPreferenceChangeListener(changeListener);
      findPreference("delta_moves").setSummary(
          String.format("Spend 2x time on a move with score X than a move with score X-%s",
              getPreferenceScreen().getSharedPreferences().getString("delta_moves", "NA")));

      findPreference("stop_after").setSummary(
          String.format("Stop after visiting %s position",
          getPreferenceScreen().getSharedPreferences().getString("stop_after", "NA")));
    }
    @Override
    public void onPause() {
      super.onPause();

      getPreferenceScreen().getSharedPreferences().unregisterOnSharedPreferenceChangeListener(changeListener);
    }
  }
}