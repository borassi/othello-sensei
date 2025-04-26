/*
 * Copyright 2023-2025 Michele Borassi
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
import 'package:othello_sensei/ffi/ffi_engine.dart';
import 'package:othello_sensei/state.dart';
import 'package:othello_sensei/utils.dart';
import 'package:othello_sensei/widgets_utils/hide_inactive.dart';

import '../widgets_spacers/app_sizes.dart';

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
      return Text(text.replaceAll('_', ' '), style: style);
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

class Annotations extends StatelessWidget {
  final int index;
  const Annotations(this.index, {super.key});


  @override
  Widget build(BuildContext context) {
    var colorScheme = Theme.of(context).colorScheme;
    var annotations = GlobalState.annotations[index];
    if (annotations.annotations == null) {
      return const Text("");
    }
    var annotation = annotations.annotations!;
    var eval = annotations.getEval(false);
    var bestEval = GlobalState.globalAnnotations.bestEval;

    var delta = GlobalState.preferences.get('Highlight distance from best move');
    var bestMoveGreen = GlobalState.preferences.get('Best move green, other yellow');
    var color = (eval > bestEval - delta) != bestMoveGreen ? colorScheme.onSecondaryContainer : colorScheme.onPrimaryContainer;

    String evalText = '${eval < 0 ? "-" : "+"}${formatEval(eval.abs(), false)}';
    String line1;
    String line2;
    String line3 = '';
    switch(annotation.provenance) {
      case AnnotationsProvenance.BOOK:
      case AnnotationsProvenance.CHILD_BOOK:
        line3 = 'bk_';
        break;
      case AnnotationsProvenance.CHILD_MIXED:
      case AnnotationsProvenance.EVALUATE_MIXED:
        line3 = '(bk)_';
        break;
      case AnnotationsProvenance.EVALUATE:
      case AnnotationsProvenance.CHILD_EVALUATE:
      case AnnotationsProvenance.GAME_OVER:
        break;
    }
    var descendants = annotation.descendants + annotation.descendants_book;
    line3 += descendants < 100 ? descendants.toString() : prettyPrintDouble(descendants.toDouble());
    var archive = GlobalState.preferences.get('Active tab') == 1 && annotation.father.ref.num_thor_games > 0;
    var showExtra = archive || GlobalState.preferences.get('Show extra data in evaluate mode');
    var roundEvaluation = GlobalState.preferences.get('Round evaluations') != 'Never';


    if (archive) {
      line1 = annotation.num_thor_games < 10000 ? annotation.num_thor_games.toString() : prettyPrintDouble(annotation.num_thor_games.toDouble());
      line2 = evalText;
    } else {
      line1 = evalText;
      line2 = "${getRemaining(annotation.prob_upper_eval, annotation.disproof_number_upper)} ${getRemaining(annotation.prob_lower_eval, annotation.proof_number_lower)}";
    }

    var extraInfo = <Widget>[];
    if (showExtra) {
      extraInfo = <Widget>[
        AnnotationRow(
          text: line2,
          style: TextStyle(
            fontSize: Theme.of(context).textTheme.bodySmall!.fontSize,
            color: color,
            height: 1.6,
          ),
        ),
        AnnotationRow(
          text: line3,
          style: TextStyle(
            fontSize: Theme.of(context).textTheme.bodySmall!.fontSize,
            color: color,
            height: 1,
          ),
        )
      ];
    }
    var mainFontSize = Theme.of(context).textTheme.bodyMedium!.fontSize!;
    if (roundEvaluation && !showExtra) {
      mainFontSize = Theme.of(context).textTheme.bodyLarge!.fontSize!;
    }

    return LayoutBuilder(
      builder: (BuildContext context, BoxConstraints constraints) {
        return Center(
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            crossAxisAlignment: CrossAxisAlignment.center,
            children: <Widget>[
              Text(
                line1,
                style: TextStyle(
                  fontSize: mainFontSize,
                  fontWeight: FontWeight.bold,
                  height: 1,
                  color: color
                ),
              ),
              // Hack: the font ascent and descent are different and this is the
              // only way to center the numbers AFAIK.
              SizedBox(height: mainFontSize * 0.03),
            ] + extraInfo
          )
        );
      }
    );
  }
}

bool highlightCase(int index) {
  if (index == 255) {
    return false;
  }
  var annotations = GlobalState.annotations[index].annotations;
  var globalAnnotations = GlobalState.globalAnnotations.annotations;
  if (annotations == null || globalAnnotations == null) {
    return false;
  }
  if (GlobalState.preferences.get('Highlight next move in analysis')
      && globalAnnotations.ref.next_state_in_analysis != nullptr) {
    return globalAnnotations.ref.next_state_in_analysis.ref.move == index;
  }
  return GlobalState.preferences.get('Highlight next moves outside analysis') &&
      annotations.first_child != nullptr && annotations.first_child.ref.valid;
}

class Case extends StatelessWidget {
  final CaseState state;
  final bool isLastMove;
  final int index;
  const Case(this.state, this.index, this.isLastMove, {super.key});

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

  void onTapDown() {
    if (GlobalState.setupBoardState.settingUpBoard) {
      switch(GlobalState.setupBoardState.onClick) {
        case CaseState.black:
          GlobalState.ffiEngine.SetBlackSquare(GlobalState.ffiMain, index);
          return;
        case CaseState.white:
          GlobalState.ffiEngine.SetWhiteSquare(GlobalState.ffiMain, index);
          return;
        case CaseState.empty:
          GlobalState.ffiEngine.SetEmptySquare(GlobalState.ffiMain, index);
          return;
      }
    }
    GlobalState.playMove(index, false);
  }

  void onSecondaryTapDown() {
    if (GlobalState.setupBoardState.settingUpBoard) {
      switch(GlobalState.setupBoardState.onClick) {
        case CaseState.black:
          GlobalState.ffiEngine.SetWhiteSquare(GlobalState.ffiMain, index);
          return;
        case CaseState.white:
          GlobalState.ffiEngine.SetBlackSquare(GlobalState.ffiMain, index);
          return;
        case CaseState.empty:
          GlobalState.ffiEngine.SetEmptySquare(GlobalState.ffiMain, index);
          return;
      }
    }
    GlobalState.undo();
  }

  @override
  Widget build(BuildContext context) {
    return ListenableBuilder(
      listenable: index == 255 ? Listenable.merge([]) : GlobalState.annotations[index],
      builder: (BuildContext context, Widget? child) {
        var squareSize = Theme.of(context).extension<AppSizes>()!.squareSize;
        var colorScheme = Theme.of(context).colorScheme;
        List<Widget> children = [
          Container(
            decoration: BoxDecoration(
              color: highlightCase(index) ? colorScheme.secondaryContainer : colorScheme.primaryContainer,
              border: Border.all(
                color: colorScheme.background,
                width: 0.5,
              ),
            ),
          )
        ];
        Widget? disk = getDisk(state, colorScheme);
        if (disk != null) {
          children.add(disk);
        }
        if (isLastMove) {
          var color = state == CaseState.black ? colorScheme.surfaceVariant : colorScheme.surface;
          var marker = GlobalState.preferences.get('Last move marker');
          switch (marker) {
            case 'None':
              break;
            case 'Number':
              children.add(
                Align(
                  alignment: Alignment.center,
                  child: Text(
                    '${60 - GlobalState.board.emptySquares()}',
                    style: TextStyle(
                      fontSize: Theme.of(context).textTheme.bodyMedium!.fontSize!,
                      fontWeight: FontWeight.bold,
                      height: 1,
                      color: color
                    ),
                  )
                )
              );
              break;
            case 'Dot':
              children.add(
                Align(
                  alignment: Alignment.center,
                  child: Container(
                    height: 0.14 * squareSize,
                    width: 0.14 * squareSize,
                    decoration: BoxDecoration(
                      color: color,
                      shape: BoxShape.circle,
                    ),
                  )
                )
              );
            default:
              throw Exception('Invalid last move marker preference value: $marker');
          }
        }
        if (index != 255) {
          children.add(
              ListenableBuilder(
                listenable: GlobalState.annotations[index],
                builder: (BuildContext context, Widget? child) => Annotations(index),
              )
          );
          children.add(
            GestureDetector(
              onTapDown: (TapDownDetails details) => onTapDown(),
              onSecondaryTapDown: (TapDownDetails details) => onSecondaryTapDown(),
            )
          );
        }

        return SizedBox(
          width: squareSize,
          height: squareSize,
          child: Stack(children: children)
        );
      }
    );
  }
}
