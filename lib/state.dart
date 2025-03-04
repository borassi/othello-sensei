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
import 'dart:math';

import 'package:connectivity_plus/connectivity_plus.dart';
import 'package:ffi/ffi.dart';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:othello_sensei/utils.dart';
import 'package:othello_sensei/widgets_windows/sensei_dialog.dart';
import 'package:path/path.dart';
import 'package:path_provider/path_provider.dart';
import 'package:shared_preferences/shared_preferences.dart';

import 'ffi/ffi_bridge.dart';
import 'ffi/ffi_engine.dart';
import 'files.dart';
import 'main.dart';

enum ActionWhenPlay {
  // playBlack,
  // playWhite,
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
    return -1;
  }
  return 60 - GlobalState.board.emptySquares();
}

void updateAnnotations(int currentThread, bool finished) {
  Pointer<Annotations> startAnnotations = GlobalState.ffiEngine.GetStartAnnotations(GlobalState.ffiMain, currentThread);
  Pointer<Annotations> annotations = GlobalState.ffiEngine.GetCurrentAnnotations(GlobalState.ffiMain, currentThread);
  if (annotations == nullptr || startAnnotations == nullptr ||
      !annotations.ref.valid) {
    GlobalState.globalAnnotations.reset();
    for (int i = 0; i < 64; ++i) {
      GlobalState.annotations[i].clear();
    }
  } else {
    GlobalState.globalAnnotations.setState(annotations, startAnnotations);
    for (Pointer<Annotations> child = annotations.ref.first_child; child != nullptr; child = child.ref.next_sibling) {
      GlobalState.annotations[child.ref.move].setState(child.ref);
    }
  }
  if (!finished) {
    GlobalState.evaluate();
  }
}

Future<void> copy() async {
  Pointer<Char> sequence = GlobalState.ffiEngine.GetSequence(GlobalState.ffiMain);
  var s = sequence.cast<Utf8>().toDartString();
  Clipboard.setData(ClipboardData(text: s));
  malloc.free(sequence);
}
Future<void> paste() async {
  var game = (await Clipboard.getData(Clipboard.kTextPlain))?.text;
  setGameOrError(game, "Analyze on paste");
}

Future<void> setGameOrError(String? game, String preference) async {
  if (game == null) {
    return;
  }
  var gameC = game.toNativeUtf8().cast<Char>();
  var success = GlobalState.ffiEngine.SetSequence(GlobalState.ffiMain, gameC);
  if (!success) {
    await showSenseiDialog(SenseiDialog(title: 'Not a game', content: game));
    return;
  }
  if (GlobalState.preferences.get(preference)) {
    analyze();
  } else {
    GlobalState.evaluate();
  }
}

void resetAnalyzedGame() {
  GlobalState.ffiEngine.ResetAnalyzedGame(GlobalState.ffiMain);
  GlobalState.evaluate();
}

void analyze() {
  GlobalState.preferences.fillEvaluateParams();
  GlobalState.preferences.set('Active tab', 0);
  GlobalState.actionWhenPlay.setActionWhenPlay(ActionWhenPlay.eval);
  GlobalState.ffiEngine.Analyze(GlobalState.ffiMain);
}

class GlobalState {
  static var board = BoardState();
  static var actionWhenPlay = ActionWhenPlayState();
  static List<AnnotationState> annotations = List.generate(64, (index) => AnnotationState());
  static var globalAnnotations = GlobalAnnotationState();
  static late final PreferencesState preferences;
  static late Pointer<Void> ffiMain;
  static const Main main = Main();
  static late List<ConnectivityResult> connectivity;
  static ThorMetadataState? thorMetadataOrNull;
  static late FFIEngine ffiEngine;
  static late CpuType cpuType;
  static late String localPath;
  static final navigatorKey = GlobalKey<NavigatorState>();

