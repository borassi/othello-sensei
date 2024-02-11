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

#include "../board/board.h"
#include "../board/get_flip.h"
#include "../board/get_moves.h"
#include "../book/book.h"
#include "../evaluatederivative/evaluator_derivative.h"
#include "../evaluatederivative/tree_node.h"
#include "../evaluatedepthone/pattern_evaluator.h"
#include "../thor/thor.h"
#include "../utils/misc.h"

enum SetNextStateResult {
  INVALID_MOVE,
  SAME_STATE,
  DIFFERENT_STATE,
};

class State {
 public:
  State() : move_(kNoSquare) {}

  void Unset() { move_ = kNoSquare; }

  void Set(Square move, Board board, bool black_turn) {
    move_ = move;
    board_ = board;
    black_turn_ = black_turn;
  }

  bool IsSet() const { return move_ != kNoSquare; }

  const Board& GetBoard() const {
    assert(IsSet());
    return board_;
  }
  bool GetBlackTurn() const {
    assert(IsSet());
    return black_turn_;
  }
  Square Move() const {
    assert(IsSet());
    return move_;
  }

  SetNextStateResult SetNextState(Square move, State& next_state) const {
    assert(IsSet());
    if (!board_.IsEmpty(move)) {
      return INVALID_MOVE;
    }
    BitPattern flip = GetFlip(move, board_.Player(), board_.Opponent());
    if (!flip) {
      return INVALID_MOVE;
    }

    Board new_board = board_.Next(flip);
    bool new_black_turn;

    if (HaveToPass(new_board.Player(), new_board.Opponent())) {
      new_board.PlayMove(0);
      new_black_turn = black_turn_;
    } else {
      new_black_turn = !black_turn_;
    }
    if (next_state.IsSet() && next_state.move_ == move) {
      return SAME_STATE;
    }
    next_state.Set(move, new_board, new_black_turn);
    return DIFFERENT_STATE;
  }

 private:
  Square move_;
  Board board_;
  bool black_turn_;
};

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
    states_[0].Set(kStartingPositionMove, Board(), true);
    states_[1].Unset();
    current_state_ = 0;
    BoardChanged();
  }

  void PlayMove(Square square) {
    const State& state = states_[current_state_];
    SetNextStateResult result = state.SetNextState(square, states_[current_state_ + 1]);
    if (result == INVALID_MOVE) {
      return;
    }
    Stop();
    ++current_state_;
    if (result == DIFFERENT_STATE) {
      for (int i = current_state_ + 1; states_[i].IsSet(); ++i) {
        states_[i].Unset();
      }
    }
    BoardChanged();
  }

  void SetCurrentMove(int current_move) {
    if (current_move < 0 || current_move >= states_.size() ||
        !states_[current_move].IsSet()) {
      return;
    }
    Stop();
    current_state_ = current_move;
    BoardChanged();
  }

  void Redo() {
    SetCurrentMove(current_state_ + 1);
  }

  void Undo() {
    SetCurrentMove(current_state_ - 1);
  }

  void Stop();

  void Evaluate() {
    const State& current_state = states_[current_state_];
    engine_.Start(GetSequence(), current_state.GetBoard(),
                  current_state.GetBlackTurn(), evaluate_params_);
  }

  ThorMetadata* GetThorMetadata() {
    return engine_.GetThorMetadata();
  }

  EvaluateParams& GetEvaluateParams() { return evaluate_params_; }

 private:
  static constexpr int kNumEvaluators = 60;

  SetBoard set_board_;

  std::array<State, 64> states_;
  int current_state_;

  Engine engine_;

  EvaluateParams evaluate_params_;

  void BoardChanged();

  Sequence GetSequence() {
    if (states_[0].GetBoard() != Board()) {
      return Sequence();
    }
    std::vector<Square> moves;
    for (int i = 1; i <= current_state_; ++i) {
      moves.push_back(states_[i].Move());
    }
    return Sequence(moves);
  }
};

#endif // OTHELLO_SENSEI_MAIN_H