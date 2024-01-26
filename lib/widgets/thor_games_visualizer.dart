/*
 * Copyright (c) 2024. Michele Borassi
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

TableRow getRow(String move, int year, String black, String white, int blackDisks, int whiteDisks) {
  return TableRow(
    children: <Widget>[
      TableCell(child: Text(move)),
      TableCell(child: Text(year.toString())),
      TableCell(child: Text(black)),
      TableCell(child: Text(blackDisks.toString() + ' - ' + whiteDisks.toString())),
      TableCell(child: Text(white)),
    ]
  );
}

class ThorGamesVisualizer extends StatelessWidget {
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
          getRow("e6", 2023, "Michele Borassi", "Di Mattei Alessandr", 64, 0),
        ],
      )
    );
  }

}