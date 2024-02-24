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

#include "state.h"
#include "../board/get_moves.h"

State* State::NextState(Square move) {
  State* previous_sibling = nullptr;
  for (std::shared_ptr<State> child = first_child_; child != nullptr; child = child->next_sibling_) {
    if (child->move == move) {
      next_state_played = (Annotations*) child.get();
//      if (previous_sibling != nullptr) {
//        previous_sibling->SetNextSibling(child->next_sibling_);
//        child->SetNextSibling(first_child_);
//        SetFirstChild(child);
//      }
      return child.get();
    }
    previous_sibling = child.get();
  }
  return nullptr;
}

void State::SetNextStates() {
  if (first_child_ != nullptr) {
    return;
  }
  auto flips = GetAllMovesWithPass(board_.Player(), board_.Opponent());
  State* last_state = nullptr;

  for (const auto& flip : flips) {
    Board next = board_.Next(flip);
    Square move = __builtin_ctzll(SquareFromFlip(flip, board_.Player(), board_.Opponent()));
    bool next_black_turn;

    if (HaveToPass(next) && !IsGameOver(next)) {
      next.PlayMove(0);
      next_black_turn = black_turn;
    } else {
      next_black_turn = !black_turn;
    }

    std::shared_ptr<State> state = std::make_shared<State>(move, next, next_black_turn, depth + 1);
    state->SetFather(this);

    if (last_state) {
      last_state->SetNextSibling(state);
    } else {
      SetFirstChild(state);
    }
    last_state = state.get();
  }
}