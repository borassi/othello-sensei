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

double getSideMargin() {
  switch (GlobalState.preferences.get('Margin size')) {
    case 'None':
      return 1;
    case 'Small':
      return 7;
    default:
      return 14;
  }
}

class AppSizes extends ThemeExtension<AppSizes> {
  static const double minFullAppBarSize = 280;
  static const double minWidth = 400;
  static const double minHeight = 400;
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
    sideMargin = getSideMargin();
    margin = max(14, sideMargin);
    heightMinusBarHeight = height - appBarHeight;
    var verticalBoardSize = min(width - 2 * sideMargin, 8 / 15 * (heightMinusBarHeight - 2 * margin));
    var horizontalBoardSizeFullBar = min(heightMinusBarHeight - margin - sideMargin, 8 / 16 * width - 2 * margin - sideMargin);
    var horizontalBoardSizeBrokenBar = min(height - 2 * sideMargin, 8 / 16 * width);
    if (verticalBoardSize > horizontalBoardSizeFullBar && verticalBoardSize > horizontalBoardSizeBrokenBar) {
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
    sideBarWidth = vertical() ? boardSize - 2 * sideMargin : width - boardSize - sideMargin;
    sideBarHeight = vertical() ? heightMinusBarHeight - boardSize : heightMinusBarHeight;
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