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
import 'package:othello_sensei/widgets/fixed_width_widget.dart';

import '../main.dart';
import '../state.dart';
import 'board.dart';
import 'case.dart';

Widget getCase(BuildContext context, bool black) {
  var textColor = black ? Colors.white : Colors.black;
  var text = black ? "${GlobalState.board.blackDisks()}" : "${GlobalState.board.whiteDisks()}";
  return Stack(
      alignment: Alignment.center,
      children: [
        Case(
          black ? CaseState.black : CaseState.white,
          255,
          () => {},
          () => {},
        ),
        Text(
          text,
          style: TextStyle(
              fontSize: Theme.of(context).textTheme.bodyLarge!.fontSize,
              fontWeight: FontWeight.bold,
              height: 1,
              color: textColor,
          ),
        )
      ]
  );
}

class DiskCount extends StatelessWidget {
  final bool black;

  const DiskCount(this.black, {key});

  @override
  Widget build(BuildContext context) {
    return ListenableBuilder(
      listenable: GlobalState.board,
      builder: (BuildContext context, Widget? widget) => getCase(context, black)
    );
  }
}

class DiskCountWithError extends FixedWidthWidget {
  const DiskCountWithError({super.key});

  Widget widget(BuildContext context, bool black) {
    return Column(
      crossAxisAlignment: black ? CrossAxisAlignment.start : CrossAxisAlignment.end,
      children: [
        Text("Error", style: Theme.of(context).textTheme.bodySmall),
        ListenableBuilder(
          listenable: GlobalState.globalAnnotations,
          builder: (BuildContext context, Widget? widget) => Text(
              GlobalState.globalAnnotations.getError(black).toStringAsFixed(2),
              style: Theme.of(context).textTheme.bodyLarge
          )
        )
      ]
    );
  }

  @override
  Widget buildChild(BuildContext context) {
    return HorizontalFlexWithMargins(
      children: [
        const DiskCount(true),
        widget(context, true),
        const Spacer(),
        widget(context, false),
        const DiskCount(false),
      ]
    );
  }
}