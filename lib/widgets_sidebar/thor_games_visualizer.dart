/*
 * Copyright (c) 2024. Michele Borassi
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

import 'package:ffi/ffi.dart';
import 'package:flutter/material.dart';

import '../ffi/ffi_engine.dart';
import '../state.dart';
import '../utils.dart';

class TableCase extends StatelessWidget {
  final String text;
  final Alignment? alignment;

  const TableCase(this.text, {super.key, this.alignment});
  @override
  Widget build(BuildContext context) {
    var textStyle = Theme.of(context).textTheme.bodySmall!;
    return Container(
      alignment: alignment ?? Alignment.center,
      // color: Colors.red,
      constraints: BoxConstraints(minHeight: textStyle.fontSize! * 2),
      child: Text(text, style: textStyle),
    );
  }
}

TableRow getRow(BuildContext context, ThorGame game) {
  return TableRow(
    children: <Widget>[
      TableCase(moveToString(game.moves[game.moves_played])),
      TableCase(game.year.toString()),
      TableCase(game.black.cast<Utf8>().toDartString()),
      TableCase(' ${game.score} '),
      const TableCase('-'),
      TableCase(' ${64 - game.score} '),
      TableCase(game.white.cast<Utf8>().toDartString()),
    ],
  );
}

class ThorGamesVisualizer extends StatelessWidget {

  const ThorGamesVisualizer({super.key});

  @override
  Widget build(BuildContext context) {
    return ListenableBuilder(
      listenable: GlobalState.globalAnnotations,
      builder: (BuildContext context, Widget? widget) {
        if (GlobalState.globalAnnotations.annotations == null) {
          return Table();
        }
        var annotations = GlobalState.globalAnnotations.annotations!;
        var textStyle = Theme.of(context).textTheme.bodySmall!;
        return Column(
          children: [
            Container(
              alignment: Alignment.center,
              constraints: BoxConstraints(minHeight: textStyle.fontSize! * 3),
              child: Text(
                  'Showing ${annotations.ref.num_example_thor_games} / ${annotations.ref.num_thor_games} games',
                  style: TextStyle(
                    fontSize: Theme.of(context).textTheme.bodySmall!.fontSize,
                    fontWeight: FontWeight.bold,
                  )
              ),
            ),
            Expanded(
              child: SingleChildScrollView(
                child: Table(
                  border: TableBorder(
                    top: BorderSide(color: Theme.of(context).colorScheme.onPrimaryContainer),
                    horizontalInside: BorderSide(color: Theme.of(context).colorScheme.onPrimaryContainer),
                    bottom: BorderSide(color: Theme.of(context).colorScheme.onPrimaryContainer)
                  ),
                  defaultVerticalAlignment: TableCellVerticalAlignment.middle,
                  // dataRowMaxHeight: 2 * textHeight,
                  columnWidths: const {
                    0: FlexColumnWidth(4),
                    1: FlexColumnWidth(6),
                    2: FlexColumnWidth(22),
                    3: FlexColumnWidth(3),
                    4: FlexColumnWidth(2),
                    5: FlexColumnWidth(3),
                    6: FlexColumnWidth(22),
                    // 3: FlexColumnWidth(2),
                  },
                  children: List.generate(
                    annotations.ref.num_example_thor_games,
                    (i) => getRow(context, annotations.ref.example_thor_games.elementAt(i).ref)
                  ),
                )
              ),
            ),
          ]
        );
      }
    );
  }

}