  static Future<void> init() async {
    final directory = await getApplicationSupportDirectory();
    localPath = directory.path;
    await maybeCopyAssetsToLocalPath();
    var ffiEngineCpuType = getDynamicLibrary();
    ffiEngine = FFIEngine(ffiEngineCpuType.$1);
    cpuType = ffiEngineCpuType.$2;
    var connectivityHandler = Connectivity();
    try {
      connectivity = await connectivityHandler.checkConnectivity();
      connectivityHandler.onConnectivityChanged.forEach((var result) { connectivity = result; });
    } on Exception catch(e) {
      print(
          'WARNING: cannot get connectivity. This does not prevent the app '
          'from running, but it causes less readable error messages when '
          'downloading book and archive. Error:\n$e');
      connectivity = [ConnectivityResult.other];
    }
    preferences = await PreferencesState.create();
    await _createMain();
  }

  static Future<void> resetMain(void Function() runWithoutFFIMain) async {
    try {
      GlobalState.ffiEngine.MainDelete(ffiMain);
      runWithoutFFIMain();
    } finally {
      await _createMain();
    }
  }

  static Future<void> _createMain() async {
    NativeCallable<SetBoardFunction> setBoardCallback = NativeCallable.listener(setBoard);
    NativeCallable<UpdateAnnotationsFunction> setAnnotationsCallback = NativeCallable.listener(updateAnnotations);
    var localAssetPathVar = localAssetPath();
    globalAnnotations.reset();
    for (int i = 0; i < 64; ++i) {
      GlobalState.annotations[i].clear();
    }
    thorMetadataOrNull = null;
    ffiMain = GlobalState.ffiEngine.MainInit(
        join(localAssetPathVar, 'pattern_evaluator.dat').toNativeUtf8().cast<Char>(),
        join(localAssetPathVar, 'book').toNativeUtf8().cast<Char>(),
        join(localAssetPathVar, 'archive').toNativeUtf8().cast<Char>(),
        join(localAssetPathVar, 'xot/openingssmall.txt').toNativeUtf8().cast<Char>(),
        join(localAssetPathVar, 'xot/openingslarge.txt').toNativeUtf8().cast<Char>(),
        setBoardCallback.nativeFunction,
        setAnnotationsCallback.nativeFunction
    );
    newGame();
  }

  static ThorMetadataState get thorMetadata {
    thorMetadataOrNull ??= ThorMetadataState();
    return thorMetadataOrNull!;
  }

  static void newGameXot(bool small) async {
    GlobalState.ffiEngine.RandomXOT(GlobalState.ffiMain, small);
    evaluate();
  }

  static void newGame() async {
    GlobalState.ffiEngine.NewGame(GlobalState.ffiMain);
    evaluate();
  }

  static void playMove(int i) {
    bool moved = GlobalState.ffiEngine.PlayMove(GlobalState.ffiMain, i);
    if (!moved && GlobalState.preferences.get('Use illegal moves to undo and redo')) {
      if (i % 8 < 4) {
        redo();
      } else {
        undo();
      }
      return;
    }
    evaluate();
  }

  static void redo() {
    GlobalState.ffiEngine.Redo(GlobalState.ffiMain);
    evaluate();
  }

  static void undo() {
    GlobalState.ffiEngine.Undo(GlobalState.ffiMain);
    evaluate();
  }

  static void toAnalyzedGameOrLastChoice() async {
    if (GlobalState.board.emptySquares() == 60) {
      switch (GlobalState.preferences.get('Pressing « from the first position')) {
        case 'Ask':
          if (await showSenseiDialog(SenseiDialog(
              title: 'New game?',
              content: 'Clicking "Yes" loses all analysis data.',
              actions: [
                (text: 'Yes', onPressed: (context) { Navigator.pop(context, true); }),
                (text: 'No', onPressed: (context) { Navigator.pop(context, false); }),
              ])) ?? false) {
            GlobalState.newGame();
          }
          break;
        case 'New game':
          GlobalState.newGame();
          break;
        default:
          break;
      }
    } else {
      GlobalState.ffiEngine.ToAnalyzedGameOrLastChoice(GlobalState.ffiMain);
    }
    evaluate();
  }

  static void setCurrentMove(int currentMove) {
    GlobalState.ffiEngine.SetCurrentMove(GlobalState.ffiMain, currentMove);
    evaluate();
  }

