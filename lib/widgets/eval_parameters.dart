
import 'dart:ffi';

import 'package:flutter/material.dart';
import 'package:flutter/services.dart';

import '../state.dart';

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
            onChanged: (String newDelta) async { await (await GlobalState.preferences).setDouble('delta', double.parse(newDelta)); },
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
