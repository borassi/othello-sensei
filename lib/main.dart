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
import 'dart:io';

import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:othello_sensei/widgets_sidebar/controls.dart';
import 'package:othello_sensei/widgets_windows/appbar.dart';
import 'package:othello_sensei/state.dart';
import 'package:othello_sensei/widgets_sidebar/disk_count.dart';
import 'package:othello_sensei/widgets_sidebar/disk_count_with_thor.dart';
import 'package:othello_sensei/widgets_sidebar/evaluate_stats.dart';
import 'package:othello_sensei/widgets_sidebar/score_graph.dart';
import 'package:othello_sensei/widgets_sidebar/thor_games_visualizer.dart';
import 'package:path/path.dart';
import 'package:window_manager/window_manager.dart';

import 'files.dart';
import 'widgets_board/board.dart';
import 'widgets_spacers/app_sizes.dart';
import 'widgets_spacers/margins.dart';
import 'widgets_windows/keyboard_listener.dart';

void main() async {
  await GlobalState.init();
  if (Platform.isWindows || Platform.isMacOS || Platform.isLinux) {
    WidgetsFlutterBinding.ensureInitialized();
    WindowManager.instance.setMinimumSize(const Size(AppSizes.minWidth, AppSizes.minHeight));
    WindowManager.instance.setIcon(join(await localAssetPath(), 'icons/icon_512x512.png'));
    await windowManager.ensureInitialized();
  }
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
            AppSizes(
              constraints.maxHeight - MediaQuery.of(context).viewPadding.top - MediaQuery.of(context).viewPadding.bottom,
              constraints.maxWidth - MediaQuery.of(context).viewPadding.left - MediaQuery.of(context).viewPadding.right
            )
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
    var boardContent = Flex(
      mainAxisSize: MainAxisSize.min,
      direction: Theme.of(context).extension<AppSizes>()!.layout == AppLayout.vertical ? Axis.horizontal : Axis.vertical,
      children: [board, const Margin()],
    );
    var sideContent = Expanded(
      child: Column(
        mainAxisSize: MainAxisSize.min,
        children: [
          const Margin(),
          Expanded(
            child: Row(
              mainAxisSize: MainAxisSize.min,
              children: [
                const Margin(),
                Flexible(
                  child: Container(
                    constraints: BoxConstraints(maxWidth: 8 * Theme.of(context).extension<AppSizes>()!.squareSize!),
                    child: sidebar,
                  ),
                ),
                const Margin()
              ],
            )
          )
        ]
      )
    );

    var brokenAppBar = Theme.of(context).extension<AppSizes>()!.brokenAppBar();
    return AnnotatedRegion(
      value: SystemUiOverlayStyle(
        statusBarColor: brokenAppBar ? Theme.of(context).colorScheme.background : Theme.of(context).colorScheme.primaryContainer,
        statusBarIconBrightness: Brightness.light,
      ),
      child: Scaffold(
        resizeToAvoidBottomInset: false,
        body: Row(
          children:
          (brokenAppBar ? <Widget>[SafeArea(child: boardContent)] : <Widget>[]) +
          <Widget>[
            Expanded(
              child: Column(
                children: [
                  const SenseiAppBar(),
                  Expanded(
                    child: Center(
                      child: Flex(
                        direction: Theme.of(context).extension<AppSizes>()!.vertical() ? Axis.vertical : Axis.horizontal,
                        mainAxisSize: MainAxisSize.min,
                        children:
                          (!brokenAppBar ? <Widget>[boardContent] : <Widget>[]) +
                          [sideContent]
                      )
                    )
                  )
                ]
              )
            )
          ]
        )
      )
    );
  }
}

class Main extends StatelessWidget {
  final String title = "Othello Sensei";
  static const tabName = ['Evaluate', 'Archive'];

  const Main({super.key});

  @override
  Widget build(BuildContext context) {
    List<Widget> childrenEvaluate = [
      const DiskCountWithError(),
      const Margin(),
      const Expanded(child: ScoreGraph()),
      const Margin(),
      const EvaluateStats(),
    ];
    List<Widget> childrenThor = [
      const DiskCountsWithThor(),
      const Margin(),
      const Expanded(child: ThorGamesVisualizer()),
    ];
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      title: 'Sensei',
      theme: ThemeData(
        colorScheme: ColorScheme.fromSeed(
          seedColor: Colors.green,
          secondaryContainer: const Color(0xff00731b),  // primary: 0xff005313
          onSecondaryContainer: const Color(0xffeedd33),
          surface: const Color(0xff222222),
          surfaceVariant: const Color(0xfff9f9f9),
          brightness: Brightness.dark),
        useMaterial3: true,
      ),
      home: PopScope(
        canPop: false,
        onPopInvoked: (bool didPop) {
          if (GlobalState.preferences.get('Back button action') == 'Undo') {
            GlobalState.undo();
          } else {
            SystemChannels.platform.invokeMethod('SystemNavigator.pop');
          }
        },
        child: MyKeyboardListener(
          child: AppTheme(
            child: MainContent(
              const Board(),
              DefaultTabController(
                length: 2,
                initialIndex: 0,
                child: ListenableBuilder(
                  listenable: GlobalState.preferences,
                  builder: (BuildContext context, Widget? widget) {
                    DefaultTabController.of(context).animateTo(
                        GlobalState.preferences.get('Active tab'),
                        duration: const Duration(seconds: 0));
                    var childrenControls = <Widget>[];
                    if (GlobalState.preferences.get('Controls position') == 'Side bar') {
                      childrenControls = [
                        const Margin(),
                        const Controls(),
                      ];
                    }
                    var evaluateContent = ListenableBuilder(
                      listenable: GlobalState.preferences,
                      builder: (BuildContext context, Widget? widget) => Column(
                        children: childrenEvaluate + childrenControls
                      )
                    );

                    var thorContent = ListenableBuilder(
                      listenable: GlobalState.preferences,
                      builder: (BuildContext context, Widget? widget) => Column(
                        children: childrenThor + childrenControls,
                      )
                    );
                    return Scaffold(
                      bottomNavigationBar: TabBar(
                        tabs: List.generate(2, (index) => Tab(
                          height: Theme.of(context).extension<AppSizes>()!.squareSize!,
                          child: Text(
                            tabName[index],
                            style: TextStyle(
                              fontSize: Theme.of(context).textTheme.bodyMedium!.fontSize,
                            )
                          )
                        )),
                        dividerHeight: 0,
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
                    );
                  }
                ),
              ),
            )
          )
        )
      )
    );
  }

}