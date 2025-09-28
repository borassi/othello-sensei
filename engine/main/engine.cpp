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

#include <cmath>
#include <future>

#include "engine.h"
#include "bindings.h"

constexpr double kFirstEvalTime = 0.01;
constexpr double kNextEvalTime = 0.1;

void BoardToEvaluate::EvaluateBook() {
  auto board_in_book = book_->Get(unique_);
  if (board_in_book) {
    for (auto& [state, finished] : states_finished_) {
      // If it has valid children, we only add the book descendants.
      state->SetAnnotations(*board_in_book, true, 0);
      finished = true;
    }
  }
}

void BoardToEvaluate::FinalizeEvaluation() {
  const TreeNode& first_position = *evaluator_.GetFirstPosition();
  for (auto& [state, finished] : states_finished_) {
    if (finished) {
      continue;
    }
    if (evaluator_.GetStatus() == SOLVED) {
      finished = true;
    }
    assert(!state->HasValidChildren() || state->IsBeforeModification());
    if (!state->IsValid() || state->GetNVisited() < first_position.GetNVisited()) {
      state->SetAnnotations(first_position, false, evaluator_.GetElapsedTime());
    } else {
      assert(state->IsValid());
    }
  }
}

void BoardToEvaluate::EvaluateFirst(const EvaluateParams& params) {
  if (Finished()) {
    return;
  }
  if (started_) {
    assert(evaluator_.GetFirstPosition() && evaluator_.GetFirstPosition()->ToBoard() == unique_);
    return;
  }
  started_ = true;
  evaluator_.Evaluate(
      unique_.Player(), unique_.Opponent(), (Eval) params.lower, (Eval) params.upper,
      params.max_positions, kFirstEvalTime, params.n_threads,
      params.approx);
  FinalizeEvaluation();
}

void BoardToEvaluate::Evaluate(const EvaluateParams& params) {
  assert(started_);
  assert(evaluator_.GetFirstPosition() && evaluator_.GetFirstPosition()->ToBoard() == unique_);
  evaluator_.ContinueEvaluate(params.max_positions, kNextEvalTime, params.n_threads);
  FinalizeEvaluation();
}

Engine::Engine(
    const std::string& evals_filepath,
    const std::string& book_filepath,
    const std::string& thor_filepath,
    const std::string& saved_games_filepath,
    UpdateAnnotations update_annotations,
    SetThorMetadata set_thor_metadata,
    SendMessage send_message) :
    update_annotations_(update_annotations),
    set_thor_metadata_(set_thor_metadata),
    send_message_(send_message),
    hash_map_(),
    tree_node_supplier_(),
    boards_to_evaluate_(),
    num_boards_to_evaluate_(0),
    current_thread_(0),
    current_future_(std::async(
        std::launch::async, &Engine::Initialize, this, evals_filepath,
        book_filepath, thor_filepath, saved_games_filepath
    ).share()),
    thor_metadata_(),
    last_first_state_(nullptr),
    last_state_(nullptr) {}

void Engine::Initialize(
    const std::string& evals_filepath,
    const std::string& book_filepath,
    const std::string& thor_filepath,
    const std::string& saved_games_filepath) {
  auto future_evals = std::async(std::launch::async, &Engine::BuildEvals, this, evals_filepath);
  auto future_book = std::async(std::launch::async, &Engine::BuildBook, this, book_filepath);
  auto future_thor = std::async(std::launch::async, &Engine::BuildThor, this, thor_filepath, saved_games_filepath);

  future_evals.get();
  future_book.get();
  future_thor.get();
  CreateBoardsToEvaluate();
}

void Engine::CreateBoardsToEvaluate() {
  for (int i = 0; i < kNumEvaluators; ++i) {
    boards_to_evaluate_[i] = std::make_unique<BoardToEvaluate>(
        book_.get(),
        &tree_node_supplier_, &hash_map_,
        PatternEvaluator::Factory(evals_->data()),
        static_cast<uint8_t>(i));
  }
}

