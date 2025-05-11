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
import 'package:othello_sensei/widgets_sidebar/moves_visualizer.dart';
import 'package:othello_sensei/widgets_sidebar/score_graph.dart';
import 'package:othello_sensei/widgets_sidebar/thor_games_visualizer.dart';
import 'package:othello_sensei/widgets_sidebar/timer.dart';
import 'package:othello_sensei/widgets_windows/appbar.dart';

import '../state.dart';
import '../widgets_spacers/app_sizes.dart';
import '../widgets_spacers/margins.dart';
import 'controls.dart';
import 'disk_count.dart';
import 'evaluate_stats.dart';

class Sidebar extends StatelessWidget {
  const Sidebar({super.key});
  static const tabName = ['Evaluate', 'Archive', 'Play'];

  @override
  Widget build(BuildContext context) {
    List<Widget> childrenEvaluate = [
      const DiskCountWithExtraContent(DiskCountExtraContent.error),
      const Margin.internal(),
      const Expanded(child: ScoreGraph()),
      const Margin.internal(),
      const EvaluateStats(),
    ];
    List<Widget> childrenThor = [
      const DiskCountWithExtraContent(DiskCountExtraContent.thor),
      const Margin.internal(),
      const Expanded(child: ThorGamesVisualizer()),
    ];
    List<Widget> childrenPlay = [
      const DiskCountWithExtraContent(DiskCountExtraContent.player),
      const Margin.internal(),
      const Spacer(),
      const Timers(),
      const Margin.internal(),
      const Spacer(),
      const MovesVisualizer(),
    ];
    return DefaultTabController(
      length: 3,
      initialIndex: 0,
      child: ListenableBuilder(
        listenable: GlobalState.preferences,
        builder: (BuildContext context, Widget? widget) {
          var brokenAppBar = Theme.of(context).extension<AppSizes>()!.brokenAppBar();
          DefaultTabController.of(context).animateTo(
              GlobalState.preferences.get('Active tab'),
              duration: const Duration(seconds: 0));
          var childrenControls = <Widget>[];
          if (GlobalState.preferences.get('Controls position') == 'Side bar') {
            childrenControls = [
              const Margin.internal(),
              const Controls(),
            ];
          }
          var childAppBar = brokenAppBar ? <Widget>[SenseiAppBar(), const Margin.internal()] : <Widget>[];
          var evaluateContent = Column(
              children: childAppBar + childrenEvaluate + childrenControls
          );
          var thorContent = Column(
            children: childAppBar + childrenThor + childrenControls,
          );
          var playContent = Column(
            children: childAppBar + childrenPlay + childrenControls,
          );
          return Scaffold(
            bottomNavigationBar: TabBar(
              tabs: List.generate(3, (index) => Tab(
                height: Theme.of(context).extension<AppSizes>()!.squareSize,
                child: Text(
                  tabName[index],
                  style: TextStyle(
                    fontSize: Theme.of(context).textTheme.bodyMedium!.fontSize,
                  )
                )
              )),
              physics: NeverScrollableScrollPhysics(),
              dividerHeight: 0,
              onTap: (int index) {
                GlobalState.preferences.set('Active tab', index);
                GlobalState.evaluate();
              },
            ),
            body: TabBarView(
              physics: NeverScrollableScrollPhysics(),
              children: [
                evaluateContent,
                thorContent,
                playContent,
              ],
            ),
          );
        }
      ),
    );
  }
}