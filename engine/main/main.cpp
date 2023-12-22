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
  annotation.book = book;
}
} // anonymous_namespace

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
  tree_node_supplier_(),
  current_thread_(0),
  current_future_(nullptr) {
  for (int i = 0; i < kNumEvaluators; ++i) {
    evaluators_[i] = std::make_unique<EvaluatorDerivative>(
        &tree_node_supplier_, &hash_map_,
        PatternEvaluator::Factory(evals_.data()),
        static_cast<u_int8_t>(i));
  }
  NewGame();
}

// This runs in the main thread, so that we cannot update the output afterwards.
void Main::Stop() {
  ++current_thread_;
  for (auto& [board, evaluator_annotations] : last_boards_) {
    evaluator_annotations.first->Stop();
  }
}

bool NeedNewMoveAnnotations(Square move, const Annotations& annotations, const std::vector<int>& positions) {
  for (int i = 0; i < annotations.num_moves; ++i) {
    if (annotations.moves[i].square == move) {
      return false;
    }
  }
  return true;
}

void Main::EvaluateThread(
    int lower, int upper, NVisited max_positions, double max_time, double delta,
    int n_threads, bool approx, int current_thread,
    std::shared_ptr<std::future<void>> last_future) {
  if (last_future) {
    last_future->get();
  }
  if (reset_.exchange(false)) {
    tree_node_supplier_.Reset();
    annotations_.num_moves = 0;
    last_boards_.clear();
    time_ = ElapsedTime();
  }

  const Board& board = states_[current_state_].GetBoard();
  auto moves = GetAllMovesWithPass(board.Player(), board.Opponent());
  bool finished = true;

  for (const auto& flip : moves) {
    Board next = board.Next(flip);
    auto board_in_book = book_.Get(next);
    Board unique = next.Unique();
    Square move = __builtin_ctzll(SquareFromFlip(flip, board.Player(), board.Opponent()));
    auto [iter, inserted] = last_boards_.insert({unique, {evaluators_[last_boards_.size()].get(), {}}});
    auto& [evaluator, move_indices] = iter->second;

    if (NeedNewMoveAnnotations(move, annotations_, move_indices)) {
      int index = annotations_.num_moves++;
      MoveAnnotations& annotation = annotations_.moves[index];
      annotation.square = move;
      move_indices.push_back(index);
    }

    if (board_in_book) {
      for (int index : move_indices) {
        MoveAnnotationsSet(annotations_.moves[index], *board_in_book, true);
      }
      continue;
    }

    if (inserted) {
      evaluator->Evaluate(
          unique.Player(), unique.Opponent(), lower, upper, max_positions, max_time / moves.size(), n_threads, approx);
    } else {
      evaluator->ContinueEvaluate(max_positions, max_time / moves.size(), n_threads);
    }

    if (current_thread != current_thread_) {
      break;
    }
    for (int index : move_indices) {
      MoveAnnotationsSet(
          annotations_.moves[index],
          *evaluator->GetFirstPosition(),
          false
      );
    }
    finished = finished && (evaluator->GetStatus() == SOLVED);
  }
  annotations_.seconds = time_.Get();
  annotations_.positions = 0;
  for (int i = 0; i < annotations_.num_moves; ++i) {
    annotations_.positions += annotations_.moves[i].descendants;
  }
  annotations_.finished = finished;
  if (current_thread == current_thread_) {
    update_annotations_(annotations_);
  }
}

void Main::BoardChanged() {
  const State& state = states_[current_state_];
  const Board& board = state.GetBoard();
  reset_ = true;
  set_board_({board.Player(), board.Opponent(), state.GetBlackTurn()});
}