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

import 'dart:collection';
import 'dart:convert';
import 'dart:ffi';
import 'dart:ffi' as ffi;
import 'dart:io';
import 'dart:math';

import 'package:connectivity_plus/connectivity_plus.dart';
import 'package:ffi/ffi.dart';
import 'package:file_picker/file_picker.dart';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:othello_sensei/utils.dart';
import 'package:othello_sensei/widgets_board/case.dart' as sensei_case;
import 'package:othello_sensei/widgets_windows/save_dialog.dart';
import 'package:othello_sensei/widgets_windows/saved_games_folders.dart';
import 'package:othello_sensei/widgets_windows/sensei_dialog.dart';
import 'package:path/path.dart';
import 'package:path_provider/path_provider.dart';
import 'package:shared_preferences/shared_preferences.dart';

import 'ffi/ffi_bridge.dart';
import 'ffi/ffi_engine.dart';
import 'files.dart';
import 'main.dart';

void setBoard(BoardUpdate boardUpdate) {
  for (int i = 0; i < 64; ++i) {
    GlobalState.annotations[i].clear();
  }
  GlobalState.board.setState(boardUpdate);
  if (boardUpdate.handle_game_over) {
    GlobalState.board.handleGameOver();
  }
}

int currentMove() {
  return GlobalState.globalAnnotations.annotations?.ref.depth_no_pass ?? -1;
}

void updateAnnotations(int currentThread, bool finished, int move) {
  Pointer<Annotations> startAnnotations = GlobalState.ffiEngine.GetStartAnnotations(GlobalState.ffiMain, currentThread);
  Pointer<Annotations> annotations = GlobalState.ffiEngine.GetCurrentAnnotations(GlobalState.ffiMain, currentThread);
  if (annotations == nullptr || startAnnotations == nullptr ||
      !annotations.ref.valid) {
    GlobalState.resetAnnotations();
    return;
  }
  if (GlobalState.preferences.get("Active tab") == 2) {
    GlobalState.resetAnnotations();
  } else {
    GlobalState.globalAnnotations.setState(annotations, startAnnotations);
    for (Pointer<Annotations> child = annotations.ref.first_child; child !=
        nullptr; child = child.ref.next_sibling) {
      GlobalState.annotations[child.ref.move].setState(child.ref);
    }
  }
  if (move != GlobalState.ffiEngine.NoMove()) {
    GlobalState.playMove(move, true);
  } else if (!finished) {
    GlobalState.evaluate();
  }
}

void updateTimers(double secondsBlack, double secondsWhite) {
  GlobalState.timer.setState(secondsBlack, secondsWhite);
}

void stopCountingTime() {
  GlobalState.ffiEngine.SetCountingTime(GlobalState.ffiMain, false);
}

void setCountingTime() {
  var activeTab = GlobalState.preferences.get('Active tab');
  GlobalState.ffiEngine.SetCountingTime(GlobalState.ffiMain, activeTab == 2);
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
  malloc.free(gameC);
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
  GlobalState.preferences.set('Active tab', 0);
  GlobalState.preferences.fillEvaluateParams();
  GlobalState.ffiEngine.Analyze(GlobalState.ffiMain);
}

