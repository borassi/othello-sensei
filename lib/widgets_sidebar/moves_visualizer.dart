/*
 * Copyright 2025 Michele Borassi
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

import '../state.dart';
import '../utils.dart';
import '../widgets_board/case.dart';
import '../widgets_spacers/app_sizes.dart';

void setMove(dynamic update, double size, int elementsPerRow) {
  double x;
  double y;
  if (update is DragStartDetails || update is DragUpdateDetails) {
    x = update.localPosition.dx;
    y = update.localPosition.dy;
  } else {
    return;
  }
  var depth = (y / size).floor() * elementsPerRow + (x / size).floor();
  GlobalState.setCurrentMove(depth);
}

class MovesVisualizerCase extends StatelessWidget {
  final int moveNumber;
  final Move? move;
  final double size;

  const MovesVisualizerCase(this.moveNumber, this.move, this.size, {super.key});

  @override
  Widget build(BuildContext context) {
    var colorScheme = Theme.of(context).colorScheme;
    var textColor = colorScheme.surface;

    if (move != null) {
      textColor = move!.blackPlayer ? colorScheme.surfaceVariant : colorScheme.surface;
    }
    return Stack(
        alignment: AlignmentDirectional.center,
        children: [
          Case(
            move == null ? CaseState.empty : (move!.blackPlayer ? CaseState.black : CaseState.white),
            255,
            false,
            squareSize: size,
          ),
          Text(
              move == null ? "" : moveToString(move!.square),
              style: Theme.of(context).textTheme.bodyMedium!.merge(TextStyle(
                  color: textColor,
                  height: 1.0,
                  fontWeight: FontWeight.bold
              ))
          ),
        ]
    );
  }

}

class MovesVisualizer extends StatelessWidget {
  const MovesVisualizer({super.key});

  @override
  Widget build(BuildContext context) {
    var appSizes = Theme.of(context).extension<AppSizes>()!;
    var squareSize = appSizes.squareSize;

    return ListenableBuilder(
        listenable: GlobalState.board,
        builder: (BuildContext context, Widget? widget) {
          return LayoutBuilder(builder: (context, constraints) {
            var moves = GlobalState.board.allMoves;
            var children = <List<Widget>>[];
            var width = constraints.constrainWidth();
            var desiredMovesVisualizerSize = squareSize / 2;
            var numberPerRow = (width / desiredMovesVisualizerSize).ceil();
            if (numberPerRow >= 30) {
              numberPerRow = 60;
            } else if (numberPerRow >= 20) {
              numberPerRow = 30;
            } else if (numberPerRow >= 15) {
              numberPerRow = 20;
            } else if (numberPerRow >= 12) {
              numberPerRow = 15;
            } else if (numberPerRow >= 10) {
              numberPerRow = 12;
            } else {
              numberPerRow = 10;
            }
            var size = width / numberPerRow;
            for (int i = 0; i < 60; i += numberPerRow) {
              var currentChildren = <Widget>[];
              for (int j = 0; j < numberPerRow; ++j) {
                currentChildren.add(MovesVisualizerCase(i+j, moves.elementAtOrNull(i + j), size));
              }
              children.add(currentChildren);
            }
            return GestureDetector(
                onPanStart: (DragStartDetails details) => setMove(details, size, numberPerRow),
                onPanUpdate: (DragUpdateDetails details) => setMove(details, size, numberPerRow),
                onPanEnd: (var details) => GlobalState.evaluate,
                child: Column(
                  children: children.map((List<Widget> child) => Row(children: child)).toList()
                )
            );
          });
        }
    );
  }
}