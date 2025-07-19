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
import 'package:othello_sensei/widgets_windows/settings.dart';

import '../drive/drive_downloader.dart';
import '../widgets_spacers/app_sizes.dart';
import '../widgets_spacers/margins.dart';

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
      return;
    case MenuItem.xotAutomatic:
      GlobalState.ffiEngine.SetXOTState(GlobalState.ffiMain, XOTState.XOT_STATE_AUTOMATIC);
      return;
    case MenuItem.xotNever:
      GlobalState.ffiEngine.SetXOTState(GlobalState.ffiMain, XOTState.XOT_STATE_NEVER);
      return;
    case MenuItem.editSavedGamesFolders:
      GlobalState.stop();
      Navigator.push(
        context,
        MaterialPageRoute(builder: (context) => SavedGamesFoldersWindow()),
      );
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

class SenseiAppBar extends StatelessWidget implements PreferredSizeWidget {

  @override
  Size get preferredSize => const Size.fromHeight(kToolbarHeight);

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
    const padding = EdgeInsets.symmetric(horizontal: 12);
    var textStyle = Theme.of(context).textTheme.bodyMedium;
    var row = Row(
      crossAxisAlignment: CrossAxisAlignment.center,
      children: icons + [
        ListenableBuilder(
          listenable: Listenable.merge([GlobalState.actionWhenPlay, GlobalState.globalAnnotations]),
          builder: (BuildContext context, Widget? widget) => SingleChildScrollView(
            child: PopupMenuButton<MenuItem>(
              icon: Icon(
                Icons.more_vert_rounded,
                color: Theme.of(context).colorScheme.onPrimaryContainer
              ),
              tooltip: "Menu",
              onSelected: (MenuItem i) { handleMenuItem(context, i); },
              itemBuilder: (context) => [
                PopupMenuItem<MenuItem>(
                  padding: padding,
                  value: MenuItem.newGame,
                  child: Text('New game', style: textStyle),
                ),
                PopupMenuItem<MenuItem>(
                  padding: EdgeInsets.zero,
                  onTap: () {},
                  child: PopupMenuButton<MenuItem>(
                    tooltip: "",
                    child: Container(
                      height: kMinInteractiveDimension,
                      width: double.infinity,
                      alignment: Alignment.centerLeft,
                      padding: padding,
                      child: Text('New XOT game', style: textStyle),
                    ),
                    onSelected: (MenuItem i) { Navigator.pop(context); handleMenuItem(context, i); },
                    itemBuilder: (context) => [
                      PopupMenuItem<MenuItem>(
                        padding: padding,
                        value: MenuItem.newGameXotSmall,
                        child: Text('Small list', style: textStyle),
                      ),
                      PopupMenuItem<MenuItem>(
                        padding: padding,
                        value: MenuItem.newGameXotLarge,
                        child: Text('Large list', style: textStyle),
                      )
                    ]
                  )
                ),
                PopupMenuItem<MenuItem>(
                    padding: padding,
                    value: MenuItem.open,
                    child: Text('Open', style: textStyle)),
                PopupMenuItem<MenuItem>(
                    padding: padding,
                    value: MenuItem.save,
                    child: Text('Save', style: textStyle)),
                PopupMenuItem<MenuItem>(
                  padding: padding,
                  value: MenuItem.copy,
                  child: Text('Copy', style: textStyle)),
                PopupMenuItem<MenuItem>(
                  padding: padding,
                  value: MenuItem.paste,
                  child: Text('Paste', style: textStyle)),
                PopupMenuItem<MenuItem>(
                  padding: padding,
                  value: MenuItem.analyze,
                  child: Text(GlobalState.globalAnnotations.existsAnalyzedGame() ? 'Reset analyzed game' : 'Analyze', style: textStyle)),
                PopupMenuItem<MenuItem>(
                  padding: EdgeInsets.zero,
                  onTap: () {},
                  child: PopupMenuButton<MenuItem>(
                    padding: EdgeInsets.zero,
                    onSelected: (MenuItem i) { Navigator.pop(context); handleMenuItem(context, i); },
                    tooltip: "",
                    child: Container(
                      height: kMinInteractiveDimension,
                      width: double.infinity,
                      alignment: Alignment.centerLeft,
                      padding: padding,
                      child: Text('XOT errors', style: textStyle)
                    ),
                    itemBuilder: (context) => [
                      CheckedPopupMenuItem<MenuItem>(
                        value: MenuItem.xotAutomatic,
                        checked: GlobalState.ffiEngine.GetXOTState(GlobalState.ffiMain) == XOTState.XOT_STATE_AUTOMATIC,
                        child: Text('Automatic', style: textStyle),
                      ),
                      CheckedPopupMenuItem<MenuItem>(
                        value: MenuItem.xotAlways,
                        checked: GlobalState.ffiEngine.GetXOTState(GlobalState.ffiMain) == XOTState.XOT_STATE_ALWAYS,
                        child: Text('Always XOT', style: textStyle),
                      ),
                      CheckedPopupMenuItem<MenuItem>(
                        value: MenuItem.xotNever,
                        checked: GlobalState.ffiEngine.GetXOTState(GlobalState.ffiMain) == XOTState.XOT_STATE_NEVER,
                        child: Text('Never XOT', style: textStyle),
                      )
                    ]
                  )
                ),
                PopupMenuItem<MenuItem>(value: MenuItem.setupBoard, child: Text('Setup board', style: textStyle)),
                PopupMenuItem<MenuItem>(
                    padding: EdgeInsets.zero,
                    onTap: () {},
                    child: PopupMenuButton<MenuItem>(
                        padding: EdgeInsets.zero,
                        onSelected: (MenuItem i) { Navigator.pop(context); handleMenuItem(context, i); },
                        tooltip: "",
                        child: Container(
                            height: kMinInteractiveDimension,
                            width: double.infinity,
                            alignment: Alignment.centerLeft,
                            padding: padding,
                            child: Text('Download latest book', style: textStyle)
                        ),
                        itemBuilder: (context) => [
                          PopupMenuItem<MenuItem>(
                            value: MenuItem.downloadLatestBook,
                            child: Text('Large (600MB and growing)', style: textStyle),
                          ),
                          PopupMenuItem<MenuItem>(
                            value: MenuItem.downloadLatestBookMedium,
                            child: Text('Medium (~100MB)', style: textStyle),
                          ),
                          PopupMenuItem<MenuItem>(
                            value: MenuItem.downloadLatestBookSmall,
                            child: Text('Small (~10MB)', style: textStyle),
                          )
                        ]
                    )
                ),
                PopupMenuItem<MenuItem>(value: MenuItem.downloadLatestArchive, child: Text('Download latest archive', style: textStyle)),
                PopupMenuItem<MenuItem>(value: MenuItem.settings, child: Text('Settings', style: textStyle)),
                PopupMenuItem<MenuItem>(value: MenuItem.editSavedGamesFolders, child: Text('Edit saved games folders', style: textStyle)),
              ]
            )
          )
        )
      ]
    );
    var availableWidth = appSizes.brokenAppBar() ? appSizes.sideBarWidth : appSizes.width;
    var busyWidth = showIcons ? AppSizes.minFullAppBarSize : 0;
    var title = availableWidth < busyWidth ? <Widget>[const Spacer()] : <Widget>[
      const Margin.side(),
      Text(
        'Sensei',
        style: TextStyle(
          fontSize: 20,
          height: 1.0,
        ),

      ),
      const Spacer()];
    return Row(children: title + (GlobalState.setupBoardState.settingUpBoard ? [] : <Widget>[row]));
  }

  @override
  Widget build(BuildContext context) {
    var appSizes = Theme.of(context).extension<AppSizes>()!;
    return Container(
      alignment: Alignment.center,
      color: Theme.of(context).colorScheme.primaryContainer,
      width: appSizes.brokenAppBar() ? appSizes.sideBarWidth : appSizes.width,
      // we can set width here with conditions
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