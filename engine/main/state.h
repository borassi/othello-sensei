/*
 * Copyright 2024 Michele Borassi
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

#ifndef OTHELLO_SENSEI_STATE_H
#define OTHELLO_SENSEI_STATE_H

#include <memory>

#include "bindings.h"

#include "../board/board.h"
#include "../board/sequence.h"

enum SetNextStateResult {
  INVALID_MOVE,
  SAME_STATE,
  DIFFERENT_STATE,
};

class State : public Annotations {
 public:
  State(Square move, Board board, bool black_turn, int depth) {
    this->move = move;
    this->black_turn = black_turn;
    board_ = board;
    example_thor_games = (ThorGame*) malloc(sizeof(ThorGame));
    num_example_thor_games = 0;
    num_thor_games = 0;
    SetFather(nullptr);
    SetFirstChild(nullptr);
    SetNextSibling(nullptr);
    SetNextStatePlayed(nullptr);
    SetNextStateInAnalysis(nullptr);
    this->depth = depth;
    valid = false;
    derived = false;
    finished = false;
    analyzed = false;
    prob_lower_eval = NAN;
    prob_upper_eval = NAN;
    proof_number_lower = NAN;
    disproof_number_upper = NAN;
  }
  ~State() { free(example_thor_games); }
  State(const State&) = delete;

  State* Father() const { return (State*) father; }
  const Board& GetBoard() const { return board_; }
  bool GetBlackTurn() const { return black_turn; }
  Square Move() const { return move; }
  int Depth() const { return depth; }
  bool HasValidChildren() const {
    for (State* child = first_child_.get(); child != nullptr; child = child->next_sibling_.get()) {
      if (!child->valid) {
        return false;
      }
    }
    return true;
  }

  State* NextState(Square move);

  void SetNextStatePlayed(State* new_value) { next_state_played = new_value; }

  State* NextState() const {
    return NextStateInAnalysis() ? NextStateInAnalysis() : NextStatePlayed();
  }
  State* NextStatePlayed() const { return (State*) next_state_played; }
  State* NextStateInAnalysis() const { return (State*) next_state_in_analysis; }

  void SetNextStates();

  std::vector<State*> GetChildren() const {
    std::vector<State*> result;
    for (State* child = first_child_.get(); child != nullptr; child = child->next_sibling_.get()) {
      result.push_back(child);
    }
    return result;
  }

  State* ToDepth(int new_depth) {
    State* result = this;
    assert(new_depth >= result->Depth());
    for (;
         result != nullptr && new_depth > result->Depth();
         result = result->NextState()) {}
    return result;
  }

  Sequence GetSequence() const {
    std::vector<Square> moves_inverted;
    for (const State* state = this; state->Father() != nullptr; state = state->Father()) {
      moves_inverted.push_back(state->Move());
    }
    return Sequence(moves_inverted.rbegin(), moves_inverted.rend());
  }

  bool InBook() const {
    return provenance == BOOK || provenance == CHILD_BOOK;
  }

 private:
  // TODO: Make each state contain a TreeNode.
  Board board_;
  std::shared_ptr<State> first_child_;
  std::shared_ptr<State> next_sibling_;

  void SetFather(State* new_value) {
    this->father = new_value;
  }

  void SetFirstChild(const std::shared_ptr<State>& new_value) {
    first_child_ = new_value;
    this->first_child = new_value.get();
  }

  void SetNextSibling(const std::shared_ptr<State>& new_value) {
    next_sibling_ = new_value;
    this->next_sibling = new_value.get();
  }

  void SetNextStateInAnalysis(State* new_value) {
    next_state_in_analysis = new_value;
  }
};

#endif // OTHELLO_SENSEI_STATE_H
