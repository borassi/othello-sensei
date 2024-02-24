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
void AnnotationsReset(Annotations& annotation) {
  annotation.move = kNoSquare;
  annotation.lower = kLessThenMinEval;
  annotation.upper = kLessThenMinEval;
}

void AnnotationsSet(Annotations& annotation, const Node& node, bool book, double seconds) {
  annotation.eval = node.GetEval();
  annotation.leaf_eval = node.LeafEval();
  annotation.median_eval = node.GetPercentileLower(0.5) - 1;
  annotation.provenance = book ? BOOK : EVALUATE;
  annotation.seconds = seconds;
  annotation.prob_lower_eval = node.SolveProbabilityLower(-63);
  annotation.prob_upper_eval = node.SolveProbabilityUpper(63);
  annotation.proof_number_lower = annotation.median_eval == -64 ? 0 : node.RemainingWork(-63, annotation.median_eval - 1);
  annotation.disproof_number_upper = annotation.median_eval == 64 ? 0 : node.RemainingWork(annotation.median_eval + 1, 63);
  annotation.lower = node.Lower();
  annotation.upper = node.Upper();
  annotation.weak_lower = node.WeakLower();
  annotation.weak_upper = node.WeakUpper();
  annotation.descendants = node.GetNVisited();
  annotation.descendants_no_book = book ? 0 : annotation.descendants;
  annotation.missing = node.IsSolved() ? 0 : node.RemainingWork(-63, 63);
  annotation.finished = book || node.IsSolved();
  annotation.valid = true;
}

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

void SetAnnotationsGameOver(State& state) {
  Board board = state.GetBoard();
  state.eval = GetEvaluationGameOver(board.Player(), board.Opponent());
  state.leaf_eval = state.eval;
  state.median_eval = state.eval;
  state.provenance = GAME_OVER;
  state.seconds = 0;
  state.prob_lower_eval = 0;
  state.prob_upper_eval = 0;
  state.proof_number_lower = 0;
  state.disproof_number_upper = 0;
  state.lower = state.eval;
  state.upper = state.eval;
  state.weak_lower = state.eval;
  state.weak_upper = state.eval;
  state.descendants = 0;
  state.descendants_no_book = 0;
  state.finished = true;
  state.valid = true;
}

void SetFatherAnnotations(Annotations& annotation) {
  if (annotation.provenance == GAME_OVER) {
    SetFatherAnnotations(*annotation.father);
    return;
  }
  annotation.eval = kLessThenMinEval;
  annotation.leaf_eval = kLessThenMinEval;
  annotation.median_eval = kLessThenMinEval;
  annotation.provenance = CHILD_EVALUATE;
  annotation.seconds = 0;
//  annotation.prob_lower_eval = 0;
//  annotation.prob_upper_eval = 0;
//  annotation.proof_number_lower = 0;
//  annotation.disproof_number_upper = 0;
  annotation.lower = kLessThenMinEval;
  annotation.upper = kLessThenMinEval;
//  annotation.weak_lower = score;
//  annotation.weak_upper = 0;
  annotation.descendants = 0;
  annotation.descendants_no_book = 0;
  annotation.finished = true;
  annotation.valid = true;

  for (Annotations* child = annotation.first_child; child != nullptr; child = child->next_sibling) {
    int eval_sign = child->black_turn == annotation.black_turn ? 1 : -1;
    annotation.eval = std::max(annotation.eval, eval_sign * child->eval);
    annotation.median_eval = std::max(annotation.median_eval, eval_sign * child->median_eval);
    annotation.leaf_eval = std::max(annotation.leaf_eval, eval_sign * child->leaf_eval);
    annotation.lower = MaxEval(annotation.lower, eval_sign * eval_sign == 1 ? child->lower : child->upper);
    annotation.upper = MaxEval(annotation.upper, eval_sign * eval_sign == 1 ? child->upper : child->lower);
    if (!child->derived) {
      annotation.seconds += child->seconds;
      annotation.descendants += child->descendants;
      annotation.descendants_no_book += child->descendants_no_book;
    }
    annotation.finished = annotation.finished && child->finished;
    annotation.valid = annotation.valid && child->valid;
  }
  if (annotation.father != nullptr) {
    SetFatherAnnotations(*annotation.father);
  }
}
} // anonymous_namespace

