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

import 'package:flutter/gestures.dart';
import 'package:flutter/material.dart';
import '../widgets_spacers/app_sizes.dart';
import '../state.dart';
import 'case.dart';

class Coordinate extends StatelessWidget {
  final bool vertical;
  final int position;
  final double smallSize;
  const Coordinate(this.vertical, this.position, this.smallSize, {super.key});
  
  @override
  Widget build(BuildContext context) {
    String content;
    if (vertical) {
      content = '${position + 1}';
    } else {
      content = String.fromCharCode('A'.codeUnitAt(0) + position);
    }
    var appSizes = Theme.of(context).extension<AppSizes>()!;
    var squareSize = appSizes.squareSize;
    Widget contentWidget = Container();
    if (GlobalState.preferences.get('Margin size') == 'Coordin') {
      contentWidget = Center(
          child: Text(
            content,
            style: Theme.of(context).textTheme.bodyMedium!,
          )
      );
    }
    return SizedBox(
      width: vertical ? smallSize : squareSize,
      height: vertical ? squareSize : smallSize,
      child: contentWidget
    );
  }
}

int getIndex(Offset localPosition, double sideMargin, double topMargin, double squareSize) {
  int xSquare = ((localPosition.dx - sideMargin) / squareSize).floor();
  int ySquare = ((localPosition.dy - topMargin) / squareSize).floor();
  if (xSquare < 0 || ySquare < 0 || xSquare >= 8 || ySquare >= 8) {
    return -1;
  }
  return (7 - xSquare) + 8 * (7 - ySquare);
}

void setUpDisk(int index, bool secondary) {
  if (!GlobalState.setupBoardState.settingUpBoard) {
    return;
  }
  var caseState = GlobalState.setupBoardState.onClick;
  if (secondary) {
    if (caseState == CaseState.black) {
      caseState = CaseState.white;
    } else if (caseState == CaseState.white) {
      caseState = CaseState.black;
    }
  }
  switch(caseState) {
    case CaseState.black:
      GlobalState.ffiEngine.SetBlackSquare(GlobalState.ffiMain, index);
      return;
    case CaseState.white:
      GlobalState.ffiEngine.SetWhiteSquare(GlobalState.ffiMain, index);
      return;
    case CaseState.empty:
      GlobalState.ffiEngine.SetEmptySquare(GlobalState.ffiMain, index);
      return;
  }
}

class Board extends StatelessWidget {
  const Board({super.key});

  @override
  Widget build(BuildContext context) {
    var board = GlobalState.board;
    var colorScheme = Theme.of(context).colorScheme;
    var appSizes = Theme.of(context).extension<AppSizes>()!;
    var sideMargin = appSizes.sideMargin;
    var internalMargin = appSizes.margin;
    return LayoutBuilder(
      builder: (BuildContext context, BoxConstraints constraints) {
        var squareSize = Theme.of(context).extension<AppSizes>()!.squareSize;
        return ListenableBuilder(
          listenable: GlobalState.board,
          builder: (BuildContext context, Widget? widget) {
            bool horizontal = (
                Theme.of(context).extension<AppSizes>()!.layout == AppLayout.horizontalBrokenBar ||
                Theme.of(context).extension<AppSizes>()!.layout == AppLayout.horizontalFullBar);
            var rightCoordinates = horizontal && GlobalState.preferences.get("Board on the right in horizontal mode");
            var leftMargin = rightCoordinates ? 0.0 : sideMargin;
            var topMargin = appSizes.brokenAppBar() ? sideMargin : internalMargin;
            return RawGestureDetector(
                gestures: {
                  PanGestureRecognizer: GestureRecognizerFactoryWithHandlers<PanGestureRecognizer>(
                        () => PanGestureRecognizer(
                      allowedButtonsFilter: (int buttons) { return buttons & kPrimaryButton != 0; },
                    ),
                        (PanGestureRecognizer instance) {
                      instance
                        ..onStart = (DragStartDetails details) {
                          var index = getIndex(details.localPosition, leftMargin, topMargin, squareSize);
                          setUpDisk(index, false);
                        }
                        ..onUpdate = (DragUpdateDetails details) {
                          var index = getIndex(details.localPosition, leftMargin, topMargin, squareSize);
                          setUpDisk(index, false);
                        }
                        ..onEnd = (DragEndDetails details) {
                          var index = getIndex(details.localPosition, leftMargin, topMargin, squareSize);
                          if (!GlobalState.setupBoardState.settingUpBoard) {
                            GlobalState.playMove(index, false);
                          }
                        };
                    },
                  ),
                },
                child: RawGestureDetector(
                    gestures: {
                      PanGestureRecognizer: GestureRecognizerFactoryWithHandlers<PanGestureRecognizer>(
                            () => PanGestureRecognizer(
                          allowedButtonsFilter: (int buttons) { return buttons & kSecondaryButton != 0; },
                        ),
                            (PanGestureRecognizer instance) {
                          instance
                            ..onStart = (DragStartDetails details) {
                              var index = getIndex(details.localPosition, leftMargin, topMargin, squareSize);
                              setUpDisk(index, true);
                            }
                            ..onUpdate = (DragUpdateDetails details) {
                              var index = getIndex(details.localPosition, leftMargin, topMargin, squareSize);
                              setUpDisk(index, true);
                            }
                            ..onEnd = (DragEndDetails details) {
                              if (!GlobalState.setupBoardState.settingUpBoard) {
                                GlobalState.undo();
                              }
                            };
                        },
                      ),
                    },
                    child: Stack(
                        children: <Widget>[
                          Table(
                              defaultColumnWidth: FixedColumnWidth(squareSize),
                              columnWidths: {(rightCoordinates ? 8 : 0): FixedColumnWidth(sideMargin)},
                              children:
                              [
                                TableRow(
                                  children:
                                    (rightCoordinates ? <Widget>[] : <Widget>[Container()]) +
                                    List.generate(8, (y) => Coordinate(false, y, topMargin)) +
                                    (rightCoordinates ? <Widget>[Container()] : <Widget>[])
                                )
                              ] +
                                  List.generate(8, (x) => TableRow(
                                      children:
                                      (rightCoordinates ? <Widget>[] : <Widget>[Coordinate(true, x, sideMargin)]) +
                                          List.generate(8, (y) {
                                            var index = 63 - 8 * x - y;
                                            return Case(getState(index, board), index, index == GlobalState.board.lastMove);
                                          }) +
                                      (rightCoordinates ? <Widget>[Coordinate(true, x, sideMargin)] : <Widget>[])
                                  )
                                  )
                          )
                        ] +
                            List.generate(4, (index) => Positioned(
                                left: leftMargin + (2-0.1) * squareSize + (index % 2) * 4 * squareSize,
                                top: topMargin + (2-0.1) * squareSize + (index ~/ 2) * 4 * squareSize,
                                child:
                                Container(
                                  decoration: BoxDecoration(
                                    color: colorScheme.background,
                                    shape: BoxShape.circle,
                                  ),
                                  width: squareSize * 0.2,
                                  height: squareSize * 0.2,
                                )
                            ))
                    )
                )
            );
          }
        );
      }
    );
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