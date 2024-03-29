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
import 'package:flutter/services.dart';
import 'package:othello_sensei/state.dart';
import 'package:othello_sensei/widgets_windows/settings.dart';

import '../utils.dart';

enum MenuItem {
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
    case MenuItem.copy:
      await copy();
      return;
    case MenuItem.paste:
      await pasteOrError(context);
      return;
    case MenuItem.analyze:
      GlobalState.stop();
      analyze();
      return;
    case MenuItem.downloadLatestBook:
      GlobalState.stop();
      GlobalState.driveDownloader.downloadBook(context);
      return;
    case MenuItem.downloadLatestArchive:
      GlobalState.stop();
      GlobalState.driveDownloader.downloadArchive(context);
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
  final Icon icon;
  final void Function() onPressed;

  const SenseiIconButton({super.key, required this.tooltip, required this.icon, required this.onPressed});

  @override
  Widget build(BuildContext context) {
    return Semantics(
      label: tooltip,
      child: IconButton(
        icon: icon,
        tooltip: tooltip,
        onPressed: onPressed,
      ),
    );
  }
}

class SenseiAppBar extends StatelessWidget implements PreferredSizeWidget {

  @override
  Size get preferredSize => const Size.fromHeight(kToolbarHeight);

  const SenseiAppBar({super.key});

  @override
  Widget build(BuildContext context) {
    return AppBar(
        backgroundColor: Theme.of(context).colorScheme.primaryContainer,
        foregroundColor: Theme.of(context).colorScheme.onPrimaryContainer,
        title: const Text("Sensei"),
        actions: <Widget>[
          const SenseiIconButton(
            icon: Icon(Icons.keyboard_double_arrow_left_rounded),
            tooltip: 'New game',
            onPressed: GlobalState.newGame,
          ),
          const SenseiIconButton(
            icon: Icon(Icons.chevron_left_rounded),
            tooltip: 'Undo',
            onPressed: GlobalState.undo,
          ),
          const SenseiIconButton(
            icon: Icon(Icons.chevron_right_rounded),
            tooltip: 'Redo',
            onPressed: GlobalState.redo,
          ),
          const SenseiIconButton(
            icon: Icon(Icons.stop_rounded),
            tooltip: 'Stop',
            onPressed: GlobalState.stop,
          ),
          Semantics(
            label: 'Show menu',
            child: ListenableBuilder(
              listenable: GlobalState.actionWhenPlay,
              builder: (BuildContext context, Widget? widget) => PopupMenuButton<MenuItem>(
                icon: const Icon(Icons.more_vert_rounded),
                onSelected: (MenuItem i) { handleMenuItem(context, i); },
                itemBuilder: (context) => MenuItem.values.map((MenuItem i) {
                  if (i == MenuItem.senseiEvaluates || i == MenuItem.senseiIsInactive) {
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
        ]
      );
  }
}