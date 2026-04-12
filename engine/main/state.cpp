/*
 * Copyright 2024-2026 Michele Borassi
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

void GameToThorGame(const Game& game, ThorGame& thor_game, const Sequence& sequence) {
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

void EvaluationState::SetThor(const GamesList& games) {
  assert(!IsModified());
  std::optional<Sequence> sequence_opt = GetSequence();
  if (!sequence_opt) {
    assert(false);
    return;
  }
  const auto& sequence = *sequence_opt;
  annotations_.num_thor_games = games.num_games;
  annotations_.num_example_thor_games = (int) games.examples.size();
  annotations_.example_thor_games = (ThorGame*) realloc(
      annotations_.example_thor_games,
      // Avoid a realloc with size 0.
      std::max(1U, annotations_.num_example_thor_games) * sizeof(ThorGame));

  for (unsigned i = 0; i < annotations_.num_example_thor_games; ++i) {
    assert(games.examples[i].Moves().Size() >= sequence.Size());
    GameToThorGame(games.examples[i], annotations_.example_thor_games[i], sequence);
  }
}

void EvaluationState::ExecuteTransposition(int transposition) {
  Board transposition_board = ToBoard().AllTranspositions()[transposition];
  player_ = transposition_board.Player();
  opponent_ = transposition_board.Opponent();
  for (int i = 0; i < annotations_.num_moves; ++i) {
    if (annotations_.moves[i] >= 0 && annotations_.moves[i] <= 63) {
      annotations_.moves[i] = TransposeMove(annotations_.moves[i], transposition);
    }
  }
}

namespace {
int GetTransposition(Board original, Board transposed) {
  auto original_transpositions = original.AllTranspositions();
  for (int i = 0; i < original_transpositions.size(); ++i) {
    if (original_transpositions[i] == transposed) {
      return i;
    }
  }
  assert(false);
  return 0;
}
}

EvaluationState* EvaluationState::NextState(Square move) const {
  for (const std::shared_ptr<EvaluationState>& child : children_) {
    for (int i = 0; i < child->annotations_.num_moves; ++i) {
      if (child->annotations_.moves[i] == move) {
        if (i != 0) {
          // NOTE: We can't just run ExecuteTransposition, because one of the moves might map to a
          // non-move. Counterexample:
          // XOOOOOO-
          // O------O
          // O------O
          // O------O
          // O------O
          // O------O
          // O------O
          // -OOOOOO-
          // Here, playing H1 and A8 yield the same position, but if we just run
          // ExecuteTransposition we map A8 to H8 which is not a legal move.
          Board new_board = ToBoard();
          new_board.PlayMove(GetFlip(move, new_board.Player(), new_board.Opponent()));
          int transposition = GetTransposition(child->ToBoard(), new_board);
          assert(transposition != 0);
          child->ExecuteTranspositionRecursive(transposition);
          child->player_ = new_board.Player();
          child->opponent_ = new_board.Opponent();
          std::swap(child->annotations_.moves[0], child->annotations_.moves[i]);
        }
        return child->ThisOrNextLandable(SENSEI_EVALUATES);
      }
    }
  }
  return nullptr;
}

namespace {

EvaluationState* TranspositionChild(const std::vector<std::shared_ptr<EvaluationState>> children, const Board& board) {
  std::vector<Board> all_transpositions = board.AllTranspositions();
  for (const auto& child : children) {
    Board child_board = child->ToBoard();
    for (int i = 0; i < all_transpositions.size(); ++i) {
      if (child_board == all_transpositions[i]) {
        return child.get();
      }
    }
  }
  return nullptr;
}

}  // namespace

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
      assert(flips.size() == 1);
      assert(!IsGameOver(next));
      move = kPassMove;
    } else {
      move = (Square) __builtin_ctzll(SquareFromFlip(flip, board.Player(), board.Opponent()));
    }

    EvaluationState* transposition_child = TranspositionChild(children_, next);
    if (transposition_child != nullptr) {
      transposition_child->AddMove(move);
      continue;
    }
    std::shared_ptr<EvaluationState> state = std::make_shared<EvaluationState>(
        move, next, !BlackTurn(), Depth() + 1, annotations_.depth_no_pass + (flip == 0 ? 0 : 1), false);
    if (HaveToPass(state->ToBoard())) {
      state->SetNextStates();
    }

    children_.push_back(state);
    children.push_back(state.get());
  }
  SetChildrenNoUpdate(children);
  UpdateAnnotationsTree();
}