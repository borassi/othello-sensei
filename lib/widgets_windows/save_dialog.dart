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

import 'dart:ffi';
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
import '../widgets_utils/misc.dart';

String _arrayToString(Array<Char> array) {
  final stringList = <int>[];
  var i = 0;
  while (array[i] != 0) {
    stringList.add(array[i]);
    i++;
  }
  try {
    return String.fromCharCodes(stringList);
  } on ArgumentError {
    return '';
  }
}

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
  final void Function(String)? onChanged;
  final List<TextInputFormatter>? inputFormatters;
  final TextEditingController? controller;
  final FocusNode? focusNode;
  final String? initialValue;
  final TextInputType? keyboardType;

  const _InputFormField({this.onChanged, this.inputFormatters, this.controller, this.focusNode, this.initialValue, this.keyboardType});

  @override
  Widget build(BuildContext context) {
    var squareSize = Theme.of(context).extension<AppSizes>()!.squareSize;
    return TextFormField(
        controller: controller,
        focusNode: focusNode,
        style: Theme.of(context).textTheme.bodyLarge!,
        onChanged: onChanged,
        textAlign: TextAlign.right,
        inputFormatters: inputFormatters,
        decoration: InputDecoration(
            contentPadding: EdgeInsets.fromLTRB(0, 0, 0, squareSize * 0.1),
            isDense: true,
            enabledBorder: InputBorder.none
        )
      );
  }
}

class SaveDialogContent extends StatelessWidget {

  Widget playerSearch(BuildContext context, bool black) {
    var players = GlobalState.thorMetadata.playerStringToIndex.keys.toList();
    players.add("");
    players.sort();
    return ListenableBuilder(
        listenable: GlobalState.gameMetadataState,
        builder: (BuildContext context, Widget? child) {
          var metadata = GlobalState.gameMetadataState.getMetadata();
          Array<Char> initialValue = black ? metadata.black : metadata.white;
          return Autocomplete<String>(
            initialValue: TextEditingValue(text: _arrayToString(initialValue)),
            optionsBuilder: (TextEditingValue textEditingValue) {
              if (textEditingValue.text == '') {
                return const Iterable<String>.empty();
              }
              if (black) {
                GlobalState.gameMetadataState.setBlack(textEditingValue.text);
              } else {
                GlobalState.gameMetadataState.setWhite(textEditingValue.text);
              }
              return players.where((String option) {
                return option.toLowerCase().contains(textEditingValue.text.toLowerCase());
              });
            },
            fieldViewBuilder: (context, textEditingController, focusNode, onFieldSubmitted) {
              return Align(
                  alignment: Alignment.centerRight,
                  child: _InputFormField(
                    controller: textEditingController,
                    focusNode: focusNode,
                  )
              );
            },
            optionsViewBuilder: (context, onSelected, options) {
              return Align(
                  alignment: Alignment.topLeft,
                  child: Container(
                    width: 7 * Theme.of(context).extension<AppSizes>()!.squareSize - Theme.of(context).extension<AppSizes>()!.margin,
                    color: Theme.of(context).colorScheme.primaryContainer,
                    child: ListView.builder(
                      padding: EdgeInsets.zero,
                      shrinkWrap: false,
                      itemCount: options.length,
                      itemBuilder: (BuildContext context, int index) {
                        final String option = options.elementAt(index);
                        return InkWell(
                            onTap: () {
                              onSelected(option);
                            },
                            child: Row(children: [const Spacer(), Text(option, style: Theme.of(context).textTheme.bodyLarge)])
                        );
                      },
                    ),
                  )
              );
            },
          );
        }
    );
  }

  @override
  Widget build(BuildContext context) {
    var gameMetadata = GlobalState.gameMetadataState.getMetadata();
    return Scaffold(
        resizeToAvoidBottomInset: true,
        body: SingleChildScrollView(
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.stretch,
          mainAxisSize: MainAxisSize.min,
          children: [
            SaveDialogRow(Case(CaseState.black, 255, false), playerSearch(context, true)),
            const Margin.internal(),
            SaveDialogRow(Case(CaseState.white, 255, false), playerSearch(context, false)),
            const Margin.internal(),
            SaveDialogRow(
                const CaseWithCup(),
                _InputFormField(
                  onChanged: (String value) => GlobalState.gameMetadataState.setTournament(value),
                  inputFormatters: [LengthLimitingTextInputFormatter(26)],
                )
            ),
            const Margin.internal(),
            SaveDialogRow(
                Text("Year", style: Theme.of(context).textTheme.bodyLarge!),
                _InputFormField(
                  initialValue: gameMetadata.year.toString(),
                  onChanged: (String year) => GlobalState.gameMetadataState.setYear(int.parse(year)),
                  keyboardType: TextInputType.numberWithOptions(signed: false, decimal: false),
                  inputFormatters: [LengthLimitingTextInputFormatter(4), IntInputFormatter()],
                )
            ),
            const Margin.internal(),
            SaveDialogRow(
                Text("Score", style: Theme.of(context).textTheme.bodyLarge!),
                Row(
                    crossAxisAlignment: CrossAxisAlignment.center,
                    children: [
                      const Spacer(),
                      SizedBox(
                          width: 0.8 * Theme.of(context).extension<AppSizes>()!.squareSize,
                          child: ListenableBuilder(
                              listenable: GlobalState.gameMetadataState,
                              builder: (context, widget) => TextFormField(
                                key: Key('Score black: ${gameMetadata.black_disks.toString()}'), // To update the form
                                initialValue: gameMetadata.black_disks.toString(),
                                style: Theme.of(context).textTheme.bodyLarge!,
                                onChanged: (String value) => GlobalState.gameMetadataState.setScore(value, true),
                                onEditingComplete: GlobalState.gameMetadataState.finishedSetScore,
                                onTapOutside: (unused) => GlobalState.gameMetadataState.finishedSetScore(),
                                textAlign: TextAlign.left,
                                onTapAlwaysCalled: true,
                                keyboardType: TextInputType.numberWithOptions(signed: false, decimal: false),
                                inputFormatters: [LengthLimitingTextInputFormatter(2), IntInputFormatter(minValue: 0, maxValue: 64)],
                              )
                          )
                      ),
                      Text("-", style: Theme.of(context).textTheme.bodyLarge!),
                      SizedBox(
                          width: 0.8 * Theme.of(context).extension<AppSizes>()!.squareSize,
                          child: ListenableBuilder(
                              listenable: GlobalState.gameMetadataState,
                              builder: (context, widget) => TextFormField(
                                key: Key('Score white: ${gameMetadata.black_disks.toString()}'), // To update the form
                                initialValue: (64 - gameMetadata.black_disks).toString(),
                                style: Theme.of(context).textTheme.bodyLarge!,
                                onChanged: (String value) => GlobalState.gameMetadataState.setScore(value, false),
                                textAlign: TextAlign.right,
                                keyboardType: TextInputType.numberWithOptions(signed: false, decimal: false),
                                inputFormatters: [LengthLimitingTextInputFormatter(2), IntInputFormatter()],
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
                  onPressed: () async {
                    await GlobalState.save();
                    Navigator.pop(context);
                  },
                  text: "Save",
                  textStyle: Theme.of(context).textTheme.bodyLarge,
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