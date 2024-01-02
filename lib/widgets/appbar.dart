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
import 'package:othello_sensei/widgets/settings.dart';

import '../utils.dart';

enum MenuItem {
  save,
  open,
  loadThor,
  setFirstPosition,
  resetFirstPosition,
  settings
}

void handleMenuItem(BuildContext context, MenuItem item) {
  switch (item) {
    case MenuItem.save:
      return;
    case MenuItem.open:
      return;
    case MenuItem.loadThor:
      return;
    case MenuItem.setFirstPosition:
      return;
    case MenuItem.resetFirstPosition:
      return;
    case MenuItem.settings:
      GlobalState.main.stop();
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

  final void Function() newGame;
  final void Function() undo;
  final void Function() redo;
  final void Function() stop;

  static const Map<ActionWhenPlay, Icon> actionToIcon = {
      ActionWhenPlay.playBlack: Icon(Icons.circle),
      ActionWhenPlay.playWhite: Icon(Icons.circle_outlined),
      ActionWhenPlay.eval: Icon(Icons.notifications_none),
      ActionWhenPlay.none: Icon(Icons.notifications_off_outlined),
  };

  const SenseiAppBar(this.newGame, this.undo, this.redo, this.stop, {super.key});

  @override
  Widget build(BuildContext context) {
    return AppBar(
        backgroundColor: Theme.of(context).colorScheme.primaryContainer,
        foregroundColor: Theme.of(context).colorScheme.onPrimaryContainer,
        title: Text("Sensei"),
        actions: <Widget>[
          ListenableBuilder(
              listenable: GlobalState.actionWhenPlay,
              builder: (BuildContext context, Widget? child) => IconButton(
                icon: actionToIcon[GlobalState.actionWhenPlay.actionWhenPlay]!,
                tooltip: "Change action",
                onPressed: GlobalState.actionWhenPlay.rotateActions,
              )
          ),
          IconButton(
            icon: const Icon(Icons.home),
            tooltip: 'New game',
            onPressed: newGame,
          ),
          IconButton(
            icon: const Icon(Icons.stop_rounded),
            tooltip: 'Stop',
            onPressed: stop,
          ),
          IconButton(
            icon: const Icon(Icons.chevron_left_rounded),
            tooltip: 'Undo',
            onPressed: undo,
          ),
          IconButton(
            icon: const Icon(Icons.chevron_right_rounded),
            tooltip: 'Redo',
            onPressed: redo,
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