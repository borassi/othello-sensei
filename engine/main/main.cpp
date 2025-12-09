/*
 * Copyright 2023-2025 Michele Borassi
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
#include "../utils/misc.h"

Main::Main(
    const std::string& evals_filepath,
    const std::string& book_filepath,
    const std::string& thor_filepath,
    const std::string& saved_games_filepath,
    const std::string& xot_small_filepath,
    const std::string& xot_large_filepath,
    SetBoard set_board,
    UpdateAnnotations update_annotations,
    SetThorMetadata set_thor_metadata,
    UpdateTimers update_timers,
    SendMessage send_message) :
    set_board_(set_board),
    update_timers_(update_timers),
    last_state_flutter_(nullptr),
    current_state_(nullptr),
    xot_small_(LoadTextFile(xot_small_filepath)),
    xot_large_(LoadTextFile(xot_large_filepath)),
    xot_state_(XOT_STATE_AUTOMATIC),
    engine_(evals_filepath, book_filepath, thor_filepath, saved_games_filepath, update_annotations, set_thor_metadata, send_message),
    analyzing_(0),
    time_on_this_position_(std::nullopt),
    is_being_destroyed_(false) {
  evaluate_params_.sensei_action = SENSEI_INVALID_ACTION;
  srand((int) time(nullptr));
  PrintSupportedFeatures();
  update_timers_future_ = std::async(std::launch::async, &Main::RunUpdateTimersThread, this);
  NewGame();
}

void Main::Evaluate() {
  SenseiAction action = evaluate_params_.sensei_action;
  if (analyzing_) {
    if (analyzing_ == 2) {
      // This changes analyzing_ to 0, but we set it below anyway.
      Undo();
    }
    if (!current_state_->PreviousLandable(action)) {
      analyzing_ = 0;
      engine_.RunUpdateAnnotations();
      return;
    } else {
      analyzing_ = 2;
    }
  }
  assert(current_state_->IsLandable(action) || current_state_->MustPlay(action));
  engine_.Start(
      analyzing_ ? current_state_->PreviousLandable(action) : current_state_,
      first_state_,
      evaluate_params_,
      analyzing_);
}

void Main::Stop() {
  analyzing_ = 0;
  engine_.Stop();
}

bool Main::ToState(EvaluationState* new_state, bool handle_game_over) {
  if (!new_state) {
    return false;
  }
  Stop();
  ToStateNoStop(new_state, handle_game_over);
  return true;
}

void Main::SetCountingTimeNoLock(bool value) {
  if (current_state_ && time_on_this_position_) {
    current_state_->AddSecondsOnThisNode(time_on_this_position_->Get());
  }
  if (value) {
    time_on_this_position_ = ElapsedTime();
  } else {
    time_on_this_position_ = std::nullopt;
  }
}

void Main::ToStateNoStop(EvaluationState* new_state, bool handle_game_over) {
  std::lock_guard<std::mutex> guard(time_on_this_position_mutex_);
  assert(new_state);
  assert((evaluate_params_.sensei_action == SENSEI_INVALID_ACTION &&
          current_state_ == nullptr) ||
         new_state->IsLandable(evaluate_params_.sensei_action) ||
         new_state->MustPlay(evaluate_params_.sensei_action));
  SetCountingTimeNoLock(time_on_this_position_ != std::nullopt);
  current_state_ = new_state;
  current_state_->SetPlayed();
  current_state_->SetNextStates();
  int score = -64;
  if (IsGameOver(current_state_->ToBoard())) {
    score = GetEvaluationGameOver(current_state_->ToBoard().Player(), current_state_->ToBoard().Opponent());
  }
  int player_disks = (score + 64) / 2;
  game_metadata_.black_disks = current_state_->BlackTurn() ? player_disks : 64 - player_disks;
  RunSetBoard(handle_game_over);
}

namespace {

bool AreParamsCompatible(EvaluateParams params1, EvaluateParams params2) {
  // To avoid a Valgrind error at startup.
  if (params1.sensei_action == SENSEI_INVALID_ACTION || params2.sensei_action == SENSEI_INVALID_ACTION) {
    return false;
  }
  return
      params1.lower == params2.lower &&
      params1.upper == params2.upper &&
      // Still compatible if times are different.
      params1.error_play == params2.error_play &&
      params1.n_threads == params2.n_threads &&
      params1.delta == params2.delta &&
      params1.approx == params2.approx &&
      params1.use_book == params2.use_book &&
      // Still compatible if reevaluate_during_analysis are different.
      params1.thor_filters.start_year == params2.thor_filters.start_year &&
      params1.thor_filters.end_year == params2.thor_filters.end_year &&
      params1.thor_filters.max_games == params2.thor_filters.max_games;
      // Still compatible if Sensei action changes.
}
}  // namespace

void Main::SetEvaluateParams(const EvaluateParams& params) {
  if (!AreParamsCompatible(evaluate_params_, params)) {
    first_state_->InvalidateRecursive();
  }
  evaluate_params_ = params;
}