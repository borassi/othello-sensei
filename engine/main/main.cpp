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
    SetBoard set_board,
    UpdateAnnotations update_annotations) :
  evals_(LoadEvals(evals_filepath)),
  book_(book_filepath),
  set_board_(set_board),
  update_annotations_(update_annotations),
  hash_map_(),
  boards_to_evaluate_(),
  num_boards_to_evaluate_(0),
  tree_node_supplier_(),
  current_thread_(0),
  current_future_(nullptr) {
  for (int i = 0; i < kNumEvaluators; ++i) {
    boards_to_evaluate_[i] = std::make_unique<BoardToEvaluate>(
        &book_,
        &tree_node_supplier_, &hash_map_,
        PatternEvaluator::Factory(evals_.data()),
        static_cast<u_int8_t>(i));
  }
  NewGame();
}

// This runs in the main thread, so that we cannot update the output afterwards.
void Main::Stop() {
  ++current_thread_;
  for (int i = 0; i < num_boards_to_evaluate_; ++i) {
    boards_to_evaluate_[i]->Stop();
  }
}

void Main::UpdateBoardsToEvaluate() {
  const Board& board = states_[current_state_].GetBoard();
  auto moves = GetAllMovesWithPass(board.Player(), board.Opponent());
  bool finished = true;

  num_boards_to_evaluate_ = 0;
  annotations_.num_moves = 0;

  for (const auto& flip : moves) {
    Board next = board.Next(flip);
    Board unique = next.Unique();
    Square move = __builtin_ctzll(SquareFromFlip(flip, board.Player(), board.Opponent()));

    // Create or get the BoardToEvaluate.
    BoardToEvaluate* board_to_evaluate;
    bool found = false;
    for (int i = 0; i < num_boards_to_evaluate_; ++i) {
      BoardToEvaluate* b = boards_to_evaluate_[i].get();
      if (b->Unique() == unique) {
        found = true;
        break;
      }
    }
    if (!found) {
      board_to_evaluate = boards_to_evaluate_[num_boards_to_evaluate_++].get();
      board_to_evaluate->Reset(unique);
    }

    // Add the current move.
    MoveAnnotations& annotation = annotations_.moves[annotations_.num_moves++];
    annotation.square = move;
    board_to_evaluate->AddAnnotation(&annotation);
  }
}

void Main::EvaluateThread(
    const EvaluateParams& params, int current_thread,
    std::shared_ptr<std::future<void>> last_future) {
  if (last_future) {
    last_future->get();
  }
  if (current_thread != current_thread_) {
    return;
  }
  if (reset_.exchange(false)) {
    time_ = ElapsedTime();
    tree_node_supplier_.Reset();
    UpdateBoardsToEvaluate();

    if (params.use_book) {
      for (int i = 0; i < num_boards_to_evaluate_; ++i) {
        boards_to_evaluate_[i]->EvaluateBook();
      }
    }
  }

  int steps = num_boards_to_evaluate_;
  for (int i = 0; i < steps; ++i) {
    auto board_to_evaluate = NextBoardToEvaluate(params.delta);
    if (!board_to_evaluate) {
      RunUpdateAnnotations(current_thread, true);
      return;
    }
    board_to_evaluate->Evaluate(params, steps);
  }
  RunUpdateAnnotations(current_thread, false);
}

void Main::RunUpdateAnnotations(int current_thread, bool finished) {
  if (current_thread != current_thread_) {
    return;
  }
  annotations_.seconds = time_.Get();
  annotations_.positions = 0;
  annotations_.positions_calculated = 0;
  annotations_.missing = 0;
  for (int i = 0; i < annotations_.num_moves; ++i) {
    auto& move = annotations_.moves[i];
    annotations_.positions += move.descendants;
    if (!move.book) {
      annotations_.positions_calculated += move.descendants;
    }
    annotations_.missing += move.missing;
  }
  annotations_.finished = finished;
  update_annotations_(annotations_);
}

void Main::BoardChanged() {
  const State& state = states_[current_state_];
  const Board& board = state.GetBoard();
  reset_ = true;
  set_board_({board.Player(), board.Opponent(), state.GetBlackTurn()});
}