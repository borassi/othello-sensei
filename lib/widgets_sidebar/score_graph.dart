/*
 * Copyright 2024-2025 Michele Borassi
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

import '../state.dart';
import '../utils.dart';
import '../widgets_utils/hide_inactive.dart';

class ScoreGraph extends HideInactiveWidget {
  const ScoreGraph({super.key});

  BarChartGroupData generateGroupData(int x, List<double> scores, ColorScheme colorScheme, double height, double width, double maxY, var moveToHighlight) {
    var barWidth = width / 61;
    var score = x >= scores.length ? double.nan : scores[x];
    Color color = colorScheme.background;
    Color? borderColor;
    if (x == moveToHighlight) {
      color = colorScheme.onSecondaryContainer;
    } else if (GlobalState.isXot() && x <= 8) {
      color = colorScheme.primaryContainer;
    } else if (!GlobalState.preferences.get('Black and white bars in the graph')) {
      color = colorScheme.onPrimaryContainer;
    } else {
      if (score > 1) {
        color = colorScheme.surface;
        borderColor = colorScheme.surfaceVariant;
      } else if (score < -1) {
        color = colorScheme.surfaceVariant;
      } else {
        color = colorScheme.onPrimaryContainer;
      }
    }

    double fromY;
    double toY;
    if (score.isNaN) {
      fromY = 0;
      toY = 0;
    } else {
      // Want fromY / (barWidth / 2) = (2 * maxY) / height
      fromY = (score > 0 ? -1 : 1) * barWidth / 2 * (2 * maxY) / height;
      toY = score;
    }
    return BarChartGroupData(
      x: x,
      groupVertically: true,
      barRods: [
        BarChartRodData(
          fromY: fromY,
          toY: toY,
          width: barWidth,
          color: color,
          borderSide: BorderSide(color: borderColor ?? color),
        ),
        BarChartRodData(
          fromY: -maxY,
          toY: maxY,
          width: barWidth,
          color: Colors.transparent,
        ),
      ],
    );
  }

  @override
  Widget buildChild(BuildContext context) {
    return LayoutBuilder(
      builder: (BuildContext context, BoxConstraints constraints) => ListenableBuilder(
        listenable: GlobalState.globalAnnotations,
        builder: (BuildContext context, Widget? widget) {
          var (scores, move) = GlobalState.globalAnnotations.getAllScoresAndLastMove();
          var maxY = maxIgnoreNaN((scores + [10]).reduce(maxIgnoreNaN), -(scores + [-10]).reduce(minIgnoreNaN));
          maxY = (maxY / 10).ceilToDouble() * 10;
          var horizontalLinesSpace = maxY / 2;
          var textSpace = Theme.of(context).textTheme.bodyMedium!.fontSize! * 2.8;
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
                  GlobalState.setCurrentMove(barTouchResponse.spot!.spot.x.toInt());
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
                      child: Text(
                          x > 0 ? 'B+${x.toStringAsFixed(0)}' : (x == 0 ? '+0 ' : 'W+${(-x).toStringAsFixed(0)}'),
                          style: Theme.of(context).textTheme.bodyMedium!)
                    ),
                    reservedSize: textSpace,
                  )
                ),
                rightTitles: const AxisTitles(),
                topTitles: const AxisTitles(),
                bottomTitles: const AxisTitles(),
              ),
              maxY: maxY + 1,
              minY: -maxY - 1,
              baselineY: 0,
              borderData: FlBorderData(show: false),
              gridData: const FlGridData(show: false),
              barGroups: List.generate(61, (i) => generateGroupData(i, scores, Theme.of(context).colorScheme, height, width, maxY + 1, move)),
              extraLinesData: ExtraLinesData(
                extraLinesOnTop: false,
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