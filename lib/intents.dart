/*
 * Copyright (c) 2024-2025 Michele Borassi
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

import 'dart:async';
import 'dart:ffi';
import 'dart:io';

import 'package:android_intent_plus/android_intent.dart';
import 'package:android_intent_plus/flag.dart';
import 'package:ffi/ffi.dart';
import 'package:flutter/services.dart';
import 'package:othello_sensei/state.dart';
import 'package:othello_sensei/widgets_windows/sensei_dialog.dart';
import 'package:path_provider/path_provider.dart';
import 'package:receive_intent/receive_intent.dart';
import 'package:receive_sharing_intent/receive_sharing_intent.dart';
import 'package:uri_content/uri_content.dart';

Future<bool> maybeForwardIntent() async {
  if (!Platform.isAndroid) {
    return false;
  }
  final intent = await ReceiveIntent.getInitialIntent();
  if (intent == null || intent.fromPackageName == null || intent.action != 'android.intent.action.SEND') {
    return false;
  }
  SystemNavigator.pop();
  final AndroidIntent sendIntent = AndroidIntent(
    action: 'com.othellosensei.app.FORWARD_INTENT',
    flags: [Flag.FLAG_ACTIVITY_SINGLE_TOP],
    data: intent.data,
    category: intent.categories?.elementAtOrNull(0),
    arguments: intent.extra,
  );
  await sendIntent.launch();
  return true;
}

Future<String> _resolveToRealFilePath(String uriString) async {
  final uri = Uri.parse(uriString);

  // If it's already a file:// URI, just return the path
  if (uri.scheme == 'file') {
    return uri.toFilePath();
  }

  // If content://, copy it to a temp file
  final bytes = await uri.getContent(); // Reads content into memory
  final tempDir = await getTemporaryDirectory();

  // Create a temp file with a specific extension if needed by C++
  final tempFile = File('${tempDir.path}/temp_import.pgn');
  await tempFile.writeAsBytes(bytes);

  return tempFile.path;
}

void _handleIntentAndroid(Intent? intent) async {
  if (intent == null) {
    return;
  }
  // 1. Check for file URI in data (Open) or STREAM (Share)
  final uriString = intent.data ?? intent.extra?['android.intent.extra.STREAM'];

  if (uriString != null) {
    try {
      var pathC = (await _resolveToRealFilePath(uriString)).toNativeUtf8().cast<Char>();
      GlobalState.ffiEngine.Open(GlobalState.ffiMain, pathC);
      malloc.free(pathC);
      if (GlobalState.preferences.get('Analyze on open')) {
        analyze();
      } else {
        GlobalState.evaluate();
      }
      return;
    } catch (e) {
      showSenseiDialog(SenseiDialog(title: 'Error when opening file $uriString', content: e.toString()));
    }
  }
  var game = intent.extra?['android.intent.extra.TEXT'];
  setGameOrError(game, 'Analyze on import');
}

void _handleIntentIOS(List<SharedMediaFile>? intents) {
  if (intents == null || intents.isEmpty) {
    ReceiveSharingIntent.instance.reset();
    return;
  }
  var game = intents[0].path;
  ReceiveSharingIntent.instance.reset();
  setGameOrError(game, 'Analyze on import');
}

Future<void> handleIntentIOS() async {
  ReceiveSharingIntent.instance.getMediaStream().listen(_handleIntentIOS);
  _handleIntentIOS(await ReceiveSharingIntent.instance.getInitialMedia());
}

Future<void> handleIntentAndroid() async {
  _handleIntentAndroid(await ReceiveIntent.getInitialIntent());
  ReceiveIntent.receivedIntentStream.forEach(_handleIntentAndroid);
}

Future<void> handleIntent() async {
  if (Platform.isAndroid) {
    await handleIntentAndroid();
  } else if (Platform.isIOS) {
    await handleIntentIOS();
  }
}