  static void stop() {
    GlobalState.ffiEngine.Stop(GlobalState.ffiMain);
  }

  static void evaluate() {
    if (GlobalState.actionWhenPlay.actionWhenPlay != ActionWhenPlay.eval) {
      GlobalState.globalAnnotations.reset();
      return;
    }
    GlobalState.preferences.fillEvaluateParams();
    GlobalState.ffiEngine.Evaluate(GlobalState.ffiMain);
  }
}

class BoardState with ChangeNotifier {
  int player;
  int opponent;
  bool blackTurn;
  int lastMove;
  bool xot;

  BoardState() : player = 0, opponent = 0, blackTurn = true, lastMove = -1, xot = false;

  void setState(BoardUpdate boardUpdate) {
    player = boardUpdate.player;
    opponent = boardUpdate.opponent;
    blackTurn = boardUpdate.black_turn;
    lastMove = boardUpdate.last_move;
    xot = boardUpdate.xot;
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
    this.annotations = annotations.valid ? annotations : null;
    notifyListeners();
  }

  double getEval(bool inError) {
    return getEvalFromAnnotations(annotations!, annotations!.father.ref.black_turn, inError);
  }
}

class InvalidPreferenceException implements Exception {
  String cause;
  InvalidPreferenceException(this.cause);
}

class PreferencesState with ChangeNotifier {
  final Map<String, dynamic> defaultPreferences = {
    'Controls position': 'App bar',
    'Margin size': 'Small',
    'Last move marker': 'None',
    'Black and white bars in the graph': false,
    'Show extra data in evaluate mode': true,
    'Evaluate if watching an analyzed game': false,
    'Back button action': 'Undo',
    'Number of threads': Platform.numberOfProcessors,
    'Positions when evaluating': 100000000000,
    // 'Positions when playing': 50000000,
    'Seconds until first evaluation': 0.1,
    'Seconds between evaluations': 1.0,
    'Spend half time on positions worse by': 6.0,
    'Round evaluations': 'Never',
    'Use book': true,
    'Seconds/position in game analysis': 1.0,
    'Analyze on paste': true,
    'Analyze on import': true,
    'Approximate': false,
    'Highlight distance from best move': 1.0,
    'Best move green, other yellow': false,
    'Lower': -63,
    'Upper': 63,
    'Active tab': 0,
    'Highlight next move in analysis': true,
    'Highlight next moves outside analysis': true,
    'Show unsupported CPU at startup': true,
    'Use illegal moves to undo and redo': false,
    'Use disk count to undo and redo': true,
    'Pressing « from the first position': 'Ask',
  };
  static const Map<String, List<String>> preferencesValues = {
    'Last move marker': ['None', 'Dot', 'Number'],
    'Back button action': ['Undo', 'Close app'],
    'Round evaluations': ['Never', 'Only errors', 'Always'],
    'Controls position': ['App bar', 'Side bar'],
    'Margin size': ['None', 'Small', 'Large', 'Coordin'],
    'Pressing « from the first position': ['Do nothing', 'Ask', 'New game'],
  };
  late final SharedPreferences _preferences;

  PreferencesState._initialize(this._preferences) {
    for (String name in _preferences.getKeys()) {
      try {
        _checkPreferenceNameAndValue(name, _preferences.get(name));
      } on InvalidPreferenceException {
        if (defaultPreferences.containsKey(name)) {
          set(name, defaultPreferences[name]);
        } else {
          _preferences.remove(name);
        }
      }
    }
  }

  static Future<PreferencesState> create() async {
    var preferences = await SharedPreferences.getInstance();
    return PreferencesState._initialize(preferences);
  }

  dynamic get(String name) {
    var value = _preferences.get(name) ?? defaultPreferences[name];
    _checkPreferenceNameAndValue(name, value);
    return value;
  }

