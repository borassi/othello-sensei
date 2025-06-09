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
import 'package:flutter_svg/svg.dart';

import '../widgets_spacers/app_sizes.dart';

class CaseWithCup extends StatelessWidget {
  const CaseWithCup({super.key});

  static const svg = """
    <svg viewBox="0 0 64 64">
    <path fill="#231F20" d="M60,4H48c0-2.215-1.789-4-4-4H20c-2.211,0-4,1.785-4,4H4C1.789,4,0,5.785,0,8v8c0,8.836,7.164,16,16,16
      c0.188,0,0.363-0.051,0.547-0.059C17.984,37.57,22.379,41.973,28,43.43V56h-8c-2.211,0-4,1.785-4,4v4h32v-4c0-2.215-1.789-4-4-4h-8
      V43.43c5.621-1.457,10.016-5.859,11.453-11.488C47.637,31.949,47.812,32,48,32c8.836,0,16-7.164,16-16V8C64,5.785,62.211,4,60,4z
       M8,16v-4h8v12C11.582,24,8,20.414,8,16z M56,16c0,4.414-3.582,8-8,8V12h8V16z"/>
    </svg>
    """;

  @override
  Widget build(BuildContext context) {
    var colorScheme = Theme.of(context).colorScheme;
    var squareSize = Theme.of(context).extension<AppSizes>()!.squareSize;
    return Container(
      height: squareSize,
      width: squareSize,
      decoration: BoxDecoration(
        color: colorScheme.primaryContainer,
        border: Border.all(
          color: colorScheme.background,
          width: 0.5,
        ),
      ),
      child: Align(
        alignment: Alignment.center,
        child: SvgPicture.string(
          height: 0.8 * squareSize,
          width: 0.8 * squareSize,
          svg,
          colorFilter: ColorFilter.mode(colorScheme.onSecondaryContainer, BlendMode.srcIn),
        )
      )
    );
  }
}