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


import 'package:flutter/material.dart';

import 'app_sizes.dart';

enum FlexWithMarginsDirection {
  vertical,
  horizontal,
  asAppMain,
  inverseAppMain,
}

class Margin extends StatelessWidget {
  const Margin({super.key});

  @override
  Widget build(BuildContext context) {
    var margin = Theme.of(context).extension<AppSizes>()!.margin!;
    return SizedBox(height: margin, width: margin);
  }
}

class FlexWithMargins extends StatelessWidget {
  FlexWithMarginsDirection direction;
  List<Widget> children;
  CrossAxisAlignment crossAxisAlignment;

  FlexWithMargins({
    super.key,
    required this.direction,
    required this.children,
    this.crossAxisAlignment = CrossAxisAlignment.center,
  });

  Axis getDirection(BuildContext context) {
    switch (direction) {
      case FlexWithMarginsDirection.vertical:
        return Axis.vertical;
      case FlexWithMarginsDirection.horizontal:
        return Axis.horizontal;
      case FlexWithMarginsDirection.asAppMain:
        return Theme.of(context).extension<AppSizes>()!.vertical! ? Axis.vertical : Axis.horizontal;
      case FlexWithMarginsDirection.inverseAppMain:
        return Theme.of(context).extension<AppSizes>()!.vertical! ? Axis.horizontal : Axis.vertical;
    }
  }

  bool isSpacerWidget(Widget widget) {
    return widget is Margin || widget is Spacer;
  }

  @override
  Widget build(BuildContext context) {
    var finalWidgets = <Widget>[];
    for (int i = 0; i < children.length; ++i) {
      finalWidgets.add(children[i]);
      if (i+1 < children.length && !isSpacerWidget(children[i]) && !isSpacerWidget(children[i+1])) {
        finalWidgets.add(const Margin());
      }
    }

    return Flex(
      crossAxisAlignment: crossAxisAlignment,
      direction: getDirection(context),
      children: finalWidgets,
    );
  }
}

class HorizontalFlexWithMargins extends FlexWithMargins {
  HorizontalFlexWithMargins({super.key, required super.children}) :
      super(direction: FlexWithMarginsDirection.horizontal);
}

class VerticalFlexWithMargins extends FlexWithMargins {
  VerticalFlexWithMargins({super.key, required super.children}) :
      super(direction: FlexWithMarginsDirection.vertical);
}