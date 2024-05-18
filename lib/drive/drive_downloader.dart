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
import 'dart:math';

import 'package:connectivity_plus/connectivity_plus.dart';
import 'package:flutter/material.dart';
import 'package:googleapis/drive/v3.dart';
import 'package:logger/logger.dart';
import 'package:othello_sensei/state.dart';
import 'package:othello_sensei/widgets_spacers/margins.dart';
import 'package:path/path.dart';
import 'dart:io' as io;
import 'package:http/http.dart' as http;
import 'package:googleapis_auth/googleapis_auth.dart';
import 'package:tar/tar.dart';

import '../env.dart';
import '../files.dart';
import '../main.dart';

const kBaseFolderId = '1V9GKU4X30l2oppfC3dG80qFh4PHb6sUY';

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

  Future<String> getIdFromPath(String path) async {
    var folderId = kBaseFolderId;
    for (var pathSegment in split(path)) {
      var found = false;
      for (var file in (await listFiles(folderId)).files!) {
        if (file.name == pathSegment) {
          folderId = file.shortcutDetails?.targetId ?? file.id!;
          found = true;
          break;
        }
      }
      if (!found) {
        throw ArgumentError('Cannot open file $path because $pathSegment does not exist');
      }
    }
    return folderId;
  }

  Future<void> downloadFile(String id, String output) async {
    var metadata = await _driveApi.files.get(id, $fields: "size") as File;
    var totalSize = int.parse(metadata.size!);
    var downloadedSize = 0;
    var media = _driveApi.files.get(id, downloadOptions: DownloadOptions.fullMedia);
    var outputFile = io.File(output);
    await (await media as Media).stream.forEach((data) {
      outputFile.writeAsBytesSync(data, mode: FileMode.append);
      downloadedSize += data.length;
      downloadProgress.setProgress(downloadedSize / totalSize);
    });
    downloadProgress.setProgress(1);
  }

  Future<void> extractTarGz(String inputPath, String outputPath, int numFilesForProgressBar) async {
    final input = io.File(inputPath).openRead().transform(gzip.decoder);
    var done = 0;
    await TarReader.forEach(input, (TarEntry entry) async {
      done += 1;
      // Just to avoid failures if the number of files is not correct.
      unzipProgress.setProgress(min(1, done / numFilesForProgressBar));
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

  void downloadWithConfirmation(BuildContext context, String alertText, String title, String path, int numFilesForProgressBar) {
    showDialog<void>(
      context: context,
      barrierDismissible: false, // user must tap button!
      builder: (BuildContext childContext) {
        return AppTheme(
          child: AlertDialog(
            title: Text('Downloading ${title}'),
            content: Text(alertText, style: Theme.of(childContext).textTheme.bodyMedium!),
            actions: <Widget>[
              TextButton(
                child: const Text('OK'),
                onPressed: () {
                  Navigator.of(childContext).pop();
                  downloadNoConfirmation(context, title, path, numFilesForProgressBar).catchError((e, s) {
                    String error = 'Error in downloading $title\n$e\n$s';
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

  void download(BuildContext context, String name, String path, int sizeMB, int numFilesForProgressBar) async {
    final connectivityResult = await (Connectivity().checkConnectivity());
    switch (connectivityResult) {
      case ConnectivityResult.bluetooth:
      case ConnectivityResult.wifi:
      case ConnectivityResult.ethernet:
        downloadNoConfirmation(context, name, path, numFilesForProgressBar);
      case ConnectivityResult.vpn:
      case ConnectivityResult.mobile:
      case ConnectivityResult.other:
        downloadWithConfirmation(
            context,
            'File $name is large (approximately ${sizeMB}MB).\n'
            'Make sure you are connected to a WiFi, or \n'
            'you are OK downloading it on your data plan.',
            name,
            path,
            numFilesForProgressBar
        );
      case ConnectivityResult.none:
        showDialog<void>(
          context: context,
          builder: (BuildContext childContext) {
            return AppTheme(
              child: const AlertDialog(
                title: Text('No internet connection available'),
              )
            );
          }
        );
    }
  }

  void downloadBook(BuildContext context) {
    download(context, 'book', 'Books/latest.tar.gz', 300, 258);
  }

  void downloadArchive(BuildContext context) {
    download(context, 'archive', 'Archive/latest.tar.gz', 100, 58);
  }

  Future<void> downloadNoConfirmation(BuildContext context, String title, String path, int numFilesForProgressBar) async {
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
                title: Text('Downloading $title'),
                backgroundColor: Theme.of(context).colorScheme.primaryContainer,
                foregroundColor: Theme.of(context).colorScheme.onPrimaryContainer,
              ),
              body: Row(
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
                      return Column(
                        children: [
                          const Spacer(),
                          Text(text, style: Theme.of(context).textTheme.bodyMedium!),
                          const Margin(),
                          CircularProgressIndicator(
                            value: progress,
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
    var tempCompressedPath = join(localTempPathVar, '${title}.tar.gz');
    var tempUncompressedPath = join(localTempPathVar, title);
    var targetPath = join(await localAssetPath(), title);
    var oldTargetPathBeforeDelete = join(localTempPathVar, '${title}_old');
    io.Directory(localTempPathVar).createSync(recursive: true);

    var driveId = await getIdFromPath(path);
    await downloadFile(driveId, tempCompressedPath);

    await extractTarGz(tempCompressedPath, localTempPathVar, numFilesForProgressBar);
    var toMove = io.Directory(targetPath);
    if (toMove.existsSync()) {
      toMove.renameSync(oldTargetPathBeforeDelete);
    }
    io.Directory(tempUncompressedPath).renameSync(targetPath);
    io.Directory(localTempPathVar).deleteSync(recursive: true);
    GlobalState.resetMain();
    Navigator.pop(context);
  }
}