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

  void SetCurrentMove(Square current_move) {
    ToState(first_state_->ToDepth(current_move));
    State* child = current_state_;
    for (State* father = current_state_->Father();
         father != nullptr;
         father = father->Father()) {
      father->SetNextStatePlayed(child);
      child = father;
    }
  }

  void Redo() {
    ToState((State*) (
        current_state_->next_state_in_analysis ?
        current_state_->next_state_in_analysis :
        current_state_->next_state_played));
  }

  void Undo() {
    ToState((State*) current_state_->father);
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

  void Evaluate() {
    engine_.Start(current_state_, first_state_, evaluate_params_);
  }

  ThorMetadata* GetThorMetadata() {
    return engine_.GetThorMetadata();
  }

  EvaluateParams& GetEvaluateParams() { return evaluate_params_; }

  void Analyze() {
    for (State* state = first_state_.get(); state != nullptr; state = (State*) state->next_state_played) {
      state->next_state_in_analysis = state->next_state_played;
    }
    engine_.StartAnalysis(first_state_, evaluate_params_);
  }

  // We need a separate call to get the annotations in the UI thread, to prevent
  // concurrent modification.
  Annotations* GetCurrentAnnotations(int current_thread) {
    if (engine_.CurrentThread() != current_thread) {
      return nullptr;
    }
    return current_state_;
  }

  // We need a separate call to get the annotations in the UI thread, to prevent
  // concurrent modification.
  Annotations* GetStartAnnotations(int current_thread) {
    if (engine_.CurrentThread() != current_thread) {
      return nullptr;
    }
    return first_state_.get();
  }

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