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
import 'package:othello_sensei_flutter/state.dart';
import 'package:othello_sensei_flutter/utils.dart';

import '../ffi_engine.dart';

enum CaseState {
  black,
  white,
  empty
}

class Case extends StatelessWidget {
  final CaseState state;
  final Function playMove;
  final Function undo;
  final int index;
  final double squareSize;
  const Case(this.state, this.index, this.squareSize, this.playMove, this.undo, {super.key});

  static const Color diskBorder = Color(0xff1a1c19);
  static const Color diskBlack = Color(0xff1a1c19);
  static const Color diskWhite = Color(0xfffcfdf6);

  Widget showAnnotations(ColorScheme colorScheme) {
    var annotations = GlobalState.annotations[index];
    if (annotations.annotations == null || annotations.annotations!.square == 255) {
      return const Text("");
    }
    var annotation = annotations.annotations!;
    var eval = -annotation.eval;
    var color = eval > annotations.bestEval - 1 ? colorScheme.onSecondaryContainer : colorScheme.onPrimaryContainer;

    return LayoutBuilder(
        builder: (BuildContext context, BoxConstraints constraints) {
          return Center(
              child: Column(
                  mainAxisAlignment: MainAxisAlignment.center,
                  crossAxisAlignment: CrossAxisAlignment.center,
                  children: [
                      Text(
                          "${eval < 0 ? '-' : '+'}${eval.abs().toStringAsFixed(2)}",
                          style: TextStyle(
                              fontSize: Theme.of(context).textTheme.bodyLarge!.fontSize,
                              fontWeight: FontWeight.bold,
                              height: 1,
                              color: color
                          ),
                      ),
                      Text(
                          prettyPrintDouble(annotation.descendants.toDouble()),
                          style: TextStyle(
                              fontSize: Theme.of(context).textTheme.bodySmall!.fontSize,
                              height: 1.5,
                              color: color
                          ),
                      ),
                      Text(
                          "[${annotation.lower}, ${annotation.upper}]",
                          style: TextStyle(
                              fontSize: Theme.of(context).textTheme.bodySmall!.fontSize,
                              color: color,
                          ),
                      )
                  ]
              )
          );
        }
    );
  }

  static Widget? getDisk(CaseState state, ColorScheme colorScheme) {
    Color fill;
    Color border;
    switch (state) {
      case CaseState.empty:
        return null;
      case CaseState.black:
        fill = diskBlack;
        border = diskBorder;
        break;
      case CaseState.white:
        fill = diskWhite;
        border = diskBorder;
        break;
    }
    return Transform.scale(
        scale: 0.9,
        child: Container(
          decoration: BoxDecoration(
              color: fill,
              shape: BoxShape.circle,
              border: Border.all(width: 1, color: border)
          ),
        )
    );
  }

  @override
  Widget build(BuildContext context) {
    var colorScheme = Theme.of(context).colorScheme;
    List<Widget> children = [
      Container(
        decoration: BoxDecoration(
          color: colorScheme.primaryContainer,
          border: Border.all(
            color: colorScheme.background,
            width: 0.5,
          ),
        ),
      ),
    ];
    Widget? disk = getDisk(state, colorScheme);
    if (disk != null) {
      children.add(disk);
    }
    if (index != 255) {
      children.add(
          ListenableBuilder(
            listenable: GlobalState.annotations[index],
            builder: (BuildContext context, Widget? child) => showAnnotations(colorScheme),
          )
      );
      children.add(
          GestureDetector(
            onTapDown: (TapDownDetails details) => playMove(),
            onSecondaryTapDown: (TapDownDetails details) => undo(),
          )
      );
    }

    return SizedBox(
        width: squareSize,
        height: squareSize,
        child: Stack(children: children)
    );
  }
}
