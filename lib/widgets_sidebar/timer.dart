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

import '../state.dart';
import '../widgets_spacers/margins.dart';

class Timer extends StatelessWidget {
  final bool _black;

  const Timer(this._black, {super.key});

  @override
  Widget build(BuildContext context) {
    var largeTextStyle = Theme.of(context).textTheme.bodyLarge!;
    return ListenableBuilder(listenable: GlobalState.timer, builder: (BuildContext context, Widget? widget) {
      return Text(
          GlobalState.timer.getString(_black),
          style: largeTextStyle.merge(TextStyle(
              height: 1.0,
              fontWeight: FontWeight.bold)
          )
      );
    });
  }
}

class Timers extends StatelessWidget {
  const Timers({super.key});

  @override
  Widget build(BuildContext context) {
    return Row(children: [
      const Spacer(),
      Timer(true),
      const Spacer(),
      const Margin.internal(),
      const Spacer(),
      Timer(false),
      const Spacer()
    ]);
  }
}