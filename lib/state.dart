/*
 * Copyright (c) 2023. Michele Borassi
 *
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

import 'dart:convert';
import 'dart:ffi';
import 'dart:io';

import 'package:ffi/ffi.dart';
import 'package:flutter/material.dart';
import 'package:othello_sensei/utils.dart';
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

class GlobalState {
  static var board = BoardState();
  static var actionWhenPlay = ActionWhenPlayState();
  static var squareSize = SquareSizeState();
  static List<AnnotationState> annotations = List.generate(64, (index) => AnnotationState());
  static var globalAnnotations = GlobalAnnotationState();
  static late final PreferencesState preferences;
  static late Pointer<Void> ffiMain;
  static const Main main = Main();
  static late ThorMetadataState thorMetadata;

  static Future<void> init() async {
    await maybeCopyAssetsToLocalPath();
    print('Copied!');
    NativeCallable<SetBoardFunction> setBoardCallback = NativeCallable.listener(setBoard);
    NativeCallable<UpdateAnnotationsFunction> setAnnotationsCallback = NativeCallable.listener(updateAnnotations);
    var localAssetPathVar = await localAssetPath();
    preferences = await PreferencesState.create();
    print('MainInit!');
    ffiMain = ffiEngine.MainInit(
        join(localAssetPathVar, 'pattern_evaluator.dat').toNativeUtf8().cast<Char>(),
        join(localAssetPathVar, 'book').toNativeUtf8().cast<Char>(),
        join(localAssetPathVar, 'thor').toNativeUtf8().cast<Char>(),
        setBoardCallback.nativeFunction,
        setAnnotationsCallback.nativeFunction
    );
    thorMetadata = ThorMetadataState(ffiEngine.MainGetThorMetadata(ffiMain));
    print('DoneMainInit!');
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

class PreferencesState with ChangeNotifier {
  final Map<String, dynamic> defaultPreferences = {
    'Number of threads': Platform.numberOfProcessors,
    'Positions when evaluating': 1000000000000,
    'Positions when playing': 50000000,
    'Seconds until first evaluation': 0.1,
    'Seconds between evaluations': 1.0,
    'Delta': 6.0,
    'Use book': true,
    'Approximate': false,
    'Lower': -63,
    'Upper': 63,
    'Tab': 0,
  };
  late final SharedPreferences _preferences;

  PreferencesState._initialize(this._preferences);

  static Future<PreferencesState> create() async {
    var preferences = await SharedPreferences.getInstance();
    return PreferencesState._initialize(preferences);
  }

  dynamic get(String name) {
    _checkPreferenceName(name);
    return _preferences.get(name) ?? defaultPreferences[name];
  }

  void fillEvaluateParams(EvaluateParams params, bool isFirst) {
    params.lower = get('Lower');
    params.upper = get('Upper');
    params.max_positions = get('Positions when evaluating');
    params.max_time = isFirst ? get('Seconds until first evaluation') : get('Seconds between evaluations');
    params.delta = get('Delta');
    params.n_threads = get('Number of threads');
    params.approx = get('Approximate');
    params.use_book = get('Use book');

    if (Main.tabName[get('Tab')] == 'Archive') {
      params.thor_filters.use_thor = true;
      params.thor_filters.max_games = 100;
      params.thor_filters.start_year = 1000;
      params.thor_filters.end_year = 3000;
    } else {
      params.thor_filters.use_thor = false;
    }
  }

  Future<void> setAll(Map<String, dynamic> newValues) async {
    for (var entry in newValues.entries) {
      await setNoUpdate(entry.key, entry.value);
    }
    notifyListeners();
  }

  Future<void> set(String name, dynamic value) async {
    await setNoUpdate(name, value);
    notifyListeners();
  }

  void reset() async {
    setAll(defaultPreferences);
  }

  Future<void> setNoUpdate(String name, dynamic value) async {
    _checkPreferenceName(name);
    if (value == null) {
      return;
    }
    switch (value.runtimeType) {
      case bool:
        await _preferences.setBool(name, value);
      case int:
        await _preferences.setInt(name, value);
      case double:
        await _preferences.setDouble(name, value);
      case String:
        await _preferences.setString(name, value);
      case const (List<String>):
        await _preferences.setStringList(name, value);
      default:
        throw Exception('Invalid preference type ${value.runtimeType} for value ${value}');
    }
  }

  void _checkPreferenceName(String name) {
    if (!defaultPreferences.containsKey(name)) {
      throw Exception('Invalid preference name ${name}');
    }
  }
}

class GlobalAnnotationState with ChangeNotifier {
  Annotations? annotations = null;

  void reset() {
    annotations = null;
  }

  int getNumThorGames() {
    if (annotations == null) {
      return 0;
    }
    return annotations!.num_thor_games;
  }

  void setState(Annotations annotations) {
    this.annotations = annotations;
    notifyListeners();
  }

  String getPositions() {
    if (annotations == null) {
      return '-';
    }
    return prettyPrintDouble(annotations!.positions.toDouble());
  }

  String getPositionsPerSec() {
    if (annotations == null || annotations!.positions_calculated == 0) {
      return '-';
    }
    return prettyPrintDouble(annotations!.positions_calculated / annotations!.seconds);
  }

  String getTimeString() {
    if (annotations == null) {
      return '-';
    }
    return annotations!.seconds.toStringAsFixed(1);
  }

  String getMissing() {
    if (annotations == null) {
      return '-';
    }
    return prettyPrintDouble(annotations!.missing.toDouble());
  }
}

class SourcePlayerIndex {
  int sourceIndex;
  int playerIndex;
  SourcePlayerIndex(this.sourceIndex, this.playerIndex);
}

class ThorMetadataState {
  ThorMetadata thorMetadata;
  Map<String, SourcePlayerIndex> playerStringToIndex;

  ThorMetadataState(this.thorMetadata) : playerStringToIndex = {} {
    var playerToSources = <String, List<SourcePlayerIndex>>{};
    for (int i = 0; i < thorMetadata.num_sources; ++i) {
      ThorSourceMetadata source = thorMetadata.sources.elementAt(i).value.ref;
      for (int j = 0; j < source.num_players; ++j) {
        String player = source.players.elementAt(j).value.cast<Utf8>().toDartString();
        if (!playerToSources.containsKey(player)) {
          playerToSources[player] = [];
        }
        playerToSources[player]!.add(SourcePlayerIndex(i, j));
      }
    }

    for (var playerSource in playerToSources.entries) {
      var player = playerSource.key;
      var indices = playerSource.value;
      if (indices.length == 1) {
        playerStringToIndex[player] = indices[0];
      } else {
        for (var index in indices) {
          var playerString = '$player / ${thorMetadata.sources[index.sourceIndex].ref.name.cast<Utf8>().toDartString()}';
          playerStringToIndex[playerString] = index;
        }
      }
    }
  }

  void setBlack(List<String> elements) {
    List<int> nextIndex = List.generate(thorMetadata.num_sources, (index) => 0);

    for (var element in elements) {
      var index = playerStringToIndex[element]!;
      var sourceIndex = index.sourceIndex;
      thorMetadata.sources[sourceIndex].ref.selected_blacks[nextIndex[sourceIndex]++] = index.playerIndex;
    }

    for (int i = 0; i < thorMetadata.num_sources; ++i) {
      thorMetadata.sources[i].ref.selected_blacks[nextIndex[i]] = -1;
    }
  }
  void setWhite(List<String> elements) {
    List<int> nextIndex = List.generate(thorMetadata.num_sources, (index) => 0);

    for (var element in elements) {
      var index = playerStringToIndex[element]!;
      var sourceIndex = index.sourceIndex;
      thorMetadata.sources[sourceIndex].ref.selected_whites[nextIndex[sourceIndex]++] = index.playerIndex;
    }

    for (int i = 0; i < thorMetadata.num_sources; ++i) {
      thorMetadata.sources[i].ref.selected_whites[nextIndex[i]] = -1;
    }
  }
}