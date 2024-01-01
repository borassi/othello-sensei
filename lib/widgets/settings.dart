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
import 'package:card_settings/widgets/card_settings_panel.dart';
import 'package:card_settings/widgets/card_settings_widget.dart';
import 'package:card_settings/widgets/information_fields/card_settings_header.dart';
import 'package:card_settings/widgets/numeric_fields/card_settings_double.dart';
import 'package:card_settings/widgets/numeric_fields/card_settings_int.dart';
import 'package:card_settings/widgets/numeric_fields/card_settings_switch.dart';
import 'package:flutter/material.dart';

import '../state.dart';

CardSettingsWidget getCardSettings(String key, BuildContext context) {
  var value = GlobalState.preferences.get(key);
  onChanged(newValue) async {
    await GlobalState.preferences.set(key, newValue);
  };
  switch (value.runtimeType) {
    case bool:
      return CardSettingsSwitch(
        key: Key(value.toString()),
        label: key,
        labelWidth: 200,
        trueLabel: '',
        falseLabel: '',
        initialValue: value,
        onChanged: onChanged,
        contentAlign: TextAlign.right,
      );
    case int:
      return CardSettingsInt(
        key: Key(value.toString()),
        label: key,
        labelWidth: 200,
        initialValue: value,
        onChanged: onChanged,
        contentAlign: TextAlign.right,
      );
    case double:
      return CardSettingsDouble(
        key: Key(value.toString()),
        label: key,
        labelWidth: 200,
        initialValue: value,
        onChanged: onChanged,
        contentAlign: TextAlign.right,
      );
    case String:
      return CardSettingsText(
        key: Key(value.toString()),
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
  const Settings({super.key});

  static const _evalPreferences = [
    'Number of threads',
    'Positions when evaluating',
    'Positions when playing',
    'Seconds until first evaluation',
    'Seconds between evaluations',
    'Delta',
  ];

  @override
  Widget build(BuildContext context) {
    var nerdPreferences = GlobalState.preferences.defaultPreferences.keys.toSet();
    nerdPreferences.removeAll(_evalPreferences);

    return Scaffold(
      appBar: AppBar(
        title: const Text('Settings'),
        backgroundColor: Theme.of(context).colorScheme.primaryContainer,
        foregroundColor: Theme.of(context).colorScheme.onPrimaryContainer,
      ),
      body: SingleChildScrollView(
        child: Column(
          children: [
            ListenableBuilder(
              listenable: GlobalState.preferences,
              builder: (BuildContext context, Widget? widget) => CardSettings.sectioned(
                children: <CardSettingsSection>[
                  CardSettingsSection(
                    header: CardSettingsHeader(
                      label: 'Evaluation',
                    ),
                    children: _evalPreferences.map((s) { return getCardSettings(s, context); }).toList()
                  ),
                  CardSettingsSection(
                    header: CardSettingsHeader(label: 'Stuff for nerds'),
                    children: nerdPreferences.map((s) { return getCardSettings(s, context); }).toList()
                  ),
                ],
              ),
            ),
            TextButton(
              onPressed: () { GlobalState.preferences.reset(); },
              child: const Text('Reset'),
            )
          ],
        )
      )
    );
  }
}