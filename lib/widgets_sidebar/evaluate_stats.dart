/*
 * Copyright (c) 2023-2024. Michele Borassi
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

TableRow getRow(String name, ListenableBuilder listenable) {
  return TableRow(
    children: <Widget>[
      TableCell(child: Text(name)),
      TableCell(
        child: Align(
          alignment: Alignment.centerRight,
          child: listenable
        ),
      ),
    ]
  );
}

class EvaluateStats extends StatelessWidget {
  const EvaluateStats({super.key});

  @override
  Widget build(BuildContext context) {
    return ListenableBuilder(
      listenable: GlobalState.globalAnnotations,
      builder: (BuildContext context, Widget? widget) => Table(
        columnWidths: const <int, TableColumnWidth>{
          0: IntrinsicColumnWidth(),
          1: FlexColumnWidth(),
        },
        defaultVerticalAlignment: TableCellVerticalAlignment.middle,

        children: <TableRow>[
          getRow("Positions", TextAnnotationListenable(textFunction: GlobalState.globalAnnotations.getPositions)),
          getRow("Seconds", TextAnnotationListenable(textFunction: GlobalState.globalAnnotations.getTimeString)),
          getRow("Positions / second:", TextAnnotationListenable(textFunction: GlobalState.globalAnnotations.getPositionsPerSec)),
          getRow("Missing:", TextAnnotationListenable(textFunction: GlobalState.globalAnnotations.getMissing)),
          getRow("Empties:", TextListenable(listenable: GlobalState.board, textFunction: () => "${GlobalState.board.emptySquares()}")),
        ],
      )
    );
  }
}

class TextListenable extends ListenableBuilder {
  TextListenable({super.key, required super.listenable, required String? Function() textFunction}) : super(
      builder: (BuildContext context, Widget? widget) => Text(textFunction() ?? "-"));
}

class TextAnnotationListenable extends TextListenable {
  TextAnnotationListenable({required textFunction}) : super(listenable: GlobalState.globalAnnotations, textFunction: textFunction);
}