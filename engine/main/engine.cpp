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

#include<math.h>

#include "main.h"
#include "bindings.h"

constexpr double kFirstEvalTime = 0.01;
constexpr double kNextEvalTime = 0.1;

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
  annotation.finished |= book || node.IsSolved();
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

void SetFatherAnnotations(State& state) {
  if (state.provenance == GAME_OVER || !state.HasValidChildren()) {
    if (state.father) {
      SetFatherAnnotations(*((State*) state.father));
    }
    return;
  }
  state.eval = kLessThenMinEval;
  state.leaf_eval = kLessThenMinEval;
  state.median_eval = kLessThenMinEval;
  state.provenance = CHILD_BOOK;
  state.seconds = 0;
  state.missing = NAN;
//  state.prob_lower_eval = 0;
//  state.prob_upper_eval = 0;
//  state.proof_number_lower = 0;
//  state.disproof_number_upper = 0;
  state.lower = kLessThenMinEval;
  state.upper = kLessThenMinEval;
//  state.weak_lower = score;
//  state.weak_upper = 0;
  state.descendants = 0;
  state.descendants_no_book = 0;
  state.finished = true;
  state.valid = true;

  for (State* child = (State*) state.first_child; child != nullptr; child = (State*) child->next_sibling) {
    int eval_sign = child->black_turn == state.black_turn ? 1 : -1;
    state.eval = std::max(state.eval, eval_sign * child->eval);
    state.median_eval = std::max(state.median_eval, eval_sign * child->median_eval);
    state.leaf_eval = std::max(state.leaf_eval, eval_sign * child->leaf_eval);
    state.lower = MaxEval(state.lower, eval_sign * eval_sign == 1 ? child->lower : child->upper);
    state.upper = MaxEval(state.upper, eval_sign * eval_sign == 1 ? child->upper : child->lower);
    if (!child->derived) {
      state.seconds += child->seconds;
      state.descendants += child->descendants;
      state.descendants_no_book += child->descendants_no_book;
    }
    state.finished = state.finished && child->finished;
    state.valid = state.valid && child->valid;
    if (!child->InBook()) {
      state.provenance = CHILD_EVALUATE;
    }
  }
  if (state.father) {
    SetFatherAnnotations(*((State*) state.father));
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

void BoardToEvaluate::FinalizeEvaluation() {
  for (Annotations* annotation : annotations_) {
    AnnotationsSet(*annotation, *evaluator_.GetFirstPosition(), false, evaluator_.GetElapsedTime());
  }
  if (evaluator_.GetStatus() == SOLVED) {
    state_ = STATE_FINISHED;
  } else {
    state_ = STATE_STARTED;
  }
}

void BoardToEvaluate::EvaluateFirst(EvaluateParams params) {
  assert(state_ == STATE_NOT_STARTED);
  if (AllAnnotationsFinished()) {
    state_ = STATE_FINISHED;
    return;
  }
  stoppable_ = false;
  evaluator_.Evaluate(
      unique_.Player(), unique_.Opponent(), params.lower, params.upper,
      params.max_positions, kFirstEvalTime, params.n_threads,
      params.approx);
  FinalizeEvaluation();
  stoppable_ = true;
}

void BoardToEvaluate::Evaluate(EvaluateParams params) {
  assert(state_ == STATE_STARTED);
  evaluator_.ContinueEvaluate(params.max_positions, kNextEvalTime, params.n_threads);
  FinalizeEvaluation();
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

void Engine::UpdateBoardsToEvaluate(const State& state, bool in_analysis) {
  Board board = state.GetBoard();
  auto moves = GetAllMovesWithPass(board.Player(), board.Opponent());
  bool finished = true;

  num_boards_to_evaluate_ = 0;

  for (State* child : state.GetChildren()) {
    if (child->valid && !in_analysis) {
      child->finished = true;
    } else {
      child->finished = false;
    }
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
    if (!source.active) {
      continue;
    }
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
  time_ = ElapsedTime();
  last_future->get();
  assert(current_state);
  if (current_thread != current_thread_ && current_state->valid && current_state->HasValidChildren()) {
    return;
  }
  AnalyzePosition(current_thread, current_state, first_state, params, false);
}

void Engine::AnalyzePosition(
    int current_thread, State* current_state,
    const std::shared_ptr<State>& first_state,
    const EvaluateParams& params, bool in_analysis) {
  bool first_eval = false;
  if (last_state_ != current_state || last_first_state_ != first_state) {
    first_eval = true;
    last_state_ = current_state;
    last_first_state_ = first_state;
    time_ = ElapsedTime();
    tree_node_supplier_.Reset();
    UpdateBoardsToEvaluate(*current_state, in_analysis);
  }
  EvaluateThor(params, *current_state);
  if (current_state->first_child == nullptr) {
    assert(num_boards_to_evaluate_ == 0);
    Board board = current_state->GetBoard();
    SetAnnotationsGameOver(*current_state);
  } else {
    if (params.use_book) {
      for (int i = 0; i < num_boards_to_evaluate_; ++i) {
        boards_to_evaluate_[i]->EvaluateBook();
      }
    }
    int steps = num_boards_to_evaluate_;
    // We finish if we cannot get another board to work on.
    double max_time;
    if (in_analysis) {
      max_time = params.max_time_analysis;
    } else if (first_eval) {
      max_time = params.max_time_first_eval;
    } else {
      max_time = params.max_time_next_evals;
    }
    for (int i = 0; i < num_boards_to_evaluate_; ++i) {
      BoardToEvaluate& board_to_evaluate = *boards_to_evaluate_[i];
      if (board_to_evaluate.State() == STATE_NOT_STARTED) {
        board_to_evaluate.EvaluateFirst(params);
      }
    }
    for (
        auto* board_to_evaluate = NextBoardToEvaluate(params.delta);
        board_to_evaluate != nullptr &&
          time_.Get() < max_time - kNextEvalTime / 2 &&
          current_thread_ == current_thread;
        board_to_evaluate = NextBoardToEvaluate(params.delta)) {
      board_to_evaluate->Evaluate(params);
    }
  }
  SetFatherAnnotations(*current_state);
  current_state->analyzed |= in_analysis;

  if (current_thread == current_thread_) {
    update_annotations_(current_thread, in_analysis);
  }
}

void Engine::RunAnalysis(
    int current_thread, std::shared_ptr<std::future<void>> last_future,
    std::shared_ptr<State> first_state, EvaluateParams params) {
  last_future->get();
  if (current_thread != current_thread_) {
    return;
  }
  for (State* state = first_state.get(); state != nullptr; state = (State*) state->next_state_in_analysis) {
    time_ = ElapsedTime();
    AnalyzePosition(current_thread, state, first_state, params, true);
    if (current_thread != current_thread_) {
      return;
    }
  }
}