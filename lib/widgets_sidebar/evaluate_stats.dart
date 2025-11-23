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
import '../utils.dart';
import '../widgets_utils/text.dart';

TableRow getRow(String name, ListenableBuilder listenable) {
  return TableRow(
    children: <Widget>[
      TableCell(child: MediumText(name)),
      TableCell(
        child: Align(
          alignment: Alignment.centerRight,
          child: listenable
        ),
      ),
    ]
  );
}

String getPositionsText(EvaluationStats stats) {
  int totalPositions = stats.nVisited + stats.nVisitedBook;
  if (totalPositions > 0) {
    return 'Positions: ${prettyPrintDouble(totalPositions.toDouble())}';
  } else {
    return '';
  }
}

String getTimeText(EvaluationStats stats) {
  if (stats.seconds > 0) {
    return 'Pos / sec: ${prettyPrintDouble(stats.nVisited / stats.seconds)}';
  } else {
    return '';
  }
}

String getPosPerSecText(EvaluationStats stats) {
  if (stats.nVisitedBook > 0) {
    if (stats.nVisited > 0) {
      return 'Book + Evaluate';
    }
    return 'Book';
  } else if (stats.nVisited > 0) {
    return 'Time: ${stats.seconds.toStringAsFixed(1)} sec';
  } else {
    return '';
  }
}

class EvaluateStats extends StatelessWidget {
  const EvaluateStats({super.key});

  @override
  Widget build(BuildContext context) {
    return ListenableBuilder(
      listenable: Listenable.merge([GlobalState.globalAnnotations, GlobalState.board]),
      builder: (BuildContext context, Widget? widget) {
        EvaluationStats stats = GlobalState.globalAnnotations.getEvaluationStats();
        return Column(
          crossAxisAlignment: CrossAxisAlignment.center,
          children: [
            Row(
              children: [
                MediumText(getPositionsText(stats)),
                const Spacer(),
                MediumText(getTimeText(stats)),
              ]
            ),
            SizedBox(height: 10),
            Row(
                children: [
                  MediumText(getPosPerSecText(stats)),
                  const Spacer(),
                  MediumText("Empties: ${GlobalState.board.emptySquares()}")
                ]
            ),
          ]
        );
      }
    );
  }
}
