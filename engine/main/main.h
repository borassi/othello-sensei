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
    first_state_ = std::make_shared<EvaluationState>(kStartingPositionMove, Board(), true, 0);
    ToState(first_state_.get());
  }

  void PlayMove(Square square) {
    ToState(current_state_->NextState(square));
  }

  void SetCurrentMove(Square current_move) {
    ToState(first_state_->ToDepth(current_move));
    current_state_->SetPlayed();
  }

  void Redo() {
    ToState(current_state_->NextState());
  }

  void Undo() {
    auto father = current_state_->Father();
    ToState(current_state_->AfterPass() ? father->Father() : father);
  }

  void Stop();

  char* GetSequence() {
    std::string sequence = current_state_->GetSequence().ToString();
    char* result = (char*) malloc(sequence.size() * sizeof(char) + 1);
    strcpy(result, sequence.c_str());
    return result;
  }

  bool SetSequence(const std::string& sequence) {
    Sequence moves = Sequence::ParseFromString(sequence);
    if (moves.Size() == 0) {
      return false;
    }
    NewGame();
    for (Square move : moves.Moves()) {
      PlayMove(move);
    }
    return true;
  }

  ThorMetadata* GetThorMetadata() {
    return engine_.GetThorMetadata();
  }

  EvaluateParams& GetEvaluateParams() { return evaluate_params_; }

  void Evaluate() {
    if (analyzing_) {
      if (analyzing_ == 2) {
        ToStateNoStop(current_state_->Father());
      }
      if (!current_state_->Father()) {
        analyzing_ = 0;
      } else {
        analyzing_ = 2;
      }
    }
    engine_.Start(
        analyzing_ ? current_state_->Father() : current_state_,
        first_state_,
        evaluate_params_,
        analyzing_);
  }

  void Analyze() {
    ToState(first_state_->SetAnalyzed());
    analyzing_ = 1;
    engine_.Start(current_state_, first_state_, evaluate_params_, analyzing_);
  }

  // We need a separate call to get the annotations in the UI thread, to prevent
  // concurrent modification.
  Annotations* GetCurrentAnnotations(int current_thread) {
    if (engine_.CurrentThread() != current_thread) {
      return nullptr;
    }
    return current_state_->GetAnnotations();
  }

  // We need a separate call to get the annotations in the UI thread, to prevent
  // concurrent modification.
  Annotations* GetStartAnnotations(int current_thread) {
    if (engine_.CurrentThread() != current_thread) {
      return nullptr;
    }
    last_state_flutter_ = first_state_;
    return last_state_flutter_->GetAnnotations();
  }

 private:
  static constexpr int kNumEvaluators = 60;

  SetBoard set_board_;

  // We keep the last state used by Flutter, so that it does not get deleted
  // when Flutter is still reading it.
  std::shared_ptr<EvaluationState> last_state_flutter_;
  std::shared_ptr<EvaluationState> first_state_;
  EvaluationState* current_state_;

  Engine engine_;
  // 0 = not in analysis; 1 = started analysis (skip undo), 2 = in analysis.
  int analyzing_;

  EvaluateParams evaluate_params_;

  void ToState(EvaluationState* new_state);

  void ToStateNoStop(EvaluationState* new_state);
};

#endif // OTHELLO_SENSEI_MAIN_H