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
    const std::string& xot_small_filepath,
    const std::string& xot_large_filepath,
    SetBoard set_board,
    UpdateAnnotations update_annotations,
    UpdateTimers update_timers) :
    set_board_(set_board),
    update_timers_(update_timers),
    is_being_destroyed_(false),
    current_state_(nullptr),
    last_state_flutter_(nullptr),
    analyzing_(0),
    engine_(evals_filepath, book_filepath, thor_filepath, update_annotations),
    xot_state_(XOT_STATE_AUTOMATIC),
    xot_small_(LoadTextFile(xot_small_filepath)),
    xot_large_(LoadTextFile(xot_large_filepath)) {
  evaluate_params_.sensei_action = SENSEI_EVALUATES;
  srand(time(nullptr));
  PrintSupportedFeatures();
  update_timers_future_ = std::async(std::launch::async, &Main::RunUpdateTimersThread, this);
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
  SenseiAction action = evaluate_params_.sensei_action;
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

bool Main::ToState(EvaluationState* new_state) {
  if (!new_state) {
    return false;
  }
  Stop();
  ToStateNoStop(new_state);
  return true;
}

void Main::ToStateNoStop(EvaluationState* new_state) {
  assert(new_state);
  assert(new_state->IsLandable(evaluate_params_.sensei_action) ||
         new_state->MustPlay(evaluate_params_.sensei_action));
  if (current_state_) {
    current_state_->AddSecondsOnThisNode(time_on_this_position_.Get());
  }
  current_state_ = new_state;
  current_state_->SetPlayed();
  current_state_->SetNextStates();
  time_on_this_position_ = ElapsedTime();
  RunSetBoard();
}

namespace {

bool AreParamsCompatible(EvaluateParams params1, EvaluateParams params2) {
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