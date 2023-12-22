import 'dart:math';

import 'package:flutter/material.dart';
import '../state.dart';
import 'case.dart';
import '../ffi_engine.dart';

class Board extends StatelessWidget {
  final double size;
  final double squareSize;
  final Function playMove;
  final Function undo;

  Board(this.size, this.squareSize, this.playMove, this.undo, {super.key}) {
    assert(squareSize * 8 <= size);
  }

  @override
  Widget build(BuildContext context) {
    var board = GlobalState.board;
    var result = ListenableBuilder(
      listenable: GlobalState.board,
      builder: (BuildContext context, Widget? widget) =>
        Container(
          height: size,
          width: size,
          alignment: Alignment.center,
          child: Container(
            alignment: Alignment.center,
            child: Table(
              defaultColumnWidth: FixedColumnWidth(squareSize),
              children: List.generate(8, (x) => TableRow(
                children: List.generate(8, (y) {
                  var index = 8 * x + y;
                  return Case(getState(index, board), index, squareSize, () => playMove(8*x + y), undo);
                })
              ))
            )
          )
        )
    );
    return result;
  }
}

CaseState getState(int square, BoardState state) {
  if ((1 << square) & state.black() != 0) {
    return CaseState.black;
  } else if ((1 << square) & state.white() != 0) {
    return CaseState.white;
  } else {
    return CaseState.empty;
  }
}