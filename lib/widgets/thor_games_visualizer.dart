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
import 'dart:ui';

import 'package:ffi/ffi.dart';
import 'package:flutter/material.dart';

import '../ffi_engine.dart';
import '../state.dart';
import '../utils.dart';

TableRow getRow(BuildContext context, ThorGame game) {
  return TableRow(
    children: <Widget>[
      Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [Text(moveToString(game.moves[game.moves_played]), style: Theme.of(context).textTheme.bodySmall)]
      ),
      Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [Text(game.year.toString(), style: Theme.of(context).textTheme.bodySmall)]
      ),
      Column(
        crossAxisAlignment: CrossAxisAlignment.end,
        children: [
          Text(game.black.cast<Utf8>().toDartString(), style: Theme.of(context).textTheme.bodySmall),
          Text(game.white.cast<Utf8>().toDartString(), style: Theme.of(context).textTheme.bodySmall),
        ]
      ),
      Column(
        crossAxisAlignment: CrossAxisAlignment.end,
        children: [
          Text(game.score.toString(), style: Theme.of(context).textTheme.bodySmall),
          Text((64 - game.score).toString(), style: Theme.of(context).textTheme.bodySmall),
        ]
      ),
    ],
  );
}

class ThorGamesVisualizer extends StatelessWidget {
  final double squareSize;

  const ThorGamesVisualizer(this.squareSize, {super.key});

  @override
  Widget build(BuildContext context) {
    return ListenableBuilder(
      listenable: GlobalState.globalAnnotations,
      builder: (BuildContext context, Widget? widget) {
        if (GlobalState.globalAnnotations!.annotations == null) {
          return Table();
        }
        var fontWidth = Theme.of(context).textTheme.bodySmall!.fontSize! * 0.5;
        var textHeight = Theme.of(context).textTheme.bodySmall!.fontSize! * 2;
        return Column(
          children: [
            SizedBox(height: 0.25 * squareSize),
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
                  columnWidths: {
                    0: FixedColumnWidth(4 * fontWidth),
                    1: FixedColumnWidth(6 * fontWidth),
                    2: FixedColumnWidth(20 * fontWidth),
                    3: FixedColumnWidth(6 * fontWidth),
                  },
                  children: List.generate(
                    GlobalState.globalAnnotations!.annotations!.num_example_thor_games,
                    (i) => getRow(context, GlobalState.globalAnnotations.annotations!.example_thor_games.elementAt(i).ref)
                  ),
                )
              ),
            ),
            SizedBox(height: 0.25 * squareSize),
          ]
        );
      }
    );
  }

}