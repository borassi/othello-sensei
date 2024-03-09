/*
 * Copyright (c) 2024. Michele Borassi
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

import 'package:flutter/material.dart';
import 'package:googleapis/drive/v3.dart';
import 'package:path/path.dart';
import 'dart:io' as io;
import 'package:http/http.dart' as http;
import 'package:googleapis_auth/googleapis_auth.dart';
import 'package:tar/tar.dart';

import '../env.dart';
import '../files.dart';

const kBookFolderId = '1_-NOJ_QRjoHiVlvvX4IAKivWREvrxjQe';

class DriveDownloader {
  final DriveApi _driveApi;
  late String? message;
  late bool? downloaded;

  DriveDownloader._create(this._driveApi);

  static Future<DriveDownloader> create() async {
    var authKey = Env.driveKey;
    http.Client client = clientViaApiKey(authKey);
    return DriveDownloader._create(DriveApi(client));
  }

  Future<FileList> listFiles(String id) async {
    return await _driveApi.files.list(q: "'$id' in parents", $fields: "files(*)");
  }

  Future<void> download(String id, String output) async {
    var metadata = await _driveApi.files.get(id, $fields: "size") as File;
    var totalSize = int.parse(metadata.size!);
    var downloadedSize = 0;
    var media = _driveApi.files.get(id, downloadOptions: DownloadOptions.fullMedia);
    List<int> dataStore = [];
    await (await media as Media).stream.forEach((data) {
      dataStore.insertAll(dataStore.length, data);
      downloadedSize += data.length;
    });
    var outputFile = io.File(output);
    await outputFile.writeAsBytes(dataStore);
  }

  Future<void> extractTarGz(String inputPath, String outputPath) async {
    final input = io.File(inputPath).openRead().transform(gzip.decoder);
    await TarReader.forEach(input, (TarEntry entry) async {
      final destination =
          // NOTE: If `entry.name` contained `../`, this may escape the target
          // directory (but the file should be safe, as I'm the only owner).
          joinAll([outputPath, ...posix.split(entry.name)]);
      if (entry.header.typeFlag == TypeFlag.reg) {
        final f = io.File(destination);
        await f.create(recursive: true);
        await entry.contents.pipe(f.openWrite());
      }
    });
  }

  Future<void> downloadBook(BuildContext context) async {
    Navigator.push(
      context,
      MaterialPageRoute(builder: (context) => PopScope(
        canPop: false,
        child: Scaffold(
          appBar: AppBar(
            automaticallyImplyLeading: false,
            title: const Text('Downloading book'),
            backgroundColor: Theme.of(context).colorScheme.primaryContainer,
            foregroundColor: Theme.of(context).colorScheme.onPrimaryContainer,
          ),
          body: const Text(''),
        )
      )
    ));
    var localTempPathVar = await localTempPath();
    try {
      io.Directory(localTempPathVar).deleteSync(recursive: true);
    } on PathNotFoundException {}
    var tempBookCompressedPath = join(localTempPathVar, 'book.tar.gz');
    var tempBookPath = join(localTempPathVar, 'book');
    var bookPath = join(await localAssetPath(), 'book');
    var bookPathBeforeDelete = join(localTempPathVar, 'book_old');

    io.Directory(localTempPathVar).createSync(recursive: true);
    for (var file in (await listFiles(kBookFolderId)).files!) {
      if (file.name == 'latest.tar.gz') {
        await download(file.shortcutDetails!.targetId!, tempBookCompressedPath);
        break;
      }
    }
    await extractTarGz(tempBookCompressedPath, localTempPathVar);
    io.Directory(bookPath).renameSync(bookPathBeforeDelete);
    io.Directory(tempBookPath).renameSync(bookPath);
    io.Directory(localTempPathVar).deleteSync(recursive: true);
    Navigator.push(
      context,
      MaterialPageRoute(builder: (context) => PopScope(
        canPop: false,
        child: Scaffold(
          appBar: AppBar(
            automaticallyImplyLeading: false,
            title: const Text('Downloaded book'),
            backgroundColor: Theme.of(context).colorScheme.primaryContainer,
            foregroundColor: Theme.of(context).colorScheme.onPrimaryContainer,
          ),
          body: const Text('Please reset the app.'),
        )
      )
    ));
  }
}