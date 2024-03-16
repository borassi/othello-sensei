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

import 'package:dropdown_search/dropdown_search.dart';
import 'package:flutter/material.dart';

import '../state.dart';
import '../widgets_board/case.dart';
import '../widgets_spacers/margins.dart';
import '../main.dart';



class ThorFiltersWidget extends StatelessWidget {
  double squareSize;

  ThorFiltersWidget(this.squareSize, {super.key});

  Widget playerSearch(BuildContext context, bool black) {
    var fontSize = Theme.of(context).textTheme.bodyMedium!.fontSize!;
    var players = GlobalState.thorMetadata.playerStringToIndex.keys.toList();
    players.sort();
    return Expanded(
      child: DropdownSearch<String>.multiSelection(
        items: players,
        popupProps: PopupPropsMultiSelection.menu(
          showSearchBox: true,
          fit: FlexFit.loose,
          searchFieldProps: TextFieldProps(
            style: TextStyle(fontSize: fontSize)
          ),
          itemBuilder: (context, branch, val) {
            return DropdownMenuItem(
              child: Text(branch, style: TextStyle(fontSize: fontSize)),
            );
          },
        ),
        clearButtonProps: ClearButtonProps(
          icon: Icon(Icons.clear, size: fontSize),
          isVisible: true
        ),
        dropdownButtonProps: DropdownButtonProps(
          icon: Icon(Icons.arrow_drop_down, size: fontSize),
          isVisible: true
        ),
        onChanged: (List<String> elements) {
          if (black) {
            GlobalState.thorMetadata.setBlack(elements);
          } else {
            GlobalState.thorMetadata.setWhite(elements);
          }
        },
      )
    );
  }
  @override
  Widget build(BuildContext context) {
    GlobalState.thorMetadata.setBlack([]);
    GlobalState.thorMetadata.setWhite([]);
    return PopScope(
      onPopInvoked: (bool didPop) {
        if (didPop) {
          GlobalState.evaluate();
        }
      },
      child: Scaffold(
        appBar: AppBar(
          title: const Text('Thor filters'),
          backgroundColor: Theme.of(context).colorScheme.primaryContainer,
          foregroundColor: Theme.of(context).colorScheme.onPrimaryContainer,
        ),
        body: AppTheme(
          child: SingleChildScrollView(
            child: Column(
              children: [
                const Margin(),
                Row(
                  children: [
                    const Margin(),
                    Case(CaseState.black, 255, () => {}, () => {}),
                    const Margin(),
                    playerSearch(context, true),
                    const Margin(),
                  ]
                ),
                const Margin(),
                Row(
                  children: [
                    const Margin(),
                    Case(CaseState.white, 255, () => {}, () => {}),
                    const Margin(),
                    playerSearch(context, false),
                    const Margin(),
                  ]
                ),
                const Margin(),
              ],
            )
          )
        )
      )
    );
  }
}