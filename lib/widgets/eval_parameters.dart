
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
