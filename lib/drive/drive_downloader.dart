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
import 'package:flutter/gestures.dart';
import 'package:flutter/material.dart';
import 'package:googleapis/drive/v3.dart';
import 'package:othello_sensei/state.dart';
import 'package:othello_sensei/widgets_spacers/margins.dart';
import 'package:othello_sensei/widgets_windows/sensei_dialog.dart';
import 'package:path/path.dart';
import 'dart:io' as io;
import 'package:http/http.dart' as http;
import 'package:googleapis_auth/googleapis_auth.dart';
import 'package:tar/tar.dart';
import 'package:url_launcher/url_launcher_string.dart';

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

enum DialogOptions {
  dialogFailNoConnection,
  dialogShow,
  dialogShowMobileOnly,
  dialogSkip,
}

DialogOptions toDialogOption(ConnectivityResult connectivity) {
  switch (connectivity) {
    case ConnectivityResult.bluetooth:
    case ConnectivityResult.ethernet:
    case ConnectivityResult.vpn:
    case ConnectivityResult.wifi:
      return DialogOptions.dialogSkip;
    case ConnectivityResult.mobile:
      return DialogOptions.dialogShow;
    case ConnectivityResult.other:
      return DialogOptions.dialogShowMobileOnly;
    case ConnectivityResult.none:
      return DialogOptions.dialogFailNoConnection;
  }
}

DialogOptions getDialogOption(List<ConnectivityResult> connectivities) {
  var allDialogOptions = connectivities.map(toDialogOption).toSet();
  for (var result in DialogOptions.values) {
    if (allDialogOptions.contains(result)) {
      return result;
    }
  }
  return DialogOptions.dialogShow;
}

void _downloadMaybeWithConfirmation(BuildContext context, String name, String path, int sizeMB, int numFilesForProgress) {
  switch (getDialogOption(GlobalState.connectivity)) {
    case DialogOptions.dialogFailNoConnection:
      showSenseiDialog(SenseiDialog(title: 'Download failed', content: 'No internet connection available'));
      return;
    case DialogOptions.dialogShow:
      _downloadWithConfirmation(
          context,
          name,
          path,
          sizeMB,
          numFilesForProgress
      );
      return;
    case DialogOptions.dialogShowMobileOnly:
      // If we are not sure about the connection, we ask for confirmation only
      // on mobile. This causes a strange error message if there is no
      // connection.
      // TODO: Avoid missing the connection on Linux, using Snapcraft.
      if (Platform.isLinux || Platform.isMacOS || Platform.isWindows) {
        _download(context, name, path, sizeMB, numFilesForProgress);
      } else {
        _downloadWithConfirmation(context, name, path, sizeMB, numFilesForProgress);
      }
      return;
    case DialogOptions.dialogSkip:
      _download(context, name, path, sizeMB, numFilesForProgress);
      return;
  }
}

void _download(BuildContext context, String name, String path, int sizeMB, int numFilesForProgress) {
  var window = DownloadSecondaryWindow(name, path, sizeMB, numFilesForProgress);
  Navigator.push(context, MaterialPageRoute(builder: (context) => window));
}

