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
import 'package:othello_sensei/state.dart';
import 'package:othello_sensei/utils.dart';

import '../widgets_spacers/app_sizes.dart';
import '../widgets_windows/main.dart';

enum CaseState {
  black,
  white,
  empty
}

String formatProb(double prob) {
  return '${(100 * prob).toStringAsFixed(0)}%';
}

String getRemaining(double prob, double proof) {
  if (prob > 0.01) {
    return formatProb(prob);
  } else if (proof != 0) {
    return prettyPrintDouble(proof);
  } else {
    return '-';
  }
}

class AnnotationRow extends StatelessWidget {
  final String text;
  final TextStyle style;

  const AnnotationRow({required this.text, required this.style, super.key});

  @override
  Widget build(BuildContext context) {
    var texts = text.split(' ');
    if (texts.length == 1) {
      return Text(text, style: style);
    } else {
      assert(texts.length == 2);
      return Row(
        children: [
          Expanded(
            child: Center(
              child: Text(texts[0], style: style)
            )
          ),
          Expanded(
            child: Center(
              child: Text(texts[1], style: style)
            )
          ),
        ]
      );
    }
  }
}

class Case extends StatelessWidget {
  final CaseState state;
  final Function playMove;
  final Function undo;
  final int index;
  const Case(this.state, this.index, this.playMove, this.undo, {super.key});

  Widget showAnnotations(ColorScheme colorScheme) {
    var annotations = GlobalState.annotations[index];
    if (annotations.annotations == null || annotations.annotations!.square == 255) {
      return const Text("");
    }
    var annotation = annotations.annotations!;
    var eval = -annotations.getEval();
    var bestEval = GlobalState.globalAnnotations.getEval();
    var color = eval > bestEval - 1 ? colorScheme.onSecondaryContainer : colorScheme.onPrimaryContainer;

    String evalText = '${eval < 0 ? "-" : "+"}${formatEval(eval.abs())}';
    String line1;
    String line2;
    String line3 = prettyPrintDouble(annotation.descendants.toDouble());

    if (Main.tabName[GlobalState.preferences.get('Tab')] == 'Archive') {
      line1 = annotation.num_thor_games < 10000 ? annotation.num_thor_games.toString() : prettyPrintDouble(annotation.num_thor_games.toDouble());
      line2 = evalText;
    } else {
      line1 = evalText;
      line2 = "${getRemaining(annotation.prob_upper_eval, annotation.disproof_number_upper)} ${getRemaining(annotation.prob_lower_eval, annotation.proof_number_lower)}";
    }

    return LayoutBuilder(
        builder: (BuildContext context, BoxConstraints constraints) {
          return Center(
              child: Column(
                  mainAxisAlignment: MainAxisAlignment.center,
                  crossAxisAlignment: CrossAxisAlignment.center,
                  children: [
                      Text(
                          line1,
                          style: TextStyle(
                              fontSize: Theme.of(context).textTheme.bodyMedium!.fontSize,
                              fontWeight: FontWeight.bold,
                              height: 1,
                              color: color
                          ),
                      ),
                      SizedBox(height: 0.2 * Theme.of(context).textTheme.bodySmall!.fontSize!),
                      AnnotationRow(
                          text: line2,
                          style: TextStyle(
                              fontSize: Theme.of(context).textTheme.bodySmall!.fontSize,
                              color: color
                          ),
                      ),
                      AnnotationRow(
                          text: line3,
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
    switch (state) {
      case CaseState.empty:
        return null;
      case CaseState.black:
        fill = colorScheme.surface;
        break;
      case CaseState.white:
        fill = colorScheme.surfaceVariant;
        break;
    }
    return Transform.scale(
        scale: 0.9,
        child: Container(
          decoration: BoxDecoration(
              color: fill,
              shape: BoxShape.circle,
          ),
        )
    );
  }

  @override
  Widget build(BuildContext context) {
    var squareSize = Theme.of(context).extension<AppSizes>()!.squareSize!;
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
