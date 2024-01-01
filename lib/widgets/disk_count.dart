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

import '../state.dart';
import 'board.dart';
import 'case.dart';

Widget getCase(BuildContext context, CaseState state, double squareSize) {
  var textColor = state == CaseState.black ? Colors.white : Colors.black;
  var text = state == CaseState.black ?
      "${GlobalState.board.blackDisks()}" :
      state == CaseState.white ?
          "${GlobalState.board.whiteDisks()}" :
          "${GlobalState.board.emptySquares()}";
  return Stack(
      alignment: Alignment.center,
      children: [
        Case(
          state,
          255,
          squareSize,
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

class DiskCount extends FixedWidthWidget {
  const DiskCount(super.squareSize, {super.key});

  @override
  Widget buildChild(BuildContext context) {
    return ListenableBuilder(
      listenable: GlobalState.board,
      builder: (BuildContext context, Widget? widget) => Row(
        mainAxisAlignment: MainAxisAlignment.center,
        children: [
          getCase(context, CaseState.black, squareSize),
          ConstrainedBox(
              constraints: BoxConstraints(maxHeight: squareSize),
              child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    Text("Error", style: Theme.of(context).textTheme.bodySmall),
                    ListenableBuilder(
                      listenable: GlobalState.globalAnnotations,
                      builder: (BuildContext context, Widget? widget) => Text(
                          "12", style: Theme.of(context).textTheme.bodyLarge!
                      )
                    )
                  ]
              )
          ),
          Spacer(),
          ConstrainedBox(
              constraints: BoxConstraints(maxHeight: squareSize),
              child: Column(
                  crossAxisAlignment: CrossAxisAlignment.end,
                  children: [
                    Text("Error", style: Theme.of(context).textTheme.bodySmall),
                    ListenableBuilder(
                      listenable: GlobalState.globalAnnotations,
                      builder: (BuildContext context, Widget? widget) => Text(
                          "12", style: Theme.of(context).textTheme.bodyLarge!
                      )
                    )
                  ]
              )
          ),
          // getCase(CaseState.empty, squareSize),
          getCase(context, CaseState.white, squareSize)
        ]
      )
    );
  }
}