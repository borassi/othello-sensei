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
import 'dart:convert';
import 'dart:io';

import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:path/path.dart';
import 'package:path_provider/path_provider.dart';

const String assetVersion = "3";

Future<String> localPath() async {
  WidgetsFlutterBinding.ensureInitialized();
  final directory = await getApplicationSupportDirectory();
  return directory.path;
}

Future<String> localAssetPath() async {
  return join(await localPath(), "assets");
}

Future<String> versionFilePath() async {
  return join(await localAssetPath(), "version.txt");
}

Future<void> copyAssetsToLocalPath() async {
  final currentAssetPath = await localPath();
  final manifestContent = await rootBundle.loadString('AssetManifest.json');
  final Map<String, dynamic> manifestMap = json.decode(manifestContent);
  var directory = Directory(await localAssetPath());
  if (directory.existsSync()) {
    directory.deleteSync(recursive: true);
  }
  for (var path in manifestMap.keys) {
    var destinationPath = join(currentAssetPath, path);
    var destinationDirName = dirname(destinationPath);
    Directory(destinationDirName).createSync(recursive: true);
    final encryptedByteData = await rootBundle.load(path);
    File(destinationPath).writeAsBytesSync(encryptedByteData.buffer.asUint8List());
  }
  File(await versionFilePath()).writeAsString(assetVersion);
}

Future<void> maybeCopyAssetsToLocalPath() async {
  String storedAssetVersion = '';
  try {
    storedAssetVersion = File(await versionFilePath()).readAsStringSync();
  } on PathNotFoundException {}
  if (assetVersion != storedAssetVersion) {
    await copyAssetsToLocalPath();
  }
}