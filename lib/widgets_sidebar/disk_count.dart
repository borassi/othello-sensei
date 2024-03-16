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

import '../utils.dart';
import '../widgets_spacers/app_sizes.dart';
import '../widgets_spacers/margins.dart';
import '../state.dart';
import '../widgets_board/case.dart';

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

class DiskCountWithError extends StatelessWidget {
  const DiskCountWithError({super.key});

  Widget widget(BuildContext context, bool black) {
    return Column(
      mainAxisSize: MainAxisSize.min,
      children: [
        Spacer(),
        Text("Error", style: Theme.of(context).textTheme.bodySmall),
        Spacer(),
        ListenableBuilder(
          listenable: GlobalState.globalAnnotations,
          builder: (BuildContext context, Widget? widget) {
            var (errorBlack, errorWhite, hasNaN) = GlobalState.globalAnnotations.getErrors();
            return Text(
              (hasNaN ? "≥" : "") + formatEval(black ? errorBlack : errorWhite),
              style: TextStyle(
                fontSize: Theme.of(context).textTheme.bodyLarge!.fontSize!,
                height: 1.0
              )
            );
          }
        ),
        Spacer(),
      ]
    );
  }

  @override
  Widget build(BuildContext context) {
    var squareSize = Theme.of(context).extension<AppSizes>()!.squareSize!;
    return Row(
      crossAxisAlignment: CrossAxisAlignment.center,
      children: [
        const DiskCount(true),
        const Margin(),
        SizedBox(height: squareSize, child: widget(context, true)),
        const Spacer(),
        SizedBox(height: squareSize, child: widget(context, false)),
        const Margin(),
        const DiskCount(false),
      ]
    );
  }
}