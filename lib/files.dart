/*
 * Copyright (c) 2023-2025 Michele Borassi
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
import 'dart:io';

import 'package:flutter/services.dart';
import 'package:othello_sensei/state.dart';
import 'package:path/path.dart';

const String assetVersion = "14";

String localAssetPath() {
  return join(GlobalState.localPath, "assets");
}

String localSavedGamesFoldersPath() {
  return join(GlobalState.localPath, "saved_games_folders.txt");
}

String localTempPath() {
  return join(GlobalState.localPath, "tmp");
}

String versionFilePath() {
  return join(localAssetPath(), "version.txt");
}

Future<void> copyAssetsToLocalPath() async {
  final assetManifest = await AssetManifest.loadFromAssetBundle(rootBundle);
  final assets = assetManifest.listAssets();
  var directory = Directory(localAssetPath());
  if (directory.existsSync()) {
    directory.deleteSync(recursive: true);
  }
  for (var path in assets) {
    // Avoid copying .DS_Store in Mac as it causes a failure.
    if (basename(path).startsWith('.')) {
      continue;
    }
    var destinationPath = join(GlobalState.localPath, path);
    var destinationDirName = dirname(destinationPath);
    Directory(destinationDirName).createSync(recursive: true);
    final encryptedByteData = await rootBundle.load(path);
    File(destinationPath).writeAsBytesSync(encryptedByteData.buffer.asUint8List());
  }
  File(versionFilePath()).writeAsString(assetVersion);
}

Future<void> maybeCopyAssetsToLocalPath() async {
  String storedAssetVersion = '';
  try {
    storedAssetVersion = File(versionFilePath()).readAsStringSync();
  } on PathNotFoundException {}
  if (assetVersion != storedAssetVersion) {
    await copyAssetsToLocalPath();
  }
}