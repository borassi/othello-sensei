
import 'package:flutter/material.dart';

import '../state.dart';
import 'board.dart';
import 'case.dart';

Widget getCase(BuildContext context, CaseState state, double squareSize) {
  var textColor = state == CaseState.black ? Colors.white : Colors.black;
  var text = state == CaseState.black ?
      "${GlobalState.board.blackDisks()}" :
      state == CaseState.white ?
          "${GlobalState.board.whiteDisks()}" :
          "${GlobalState.board.emptySquares()}";
  return Stack(
      alignment: Alignment.center,
      children: [
        Case(
          state,
          255,
          squareSize,
          () => {},
          () => {},
        ),
        Text(
          text,
          style: TextStyle(
              fontSize: Theme.of(context).textTheme.bodyLarge!.fontSize,
              fontWeight: FontWeight.bold,
              height: 1,
              color: textColor,
          ),
        )
      ]
  );
}

class DiskCount extends StatelessWidget {
  final double squareSize;

  const DiskCount(this.squareSize, {super.key});

  @override
  Widget build(BuildContext context) {
    return ListenableBuilder(
      listenable: GlobalState.board,
      builder: (BuildContext context, Widget? widget) => Row(
        mainAxisAlignment: MainAxisAlignment.center,
        children: [
          getCase(context, CaseState.black, squareSize),
          ConstrainedBox(
              constraints: BoxConstraints(maxHeight: squareSize),
              child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    Text("Error:"),
                    ListenableBuilder(
                      listenable: GlobalState.globalAnnotations,
                      builder: (BuildContext context, Widget? widget) => Text(
                          "12", style: Theme.of(context).textTheme.bodyLarge!
                      )
                    )
                  ]
              )
          ),
          Spacer(),
          ConstrainedBox(
              constraints: BoxConstraints(maxHeight: squareSize),
              child: Column(
                  crossAxisAlignment: CrossAxisAlignment.end,
                  children: [
                    Text("Error"),
                    ListenableBuilder(
                      listenable: GlobalState.globalAnnotations,
                      builder: (BuildContext context, Widget? widget) => Text(
                          "12", style: Theme.of(context).textTheme.bodyLarge!
                      )
                    )
                  ]
              )
          ),
          // getCase(CaseState.empty, squareSize),
          getCase(context, CaseState.white, squareSize)
        ]
      )
    );
  }
}