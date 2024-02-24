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

import 'package:flutter/material.dart';
import 'package:flutter/services.dart';

import '../state.dart';


class CopyIntent extends Intent {
  const CopyIntent();
}
class PasteIntent extends Intent {
  const PasteIntent();
}


class MyKeyboardListener extends StatelessWidget {
  final Widget child;

  const MyKeyboardListener({super.key, required this.child});

  @override
  Widget build(BuildContext context) {
    return Shortcuts(
      shortcuts: const <ShortcutActivator, Intent>{
        SingleActivator(LogicalKeyboardKey.keyC, control: true): CopyIntent(),
        SingleActivator(LogicalKeyboardKey.keyV, control: true): PasteIntent(),
      },
      child: Actions(
        actions: <Type, Action<Intent>>{
          CopyIntent: CallbackAction<CopyIntent>(
            onInvoke: (CopyIntent intent) => copy(),
          ),
          PasteIntent: CallbackAction<PasteIntent>(
            onInvoke: (PasteIntent intent) => pasteOrError(context),
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