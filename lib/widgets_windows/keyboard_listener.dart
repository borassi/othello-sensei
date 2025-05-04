/*
 * Copyright (c) 2024. Michele Borassi
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

import 'dart:io';

import 'package:flutter/material.dart';
import 'package:flutter/services.dart';

import '../state.dart';


class CopyIntent extends Intent {
  const CopyIntent();
}
class PasteIntent extends Intent {
  const PasteIntent();
}
class NewGameIntent extends Intent {
  const NewGameIntent();
}
class UndoIntent extends Intent {
  const UndoIntent();
}
class RedoIntent extends Intent {
  const RedoIntent();
}
class BackIntent extends Intent {
  const BackIntent();
}


class MyKeyboardListener extends StatelessWidget {
  final Widget child;

  const MyKeyboardListener({super.key, required this.child});

  @override
  Widget build(BuildContext context) {
    var isMac = Platform.isMacOS;
    return Shortcuts(
      shortcuts: <ShortcutActivator, Intent>{
        SingleActivator(LogicalKeyboardKey.keyC, control: !isMac, meta: isMac): CopyIntent(),
        SingleActivator(LogicalKeyboardKey.keyV, control: !isMac, meta: isMac): PasteIntent(),
        SingleActivator(LogicalKeyboardKey.keyN, control: !isMac, meta: isMac): NewGameIntent(),
        SingleActivator(LogicalKeyboardKey.arrowLeft): UndoIntent(),
        SingleActivator(LogicalKeyboardKey.arrowRight): RedoIntent(),
        SingleActivator(LogicalKeyboardKey.arrowUp): BackIntent(),
        SingleActivator(LogicalKeyboardKey.pageUp): BackIntent(),
      },
      child: Actions(
        actions: <Type, Action<Intent>>{
          CopyIntent: CallbackAction<CopyIntent>(
            onInvoke: (CopyIntent intent) => copy(),
          ),
          PasteIntent: CallbackAction<PasteIntent>(
            onInvoke: (PasteIntent intent) => paste(),
          ),
          NewGameIntent: CallbackAction<NewGameIntent>(
            onInvoke: (NewGameIntent intent) => GlobalState.newGame(),
          ),
          UndoIntent: CallbackAction<UndoIntent>(
            onInvoke: (UndoIntent intent) => GlobalState.undo(),
          ),
          RedoIntent: CallbackAction<RedoIntent>(
            onInvoke: (RedoIntent intent) => GlobalState.redo(),
          ),
          BackIntent: CallbackAction<BackIntent>(
            onInvoke: (BackIntent intent) => GlobalState.toLastImportantNode(),
          ),
        },
        child: Focus(
          autofocus: true,
          child: child,
        ),
      ),
    );
  }
}