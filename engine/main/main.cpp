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

namespace {
void MoveAnnotationsReset(MoveAnnotations& annotation) {
  annotation.square = kNoSquare;
  annotation.lower = kLessThenMinEval;
  annotation.upper = kLessThenMinEval;
}

void MoveAnnotationsSet(MoveAnnotations& annotation, const Node& node, bool book) {
  annotation.eval = node.GetEval();
  annotation.leaf_eval = node.LeafEval();
  annotation.lower = node.Lower();
  annotation.upper = node.Upper();
  annotation.weak_lower = node.WeakLower();
  annotation.weak_upper = node.WeakUpper();
  annotation.descendants = node.GetNVisited();
  annotation.missing = node.IsSolved() ? 0 : node.RemainingWork(-63, 63);
  annotation.book = book;
}

void ThorGameSet(const Game& game, ThorGame& thor_game, const Sequence& sequence) {
  thor_game.black = game.BlackC();
  thor_game.white = game.WhiteC();
  thor_game.tournament = game.TournamentC();
  thor_game.score = game.Score();
  thor_game.year = game.Year();
  int transposition = sequence.GetTransposition(game.Moves().Subsequence(sequence.Size()));
  for (int i = 0; i < 60; ++i) {
    thor_game.moves[i] = TransposeMove(game.Moves().Moves()[i], transposition);
  }
  thor_game.moves_played = sequence.Size();
}

void CalculateGlobalAnnotations(Annotations& annotations) {
  annotations.positions = 0;
  annotations.positions_calculated = 0;
  annotations.missing = 0;
  annotations.eval = -66;
  for (int i = 0; i < annotations.num_moves; ++i) {
    auto& move = annotations.moves[i];
    annotations.eval = std::max(annotations.eval, -move.eval);
    annotations.positions += move.descendants;
    if (!move.book) {
      annotations.positions_calculated += move.descendants;
    }
    annotations.missing += move.missing;
  }
}
} // anonymous_namespace

void BoardToEvaluate::EvaluateBook() {
  auto board_in_book = book_->Get(unique_);
  if (board_in_book) {
    for (auto* annotation : annotations_) {
      MoveAnnotationsSet(*annotation, *board_in_book, true);
    }
    state_ = STATE_FINISHED;
  }
}

void BoardToEvaluate::Evaluate(EvaluateParams params, int steps) {
  double current_max_time = params.max_time / steps;
  if (state_ == STATE_NOT_STARTED) {
    evaluator_.Evaluate(
        unique_.Player(), unique_.Opponent(), params.lower, params.upper,
        params.max_positions, current_max_time, params.n_threads,
        params.approx);
    state_ = STATE_STARTED;
  } else {
    evaluator_.ContinueEvaluate(params.max_positions, current_max_time,
                                params.n_threads);
  }
  for (MoveAnnotations* annotation : annotations_) {
    MoveAnnotationsSet(*annotation, *evaluator_.GetFirstPosition(), false);
  }
  if (evaluator_.GetStatus() == SOLVED) {
    state_ = STATE_FINISHED;
  }
}

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