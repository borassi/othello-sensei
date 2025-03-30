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
import 'package:othello_sensei/widgets_sidebar/setup_board.dart';
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

class MainContent extends StatelessWidget {
  final Widget board;
  final Widget sidebar;
  const MainContent(this.board, this.sidebar, {super.key});

  @override
  Widget build(BuildContext context) {
    var appSizes = Theme.of(context).extension<AppSizes>()!;
    bool vertical = appSizes.layout == AppLayout.vertical;
    var boardContent = Flex(
      mainAxisSize: MainAxisSize.min,
      direction: vertical ? Axis.horizontal : Axis.vertical,
      children: [board, const Margin.side()],
    );
    var sideContent = Expanded(
      child: Column(
        mainAxisSize: MainAxisSize.min,
        children:
          (vertical ? <Widget>[const Margin.internal()] : (appSizes.brokenAppBar() ? <Widget>[] : <Widget>[const Margin.side()])) + <Widget>[
          Expanded(
            child: Row(
              mainAxisSize: MainAxisSize.min,
              children: [
                vertical ? const Margin.side() : const Margin.internal(),
                Flexible(
                  child: Container(
                    constraints: BoxConstraints(maxWidth: 8 * appSizes.squareSize),
                    child: sidebar,
                  ),
                ),
                const Margin.side()
              ],
            )
          )
        ]
      )
    );

    var brokenAppBar = Theme.of(context).extension<AppSizes>()!.brokenAppBar();
    return Container(
      color: brokenAppBar ? Theme.of(context).colorScheme.surface : Theme.of(context).colorScheme.primaryContainer,
      child: SafeArea(
        bottom: false,
        child: AnnotatedRegion(
          value: SystemUiOverlayStyle(
            statusBarColor: Theme.of(context).colorScheme.primaryContainer,
            statusBarIconBrightness: Brightness.light,
          ),
          child: Scaffold(
            resizeToAvoidBottomInset: false,
            body: Row(
              children:
              (brokenAppBar ? <Widget>[boardContent] : <Widget>[]) +
              <Widget>[
                Expanded(
                  child: Column(
                    children: <Widget>[
                      Row(
                        children: (brokenAppBar ? <Widget>[const Margin.internal()] : <Widget>[])
                            + <Widget>[const Expanded(child: SenseiAppBar())]
                      )] +
                      (brokenAppBar ? <Widget>[const Margin.internal()] : <Widget>[]) + [
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
        )
      )
    );
  }
}

class MainApp extends StatelessWidget {
  const MainApp({super.key});

  @override
  Widget build(BuildContext context) {
    WidgetsBinding.instance.addPostFrameCallback((_) {
      if (GlobalState.preferences.get(
          'Show unsupported CPU at startup') &&
          [CpuType.popcnt, CpuType.noFeature].contains(GlobalState.cpuType)) {
        showDialog<void>(
            context: context,
            builder: (BuildContext context) => const CpuErrorDialog()
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
                        ListenableBuilder(
                          listenable: GlobalState.setupBoardState,
                          builder: (BuildContext context, Widget? widget) {
                            if (GlobalState.setupBoardState.settingUpBoard) {
                              return const SetupBoardControls();
                            } else {
                              return const Sidebar();
                            }
                          }
                        )
                      )
                  )
              )
          );
        }
    );
  }

}

class Main extends StatelessWidget {
  const Main({super.key});

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