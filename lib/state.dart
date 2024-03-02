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

import 'dart:ffi';
import 'dart:io';

import 'package:ffi/ffi.dart';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:othello_sensei/utils.dart';
import 'package:path/path.dart';
import 'package:receive_sharing_intent/receive_sharing_intent.dart';
import 'package:shared_preferences/shared_preferences.dart';

import 'ffi/ffi_bridge.dart';
import 'ffi/ffi_engine.dart';
import 'files.dart';
import 'widgets_windows/main.dart';

enum ActionWhenPlay {
  playBlack,
  playWhite,
  eval,
  none,
}

void setBoard(BoardUpdate boardUpdate) {
  for (int i = 0; i < 64; ++i) {
    GlobalState.annotations[i].clear();
  }
  GlobalState.board.setState(boardUpdate);
}

int currentMove() {
  if (GlobalState.globalAnnotations.annotations == null) {
    return 0;
  }
  return GlobalState.globalAnnotations.annotations!.ref.depth;
}

void updateAnnotations(int currentThread) {
  Pointer<Annotations> annotations = ffiEngine.GetCurrentAnnotations(GlobalState.ffiMain, currentThread);
  Pointer<Annotations> startAnnotations = ffiEngine.GetStartAnnotations(GlobalState.ffiMain, currentThread);
  if (annotations == nullptr || startAnnotations == nullptr) {
    return;
  }
  GlobalState.globalAnnotations.setState(annotations, startAnnotations);
  for (Pointer<Annotations> child = annotations.ref.first_child; child != nullptr; child = child.ref.next_sibling) {
    GlobalState.annotations[child.ref.move].setState(child.ref);
  }
  if (!annotations.ref.finished && !annotations.ref.analyzed) {
    GlobalState.evaluate(type: EvaluateType.subsequent);
  }
}

Future<void> copy() async {
  Pointer<Char> sequence = ffiEngine.GetSequence(GlobalState.ffiMain);
  var s = sequence.cast<Utf8>().toDartString();
  Clipboard.setData(ClipboardData(text: s));
  malloc.free(sequence);
}

Future<bool> paste() async {
  String? game = (await Clipboard.getData(Clipboard.kTextPlain))?.text;
  if (game == null) {
    return false;
  }
  var gameC = game!.toNativeUtf8().cast<Char>();
  return ffiEngine.SetSequence(GlobalState.ffiMain, gameC);
}

Future<void> pasteOrError(BuildContext context) async {
  bool success = await paste();
  if (!success && context.mounted) {
    showDialog<void>(
      context: context,
      builder: (BuildContext ctx) {
        return const AlertDialog(
          title: Text('The clipboard does not contain a game!'),
        );
      },
    );
  };
  if (GlobalState.preferences.get("Analyze on paste")) {
    analyze();
  } else {
    GlobalState.evaluate();
  }
}

void receiveOthelloQuestEvent(List<SharedMediaFile> event) {
  if (event.length != 1 || event[0].mimeType != "message/rfc822") {
    return;
  }
  var game = event[0].path.toNativeUtf8().cast<Char>();
  ffiEngine.SetSequence(GlobalState.ffiMain, game);
  if (GlobalState.preferences.get("Analyze on import")) {
    analyze();
  } else {
    GlobalState.evaluate();
  }
}

void analyze() {
  GlobalState.preferences.fillEvaluateParams(EvaluateType.analysis);
  ffiEngine.Analyze(GlobalState.ffiMain);
}

class GlobalState {
  static var board = BoardState();
  static var actionWhenPlay = ActionWhenPlayState();
  static List<AnnotationState> annotations = List.generate(64, (index) => AnnotationState());
  static var globalAnnotations = GlobalAnnotationState();
  static late final PreferencesState preferences;
  static late Pointer<Void> ffiMain;
  static const Main main = Main();
  static ThorMetadataState? thorMetadataOrNull;

