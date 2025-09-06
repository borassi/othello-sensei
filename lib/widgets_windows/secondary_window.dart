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

import '../main.dart';
import '../widgets_spacers/app_sizes.dart';
import '../widgets_spacers/margins.dart';

class _SecondaryWindowMainContent extends StatelessWidget {
  final Widget child;
  final bool sideMarginAlwaysLarge;

  const _SecondaryWindowMainContent({required this.child, required this.sideMarginAlwaysLarge});

  @override
  Widget build(BuildContext context) {
    var appSizes = Theme.of(context).extension<AppSizes>()!;
    return Row(
      children: [
        sideMarginAlwaysLarge ? const Margin.internal() : const Margin.side(),
        const Spacer(),
        SizedBox(
          width: 8 * appSizes.squareSize - (sideMarginAlwaysLarge ? 2 * (appSizes.margin - appSizes.sideMargin) : 0),
          child: Column(
            children: [
              const Margin.internal(),
              Expanded(child: SafeArea(child: child)),
              const Margin.side(),
            ]
          )
        ),
        const Spacer(),
        sideMarginAlwaysLarge ? const Margin.internal() : const Margin.side(),
      ]
    );
  }

}

class SecondaryWindow extends StatelessWidget {
  final Function(bool)? onPopInvoked;
  final String title;
  final Widget child;
  final bool sideMarginAlwaysLarge;

  const SecondaryWindow({super.key, required this.title, required this.child, this.onPopInvoked, this.sideMarginAlwaysLarge = false});

  @override
  Widget build(BuildContext context) {
    return PopScope(
      onPopInvoked: onPopInvoked,
      child: AppTheme(
        child: Scaffold(
          resizeToAvoidBottomInset: false,
          appBar: AppBar(
            title: Text(title),
            backgroundColor: Theme.of(context).colorScheme.primaryContainer,
            foregroundColor: Theme.of(context).colorScheme.onPrimaryContainer,
          ),
          body: _SecondaryWindowMainContent(sideMarginAlwaysLarge: sideMarginAlwaysLarge, child: child)
        )
      )
    );
  }
}