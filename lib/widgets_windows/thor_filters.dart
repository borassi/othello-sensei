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
import 'package:othello_sensei/widgets_windows/secondary_window.dart';

import '../state.dart';
import '../widgets_board/case.dart';
import '../widgets_spacers/margins.dart';
import '../widgets_utils/misc.dart';

class ThorSourcesWidget extends StatelessWidget {
  const ThorSourcesWidget({super.key});

  @override
  Widget build(BuildContext context) {
    return ListenableBuilder(
      listenable: GlobalState.thorMetadata,
      builder: (BuildContext context, Widget? widget) {
        var checkboxes = <Widget>[];
        for (var iter in GlobalState.thorMetadata.sourceToActive.entries) {
          checkboxes.add(
            Row(
              children: [
                SenseiToggle(
                  initialValue: iter.value,
                  onChanged: (bool? newValue) {
                    if (newValue != null) {
                      GlobalState.thorMetadata.setSelectedSource(iter.key, newValue);
                    }
                  },
                ),
                const Margin.internal(),
                Text(iter.key, style: Theme.of(context).textTheme.bodyMedium!),
              ]
            )
          );
        }
        return Column(children: checkboxes);
      }
    );
  }
}

class ThorFiltersWidget extends StatelessWidget {
  final double squareSize;

  const ThorFiltersWidget(this.squareSize, {super.key});

  Widget playerSearch(BuildContext context, bool black) {
    var fontSize = Theme.of(context).textTheme.bodyMedium!.fontSize!;
    var players = GlobalState.thorMetadata.playerStringToIndex.keys.toList();
    players.sort();
    return ListenableBuilder(
      listenable: GlobalState.thorMetadata,
      builder: (BuildContext context, Widget? child) => DropdownSearch<String>.multiSelection(
        items: (filter, infiniteScrollProps) => players,
        selectedItems: black ? GlobalState.thorMetadata.selectedBlacks : GlobalState.thorMetadata.selectedWhites,
        popupProps: PopupPropsMultiSelection.menu(
          showSearchBox: true,
          fit: FlexFit.tight,
          searchFieldProps: TextFieldProps(style: TextStyle(fontSize: fontSize)),
          constraints: BoxConstraints(maxHeight: 8 * squareSize),
          itemBuilder: (BuildContext context, String s, bool x, bool y) => Text(s, style: TextStyle(fontSize: fontSize)),
          searchDelay: const Duration(seconds: 0),
        ),
        dropdownBuilder: (context, selectedItems) {
          return Column(
            children: selectedItems.map((String item) {
              return Align(
                alignment: Alignment.centerLeft,
                child: TextButton(
                  child: Text(item, style: TextStyle(fontSize: fontSize)),
                  onPressed: () {
                    selectedItems.remove(item);
                    if (black) {
                      GlobalState.thorMetadata.setSelectedBlacks(selectedItems);
                    } else {
                      GlobalState.thorMetadata.setSelectedWhites(selectedItems);
                    }
                  }),
              );
            }).toList()
          );
        },
        onChanged: (List<String> elements) {
          if (black) {
            GlobalState.thorMetadata.setSelectedBlacks(elements);
          } else {
            GlobalState.thorMetadata.setSelectedWhites(elements);
          }
        },
      )
    );
  }
  @override
  Widget build(BuildContext context) {
    return SecondaryWindow(
      onPopInvoked: (bool didPop) {
        GlobalState.thorMetadata.setFilters();
        GlobalState.evaluate();
      },
      title: 'Archive filters',
      child: LayoutBuilder(
        builder: (context, constraint) {
          return SingleChildScrollView(
            child: ConstrainedBox(
            constraints: BoxConstraints(minHeight: constraint.maxHeight),
            child: IntrinsicHeight(
        child: Column(
          children: [
            const ThorSourcesWidget(),
            const Margin.internal(),
            Row(
              children: [
                Case(CaseState.black, 255, () => {}, () => {}, false),
                const Margin.internal(),
                Expanded(child: playerSearch(context, true)),
              ]
            ),
            const Spacer(),
            Row(
              children: [
                Case(CaseState.white, 255, () => {}, () => {}, false),
                const Margin.internal(),
                Expanded(child: playerSearch(context, false)),
              ]
            ),
            const Spacer(),
          ],
        )
      )
    ));
  }));}
}