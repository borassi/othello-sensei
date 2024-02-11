/*
 * Copyright 2023 Michele Borassi
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
 */

#include "main.h"
#include "bindings.h"

Main::Main(
    const std::string& evals_filepath,
    const std::string& book_filepath,
    const std::string& thor_filepath,
    SetBoard set_board,
    UpdateAnnotations update_annotations) :
    set_board_(set_board),
    engine_(evals_filepath, book_filepath, thor_filepath, update_annotations) {
  NewGame();
}

void Main::Stop() { engine_.Stop(); }

bool IncludeAllSources(ThorMetadata thor_metadata) {
  for (int i = 0; i < thor_metadata.num_sources; ++i) {
    const ThorSourceMetadata& source = *thor_metadata.sources[i];
    if (source.selected_blacks[0] >= 0 || source.selected_whites[0] >= 0 || source.selected_tournaments[0] >= 0) {
      return false;
    }
  }
  return true;
}

void Main::BoardChanged() {
  const State& state = states_[current_state_];
  const Board& board = state.GetBoard();
  set_board_({board.Player(), board.Opponent(), state.GetBlackTurn()});
}