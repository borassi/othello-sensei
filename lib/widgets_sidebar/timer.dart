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

import 'dart:math';

import 'package:flutter/material.dart';

import '../state.dart';
import '../widgets_spacers/app_sizes.dart';
import '../widgets_spacers/margins.dart';
import '../widgets_spacers/text_size_groups.dart';
import '../widgets_utils/misc.dart';

class Timer extends StatelessWidget {
  final bool _black;

  const Timer(this._black, {super.key});

  @override
  Widget build(BuildContext context) {
    return ListenableBuilder(listenable: Listenable.merge([GlobalState.preferences, GlobalState.timer]), builder: (BuildContext context, Widget? widget) {
      return LargeText(
          GlobalState.timer.getString(_black),
          style: TextStyle(fontWeight: FontWeight.bold)
      );
    });
  }
}

void addToTimer(double minutes) {
  var currentTime = GlobalState.preferences.get('Timer (minutes, 0 for no countdown)');
  var newTime = max(0.0, min(currentTime + minutes, 60.0));
  GlobalState.preferences.set('Timer (minutes, 0 for no countdown)', newTime);
}

class Timers extends StatelessWidget {
  const Timers({super.key});

  @override
  Widget build(BuildContext context) {
    var squareSize = Theme.of(context).extension<AppSizes>()!.squareSize;
    return Row(children: [
      SizedBox(
        height: squareSize,
        width: squareSize,
        child: SenseiButton(icon: Icons.remove, onPressed: () => addToTimer(-1), iconSize: 0.6 * squareSize),
      ),
      const Margin.internal(),
      const Spacer(),
      Timer(true),
      const Spacer(),
      const Margin.internal(),
      const Spacer(),
      Timer(false),
      const Spacer(),
      const Margin.internal(),
      SizedBox(
        height: squareSize,
        width: squareSize,
        child: SenseiButton(icon: Icons.add, onPressed: () => addToTimer(1), iconSize: 0.6 * squareSize),
      )
    ]);
  }
}