  void fillEvaluateParams() {
    var params = GlobalState.ffiEngine.MainGetEvaluateParams(GlobalState.ffiMain).ref;
    params.lower = get('Lower');
    params.upper = get('Upper');
    params.max_positions = get('Positions when evaluating');
    params.max_time_first_eval = get('Seconds until first evaluation');
    params.max_time_next_evals = get('Seconds between evaluations');
    params.max_time_analysis = get('Seconds/position in game analysis');
    params.delta = get('Spend half time on positions worse by');
    params.n_threads = get('Number of threads');
    params.approx = get('Approximate');
    params.use_book = get('Use book');
    params.reevaluate_during_analysis = get('Evaluate if watching an analyzed game');
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

  Future<void> reset() async {
    await setAll(defaultPreferences);
  }

  Future<void> setNoUpdate(String name, dynamic value) async {
    _checkPreferenceNameAndValue(name, value);
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
        throw InvalidPreferenceException('Invalid preference type ${value.runtimeType} for value $value');
    }
  }

  void _checkPreferenceName(String name) {
    if (!defaultPreferences.containsKey(name)) {
      throw InvalidPreferenceException('Invalid preference name $name');
    }
  }

  void _checkPreferenceNameAndValue(String name, dynamic value) {
    _checkPreferenceName(name);
    var validValues = preferencesValues[name];
    if (!(validValues?.contains(value) ?? true)) {
      throw InvalidPreferenceException('Invalid value $value for preference $name. Valid values: $validValues');
    }
    if (value.runtimeType != defaultPreferences[name].runtimeType) {
      throw InvalidPreferenceException('Invalid type ${value.runtimeType} for preference $name. Expected: ${defaultPreferences[name].runtimeType}');
    }
  }
}

class EvaluationStats {
  final int nVisited;
  final int nVisitedBook;
  final double seconds;

  EvaluationStats(this.nVisited, this.nVisitedBook, this.seconds);
}

class GlobalAnnotationState with ChangeNotifier {
  Pointer<Annotations>? annotations;
  Pointer<Annotations>? startAnnotations;
  // We cannot use the father eval to highlight because it might not be the max
  // of the child evals.
  double bestEval;

  GlobalAnnotationState() :
      annotations = null,
      startAnnotations = null,
      bestEval = -66;

  void reset() {
    annotations = null;
    startAnnotations = null;
    bestEval = -66;
    notifyListeners();
  }

  void setState(Pointer<Annotations> annotations, Pointer<Annotations> startAnnotations) {
    assert(annotations.ref.valid);
    this.annotations = annotations;
    this.startAnnotations = startAnnotations;
    bestEval = -66;
    for (var child = annotations.ref.first_child;
         child != nullptr;
         child = child.ref.next_sibling) {
      bestEval = max(bestEval, -getEvalFromAnnotations(
          child.ref,
          child.ref.black_turn, false));
    }
    notifyListeners();
  }

  bool existsAnalyzedGame() {
    return (
        startAnnotations != null &&
        startAnnotations!.ref.next_state_in_analysis != nullptr
    );
  }

  (List<double>, int) getAllScoresAndLastMove() {
    var scores = <double>[];
    var currentMoveVar = currentMove();
    int lastMove = -1;
    if (startAnnotations == null) {
      return (scores, lastMove);
    }
    if (!existsAnalyzedGame()) {
      lastMove = currentMoveVar;
    }
    var depth = 0;
    for (var annotation = startAnnotations;
         annotation != null && annotation != nullptr;
         annotation = annotation.ref.next_state_in_analysis != nullptr ?
                          annotation.ref.next_state_in_analysis :
                          annotation.ref.next_state_played) {
      if (annotation.ref.move == GlobalState.ffiEngine.PassMove()) {
        continue;
      }
      scores.add(getEvalFromAnnotations(annotation.ref, true, true, bestLine: true));
      if (lastMove == -1 && (
          annotation.ref.next_state_in_analysis == nullptr
          || annotation.ref.next_state_in_analysis != annotation.ref.next_state_played
          || depth == currentMoveVar)) {
        lastMove = depth;
      }
      ++depth;
    }
    return (scores, lastMove);
  }