  static Future<void> init() async {
    await maybeCopyAssetsToLocalPath();
    NativeCallable<SetBoardFunction> setBoardCallback = NativeCallable.listener(setBoard);
    NativeCallable<UpdateAnnotationsFunction> setAnnotationsCallback = NativeCallable.listener(updateAnnotations);
    var localAssetPathVar = await localAssetPath();
    preferences = await PreferencesState.create();
    ffiMain = ffiEngine.MainInit(
        join(localAssetPathVar, 'pattern_evaluator.dat').toNativeUtf8().cast<Char>(),
        join(localAssetPathVar, 'book').toNativeUtf8().cast<Char>(),
        join(localAssetPathVar, 'thor').toNativeUtf8().cast<Char>(),
        setBoardCallback.nativeFunction,
        setAnnotationsCallback.nativeFunction
    );
    newGame();
    if (Platform.isAndroid || Platform.isIOS) {
      ReceiveSharingIntent.getInitialMedia().then(receiveOthelloQuestEvent);
      ReceiveSharingIntent.getMediaStream().listen(receiveOthelloQuestEvent);
    }
  }
  static ThorMetadataState get thorMetadata {
    thorMetadataOrNull ??= ThorMetadataState();
    return thorMetadataOrNull!;
  }

  static void newGame() async {
    ffiEngine.NewGame(GlobalState.ffiMain);
    evaluate();
  }

  static void playMove(int i) {
    ffiEngine.PlayMove(GlobalState.ffiMain, i);
    evaluate();
  }

  static void redo() {
    ffiEngine.Redo(GlobalState.ffiMain);
    evaluate();
  }

  static void undo() {
    ffiEngine.Undo(GlobalState.ffiMain);
    evaluate();
  }

  static void setCurrentMove(int current_move) {
    ffiEngine.SetCurrentMove(GlobalState.ffiMain, current_move);
    evaluate();
  }

  static void stop() {
    ffiEngine.Stop(GlobalState.ffiMain);
  }

