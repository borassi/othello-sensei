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
    case MenuItem.settings:
      GlobalState.stop();
      Navigator.push(
        context,
        MaterialPageRoute(builder: (context) => Settings()),
      );
      return;
  }
}

class SenseiAppBar extends StatelessWidget implements PreferredSizeWidget {

  @override
  Size get preferredSize => const Size.fromHeight(kToolbarHeight);

  static const Map<ActionWhenPlay, Icon> actionToIcon = {
      ActionWhenPlay.playBlack: Icon(Icons.circle),
      ActionWhenPlay.playWhite: Icon(Icons.circle_outlined),
      ActionWhenPlay.eval: Icon(Icons.notifications_none),
      ActionWhenPlay.none: Icon(Icons.notifications_off_outlined),
  };

  const SenseiAppBar({super.key});

  @override
  Widget build(BuildContext context) {
    return AppBar(
        backgroundColor: Theme.of(context).colorScheme.primaryContainer,
        foregroundColor: Theme.of(context).colorScheme.onPrimaryContainer,
        title: const Text("Sensei"),
        actions: <Widget>[
          ListenableBuilder(
              listenable: GlobalState.actionWhenPlay,
              builder: (BuildContext context, Widget? child) => IconButton(
                icon: actionToIcon[GlobalState.actionWhenPlay.actionWhenPlay]!,
                tooltip: "Change action",
                onPressed: GlobalState.actionWhenPlay.rotateActions,
              )
          ),
          const IconButton(
            icon: Icon(Icons.home),
            tooltip: 'New game',
            onPressed: GlobalState.newGame,
          ),
          const IconButton(
            icon: Icon(Icons.stop_rounded),
            tooltip: 'Stop',
            onPressed: GlobalState.stop,
          ),
          const IconButton(
            icon: Icon(Icons.chevron_left_rounded),
            tooltip: 'Undo',
            onPressed: GlobalState.undo,
          ),
          const IconButton(
            icon: Icon(Icons.chevron_right_rounded),
            tooltip: 'Redo',
            onPressed: GlobalState.redo,
          ),
          PopupMenuButton<MenuItem>(
            icon: const Icon(Icons.more_vert_rounded),
            onSelected: (MenuItem i) { handleMenuItem(context, i); },
            itemBuilder: (context) => MenuItem.values.map((MenuItem i) {
              return PopupMenuItem<MenuItem>(value: i, child: Text(camelCaseToSpaces(i.name)));
            }).toList()
          ),
        ]
      );
  }
}