void _downloadWithConfirmation(BuildContext context, String name, String path, int sizeMB, int numFilesForProgress) {
  showDialog(
    context: context,
    barrierDismissible: false, // user must tap button!
    builder: (BuildContext childContext) => SenseiDialog(
      title: 'Downloading $name',
      content: (
          'The $name is large (approximately ${sizeMB}MB).\n'
              'Make sure you are connected to a WiFi, or \n'
              'you are OK downloading it on your data plan.'),
      actions: [
        (
          text: 'Download',
          onPressed: (ctx) {
            Navigator.of(ctx).pop();
            _download(context, name, path, sizeMB, numFilesForProgress);
          }
        ),
        (
          text: 'Abort',
          onPressed: (ctx) { Navigator.of(ctx).pop(); }
        ),
      ]
    )
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

class WindowsHelpText extends StatelessWidget {
  const WindowsHelpText({super.key});
  static const windowsHelpUrl = (
      'https://support.microsoft.com/en-us/windows/add-an-exclusion-to-windows'
      '-security-811816c0-4dfd-af4a-47e4-c301afe13b26'
  );

  @override
  Widget build(BuildContext context) {
    return SelectableText.rich(
      TextSpan(
        children: <TextSpan>[
          const TextSpan(
            text: (
                'If the download takes more than a few minutes, it might\n'
                'be because of Windows Defender checks.\n'
                '\n'
                'Possible solutions:\n'
                ' - Wait (it should take at most 1 hour).\n'
                ' - Add a Windows Defender exclusion:\n'
                '     1. Follow these '
            ),
          ),
          TextSpan(
            text: 'instructions',
            style: TextStyle(
              fontSize: Theme.of(context).textTheme.bodySmall!.fontSize,
              color: Colors.blue,
              decoration: TextDecoration.underline,
            ),
            recognizer: TapGestureRecognizer()
              ..onTap = () => launchUrlString(windowsHelpUrl),
          ),
          const TextSpan(
            text: (
                ' (section Virus & threat protection\n'
                '        settings).\n'
                '     2. After clicking "Add an exclusion", select "Process".\n'
                '     3. Enter process name "othello_sensei.exe".\n'
                '     4. The download will become much faster (no need to restart it).\n'
                ' - Download the latest book or archive from this Drive '),
          ),
          TextSpan(
            text: 'folder',
            style: TextStyle(
              fontSize: Theme.of(context).textTheme.bodySmall!.fontSize,
              color: Colors.blue,
              decoration: TextDecoration.underline,
            ),
            recognizer: TapGestureRecognizer()
              ..onTap = () => launchUrlString('https://drive.google.com/drive/folders/1V9GKU4X30l2oppfC3dG80qFh4PHb6sUY?usp=drive_link'),
          ),
          TextSpan(
              text: (
                  ':\n'
                  '     1. Open the link and enter the "Books" or "Archive" subfolder.\n'
                  '     2. Double click on "latest.tar.gz".\n'
                  '     3. Click "Download".\n'
                  '     4. Open the downloaded file and the folder\n'
                  '        ${localAssetPath()}\n'
                  '        (some folders in this path might be hidden).\n'
                  '     5. Close Sensei.\n'
                  '     6. Copy "book" or "archive" from the former to the latter folder.\n'
                  '     7. Reopen Sensei. You should see the new book or archive.'
              ),
          ),
        ],
      ),
      style: Theme.of(context).textTheme.bodySmall!
    );
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
    var localTempPathVar = localTempPath();
    var localAssetPathVar = localAssetPath();
    var errorReceiver = ReceivePort();
    await Isolate.spawn(
      DriveDownloaderWorker.start,
      [resultPort.sendPort, name, path, localTempPathVar, numFilesForProgressBar],
      errorsAreFatal: true,
      onError: errorReceiver.sendPort,
    );
    errorReceiver.listen((dynamic message) async {
      io.Directory(localTempPathVar).deleteSync(recursive: true);
      Navigator.pop(popContext!);
      // Hack: for some reason the message is returned as a string.
      if (!message[0].toString().contains('StoppedDownloadError')) {
        showSenseiDialog(SenseiDialog(title: 'Error when downloading $name', content: message.toString()));
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
      } else if (message is (String, String) && message.$1 == 'Done') {
        var targetPath = join(localAssetPathVar, name);
        var oldTargetPathBeforeDelete = join(localTempPathVar, '${name}_old');
        await GlobalState.resetMain(() {
          var toMove = io.Directory(targetPath);
          if (toMove.existsSync()) {
            toMove.renameSync(oldTargetPathBeforeDelete);
          }
          io.Directory(message.$2).renameSync(targetPath);
          io.Directory(localTempPathVar).deleteSync(recursive: true);
        });
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
                            var windowsHelpTextWidgets = <Widget>[];
                            if (downloadProgress.getProgress() == 0) {
                              text = "Starting";
                              progress = 0;
                            } else if (downloadProgress.getProgress() < 1) {
                              text = "Downloading";
                              progress = downloadProgress.getProgress();
                              if (Platform.isWindows) {
                                windowsHelpTextWidgets = <Widget>[
                                  const WindowsHelpText(),
                                  const Spacer()
                                ];
                              }
                            } else {
                              text = "Unzipping";
                              progress = downloadProgress.getProgress() - 1;
                            }
                            return Column(
                                children: [
                                  const Spacer(),
                                  Text(text, style: Theme.of(context).textTheme.bodyMedium!),
                                  const Margin.side(),
                                  CircularProgressIndicator(value: progress),
                                  const Spacer(),
                                ] + windowsHelpTextWidgets
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
    int numFilesForProgressBar = args[4];
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
    io.Directory(localTempPathVar).createSync(recursive: true);

    var driveId = await getIdFromPath(path);
    await downloadFile(driveId, tempCompressedPath);
    await extractTarGz(tempCompressedPath, localTempPathVar, numFilesForProgressBar);

    sendPort!.send(('Done', tempUncompressedPath));
    receivePort.close();
  }
}