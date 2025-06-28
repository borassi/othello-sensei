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
import '../widgets_spacers/margins.dart';

class TableCase extends StatelessWidget {
  final String text;
  final int flex;
  final Alignment? alignment;
  final TextStyle? textStyle;

  const TableCase(this.text, this.flex, {this.textStyle, super.key, this.alignment});
  @override
  Widget build(BuildContext context) {
    var textStyle = Theme.of(context).textTheme.bodyMedium!.merge(this.textStyle);
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
      GlobalState.ffiEngine.PlayMove(GlobalState.ffiMain, game.moves[game.moves_played], false);
    }
    GlobalState.evaluate();
  }
  void playWholeGame() {
    for (int i = game.moves_played; i < 60; ++i) {
      if (game.moves[i] != 255) {
        GlobalState.ffiEngine.PlayMove(GlobalState.ffiMain, game.moves[i], false);
      }
    }
    GlobalState.evaluate();
  }

  var boldTextStyle = TextStyle(
      fontWeight: FontWeight.bold,
  );
  var tournamentStyle = Theme.of(context).textTheme.bodySmall!.merge(
    TextStyle(
      color: Theme.of(context).colorScheme.onSurface
    )
  );
  String tournament;
  try {
    tournament = game.tournament.cast<Utf8>().toDartString();
  } on FormatException {
    tournament = cStringToString(game.tournament);
  }
  return GestureDetector(
    onTap: playOneMove,
    onLongPress: playWholeGame,
    onDoubleTap: playWholeGame,
    behavior: HitTestBehavior.opaque,
    child: Row(
      children: [
        Expanded(
          flex: 40,
          child: Column(
            children: [
              Row(
                children: <Widget>[
                  TableCase(game.black.cast<Utf8>().toDartString(), 24, alignment: Alignment.centerLeft, textStyle: game.score > 32 ? boldTextStyle : null),
                  TableCase(' ${game.score} ', 4, textStyle: game.score > 32 ? boldTextStyle : null),
                  TableCase('-', 2),
                  TableCase(' ${64 - game.score} ', 4, textStyle: game.score < 32 ? boldTextStyle : null),
                  TableCase(game.white.cast<Utf8>().toDartString(), 24, alignment: Alignment.centerRight, textStyle: game.score < 32 ? boldTextStyle : null),
                ]
              ),
              Row(
                children: <Widget>[
                  TableCase(game.moves_played == 60 ? '--' : moveToString(game.moves[game.moves_played]), 3, alignment: Alignment.centerLeft, textStyle: tournamentStyle),
                  TableCase('${game.year} $tournament', 20, textStyle: tournamentStyle),
                  TableCase('', 3, alignment: Alignment.centerLeft, textStyle: tournamentStyle),
                ]
              )
            ]
          )
        ),
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
        if (annotations.ref.num_example_thor_games == 0) {
          return Text(
              'No game',
              style: TextStyle(
                fontSize: Theme.of(context).textTheme.bodyMedium!.fontSize,
                fontWeight: FontWeight.bold,
              ));
        }
        return Column(
          children: [
            Container(
              alignment: Alignment.center,
              child: Text(
                'Showing ${annotations.ref.num_example_thor_games} / ${annotations.ref.num_thor_games} games',
                style: TextStyle(
                  fontSize: Theme.of(context).textTheme.bodyMedium!.fontSize,
                  fontWeight: FontWeight.bold,
                )
              ),
            ),
            Margin.internal(),
            Expanded(
              child: SingleChildScrollView(
                child: Column(
                  children: List.generate(
                    2 * annotations.ref.num_example_thor_games + 1,
                    (i) => i % 2 == 0 ?
                      Divider(
                        color: Theme.of(context).colorScheme.onSurface,
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