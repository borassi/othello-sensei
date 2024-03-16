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

import 'package:connectivity_plus/connectivity_plus.dart';
import 'package:flutter/material.dart';
import 'package:googleapis/drive/v3.dart';
import 'package:logger/logger.dart';
import 'package:othello_sensei/widgets_spacers/margins.dart';
import 'package:path/path.dart';
import 'dart:io' as io;
import 'package:http/http.dart' as http;
import 'package:googleapis_auth/googleapis_auth.dart';
import 'package:tar/tar.dart';
import 'package:worker_manager/worker_manager.dart';

import '../env.dart';
import '../files.dart';
import '../main.dart';

const kBookFolderId = '1_-NOJ_QRjoHiVlvvX4IAKivWREvrxjQe';

class DriveDownloaderProgress with ChangeNotifier {
  double progress;

  DriveDownloaderProgress() : progress = 0;

  void setProgress(double progress) {
    this.progress = progress;
    notifyListeners();
  }

  double getProgress() { return progress; }
}

class DriveDownloader {
  final DriveApi _driveApi;
  late String? message;
  late bool? downloaded;
  DriveDownloaderProgress downloadProgress;
  DriveDownloaderProgress unzipProgress;

  DriveDownloader._create(this._driveApi) : downloadProgress = DriveDownloaderProgress(), unzipProgress = DriveDownloaderProgress();

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
      downloadProgress.setProgress(downloadedSize / totalSize);
    });
    var outputFile = io.File(output);
    await outputFile.writeAsBytes(dataStore);
    downloadProgress.setProgress(1);
  }

  Future<void> extractTarGz(String inputPath, String outputPath, int numFilesForProgressBar) async {
    final input = io.File(inputPath).openRead().transform(gzip.decoder);
    var done = 0;
    await TarReader.forEach(input, (TarEntry entry) async {
      done += 1;
      unzipProgress.setProgress(done / 258);
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

  void downloadBookWithConfirmation(BuildContext context, String alertText) {
    showDialog<void>(
      context: context,
      barrierDismissible: false, // user must tap button!
      builder: (BuildContext childContext) {
        return AppTheme(
          child: AlertDialog(
            title: const Text('Downloading book'),
            content: Text(alertText, style: Theme.of(childContext).textTheme.bodyMedium!),
            actions: <Widget>[
              TextButton(
                child: const Text('OK'),
                onPressed: () {
                  Navigator.of(childContext).pop();
                  downloadBookNoConfirmation(context).catchError((e, s) {
                    String error = 'Error when downloading the book\n$e\n$s';
                    Logger().e(error);
                  });
                },
              ),
              TextButton(
                child: const Text('Abort'),
                onPressed: () {
                  Navigator.of(childContext).pop();
                },
              ),
            ],
          )
        );
      }
    );
  }

  void downloadBook(BuildContext context) async {
    final connectivityResult = await (Connectivity().checkConnectivity());
    switch (connectivityResult) {
      case ConnectivityResult.bluetooth:
      case ConnectivityResult.wifi:
      case ConnectivityResult.ethernet:
        downloadBookNoConfirmation(context);
      case ConnectivityResult.vpn:
      case ConnectivityResult.mobile:
      case ConnectivityResult.other:
        downloadBookWithConfirmation(
            context,
            'The book is large (approximately 200MB).\n'
            'Make sure you are connected to a WiFi, or \n'
            'you are OK downloading it on your data plan.'
        );
      case ConnectivityResult.none:
        showDialog<void>(
          context: context,
          builder: (BuildContext childContext) {
            return AppTheme(
              child: AlertDialog(
                title: const Text('No internet connection available'),
              )
            );
          }
        );
    }
  }

  Future<void> downloadBookNoConfirmation(BuildContext context) async {
    downloadProgress.setProgress(0);
    unzipProgress.setProgress(0);
    Navigator.push(
      context,
      MaterialPageRoute(
        builder: (context) => PopScope(
          canPop: false,
          child: AppTheme(
            child: Scaffold(
              appBar: AppBar(
                automaticallyImplyLeading: false,
                title: const Text('Downloading book'),
                backgroundColor: Theme.of(context).colorScheme.primaryContainer,
                foregroundColor: Theme.of(context).colorScheme.onPrimaryContainer,
              ),
              body: RowWithMargins(
                children: [
                  const Spacer(),
                  ListenableBuilder(
                    listenable: Listenable.merge([downloadProgress, unzipProgress]),
                    builder: (BuildContext context, Widget? w) {
                      double progress;
                      String text;
                      if (downloadProgress.getProgress() == 0) {
                        text = "Starting";
                        progress = 0;
                      } else if (downloadProgress.getProgress() < 1) {
                        text = "Downloading";
                        progress = downloadProgress.getProgress();
                      } else {
                        text = "Unzipping";
                        progress = unzipProgress.getProgress();
                      }
                      return ColumnWithMargins(
                        children: [
                          const Spacer(),
                          Text(text, style: Theme.of(context).textTheme.bodyMedium!),
                          CircularProgressIndicator(
                            value: progress,
                            semanticsLabel: 'Circular progress indicator',
                          ),
                          const Spacer(),
                        ]
                      );
                    }
                  ),
                  const Spacer(),
                ]
              )
            )
          )
        )
      )
    );
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
    // We expect 258 files in the book, and AFAIK there is no way to get this
    // number without decompressing the file. We hard-code it, since it's just
    // used to show the progress bar.
    await extractTarGz(tempBookCompressedPath, localTempPathVar, 258);
    io.Directory(bookPath).renameSync(bookPathBeforeDelete);
    io.Directory(tempBookPath).renameSync(bookPath);
    io.Directory(localTempPathVar).deleteSync(recursive: true);
    Navigator.pop(context);
  }
}