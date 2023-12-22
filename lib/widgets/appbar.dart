import 'package:flutter/material.dart';
import 'package:othello_sensei_flutter/state.dart';

class SenseiAppBar extends StatelessWidget implements PreferredSizeWidget {

  @override
  Size get preferredSize => const Size.fromHeight(kToolbarHeight);

  final void Function() stop;
  final void Function() undo;
  final void Function() redo;
  final void Function() openSettings;

  static const Map<ActionWhenPlay, Icon> actionToIcon = {
      ActionWhenPlay.playBlack: Icon(Icons.circle),
      ActionWhenPlay.playWhite: Icon(Icons.circle_outlined),
      ActionWhenPlay.eval: Icon(Icons.notifications_none),
      ActionWhenPlay.none: Icon(Icons.notifications_off_outlined),
  };

  const SenseiAppBar(this.undo, this.redo, this.stop, this.openSettings, {super.key});

  @override
  Widget build(BuildContext context) {
    return AppBar(
        backgroundColor: Theme.of(context).colorScheme.primaryContainer,
        foregroundColor: Theme.of(context).colorScheme.onPrimaryContainer,
        title: Text("Othello Sensei"),
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
          IconButton(
            icon: const Icon(Icons.settings),
            tooltip: 'Settings',
            onPressed: openSettings,
          ),
        ]
      );
  }
}