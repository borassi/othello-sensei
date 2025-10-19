/*
 * Copyright (c) 2025 Michele Borassi
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
import 'package:othello_sensei/widgets_sidebar/score_graph.dart';

import '../utils.dart';
import '../widgets_board/case.dart';

String _generateNumbers(int value, int beforeDecimal, int afterDecimal) {
  String result = value.toString() * beforeDecimal;
  if (afterDecimal <= 0) {
    return result;
  }
  return '$result.${value.toString() * afterDecimal}';
}

List<Widget> annotationsFirstRowEval() {
  var result = <Widget>[];
  for (int i = 0; i < 10; ++i) {
    for (bool large in [true, false]) {
      for (String prepend in ['+', '-']) {
        result.add(AnnotationFirstRow(
            text: '$prepend${_generateNumbers(i, 2, large ? 0 : 2)}',
            color: Colors.red,
            large: large
        ));
      }
    }
  }
  return result;
}

List<Widget> annotationsFirstRowArchive() {
  var result = <Widget>[];
  for (int i = 0; i < 10; ++i) {
    for (String append in ['k', 'M', 'G']) {
      result.add(AnnotationFirstRow(
          text: '${_generateNumbers(i, 3, 0)}$append',
          color: Colors.red,
          large: false
      ));
    }
  }
  return result;
}

List<Widget> annotationsOtherRowsEval(BuildContext context) {
  var result = <Widget>[];
  for (int i = 0; i < 10; ++i) {
    result.add(
        SizedBox(
            width: squareSize(context),
            child: AnnotationOtherRows(
                text: '${_generateNumbers(i, 3, 0)}M ${_generateNumbers(i, 3, 0)}M',
                color: Colors.red
            )
    ));
  }
  return result;
}

List<Widget> graphSideText(BuildContext context) {
  return [GraphTickLabel(25), GraphTickLabel(-25)];
}

class HiddenItemsForSize extends StatelessWidget {
  final Widget child;

  const HiddenItemsForSize(this.child, {super.key});

  @override
  Widget build(BuildContext context) {
    return Stack(
      children: [
        Visibility(
            visible: false,
            maintainState: true,
            maintainAnimation: true,
            maintainSize: true,
            child: Stack(
                children:
                    annotationsFirstRowEval() +
                    annotationsFirstRowArchive() +
                    annotationsOtherRowsEval(context) +
                    graphSideText(context)
            )
        ),
        child,
      ]
    );
  }
}