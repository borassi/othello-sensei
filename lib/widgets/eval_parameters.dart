/*
 * Copyright (c) 2023. Michele Borassi
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

import 'dart:ffi';

import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:othello_sensei/widgets/fixed_width_widget.dart';

import '../state.dart';

void setDelta(String? newDelta) async {
  GlobalState.preferences.set('Delta', newDelta == null ? null : double.parse(newDelta));
}

class ParametersTextField extends StatelessWidget {
  final String labelText;
  final TextInputFormatter formatter;
  @override
  final void Function(String)? onChanged;

  const ParametersTextField(this.labelText, this.formatter, this.onChanged, {super.key});

  @override
  Widget build(BuildContext context) {
    return TextField(
      decoration: InputDecoration(
        labelText: labelText,
        labelStyle: Theme.of(context).textTheme.bodySmall!,
      ),
      keyboardType: TextInputType.number,
      style: Theme.of(context).textTheme.bodyMedium!,
      inputFormatters: <TextInputFormatter>[formatter],
      onChanged: onChanged,
      controller: TextEditingController(text: GlobalState.preferences.get(labelText).toString()),
    );
  }

}

class EvalParameters extends FixedWidthWidget {
  const EvalParameters(super.squareSize, {super.key});

  @override
  Widget buildChild(BuildContext context) {
    return ListenableBuilder(
        listenable: GlobalState.preferences,
        builder: (BuildContext context, Widget? widget) => Column(
          children: [
            ParametersTextField(
              "Delta",
              FilteringTextInputFormatter.allow(RegExp(r'^\d*\.?\d*'), replacementString: ""),
              setDelta,
            ),
            ParametersTextField(
              "Positions when evaluating",
              FilteringTextInputFormatter.digitsOnly,
              null,
            ),
            ParametersTextField(
              "Positions when playing",
              FilteringTextInputFormatter.digitsOnly,
              null,
            ),
          ]
        )
    );
  }
}
