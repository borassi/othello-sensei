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
#include "../thor/thor.h"

namespace {

void ThorGameSet(const Game& game, ThorGame& thor_game, const Sequence& sequence) {
  thor_game.black = game.BlackC();
  thor_game.white = game.WhiteC();
  thor_game.tournament = game.TournamentC();
  thor_game.score = game.Score();
  thor_game.year = game.Year();
  auto game_sequence = game.Moves().Subsequence(sequence.Size());
  assert(game.Moves().Size() >= sequence.Size());
  int transposition = sequence.GetTransposition(game_sequence);
  for (int i = 0; i < 60; ++i) {
    thor_game.moves[i] = TransposeMove(game.Moves().Move(i), transposition);
    assert(thor_game.moves[i] == kNoSquare || (thor_game.moves[i] >= 0 && thor_game.moves[i] <= 63));
  }
  thor_game.moves_played = sequence.Size();
}

}  // anonymous namespace

void EvaluationState::SetThor(const GamesList& games) {
  auto sequence = GetSequence();

  annotations_.num_thor_games = games.num_games;
  annotations_.num_example_thor_games = (int) games.examples.size();
  annotations_.example_thor_games = (ThorGame*) realloc(
      annotations_.example_thor_games,
      annotations_.num_example_thor_games * sizeof(ThorGame));

  for (unsigned i = 0; i < annotations_.num_example_thor_games; ++i) {
    assert(games.examples[i].Moves().Size() >= sequence.Size());
    ThorGameSet(games.examples[i], annotations_.example_thor_games[i], sequence);
  }
}

EvaluationState* EvaluationState::NextState(Square move) const {
  for (const std::shared_ptr<EvaluationState>& child : children_) {
    if (child->annotations_.move == move) {
      if (child->AfterPass()) {
        assert(HaveToPass(child->Father()->ToBoard()));
        return child->GetChildren()[0];
      }
      return child.get();
    }
  }
  return nullptr;
}

void EvaluationState::SetNextStates() {
  if (!IsLeaf()) {
    return;
  }
  if (MaybeSetGameOver()) {
    return;
  }
  Board board = ToBoard();
  auto flips = GetAllMovesWithPass(board.Player(), board.Opponent());
  std::vector<TreeNode*> children;

  for (const auto& flip : flips) {
    Board next = board.Next(flip);
    Square move;
    if (flip == 0) {
      assert(!IsGameOver(next));
      move = kPassMove;
    } else {
      move = (Square) __builtin_ctzll(SquareFromFlip(flip, board.Player(), board.Opponent()));
    }

    std::shared_ptr<EvaluationState> state = std::make_shared<EvaluationState>(
        move, next, !BlackTurn(), Depth() + 1);

    children_.push_back(state);
    children.push_back(state.get());
  }
  SetChildrenNoUpdate(children);
  UpdateAnnotationsTree();
}