void BoardToEvaluate::EvaluateBook() {
  auto board_in_book = book_->Get(unique_);
  if (board_in_book) {
    for (auto* annotation : annotations_) {
      AnnotationsSet(*annotation, *board_in_book, true, 0);
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
  for (Annotations* annotation : annotations_) {
    AnnotationsSet(*annotation, *evaluator_.GetFirstPosition(), false, evaluator_.GetElapsedTime());
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
    last_state_(nullptr),
    last_first_state_(nullptr),
    current_thread_(0),
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

void Engine::UpdateBoardsToEvaluate(const State& state) {
  Board board = state.GetBoard();
  auto moves = GetAllMovesWithPass(board.Player(), board.Opponent());
  bool finished = true;

  num_boards_to_evaluate_ = 0;

  for (State* child : state.GetChildren()) {
    Board unique = child->GetBoard().Unique();
    // Create or get the BoardToEvaluate.
    BoardToEvaluate* board_to_evaluate;
    bool found = false;
    for (int i = 0; i < num_boards_to_evaluate_; ++i) {
      BoardToEvaluate* b = boards_to_evaluate_[i].get();
      if (b->Unique() == unique) {
        board_to_evaluate = b;
        found = true;
        break;
      }
    }
    if (!found) {
      board_to_evaluate = boards_to_evaluate_[num_boards_to_evaluate_++].get();
      board_to_evaluate->Reset(unique);
    } else {
      child->derived = true;
    }
    board_to_evaluate->AddAnnotation((Annotations*) child);
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

void Engine::EvaluateThor(const EvaluateParams& params, State& state) {
  auto filters = params.thor_filters;
  Sequence sequence = state.GetSequence();
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

  state.num_thor_games = games.num_games;
  state.num_example_thor_games = games.examples.size();
  state.example_thor_games = (ThorGame*) realloc(state.example_thor_games, state.num_example_thor_games * sizeof(ThorGame));
  for (int i = 0; i < state.num_example_thor_games; ++i) {
    assert(games.examples[i]->Moves().Size() >= sequence.Size());
    ThorGameSet(*games.examples[i], state.example_thor_games[i], sequence);
  }

  for (int i = 0; i < num_boards_to_evaluate_; ++i) {
    boards_to_evaluate_[i]->UpdateWithThor(games.next_moves);
  }
}

// NOTE: Pass variables by value, to avoid concurrent modifications.
void Engine::Run(
    int current_thread, std::shared_ptr<std::future<void>> last_future,
    State* current_state, std::shared_ptr<State> first_state,
    EvaluateParams params) {
  last_future->get();
  assert(current_state);
  if (current_thread != current_thread_ && current_state->valid && current_state->HasValidChildren()) {
    return;
  }
  if (last_state_ != current_state || last_first_state_ != first_state) {
    last_state_ = current_state;
    last_first_state_ = first_state;
    time_ = ElapsedTime();
    tree_node_supplier_.Reset();
    UpdateBoardsToEvaluate(*current_state);

    if (params.use_book) {
      for (int i = 0; i < num_boards_to_evaluate_; ++i) {
        boards_to_evaluate_[i]->EvaluateBook();
      }
    }
    EvaluateThor(params, *current_state);
  }
  if (num_boards_to_evaluate_ == 0) {
    Board board = current_state->GetBoard();
    SetAnnotationsGameOver(*current_state);
  } else {
    int steps = num_boards_to_evaluate_;
    // We finish if we cannot get another board to work on.
    for (int i = 0; i < steps; ++i) {
      auto board_to_evaluate = NextBoardToEvaluate(params.delta);
      if (!board_to_evaluate || (current_thread_ != current_thread && board_to_evaluate->State() != STATE_NOT_STARTED)) {
        break;
      }
      board_to_evaluate->Evaluate(params, steps);
    }
  }
  SetFatherAnnotations(*current_state);

  if (current_thread == current_thread_) {
    // TODO: what if we delete first_state before we're done with the update?
    update_annotations_(current_state, first_state.get());
  }
}