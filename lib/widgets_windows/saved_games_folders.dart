/*
 * Copyright (c) 2025 Michele Borassi
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

import 'dart:ffi';

import 'package:ffi/ffi.dart';
import 'package:file_picker/file_picker.dart';
import 'package:flutter/material.dart';
import 'package:othello_sensei/widgets_utils/misc.dart';
import 'package:othello_sensei/widgets_windows/secondary_window.dart';
import 'package:othello_sensei/widgets_windows/sensei_dialog.dart';

import '../state.dart';
import '../widgets_spacers/app_sizes.dart';

class SquareHeightWidget extends StatelessWidget {
  final Widget child;

  const SquareHeightWidget({required this.child, super.key});

  @override
  Widget build(BuildContext context) {
    var size = Theme.of(context).extension<AppSizes>()!.squareSize;
    return SizedBox(height: size, width: size, child: child);
  }
}

class _HalfMargin extends StatelessWidget {
  const _HalfMargin();

  @override
  Widget build(BuildContext context) {
    var margin = Theme.of(context).extension<AppSizes>()!.margin / 2;
    return SizedBox(height: margin, width: margin);
  }
}
class _GameFolderRow extends StatelessWidget {
  final String item;
  final void Function() onDelete;

  const _GameFolderRow({required this.item, required this.onDelete});

  @override
  Widget build(BuildContext context) {
    var margin = Theme.of(context).extension<AppSizes>()!.margin / 2;
    return Container(
      padding: EdgeInsetsGeometry.directional(start: 0, end: 0, top: margin, bottom: margin),
      child: Container(
        color: Theme.of(context).colorScheme.primaryContainer,
        padding: EdgeInsetsGeometry.directional(start: margin, end: 0, top: 0, bottom: 0),
        child: Row(
      // crossAxisAlignment: CrossAxisAlignment.stretch,
          children: [
            Expanded(child: Align(alignment: Alignment.centerLeft, child: Text(item, style: Theme.of(context).textTheme.bodyMedium))),
            SquareHeightWidget(child: SenseiButton(icon: Icons.delete, onPressed: onDelete)),
          ]
        )
      )
    );
  }

}

void setFileSources(List<String> folders) {
  Pointer<Pointer<Char>> foldersC = malloc(folders.length);
  for (int i = 0; i < folders.length; ++i) {
    String folder = folders[i];
    foldersC[i] = folder.toNativeUtf8().cast<Char>();
  }
  GlobalState.thorMetadata.invalidate();
  GlobalState.ffiEngine.SetFileSources(GlobalState.ffiMain, folders.length, foldersC);
  for (int i = 0; i < folders.length; ++i) {
    malloc.free(foldersC[i]);
  }
  malloc.free(foldersC);
}

class SavedGamesFoldersWindow extends StatefulWidget {
  const SavedGamesFoldersWindow({super.key});

  @override
  State<StatefulWidget> createState() { return _SavedGamesFoldersWindowState(); }
}

class _SavedGamesFoldersWindowState extends State<SavedGamesFoldersWindow> {
  final List<String> _folders;

  _SavedGamesFoldersWindowState() : _folders = GlobalState.thorMetadata.gameFolders;

  void _addItem() async {
    if (_folders.length >= 20) {
      await showSenseiDialog(
        SenseiDialog(
          title: 'You can add at most 20 archive folders',
          content: (
              'You can always add a "parent" folder, and create as many '
              'subfolders as you want inside it.'
          )
        )
      );
      return;
    }
    var filePickerResult = await FilePicker.platform.getDirectoryPath();
    if (filePickerResult == null || filePickerResult.isEmpty) {
      return;
    }
    if (_folders.contains(filePickerResult)) {
      await showSenseiDialog(
          SenseiDialog(
              content: 'Folder $filePickerResult is already an archive folder.'
          )
      );
      return;
    }
    setState(() { _folders.add(filePickerResult); });
  }

  void _removeItem(int index) {
    setState(() { _folders.removeAt(index); });
  }

  void _onReorder(int oldIndex, int newIndex) {
    setState(() {
      if (newIndex > oldIndex) {
        newIndex -= 1;
      }
      final String item = _folders.removeAt(oldIndex);
      _folders.insert(newIndex, item);
    });
  }

  @override
  Widget build(BuildContext context) {
    return SecondaryWindow(
        onPopInvoked: (bool didPop) {
          setFileSources(_folders);
          GlobalState.evaluate();
        },
        title: 'Saved games folders',
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.end,
          children: [
            SquareHeightWidget(
              child: SenseiButton( // Here's the new add button
                onPressed: _addItem,
                icon: Icons.add,
              ),
            ),
            const _HalfMargin(),
            Expanded(
              child: ReorderableListView.builder(
                buildDefaultDragHandles: false, // <--- Add this
                itemCount: _folders.length,
                itemBuilder: (context, index) {
                  final item = _folders[index];
                  return ReorderableDragStartListener(
                    key: Key(item),
                    index: index,
                    child: _GameFolderRow(item: item, onDelete: () => _removeItem(index)),
                  );
                },
                onReorder: _onReorder,
              ),
            )
        ]
      )
    );
  }
}