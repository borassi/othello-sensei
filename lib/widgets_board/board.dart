/*
 * Copyright (c) 2023. Michele Borassi
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

import 'package:flutter/material.dart';
import '../widgets_spacers/app_sizes.dart';
import '../state.dart';
import 'case.dart';

class Coordinate extends StatelessWidget {
  final bool vertical;
  final int position;
  const Coordinate(this.vertical, this.position, {super.key});
  
  @override
  Widget build(BuildContext context) {
    String content;
    if (vertical) {
      content = '${position + 1}';
    } else {
      content = String.fromCharCode('A'.codeUnitAt(0) + position);
    }
    var appSizes = Theme.of(context).extension<AppSizes>()!;
    var squareSize = appSizes.squareSize;
    Widget contentWidget = Container();
    if (GlobalState.preferences.get('Margin size') == 'Coordin') {
      contentWidget = Center(
          child: Text(
            content,
            style: Theme.of(context).textTheme.bodyMedium!,
          )
      );
    }
    return SizedBox(
      width: vertical ? appSizes.sideMargin : squareSize,
      height: vertical ? squareSize : appSizes.sideMargin,
      child: contentWidget
    );
  }

}

class Board extends StatelessWidget {
  const Board({super.key});

  @override
  Widget build(BuildContext context) {
    var board = GlobalState.board;
    var colorScheme = Theme.of(context).colorScheme;
    var margin = Theme.of(context).extension<AppSizes>()!.sideMargin;
    return LayoutBuilder(
      builder: (BuildContext context, BoxConstraints constraints) {
        var squareSize = Theme.of(context).extension<AppSizes>()!.squareSize;
        return ListenableBuilder(
          listenable: GlobalState.board,
          builder: (BuildContext context, Widget? widget) => Stack(
            children: <Widget>[
              Table(
                defaultColumnWidth: FixedColumnWidth(squareSize),
                columnWidths: {0: FixedColumnWidth(margin)},
                children:
                  [TableRow(children: <Widget>[Container()] + List.generate(8, (y) => Coordinate(false, y)))] +
                  List.generate(8, (x) => TableRow(
                    children: <Widget>[Coordinate(true, x)] + List.generate(8, (y) {
                      var index = 63 - 8 * x - y;
                      return Case(getState(index, board), index, () => GlobalState.playMove(index), GlobalState.undo, index == GlobalState.board.lastMove);
                    })
                  ))
              )
            ] +
            List.generate(4, (index) => Positioned(
              left: margin + (2-0.1) * squareSize + (index % 2) * 4 * squareSize,
              top: margin + (2-0.1) * squareSize + (index ~/ 2) * 4 * squareSize,
              child:
              Container(
                decoration: BoxDecoration(
                  color: colorScheme.background,
                  shape: BoxShape.circle,
                ),
                width: squareSize * 0.2,
                height: squareSize * 0.2,
              )
            ))
          )
        );
      }
    );
  }
}

CaseState getState(int square, BoardState state) {
  if ((1 << square) & state.black() != 0) {
    return CaseState.black;
  } else if ((1 << square) & state.white() != 0) {
    return CaseState.white;
  } else {
    return CaseState.empty;
  }
}