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

import '../widgets_spacers/app_sizes.dart' as app_sizes;
import 'text.dart';
import '../widgets_spacers/text_sizes.dart';

class SenseiButton extends StatelessWidget {
  final String? text;
  final IconData? icon;
  final double? iconSize;
  final void Function() onPressed;
  final TextStyle? textStyle;
  final TextType? textType;
  final bool intrinsicSize;

  const SenseiButton(
      {super.key, this.text, required this.onPressed, this.textStyle, this.icon, this.iconSize, this.textType, this.intrinsicSize = false});

  @override
  Widget build(BuildContext context) {
    var textColor = Theme.of(context).colorScheme.onPrimaryContainer;
    Widget content;
    WidgetStateProperty<EdgeInsetsGeometry>? padding;
    if (icon != null) {
      content = Icon(icon, color: textColor, size: iconSize ?? app_sizes.iconSize(context), applyTextScaling: false);
      padding = WidgetStateProperty.all(EdgeInsetsGeometry.zero);
    } else if (text != null) {
      content = AnyText(
          textType ?? TextType.medium,
          text!,
          style: textStyle,
          textAlign: TextAlign.center,
          intrinsicSize: true,
          maxLines: 3);
    } else {
      throw Exception('Error: button without icon or text.');
    }
    return ConstrainedBox(
      constraints: BoxConstraints(minHeight: app_sizes.minButtonSize(context)),
      child: TextButton(
          onPressed: onPressed,
          style: ButtonStyle(
              padding: padding,
              backgroundColor: WidgetStateProperty.all(
                  Theme.of(context).colorScheme.primaryContainer)
          ),
          child: Semantics(label: text, child: content)
      )
    );
  }
}

class SenseiToggle extends StatelessWidget {
  final bool initialValue;
  final void Function(bool? value) onChanged;

  const SenseiToggle({super.key, required this.initialValue, required this.onChanged});

  @override
  Widget build(BuildContext context) {
    return Switch(
      value: initialValue,
      onChanged: onChanged,
    );
  }
}

class WindowTitle extends StatelessWidget {
  final String text;

  const WindowTitle(this.text, {super.key});

  @override
  Widget build(BuildContext context) {
    return LayoutBuilder(
        builder: (context, constraints) {
          final textPainter = TextPainter(
            text: TextSpan(
              text: text,
              style: Theme.of(context).textTheme.bodyMedium!.copyWith(fontSize: 24),
            ),
            textScaler: MediaQuery.textScalerOf(context),
            textDirection: TextDirection.ltr,
            maxLines: 1,
          );
          textPainter.layout(maxWidth: constraints.maxWidth);
          if (textPainter.didExceedMaxLines ||
              textPainter.height > constraints.maxHeight) {
            return SizedBox();
          }
          return Text(text, maxLines: 1, style: TextStyle(fontSize: 24, height: 1.0));
        }
    );
  }
}