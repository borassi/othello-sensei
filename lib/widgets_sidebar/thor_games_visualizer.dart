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

import 'package:flutter/material.dart';

import '../ffi/ffi_engine.dart';
import '../state.dart';
import '../utils.dart';
import '../widgets_spacers/app_sizes.dart';
import '../widgets_spacers/margins.dart';
import '../widgets_utils/text.dart';
import '../widgets_spacers/text_sizes.dart';

class TableCase extends StatelessWidget {
  final String text;
  final int flex;
  final Alignment alignment;
  final TextStyle? textStyle;
  final TextType textType;

  const TableCase(this.text, this.flex, {this.textType = TextType.medium, this.textStyle, this.alignment = Alignment.center, super.key});
  @override
  Widget build(BuildContext context) {
    var totalWidth = Theme.of(context).extension<AppSizes>()!.sideBarWidth;
    return Container(
      width: totalWidth * flex / 58,
      alignment: alignment,
      child: AnyText(textType, text, style: textStyle)
    );
  }
}

Widget getRow(BuildContext context, ThorGame game) {
  void playOneMove() {
    GlobalState.ffiEngine.PlayOneMove(GlobalState.ffiMain, game);
    GlobalState.evaluate();
  }
  void playWholeGame() {
    GlobalState.ffiEngine.OpenThorGame(GlobalState.ffiMain, game);
    GlobalState.evaluate();
  }

  var boldTextStyle = TextStyle(fontWeight: FontWeight.bold);
  var tournamentStyle = TextStyle(
      color: Theme.of(context).colorScheme.onSurface
  );
  String tournament = cStringToString(game.tournament);
  return GestureDetector(
    onTap: playOneMove,
    onLongPress: playWholeGame,
    // onDoubleTap: playWholeGame,
    behavior: HitTestBehavior.opaque,
    child: SizedBox(height: minButtonSize(context), child: Column(
      children: [
        const Spacer(),
        Row(
          children: <Widget>[
            TableCase(cStringToString(game.black), 24, alignment: Alignment.centerLeft, textStyle: game.score > 32 ? boldTextStyle : null),
            TableCase(' ${game.score} ', 4, textStyle: game.score > 32 ? boldTextStyle : null),
            TableCase('-', 2),
            TableCase(' ${64 - game.score} ', 4, textStyle: game.score < 32 ? boldTextStyle : null),
            TableCase(cStringToString(game.white), 24, alignment: Alignment.centerRight, textStyle: game.score < 32 ? boldTextStyle : null),
          ]
        ),
        const Spacer(),
        Row(
          children: <Widget>[
            TableCase(textType: TextType.small, game.moves_played == 60 ? '--' : moveToString(game.moves[game.moves_played]), 6, alignment: Alignment.centerLeft, textStyle: tournamentStyle),
            TableCase(textType: TextType.small, '${game.year} $tournament', 46, textStyle: tournamentStyle),
            TableCase(textType: TextType.small, '', 6, alignment: Alignment.centerLeft, textStyle: tournamentStyle),
          ]
        ),
        const Spacer(),
      ]
    ))
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
          return Align(
              alignment: Alignment.topCenter,
              child: MediumText(
                'No game',
                style: TextStyle(fontWeight: FontWeight.bold)
              )
          );
        }
        return Column(
          children: [
            Container(
              alignment: Alignment.center,
              child: MediumText(
                'Showing ${annotations.ref.num_example_thor_games} / ${annotations.ref.num_thor_games} games',
                style: TextStyle(fontWeight: FontWeight.bold)
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