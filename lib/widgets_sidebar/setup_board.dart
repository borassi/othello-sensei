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
import 'package:othello_sensei/widgets_utils/misc.dart';

import '../state.dart';
import '../widgets_board/case.dart';
import '../widgets_spacers/app_sizes.dart';
import '../widgets_spacers/margins.dart';

class SetupBoardControls extends StatelessWidget {

  const SetupBoardControls({super.key});

  @override
  Widget build(BuildContext context) {
    var squareSize = Theme.of(context).extension<AppSizes>()!.squareSize;
    var blackDisk = Case(CaseState.black, 255, false);
    var whiteDisk = Case(CaseState.white, 255, false);
    return
      ListenableBuilder(
        listenable: Listenable.merge([GlobalState.setupBoardState, GlobalState.board]),
        builder: (BuildContext context, Widget? widget) => Column(
          crossAxisAlignment: CrossAxisAlignment.center,
          children: [
            const Spacer(),
            Row(
              children: [
                Text('On click:', style: Theme.of(context).textTheme.bodyLarge),
                const Spacer(),
                GestureDetector(
                  behavior: HitTestBehavior.opaque,
                  onTap: () { GlobalState.setupBoardState.updateOnClick(); },
                  child: Case(GlobalState.setupBoardState.onClick, 255, false)
                ),
              ]
            ),
            const Spacer(),
            Row(
              children: [
                Text('Turn:', style: Theme.of(context).textTheme.bodyLarge),
                const Spacer(),
                GestureDetector(
                    behavior: HitTestBehavior.opaque,
                    onTap: () { GlobalState.ffiEngine.InvertTurn(GlobalState.ffiMain); },
                    child: GlobalState.board.blackTurn ? blackDisk : whiteDisk
                ),
              ]
            ),
            const Spacer(),
            Container(
                constraints: BoxConstraints.expand(height: squareSize),
                child: SenseiButton(
                  onPressed: () { GlobalState.newGame(); },
                  text: 'Reset',
                  textStyle: Theme.of(context).textTheme.bodyLarge,
                )
            ),
            const Spacer(),
            Container(
              constraints: BoxConstraints.expand(height: squareSize),
              child: SenseiButton(
                onPressed: () { GlobalState.setupBoardState.setSettingUpBoard(false); GlobalState.evaluate(); },
                text: 'Done',
                textStyle: Theme.of(context).textTheme.bodyLarge,
              )
            )
          ]
        )
    );
  }
}