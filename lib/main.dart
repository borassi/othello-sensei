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
import 'package:othello_sensei/intents.dart';
import 'package:othello_sensei/widgets_sidebar/sidebar.dart';
import 'package:othello_sensei/widgets_windows/appbar.dart';
import 'package:othello_sensei/state.dart';
import 'package:othello_sensei/widgets_windows/sensei_dialog.dart';
import 'package:path/path.dart';
import 'package:window_manager/window_manager.dart';

import 'ffi/ffi_bridge.dart';
import 'files.dart';
import 'widgets_board/board.dart';
import 'widgets_spacers/app_sizes.dart';
import 'widgets_spacers/margins.dart';
import 'widgets_windows/keyboard_listener.dart';

void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  maybeForwardIntent();
  if (Platform.isWindows || Platform.isMacOS || Platform.isLinux) {
    await windowManager.ensureInitialized();
  }
  await GlobalState.init();
  await handleIntent();
  if (Platform.isWindows || Platform.isMacOS || Platform.isLinux) {
    WindowManager.instance.setMinimumSize(const Size(AppSizes.minWidth, AppSizes.minHeight));
    if (!Platform.isMacOS) {
      WindowManager.instance.setIcon(
          join(localAssetPath(), 'icons/icon_512x512.png'));
    }
  }
  runApp(GlobalState.main);
}

class AppTheme extends StatelessWidget {
  final Widget child;

  const AppTheme({super.key, required this.child});

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

        var squareSize = theme.extension<AppSizes>()!.squareSize;
        return Theme(
          data: theme.copyWith(
            textTheme: TextTheme(
              bodyLarge: TextStyle(fontSize: squareSize / 2.1, color: Theme.of(context).colorScheme.onPrimaryContainer),
              bodyMedium: TextStyle(fontSize: squareSize / 3.9, color: Theme.of(context).colorScheme.onPrimaryContainer),
              bodySmall: TextStyle(fontSize: squareSize / 6, color: Theme.of(context).colorScheme.onPrimaryContainer),
            ),
          ),
          child: child
        );
      }
    );
  }
}

class CpuErrorDialog extends StatelessWidget {
  const CpuErrorDialog({super.key});

  @override
  Widget build(BuildContext context) {
    // We do it this way because Flutter refresh keeps post frame callbacks,
    // causing the window to show multiple times.
    GlobalState.preferences.set('Show unsupported CPU at startup', false);
    return SenseiDialog(
      content: (
        'Your CPU does not support the commands '
        '${GlobalState.cpuType == CpuType.popcnt ? "BMI2" : "POPCNT and BMI2"}.'
        ' Sensei will work, but the evaluation will be slower.'),
      actions: [
        (
          text: 'OK - show again next time',
          onPressed: (context) {
            GlobalState.preferences.set('Show unsupported CPU at startup', true);
            Navigator.of(context).pop();
          },
        ),
        (
          text: 'OK - do not show again',
          onPressed: (context) {
            Navigator.of(context).pop();
          },
        ),
      ]
    );
  }
}

class DecideSettingsDialog extends StatelessWidget {
  const DecideSettingsDialog({super.key});

  @override
  Widget build(BuildContext context) {
    // We do it this way because Flutter refresh keeps post frame callbacks,
    // causing the window to show multiple times.
    return SenseiDialog(
        title: 'Choose your settings!',
        content: 'You can always change your settings by clicking on ⋮ > Settings',
        actions: [
          (
          text: 'Beginner settings',
          onPressed: (context) async {
            await GlobalState.preferences.reset(true);
            await GlobalState.preferences.set('Show settings dialog at startup', false);
            Navigator.of(context).pop();
          },
          ),
          (
          text: 'Advanced settings',
          onPressed: (context) async {
            await GlobalState.preferences.reset(false);
            await GlobalState.preferences.set('Show settings dialog at startup', false);
            Navigator.of(context).pop();
          },
          ),
        ]
    );
  }
}


class MainContent extends StatelessWidget {
  final Widget board;
  final Widget sidebar;
  const MainContent(this.board, this.sidebar, {super.key});

