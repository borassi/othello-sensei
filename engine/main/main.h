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
#include "../xot/xot.h"

class Main {
 public:
  Main(
      const std::string& evals_filepath,
      const std::string& book_filepath,
      const std::string& thor_filepath,
      const std::string& xot_small_filepath,
      const std::string& xot_large_filepath,
      SetBoard set_board,
      UpdateAnnotations update_annotations);

  void NewGame() {
    first_state_ = std::make_shared<EvaluationState>(kStartingPositionMove, Board(), true, 0);
    [[maybe_unused]] bool new_state = ToState(first_state_.get());
    assert(new_state);
  }

  bool PlayMove(Square square) {
    return ToState(current_state_->NextState(square));
  }

  bool SetCurrentMove(Square current_move) {
    bool valid = ToState(first_state_->ToDepth(current_move));
    current_state_->SetPlayed();
    return valid;
  }

  bool Redo() {
    return ToState(current_state_->NextState());
  }

  bool Undo() {
    return ToState(current_state_->PreviousNonPass());
  }

  bool ToAnalyzedGameOrLastChoice() {
    EvaluationState* non_xot_state = nullptr;
    if (first_state_->NextStateInAnalysis()) {
      non_xot_state = current_state_->LastAnalyzedState();
    } else {
      non_xot_state = current_state_->LastChoice();
    }
    if (IsXOT() && non_xot_state->NEmpties() > 52 && current_state_->NEmpties() < 52) {
      return ToState(first_state_->ToDepth(8));
    }
    return ToState(non_xot_state);
  }

  void ResetEvaluations() {
    SetSequence(current_state_->GetSequence());
  }

  void Stop();

  char* GetSequence() {
    std::string sequence = current_state_->GetSequence().ToString();
    char* result = (char*) malloc(sequence.size() * sizeof(char) + 1);
    strncpy(result, sequence.c_str(), sequence.size() + 1);
    return result;
  }

  void SetSequence(const Sequence& moves) {
    NewGame();
    for (int i = 0; i < moves.Size(); ++i) {
      PlayMove(moves.Move(i));
    }
  }

  bool SetSequence(const std::string& sequence) {
    Sequence moves = Sequence::ParseFromString(sequence);
    if (moves.Size() == 0 && sequence != "") {
      return false;
    }
    SetSequence(moves);
    return true;
  }

  ThorMetadata* GetThorMetadata() {
    return engine_.GetThorMetadata();
  }

  EvaluateParams& GetEvaluateParams() { return evaluate_params_; }

  void Evaluate();

  void Analyze() {
    last_params_ = evaluate_params_;
    // This also resets all the evaluations.
    SetSequence(first_state_->GetLongestSequence());
    first_state_->SetAnalyzed();
    analyzing_ = 1;
    engine_.Start(current_state_, first_state_, evaluate_params_, analyzing_);
  }

  void ResetAnalyzedGame() {
    first_state_->SetNotAnalyzed();
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

  void RandomXOT(bool large) {
    XOT& xot = large ? xot_small_ : xot_large_;
    Sequence sequence = xot.RandomSequence();
    SetSequence(sequence);
  }

  void SetXOTState(XOTState xot_state) {
    xot_state_ = xot_state;
    RunSetBoard();
  }
  XOTState GetXOTState() { return xot_state_; }

 private:
  static constexpr int kNumEvaluators = 60;

  SetBoard set_board_;

  // We keep the last state used by Flutter, so that it does not get deleted
  // when Flutter is still reading it.
  std::shared_ptr<EvaluationState> last_state_flutter_;
  std::shared_ptr<EvaluationState> first_state_;
  EvaluationState* current_state_;
  XOT xot_small_;
  XOT xot_large_;
  XOTState xot_state_;

  Engine engine_;
  // 0 = not in analysis; 1 = started analysis (skip undo), 2 = in analysis.
  int analyzing_;

  EvaluateParams evaluate_params_;
  EvaluateParams last_params_;

  bool ToState(EvaluationState* new_state);

  void ToStateNoStop(EvaluationState* new_state);

  void RunSetBoard() {
    Board board = current_state_->ToBoard();
    set_board_({board.Player(), board.Opponent(), current_state_->BlackTurn(), IsXOT(), current_state_->LastMove()});
  }

  bool IsXOT() {
    switch (xot_state_) {
      case XOT_STATE_ALWAYS:
        return true;
      case XOT_STATE_NEVER:
        return false;
      case XOT_STATE_AUTOMATIC:
        auto state_in_xot = first_state_->ToDepth(8);
        if (!state_in_xot) {
          return false;
        }
        if (!xot_large_.IsInListPrefix(state_in_xot->GetSequence())) {
          return false;
        }
        auto [error_black, error_white] = state_in_xot->TotalError();
        return std::max(error_black, error_white) > 5;
    }
  }
};

#endif // OTHELLO_SENSEI_MAIN_H