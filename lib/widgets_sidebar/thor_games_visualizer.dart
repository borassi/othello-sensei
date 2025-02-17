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
  final int flex;
  final Alignment? alignment;

  const TableCase(this.text, this.flex, {super.key, this.alignment});
  @override
  Widget build(BuildContext context) {
    var textStyle = Theme.of(context).textTheme.bodySmall!;
    return Expanded(
      flex: flex,
      child: Container(
        alignment: alignment ?? Alignment.center,
        constraints: BoxConstraints(minHeight: textStyle.fontSize! * 2),
        child: Text(text, style: textStyle),
      )
    );
  }
}

Widget getRow(BuildContext context, ThorGame game) {
  void playOneMove() {
    if (game.moves_played < 60 && game.moves[game.moves_played] != 255) {
      GlobalState.ffiEngine.PlayMove(GlobalState.ffiMain, game.moves[game.moves_played]);
    }
    GlobalState.evaluate();
  }
  void playWholeGame() {
    for (int i = game.moves_played; i < 60; ++i) {
      if (game.moves[i] != 255) {
        GlobalState.ffiEngine.PlayMove(GlobalState.ffiMain, game.moves[i]);
      }
    }
    GlobalState.evaluate();
  }

  return GestureDetector(
    onTap: playOneMove,
    onLongPress: playWholeGame,
    onDoubleTap: playWholeGame,
    behavior: HitTestBehavior.opaque,
    child: Row(
      children: <Widget>[
        TableCase(game.moves_played == 60 ? '--' : moveToString(game.moves[game.moves_played]), 4),
        TableCase(game.year.toString(), 6),
        TableCase(game.black.cast<Utf8>().toDartString(), 22),
        TableCase(' ${game.score} ', 3),
        TableCase('-', 2),
        TableCase(' ${64 - game.score} ', 3),
        TableCase(game.white.cast<Utf8>().toDartString(), 22),
      ]
    )
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
                child: Column(
                  children: List.generate(
                    2 * annotations.ref.num_example_thor_games + 1,
                    (i) => i % 2 == 0 ?
                      Divider(
                        color: Theme.of(context).colorScheme.onPrimaryContainer,
                        height: 1
                      ) :
                      getRow(context, annotations.ref.example_thor_games[i ~/ 2])
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