  @override
  Widget build(BuildContext context) {
    var appSizes = Theme.of(context).extension<AppSizes>()!;
    var brokenAppBar = appSizes.brokenAppBar();
    bool vertical = appSizes.vertical();

    Widget content;
    if (vertical) {
      content = Column(
        children: [
          Row(mainAxisAlignment: MainAxisAlignment.center, children: [board, const Margin.side()]),
          const Margin.internal(),
          Expanded(
            child: Row(
                mainAxisAlignment: MainAxisAlignment.center,
                children: [const Margin.side(), SizedBox(width: 8 * appSizes.squareSize,child: sidebar), const Margin.side()])
          )
        ]
      );
    } else {
      var sidebarColumn = Expanded(
          child: Column(
              children: [
                brokenAppBar ? const Margin.side() : const Margin.internal(),
                Expanded(child: sidebar),
              ]
          )
      );
      double availableHeight = appSizes.height - (brokenAppBar ? 0 : appSizes.appBarHeight);
      bool spaceAroundBoard = availableHeight > appSizes.boardSize + appSizes.margin + appSizes.squareSize;
      var boardColumn = Column(
          children: [
              spaceAroundBoard ? const Spacer() : Container(),
              board,
              const Spacer(),
              spaceAroundBoard ? SizedBox(height: appSizes.squareSize) : Container(),
          ]
      );
      var children = [
        boardColumn,
        const Margin.internal(),
        sidebarColumn,
        const Margin.side(),
      ];
      if (GlobalState.preferences.get("Board on the right in horizontal mode")) {
        children = [
          const Margin.side(),
          sidebarColumn,
          const Margin.internal(),
          boardColumn,
        ];
      }
      content = Row(children: children);
    }

    return Container(
      color: Theme.of(context).colorScheme.surface,
      child: SafeArea(
        // If top: true, then the Container and SafeArea below do nothing.
        // If top: false, then the Container below makes the top bar green.
        top: brokenAppBar,
        child: Container(
          color: Theme.of(context).colorScheme.primaryContainer,
          child: SafeArea(
            child: Scaffold(
              body: Column(
                mainAxisAlignment: MainAxisAlignment.start,
                crossAxisAlignment: CrossAxisAlignment.center,
                children:
                  (brokenAppBar ? <Widget>[] : <Widget>[SenseiAppBar()]) +
                      [Expanded(child: content)]
              )
            )
          )
        )
      )
    );
  }
}

class MainApp extends StatelessWidget {
  const MainApp({super.key});

  @override
  Widget build(BuildContext context) {
    WidgetsBinding.instance.addPostFrameCallback((_) async {
      if (GlobalState.preferences.get(
          'Show unsupported CPU at startup') &&
          [CpuType.popcnt, CpuType.noFeature].contains(GlobalState.cpuType)) {
        await showDialog<void>(
            context: context,
            builder: (BuildContext context) => const CpuErrorDialog()
        );
      }
      if (GlobalState.preferences.get('Show settings dialog at startup')) {
        await showDialog<void>(
            context: context,
            builder: (BuildContext context) => const DecideSettingsDialog()
        );
      }
    });
    return ListenableBuilder(
        listenable: GlobalState.preferences,
        builder: (BuildContext context, Widget? widget) {
          return PopScope(
              canPop: false,
              onPopInvokedWithResult: (bool didPop, dynamic result) async {
                if (GlobalState.preferences.get('Back button action') == 'Undo') {
                  GlobalState.undo();
                } else {
                  if (await showDialog(context: context, builder: (ctx) => SenseiDialog(
                    title: 'Close the app?',
                    actions: [
                      (text: 'Yes', onPressed: (ctx) { Navigator.pop(ctx, true); }),
                      (text: 'No', onPressed: (ctx) { Navigator.pop(ctx, false); }),
                    ]
                  ))) {
                    SystemNavigator.pop();
                  }
                }
              },
              child: MyKeyboardListener(
                  child: AppTheme(
                      child: MainContent(
                        const Board(),
                        const Sidebar(),
                      )
                  )
              )
          );
        }
    );
  }

}

class Main extends StatefulWidget {
  const Main({super.key});

  @override
  State<StatefulWidget> createState() {
    return _MainState();
  }
}

class _MainState extends State<Main> with WidgetsBindingObserver {

  @override
  void initState() {
    super.initState();
    WidgetsBinding.instance.addObserver(this);
  }

  @override
  void dispose() {
    WidgetsBinding.instance.removeObserver(this);
    super.dispose();
  }

  @override
  void didChangeAppLifecycleState(AppLifecycleState state) {
    switch (state) {
      case AppLifecycleState.resumed:
        setCountingTime();
        break;
      case AppLifecycleState.detached:
      case AppLifecycleState.inactive:
      case AppLifecycleState.hidden:
      case AppLifecycleState.paused:
        stopCountingTime();
        break;
    }
  }


  @override
  Widget build(BuildContext context) {
    return MaterialApp(
        navigatorKey: GlobalState.navigatorKey,
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
        home: const MainApp()
    );
  }

}