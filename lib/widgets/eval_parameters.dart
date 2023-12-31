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

import '../state.dart';

void setDelta(String newDelta) async {
  if (newDelta == "") {
    return;
  }
  (await GlobalState.preferences).setDouble('delta', double.parse(newDelta));
}

class EvalParameters extends StatelessWidget {
  const EvalParameters({super.key});

  @override
  Widget build(BuildContext context) {
    return Column(
        children: [
          TextField(
            decoration: InputDecoration(labelText: "Delta"),
            keyboardType: TextInputType.number,
            inputFormatters: <TextInputFormatter>[
              FilteringTextInputFormatter.allow(RegExp(r'^\d*\.?\d*'), replacementString: "")
            ],
            onChanged: setDelta,
          ),
          TextField(
            decoration: InputDecoration(labelText: "Positions when evaluating"),
            keyboardType: TextInputType.number,
            inputFormatters: <TextInputFormatter>[
              FilteringTextInputFormatter.digitsOnly
            ], // Only numbers can be entered
          ),
          TextField(
            decoration: InputDecoration(labelText: "Positions when playing"),
            keyboardType: TextInputType.number,
            inputFormatters: <TextInputFormatter>[
              FilteringTextInputFormatter.digitsOnly
            ], // Only numbers can be entered
          )
        ]
    );
  }
}
