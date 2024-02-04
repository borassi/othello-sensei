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
    stoppable_ = false;
    evaluator_.Evaluate(
        unique_.Player(), unique_.Opponent(), params.lower, params.upper,
        params.max_positions, current_max_time, params.n_threads,
        params.approx);
    state_ = STATE_STARTED;
  } else {
    stoppable_ = true;
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

Engine::Engine(
    const std::string& evals_filepath,
    const std::string& book_filepath,
    const std::string& thor_filepath,
    UpdateAnnotations update_annotations) :
    update_annotations_(update_annotations),
    hash_map_(),
    boards_to_evaluate_(),
    num_boards_to_evaluate_(0),
    tree_node_supplier_(),
    last_offset_(-1),
    last_black_turn_(true),
    current_future_(std::make_shared<std::future<void>>(std::async(
        std::launch::async, &Engine::Initialize, this, evals_filepath,
        book_filepath, thor_filepath
    ))) {}

void Engine::Initialize(
    const std::string& evals_filepath,
    const std::string& book_filepath,
    const std::string& thor_filepath) {
  evals_ = std::make_unique<EvalType>(LoadEvals(evals_filepath));
  book_ = std::make_unique<Book<kBookVersion>>(book_filepath);
  thor_ = std::make_unique<Thor>(thor_filepath);
  for (int i = 0; i < annotations_.size(); ++i) {
    annotations_[i].valid = false;
    annotations_[i].example_thor_games = (ThorGame*) malloc(sizeof(ThorGame));
  }
  for (int i = 0; i < kNumEvaluators; ++i) {
    boards_to_evaluate_[i] = std::make_unique<BoardToEvaluate>(
        book_.get(),
        thor_.get(),
        &tree_node_supplier_, &hash_map_,
        PatternEvaluator::Factory(evals_->data()),
        static_cast<u_int8_t>(i));
  }
  auto players = thor_->Players();
  auto tournaments = thor_->Tournaments();
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
}

// This runs in the main thread, so that we cannot update the output afterwards.
void Engine::Stop() {
  ++current_thread_;
  for (int i = 0; i < num_boards_to_evaluate_; ++i) {
    boards_to_evaluate_[i]->Stop();
  }
}

void Engine::UpdateBoardsToEvaluate(const Board& board, Annotations& annotations) {
  auto moves = GetAllMovesWithPass(board.Player(), board.Opponent());
  bool finished = true;

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

void Engine::EvaluateThor(const Sequence& sequence, const EvaluateParams& params, Annotations& annotations) {
  auto filters = params.thor_filters;
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
}

void Engine::Run(
    int current_thread, std::shared_ptr<std::future<void>> last_future,
    const Sequence& sequence, Board board, bool black_turn,
    const EvaluateParams& params) {
  int annotations_offset = 60 - board.NEmpties();
  last_future->get();
  Annotations& annotations = annotations_[annotations_offset];
  if (current_thread != current_thread_ && annotations.valid) {
    return;
  }
  if (board != last_board_ || annotations_offset != last_offset_ ||
      black_turn != last_black_turn_) {
    last_board_ = board;
    last_offset_ = annotations_offset;
    last_black_turn_ = black_turn;
    time_ = ElapsedTime();
    tree_node_supplier_.Reset();
    Square move = sequence.Size() > 0 ? sequence.LastMove() : kStartingPositionMove;
    if (annotations.move != move || annotations.black_turn != black_turn) {
      annotations.move = move;
      annotations.black_turn = black_turn;
      for (int i = annotations_offset+1; annotations_[i].valid; ++i) {
        annotations_[i].valid = false;
      }
    }
    UpdateBoardsToEvaluate(board, annotations);
    annotations.valid = false;

    if (params.use_book) {
      for (int i = 0; i < num_boards_to_evaluate_; ++i) {
        boards_to_evaluate_[i]->EvaluateBook();
      }
    }
    if (params.thor_filters.use_thor) {
      EvaluateThor(sequence, params, annotations);
    } else {
      annotations.num_thor_games = 0;
      annotations.num_example_thor_games = 0;
    }
  }

  int steps = num_boards_to_evaluate_;
  bool finished = num_boards_to_evaluate_ == 0;
  // We finish if num_boards_to_evaluate_ == 0, or if we cannot get another
  // board to work on.
  for (int i = 0; i < steps; ++i) {
    auto board_to_evaluate = NextBoardToEvaluate(params.delta);
    if (!board_to_evaluate) {
      finished = true;
      break;
    }
    board_to_evaluate->Evaluate(params, steps);
  }
  annotations.seconds = time_.Get();
  annotations.finished = finished;
  CalculateGlobalAnnotations(annotations);
  if (annotations_[annotations_offset + 1].valid) {
    UpdateFatherAnnotations(annotations_offset);
  }
  if (annotations_offset > 0) {
    UpdateFatherAnnotations(annotations_offset-1);
  }
  annotations.valid = true;
  if (current_thread == current_thread_) {
    update_annotations_(annotations_.data());
  }
}

void Engine::UpdateFatherAnnotations(int annotations_offset) {
  assert(annotations_offset >= 0 && annotations_offset < annotations_.size() - 1);
  Annotations& annotations = annotations_[annotations_offset];
  const Annotations& next_annotations = annotations_[annotations_offset + 1];
  for (int i = 0; i < annotations.num_moves; ++i) {
    MoveAnnotations& move_annotations = annotations.moves[i];
    if (move_annotations.square == next_annotations.move) {
      move_annotations.eval = (annotations.black_turn == next_annotations.black_turn ? -1 : 1) * next_annotations.eval;
      break;
    }
  }
  double old_eval = annotations.eval;
  CalculateGlobalAnnotations(annotations);
  if (annotations.eval != old_eval && annotations_offset > 0) {
    UpdateFatherAnnotations(annotations_offset - 1);
  }
}