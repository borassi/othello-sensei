/*
 * Copyright (c) 2024. Michele Borassi
 *
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

import 'package:card_settings/card_settings.dart';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';

import '../state.dart';
import '../utils.dart';

class SettingsLocalState with ChangeNotifier {
  Map<String, dynamic> updates;

  SettingsLocalState() : updates = <String, dynamic>{};

  int version = 0;

  void reset() {
    updates.clear();
    ++version;
    notifyListeners();
  }
}

CardSettingsWidget getCardSettings(String key, BuildContext context, SettingsLocalState state) {
  var value = GlobalState.preferences.get(key);
  onChanged(newValue) async {
    state.updates[key] = newValue;
  };
  var settingsKey = '${key.toString()}: ${state.version}';
  switch (value.runtimeType) {
    case bool:
      return CardSettingsSwitch(
        key: Key(settingsKey),
        label: key,
        labelWidth: 200,
        trueLabel: '',
        falseLabel: '',
        initialValue: value,
        onChanged: onChanged,
        contentAlign: TextAlign.right,
      );
    case int:
      return CardSettingsText(
        key: Key(settingsKey),
        label: key,
        labelWidth: 200,
        maxLength: 20,
        initialValue: value.toString(),
        onChanged: (String? newValue) {
          if (newValue != null) {
            onChanged(int.parse(newValue!));
          }
        },
        contentAlign: TextAlign.right,
        keyboardType: const TextInputType.numberWithOptions(signed: true, decimal: false),
        inputFormatters: [
          IntInputFormatter()
        ],
      );
    case double:
      return CardSettingsText(
        key: Key(settingsKey),
        label: key,
        labelWidth: 200,
        initialValue: value.toString(),
        onChanged: (String? newValue) {
          if (newValue != null) {
            onChanged(double.parse(newValue!));
          }
        },
        contentAlign: TextAlign.right,
        keyboardType: const TextInputType.numberWithOptions(signed: true, decimal: true),
        inputFormatters: [
          DoubleInputFormatter()
          // FilteringTextInputFormatter.deny(RegExp(r'(?<!(\.\d+))\.')),
        ],
      );
    case String:
      return CardSettingsText(
        key: Key(settingsKey),
        label: key,
        labelWidth: 200,
        initialValue: value,
        onChanged: onChanged,
        contentAlign: TextAlign.right,
      );
    default:
      throw Exception('Invalid preference type ${value.runtimeType} for value ${value}');
  }
}

class Settings extends StatelessWidget {
  Settings({super.key}) : _state = SettingsLocalState();

  SettingsLocalState _state;

  static const _evalPreferences = [
    'Number of threads',
    'Positions when evaluating',
    'Positions when playing',
    'Seconds until first evaluation',
    'Seconds between evaluations',
    'Delta',
    'Use book',
    'Round evaluations',
  ];

  static const _analyzePreferences = [
    'Seconds/position in game analysis',
    'Analyze on paste',
    'Analyze on import',
  ];
  static const _unshownPreferences = [
    'Active tab',
  ];

  @override
  void close() {
    GlobalState.preferences.setAll(_state.updates);
  }

  @override
  Widget build(BuildContext context) {
    var nerdPreferences = GlobalState.preferences.defaultPreferences.keys.toSet();
    nerdPreferences.removeAll(_evalPreferences);
    nerdPreferences.removeAll(_analyzePreferences);
    nerdPreferences.removeAll(_unshownPreferences);

    return PopScope(
      onPopInvoked: (bool didPop) {
        GlobalState.preferences.setAll(_state.updates);
      },
      child: Scaffold(
        appBar: AppBar(
          title: const Text('Settings'),
          backgroundColor: Theme.of(context).colorScheme.primaryContainer,
          foregroundColor: Theme.of(context).colorScheme.onPrimaryContainer,
        ),
        body: SingleChildScrollView(
          child: Column(
            children: [
              ListenableBuilder(
                listenable: _state,
                builder: (BuildContext context, Widget? widget) => CardSettings.sectioned(
                  children: <CardSettingsSection>[
                    CardSettingsSection(
                      header: CardSettingsHeader(
                        label: 'Evaluation',
                      ),
                      children: _evalPreferences.map((s) { return getCardSettings(s, context, _state); }).toList()
                    ),
                    CardSettingsSection(
                      header: CardSettingsHeader(
                        label: 'Analysis',
                      ),
                      children: _analyzePreferences.map((s) { return getCardSettings(s, context, _state); }).toList()
                    ),
                    CardSettingsSection(
                      header: CardSettingsHeader(label: 'Stuff for nerds'),
                      children: nerdPreferences.map((s) { return getCardSettings(s, context, _state); }).toList()
                    ),
                  ],
                ),
              ),
              TextButton(
                onPressed: () { _state.reset(); },
                child: const Text('Reset to previous values'),
              ),
              TextButton(
                onPressed: () { GlobalState.preferences.reset(); _state.reset(); },
                child: const Text('Reset to app defaults'),
              )
            ],
          )
        )
      )
    );
  }
}