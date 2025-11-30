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

import 'package:auto_size_text/auto_size_text.dart';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:othello_sensei/widgets_spacers/app_sizes.dart';
import 'package:othello_sensei/widgets_spacers/text_sizes.dart';

class AnyText extends StatelessWidget {
  final TextType type;
  final String text;
  final TextStyle? style;
  final int maxLines;
  final bool intrinsicSize;
  final TextAlign? textAlign;

  const AnyText(
    this.type,
    this.text, {
    super.key,
    this.style,
    this.textAlign,
    this.intrinsicSize = false,
    this.maxLines = 1,
  });

  @override
  Widget build(BuildContext context) {
    var fontSize = textSize(context, type);
    var style =
        (this.style ?? Theme.of(context).textTheme.bodyMedium ?? TextStyle())
            .copyWith(fontSize: fontSize);

    if (intrinsicSize) {
      return Text(
        text,
        style: style,
        textAlign: textAlign,
        maxLines: maxLines,
      );
    }

    return LayoutBuilder(
      builder: (context, constraints) {
        if (constraints.hasInfiniteWidth) {
          return Text(text, style: style, maxLines: maxLines);
        }
        final textPainter = TextPainter(
          text: TextSpan(text: text, style: style),
          maxLines: maxLines,
          textDirection: TextDirection.ltr,
        )..layout(maxWidth: constraints.maxWidth);
        if (textPainter.didExceedMaxLines ||
            textPainter.height > constraints.maxHeight ||
            textPainter.width > constraints.maxWidth) {
          print('Text "$text" does not fit at size $fontSize');
        }
        return AutoSizeText(
          text,
          style: style,
          textAlign: textAlign,
          minFontSize: 1,
          maxLines: maxLines,
        );
      },
    );
  }
}

class SmallText extends AnyText {
  const SmallText(text, {TextStyle? style, TextAlign? textAlign, int maxLines = 1, bool intrinsicSize = false, super.key})
    : super(TextType.small, text, style: style, textAlign: textAlign, intrinsicSize: intrinsicSize, maxLines: maxLines);
}

class MediumText extends AnyText {
  const MediumText(text, {TextStyle? style, TextAlign? textAlign, int maxLines = 1, bool intrinsicSize = false, super.key})
    : super(TextType.medium, text, style: style, textAlign: textAlign, intrinsicSize: intrinsicSize, maxLines: maxLines);
}

class LargeText extends AnyText {
  const LargeText(text, {TextStyle? style, TextAlign? textAlign, int maxLines = 1, bool intrinsicSize = false, super.key})
    : super(TextType.large, text, style: style, textAlign: textAlign, intrinsicSize: intrinsicSize, maxLines: maxLines);
}

// This is the widget with the confirmation logic
class SenseiTextFormField extends StatefulWidget {
  final void Function(String) onSubmitted;
  final TextType type;
  final TextEditingController? controller;
  final FocusNode? focusNode;
  final TextStyle? style;
  final TextAlign? textAlign;
  final TextInputType? keyboardType;
  final List<TextInputFormatter> inputFormatters;
  final bool autocorrect;

  const SenseiTextFormField({
    required this.onSubmitted,
    this.type = TextType.medium,
    this.controller,
    this.focusNode,
    this.style,
    this.textAlign,
    this.keyboardType,
    this.inputFormatters = const [],
    this.autocorrect = false,
    super.key,
  });

  @override
  State<SenseiTextFormField> createState() => _SenseiTextFormFieldState();
}

class _SenseiTextFormFieldState extends State<SenseiTextFormField> {
// Internal fallbacks
  TextEditingController? _internalController;
  FocusNode? _internalFocusNode;

  // Getters to determine which one to use
  TextEditingController get _controller => widget.controller ?? (_internalController ??= TextEditingController());
  FocusNode get _focusNode => widget.focusNode ?? (_internalFocusNode ??= FocusNode());

  @override
  void initState() {
    super.initState();
    _focusNode.addListener(_onFocusChange);
  }

  @override
  void dispose() {
    _focusNode.removeListener(_onFocusChange);
    _onSubmitted(_controller.text);
    _internalController?.dispose();
    _internalFocusNode?.dispose();
    super.dispose();
  }

  void _onFocusChange() {
    if (!_focusNode.hasFocus) {
      _onSubmitted(_controller.text);
    }
  }

  void _onSubmitted(String text) {
    widget.onSubmitted(text);
  }

  @override
  Widget build(BuildContext context) {
    var style = Theme.of(context).textTheme.bodyMedium!.merge(widget.style);
    var fontSize = textSize(context, widget.type);
    return TextField(
      controller: _controller,
      focusNode: _focusNode,
      onSubmitted: _onSubmitted,
      style: style.merge(TextStyle(fontSize: fontSize)),
      textAlign: widget.textAlign ?? TextAlign.left,
      keyboardType: widget.keyboardType,
      inputFormatters: widget.inputFormatters,
      autocorrect: widget.autocorrect,
    );
  }
}
