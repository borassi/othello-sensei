/*
 * Copyright 2023 Michele Borassi
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
 */

import 'dart:ffi';
import 'dart:math';

import 'package:ffi/ffi.dart';
import 'package:flutter/material.dart';
import 'package:othello_sensei/ffi_bridge.dart';
import 'package:othello_sensei/widgets/appbar.dart';
import 'package:othello_sensei/state.dart';
import 'package:othello_sensei/widgets/disk_count.dart';
import 'package:othello_sensei/widgets/disk_count_with_thor.dart';
import 'package:othello_sensei/widgets/evaluate_stats.dart';
import 'package:othello_sensei/widgets/thor_games_visualizer.dart';

import 'widgets/board.dart';
import 'ffi_engine.dart';

void setBoard(BoardUpdate boardUpdate) {
  for (int i = 0; i < 64; ++i) {
    GlobalState.annotations[i].clear();
  }
  GlobalState.board.setState(boardUpdate);
}

int currentMove() {
  return 60 - GlobalState.board.emptySquares();
}

void updateAnnotations(Pointer<Annotations> allAnnotations) {
  Annotations annotations = allAnnotations[currentMove()];
  for (int i = 0; i < annotations.num_moves; ++i) {
    MoveAnnotations move = annotations.moves[i];
    GlobalState.annotations[move.square].setState(move, annotations.eval);
  }
  GlobalState.globalAnnotations.setState(allAnnotations);
  if (!annotations.finished) {
    GlobalState.main.evaluate(isFirst: false);
  }
}

void main() async {
  await GlobalState.init();
  runApp(GlobalState.main);
}

class Main extends StatelessWidget {
  final String title = "Othello Sensei";
  static const tabName = ['Evaluate', 'Archive'];

  const Main({super.key});

  void newGame() async {
    ffiEngine.NewGame(GlobalState.ffiMain);
    evaluate();
  }

  void playMove(int i) {
    ffiEngine.PlayMove(GlobalState.ffiMain, i);
    evaluate();
  }

  void redo() {
    ffiEngine.Redo(GlobalState.ffiMain);
    evaluate();
  }

  void undo() {
    ffiEngine.Undo(GlobalState.ffiMain);
    evaluate();
  }

  void stop() {
    ffiEngine.Stop(GlobalState.ffiMain);
  }

  void evaluate({bool isFirst = true}) {
    if (GlobalState.actionWhenPlay.actionWhenPlay != ActionWhenPlay.eval) {
      return;
    }
    var params = ffiEngine.MainGetEvaluateParams(GlobalState.ffiMain!);
    GlobalState.preferences.fillEvaluateParams(params.ref, isFirst);
    ffiEngine.Evaluate(GlobalState.ffiMain!);
  }

  @override
  Widget build(BuildContext context) {
    var height = MediaQuery.of(context).size.height - (MediaQuery.of(context).padding.top + kToolbarHeight);
    var width = MediaQuery.of(context).size.width;
    var verticalSize = min(width, 8 / 14 * height);
    var horizontalSize = min(height, 8 / 13 * width);
    var vertical = verticalSize > horizontalSize;
    var size = max(verticalSize, horizontalSize);
    var squareSize = (size / 8.5 / 2).floorToDouble() * 2;

    var theme = ThemeData(
        colorScheme: ColorScheme.fromSeed(
          seedColor: Colors.green,
          onSecondaryContainer: const Color(0xffeedd33),
          brightness: Brightness.dark),
        textTheme: TextTheme(
          bodyLarge: TextStyle(fontSize: squareSize / 2.3),
          bodyMedium: TextStyle(fontSize: squareSize / 3.3),
          bodySmall: TextStyle(fontSize: squareSize / 5.5),
        ),
        useMaterial3: true,
      );

    var evaluateContent = Align(
      alignment: Alignment.center,
      child: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        crossAxisAlignment: CrossAxisAlignment.center,
        children: [
          SizedBox(height: 0.25 * squareSize),
          DiskCountWithError(squareSize),
          Spacer(),
          EvaluateStats(squareSize),
          Spacer(),
        ]
      )
    );

    var thorContent = Align(
      alignment: Alignment.center,
      child: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        crossAxisAlignment: CrossAxisAlignment.center,
        children: [
          SizedBox(height: 0.25 * squareSize),
          DiskCountsWithThor(squareSize),
          Expanded(child: ThorGamesVisualizer(squareSize)),
        ]
      )
    );
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      title: 'Sensei',
      theme: theme,
      home: Scaffold(
        resizeToAvoidBottomInset: false,
        appBar: SenseiAppBar(newGame, undo, redo, stop),
        body: Flex(
          mainAxisAlignment: MainAxisAlignment.start,
          crossAxisAlignment: CrossAxisAlignment.center,
          direction: vertical ? Axis.vertical : Axis.horizontal,
          children: [
            Board(size, squareSize, playMove, undo),
            SizedBox(
              width: vertical ? width : width - horizontalSize,
              height: vertical ? height - verticalSize : height,
              child: DefaultTabController(
                length: 2,
                initialIndex: GlobalState.preferences.get('Tab'),
                child: Scaffold(
                  bottomNavigationBar: TabBar(
                    tabs: List.generate(2, (index) => Text(tabName[index])),
                    onTap: (int index) {
                      GlobalState.preferences.set('Tab', index);
                    },
                  ),
                  body: TabBarView(
                    children: [
                      evaluateContent,
                      thorContent,
                    ],
                  ),
                ),
              ),
            ),
          ]
        )
      )
    );
  }

}