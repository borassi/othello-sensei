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
import 'package:othello_sensei/widgets_sidebar/player.dart';

import '../utils.dart';
import '../widgets_spacers/app_sizes.dart';
import '../widgets_spacers/margins.dart';
import '../state.dart';
import '../widgets_board/case.dart';
import '../widgets_utils/misc.dart';
import '../widgets_windows/thor_filters.dart';

Widget getCase(BuildContext context, bool black) {
  var textColor = black ? Colors.white : Colors.black;
  var text = black ? "${GlobalState.board.blackDisks()}" : "${GlobalState.board.whiteDisks()}";
  return Stack(
      alignment: Alignment.center,
      children: [
        Case(
          black ? CaseState.black : CaseState.white,
          255,
          false
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

  const DiskCount(this.black, {super.key});

  @override
  Widget build(BuildContext context) {
    return ListenableBuilder(
      listenable: GlobalState.board,
      builder: (BuildContext context, Widget? widget) => getCase(context, black)
    );
  }
}

class ShowError extends StatelessWidget {
  final bool black;
  const ShowError(this.black, {super.key});

  @override
  Widget build(BuildContext context) {
    return Column(
      mainAxisSize: MainAxisSize.min,
      children: [
        const Spacer(),
        Text("Error", style: Theme.of(context).textTheme.bodySmall),
        const Spacer(),
        ListenableBuilder(
          listenable: GlobalState.globalAnnotations,
          builder: (BuildContext context, Widget? widget) {
            var (errorBlack, errorWhite, hasNaN) = GlobalState.globalAnnotations.getErrors();
            return Text(
              (hasNaN ? "≥" : "") + formatEval(black ? errorBlack : errorWhite, true),
              style: TextStyle(
                fontSize: Theme.of(context).textTheme.bodyLarge!.fontSize!,
                height: 1.0
              )
            );
          }
        ),
        const Spacer(),
      ]
    );
  }

}

class FiltersButton extends StatelessWidget {
  const FiltersButton({super.key});

  @override
  Widget build(BuildContext context) {
    var squareSize = Theme.of(context).extension<AppSizes>()!.squareSize;
    return Expanded(
        child: SenseiButton(
          text: 'Filters',
          onPressed: () {
            GlobalState.stop();
            Navigator.push(
              context,
              MaterialPageRoute(builder: (context) => ThorFiltersWidget(squareSize)),
            );
          },
        )
    );
  }
}

enum DiskCountExtraContent {
  error,
  thor,
  player,
  none,
}

class DiskCountWithExtraContent extends StatelessWidget {
  final DiskCountExtraContent extraContent;
  const DiskCountWithExtraContent(this.extraContent, {super.key});

  Widget widget(BuildContext context, bool black) {
    return ShowError(black);
  }

  static bool undoRedoEnabled() {
    return GlobalState.preferences.get('Use disk count to undo and redo');
  }

  static void maybeUndo() {
    if (undoRedoEnabled()) {
      GlobalState.undo();
    }
  }
  static void maybeRedo() {
    if (undoRedoEnabled()) {
      GlobalState.redo();
    }
  }

  @override
  Widget build(BuildContext context) {
    var squareSize = Theme.of(context).extension<AppSizes>()!.squareSize;
    List<Widget> leftContentWidget = [const Spacer()];
    List<Widget> rightContentWidget = [const Spacer()];
    List<Widget> centerContentWidget = [];
    switch(extraContent) {
      case DiskCountExtraContent.none:
        break;
      case DiskCountExtraContent.error:
        leftContentWidget = [const Margin.internal(), ShowError(true), const Spacer()];
        rightContentWidget = [const Spacer(), ShowError(false), const Margin.internal()];
        break;
      case DiskCountExtraContent.thor:
        centerContentWidget = [const Margin.internal(), FiltersButton(), const Margin.internal()];
        break;
      case DiskCountExtraContent.player:
        centerContentWidget = [const PlayerWidget(true), const Margin.internal(), const PlayerWidget(false)];
        break;
    }
    return SizedBox(
      height: squareSize,
      child: Row(
        crossAxisAlignment: CrossAxisAlignment.stretch,
        children: <Widget>[
          Expanded(
            child: GestureDetector(
              behavior: HitTestBehavior.opaque,
              onTapDown: (TapDownDetails details) => maybeUndo(),
              child: Row(
                  children: <Widget>[const DiskCount(true)] + leftContentWidget
              )
            )
          ),
        ] + centerContentWidget + [
          Expanded(
            child: GestureDetector(
              behavior: HitTestBehavior.opaque,
              onTapDown: (TapDownDetails details) => maybeRedo(),
              child: Row(
                  children: rightContentWidget + <Widget>[const DiskCount(false)]
              )
            )
          )
        ]
      )
    );
  }
}