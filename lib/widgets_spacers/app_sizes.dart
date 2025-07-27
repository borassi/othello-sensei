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


import 'dart:math';

import 'package:flutter/material.dart';

import '../state.dart';

enum AppLayout {
  vertical,
  horizontalFullBar,
  horizontalBrokenBar,
}

double getSideFraction() {
  switch (GlobalState.preferences.get('Margin size')) {
    case 'None':
      return 0;
    case 'Small':
      return 0.02;
    default:
      return 0.04;
  }
}

class AppSizes extends ThemeExtension<AppSizes> {
  static const double minFullAppBarSize = 280;
  static const double minWidth = 300;
  static const double minHeight = 350;
  double height;
  double width;
  late double heightMinusBarHeight;
  late double boardSize;
  late AppLayout layout;
  late double squareSize;
  late double sideBarWidth;
  late double sideBarHeight;
  late double margin;
  late double sideMargin;
  late double appBarHeight;

  AppSizes(this.height, this.width) : appBarHeight = AppBar().preferredSize.height {
    // Pixel Fold (very square) is 785/851 = 0.92 -> horizontal.
    // Most "square" tablet is 4/3 = 0.75 -> vertical.
    // We choose a 0.88 ratio to discriminate.
    bool vertical = width < 0.88 * height;
    var sideMarginFraction = getSideFraction();
    var internalMarginFraction = max(0.04, sideMarginFraction);
    heightMinusBarHeight = height - appBarHeight;
    var verticalBoardSize = min(
        width / (1 + 2 * sideMarginFraction),
        8 / 13.5 * heightMinusBarHeight / (1 + 2 * internalMarginFraction));
    var horizontalBoardSizeFullBar = min(
        heightMinusBarHeight / (1 + internalMarginFraction + sideMarginFraction),
        8 / 16 * width / (1 + sideMarginFraction + internalMarginFraction / 2.0)
    );
    var horizontalBoardSizeBrokenBar = min(
        height / (1 + 2 * sideMarginFraction),
        8 / 16 * width / (1 + sideMarginFraction + internalMarginFraction / 2.0)
    );
    if (vertical) {
      layout = AppLayout.vertical;
      boardSize = verticalBoardSize;
    } else if (horizontalBoardSizeFullBar >= horizontalBoardSizeBrokenBar - appBarHeight / 2 || height > appBarHeight * 9) {
      layout = AppLayout.horizontalFullBar;
      boardSize = horizontalBoardSizeFullBar;
    } else {
      layout = AppLayout.horizontalBrokenBar;
      boardSize = horizontalBoardSizeBrokenBar;
    }
    squareSize = (boardSize) / 8;
    sideMargin = sideMarginFraction * boardSize;
    margin = internalMarginFraction * boardSize;
    sideBarWidth = vertical ? boardSize - 2 * sideMargin : width - margin - boardSize - 2 * sideMargin;
    sideBarHeight = vertical ? heightMinusBarHeight - boardSize : heightMinusBarHeight;
  }

  bool vertical() {
    return layout == AppLayout.vertical;
  }

  bool brokenAppBar() {
    return layout == AppLayout.horizontalBrokenBar;
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