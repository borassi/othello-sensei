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

import 'dart:async';
import 'dart:ffi';
import 'dart:io';
import 'dart:math';

import 'package:ffi/ffi.dart';
import 'package:flutter/material.dart';
import 'package:othello_sensei_flutter/ffi_bridge.dart';
import 'package:othello_sensei_flutter/widgets/appbar.dart';
import 'package:othello_sensei_flutter/state.dart';
import 'package:othello_sensei_flutter/widgets/disk_count.dart';
import 'package:othello_sensei_flutter/widgets/eval_parameters.dart';
import 'package:othello_sensei_flutter/widgets/evaluate_stats.dart';
import 'package:othello_sensei_flutter/widgets/settings.dart';
import 'package:path/path.dart';

import 'files.dart';
import 'widgets/board.dart';
import 'ffi_engine.dart';

void main() async {
  runApp(HomePage());
}

class HomePage extends StatefulWidget {

  @override
  State<StatefulWidget> createState() => _HomePageState();
}

class _HomePageState extends State<HomePage> {
  final String title = "Othello Sensei";
  final StreamController<bool> evaluateStreamController;

  _HomePageState() :
      evaluateStreamController = StreamController() {
    NativeCallable<SetBoardFunction> setBoardCallback = NativeCallable.listener(setBoard);
    NativeCallable<UpdateAnnotationsFunction> setAnnotationsCallback = NativeCallable.listener(updateAnnotations);

    GlobalState.ffiMain = GlobalState.getFFIMain(
        setBoardCallback.nativeFunction,
        setAnnotationsCallback.nativeFunction
    );
  }

  @override
  void free() async {
    ffiEngine.MainDelete(await GlobalState.ffiMain);
  }

  void newGame() async {
    ffiEngine.NewGame(await GlobalState.ffiMain);
    evaluate();
  }

  void playMove(int i) async {
    ffiEngine.PlayMove(await GlobalState.ffiMain, i);
    evaluate();
  }

  void redo() async {
    ffiEngine.Redo(await GlobalState.ffiMain);
    evaluate();
  }

  void undo() async {
    ffiEngine.Undo(await GlobalState.ffiMain);
    evaluate();
  }

  void stop() async {
    ffiEngine.Stop(await GlobalState.ffiMain);
  }

  void evaluate() async {
    if (GlobalState.actionWhenPlay.actionWhenPlay != ActionWhenPlay.eval) {
      return;
    }
    _evaluateInternal(0.1);
  }

  void _evaluateInternal(double time) async {
    final double? delta = (await GlobalState.preferences).getDouble('delta');
    ffiEngine.Evaluate(await GlobalState.ffiMain, -63, 63, 1000000000, time, delta ?? 0, 12, false);
  }

  void setBoard(BoardUpdate boardUpdate) {
    for (int i = 0; i < 64; ++i) {
      GlobalState.annotations[i].clear();
    }
    GlobalState.board.setState(boardUpdate);
  }

  void updateAnnotations(Annotations annotations) {
    var bestEval = double.negativeInfinity;
    for (int i = 0; i < annotations.num_moves; ++i) {
      MoveAnnotations move = annotations.moves[i];
      bestEval = max(bestEval, -move.eval);
    }
    for (int i = 0; i < annotations.num_moves; ++i) {
      MoveAnnotations move = annotations.moves[i];
      GlobalState.annotations[move.square].setState(move, bestEval);
    }
    GlobalState.globalAnnotations.setState(annotations);
    if (!annotations.finished) {
      _evaluateInternal(1);
    }
  }

  @override
  Widget build(BuildContext context) {
    var height = MediaQuery.of(context).size.height - (MediaQuery.of(context).padding.top + kToolbarHeight);
    var width = MediaQuery.of(context).size.width;
    var verticalSize = min(width, 8 / 13 * height);
    var horizontalSize = min(height, 8 / 13 * width);
    var vertical = verticalSize > horizontalSize;
    var size = max(verticalSize, horizontalSize);
    var squareSize = (size / 8.5).floorToDouble();

    // final ColorScheme colorScheme = ColorScheme.fromSeed(
    //   brightness: MediaQuery.platformBrightnessOf(context),
    //   seedColor: Colors.green,
    // );
    var theme = ThemeData(
        colorScheme: ColorScheme.fromSeed(
          seedColor: Colors.green,
          onSecondaryContainer: const Color(0xffeedd33),
          brightness: Brightness.dark),
        textTheme: TextTheme(
          bodyLarge: TextStyle(fontSize: squareSize / 3.3),
          bodyMedium: TextStyle(fontSize: squareSize / 3.3),
          bodySmall: TextStyle(fontSize: squareSize / 7),
        ),
        useMaterial3: true,
      );
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      title: 'Sensei',
      theme: theme,
      home: Scaffold(
        appBar: SenseiAppBar(newGame, undo, redo, stop, () => runApp(const SettingsPage())),
        body: Flex(
          mainAxisAlignment: MainAxisAlignment.start,
          crossAxisAlignment: CrossAxisAlignment.center,
          direction: vertical ? Axis.vertical : Axis.horizontal,
          children: [
            Board(size, squareSize, playMove, undo),
            Flexible(
                child: SizedBox(
                  width: vertical ? 8 * squareSize : null,
                  height: vertical ? null : 8 * squareSize,
                  child: DefaultTextStyle.merge(
                      style: theme.textTheme.bodyMedium,
                      child: Column(
                        mainAxisAlignment: MainAxisAlignment.center,
                        crossAxisAlignment: CrossAxisAlignment.center,
                        children: [
                          DiskCount(squareSize),
                          EvaluateStats(),
                          const EvalParameters()
                        ]
                      )
                  )
                )
            ),
          ]
        )
      )
    );
  }

}