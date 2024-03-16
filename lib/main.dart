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

import 'dart:core';

import 'package:flutter/material.dart';
import 'package:othello_sensei/widgets_windows/appbar.dart';
import 'package:othello_sensei/state.dart';
import 'package:othello_sensei/widgets_sidebar/disk_count.dart';
import 'package:othello_sensei/widgets_sidebar/disk_count_with_thor.dart';
import 'package:othello_sensei/widgets_sidebar/evaluate_stats.dart';
import 'package:othello_sensei/widgets_sidebar/score_graph.dart';
import 'package:othello_sensei/widgets_sidebar/thor_games_visualizer.dart';

import 'widgets_board/board.dart';
import 'widgets_spacers/app_sizes.dart';
import 'widgets_spacers/margins.dart';
import 'widgets_windows/keyboard_listener.dart';

void main() async {
  await GlobalState.init();
  runApp(GlobalState.main);
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
              bodyLarge: TextStyle(fontSize: squareSize / 2.1),
              bodyMedium: TextStyle(fontSize: squareSize / 3.5),
              bodySmall: TextStyle(fontSize: squareSize / 5.5),
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
    return FlexWithMargins(
      direction: FlexWithMarginsDirection.asAppMain,
      children: [
        SizedBox(
          child: FlexWithMargins(
            direction: FlexWithMarginsDirection.inverseAppMain,
            children: [const Spacer(), board, const Spacer(), const Margin()],
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

  @override
  Widget build(BuildContext context) {
    var evaluateContent = ColumnWithMargins(
      children: const [
        DiskCountWithError(),
        Expanded(child: ScoreGraph()),
        EvaluateStats(),
      ],
    );

    var thorContent = ColumnWithMargins(
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
      home: MyKeyboardListener(
        child: Scaffold(
          // resizeToAvoidBottomInset: false,
          appBar: const SenseiAppBar(),
          body: AppTheme(
            child: MainContent(
              const Board(),
              DefaultTabController(
                length: 2,
                initialIndex: GlobalState.preferences.get('Active tab'),
                child: Scaffold(
                  bottomNavigationBar: TabBar(
                    tabs: List.generate(2, (index) => Tab(text: tabName[index])),
                    onTap: (int index) {
                      GlobalState.preferences.set('Active tab', index);
                      GlobalState.evaluate();
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
      )
    );
  }

}