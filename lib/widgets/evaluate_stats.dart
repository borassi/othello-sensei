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

import '../state.dart';
import 'case.dart';

class EvaluateStats extends StatelessWidget {

  @override
  Widget build(BuildContext context) {
    return ListenableBuilder(
      listenable: GlobalState.globalAnnotations,
      builder: (BuildContext context, Widget? widget) => Table(
        columnWidths: const <int, TableColumnWidth>{
          0: IntrinsicColumnWidth(),
          1: IntrinsicColumnWidth(),
        },
        defaultVerticalAlignment: TableCellVerticalAlignment.middle,
        children: <TableRow>[
          TableRow(
            children: <Widget>[
              const TableCell(child: Text("Positions"),),
              TableCell(child: TextAnnotationListenable(textFunction: GlobalState.globalAnnotations.getPositions)),
            ],
          ),
          TableRow(
            children: <Widget>[
              const TableCell(child: Text("Time"),),
              TableCell(child: TextAnnotationListenable(textFunction: GlobalState.globalAnnotations.getTimeString)),
            ],
          ),
          TableRow(
            children: <Widget>[
              const TableCell(child: Text("Positions / sec:"),),
              TableCell(child: TextAnnotationListenable(textFunction: GlobalState.globalAnnotations.getPositionsPerSec)),
            ],
          ),
          const TableRow(
            children: <Widget>[
              TableCell(child: Text("Missing"),),
              TableCell(child: Text("200T"),),
            ],
          ),
          TableRow(
            children: <Widget>[
              const TableCell(child: Text("Empties"),),
              TableCell(
                child: TextListenable(listenable: GlobalState.board, textFunction: () => "${GlobalState.board.emptySquares()}"),
              ),
            ],
          )
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