void Engine::BuildThorSourceMetadata() {
  thor_sources_metadata_.clear();
  thor_sources_metadata_extended_.clear();
  for (auto& source : thor_->Sources()) {
    thor_sources_metadata_extended_.push_back(
        std::make_unique<ThorSourceMetadataExtended>(source, thor_->GetSource(source)));
    thor_sources_metadata_.push_back(thor_sources_metadata_extended_.back()->GetThorSourceMetadata());
  }
  thor_metadata_.sources = thor_sources_metadata_.data();
  thor_metadata_.num_sources = (int) thor_sources_metadata_.size();
  set_thor_metadata_(&thor_metadata_);
}

void Engine::BuildThor(const std::string& filepath, const std::string& saved_games_filepath) {
  thor_ = nullptr;
  if (filepath.empty()) {
    return;
  }
  thor_ = std::make_unique<Thor<GameGetterOnDisk>>(filepath, saved_games_filepath);
  BuildThorSourceMetadata();
}

// This runs in the main thread, so that we cannot update the output afterwards.
void Engine::Stop() {
  ++current_thread_;
}

void Engine::Start(EvaluationState* current_state,
           std::shared_ptr<EvaluationState>& first_state,
           const EvaluateParams& params, bool in_analysis) {
  // Propagate errors to the main thread.
  auto status = current_future_.wait_for(std::chrono::seconds(0));
  if (status == std::future_status::ready) {
    current_future_.get();
  }

  current_future_ = std::async(
      std::launch::async, &Engine::Run, this, ++current_thread_,
      current_future_, current_state, first_state, params, in_analysis).share();
}

namespace {

bool MustBeEvaluated(
    const EvaluationState& father,
    const EvaluationState& child,
    const EvaluateParams& params,
    bool in_analysis) {
  if (child.IsBeforeModification()) {
    return true;
  }
  if (!child.IsValid()) {
    assert(!child.HasValidChildren());
    return true;
  }
  if (child.IsSolved(-63, 63, false)) {
    return false;
  }
  // When watching an analyzed game, don't re-evaluate.
  if (father.InAnalysisLine() && !in_analysis && !params.reevaluate_during_analysis) {
    return false;
  }
  return !child.HasValidChildren();
}

} // namespace

void Engine::UpdateBoardsToEvaluate(EvaluationState& state, const EvaluateParams& params, bool in_analysis) {
  num_boards_to_evaluate_ = 0;

  for (EvaluationState* child : state.GetChildren()) {
    Board unique = child->ToBoard().Unique();
    // Create or get the BoardToEvaluate.
    BoardToEvaluate* board_to_evaluate = nullptr;
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
      assert(board_to_evaluate);
      child->SetDerived(true);
    }
    bool finished = !MustBeEvaluated(state, *child, params, in_analysis);
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
  if (state.IsModified() || thor_ == nullptr) {
    return;
  }
  auto filters = params.thor_filters;
  std::optional<Sequence> sequence_opt = state.GetSequence();
  if (!sequence_opt) {
    return;
  }
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

    for (int j = 0; source.selected_blacks[j] >= 0; ++j) {
      blacks.emplace_back(source.players[source.selected_blacks[j]]);
    }
    for (int j = 0; source.selected_whites[j] >= 0; ++j) {
      whites.emplace_back(source.players[source.selected_whites[j]]);
    }
    for (int j = 0; source.selected_tournaments[j] >= 0; ++j) {
      tournaments.emplace_back(source.tournaments[source.selected_tournaments[j]]);
    }

    if (include_all_sources || !blacks.empty() || !whites.empty() || !tournaments.empty()) {
      games.Merge(thor_->GetGames(
          source.name, *sequence_opt, filters.max_games, blacks, whites,
          tournaments, (short) filters.start_year, (short) filters.end_year));
    }
  }
  state.SetThor(games);

  for (int i = 0; i < num_boards_to_evaluate_; ++i) {
    boards_to_evaluate_[i]->UpdateWithThor(games.next_moves);
  }
}

