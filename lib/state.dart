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

import 'package:ffi/ffi.dart';
import 'package:flutter/material.dart';
import 'package:othello_sensei_flutter/utils.dart';
import 'package:path/path.dart';
import 'package:shared_preferences/shared_preferences.dart';

import 'ffi_bridge.dart';
import 'ffi_engine.dart';
import 'files.dart';
import 'main.dart';

enum ActionWhenPlay {
  playBlack,
  playWhite,
  eval,
  none,
}

// Taken from https://en.wikipedia.org/wiki/Hamming_weight.
int bitCount(int x) {
  const m1  = 0x5555555555555555; //binary: 0101...
  const m2  = 0x3333333333333333; //binary: 00110011..
  const m4  = 0x0f0f0f0f0f0f0f0f; //binary:  4 zeros,  4 ones ...
  const h01 = 0x0101010101010101;

  x -= (x >> 1) & m1;             //put count of each 2 bits into those 2 bits
  x = (x & m2) + ((x >> 2) & m2); //put count of each 4 bits into those 4 bits
  x = (x + (x >> 4)) & m4;        //put count of each 8 bits into those 8 bits
  return (x * h01) >> 56;  //returns left 8 bits of x + (x<<8) + (x<<16) + (x<<24) + ...
}

class GlobalState {
  static var board = BoardState();
  static var actionWhenPlay = ActionWhenPlayState();
  static var squareSize = SquareSizeState();
  static List<AnnotationState> annotations = List.generate(64, (index) => AnnotationState());
  static var globalAnnotations = GlobalAnnotationState();
  static final Future<SharedPreferences> preferences = SharedPreferences.getInstance();
  static late final Future<Pointer<Void>> ffiMain;

  static Future<Pointer<Void>> getFFIMain(
      Pointer<NativeFunction<SetBoardFunction>> setBoardCallback,
      Pointer<NativeFunction<UpdateAnnotationsFunction>> setAnnotationsCallback) async {
    await maybeCopyAssetsToLocalPath();
    var localAssetPathVar = await localAssetPath();
    return ffiEngine.MainInit(
        join(localAssetPathVar, "pattern_evaluator.dat").toNativeUtf8().cast<Char>(),
        join(localAssetPathVar, "book").toNativeUtf8().cast<Char>(),
        setBoardCallback,
        setAnnotationsCallback);
  }
}

class BoardState with ChangeNotifier {
  int player;
  int opponent;
  bool blackTurn;

  BoardState() : player = 0, opponent = 0, blackTurn = true;

  void setState(BoardUpdate boardUpdate) {
    player = boardUpdate.player;
    opponent = boardUpdate.opponent;
    blackTurn = boardUpdate.black_turn;
    notifyListeners();
  }

  int black() { return blackTurn ? player : opponent; }
  int white() { return blackTurn ? opponent : player; }
  int blackDisks() { return bitCount(black()); }
  int whiteDisks() { return bitCount(white()); }
  int emptySquares() { return bitCount(~(player | opponent)); }
}

class ActionWhenPlayState with ChangeNotifier {
  ActionWhenPlay actionWhenPlay;

  ActionWhenPlayState() : actionWhenPlay = ActionWhenPlay.eval;

  void setActionWhenPlay(ActionWhenPlay actionWhenPlay) {
    this.actionWhenPlay = actionWhenPlay;
    notifyListeners();
  }

  void rotateActions() {
    actionWhenPlay = ActionWhenPlay.values[(actionWhenPlay.index + 1) % ActionWhenPlay.values.length];
    notifyListeners();
  }
}

class SquareSizeState with ChangeNotifier {
  int squareSize;

  SquareSizeState() : squareSize = 100;

  void updateSquareSize(int newSquareSize) {
    squareSize = newSquareSize;
    notifyListeners();
  }
}

class AnnotationState with ChangeNotifier {
  MoveAnnotations? annotations;
  double bestEval = double.negativeInfinity;

  void clear() {
    annotations = null;
    notifyListeners();
  }

  void setState(MoveAnnotations annotations, double bestEval) {
    this.annotations = annotations;
    this.bestEval = bestEval;
    notifyListeners();
  }
}

class GlobalAnnotationState with ChangeNotifier {
  double seconds = -1;
  int positions = -1;

  void reset() {
    seconds = double.negativeInfinity;
  }

  void setState(Annotations annotations) {
    seconds = annotations.seconds;
    positions = annotations.positions;
    notifyListeners();
  }

  String? getPositions() {
    return positions == -1 ? null : prettyPrintDouble(positions.toDouble());
  }

  String? getPositionsPerSec() {
    return positions == -1 ? null : prettyPrintDouble(positions / seconds);
  }

  String? getTimeString() {
    return positions == -1 ? null : seconds.toStringAsFixed(1);
  }
}

