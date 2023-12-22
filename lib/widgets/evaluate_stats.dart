
import 'package:flutter/material.dart';

import '../state.dart';
import 'case.dart';

class EvaluateStats extends StatelessWidget {

  @override
  Widget build(BuildContext context) {
    return ListenableBuilder(
      listenable: GlobalState.globalAnnotations,
      builder: (BuildContext context, Widget? widget) => Table(
        border: TableBorder.all(),
        columnWidths: const <int, TableColumnWidth>{
          0: IntrinsicColumnWidth(),
          1: IntrinsicColumnWidth(),
        },
        defaultVerticalAlignment: TableCellVerticalAlignment.middle,
        children: <TableRow>[
          TableRow(
            children: <Widget>[
              TableCell(child: Text("Positions"),),
              TableCell(child: TextAnnotationListenable(textFunction: GlobalState.globalAnnotations.getPositions)),
            ],
          ),
          TableRow(
            children: <Widget>[
              TableCell(child: Text("Time"),),
              TableCell(child: TextAnnotationListenable(textFunction: GlobalState.globalAnnotations.getTimeString)),
            ],
          ),
          TableRow(
            children: <Widget>[
              const TableCell(child: Text("Positions / sec:"),),
              TableCell(child: TextAnnotationListenable(textFunction: GlobalState.globalAnnotations.getPositionsPerSec)),
            ],
          ),
          TableRow(
            children: <Widget>[
              TableCell(child: Text("Missing"),),
              TableCell(child: Text("200T"),),
            ],
          ),
          TableRow(
            children: <Widget>[
              const TableCell(child: Text("Empties"),),
              TableCell(
                child: TextListenable(listenable: GlobalState.board, textFunction: () => "${GlobalState.board.emptySquares()}"),
              ),
            ],
          )
        ],
      )
    );
  }
}

class TextListenable extends ListenableBuilder {
  TextListenable({super.key, required super.listenable, required String? Function() textFunction}) : super(
      builder: (BuildContext context, Widget? widget) => Text(textFunction() ?? "-"));
}

class TextAnnotationListenable extends TextListenable {
  TextAnnotationListenable({required textFunction}) : super(listenable: GlobalState.globalAnnotations, textFunction: textFunction);
}