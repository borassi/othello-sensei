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
import 'package:othello_sensei/state.dart';

import '../main.dart';
import '../widgets_utils/misc.dart';

class SenseiDialog extends StatelessWidget {
  final String? title;
  final String? content;
  final List<({String text, void Function(BuildContext ctx) onPressed})>? actions;

  const SenseiDialog({super.key, this.content, this.title, this.actions});

  @override
  Widget build(BuildContext context) {
    // We do it this way because Flutter refresh keeps post frame callbacks,
    // causing the window to show multiple times.
    GlobalState.preferences.set('Show unsupported CPU at startup', false);
    var textTheme = Theme.of(context).textTheme.bodyMedium;
    var highlightColor = Theme.of(context).colorScheme.onSurfaceVariant;
    return AlertDialog(
      title: title == null ? null : Center(child: Text(title!, style: textTheme?.copyWith(color: highlightColor))),
      content: content == null ? null : Text(content!, style: textTheme, softWrap: true,),
      actions: actions?.map(
        (var record) => SenseiButton(
          text: record.text,
          onPressed: () => record.onPressed(context),
        )
      ).toList(),
      buttonPadding: const EdgeInsets.symmetric(horizontal: 8.0),
    );
  }
}

Future<dynamic> showSenseiDialog(SenseiDialog dialog) async {
  BuildContext? context = GlobalState.navigatorKey.currentContext;
  if (context != null) {
    return await showDialog(
      context: context,
      builder: (BuildContext ctx) => AppTheme(child: dialog),
    );
  }
}