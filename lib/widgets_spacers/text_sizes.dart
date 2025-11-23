/*
 * Copyright (c) 2025 Michele Borassi
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

enum TextType { small, medium, large }

String _generateNumbers(int value, int beforeDecimal, int afterDecimal) {
  String result = value.toString() * beforeDecimal;
  if (afterDecimal <= 0) {
    return result;
  }
  return '$result.${value.toString() * afterDecimal}';
}

class FontSizeCalculator {
  double fontSize;
  BuildContext context;

  FontSizeCalculator(this.context) : fontSize = 100;

  bool _fontSizeFits(
      String text,
      double size,
      BoxConstraints constraints,
      int maxLines) {
    final textPainter = TextPainter(
      text: TextSpan(
        text: text,
        style: Theme.of(context).textTheme.bodyMedium!.copyWith(fontSize: size),
      ),
      textScaler: MediaQuery.textScalerOf(context),
      textDirection: TextDirection.ltr,
      maxLines: maxLines,
    );
    textPainter.layout(maxWidth: constraints.maxWidth);
    return !textPainter.didExceedMaxLines &&
        textPainter.height <= constraints.maxHeight;
  }

  void addConstraint(
      String text,
      BoxConstraints constraints,
      int maxLines) {
    if (text.isEmpty ||
        _fontSizeFits(text, fontSize, constraints, maxLines)) {
      return;
    }
    double low = 0;
    double high = fontSize;
    while (high > low + 0.2) {
      assert(_fontSizeFits(text, low, constraints, maxLines));
      assert(!_fontSizeFits(text, high, constraints, maxLines));
      final mid = low + (high - low) / 2;
      if (_fontSizeFits(text, mid, constraints, maxLines)) {
        low = mid;
      } else {
        high = mid;
      }
    }
    fontSize = low;
  }
}

BoxConstraints squareConstraints(double squareSize) {
  return BoxConstraints(maxHeight: squareSize - 3, maxWidth: squareSize - 3);
}

double calculateLargeTextSize(double squareSize, BuildContext context) {
  var calculator = FontSizeCalculator(context);
  BoxConstraints constraints = squareConstraints(squareSize);

  for (int i = 0; i < 10; ++i) {
    for (String prepend in ['+', '-']) {
      // Evaluate.
      calculator.addConstraint(
          '$prepend${_generateNumbers(i, 2, 0)}',
          constraints,
          1);
    }
  }
  print('large ${calculator.fontSize}');
  return calculator.fontSize;
}

double calculateMediumTextSize(double squareSize, BuildContext context) {
  var calculator = FontSizeCalculator(context);
  BoxConstraints constraints = squareConstraints(squareSize);

  for (int i = 0; i < 10; ++i) {
    // Evaluate.
    for (String prepend in ['+', '-']) {
      calculator.addConstraint(
          '$prepend${_generateNumbers(i, 2, 2)}',
          constraints,
          1);
    }
    for (String append in ['k', 'M', 'G', 'T']) {
      // Archive.
      calculator.addConstraint(
          '${_generateNumbers(i, 3, 0)}$append',
          constraints,
          1);
    }
  }
  return calculator.fontSize;
}

double calculateSmallTextSize(double squareSize, BuildContext context) {
  var calculator = FontSizeCalculator(context);
  BoxConstraints constraints = BoxConstraints(maxWidth: squareSize / 2 - 3);

  for (int i = 0; i < 10; ++i) {
    for (String append in ['k', 'M', 'G', 'T', 'P', 'E']) {
      // Second or third line.
      calculator.addConstraint(
          '${_generateNumbers(i, 3, 0)}$append',
          constraints,
          1);
      calculator.addConstraint(
          '${_generateNumbers(i, 1, 1)}$append',
          constraints,
          1);
    }
    calculator.addConstraint(
        '${_generateNumbers(i, 2, 0)}%',
        constraints,
        1);
  }
  return calculator.fontSize;
}