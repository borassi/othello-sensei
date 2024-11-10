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

import 'dart:io';
import 'dart:math';

import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:othello_sensei/widgets_windows/secondary_window.dart';

import '../state.dart';
import '../utils.dart';
import '../widgets_spacers/app_sizes.dart';
import '../widgets_spacers/margins.dart';
import '../widgets_utils/misc.dart';

class SettingsLocalState with ChangeNotifier {
  Map<String, dynamic> updates;

  SettingsLocalState() : updates = <String, dynamic>{};

  int version = 0;

  void reset() {
    updates.clear();
    ++version;
    notifyListeners();
  }
  void set(String key, dynamic value) {
    updates[key] = value;
    notifyListeners();
  }
}

class SettingsTile extends StatelessWidget {
  final String name;
  final Widget child;

  const SettingsTile({required this.name, required this.child, super.key});

  @override
  Widget build(BuildContext context) {
    var squareSize = Theme.of(context).extension<AppSizes>()!.squareSize;
    var margin = Theme.of(context).extension<AppSizes>()!.margin;
    var style = Theme.of(context).textTheme.bodyMedium!;

    return Container(
      height: max(48, squareSize * 0.75),
      alignment: Alignment.center,
      child: ListTile(
        dense: true,
        contentPadding: EdgeInsets.fromLTRB(margin, 0, 0, 0),
        title: Text(name, style: style),
        trailing: Container(
          alignment: Alignment.centerRight,
          width: 2 * squareSize,
          child: child,
        )
      ),
    );
  }
}

class SettingsTileWithTextForm extends StatelessWidget {
  final String name;
  final List<TextInputFormatter>? inputFormatters;
  final TextInputType? keyboardType;
  final SettingsLocalState state;
  final Function(String? newValue) onChanged;

  const SettingsTileWithTextForm({super.key, required this.name, required this.state, required this.onChanged, this.keyboardType, this.inputFormatters});

  @override
  Widget build(BuildContext context) {
    var squareSize = Theme.of(context).extension<AppSizes>()!.squareSize;
    var value = state.updates[key] ?? GlobalState.preferences.get(name);
    return SettingsTile(
      name: name,
      child: TextFormField(
        style: Theme.of(context).textTheme.bodyMedium!,
        decoration: InputDecoration(
          contentPadding: EdgeInsets.fromLTRB(0, 0, 0, squareSize * 0.1),
          isDense: true,
          enabledBorder: InputBorder.none
        ),
        onChanged: onChanged,
        initialValue: '$value',
        textAlign: TextAlign.right,
        keyboardType: keyboardType,
        inputFormatters: (inputFormatters ?? []) + [LengthLimitingTextInputFormatter(12)],
      )
    );
  }
}

Widget getCardSettings(String key, BuildContext context, SettingsLocalState state) {
  var value = state.updates[key] ?? GlobalState.preferences.get(key);
  onChanged(newValue) {
    state.set(key, newValue);
  }
  var values = PreferencesState.preferencesValues[key];
  if (values != null) {
    if (!values.contains(value!)) {
      value = GlobalState.preferences.defaultPreferences[key];
    }
    return SettingsTile(
      name: key,
      child: DropdownButton<String>(
        value: value,
        isExpanded: true,
        onChanged: (String? value) {
          // This is called when the user selects an item.
          if (value != null) {
            onChanged(value);
          }
        },
        items: values.map((String value) {
          return DropdownMenuItem<String>(
            value: value,
            child: Align(
              alignment: Alignment.centerRight,
              child: Text(
                value,
                style: Theme.of(context).textTheme.bodyMedium!,
                textAlign: TextAlign.right
              ),
            )
          );
        }).toList(),
      )
    );
  }
  switch (value.runtimeType) {
    case bool:
      return SettingsTile(
        name: key,
        child: SenseiToggle(initialValue: value, onChanged: onChanged)
      );
    case int:
      return SettingsTileWithTextForm(
        name: key,
        state: state,
        onChanged: (String? newValue) {
          if (newValue != null) {
            onChanged(newValue == '' ? null : int.parse(newValue));
          }
        },
        keyboardType: const TextInputType.numberWithOptions(signed: true, decimal: false),
        inputFormatters: [IntInputFormatter()],
      );
    case double:
      return SettingsTileWithTextForm(
        name: key,
        state: state,
        onChanged: (String? newValue) {
          if (newValue != null) {
            onChanged(newValue == '' ? null : double.parse(newValue));
          }
        },
        keyboardType: const TextInputType.numberWithOptions(signed: true, decimal: true),
        inputFormatters: [DoubleInputFormatter()],
      );
    case String:
      return SettingsTileWithTextForm(
        name: key,
        state: state,
        onChanged: onChanged,
      );
    default:
      throw Exception('Invalid preference type ${value.runtimeType} for value ${value}');
  }
}

