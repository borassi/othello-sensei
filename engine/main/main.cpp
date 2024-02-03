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
    update_annotations_(update_annotations),
    hash_map_(),
    boards_to_evaluate_(),
    num_boards_to_evaluate_(0),
    tree_node_supplier_(),
    current_future_(std::make_shared<std::future<void>>(std::async(
        std::launch::async, &Main::InitializeEvaluator, this, evals_filepath,
        book_filepath, thor_filepath
    ))) {
  for (int i = 0; i < annotations_.size(); ++i) {
    annotations_[i].example_thor_games = (ThorGame*) malloc(sizeof(ThorGame));
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
  State& state = states_[current_state_];
  const Board& board = state.GetBoard();
  auto moves = GetAllMovesWithPass(board.Player(), board.Opponent());
  bool finished = true;
  Annotations& annotations = annotations_[current_state_];

  num_boards_to_evaluate_ = 0;
  annotations.num_moves = 0;

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
    MoveAnnotations& annotation = annotations.moves[annotations.num_moves++];
    annotation.square = move;
    board_to_evaluate->AddAnnotation(&annotation);
  }
}

bool IncludeAllSources(ThorMetadata thor_metadata) {
  for (int i = 0; i < thor_metadata.num_sources; ++i) {
    const ThorSourceMetadata& source = *thor_metadata.sources[i];
    if (source.selected_blacks[0] >= 0 || source.selected_whites[0] >= 0 || source.selected_tournaments[0] >= 0) {
      return false;
    }
  }
  return true;
}

void Main::EvaluateThread(
    int current_thread, int current_state, Square move, bool black_turn,
    std::shared_ptr<std::future<void>> last_future) {
  Annotations& annotations = annotations_[current_state];
  annotations.black_turn = black_turn;
  annotations.move = move;
  last_future->get();
  if (current_thread != current_thread_) {
    return;
  }
  const EvaluateParams& params = evaluate_params_;
  if (reset_.exchange(false)) {
    time_ = ElapsedTime();
    tree_node_supplier_.Reset();
    UpdateBoardsToEvaluate();
    annotations.valid = false;

    if (params.use_book) {
      for (int i = 0; i < num_boards_to_evaluate_; ++i) {
        boards_to_evaluate_[i]->EvaluateBook();
      }
    }
    if (params.thor_filters.use_thor) {
      auto filters = params.thor_filters;
      const Sequence& sequence = GetSequence();
      GamesList games;
      games.max_games = filters.max_games;

      bool include_all_sources = IncludeAllSources(thor_metadata_);
      for (int i = 0; i < thor_metadata_.num_sources; ++i) {
        const ThorSourceMetadata& source = *thor_metadata_.sources[i];
        std::vector<std::string> blacks;
        std::vector<std::string> whites;
        std::vector<std::string> tournaments;

        for (int i = 0; source.selected_blacks[i] >= 0; ++i) {
          blacks.push_back(source.players[source.selected_blacks[i]]);
        }
        for (int i = 0; source.selected_whites[i] >= 0; ++i) {
          whites.push_back(source.players[source.selected_whites[i]]);
        }
        for (int i = 0; source.selected_tournaments[i] >= 0; ++i) {
          tournaments.push_back(source.tournaments[source.selected_tournaments[i]]);
        }

        if (include_all_sources || !blacks.empty() || !whites.empty() || !tournaments.empty()) {
          games.Merge(thor_->GetGames(
              source.name, sequence, filters.max_games, blacks, whites,
              tournaments, filters.start_year, filters.end_year));
        }
      }

      annotations.num_thor_games = games.num_games;
      annotations.num_example_thor_games = games.examples.size();
      annotations.example_thor_games = (ThorGame*) realloc(annotations.example_thor_games, annotations.num_example_thor_games * sizeof(ThorGame));
      for (int i = 0; i < annotations.num_example_thor_games; ++i) {
        ThorGameSet(*games.examples[i], annotations.example_thor_games[i], sequence);
      }

      for (int i = 0; i < num_boards_to_evaluate_; ++i) {
        boards_to_evaluate_[i]->UpdateWithThor(games.next_moves);
      }
    } else {
      annotations.num_thor_games = 0;
      annotations.num_example_thor_games = 0;
    }
  }

  int steps = num_boards_to_evaluate_;
  // We finish if num_boards_to_evaluate_ == 0, or if we cannot get another
  // board to work on.
  for (int i = 0; i < steps; ++i) {
    auto board_to_evaluate = NextBoardToEvaluate(params.delta);
    if (!board_to_evaluate) {
      RunUpdateAnnotations(current_thread, current_state, true);
      return;
    }
    board_to_evaluate->Evaluate(params, steps);
  }
  RunUpdateAnnotations(current_thread, current_state, num_boards_to_evaluate_ == 0);
}

void Main::RunUpdateAnnotations(int current_thread, int current_state, bool finished) {
  Annotations& annotations = annotations_[current_state];
  if (current_thread != current_thread_) {
    return;
  }
  annotations.seconds = time_.Get();
  annotations.finished = finished;
  CalculateGlobalAnnotations(annotations);
  if (current_state_ > 0) {
    UpdateFatherAnnotations(current_state - 1);
  }
  annotations.valid = true;
  update_annotations_(annotations_.data());
}

void Main::UpdateFatherAnnotations(int state) {
  assert(state >= 0 && state < annotations_.size() - 1);
  Annotations& annotations = annotations_[state];
  const Annotations& next_annotations = annotations_[state + 1];
  for (int i = 0; i < annotations.num_moves; ++i) {
    MoveAnnotations& move_annotations = annotations.moves[i];
    if (move_annotations.square == next_annotations.move) {
      move_annotations.eval = next_annotations.eval;
      break;
    }
  }
  double old_eval = annotations.eval;
  CalculateGlobalAnnotations(annotations);
  if (annotations.eval != old_eval && state > 0) {
    UpdateFatherAnnotations(state - 1);
  }
}

void Main::BoardChanged() {
  const State& state = states_[current_state_];
  const Board& board = state.GetBoard();
  reset_ = true;
  set_board_({board.Player(), board.Opponent(), state.GetBlackTurn()});
}