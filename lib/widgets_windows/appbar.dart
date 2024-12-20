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
import 'package:othello_sensei/state.dart';
import 'package:othello_sensei/widgets_windows/settings.dart';

import '../drive/drive_downloader.dart';
import '../utils.dart';
import '../widgets_spacers/app_sizes.dart';
import '../widgets_spacers/margins.dart';

enum MenuItem {
  newGame,
  copy,
  paste,
  analyze,
  senseiEvaluates,
  senseiIsInactive,
  downloadLatestBook,
  downloadLatestArchive,
  settings
}

void handleMenuItem(BuildContext context, MenuItem item) async {
  switch (item) {
    case MenuItem.newGame:
      GlobalState.newGame();
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
    case MenuItem.downloadLatestBook:
      GlobalState.stop();
      downloadBook(context);
      return;
    case MenuItem.downloadLatestArchive:
      GlobalState.stop();
      downloadArchive(context);
      return;
    case MenuItem.senseiEvaluates:
      GlobalState.actionWhenPlay.setActionWhenPlay(ActionWhenPlay.eval);
      GlobalState.evaluate();
      return;
    case MenuItem.senseiIsInactive:
      GlobalState.actionWhenPlay.setActionWhenPlay(ActionWhenPlay.none);
      return;
    case MenuItem.settings:
      GlobalState.stop();
      Navigator.push(
        context,
        MaterialPageRoute(builder: (context) => Settings()),
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
          onPressed: GlobalState.toAnalyzedGameOrLastChoice,
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
    var row = Row(
      crossAxisAlignment: CrossAxisAlignment.center,
      children: icons + [
        Semantics(
          label: 'Show menu',
          child: ListenableBuilder(
            listenable: Listenable.merge([GlobalState.actionWhenPlay, GlobalState.globalAnnotations]),
            builder: (BuildContext context, Widget? widget) => SingleChildScrollView(
              child: PopupMenuButton<MenuItem>(
                icon: Icon(
                  Icons.more_vert_rounded,
                  color: Theme.of(context).colorScheme.onPrimaryContainer
                ),
                onSelected: (MenuItem i) { handleMenuItem(context, i); },
                itemBuilder: (context) => MenuItem.values.map((MenuItem i) {
                  if (i == MenuItem.analyze) {
                    if (GlobalState.globalAnnotations.existsAnalyzedGame()) {
                      return PopupMenuItem<MenuItem>(value: i, child: Text('Reset analyzed game'));
                    } else {
                      return PopupMenuItem<MenuItem>(value: i, child: Text('Analyze'));
                    }
                  } else if (i == MenuItem.senseiEvaluates || i == MenuItem.senseiIsInactive) {
                    return CheckedPopupMenuItem<MenuItem>(
                      value: i,
                      checked:
                          (i == MenuItem.senseiEvaluates && GlobalState.actionWhenPlay.actionWhenPlay == ActionWhenPlay.eval) ||
                          (i == MenuItem.senseiIsInactive && GlobalState.actionWhenPlay.actionWhenPlay == ActionWhenPlay.none),
                      child: Text(camelCaseToSpaces(i.name))
                    );
                  }
                  return PopupMenuItem<MenuItem>(value: i, child: Text(camelCaseToSpaces(i.name)));
                }).toList()
              )
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
          color: Theme.of(context).colorScheme.onPrimaryContainer,
          height: 1.0,
        ),

      ),
      const Spacer()];
    return Row(children: title + <Widget>[row]);
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
        child: buildRow(context),
    );
  }
}