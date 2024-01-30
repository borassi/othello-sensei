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
  evals_(LoadEvals(evals_filepath)),
  book_(book_filepath),
  thor_(thor_filepath),
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
        &thor_,
        &tree_node_supplier_, &hash_map_,
        PatternEvaluator::Factory(evals_.data()),
        static_cast<u_int8_t>(i));
  }
  NewGame();
  auto players = thor_.Players();
  auto tournaments = thor_.Tournaments();
  assert(players.size() == tournaments.size());

  thor_sources_metadata_.reserve(players.size());
  for (const auto& [source_name, source_players] : players) {
    const auto& source_tournaments = tournaments.at(source_name);

    thor_sources_metadata_extended_.push_back(
        std::make_unique<ThorSourceMetadataExtended>(source_name, source_players, source_tournaments));
    thor_sources_metadata_.push_back(thor_sources_metadata_extended_.back()->GetThorSourceMetadata());
  }
  thor_metadata_.sources = thor_sources_metadata_.data();
  thor_metadata_.num_sources = thor_sources_metadata_.size();

  annotations_.example_thor_games = (ThorGame*) malloc(sizeof(ThorGame));
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
    int current_thread,
    std::shared_ptr<std::future<void>> last_future) {
  if (last_future) {
    last_future->get();
  }
  if (current_thread != current_thread_) {
    return;
  }
  const EvaluateParams& params = evaluate_params_;
  if (reset_.exchange(false)) {
    time_ = ElapsedTime();
    tree_node_supplier_.Reset();
    UpdateBoardsToEvaluate();

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
          games.Merge(thor_.GetGames(
              source.name, sequence, filters.max_games, blacks, whites,
              tournaments, filters.start_year, filters.end_year));
        }
      }

      annotations_.num_thor_games = games.num_games;
      annotations_.num_example_thor_games = games.examples.size();
      annotations_.example_thor_games = (ThorGame*) realloc(annotations_.example_thor_games, annotations_.num_example_thor_games * sizeof(ThorGame));
      for (int i = 0; i < annotations_.num_example_thor_games; ++i) {
        ThorGameSet(*games.examples[i], annotations_.example_thor_games[i], sequence);
      }

      for (int i = 0; i < num_boards_to_evaluate_; ++i) {
        boards_to_evaluate_[i]->UpdateWithThor(games.next_moves);
      }
    } else {
      annotations_.num_thor_games = 0;
      annotations_.num_example_thor_games = 0;
    }
  }

  int steps = num_boards_to_evaluate_;
  // We finish if num_boards_to_evaluate_ == 0, or if we cannot get another
  // board to work on.
  for (int i = 0; i < steps; ++i) {
    auto board_to_evaluate = NextBoardToEvaluate(params.delta);
    if (!board_to_evaluate) {
      RunUpdateAnnotations(current_thread, true);
      return;
    }
    board_to_evaluate->Evaluate(params, steps);
  }
  RunUpdateAnnotations(current_thread, num_boards_to_evaluate_ == 0);
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