  static void evaluate({EvaluateType type = EvaluateType.first}) {
    if (GlobalState.actionWhenPlay.actionWhenPlay != ActionWhenPlay.eval) {
      return;
    }
    GlobalState.preferences.fillEvaluateParams(type);
    ffiEngine.Evaluate(GlobalState.ffiMain);
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

class AnnotationState with ChangeNotifier {
  Annotations? annotations;

  void clear() {
    annotations = null;
    notifyListeners();
  }

  void setState(Annotations annotations) {
    this.annotations = annotations;
    notifyListeners();
  }

  double getEval() {
    return getEvalFromAnnotations(annotations!, annotations!.father.ref.black_turn);
  }
}

enum EvaluateType {
  first,
  subsequent,
  analysis,
}

class PreferencesState with ChangeNotifier {
  final Map<String, dynamic> defaultPreferences = {
    'Number of threads': Platform.numberOfProcessors,
    'Positions when evaluating': 1000000000000,
    'Positions when playing': 50000000,
    'Seconds until first evaluation': 0.1,
    'Seconds between evaluations': 1.0,
    'Delta': 6.0,
    'Round evaluations': false,
    'Use book': true,
    'Seconds/position in game analysis': 2.0,
    'Analyze on paste': true,
    'Analyze on import': true,
    'Approximate': false,
    'Lower': -63,
    'Upper': 63,
    'Active tab': 0,
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

  void fillEvaluateParams(EvaluateType evaluateType) {
    var params = ffiEngine.MainGetEvaluateParams(GlobalState.ffiMain).ref;
    params.lower = get('Lower');
    params.upper = get('Upper');
    params.max_positions = get('Positions when evaluating');
    params.max_time_first_eval = get('Seconds until first evaluation');
    params.max_time_next_evals = get('Seconds between evaluations');
    params.max_time_analysis = get('Seconds/position in game analysis');
    params.delta = get('Delta');
    params.n_threads = get('Number of threads');
    params.approx = get('Approximate');
    params.use_book = get('Use book');
    params.thor_filters.max_games = 100;
    params.thor_filters.start_year = 1000;
    params.thor_filters.end_year = 3000;
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
  Pointer<Annotations>? annotations;
  Pointer<Annotations>? startAnnotations;

  void reset() {
    annotations = null;
  }

  int getNumThorGames() {
    if (annotations == null) {
      return 0;
    }
    return annotations![currentMove()].num_thor_games;
  }

  void setState(Pointer<Annotations> annotations, Pointer<Annotations> startAnnotations) {
    this.annotations = annotations;
    this.startAnnotations = startAnnotations;
    notifyListeners();
  }

  (List<double>, int) getAllScoresAndLastMove() {
    var annotation = startAnnotations;
    var scores = <double>[];
    var currentMoveVar = currentMove();
    int lastMove = -1;
    if (startAnnotations == null) {
      return (scores, lastMove);
    }
    if (startAnnotations!.ref.next_state_in_analysis == nullptr) {
      lastMove = currentMoveVar;
    }
    while (annotation != null && annotation != nullptr) {
      scores.add(getEvalFromAnnotations(annotation.ref, true));
      if (lastMove == -1 && (
          annotation.ref.next_state_in_analysis == nullptr
          || annotation.ref.next_state_in_analysis != annotation.ref.next_state_played
          || annotation.ref.depth == currentMoveVar)) {
        lastMove = annotation.ref.depth;
      }
      annotation = annotation.ref.next_state_in_analysis != nullptr ? annotation.ref.next_state_in_analysis : annotation.ref.next_state_played;
    }
    return (scores, lastMove);
  }

  (double, double, bool) getErrors() {
    var errorBlack = 0.0;
    var errorWhite = 0.0;
    var (allScores, _) = getAllScoresAndLastMove();
    var oldScore = 0.0;
    var hasNaN = false;
    for (double score in allScores) {
      var error = score - oldScore;
      if (error.isNaN) {
        hasNaN = true;
      } else if (error > 0) {
        errorWhite += error;
      } else {
        errorBlack += -error;
      }
      oldScore = score;
    }
    return (errorBlack, errorWhite, hasNaN);
  }

  String getPositions() {
    if (annotations == null) {
      return '-';
    }
    return prettyPrintDouble(annotations!.ref.descendants.toDouble());
  }

  String getPositionsPerSec() {
    if (annotations == null || annotations!.ref.descendants_no_book == 0) {
      return '-';
    }
    return prettyPrintDouble(annotations!.ref.descendants_no_book / annotations!.ref.seconds);
  }

  String getTimeString() {
    if (annotations == null) {
      return '-';
    }
    return annotations!.ref.seconds.toStringAsFixed(1);
  }

  String getMissing() {
    if (annotations == null) {
      return '-';
    }
    return prettyPrintDouble(annotations!.ref.missing.toDouble());
  }
}

class SourcePlayerIndex {
  int sourceIndex;
  int playerIndex;
  SourcePlayerIndex(this.sourceIndex, this.playerIndex);
}

class ThorMetadataState {
  Pointer<ThorMetadata> thorMetadata;
  Map<String, SourcePlayerIndex> playerStringToIndex;

  ThorMetadataState() : thorMetadata = ffiEngine.MainGetThorMetadata(GlobalState.ffiMain), playerStringToIndex = {} {
    var playerToSources = <String, List<SourcePlayerIndex>>{};
    for (int i = 0; i < thorMetadata.ref.num_sources; ++i) {
      ThorSourceMetadata source = thorMetadata.ref.sources.elementAt(i).value.ref;
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
          var playerString = '$player / ${thorMetadata.ref.sources[index.sourceIndex].ref.name.cast<Utf8>().toDartString()}';
          playerStringToIndex[playerString] = index;
        }
      }
    }
  }

  void setBlack(List<String> elements) {
    List<int> nextIndex = List.generate(thorMetadata.ref.num_sources, (index) => 0);

    for (var element in elements) {
      var index = playerStringToIndex[element]!;
      var sourceIndex = index.sourceIndex;
      thorMetadata.ref.sources[sourceIndex].ref.selected_blacks[nextIndex[sourceIndex]++] = index.playerIndex;
    }

    for (int i = 0; i < thorMetadata.ref.num_sources; ++i) {
      thorMetadata.ref.sources[i].ref.selected_blacks[nextIndex[i]] = -1;
    }
  }
  void setWhite(List<String> elements) {
    List<int> nextIndex = List.generate(thorMetadata.ref.num_sources, (index) => 0);

    for (var element in elements) {
      var index = playerStringToIndex[element]!;
      var sourceIndex = index.sourceIndex;
      thorMetadata.ref.sources[sourceIndex].ref.selected_whites[nextIndex[sourceIndex]++] = index.playerIndex;
    }

    for (int i = 0; i < thorMetadata.ref.num_sources; ++i) {
      thorMetadata.ref.sources[i].ref.selected_whites[nextIndex[i]] = -1;
    }
  }
}