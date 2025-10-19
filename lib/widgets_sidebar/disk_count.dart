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

import 'package:dropdown_search/dropdown_search.dart';
import 'package:flutter/material.dart';
import 'package:othello_sensei/widgets_sidebar/player.dart';

import '../utils.dart';
import '../widgets_spacers/app_sizes.dart';
import '../widgets_spacers/margins.dart';
import '../state.dart';
import '../widgets_board/case.dart';
import '../widgets_spacers/text_size_groups.dart';
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
        LargeText(
          text,
          style: TextStyle(fontWeight: FontWeight.bold, height: 1, color: textColor),
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
        SmallText("Error"),
        const Spacer(),
        ListenableBuilder(
          listenable: GlobalState.globalAnnotations,
          builder: (BuildContext context, Widget? widget) {
            var (errorBlack, errorWhite, hasNaN) = GlobalState.globalAnnotations.getErrors();
            return LargeText(
              (hasNaN ? "≥" : "") + formatEval(black ? errorBlack : errorWhite, true),
            );
          }
        ),
        const Spacer(),
      ]
    );
  }

}

class GameFolderSearch extends StatelessWidget {
  const GameFolderSearch({super.key});

  @override
  Widget build(BuildContext context) {
    var squareSize = Theme.of(context).extension<AppSizes>()!.squareSize;
    var textStyle = Theme.of(context).textTheme.bodyMedium!;
    return DropdownSearch<String>(
      items: (f, cs) => ['All'] + GlobalState.thorMetadata.folders,
      selectedItem: GlobalState.thorMetadata.activeFolder,
      onChanged: (String? value) {
        if (value != null) {
          GlobalState.thorMetadata.setActiveFolder(value);
          GlobalState.evaluate();
        }
      },
      dropdownBuilder: (BuildContext context, String? s) {
        return MediumText(s);
      },
      // Hide the vertical triangle on the right.
      suffixProps: DropdownSuffixProps(
        dropdownButtonProps: DropdownButtonProps(isVisible: false),
      ),
      decoratorProps: DropDownDecoratorProps(
        decoration: InputDecoration(
            contentPadding: EdgeInsets.fromLTRB(0, 0, 0, 0),
            border: OutlineInputBorder(
                borderRadius: BorderRadius.all(Radius.circular(squareSize / 2))
            )
        ),
        expands: true,
        baseStyle: textStyle,
        textAlign: TextAlign.center
      ),
      popupProps: PopupProps.menu(
        menuProps: MenuProps(align: MenuAlign.bottomCenter),
        fit: FlexFit.loose,
        itemBuilder: (context, item, isDisabled, isSelected) =>
            MediumText(item, height: minButtonSize(context)),
      ),
    );
  }
}

class FiltersButton extends StatelessWidget {
  const FiltersButton({super.key});

  @override
  Widget build(BuildContext context) {
    var squareSize = Theme.of(context).extension<AppSizes>()!.squareSize;
    return SenseiButton(
        text: 'Filters',
        onPressed: () {
          GlobalState.stop();
          Navigator.push(
            context,
            MaterialPageRoute(builder: (context) => ThorFiltersWidget(squareSize)),
          );
        },
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
    var appSizes = Theme.of(context).extension<AppSizes>()!;
    var squareSize = appSizes.squareSize;
    var sideBarWidth = appSizes.sideBarWidth;
    List<Widget> content;
    switch(extraContent) {
      case DiskCountExtraContent.none:
        content = [const Spacer()];
        break;
      case DiskCountExtraContent.error:
        content = [const Margin.internal(), ShowError(true), const Spacer(), ShowError(false), const Margin.internal()];
        break;
      case DiskCountExtraContent.thor:
        content = [
          const Margin.internal(),
          Expanded(child: GameFolderSearch()),
          const Margin.internal(),
          Expanded(child: FiltersButton()),
          const Margin.internal()
        ];
        break;
      case DiskCountExtraContent.player:
        content = [
          const Margin.internal(),
          Expanded(child: PlayerWidget(true)),
          const Margin.internal(),
          Expanded(child: PlayerWidget(false)),
          const Margin.internal()
        ];
        break;
    }
    return SizedBox(
      height: squareSize,
      // Should be implicit, but we set it up so that the tap down below is
      // correct for sure.
      width: sideBarWidth,
      child: GestureDetector(
        behavior: HitTestBehavior.opaque,
        onTapDown: (TapDownDetails details) {
          if (details.localPosition.dx < sideBarWidth / 2) {
            maybeUndo();
          } else {
            maybeRedo();
          }
        },
        child: Row(
            crossAxisAlignment: CrossAxisAlignment.stretch,
            children: <Widget>[const DiskCount(true)] + content + [const DiskCount(false)]
        )
      )
    );
  }
}