  (double, double, bool) getErrors() {
    var errorBlack = 0.0;
    var errorWhite = 0.0;
    var (allScores, lastMove) = getAllScoresAndLastMove();
    var start = GlobalState.board.xot ? 8 : 0;
    int lastMoveForScores =
        GlobalState.globalAnnotations.annotations?.ref.during_analysis ?? false ?
        allScores.length : lastMove + 1;
    var hasNaN = lastMoveForScores == 0;
    var oldScore = allScores.elementAtOrNull(start) ?? double.nan;
    if (oldScore.isNaN) {
      oldScore = 0;
    }
    for (int i = start + 1; i < lastMoveForScores; ++i) {
      double score = allScores[i];
      var error = score - oldScore;
      if (error.isNaN) {
        hasNaN = true;
      } else if (error > 0) {
        errorWhite += error;
        oldScore = score;
      } else {
        errorBlack += -error;
        oldScore = score;
      }
    }
    return (errorBlack, errorWhite, hasNaN);
  }

   EvaluationStats getEvaluationStats() {
    if (annotations == null) {
      return EvaluationStats(0, 0, 0);
    }
    var positions = 0;
    var positionsBook = 0;
    var seconds = 0.0;
    for (var child = annotations!.ref.first_child; child != nullptr; child = child.ref.next_sibling) {
      if (!child.ref.derived) {
        positions += child.ref.descendants;
        positionsBook += child.ref.descendants_book;
        seconds += child.ref.seconds;
      }
    }
    return EvaluationStats(positions, positionsBook, seconds);
  }
}

class SourcePlayerIndex {
  int sourceIndex;
  int playerIndex;
  SourcePlayerIndex(this.sourceIndex, this.playerIndex);
}

class ThorMetadataState with ChangeNotifier {
  Map<String, bool> sourceToActive;
  Pointer<ThorMetadata> thorMetadata;
  Map<String, SourcePlayerIndex> playerStringToIndex;
  List<String> selectedBlacks;
  List<String> selectedWhites;

  ThorMetadataState() :
        thorMetadata = GlobalState.ffiEngine.MainGetThorMetadata(GlobalState.ffiMain),
        playerStringToIndex = {},
        sourceToActive = {},
        selectedBlacks = [],
        selectedWhites = [] {
    var playerToSources = <String, List<SourcePlayerIndex>>{};
    for (int i = 0; i < thorMetadata.ref.num_sources; ++i) {
      var source = (thorMetadata.ref.sources + i).value.ref;
      sourceToActive[source.name.cast<Utf8>().toDartString()] = true;
      for (int j = 0; j < source.num_players; ++j) {
        String player = (source.players + j).value.cast<Utf8>().toDartString();
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

  void setSelectedBlacks(List<String> value) {
    selectedBlacks = value;
    notifyListeners();
  }

  void setSelectedWhites(List<String> value) {
    selectedWhites = value;
    notifyListeners();
  }

  void setSelectedSource(String name, bool active) {
    sourceToActive[name] = active;
    notifyListeners();
  }

  void setFilters() {
    for (int i = 0; i < thorMetadata.ref.num_sources; ++i) {
      ThorSourceMetadata source = (thorMetadata.ref.sources + i).value.ref;
      source.active = sourceToActive[source.name.cast<Utf8>().toDartString()]!;
    }
    for (var (players, getSource) in [
      (selectedBlacks, (sourceIndex) => thorMetadata.ref.sources[sourceIndex].ref.selected_blacks),
      (selectedWhites, (sourceIndex) => thorMetadata.ref.sources[sourceIndex].ref.selected_whites)]) {

      List<int> nextIndex = List.generate(thorMetadata.ref.num_sources, (index) => 0);

      for (var player in players) {
        var index = playerStringToIndex[player]!;
        var sourceIndex = index.sourceIndex;
        getSource(sourceIndex)[nextIndex[sourceIndex]++] = index.playerIndex;
      }
      for (int i = 0; i < thorMetadata.ref.num_sources; ++i) {
        getSource(i)[nextIndex[i]] = -1;
      }
    }
  }
}