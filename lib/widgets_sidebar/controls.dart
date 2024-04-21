/*
 * Copyright (c) 2023-2024. Michele Borassi
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
import '../widgets_spacers/app_sizes.dart';
import '../widgets_spacers/margins.dart';
import '../widgets_utils/misc.dart';

class Controls extends StatelessWidget {

  const Controls({super.key});

  @override
  Widget build(BuildContext context) {
    var appSizes = Theme.of(context).extension<AppSizes>()!;
    return SizedBox(
      height: appSizes.squareSize,
      child: const Row(
        crossAxisAlignment: CrossAxisAlignment.stretch,
        children: [
          Expanded(
            child: SenseiButton(
              text: 'Back to the game / the first position',
              icon: Icons.keyboard_double_arrow_left_rounded,
              onPressed: GlobalState.toAnalyzedGameOrFirstState,
            )
          ),
          Margin(),
          Expanded(
            child: SenseiButton(
              text: 'Undo',
              icon: Icons.chevron_left_rounded,
              onPressed: GlobalState.undo,
            ),
          ),
          Margin(),
          Expanded(
            child: SenseiButton(
              text: 'Redo',
              icon: Icons.chevron_right_rounded,
              onPressed: GlobalState.redo,
            ),
          ),
          Margin(),
          Expanded(
            child: SenseiButton(
              text: 'Stop',
              icon: Icons.stop_rounded,
              onPressed: GlobalState.stop,
            ),
          ),
        ]
      )
    );
  }
}