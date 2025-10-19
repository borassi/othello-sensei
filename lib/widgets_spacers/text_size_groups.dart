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

import 'dart:async';

import 'package:flutter/material.dart';
import 'package:flutter/services.dart';

enum TextType { small, medium, large }

class TextGroup with ChangeNotifier {
  final double maxSize;
  double _fontSize;
  Timer? _rebuildTimer;
  Object? _bottleneck;

  TextGroup({required this.maxSize})
    : _fontSize = maxSize,
      _rebuildTimer = null;

  bool get needsRebuild {
    return _rebuildTimer?.isActive ?? false;
  }

  double get fontSize {
    return needsRebuild ? 0 : _fontSize;
  }

  void reset() {
    _fontSize = maxSize;
    _notifyListenersWithDelay();
  }

  bool _fontSizeFits(
    String text,
    TextStyle style,
    double size,
    BuildContext context,
    BoxConstraints constraints,
    int maxLines,
  ) {
    final TextScaler scaler = MediaQuery.of(context).textScaler;
    final textPainter = TextPainter(
      text: TextSpan(
        text: text,
        style: style.copyWith(fontSize: size),
      ),
      textDirection: TextDirection.ltr,
      maxLines: maxLines,
      textScaler: scaler,
    );
    textPainter.layout(maxWidth: constraints.maxWidth);
    return !textPainter.didExceedMaxLines &&
        textPainter.height <= constraints.maxHeight;
  }

  void registerText(
    String text,
    TextStyle style,
    BuildContext context,
    BoxConstraints constraints,
    Object object,
    int maxLines) {
    if (text.isEmpty ||
        _fontSizeFits(text, style, _fontSize, context, constraints, maxLines)) {
      return;
    }
    double low = 0;
    double high = _fontSize;
    while (high > low + 0.2) {
      assert(_fontSizeFits(text, style, low, context, constraints, maxLines));
      assert(!_fontSizeFits(text, style, high, context, constraints, maxLines));
      final mid = low + (high - low) / 2;
      if (_fontSizeFits(text, style, mid, context, constraints, maxLines)) {
        low = mid;
      } else {
        high = mid;
      }
    }
    _fontSize = low;
    _bottleneck = object;
    _notifyListenersWithDelay();
  }

  void _notifyListenersWithDelay() {
    if (_rebuildTimer?.isActive ?? false) {
      _rebuildTimer!.cancel();
    }
    _rebuildTimer = Timer(const Duration(milliseconds: 100), () {
      notifyListeners();
    });
  }
}

class AnyText extends StatefulWidget {
  static final smallGroup = TextGroup(maxSize: 50.0);
  static final mediumGroup = TextGroup(maxSize: 50.0);
  static final largeGroup = TextGroup(maxSize: 200.0);

  final TextGroup _group;
  final String text;
  final double? height;
  final double? width;
  final TextStyle? style;
  final Alignment alignment;
  final int maxLines;
  final bool intrinsicSize;

  AnyText(
      TextType type,
      this.text, {super.key,
        this.alignment = Alignment.center,
        this.height,
        this.width,
        this.style,
        this.maxLines = 1,
        this.intrinsicSize = false,
      }) : _group = getGroup(type);

  static TextGroup getGroup(TextType type) {
    switch (type) {
      case TextType.small:
        return smallGroup;
      case TextType.medium:
        return mediumGroup;
      case TextType.large:
        return largeGroup;
    }
  }

  @override
  State<StatefulWidget> createState() {
    return _AnyTextState();
  }
}

class _AnyTextState extends State<AnyText> {
  TextStyle getStyle(BuildContext context) {
    return Theme.of(context).textTheme.bodyMedium!.merge(widget.style);
  }