class GlobalState {
  static var board = BoardState();
  static var setupBoardState = SetupBoardState();
  static var actionWhenPlay = ActionWhenPlayState();
  static List<AnnotationState> annotations = List.generate(64, (index) => AnnotationState());
  static var globalAnnotations = GlobalAnnotationState();
  static late final PreferencesState preferences;
  static late Pointer<Void> ffiMain;
  static const Main main = Main();
  static late List<ConnectivityResult> connectivity;
  static ThorMetadataState thorMetadata = ThorMetadataState();
  static GameMetadataState gameMetadataState = GameMetadataState();
  static late FFIEngine ffiEngine;
  static late CpuType cpuType;
  static late String localPath;
  static final navigatorKey = GlobalKey<NavigatorState>();
  static final TimerState timer = TimerState();

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
    thorMetadata.invalidate();
    NativeCallable<SetBoardFunction> setBoardCallback = NativeCallable.listener(setBoard);
    NativeCallable<UpdateAnnotationsFunction> setAnnotationsCallback = NativeCallable.listener(updateAnnotations);
    NativeCallable<UpdateTimersFunction> updateTimersCallback = NativeCallable.listener(updateTimers);
    var localAssetPathVar = localAssetPath();
    GlobalState.resetAnnotations();
    var evaluatorC = join(localAssetPathVar, 'pattern_evaluator.dat').toNativeUtf8().cast<Char>();
    var bookC = join(localAssetPathVar, 'book').toNativeUtf8().cast<Char>();
    var archiveC = join(localAssetPathVar, 'archive').toNativeUtf8().cast<Char>();
    var localSavedGamesFoldersPathC = localSavedGamesFoldersPath().toNativeUtf8().cast<Char>();
    var xotSmallC = join(localAssetPathVar, 'xot/openingssmall.txt').toNativeUtf8().cast<Char>();
    var xotLargeC = join(localAssetPathVar, 'xot/openingslarge.txt').toNativeUtf8().cast<Char>();
    ffiMain = GlobalState.ffiEngine.MainInit(
        evaluatorC, bookC, archiveC, localSavedGamesFoldersPathC, xotSmallC, xotLargeC,
        setBoardCallback.nativeFunction,
        setAnnotationsCallback.nativeFunction,
        updateTimersCallback.nativeFunction,
    );
    for (var ptr in [evaluatorC, bookC, archiveC, localSavedGamesFoldersPathC,
                     xotSmallC, xotLargeC]) {
      malloc.free(ptr);
    }
    thorMetadata.init();
    evaluate();
  }

  static void newGameXot(bool small) async {
    GlobalState.ffiEngine.RandomXOT(GlobalState.ffiMain, small);
    evaluate();
  }

  static void newGame() async {
    GlobalState.ffiEngine.NewGame(GlobalState.ffiMain);
    evaluate();
  }

  static void playMove(int i, bool automatic) {
    setCountingTime();
    bool moved = GlobalState.ffiEngine.PlayMove(GlobalState.ffiMain, i, automatic);
    if (!moved && !automatic && GlobalState.preferences.get('Use illegal moves to undo and redo')) {
      if (i % 8 < 4) {
        redo();
      } else {
        undo();
      }
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

  static void toLastImportantNode() async {
    bool success = GlobalState.ffiEngine.ToLastImportantNode(GlobalState.ffiMain);
    if (!success) {
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
    }
    evaluate();
  }

  static void setCurrentMove(int depth) {
    GlobalState.ffiEngine.SetCurrentMove(GlobalState.ffiMain, depth);
  }

  static void stop() {
    GlobalState.ffiEngine.Stop(GlobalState.ffiMain);
    stopCountingTime();
  }

  static void evaluate() {
    if (GlobalState.setupBoardState.settingUpBoard) {
      stop();
      GlobalState.resetAnnotations();
      return;
    }
    GlobalState.preferences.fillEvaluateParams();
    GlobalState.ffiEngine.Evaluate(GlobalState.ffiMain);
  }

  static bool isXot() {
    if (ffiMain == nullptr) {
      return false;
    }
    return GlobalState.ffiEngine.IsXot(GlobalState.ffiMain);
  }

  static void resetAnnotations() {
    GlobalState.globalAnnotations.reset();
    for (int i = 0; i < 64; ++i) {
      GlobalState.annotations[i].clear();
    }
  }

  static Future<void> setDetailsAndSave(BuildContext context) async {
    var game = ffiEngine.GetGameToSave(ffiMain);
    if (!game.ref.success) {
      showSenseiDialog(SenseiDialog(
        title: "Failed to save the game",
        content: cArrayToString(game.ref.error),
      ));
    } else {
      await Navigator.push(
        context,
        MaterialPageRoute(builder: (context) => SaveDialogWidget()),
      );
    }
    malloc.free(game);
  }

  static Future<void> save() async {
    var game = ffiEngine.GetGameToSave(ffiMain);
    String? file = await FilePicker.platform.saveFile(
      initialDirectory: GlobalState.thorMetadata.gameFolders.elementAtOrNull(0),
      fileName: GlobalState.gameMetadataState.getGameName(),
      type: FileType.custom,
      allowedExtensions: ['txt'],
      bytes: utf8.encode(cArrayToString(game.ref.game)),
    );
    malloc.free(game);
    if (file != null) {
      var fileC = file.toNativeUtf8().cast<Char>();
      GlobalState.thorMetadata.invalidate();
      bool reloaded = GlobalState.ffiEngine.ReloadSource(GlobalState.ffiMain, fileC);
      GlobalState.thorMetadata.init();
      malloc.free(fileC);
      if (!reloaded &&
          GlobalState.preferences.get('Show dialog on save outside archive')) {
        var folder = getContainingFolder(file);
        var actions = [
          (text: 'OK - do not show again', onPressed: (ctx) {
            GlobalState.preferences.set('Show dialog on save outside archive', false);
            Navigator.pop(ctx);
          }),
          (text: 'OK - show again next time', onPressed: (ctx) { Navigator.pop(ctx); }),
        ];
        if (folder != null && GlobalState.thorMetadata.gameFolders.length < 20) {
          var folderName = basename(folder);
          actions.add((
              text: 'Add folder $folderName to the archive',
              onPressed: (ctx) {
                setFileSources(GlobalState.thorMetadata.gameFolders + [folder]);
                Navigator.pop(ctx);
              }
          ));
        }
        await showSenseiDialog(
            SenseiDialog(
              title: 'Saved game outside the archive folders',
              content: (
                'You can open the game manually, but you will not see it in '
                'the Archive tab. You can always change the archive folders '
                'by clicking ⋮ > Edit archive folders.'),
              actions: actions,
            )
        );
      }
    }
    evaluate();
  }

  static Future<void> open() async {
    var filePickerResult = await FilePicker.platform.pickFiles(
      type: FileType.custom,
      allowedExtensions: ['txt']
    );
    if (filePickerResult == null || filePickerResult.paths.isEmpty || filePickerResult.paths[0] == null) {
      return;
    }
    var pathC = filePickerResult.paths[0]!.toNativeUtf8().cast<ffi.Char>();
    ffiEngine.Open(ffiMain, pathC);
    malloc.free(pathC);
    if (GlobalState.preferences.get('Analyze on open')) {
      analyze();
    } else {
      evaluate();
    }
  }
}

class SetupBoardState with ChangeNotifier {
  bool settingUpBoard;
  sensei_case.CaseState onClick;
  bool resetState;

  SetupBoardState() : settingUpBoard = false, onClick = sensei_case.CaseState.black, resetState = false;

  void setSettingUpBoard(bool newValue) {
    settingUpBoard = newValue;
    notifyListeners();
  }

  void updateOnClick() {
    onClick = sensei_case.CaseState.values[(onClick.index + 1) % sensei_case.CaseState.values.length];
    notifyListeners();
  }

  void setResetState(bool newValue) {
    resetState = newValue;
    notifyListeners();
  }
}

class GameMetadataState with ChangeNotifier {
  GameMetadata getMetadata() {
    return GlobalState.ffiEngine.MutableGameMetadata(GlobalState.ffiMain).ref;
  }

  void assignToPointer(Array<Char> ptr, String string, int length) {
    var lengthValue = min(string.length, length - 1);
    for (int i = 0; i < lengthValue; ++i) {
      ptr[i] = string.codeUnitAt(i);
    }
    ptr[lengthValue] = 0;
  }

  void setBlack(String value) {
    assignToPointer(getMetadata().black, value, 20);
    notifyListeners();
  }

  void setWhite(String value) {
    assignToPointer(getMetadata().white, value, 20);
    notifyListeners();
  }

  void setTournament(String value) {
    assignToPointer(getMetadata().tournament, value, 26);
    notifyListeners();
  }

  void setYear(int year) {
    getMetadata().year = year;
    notifyListeners();
  }

  void setRound(String round) {
    assignToPointer(getMetadata().round, round, 10);
    notifyListeners();
  }

  void setScore(String score, bool black) {
    int scoreInt;
    try {
      scoreInt = int.parse(score);
    } on FormatException {
      return;
    }
    getMetadata().black_disks = black ? scoreInt : 64 - scoreInt;
  }

  void finishedSetScore() {
    notifyListeners();
  }

  String getGameName() {
    var metadata = getMetadata();
    var black = cArrayToString(metadata.black);
    if (black == "") {
      black = "Unknown";
    }
    var white = cArrayToString(metadata.white);
    if (white == "") {
      white = "Unknown";
    }
    var round = cArrayToString(metadata.round);
    if (round != "") {
      round += " - ";
    }
    return '$round$black - $white.sensei.txt';
  }
}

class Move {
  final int square;
  final bool blackPlayer;

  const Move(this.square, this.blackPlayer);
}

class BoardState with ChangeNotifier {
  int player;
  int opponent;
  bool blackTurn;
  int lastMove;
  List<Move> allMoves;

  BoardState() : player = 0, opponent = 0, blackTurn = true, lastMove = -1, allMoves = <Move>[];

  void setState(BoardUpdate boardUpdate) {
    player = boardUpdate.player;
    opponent = boardUpdate.opponent;
    blackTurn = boardUpdate.black_turn;
    lastMove = boardUpdate.last_move;
    allMoves = <Move>[];
    var oldBlack = true;
    for (var annotation = boardUpdate.start_annotations;
         annotation != nullptr;
         annotation = annotation.ref.next_state_primary != nullptr ?
         annotation.ref.next_state_primary :
         annotation.ref.next_state_secondary) {
      if (annotation.ref.move < 0 || annotation.ref.move >= 64) {
        oldBlack = annotation.ref.black_turn;
        continue;
      }
      allMoves.add(Move(annotation.ref.move, oldBlack));
      oldBlack = annotation.ref.black_turn;
    }
    notifyListeners();
  }

  int black() { return blackTurn ? player : opponent; }
  int white() { return blackTurn ? opponent : player; }
  int blackDisks() { return bitCount(black()); }
  int whiteDisks() { return bitCount(white()); }
  int emptySquares() { return bitCount(~(player | opponent)); }

  void handleGameOver() {
    if (GlobalState.preferences.get("Active tab") != 2
        || GlobalState.setupBoardState.settingUpBoard) {
      return;
    }
    var behavior = GlobalState.preferences.get('When the game ends');
    if (behavior == 'Message') {
      var blackDisks = this.blackDisks();
      var whiteDisks = this.whiteDisks();
      if (blackDisks > whiteDisks) {
        blackDisks = 64 - whiteDisks;
      } else if (blackDisks < whiteDisks) {
        whiteDisks = 64 - blackDisks;
      } else {
        blackDisks = 32;
        whiteDisks = 32;
      }
      String content = '';
      if (GlobalState.actionWhenPlay.getSenseiAction() == SenseiAction.SENSEI_PLAYS_WHITE) {
        if (blackDisks > whiteDisks) {
          content = 'Congratulations! You win $blackDisks - $whiteDisks!';
        } else if (blackDisks == whiteDisks) {
          content = 'Draw!';
        } else {
          content = 'You lose $blackDisks - $whiteDisks!';
        }
      } else if (GlobalState.actionWhenPlay.getSenseiAction() == SenseiAction.SENSEI_PLAYS_BLACK) {
        if (blackDisks > whiteDisks) {
          content = 'You lose $blackDisks - $whiteDisks!';
        } else if (blackDisks == whiteDisks) {
          content = 'Draw!';
        } else {
          content = 'Congratulations! You win $blackDisks - $whiteDisks!';
        }
      }
      showSenseiDialog(SenseiDialog(
          title: 'Game over!',
          content: content,
          actions: [
            (text: 'Analyze', onPressed: (context) { Navigator.pop(context, true); analyze(); }),
            (text: 'Do nothing', onPressed: (context) { Navigator.pop(context, false); }),
          ]));
    } else if (behavior == 'Analyze') {
      analyze();
    } else {
      assert(behavior == 'Do nothing');
    }
  }
}

enum Player {
  player,
  sensei,
}

class ActionWhenPlayState with ChangeNotifier {
  ActionWhenPlayState();

  void invertPlayers() {
    var newWhite = GlobalState.preferences.get("Black player");
    var newBlack = GlobalState.preferences.get("White player");
    GlobalState.preferences.set("White player", newWhite);
    GlobalState.preferences.set("Black player", newBlack);
    notifyListeners();
  }

  void rotatePlayer(bool black) {
    var preferenceName = black ? "Black player" : "White player";
    var old = GlobalState.preferences.get(preferenceName);
    GlobalState.preferences.set(preferenceName, Player.values[(old.index + 1) % Player.values.length]);
    notifyListeners();
  }

  SenseiAction getSenseiAction() {
    if (GlobalState.preferences.get('Active tab') != 2) {
      return SenseiAction.SENSEI_EVALUATES;
    }
    var black = GlobalState.preferences.get("Black player");
    var white = GlobalState.preferences.get("White player");
    switch (black) {
      case Player.player:
        switch (white) {
          case Player.player:
            return SenseiAction.SENSEI_INACTIVE;
          case Player.sensei:
            return SenseiAction.SENSEI_PLAYS_WHITE;
        }
      case Player.sensei:
        switch (white) {
          case Player.player:
            return SenseiAction.SENSEI_PLAYS_BLACK;
          case Player.sensei:
            return SenseiAction.SENSEI_PLAYS_BOTH;
        }
    }
    assert(false);
    return SenseiAction.SENSEI_INACTIVE;
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
  static const enumTypeToValues = {Player : Player.values};
  final Map<String, dynamic> defaultPreferences = {
    'Controls position': 'App bar',
    'Margin size': 'Small',
    'Last move marker': 'None',
    'Black and white bars in the graph': false,
    'Show extra data in evaluate mode': true,
    'Evaluate if watching an analyzed game': false,
    'Back button action': 'Undo',
    'Number of threads': Platform.numberOfProcessors,
    'Board on the right in horizontal mode': false,
    'Seconds until first evaluation': 0.1,
    'Seconds between evaluations': 1.0,
    'Seconds/position when playing': 1.0,
    'Approximate game error when playing': 20.0,
    'Maximum error / move when playing': 4.0,
    'Spend half time on positions worse by': 6.0,
    'Round evaluations': 'Never',
    'Use book': true,
    'Seconds/position in game analysis': 1.0,
    'Analyze on paste': true,
    'Analyze on import': true,
    'Analyze on open': true,
    'When the game ends': 'Message',
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
    'Black player': Player.player,
    'White player': Player.player,
    'Show settings dialog at startup': true,
    'Show dialog on save outside archive': !Platform.isAndroid,
  };
  static const Map<String, List<String>> preferencesValues = {
    'Last move marker': ['None', 'Dot', 'Number (S)', 'Number (L)'],
    'Back button action': ['Undo', 'Close app'],
    'Round evaluations': ['Never', 'Only errors', 'Always'],
    'Controls position': ['App bar', 'Side bar'],
    'Margin size': ['None', 'Small', 'Large', 'Coordin'],
    'Pressing « from the first position': ['Do nothing', 'Ask', 'New game'],
    'When the game ends': ['Message', 'Analyze', 'Do nothing'],
  };
  static const Set<String> nonResetPreferences = {
    'Show settings dialog at startup',
    'Show unsupported CPU at startup',
    'Active tab',
  };
  final Map<String, dynamic> beginnerPreferences = {
    'Black and white bars in the graph': true,
    'Show extra data in evaluate mode': false,
    'Approximate game error when playing': 50.0,
    'Maximum error / move when playing': 10.0,
    'Round evaluations': 'Always',
  };
  late final SharedPreferences _preferences;

  PreferencesState._initialize(this._preferences) {
    for (String name in _preferences.getKeys()) {
      if (Platform.isAndroid && name == 'Show dialog on save outside archive') {
        set(name, defaultPreferences[name]);
        continue;
      }
      try {
        _checkPreferenceNameAndValue(name, _preferences.get(name));
      } on InvalidPreferenceException catch (e) {
        print('Invalid stored preference $name.\nError: ${e.cause}');
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
    var defaultValue = defaultPreferences[name];
    var value = _preferences.get(name) ?? defaultValue;
    if (defaultValue is Enum && value is String) {
      var possibleValues = enumTypeToValues[defaultValue.runtimeType];
      value = possibleValues!.firstWhere((e) => e.name == value);
    }
    _checkPreferenceNameAndValue(name, value);
    return value;
  }

  void fillEvaluateParams() {
    var paramsPtr = malloc<EvaluateParams>();
    EvaluateParams params = paramsPtr.ref;
    params.lower = get('Lower');
    params.upper = get('Upper');
    params.max_positions = 100000000000;
    params.max_time_first_eval = get('Seconds until first evaluation');
    params.max_time_next_evals = get('Seconds between evaluations');
    params.max_time_analysis = get('Seconds/position in game analysis');
    params.max_time_play = get('Seconds/position when playing');
    params.error_play = get('Approximate game error when playing');
    params.max_error_move_play = get('Maximum error / move when playing');
    params.delta = get('Spend half time on positions worse by');
    params.n_threads = get('Number of threads');
    params.approx = get('Approximate');
    params.use_book = get('Use book');
    params.reevaluate_during_analysis = get('Evaluate if watching an analyzed game');
    params.thor_filters.max_games = 100;
    params.thor_filters.start_year = 1000;
    params.thor_filters.end_year = 3000;
    params.sensei_actionAsInt = GlobalState.actionWhenPlay.getSenseiAction().value;

    GlobalState.ffiEngine.SetEvaluateParams(GlobalState.ffiMain, paramsPtr);
    malloc.free(paramsPtr);
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

  Future<void> reset(bool beginner) async {
    var newPreferences = Map.of(defaultPreferences);
    if (beginner) {
      newPreferences.addAll(beginnerPreferences);
    }
    for (var preference in nonResetPreferences) {
      newPreferences.remove(preference);
    }
    await setAll(newPreferences);
  }

  Future<void> setNoUpdate(String name, dynamic value) async {
    _checkPreferenceNameAndValue(name, value);
    if (value == null) {
      return;
    }
    if (defaultPreferences[name] is Enum) {
      await _preferences.setString(name, (value as Enum).name);
    } else if (value is double) {
      await _preferences.setDouble(name, value);
    } else if (value is String) {
      await _preferences.setString(name, value);
    } else if (value is int) {
      await _preferences.setInt(name, value);
    } else if (value is bool) {
      await _preferences.setBool(name, value);
    } else if (value is List<String>) {
      await _preferences.setStringList(name, value);
    } else {
      throw InvalidPreferenceException(
          'Invalid preference type ${value.runtimeType} for value $value');
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
    if (value.runtimeType != defaultPreferences[name].runtimeType &&
        !(value is String && (defaultPreferences[name] is Enum))) {
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
        startAnnotations!.ref.next_state_primary != nullptr
    );
  }

  (List<double>, int) getAllScoresAndLastMove() {
    var scores = <double>[];
    if (startAnnotations == null) {
      return (scores, -1);
    }
    var moveBackToGame = 1000;
    var lastMove = 0;
    bool primary = startAnnotations!.ref.next_state_primary != nullptr;
    for (var annotation = startAnnotations!;
         annotation != nullptr;
         annotation = primary ? annotation.ref.next_state_primary : annotation.ref.next_state_secondary) {
      if ([GlobalState.ffiEngine.PassMove(), GlobalState.ffiEngine.SetupBoardMove()].contains(annotation.ref.move)) {
        continue;
      }
      scores.add(getEvalFromAnnotations(annotation.ref, true, true, bestLine: true));
      lastMove = annotation.ref.depth_no_pass;
      // During analysis, moveBackToGame is the last move in the main line.
      if (primary &&
          annotation.ref.next_state_primary != nullptr &&
          annotation.ref.next_state_primary != annotation.ref.next_state_secondary) {
        moveBackToGame = lastMove;
      }
    }
    return (scores, min(currentMove(), min(moveBackToGame, lastMove)));
  }

  (double, double, bool) getErrors() {
    var errorBlack = 0.0;
    var errorWhite = 0.0;
    var (allScores, lastMove) = getAllScoresAndLastMove();
    var start = GlobalState.isXot() ? 8 : 0;
    int lastMoveForScores =
        GlobalState.globalAnnotations.annotations?.ref.during_analysis ?? false ?
        allScores.length : min(allScores.length, lastMove + 1);
    var hasNaN = false;
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
  String activeFolder;
  List<String> folders;
  List<String> gameFolders;
  Pointer<ThorMetadata>? ptr;
  Map<String, SourcePlayerIndex> playerStringToIndex;
  List<String> players;
  List<String> tournaments;
  List<String> selectedBlacks;
  List<String> selectedWhites;

  ThorMetadataState() :
        ptr = null,
        playerStringToIndex = {},
        players = [],
        tournaments = [],
        folders = [],
        gameFolders = [],
        selectedBlacks = [],
        selectedWhites = [],
        activeFolder = 'All';

  void init() async {
    ptr = GlobalState.ffiEngine.MainGetThorMetadata(GlobalState.ffiMain);
    var playerToSources = <String, List<SourcePlayerIndex>>{};
    var tournamentsSet = HashSet<String>();
    for (int i = 0; i < ptr!.ref.num_sources; ++i) {
      var source = (ptr!.ref.sources + i).value.ref;
      folders.add(source.name.cast<Utf8>().toDartString());
      if (source.is_saved_games_folder) {
        gameFolders.add(cStringToString(source.folder));
      }
      for (int j = 0; j < source.num_players; ++j) {
        String player = cStringToString((source.players + j).value);
        if (!playerToSources.containsKey(player)) {
          playerToSources[player] = [];
        }
        playerToSources[player]!.add(SourcePlayerIndex(i, j));
      }
      for (int j = 0; j < source.num_tournaments; ++j) {
        String player = cStringToString((source.players + j).value);
        if (!playerToSources.containsKey(player)) {
          playerToSources[player] = [];
        }
        tournamentsSet.add(cStringToString(source.tournaments[j]));
      }
    }
    tournaments = tournamentsSet.toList();
    tournaments.sort();

    for (var playerSource in playerToSources.entries) {
      var player = playerSource.key;
      var indices = playerSource.value;
      players.add(player);
      if (indices.length == 1) {
        playerStringToIndex[player] = indices[0];
      } else {
        for (var index in indices) {
          var playerString = '$player / ${ptr!.ref.sources[index.sourceIndex].ref.name.cast<Utf8>().toDartString()}';
          playerStringToIndex[playerString] = index;
        }
      }
    }
    _setFilters();
    notifyListeners();
  }

  void invalidate() {
    ptr = null;
    players = [];
    playerStringToIndex = {};
    tournaments = [];
    folders = [];
    gameFolders = [];
    selectedBlacks = [];
    selectedWhites = [];
    activeFolder = 'All';
    notifyListeners();
  }

  void setSelectedBlacks(List<String> value) {
    selectedBlacks = value;
    notifyListeners();
    _setFilters();
  }

  void setSelectedWhites(List<String> value) {
    selectedWhites = value;
    notifyListeners();
    _setFilters();
  }

  void setActiveFolder(String name) {
    activeFolder = name;
    notifyListeners();
    _setFilters();
  }

  void _setFilters() {
    if (ptr == null) {
      return;
    }
    for (int i = 0; i < ptr!.ref.num_sources; ++i) {
      ThorSourceMetadata source = (ptr!.ref.sources + i).value.ref;
      source.active = (activeFolder == source.name.cast<Utf8>().toDartString()) || activeFolder == 'All';
    }
    for (var (players, getSource) in [
      (selectedBlacks, (sourceIndex) => ptr!.ref.sources[sourceIndex].ref.selected_blacks),
      (selectedWhites, (sourceIndex) => ptr!.ref.sources[sourceIndex].ref.selected_whites)]) {

      List<int> nextIndex = List.generate(ptr!.ref.num_sources, (index) => 0);

      for (var player in players) {
        var index = playerStringToIndex[player]!;
        var sourceIndex = index.sourceIndex;
        getSource(sourceIndex)[nextIndex[sourceIndex]++] = index.playerIndex;
      }
      for (int i = 0; i < ptr!.ref.num_sources; ++i) {
        getSource(i)[nextIndex[i]] = -1;
      }
    }
  }
}

class TimerState with ChangeNotifier {
  int secondsBlack;
  int secondsWhite;

  TimerState() : secondsBlack = 0, secondsWhite = 0;

  int getSeconds(bool black) {
    return black ? secondsBlack : secondsWhite;
  }

  String getString(bool black) {
    int totalSeconds = getSeconds(black);
    int minutes = (totalSeconds / 60).floor();
    int seconds = totalSeconds % 60;

    if (totalSeconds > 3600) {
      return ">1h";
    } else {
      return '${minutes.toString().padLeft(2, '0')}:${seconds.toString().padLeft(2, '0')}';
    }
  }

  void setState(double secondsBlack, double secondsWhite) {
    int newSecondsBlack = secondsBlack.floor();
    int newSecondsWhite = secondsWhite.floor();

    if (newSecondsBlack == secondsBlack && newSecondsWhite == secondsWhite) {
      return;
    }
    this.secondsBlack = newSecondsBlack;
    this.secondsWhite = newSecondsWhite;
    notifyListeners();
  }
}