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

class AppSizes extends ThemeExtension<AppSizes> {
  double height;
  double width;
  bool? vertical;
  double? squareSize;
  double? sizeBarWidth;
  double? sizeBarHeight;
  double? margin;

  AppSizes(this.height, this.width) {
    var verticalBoardSize = min(width, 8 / 14 * height);
    var horizontalBoardSize = min(height, 8 / 14 * width);
    vertical = verticalBoardSize > horizontalBoardSize;
    var boardSize = max(verticalBoardSize, horizontalBoardSize);
    squareSize = (boardSize! / 8.5);
    margin = (boardSize! - 8 * squareSize!) / 2;
    sizeBarWidth = (vertical! ? boardSize! - 2 * margin! : width - boardSize! - margin!);
    sizeBarHeight = (vertical! ? height - boardSize! : height);
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