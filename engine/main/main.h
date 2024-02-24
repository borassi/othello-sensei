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

#ifndef OTHELLO_SENSEI_MAIN_H
#define OTHELLO_SENSEI_MAIN_H

#include <list>

#include "bindings.h"
#include "engine.h"
#include "state.h"

#include "../board/board.h"
#include "../board/get_flip.h"
#include "../board/get_moves.h"
#include "../book/book.h"
#include "../evaluatederivative/evaluator_derivative.h"
#include "../evaluatederivative/tree_node.h"
#include "../evaluatedepthone/pattern_evaluator.h"
#include "../thor/thor.h"
#include "../utils/misc.h"

class Main {
 public:
  Main(
      const std::string& evals_filepath,
      const std::string& book_filepath,
      const std::string& thor_filepath,
      SetBoard set_board,
      UpdateAnnotations update_annotations);

  void NewGame() {
    Stop();
    first_state_ = std::make_shared<State>(kStartingPositionMove, Board(), true, 0);
    ToState(first_state_.get());
  }

  void PlayMove(Square square) {
    ToState(current_state_->NextState(square));
  }

  // We need an int to do SetCurrentMove(-1).
  void SetCurrentMove(int current_move) {
    ToState(current_state_->ToDepth(current_move));
  }

  void Redo() {
    SetCurrentMove(current_state_->Depth() + 1);
  }

  void Undo() {
    SetCurrentMove(current_state_->Depth() - 1);
  }

  void Stop();

  void Evaluate() {
    engine_.Start(current_state_, first_state_, evaluate_params_);
  }

  ThorMetadata* GetThorMetadata() {
    return engine_.GetThorMetadata();
  }

  EvaluateParams& GetEvaluateParams() { return evaluate_params_; }

 private:
  static constexpr int kNumEvaluators = 60;

  SetBoard set_board_;

  std::shared_ptr<State> first_state_;
  State* current_state_;

  Engine engine_;

  EvaluateParams evaluate_params_;

  void ToState(State* new_state);
};

#endif // OTHELLO_SENSEI_MAIN_H