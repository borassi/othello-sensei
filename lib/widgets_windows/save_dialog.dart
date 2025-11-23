/*
 * Copyright (c) 2025 Michele Borassi
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
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
import '../widgets_board/case.dart';
import '../widgets_board/case_with_cup.dart';
import '../widgets_spacers/app_sizes.dart';
import '../widgets_spacers/margins.dart';
import '../widgets_utils/text.dart';
import '../widgets_spacers/text_sizes.dart';
import '../widgets_utils/misc.dart';

class SaveDialogRow extends StatelessWidget {
  final Widget name;
  final Widget value;

  const SaveDialogRow(this.name, this.value, {super.key});

  @override
  Widget build(BuildContext context) {
    return SizedBox(
      height: max(48, Theme.of(context).extension<AppSizes>()!.squareSize),
      child: Row(
          crossAxisAlignment: CrossAxisAlignment.center,
          children: [
            name,
            const Margin.internal(),
            Expanded(child: value),
          ]
      ),
    );
  }
}

class _InputFormField extends StatelessWidget {
  final void Function(String) onChanged;
  final List<TextInputFormatter>? inputFormatters;
  final TextInputType? keyboardType;

  const _InputFormField({required this.onChanged, this.inputFormatters, this.keyboardType});

  @override
  Widget build(BuildContext context) {
    return SenseiTextFormField(
        type: TextType.large,
        onSubmitted: onChanged,
        textAlign: TextAlign.right,
        inputFormatters: inputFormatters ?? [],
        autocorrect: false,
      );
  }
}

class Searchable extends StatelessWidget {
  final String initialValue;
  final List<String> values;
  final void Function(String) onChanged;
  final List<TextInputFormatter>? inputFormatters;

  const Searchable({super.key, required this.initialValue, required this.values, required this.onChanged, this.inputFormatters});

  @override
  Widget build(BuildContext context) {
    return Autocomplete<String>(
      initialValue: TextEditingValue(text: initialValue),
      optionsBuilder: (TextEditingValue textEditingValue) {
        onChanged(textEditingValue.text);
        if (textEditingValue.text == '') {
          return const Iterable<String>.empty();
        }
        return values.where((String option) {
          return option.toLowerCase().contains(textEditingValue.text.toLowerCase());
        });
      },
      fieldViewBuilder: (context, textEditingController, focusNode, onFieldSubmitted) {
        return Align(
            alignment: Alignment.centerRight,
            child: _InputFormField(
              onChanged: onChanged,
              inputFormatters: inputFormatters,
            )
        );
      },
      optionsViewBuilder: (context, onSelected, options) {
        var itemCount = min(6, options.length);
        var squareSize = Theme.of(context).extension<AppSizes>()!.squareSize;
        return Align(
            alignment: Alignment.topLeft,
            child: Container(
              width: 7 * squareSize - Theme.of(context).extension<AppSizes>()!.margin,
              height: itemCount * squareSize,
              color: Theme.of(context).colorScheme.primaryContainer,
              child: ListView.builder(
                // padding: EdgeInsets.zero,
                // shrinkWrap: false,
                itemCount: itemCount,
                itemBuilder: (BuildContext context, int index) {
                  final String option = options.elementAt(index);
                  return SizedBox(
                    height: squareSize,
                    child: InkWell(
                        onTap: () { onSelected(option); },
                        child: Row(children: [const Spacer(), LargeText(option)])
                    )
                  );
                },
              ),
            )
        );
      },
    );
  }
}

class SaveDialogContent extends StatelessWidget {

  @override
  Widget build(BuildContext context) {
    var gameMetadata = GlobalState.gameMetadataState.getMetadata();
    var players = GlobalState.thorMetadata.players;
    var tournaments = GlobalState.thorMetadata.tournaments.toList();

    return Scaffold(
        resizeToAvoidBottomInset: true,
        body: SingleChildScrollView(
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.stretch,
          mainAxisSize: MainAxisSize.min,
          children: [
            SaveDialogRow(
                Case(CaseState.black, 255, false),
                Searchable(
                    initialValue: cArrayToString(gameMetadata.black),
                    values: players,
                    onChanged: GlobalState.gameMetadataState.setBlack,
                    inputFormatters: [Utf8LengthLimitingTextInputFormatter(20)],
                )
            ),
            const Margin.internal(),
            SaveDialogRow(
                Case(CaseState.white, 255, false),
                Searchable(
                    initialValue: cArrayToString(gameMetadata.white),
                    values: players,
                    onChanged: GlobalState.gameMetadataState.setWhite,
                    inputFormatters: [Utf8LengthLimitingTextInputFormatter(20)],
                )
            ),
            const Margin.internal(),
            SaveDialogRow(
                const CaseWithCup(),
                Searchable(
                    initialValue: cArrayToString(gameMetadata.tournament),
                    values: tournaments,
                    onChanged: GlobalState.gameMetadataState.setTournament,
                    inputFormatters: [Utf8LengthLimitingTextInputFormatter(26)],
                )
            ),
            const Margin.internal(),
            SaveDialogRow(
                LargeText("Round"),
                _InputFormField(
                  onChanged: GlobalState.gameMetadataState.setRound,
                  inputFormatters: [Utf8LengthLimitingTextInputFormatter(11)],
                )
            ),
            const Margin.internal(),
            SaveDialogRow(
                LargeText("Year"),
                _InputFormField(
                  onChanged: (String year) => GlobalState.gameMetadataState.setYear(int.parse(year)),
                  keyboardType: TextInputType.numberWithOptions(signed: false, decimal: false),
                  inputFormatters: [Utf8LengthLimitingTextInputFormatter(5), IntInputFormatter()],
                )
            ),
            const Margin.internal(),
            SaveDialogRow(
                LargeText("Score"),
                Row(
                    crossAxisAlignment: CrossAxisAlignment.center,
                    children: [
                      const Spacer(),
                      SizedBox(
                          width: 0.8 * Theme.of(context).extension<AppSizes>()!.squareSize,
                          child: ListenableBuilder(
                              listenable: GlobalState.gameMetadataState,
                              builder: (context, widget) => SenseiTextFormField(
                                type: TextType.large,
                                key: Key('Score black: ${gameMetadata.black_disks.toString()}'), // To update the form
                                initialText: gameMetadata.black_disks.toString(),
                                onSubmitted: (String value) => GlobalState.gameMetadataState.setScore(value, true),
                                textAlign: TextAlign.left,
                                keyboardType: TextInputType.numberWithOptions(signed: false, decimal: false),
                                inputFormatters: [Utf8LengthLimitingTextInputFormatter(3), IntInputFormatter(minValue: 0, maxValue: 64)],
                              )
                          )
                      ),
                      LargeText("-"),
                      SizedBox(
                          width: 0.8 * Theme.of(context).extension<AppSizes>()!.squareSize,
                          child: ListenableBuilder(
                              listenable: GlobalState.gameMetadataState,
                              builder: (context, widget) => SenseiTextFormField(
                                type: TextType.large,
                                key: Key('Score white: ${gameMetadata.black_disks.toString()}'), // To update the form
                                initialText: (64 - gameMetadata.black_disks).toString(),
                                onSubmitted: (String value) => GlobalState.gameMetadataState.setScore(value, false),
                                textAlign: TextAlign.right,
                                keyboardType: TextInputType.numberWithOptions(signed: false, decimal: false),
                                inputFormatters: [Utf8LengthLimitingTextInputFormatter(3), IntInputFormatter(minValue: 0, maxValue: 64)],
                              )
                          )
                      )
                    ]
                )
            ),
            const Flexible(child: Margin.internal()),
            SizedBox(
                height: max(48, Theme.of(context).extension<AppSizes>()!.squareSize),
                child: SenseiButton(
                  textType: TextType.large,
                  onPressed: () async {
                    Navigator.pop(context);
                    await GlobalState.save();
                  },
                  text: "Save",
                )
            )
          ],
        ))
    );
  }

}

class SaveDialogWidget extends StatelessWidget {

  const SaveDialogWidget({super.key});

  @override
  Widget build(BuildContext context) {
    return SecondaryWindow(
      onPopInvoked: (bool didPop) {},
      title: 'Game data',
      child: SaveDialogContent()
    );
  }
}