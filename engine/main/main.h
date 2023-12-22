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

#include "../board/board.h"
#include "../board/get_flip.h"
#include "../board/get_moves.h"
#include "../book/book.h"
#include "../evaluatederivative/evaluator_derivative.h"
#include "../evaluatederivative/tree_node.h"
#include "../evaluatedepthone/pattern_evaluator.h"
#include "bindings.h"
#include "../utils/misc.h"

class State {
 public:
  State(Board board, bool black_turn) : board_(board), black_turn_(black_turn) {}
  State() : State(Board(), true) {}

  const Board& GetBoard() const { return board_; }
  bool GetBlackTurn() const { return black_turn_; }

  std::optional<State> ToNextState(int square) const {
    if (!board_.IsEmpty(square)) {
      return std::nullopt;
    }
    BitPattern flip = GetFlip(square, board_.Player(), board_.Opponent());
    if (!flip) {
      return std::nullopt;
    }

    Board new_board = board_.Next(flip);
    bool new_black_turn;

    if (HaveToPass(new_board.Player(), new_board.Opponent())) {
      new_board.PlayMove(0);
      new_black_turn = black_turn_;
    } else {
      new_black_turn = !black_turn_;
    }

    return State(new_board, new_black_turn);
  }

 private:
  Board board_;
  bool black_turn_;
};

class Main {
 public:
  Main(const std::string& evals_filepath, const std::string& book_filepath, SetBoard set_board, UpdateAnnotations update_annotations);

  void NewGame() {
    Stop();
    states_ = {{Board(), true}};
    current_state_ = 0;
    BoardChanged();
  }

  void PlayMove(Square square) {
    const State& state = states_[current_state_];
    auto next_state = state.ToNextState(square);
    if (!next_state) {
      return;
    }
    Stop();
    ++current_state_;
    states_.erase(states_.begin() + current_state_, states_.end());
    states_.push_back(next_state.value());
    BoardChanged();
  }

  void Redo() {
    if (current_state_ == states_.size() - 1) {
      return;
    }
    Stop();
    ++current_state_;
    BoardChanged();
  }

  void Undo() {
    if (current_state_ == 0) {
      return;
    }
    Stop();
    --current_state_;
    BoardChanged();
  }

  void Stop();

  void Evaluate(int lower, int upper, NVisited max_positions, double max_time, double delta, int n_threads, bool approx) {
    Stop();
    current_future_ = std::make_shared<std::future<void>>(std::async(
        std::launch::async, &Main::EvaluateThread, this, lower, upper,
        max_positions, max_time, delta, n_threads, approx, current_thread_.load(),
        current_future_));
  }

 private:
  static constexpr int kNumEvaluators = 60;

  SetBoard set_board_;
  UpdateAnnotations update_annotations_;

  std::vector<State> states_;
  int current_state_;

  EvalType evals_;
  HashMap<kBitHashMap> hash_map_;
  TreeNodeSupplier tree_node_supplier_;
  std::array<std::unique_ptr<EvaluatorDerivative>, kNumEvaluators> evaluators_;

  std::unordered_map<Board, std::pair<EvaluatorDerivative*, std::vector<int>>> last_boards_;
  Annotations annotations_;
  ElapsedTime time_;

  std::atomic_uint32_t current_thread_;
  std::shared_ptr<std::future<void>> current_future_;
  std::atomic_bool reset_;

  Book<kBookVersion> book_;

  void BoardChanged();

  void EvaluateThread(
      int lower, int upper, NVisited max_positions, double max_time,
      double delta, int n_threads, bool approx, int current_thread,
      std::shared_ptr<std::future<void>> last_future);
};

#endif // OTHELLO_SENSEI_MAIN_H