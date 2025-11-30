/*
 * Copyright (c) 2024-2025. Michele Borassi
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

import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:othello_sensei/widgets_windows/secondary_window.dart';

import '../state.dart';
import '../utils.dart';
import '../widgets_spacers/app_sizes.dart';
import '../widgets_spacers/margins.dart';
import '../widgets_utils/text.dart';
import '../widgets_utils/misc.dart';

class SettingsLocalState with ChangeNotifier {
  final Map<String, dynamic> updates = {};

  SettingsLocalState();

  void reset() {
    updates.clear();
    notifyListeners();
  }
  void set(String name, dynamic value) {
    updates[name] = value;
    notifyListeners();
  }
  dynamic get(String name) {
    return updates[name] ?? GlobalState.preferences.get(name) ?? GlobalState.preferences.defaultPreferences[name];
  }
}

class SettingsTile extends StatelessWidget {
  final String name;
  final SettingsLocalState state;
  // We can't just pass the widget because we need to recompute it when the
  // settings change.
  final Widget Function() childBuilder;

  const SettingsTile({required this.name, required this.childBuilder, required this.state, super.key});

  @override
  Widget build(BuildContext context) {
    var appSizes = Theme.of(context).extension<AppSizes>()!;
    var availableWidth = appSizes.secondaryWindowWidth - 2 * appSizes.margin - (Platform.isIOS || Platform.isAndroid ? 0 : 12);
    var nameWidth = 0.75 * availableWidth;
    var valueWidth = 0.25 * availableWidth;
    var minButtonSize = appSizes.minButtonSize;

    return Container(
      height: minButtonSize,
      alignment: Alignment.center,
      child: Row(
        children: [
          const Margin.internal(),
          Container(width: nameWidth, alignment: Alignment.centerLeft, child: MediumText(name)),
          Container(
            alignment: Alignment.center,
            width: valueWidth,
            child: ListenableBuilder(
                listenable: state,
                builder: (BuildContext context, Widget? widget) => childBuilder()
            ),
          ),
          const Margin.internal()
        ]
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
    return SettingsTile(
      name: name,
      state: state,
      childBuilder: () => SenseiTextFormField(
        controller: TextEditingController(text: state.get(name).toString()),
        onSubmitted: onChanged,
        textAlign: TextAlign.center,
        keyboardType: keyboardType,
        inputFormatters: (inputFormatters ?? []) + [Utf8LengthLimitingTextInputFormatter(12)],
      )
    );
  }
}

Widget getCardSettings(String name, BuildContext context, SettingsLocalState state) {
  bool signed = {'Lower', 'Upper'}.contains(name);
  var value = state.updates[name] ?? GlobalState.preferences.get(name);
  onChanged(newValue) {
    state.set(name, newValue);
  }
  var values = PreferencesState.preferencesValues[name];
  var dropdownHeight = minButtonSize(context);
  if (values != null) {
    return SettingsTile(
        name: name,
        state: state,
        childBuilder: () => InputDecorator(
            decoration: const InputDecoration(
              // Control the vertical gap between text and line here:
              contentPadding: EdgeInsets.symmetric(vertical: -30, horizontal: 0),

              // Use UnderlineInputBorder for a single line, or OutlineInputBorder for a box
              border: UnderlineInputBorder(),
            ),
            child: DropdownButtonHideUnderline(child: DropdownButton<String>(
            itemHeight: dropdownHeight,
            value: state.get(name),
            isExpanded: true,
            icon: const SizedBox.shrink(),
            iconSize: 0,
            onChanged: (String? value) {
              // This is called when the user selects an item.
              if (value != null) {
                onChanged(value);
              }
            },
            selectedItemBuilder: (BuildContext context) {
              return values.map((String value) {
                return Center(child: MediumText(value));
              }).toList();
            },
            items: values.map((String value) {
              return DropdownMenuItem<String>(
                value: value,
                child: Center(child: MediumText(value))
              );
            }).toList(),
          )
    )));
  }
  switch (value.runtimeType) {
    case bool:
      return SettingsTile(
        name: name,
        state: state,
        childBuilder: () => SenseiToggle(
            initialValue: state.get(name),
            onChanged: onChanged
        )
      );
    case int:
      return SettingsTileWithTextForm(
        name: name,
        state: state,
        onChanged: (String? newValue) {
          if (newValue != null) {
            onChanged(newValue == '' ? null : int.parse(newValue));
          }
        },
        keyboardType: TextInputType.numberWithOptions(signed: signed, decimal: false),
        inputFormatters: [IntInputFormatter(minValue: signed ? null : 0)],
      );
    case double:
      return SettingsTileWithTextForm(
        name: name,
        state: state,
        onChanged: (String? newValue) {
          if (newValue != null) {
            onChanged(newValue == '' ? null : double.parse(newValue));
          }
        },
        keyboardType: TextInputType.numberWithOptions(signed: signed, decimal: true),
        inputFormatters: [DoubleInputFormatter(minValue: signed ? null : 0)],
      );
    case String:
      return SettingsTileWithTextForm(
        name: name,
        state: state,
        onChanged: onChanged,
      );
    default:
      throw Exception('Invalid preference type ${value.runtimeType} for value $value');
  }
}

class SettingsHeader extends StatelessWidget {
  final String text;

  const SettingsHeader(this.text, {super.key});

  @override
  Widget build(BuildContext context) {
    var minButtonSize = Theme.of(context).extension<AppSizes>()!.minButtonSize;
    var titleStyle = Theme.of(context).textTheme.bodyMedium!.merge(
        TextStyle(color: Theme.of(context).colorScheme.onSurface)
    );
    return Container(
        height: minButtonSize,
        alignment: Alignment.centerLeft,
        child: MediumText(text, style: titleStyle)
    );
  }
}

class Settings extends StatelessWidget {
  final SettingsLocalState _state;

  Settings({super.key}) : _state = SettingsLocalState();

  static const _appearancePreferences = [
    'Controls position',
    'Margin size',
    'Last move marker',
    'Timer (minutes, 0 for no countdown)',
    'Black and white bars in the graph',
    'Show dialog on save outside archive',
    'Show settings dialog at startup',
    'Show unsupported CPU at startup',
    'Board on the right in horizontal mode',
    'Board color',
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
    'Pressing « from the first position',
    'Evaluate if watching an analyzed game',
    'Analyze on paste',
    'Analyze on import',
    'Analyze on open',
    'When the game ends',
  ];

  static const _enginePreferences = [
    'Seconds until first evaluation',
    'Seconds between evaluations',
    'Seconds/position in game analysis',
    'Seconds/position when playing',
    'Approximate game error when playing',
    'Maximum error / move when playing',
    'Spend half time on positions worse by',
    'Use book',
    'Number of threads',
  ];

  static const _unshownPreferences = [
    'Active tab',
    'Black player',
    'White player',
  ];

  @override
  Widget build(BuildContext context) {
    var nerdPreferences = GlobalState.preferences.defaultPreferences.keys.toSet();
    nerdPreferences.removeAll(_appearancePreferences);
    nerdPreferences.removeAll(_evalPreferences);
    nerdPreferences.removeAll(_behaviorPreferences);
    nerdPreferences.removeAll(_enginePreferences);
    nerdPreferences.removeAll(_unshownPreferences);
    var appearancePreferences = List<String>.of(_appearancePreferences);
    if (Platform.isAndroid) {
      appearancePreferences.remove('Show dialog on save outside archive');
    }

    return SecondaryWindow(
      onPopInvoked: (bool didPop) {
        GlobalState.preferences.setAll(_state.updates);
        GlobalState.evaluate();
      },
      title: 'Settings',
      child: Builder(
        builder: (BuildContext context) {
          return Scaffold(
            resizeToAvoidBottomInset: true,
            body: Column(
              crossAxisAlignment: CrossAxisAlignment.stretch,
              children: [
                Expanded(
                  child: SingleChildScrollView(
                    padding: EdgeInsets.fromLTRB(0, 0, Platform.isIOS || Platform.isAndroid ? 0 : 12, 0),
                    child: Column(
                      crossAxisAlignment: CrossAxisAlignment.stretch,
                      children: [
                        const SettingsHeader('Appearance'),
                        Column(
                          crossAxisAlignment: CrossAxisAlignment.start,
                          children: appearancePreferences.map((s) => getCardSettings(s, context, _state)).toList()
                        ),
                        const SettingsHeader('Evaluation'),
                        Column(
                          crossAxisAlignment: CrossAxisAlignment.start,
                          children: _evalPreferences.map((s) => getCardSettings(s, context, _state)).toList()
                        ),
                        const SettingsHeader('Behavior'),
                        Column(
                          crossAxisAlignment: CrossAxisAlignment.start,
                          children: _behaviorPreferences.map((s) => getCardSettings(s, context, _state)).toList()
                        ),
                        const SettingsHeader('Engine'),
                        Column(
                            crossAxisAlignment: CrossAxisAlignment.start,
                            children: _enginePreferences.map((s) => getCardSettings(s, context, _state)).toList()
                        ),
                        const SettingsHeader('Stuff for nerds'),
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
                    Expanded(
                      child: SenseiButton(
                        onPressed: () { _state.reset(); },
                        text: 'To previous\nvalues',
                      )
                    ),
                    const Margin.internal(),
                    Expanded(
                      child: SenseiButton(
                        onPressed: () async {
                          for (var preference in GlobalState.preferences.getDefault(false).entries) {
                            _state.set(preference.key, preference.value);
                          }
                        },
                        text: 'To advanced\nsettings',
                      )
                    ),
                    const Margin.internal(),
                    Expanded(
                      child: SenseiButton(
                        onPressed: () async {
                          for (var preference in GlobalState.preferences.getDefault(true).entries) {
                            _state.set(preference.key, preference.value);
                          }
                        },
                        text: 'To beginner\nsettings',
                      )
                    )
                  ],
                ),
              ]
            )
          );
        }
      )
    );
  }
}