class Settings extends StatelessWidget {
  Settings({super.key}) : _state = SettingsLocalState();

  final SettingsLocalState _state;

  static const _appearancePreferences = [
    'Controls position',
    'Margin size',
    'Show unsupported CPU at startup',
  ];

  static const _evalPreferences = [
    'Round evaluations',
    'Show extra data in evaluate mode',
    'Highlight distance from best move',
    'Best move green, other yellow',
    'Highlight next move in analysis',
    'Highlight next moves outside analysis',
  ];

  static const _behaviorPreferences = [
    'Use illegal moves to undo and redo',
    'Use disk count to undo and redo',
    'Back button action',
    'Analyze on paste',
    'Analyze on import',
  ];

  static const _enginePreferences = [
    'Seconds until first evaluation',
    'Seconds between evaluations',
    'Seconds/position in game analysis',
    'Spend half time on positions worse by',
    'Use book',
    'Number of threads',
    'Positions when evaluating',
  ];

  static const _unshownPreferences = [
    'Active tab',
  ];

  @override
  Widget build(BuildContext context) {
    var nerdPreferences = GlobalState.preferences.defaultPreferences.keys.toSet();
    nerdPreferences.removeAll(_appearancePreferences);
    nerdPreferences.removeAll(_evalPreferences);
    nerdPreferences.removeAll(_behaviorPreferences);
    nerdPreferences.removeAll(_enginePreferences);
    nerdPreferences.removeAll(_unshownPreferences);

    return SecondaryWindow(
      onPopInvoked: (bool didPop) {
        GlobalState.preferences.setAll(_state.updates);
        GlobalState.evaluate();
      },
      title: 'Settings',
      child: ListenableBuilder(
        listenable: _state,
        builder: (BuildContext context, Widget? widget) {
          var titleStyle = TextStyle(
            color: Theme.of(context).colorScheme.onPrimaryContainer,
            fontSize: Theme.of(context).textTheme.bodyMedium!.fontSize!
          );
          var squareSize = Theme.of(context).extension<AppSizes>()!.squareSize;
          return Column(
            children: [
              Expanded(
                child: SingleChildScrollView(
                  padding: EdgeInsets.fromLTRB(0, 0, Platform.isIOS || Platform.isAndroid ? 0 : 12, 0),
                  child: Column(
                    crossAxisAlignment: CrossAxisAlignment.stretch,
                    children: [
                      Container(height: 0.75 * squareSize, alignment: Alignment.centerLeft, child: Text('Appearance', style: titleStyle)),
                      Column(
                        crossAxisAlignment: CrossAxisAlignment.start,
                        children: _appearancePreferences.map((s) => getCardSettings(s, context, _state)).toList()
                      ),
                      Container(height: 0.75 * squareSize, alignment: Alignment.centerLeft, child: Text('Evaluation', style: titleStyle)),
                      Column(
                        crossAxisAlignment: CrossAxisAlignment.start,
                        children: _evalPreferences.map((s) => getCardSettings(s, context, _state)).toList()
                      ),
                      Container(height: 0.75 * squareSize, alignment: Alignment.centerLeft, child: Text('Behavior', style: titleStyle)),
                      Column(
                        crossAxisAlignment: CrossAxisAlignment.start,
                        children: _behaviorPreferences.map((s) => getCardSettings(s, context, _state)).toList()
                      ),
                      Container(height: 0.75 * squareSize, alignment: Alignment.centerLeft, child: Text('Engine', style: titleStyle)),
                      Column(
                          crossAxisAlignment: CrossAxisAlignment.start,
                          children: _enginePreferences.map((s) => getCardSettings(s, context, _state)).toList()
                      ),
                      Container(height: 0.75 * squareSize, alignment: Alignment.centerLeft, child: Text('Stuff for nerds', style: titleStyle)),
                      Column(
                        crossAxisAlignment: CrossAxisAlignment.start,
                        children: nerdPreferences.map((s) => getCardSettings(s, context, _state)).toList()
                      ),
                    ],
                  )
                ),
              ),
              Row(
                children: [
                  Expanded(child: SenseiButton(
                    onPressed: () { _state.reset(); },
                    text: 'Reset to previous values',
                  )),
                  const Margin.internal(),
                  Expanded(child: SenseiButton(
                    onPressed: () { GlobalState.preferences.reset(); _state.reset(); },
                    text: 'Reset to app defaults',
                  )),
                ],
              ),
            ]
          );
        }
      )
    );
  }
}