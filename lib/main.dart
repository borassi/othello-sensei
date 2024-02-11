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

import 'dart:core';
import 'dart:ffi';
import 'dart:math';

import 'package:flutter/material.dart';
import 'package:othello_sensei/ffi_bridge.dart';
import 'package:othello_sensei/widgets/appbar.dart';
import 'package:othello_sensei/state.dart';
import 'package:othello_sensei/widgets/disk_count.dart';
import 'package:othello_sensei/widgets/disk_count_with_thor.dart';
import 'package:othello_sensei/widgets/evaluate_stats.dart';
import 'package:othello_sensei/widgets/score_graph.dart';
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

enum FlexWithMarginsDirection {
  vertical,
  horizontal,
  asAppMain,
  inverseAppMain,
}

class Margin extends StatelessWidget {
  const Margin({super.key});

  @override
  Widget build(BuildContext context) {
    var margin = Theme.of(context).extension<AppSizes>()!.margin!;
    return SizedBox(height: margin, width: margin);
  }
}

class FlexWithMargins extends StatelessWidget {
  FlexWithMarginsDirection direction;
  List<Widget> children;
  CrossAxisAlignment crossAxisAlignment;

  FlexWithMargins({
    super.key,
    required this.direction,
    required this.children,
    this.crossAxisAlignment = CrossAxisAlignment.center,
  });

  Axis getDirection(BuildContext context) {
    switch (direction) {
      case FlexWithMarginsDirection.vertical:
        return Axis.vertical;
      case FlexWithMarginsDirection.horizontal:
        return Axis.horizontal;
      case FlexWithMarginsDirection.asAppMain:
        return Theme.of(context).extension<AppSizes>()!.vertical! ? Axis.vertical : Axis.horizontal;
      case FlexWithMarginsDirection.inverseAppMain:
        return Theme.of(context).extension<AppSizes>()!.vertical! ? Axis.horizontal : Axis.vertical;
    }
  }

  bool isSpacerWidget(Widget widget) {
    return widget is Margin || widget is Spacer;
  }

  @override
  Widget build(BuildContext context) {
    var finalWidgets = <Widget>[];
    for (int i = 0; i < children.length; ++i) {
      finalWidgets.add(children[i]);
      if (i+1 < children.length && !isSpacerWidget(children[i]) && !isSpacerWidget(children[i+1])) {
        finalWidgets.add(const Margin());
      }
    }

    return Flex(
      crossAxisAlignment: crossAxisAlignment,
      direction: getDirection(context),
      children: finalWidgets,
    );
  }
}

class HorizontalFlexWithMargins extends FlexWithMargins {
  HorizontalFlexWithMargins({super.key, required super.children}) :
      super(direction: FlexWithMarginsDirection.horizontal);
}

class VerticalFlexWithMargins extends FlexWithMargins {
  VerticalFlexWithMargins({super.key, required super.children}) :
      super(direction: FlexWithMarginsDirection.vertical);
}

class AppSizes extends ThemeExtension<AppSizes> {
  double height;
  double width;
  bool? vertical;
  double? squareSize;
  double? sizeBarWidth;
  double? sizeBarHeight;
  double? margin;

  AppSizes(this.height, this.width) {
    var verticalBoardSize = min(width, 8 / 14 * height);
    var horizontalBoardSize = min(height, 8 / 13 * width);
    vertical = verticalBoardSize > horizontalBoardSize;
    var boardSize = max(verticalBoardSize, horizontalBoardSize);
    squareSize = (boardSize! / 8.5);
    margin = (boardSize! - 8 * squareSize!) / 2;
    sizeBarWidth = (vertical! ? boardSize! - 2 * margin! : width - boardSize! - margin!);
    sizeBarHeight = (vertical! ? height - boardSize! : height);
  }

  @override
  AppSizes copyWith({double? height, double? width}) {
    return AppSizes(height ?? this.height, width ?? this.width);
  }

  @override
  AppSizes lerp(AppSizes? other, double t) {
    if (other is! AppSizes) {
      return this;
    }
    return AppSizes(height * t + other.height * (1-t), width * t + other.width * (1-t));
  }
}

class AppTheme extends StatelessWidget {
  Widget child;

  AppTheme({super.key, required this.child});

  @override
  Widget build(BuildContext context) {
    return LayoutBuilder(
      builder: (context, constraints) {
        var theme = Theme.of(context).copyWith(
          extensions: <ThemeExtension<AppSizes>>[
            AppSizes(constraints.maxHeight, constraints.maxWidth)
          ],
        );

        var squareSize = theme.extension<AppSizes>()!.squareSize!;
        return Theme(
          data: theme.copyWith(
            textTheme: TextTheme(
              bodyLarge: TextStyle(fontSize: squareSize / 2.4),
              bodyMedium: TextStyle(fontSize: squareSize / 3.5),
              bodySmall: TextStyle(fontSize: squareSize / 6.5),
            ),
          ),
          child: child
        );
      }
    );
  }
}

class MainContent extends StatelessWidget {
  Widget board;
  Widget sidebar;
  MainContent(this.board, this.sidebar, {super.key});

  @override
  Widget build(BuildContext context) {
    var vertical = Theme.of(context).extension<AppSizes>()!.vertical!;
    return FlexWithMargins(
      direction: FlexWithMarginsDirection.asAppMain,
      children: [
        const Margin(),
        SizedBox(
          child: FlexWithMargins(
            direction: FlexWithMarginsDirection.inverseAppMain,
            children: [const Margin(), const Spacer(), board, const Spacer(), const Margin()],
          )
        ),
        Expanded(
          child: FlexWithMargins(
            direction: FlexWithMarginsDirection.inverseAppMain,
            children: [
              const Margin(),
              Expanded(child: FlexWithMargins(
                direction: FlexWithMarginsDirection.horizontal,
                children: [Expanded(child: sidebar), const Margin()],
              ))
            ]
          ),
        )
      ]
    );
  }
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

  void setCurrentMove(int current_move) {
    ffiEngine.SetCurrentMove(GlobalState.ffiMain, current_move);
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
    var evaluateContent = VerticalFlexWithMargins(
      children: const [
        DiskCountWithError(),
        Expanded(child: ScoreGraph()),
        EvaluateStats(),
      ],
    );

    var thorContent = VerticalFlexWithMargins(
      children: [
        DiskCountsWithThor(),
        const Expanded(child: ThorGamesVisualizer()),
      ]
    );
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      title: 'Sensei',
      theme: ThemeData(
        colorScheme: ColorScheme.fromSeed(
          seedColor: Colors.green,
          onSecondaryContainer: const Color(0xffeedd33),
          surface: const Color(0xff191919),
          surfaceVariant: const Color(0xfff9f9f9),
          brightness: Brightness.dark),
        useMaterial3: true,
      ),
      home: Scaffold(
        // resizeToAvoidBottomInset: false,
        appBar: SenseiAppBar(newGame, undo, redo, stop),
        body: AppTheme(
          child: MainContent(
            Board(playMove, undo),
            DefaultTabController(
              length: 2,
              initialIndex: GlobalState.preferences.get('Tab'),
              child: Scaffold(
                bottomNavigationBar: TabBar(
                  tabs: List.generate(2, (index) => Tab(text: tabName[index])),
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
          )
        )
      )
    );
  }

}