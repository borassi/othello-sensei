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


import 'package:fl_chart/fl_chart.dart';
import 'package:flutter/material.dart';
import 'package:othello_sensei/main.dart';

import '../state.dart';
import '../utils.dart';
import 'fixed_width_widget.dart';

class ScoreGraph extends FixedWidthWidget {
  const ScoreGraph({super.key});

  BarChartGroupData generateGroupData(int x, List<double> scores, Color highlightColor, Color standardColor, double height, double width, double maxY) {
    var score = scores[x];
    var barWidth = width / 61;
    if (score.isNaN) {
      return BarChartGroupData(
        x: x,
        barRods: [
          BarChartRodData(
            fromY: 0,
            toY: 0,
            width: barWidth,
            color: standardColor,
          )
        ]
      );
    }
    // Want fromY / (barWidth / 2) = (2 * maxY) / height
    var fromY = (score > 0 ? -1 : 1) * barWidth / 2 * (2 * maxY) / height;
    return BarChartGroupData(
      x: x,
      barsSpace: 0,
      groupVertically: true,
      barRods: [
        BarChartRodData(
          fromY: fromY,
          toY: score,
          width: barWidth,
          color: x == currentMove() ? highlightColor : standardColor,
        ),
        BarChartRodData(
          fromY: -maxY,
          toY: maxY,
          color: Colors.transparent,
        ),
      ],
    );
  }

  @override
  Widget buildChild(BuildContext context) {
    var highlightColor = Theme.of(context).colorScheme.onSecondaryContainer;
    var standardColor = Theme.of(context).colorScheme.onPrimaryContainer;
    return LayoutBuilder(
      builder: (BuildContext context, BoxConstraints constraints) => ListenableBuilder(
        listenable: GlobalState.globalAnnotations,
        builder: (BuildContext context, Widget? widget) {
          var scores = GlobalState.globalAnnotations.getAllScores();
          var maxY = maxIgnoreNaN(scores.reduce(maxIgnoreNaN), -scores.reduce(minIgnoreNaN));
          maxY = maxIgnoreNaN(10, (maxY / 10).ceilToDouble() * 10) + 1;
          var horizontalLinesSpace = (maxY - 1) / 2;
          var textSpace = Theme.of(context).textTheme.bodySmall!.fontSize! * 2;
          var width = (constraints.maxWidth - textSpace);
          var height = constraints.maxHeight;

          return BarChart(
            swapAnimationDuration: const Duration(seconds: 0),
            BarChartData(
              barTouchData: BarTouchData(
                handleBuiltInTouches: false,
                touchCallback: (FlTouchEvent event, barTouchResponse) {
                  if (barTouchResponse == null ||
                      barTouchResponse.spot == null ||
                      !(event is FlTapUpEvent || event is FlPanEndEvent)) {
                    return;
                  }
                  GlobalState.main.setCurrentMove(barTouchResponse!.spot!.spot.x.toInt());
                },
              ),
              alignment: BarChartAlignment.spaceEvenly,

              titlesData: FlTitlesData(
                leftTitles: AxisTitles(
                  sideTitles: SideTitles(
                    showTitles: true,
                    interval: horizontalLinesSpace,
                    getTitlesWidget: (double x, TitleMeta meta) => x % 5 != 0 ? const Text('') : Align(
                      alignment: Alignment.center,
                      child: Text(x.toStringAsFixed(0), style: Theme.of(context).textTheme.bodySmall!)
                    ),
                    reservedSize: textSpace,
                  )
                ),
                rightTitles: AxisTitles(),
                topTitles: AxisTitles(),
                bottomTitles: AxisTitles(),
              ),
              maxY: maxY,
              minY: -maxY,
              baselineY: 0,
              borderData: FlBorderData(show: false),
              gridData: const FlGridData(show: false),
              barGroups: List.generate(61, (i) => generateGroupData(i, scores, highlightColor, standardColor, height, width, maxY)),
              extraLinesData: ExtraLinesData(
                horizontalLines: List.generate(
                  5, (i) => HorizontalLine(
                    y: (i - 2) * horizontalLinesSpace,
                    color: Theme.of(context).colorScheme.surfaceVariant,
                    dashArray: i == 2 ? null : [1, 3],
                    strokeWidth: 1,
                  ),
                )
              ),
            ),
          );
        }
      )
    );
  }
}