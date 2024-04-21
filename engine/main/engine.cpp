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

void BoardToEvaluate::EvaluateBook() {
  book_->ReloadSizes();
  auto board_in_book = book_->Get(unique_);
  if (board_in_book) {
    for (auto& [state, finished] : states_finished_) {
      if (!finished) {
        state->SetAnnotations(*board_in_book, true, 0);
      }
      finished = true;
    }
  }
}

void BoardToEvaluate::FinalizeEvaluation() {
  const TreeNode& first_position = *evaluator_.GetFirstPosition();
  for (auto& [state, finished] : states_finished_) {
    state->SetAnnotations(first_position, false, evaluator_.GetElapsedTime());
    if (evaluator_.GetStatus() == SOLVED) {
      finished = true;
    }
  }
}

void BoardToEvaluate::EvaluateFirst(const EvaluateParams& params) {
  if (Valid() || Finished()) {
    return;
  }
  evaluator_.Evaluate(
      unique_.Player(), unique_.Opponent(), params.lower, params.upper,
      params.max_positions, kFirstEvalTime, params.n_threads,
      params.approx);
  FinalizeEvaluation();
}

void BoardToEvaluate::Evaluate(const EvaluateParams& params) {
  evaluator_.ContinueEvaluate(params.max_positions, kNextEvalTime, params.n_threads);
  FinalizeEvaluation();
}

Engine::Engine(
    const std::string& evals_filepath,
    const std::string& book_filepath,
    const std::string& thor_filepath,
    Main& main,
    UpdateAnnotations update_annotations) :
    update_annotations_(update_annotations),
    main_(main),
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
}

namespace {

bool MustBeEvaluated(
    const EvaluationState& father,
    const EvaluationState& child,
    const EvaluateParams& params,
    bool in_analysis) {
  if (!child.IsValid()) {
    assert(!child.HasValidChildren());
    return true;
  }
  // When analyzing, always re-evaluate everything.
  if (in_analysis) {
    return &child != father.NextStateInAnalysis();
  }
  // When watching an analyzed game, don't re-evaluate.
  if (father.InAnalysisLine()) {
    return false;
  }
  return !child.HasValidChildren();
}

bool operator==(const EvaluateParams& lhs, const EvaluateParams& rhs) {
  return std::forward_as_tuple(
      lhs.lower,
      lhs.upper,
      lhs.approx,
      lhs.use_book,
      lhs.thor_filters.max_games,
      lhs.thor_filters.start_year,
      lhs.thor_filters.end_year
  ) == std::forward_as_tuple(
      rhs.lower,
      rhs.upper,
      rhs.approx,
      rhs.use_book,
      rhs.thor_filters.max_games,
      rhs.thor_filters.start_year,
      rhs.thor_filters.end_year
  );
}
bool operator!=(const EvaluateParams& lhs, const EvaluateParams& rhs) {
  return !(lhs == rhs);
}
} // namespace

void Engine::UpdateBoardsToEvaluate(EvaluationState& state, const EvaluateParams& params, bool in_analysis) {
  Board board = state.ToBoard();
  num_boards_to_evaluate_ = 0;

  for (EvaluationState* child : state.GetChildren()) {
    Board unique = child->ToBoard().Unique();
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
      child->SetDerived(true);
    }
    bool finished =
        !MustBeEvaluated(state, *child, params, in_analysis) &&
        last_params_ == params;
    if (!finished) {
      child->InvalidateRecursive();
    }
    board_to_evaluate->AddState(child, finished);
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

void Engine::EvaluateThor(const EvaluateParams& params, EvaluationState& state) {
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
  state.SetThor(games);

  for (int i = 0; i < num_boards_to_evaluate_; ++i) {
    boards_to_evaluate_[i]->UpdateWithThor(games.next_moves);
  }
}

// NOTE: Pass variables by value, to avoid concurrent modifications.
void Engine::Run(
    int current_thread, std::shared_ptr<std::future<void>> last_future,
    EvaluationState* current_state, std::shared_ptr<EvaluationState> first_state,
    EvaluateParams params, bool in_analysis) {
  time_ = ElapsedTime();
  last_future->get();
  assert(current_state);
  if (in_analysis && current_thread == current_thread_) {
    update_annotations_(current_thread, true);
  }
  AnalyzePosition(current_thread, current_state, first_state, params, in_analysis);

  current_state->UpdateFathers();
  bool finished = true;
  for (int i = 0; i < num_boards_to_evaluate_; ++i) {
    finished = finished && boards_to_evaluate_[i]->Finished();
  }
  current_state->SetDuringAnalysis(in_analysis);
  if (current_thread == current_thread_) {
    update_annotations_(current_thread, finished && !in_analysis);
  }
}

void Engine::AnalyzePosition(
    int current_thread, EvaluationState* current_state,
    const std::shared_ptr<EvaluationState>& first_state,
    const EvaluateParams& params, bool in_analysis) {
  bool first_eval = false;
  if (last_state_ != current_state || last_first_state_ != first_state || last_params_ != params) {
    first_eval = true;
    time_ = ElapsedTime();
    tree_node_supplier_.Reset();
    UpdateBoardsToEvaluate(*current_state, params, in_analysis);
    last_state_ = current_state;
    last_first_state_ = first_state;
    last_params_ = params;
  }
  EvaluateThor(params, *current_state);

  if (current_state->MaybeSetGameOver()) {
    return;
  }

  if (params.use_book) {
    for (int i = 0; i < num_boards_to_evaluate_; ++i) {
      boards_to_evaluate_[i]->EvaluateBook();
    }
  }
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
    board_to_evaluate.EvaluateFirst(params);
  }
  for (
      auto* board_to_evaluate = NextBoardToEvaluate(params.delta);
      board_to_evaluate != nullptr &&
        time_.Get() < max_time - kNextEvalTime / 2 &&
        current_thread_ == current_thread;
      board_to_evaluate = NextBoardToEvaluate(params.delta)) {
    board_to_evaluate->Evaluate(params);
  }
  current_state->UpdateFather();
}