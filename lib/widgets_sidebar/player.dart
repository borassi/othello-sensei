/*
 * Copyright 2025 Michele Borassi
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
import 'package:othello_sensei/utils.dart';

import '../state.dart';
import '../widgets_spacers/app_sizes.dart';
import '../widgets_utils/misc.dart';

class PlayerWidget extends StatelessWidget {
  final bool _black;

  const PlayerWidget(this._black, {super.key});

  @override
  Widget build(BuildContext context) {
    var largeTextStyle = Theme.of(context).textTheme.bodyMedium;
    var squareSize = Theme.of(context).extension<AppSizes>()!.squareSize;
    return
      GestureDetector(
        child: ListenableBuilder(listenable: GlobalState.actionWhenPlay, builder: (BuildContext context, Widget? widget) {
          var preference = _black ? 'Black player' : 'White player';
          var text = (GlobalState.preferences.get(preference) as Player).name.capitalize();
          return SizedBox(
              width: 2 * squareSize,
              height: squareSize,
              child: SenseiButton(
                text: text,
                textStyle: largeTextStyle,
                onPressed: () { GlobalState.actionWhenPlay.rotatePlayer(_black); GlobalState.evaluate(); },
              )
          );
        }),
      );
  }
}