  @override
  void dispose() {
    if (widget._group._bottleneck == this) {
      widget._group.reset();
    }
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    if (widget._group._bottleneck == this) {
      widget._group.reset();
    }
    if (widget.intrinsicSize) {
      return Text(
        widget.text,
        style: getStyle(
          context,
        ).merge(TextStyle(fontSize: widget._group.fontSize)),
      );
    }
    return Container(
      width: widget.width,
      height: widget.height,
      alignment: widget.alignment,
      child: ListenableBuilder(
        listenable: widget._group,
        builder: (BuildContext context, Widget? w) {
          return LayoutBuilder(
            builder: (BuildContext ctx, BoxConstraints constraints) {
              widget._group.registerText(widget.text,
                  getStyle(context),
                  context,
                  constraints,
                  this,
                  widget.maxLines);
              return Text(
                widget.text,
                style: getStyle(context).copyWith(
                  fontSize: widget._group.fontSize,
                ),
                textAlign: TextAlign.center,
              );
            },
          );
        },
      ),
    );
  }
}

class SmallText extends AnyText {
  SmallText(
    text, {
    double? height,
    double? width,
    TextStyle? style,
    Alignment alignment = Alignment.center,
    int maxLines = 1,
    bool intrinsicSize = false,
    super.key,
  }) : super(
         TextType.small,
         text,
         height: height,
         width: width,
         alignment: alignment,
         style: style,
         maxLines: maxLines,
         intrinsicSize: intrinsicSize,
       );
}

class MediumText extends AnyText {
  MediumText(
    text, {
    double? height,
    double? width,
    TextStyle? style,
    Alignment alignment = Alignment.center,
    int maxLines = 1,
    bool intrinsicSize = false,
    super.key,
  }) : super(
         TextType.medium,
         text,
         height: height,
         width: width,
         alignment: alignment,
         style: style,
         maxLines: maxLines,
         intrinsicSize: intrinsicSize,
       );
}

class LargeText extends AnyText {
  LargeText(
    text, {
    double? height,
    double? width,
    TextStyle? style,
    Alignment alignment = Alignment.center,
    int maxLines = 1,
    bool intrinsicSize = false,
    super.key,
  }) : super(
         TextType.large,
         text,
         height: height,
         width: width,
         alignment: alignment,
         style: style,
         maxLines: maxLines,
         intrinsicSize: intrinsicSize,
       );
}

class SenseiTextFormField extends StatelessWidget {
  final TextType type;
  final void Function(String)? onChanged;
  final void Function()? onEditingComplete;
  final void Function(PointerDownEvent)? onTapOutside;
  final TextStyle? style;
  final TextAlign? textAlign;
  final TextInputType? keyboardType;
  final List<TextInputFormatter> inputFormatters;
  final TextEditingController? controller;
  final bool onTapAlwaysCalled;
  final bool autocorrect;
  final FocusNode? focusNode;

  const SenseiTextFormField({
    this.type = TextType.medium,
    this.style,
    this.controller,
    this.onChanged,
    this.textAlign,
    this.keyboardType,
    this.inputFormatters = const [],
    this.onEditingComplete,
    this.onTapOutside,
    this.onTapAlwaysCalled = false,
    this.autocorrect = false,
    this.focusNode,
    super.key,
  });

  @override
  Widget build(BuildContext context) {
    var style = Theme.of(context).textTheme.bodyMedium!.merge(this.style);
    return ListenableBuilder(
      listenable: AnyText.getGroup(type),
      builder: (BuildContext context, Widget? w) {
        var fontSize = AnyText.getGroup(type).fontSize;
        return TextField(
          style: style.merge(TextStyle(fontSize: fontSize)),
          onChanged: onChanged,
          controller: controller,
          textAlign: textAlign ?? TextAlign.left,
          keyboardType: keyboardType,
          inputFormatters: inputFormatters,
          onEditingComplete: onEditingComplete,
          onTapOutside: onTapOutside,
          onTapAlwaysCalled: onTapAlwaysCalled,
          autocorrect: autocorrect,
          focusNode: focusNode,
        );
      }
    );
  }
}
