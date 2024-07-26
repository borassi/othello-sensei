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

import 'dart:async';
import 'dart:io';
import 'dart:isolate';
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

void downloadBook(BuildContext context) {
  _downloadMaybeWithConfirmation(context, 'book', 'Books/latest.tar.gz', 400, 258);
}

void downloadArchive(BuildContext context) {
  _downloadMaybeWithConfirmation(context, 'archive', 'Archive/latest.tar.gz', 150, 58);
}

void _downloadMaybeWithConfirmation(BuildContext context, String name, String path, int sizeMB, int numFilesForProgress) {
  switch (GlobalState.connectivity) {
    case ConnectivityResult.bluetooth:
    case ConnectivityResult.ethernet:
    case ConnectivityResult.vpn:
    case ConnectivityResult.wifi:
      _download(context, name, path, sizeMB, numFilesForProgress);
      return;
    case ConnectivityResult.mobile:
    case ConnectivityResult.other:
      _downloadWithConfirmation(
          context,
          name,
          path,
          sizeMB,
          numFilesForProgress
      );
      return;
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
      return;
  }
}

void _download(BuildContext context, String name, String path, int sizeMB, int numFilesForProgress) {
  var window = DownloadSecondaryWindow(name, path, sizeMB, numFilesForProgress);
  Navigator.push(context, MaterialPageRoute(builder: (context) => window));
}

void _downloadWithConfirmation(BuildContext context, String name, String path, int sizeMB, int numFilesForProgress) {
  showDialog<void>(
      context: context,
      barrierDismissible: false, // user must tap button!
      builder: (BuildContext childContext) {
        return AppTheme(
            child: AlertDialog(
              title: Text('Downloading $name'),
              content: Text(
                  'The $name is large (approximately ${sizeMB}MB).\n'
                      'Make sure you are connected to a WiFi, or \n'
                      'you are OK downloading it on your data plan.',
                  style: Theme.of(childContext).textTheme.bodyMedium!),
              actions: <Widget>[
                TextButton(
                  child: const Text('Download'),
                  onPressed: () {
                    Navigator.of(childContext).pop();
                    _download(context, name, path, sizeMB, numFilesForProgress);
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

class DriveDownloaderProgress with ChangeNotifier {
  double progress;

  DriveDownloaderProgress() : progress = 0;

  void setProgress(double progress) {
    this.progress = progress;
    notifyListeners();
  }

  double getProgress() { return progress; }
}

class DownloadFinishedHandler with ChangeNotifier {
  bool finished = false;

  void finish() {
    finished = true;
    notifyListeners();
  }
}

class DownloadSecondaryWindow extends StatelessWidget {
  final String name;
  final String path;
  final int sizeMB;
  final int numFilesForProgressBar;
  final downloadProgress = DriveDownloaderProgress();
  final downloadFinishedHandler = DownloadFinishedHandler();
  BuildContext? popContext;
  SendPort? sendPort;
  bool stopping;

  DownloadSecondaryWindow(this.name, this.path, this.sizeMB, this.numFilesForProgressBar, {super.key}) : stopping = false, sendPort = null;

  Future<void> _startDownload() async {
    final resultPort = ReceivePort();
    var localTempPathVar = await localTempPath();
    var localAssetPathVar = await localAssetPath();
    var errorReceiver = ReceivePort();
    await Isolate.spawn(
      DriveDownloaderWorker.start,
      [resultPort.sendPort, name, path, localTempPathVar, localAssetPathVar, numFilesForProgressBar],
      errorsAreFatal: true,
      onError: errorReceiver.sendPort,
    );
    errorReceiver.listen((dynamic message) async {
      Navigator.pop(popContext!);
      // Hack: for some reason the message is returned as a string.
      if (!message[0].toString().contains('StoppedDownloadError')) {
        showDialog<void>(
            context: popContext!,
            builder: (BuildContext childContext) {
              return AlertDialog(title: Text('Error when downloading $name'), content: Text(message.toString()));
            });
      }
    });
    resultPort.listen((dynamic message) async {
      if (message is SendPort) {
        sendPort = message;
        if (stopping) {
          sendPort!.send('Stop');
        }
      } else if (message is double) {
        downloadProgress.setProgress(message);
      } else if (message == 'Done') {
        GlobalState.resetMain();
        resultPort.close();
        Navigator.pop(popContext!, false);
      } else if (message == 'Stopped') {
        resultPort.close();
        Navigator.pop(popContext!, false);
      }
    });
  }

  @override
  Widget build(BuildContext context) {
    popContext = context;
    _startDownload();
    return PopScope(
        canPop: false,
        onPopInvoked: (bool didPop) {
          if (!didPop) {
            if (sendPort != null) {
              sendPort!.send('Stop');
            } else {
              stopping = true;
            }
          }
        },
        child: AppTheme(
            child: Scaffold(
                appBar: AppBar(
                  automaticallyImplyLeading: true,
                  title: Text('Downloading $name'),
                  backgroundColor: Theme.of(context).colorScheme.primaryContainer,
                  foregroundColor: Theme.of(context).colorScheme.onPrimaryContainer,
                ),
                body: Row(
                    children: [
                      ListenableBuilder(listenable: downloadFinishedHandler, builder: (BuildContext context, Widget? widget) {
                        if (downloadFinishedHandler.finished) {
                          Navigator.pop(context);
                        }
                        return const Spacer();
                      }),
                      ListenableBuilder(
                          listenable: downloadProgress,
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
                              progress = downloadProgress.getProgress() - 1;
                            }
                            return Column(
                                children: [
                                  const Spacer(),
                                  Text(text, style: Theme.of(context).textTheme.bodyMedium!),
                                  const Margin.side(),
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
    );
  }
}

class StoppedDownloadError extends Error {}

class DriveDownloaderWorker {
  final DriveApi _driveApi;
  late String? message;
  late bool? downloaded;
  SendPort? sendPort;

  DriveDownloaderWorker._create(this._driveApi);

  static Future<void> start(List<dynamic> args) {
    var authKey = Env.driveKey;
    http.Client client = clientViaApiKey(authKey);
    return DriveDownloaderWorker._create(DriveApi(client)).run(args);
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
      sendPort!.send(downloadedSize / totalSize);
    });
    sendPort!.send(1.0);
  }

  Future<void> extractTarGz(String inputPath, String outputPath, int numFilesForProgressBar) async {
    final input = io.File(inputPath).openRead().transform(gzip.decoder);
    var done = 0;
    await TarReader.forEach(input, (TarEntry entry) async {
      done += 1;
      // The sendPort was initialized at the beginning.
      // Just to avoid failures if the number of files is not correct.
      sendPort!.send(1 + min(1, done / numFilesForProgressBar.toDouble()));
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

  Future<void> run(List<dynamic> args) async {
    sendPort = args[0];
    String name = args[1];
    String path = args[2];
    String localTempPathVar = args[3];
    String localAssetPathVar = args[4];
    int numFilesForProgressBar = args[5];
    var receivePort = ReceivePort();

    sendPort!.send(receivePort.sendPort);
    receivePort.listen((dynamic message) async {
      if (message == 'Stop') {
        throw StoppedDownloadError();
      }
    });

    try {
      io.Directory(localTempPathVar).deleteSync(recursive: true);
    } on PathNotFoundException {}
    var tempCompressedPath = join(localTempPathVar, '$name.tar.gz');
    var tempUncompressedPath = join(localTempPathVar, name);
    var targetPath = join(localAssetPathVar, name);
    var oldTargetPathBeforeDelete = join(localTempPathVar, '${name}_old');
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
    sendPort!.send('Done');
    receivePort.close();
  }
}