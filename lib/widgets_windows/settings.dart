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
    var squareSize = Theme.of(context).extension<AppSizes>()!.squareSize!;
    var margin = Theme.of(context).extension<AppSizes>()!.margin!;
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
    var squareSize = Theme.of(context).extension<AppSizes>()!.squareSize!;
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
  var squareSize = Theme.of(context).extension<AppSizes>()!.squareSize!;
  onChanged(newValue) {
    state.set(key, newValue);
  };
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
            onChanged(newValue == '' ? null : int.parse(newValue!));
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
            onChanged(newValue == '' ? null : double.parse(newValue!));
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

  SettingsLocalState _state;

  static const _uiPreferences = [
    'Show coordinates',
    'Highlight distance from best move',
    'Best move green, other yellow',
    'Highlight next move in analysis',
    'Highlight next moves outside analysis',
  ];

  static const _evalPreferences = [
    'Number of threads',
    'Positions when evaluating',
    // 'Positions when playing',
    'Seconds until first evaluation',
    'Seconds between evaluations',
    'Spend half time on positions worse by',
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
  Widget build(BuildContext context) {
    var nerdPreferences = GlobalState.preferences.defaultPreferences.keys.toSet();
    nerdPreferences.removeAll(_uiPreferences);
    nerdPreferences.removeAll(_evalPreferences);
    nerdPreferences.removeAll(_analyzePreferences);
    nerdPreferences.removeAll(_unshownPreferences);

    return SecondaryWindow(
      onPopInvoked: (bool didPop) {
        GlobalState.preferences.setAll(_state.updates);
        GlobalState.evaluate();
      },
      leaveSpaceRight: true,
      title: 'Settings',
      child: ListenableBuilder(
        listenable: _state,
        builder: (BuildContext context, Widget? widget) {
          var titleStyle = TextStyle(
            color: Theme.of(context).colorScheme.onPrimaryContainer,
            fontSize: Theme.of(context).textTheme.bodyMedium!.fontSize!
          );
          var squareSize = Theme.of(context).extension<AppSizes>()!.squareSize!;
          return SingleChildScrollView(
            padding: EdgeInsets.fromLTRB(0, 0, Theme.of(context).extension<AppSizes>()!.margin!, 0),
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.stretch,
              children: [
                Container(height: 0.75 * squareSize, alignment: Alignment.centerLeft, child: Text('User interface', style: titleStyle)),
                Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: _uiPreferences.map((s) => getCardSettings(s, context, _state)).toList()
                ),
                Container(height: 0.75 * squareSize, alignment: Alignment.centerLeft, child: Text('Evaluation', style: titleStyle)),
                Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: _evalPreferences.map((s) => getCardSettings(s, context, _state)).toList()
                ),
                Container(height: 0.75 * squareSize, alignment: Alignment.centerLeft, child: Text('Analysis', style: titleStyle)),
                Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: _analyzePreferences.map((s) => getCardSettings(s, context, _state)).toList()
                ),
                Container(height: 0.75 * squareSize, alignment: Alignment.centerLeft, child: Text('Stuff for nerds', style: titleStyle)),
                Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: nerdPreferences.map((s) => getCardSettings(s, context, _state)).toList()
                ),
                const Margin(),
                SenseiButton(
                  onPressed: () { GlobalState.preferences.reset(); _state.reset(); },
                  text: 'Reset to previous values',
                ),
                const Margin(),
                SenseiButton(
                  onPressed: () { GlobalState.preferences.reset(); _state.reset(); },
                  text: 'Reset to app defaults',
                ),
                const Margin(),
              ],
            )
          );
        }
      )
    );
  }
}