// NOTE: Pass variables by value, to avoid concurrent modifications.
void Engine::Run(
    int current_thread, std::shared_future<void> last_future,
    EvaluationState* current_state, std::shared_ptr<EvaluationState> first_state,
    EvaluateParams params, bool in_analysis) {
  last_future.get();
  assert(current_state);
  // Useful if we are in analysis.
  if (current_thread == current_thread_ && in_analysis) {
    update_annotations_(current_thread, true, kNoMove);
  }
  AnalyzePosition(current_thread, current_state, first_state, params, in_analysis);

  int move = kNoMove;
  if (current_state->MustPlay(params.sensei_action)) {
    move = current_state->ChooseRandomMoveToPlay(params.error_play, params.max_error_move_play);
  }
  bool finished = true;
  for (int i = 0; i < num_boards_to_evaluate_; ++i) {
    finished = finished && boards_to_evaluate_[i]->Finished();
  }
  current_state->SetDuringAnalysis(in_analysis);
  if (current_thread == current_thread_) {
    update_annotations_(current_thread, params.sensei_action != SENSEI_EVALUATES || (finished && !in_analysis), move);
  }
}

namespace {
double MaxTime(SenseiAction action, double seconds_on_this_node, bool first_eval, bool in_analysis, const EvaluateParams& params) {
  if (in_analysis) {
    return params.max_time_analysis;
  }
  switch(action) {
    case SENSEI_INVALID_ACTION:
      assert(false);
    case SENSEI_EVALUATES:
      return first_eval ? params.max_time_first_eval : params.max_time_next_evals + seconds_on_this_node;
    case SENSEI_PLAYS_BLACK:
    case SENSEI_PLAYS_WHITE:
    case SENSEI_PLAYS_BOTH:
    case SENSEI_INACTIVE:
      return params.max_time_play;
    default:
      assert(false);
      return params.max_time_first_eval;
  }
}
}  // namespace

void Engine::AnalyzePosition(
    int current_thread, EvaluationState* current_state,
    const std::shared_ptr<EvaluationState>& first_state,
    const EvaluateParams& params, bool in_analysis) {
  bool first_eval = last_state_ != current_state || last_first_state_ != first_state || !current_state->HasValidChildren();
  double max_time = MaxTime(params.sensei_action, current_state->SecondsToEvaluateThisNode(), first_eval, in_analysis, params);
  ElapsedTime time;
  if (first_eval) {
    tree_node_supplier_.Reset();
    UpdateBoardsToEvaluate(*current_state, params, in_analysis);
    last_state_ = current_state;
    last_first_state_ = first_state;
  }
  // We always run EvaluateThor otherwise we might return early when we changed Thor parameters.
  EvaluateThor(params, *current_state);
  if (current_state->SecondsToEvaluateThisNode() > std::max(0.01, max_time - kNextEvalTime / 2)) {
    return;
  }
  if (first_eval) {
    current_state->ResetSecondsToEvaluateThisNode();
  }
  if (current_state->MaybeSetGameOver()) {
    return;
  }

  if (params.use_book) {
    for (int i = 0; i < num_boards_to_evaluate_; ++i) {
      boards_to_evaluate_[i]->EvaluateBook();
    }
  }
  // We finish if we cannot get another board to work on.
  for (int i = 0; i < num_boards_to_evaluate_; ++i) {
    BoardToEvaluate& board_to_evaluate = *boards_to_evaluate_[i];
    board_to_evaluate.EvaluateFirst(params);
  }
  for (
      auto* board_to_evaluate = NextBoardToEvaluate(params.delta);
      board_to_evaluate != nullptr &&
        current_state->SecondsToEvaluateThisNode() + time.Get() < max_time - kNextEvalTime / 2 &&
        current_thread_ == current_thread;
      board_to_evaluate = NextBoardToEvaluate(params.delta)) {
    board_to_evaluate->Evaluate(params);
  }
  current_state->UpdateFather();
  current_state->UpdateFathers();
  current_state->AddSecondsToEvaluateThisNode(time.Get());
}