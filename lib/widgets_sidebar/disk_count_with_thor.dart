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
import 'package:othello_sensei/widgets_windows/thor_filters.dart';

import '../widgets_spacers/app_sizes.dart';
import '../state.dart';
import '../widgets_board/case.dart';
import 'disk_count.dart';

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

class DiskCountsWithThor extends StatelessWidget {
  DiskCountsWithThor({super.key});

  @override
  Widget build(BuildContext context) {
    var squareSize = Theme.of(context).extension<AppSizes>()!.squareSize!;
    return SizedBox(
      height: squareSize,
      child: Row(
        mainAxisAlignment: MainAxisAlignment.center,
        crossAxisAlignment: CrossAxisAlignment.stretch,
        children: [
          const DiskCount(true),
          Expanded(
            child: TextButton(
              onPressed: () {
                GlobalState.stop();
                Navigator.push(
                  context,
                  MaterialPageRoute(builder: (context) => ThorFiltersWidget(squareSize)),
                );
              },
              child: Text('Filters', style: Theme.of(context).textTheme.bodyMedium!),
            )
          ),
          const DiskCount(false)
        ]
      )
    );
  }
}