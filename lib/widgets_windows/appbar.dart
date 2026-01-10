/*
 * Copyright (c) 2023-2025 Michele Borassi
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
import 'package:othello_sensei/ffi/ffi_engine.dart';
import 'package:othello_sensei/state.dart';
import 'package:othello_sensei/widgets_windows/saved_games_folders.dart';
import 'package:othello_sensei/widgets_windows/sensei_dialog.dart';
import 'package:othello_sensei/widgets_windows/settings.dart';

import '../drive/drive_downloader.dart';
import '../utils.dart';
import '../widgets_spacers/app_sizes.dart';
import '../widgets_spacers/margins.dart';
import '../widgets_utils/text.dart';
import '../widgets_utils/misc.dart';

enum MenuItem {
  newGame,
  newGameXotSmall,
  newGameXotLarge,
  open,
  save,
  copy,
  paste,
  analyze,
  downloadLatestBook,
  downloadLatestBookMedium,
  downloadLatestBookSmall,
  downloadLatestArchive,
  settings,
  xotAutomatic,
  xotAlways,
  xotNever,
  setupBoard,
  editSavedGamesFolders,
}

void handleMenuItem(BuildContext context, MenuItem item) async {
  switch (item) {
    case MenuItem.newGame:
      GlobalState.newGame();
      return;
    case MenuItem.newGameXotSmall:
      GlobalState.newGameXot(true);
      return;
    case MenuItem.newGameXotLarge:
      GlobalState.newGameXot(false);
      return;
    case MenuItem.save:
      await GlobalState.setDetailsAndSave(context);
      return;
    case MenuItem.open:
      await GlobalState.open();
      return;
    case MenuItem.copy:
      await copy();
      return;
    case MenuItem.paste:
      await paste();
      return;
    case MenuItem.analyze:
      GlobalState.stop();
      if (GlobalState.globalAnnotations.existsAnalyzedGame()) {
        resetAnalyzedGame();
      } else {
        analyze();
      }
      return;
    case MenuItem.downloadLatestBookSmall:
      GlobalState.stop();
      downloadBookSmall(context);
      return;
    case MenuItem.downloadLatestBookMedium:
      GlobalState.stop();
      downloadBookMedium(context);
      return;
    case MenuItem.downloadLatestBook:
      GlobalState.stop();
      downloadBook(context);
      return;
    case MenuItem.downloadLatestArchive:
      GlobalState.stop();
      downloadArchive(context);
      return;
    case MenuItem.settings:
      GlobalState.stop();
      Navigator.push(
        context,
        MaterialPageRoute(builder: (context) => Settings()),
      );
      return;
    case MenuItem.setupBoard:
      GlobalState.setupBoardState.setSettingUpBoard(true);
      GlobalState.evaluate();
      return;
    case MenuItem.xotAlways:
      GlobalState.ffiEngine.SetXOTState(GlobalState.ffiMain, XOTState.XOT_STATE_ALWAYS);
      GlobalState.evaluate();
      return;
    case MenuItem.xotAutomatic:
      GlobalState.ffiEngine.SetXOTState(GlobalState.ffiMain, XOTState.XOT_STATE_AUTOMATIC);
      GlobalState.evaluate();
      return;
    case MenuItem.xotNever:
      GlobalState.ffiEngine.SetXOTState(GlobalState.ffiMain, XOTState.XOT_STATE_NEVER);
      GlobalState.evaluate();
      return;
    case MenuItem.editSavedGamesFolders:
      if (GlobalState.thorMetadata.ptr != null) {
        GlobalState.stop();
        Navigator.push(
          context,
          MaterialPageRoute(builder: (context) => SavedGamesFoldersWindow()),
        );
      } else {
        showSenseiDialog(SenseiDialog(content: 'Cannot edit the archive folders while the archive is loading. Please retry in a few seconds.'));
      }
      return;
  }
}

class SenseiIconButton extends StatelessWidget {
  final String tooltip;
  final IconData icon;
  final void Function() onPressed;

  const SenseiIconButton({super.key, required this.tooltip, required this.icon, required this.onPressed});

  @override
  Widget build(BuildContext context) {
    return Semantics(
      label: tooltip,
      child: IconButton(
        color: Theme.of(context).colorScheme.onPrimaryContainer,
        icon: Icon(icon, size: 24),
        tooltip: tooltip,
        onPressed: onPressed
      ),
    );
  }
}

PopupMenuButton<MenuItem> _buildMenu({
  required BuildContext context,
  required List<PopupMenuItem<MenuItem>> items,
  required double width,
  String? tooltip,
  Icon? icon,
  String? text,
  int depth = 0}) {
  return PopupMenuButton<MenuItem>(
    icon: icon,
    constraints: BoxConstraints.tightFor(width: width),
    onSelected: (MenuItem i) {
      for (int j = 0; j < depth; ++j) {
        Navigator.pop(context);
      }
      handleMenuItem(context, i);
    },
    itemBuilder: (context) => items,
    tooltip: tooltip ?? "",
    child: icon != null ? null : Align(
      alignment: Alignment.centerLeft,
      child: Row(children: [
        const Margin.internal(),
        MediumText(text ?? ""),
        const Spacer()
      ])
    ),
  );
}

PopupMenuItem<MenuItem> _buildMenuItem(BuildContext context, {MenuItem? menuItem, String? text, Widget? child, bool? checked}) {
  var height = Theme.of(context).extension<AppSizes>()!.squareSize;
  List<Widget> checkBoxChildren;
  if (checked == null) {
    checkBoxChildren = [];
  } else if (checked) {
    checkBoxChildren = [Icon(Icons.check_box, color: Theme.of(context).colorScheme.onPrimaryContainer)];
  } else {
    checkBoxChildren = [Icon(Icons.check_box_outline_blank, color: Theme.of(context).colorScheme.onPrimaryContainer)];
  }
  child = child ?? Align(
      alignment: Alignment.centerLeft,
      child: Row(
        children: checkBoxChildren +
        [
          const Margin.internal(),
          MediumText(
              text ?? camelCaseToSpaces(menuItem?.name ?? ''),
          ),
          const Spacer(),
        ],
      )
  );
  return PopupMenuItem<MenuItem>(
    padding: EdgeInsets.zero,
    value: menuItem,
    child: SizedBox(
        height: height,
        width: 4 * height,
        child: child
    ),
  );
}

class SenseiAppBar extends StatelessWidget {

  const SenseiAppBar({super.key});

  Widget buildRow(BuildContext context) {
    var appSizes = Theme.of(context).extension<AppSizes>()!;
    List<Widget> icons = [];
    bool showIcons = GlobalState.preferences.get('Controls position') == 'App bar';

    if (showIcons) {
      icons += [
        const SenseiIconButton(
          icon: Icons.keyboard_double_arrow_left_rounded,
          tooltip: 'Back to the game / the first position',
          onPressed: GlobalState.toLastImportantNode,
        ),
        const SenseiIconButton(
          icon: Icons.chevron_left_rounded,
          tooltip: 'Undo',
          onPressed: GlobalState.undo,
        ),
        const SenseiIconButton(
          icon: Icons.chevron_right_rounded,
          tooltip: 'Redo',
          onPressed: GlobalState.redo,
        ),
        const SenseiIconButton(
          icon: Icons.stop_rounded,
          tooltip: 'Stop',
          onPressed: GlobalState.stop,
        ),
      ];
    }
    var menuItemHeight = Theme.of(context).extension<AppSizes>()!.squareSize;
    var row = Row(
      crossAxisAlignment: CrossAxisAlignment.center,
      children: icons + [
        ListenableBuilder(
          listenable: Listenable.merge([GlobalState.actionWhenPlay, GlobalState.globalAnnotations]),
          builder: (BuildContext context, Widget? widget) => SingleChildScrollView(
            child: _buildMenu(
              tooltip: "Menu",
              context: context,
              icon: Icon(
                Icons.more_vert_rounded,
                color: Theme.of(context).colorScheme.onPrimaryContainer
              ),
              width: 4 * menuItemHeight,
              items: [
                _buildMenuItem(context, menuItem: MenuItem.newGame),
                _buildMenuItem(
                  context,
                  child: _buildMenu(
                    context: context,
                    width: 3 * menuItemHeight,
                    depth: 1,
                    items: [
                      _buildMenuItem(context, menuItem: MenuItem.newGameXotSmall, text: 'Small list'),
                      _buildMenuItem(context, menuItem: MenuItem.newGameXotLarge, text: 'Large list'),
                    ],
                    text:'New XOT game',
                  )
                ),
                _buildMenuItem(context, menuItem: MenuItem.open),
                _buildMenuItem(context, menuItem: MenuItem.save),
                _buildMenuItem(context, menuItem: MenuItem.copy),
                _buildMenuItem(context, menuItem: MenuItem.paste),
                _buildMenuItem(context, menuItem: MenuItem.analyze, text: GlobalState.globalAnnotations.existsAnalyzedGame() ? 'Reset analyzed game' : 'Analyze'),
                _buildMenuItem(
                    context,
                    child: _buildMenu(
                      context: context,
                      width: 3 * menuItemHeight,
                      text: 'XOT errors',
                      depth: 1,
                      items: [
                        _buildMenuItem(
                            context,
                            menuItem: MenuItem.xotAutomatic,
                            checked: GlobalState.ffiEngine.GetXOTState(GlobalState.ffiMain) == XOTState.XOT_STATE_AUTOMATIC,
                            text: 'Automatic',
                        ),
                        _buildMenuItem(
                          context,
                          menuItem: MenuItem.xotAlways,
                          checked: GlobalState.ffiEngine.GetXOTState(GlobalState.ffiMain) == XOTState.XOT_STATE_ALWAYS,
                          text: 'Always XOT',
                        ),
                        _buildMenuItem(
                          context,
                          menuItem: MenuItem.xotNever,
                          checked: GlobalState.ffiEngine.GetXOTState(GlobalState.ffiMain) == XOTState.XOT_STATE_NEVER,
                          text: 'Never XOT',
                        ),
                      ]
                    )
                ),
                _buildMenuItem(context, menuItem: MenuItem.setupBoard),
                _buildMenuItem(
                    context,
                    child: _buildMenu(
                        depth: 1,
                        context: context,
                        items: [
                          _buildMenuItem(context, menuItem: MenuItem.downloadLatestBook, text: 'Large (>700MB)'),
                          _buildMenuItem(context, menuItem: MenuItem.downloadLatestBookMedium, text: 'Medium (~100MB)'),
                          _buildMenuItem(context, menuItem: MenuItem.downloadLatestBookSmall, text: 'Small (~10MB)'),
                        ],
                        width: 3 * menuItemHeight,
                        text: 'Download latest book'
                    ),
                ),
                _buildMenuItem(context, menuItem: MenuItem.downloadLatestArchive),
              ] + (
                  canLookupFiles() ?
                  [
                    _buildMenuItem(context, menuItem: MenuItem.editSavedGamesFolders, text: 'Edit archive folders'),
                  ] : []
              ) + [
                _buildMenuItem(context, menuItem: MenuItem.settings),
              ]
            )
          )
        )
      ]
    );
    var title = <Widget>[const Margin.side(), Expanded(child: WindowTitle('Sensei'))];
    return SizedBox(
        height: appSizes.appBarHeight,
        child: Row(crossAxisAlignment: CrossAxisAlignment.center, children: title + (GlobalState.setupBoardState.settingUpBoard ? [] : <Widget>[row] + [const Margin.side()]))
    );
  }

  @override
  Widget build(BuildContext context) {
    var appSizes = Theme.of(context).extension<AppSizes>()!;
    return Container(
      color: Theme.of(context).colorScheme.primaryContainer,
      width: appSizes.brokenAppBar() ? appSizes.sideBarWidth : appSizes.width,
      height: kToolbarHeight,
      child: ListenableBuilder(
        listenable: GlobalState.setupBoardState,
        builder: (BuildContext context, Widget? widget) {
          return buildRow(context);
        }
      )
    );
  }
}