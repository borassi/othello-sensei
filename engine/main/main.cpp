/*
 * Copyright 2023-2024 Michele Borassi
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
    current_state_(nullptr),
    last_state_flutter_(nullptr),
    analyzing_(0),
    engine_(evals_filepath, book_filepath, thor_filepath, *this, update_annotations) {
  NewGame();
}

namespace {
bool operator==(const EvaluateParams& lhs, const EvaluateParams& rhs) {
  return std::forward_as_tuple(
      lhs.lower,
      lhs.upper,
      lhs.approx,
      lhs.use_book,
      lhs.thor_filters.max_games,
      lhs.thor_filters.start_year,
      lhs.thor_filters.end_year
  ) == std::forward_as_tuple(
      rhs.lower,
      rhs.upper,
      rhs.approx,
      rhs.use_book,
      rhs.thor_filters.max_games,
      rhs.thor_filters.start_year,
      rhs.thor_filters.end_year
  );
}

bool operator!=(const EvaluateParams& lhs, const EvaluateParams& rhs) {
  return !(lhs == rhs);
}
}  // namespace

void Main::Evaluate() {
  if (last_params_ != evaluate_params_) {
    last_params_ = evaluate_params_;
    ResetEvaluations();
  }
  if (analyzing_) {
    if (analyzing_ == 2) {
      // This changes analyzing_ to 0, but we set it below anyway.
      Undo();
    }
    if (!current_state_->Father()) {
      analyzing_ = 0;
      engine_.RunUpdateAnnotations();
      return;
    } else {
      analyzing_ = 2;
    }
  }
  engine_.Start(
      analyzing_ ? current_state_->PreviousNonPass() : current_state_,
      first_state_,
      evaluate_params_,
      analyzing_);
}

void Main::Stop() {
  analyzing_ = 0;
  engine_.Stop();
}

void Main::ToState(EvaluationState* new_state) {
  if (!new_state) {
    return;
  }
  Stop();
  ToStateNoStop(new_state);
}

void Main::ToStateNoStop(EvaluationState* new_state) {
  current_state_ = new_state;
  current_state_->SetPlayed();
  current_state_->SetNextStates();
  if (current_state_->IsPass()) {
    ToStateNoStop(current_state_->GetChildren()[0]);
    return;
  }
  const Board& board = current_state_->ToBoard();
  set_board_({board.Player(), board.Opponent(), current_state_->BlackTurn()});
}