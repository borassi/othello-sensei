/*
 * Copyright 2023-2025 Michele Borassi
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

#ifndef OTHELLO_SENSEI_ENGINE_H
#define OTHELLO_SENSEI_ENGINE_H

#include <list>
#include <chrono>
#include <thread>
#include "bindings.h"
#include "state.h"

#include "../board/board.h"
#include "../board/get_flip.h"
#include "../board/get_moves.h"
#include "../book/book.h"
#include "../evaluatederivative/evaluator_derivative.h"
#include "../evaluatederivative/tree_node.h"
#include "../evaluatedepthone/pattern_evaluator.h"
#include "../thor/thor.h"
#include "../utils/misc.h"

class BoardToEvaluate {
 public:
  BoardToEvaluate(
      Book<>* book,
      TreeNodeSupplier* tree_node_supplier,
      HashMap<kBitHashMap>* hash_map,
      EvaluatorFactory evaluator_depth_one_factory,
      uint8_t index) :
      stoppable_(false),
      started_(false),
      evaluator_(tree_node_supplier, hash_map, evaluator_depth_one_factory, index),
      book_(book) {}

  bool Finished() const {
    for (auto& [_, finished] : states_finished_) {
      if (!finished) {
        return false;
      }
    }
    return true;
  }

  void Reset(Board unique) {
    unique_ = unique;
    started_ = false;
    states_finished_.clear();
  }

  bool HasMove(Square move) const {
    for (auto& [state, _] : states_finished_) {
      if (state->Move() == move) {
        return true;
      }
    }
    return false;
  }

  void AddState(EvaluationState* state, bool finished) {
    states_finished_.emplace_back(state, finished);
  }

  void UpdateWithThor(std::unordered_map<Square, int> next_moves) {
    int num_thor_games = 0;
    for (auto& [state, _] : states_finished_) {
      num_thor_games += next_moves[state->Move()];
    }
    for (auto& [state, _] : states_finished_) {
      state->SetNumThorGames(num_thor_games);
    }
  }

  void EvaluateBook();

  void Evaluate(const EvaluateParams& params);

  void EvaluateFirst(const EvaluateParams& params);

  void FinalizeEvaluation();

  const Board& Unique() const { return unique_; }

  double Priority(double delta) const {
    assert(!states_finished_.empty());
    if (Finished()) {
      return -DBL_MAX;
    }
    auto first_position = evaluator_.GetFirstPosition();
    assert(first_position);
    return pow(2, -first_position->GetEval() / delta) / (double) first_position->GetNVisited();
  }

 private:
  std::atomic_bool stoppable_;
  Board unique_;
  bool started_;
  EvaluatorDerivative evaluator_;
  std::vector<std::pair<EvaluationState*, bool>> states_finished_;
  const Book<>* book_;

  bool Valid() const {
    for (auto& [state, _] : states_finished_) {
      if (!state->IsValid()) {
        return false;
      }
    }
    return true;
  }
};

class ThorSourceMetadataExtended {
 public:
  ThorSourceMetadataExtended(const ThorSourceMetadataExtended&) = delete;
  ThorSourceMetadataExtended(const std::string& name, const GenericSource& source) :
      thor_source_metadata_(), name_(name), folder_(source.GetFolder()) {
    const auto& players = source.Players();
    selected_blacks_.reserve(players.size() + 1);
    selected_whites_.reserve(players.size() + 1);
    for (const std::string& player: players) {
      players_.push_back(player.c_str());
      selected_blacks_.push_back(-1);
      selected_whites_.push_back(-1);
    }
    selected_blacks_.push_back(-1);
    selected_whites_.push_back(-1);
    const auto& tournaments = source.Tournaments();
    for (const std::string& tournament: tournaments) {
      tournaments_.push_back(tournament.c_str());
      selected_tournaments_.push_back(-1);
    }
    selected_tournaments_.push_back(-1);
    thor_source_metadata_.active = true;
    thor_source_metadata_.name = name_.c_str();
    thor_source_metadata_.folder = folder_.c_str();
    thor_source_metadata_.players = players_.data();
    thor_source_metadata_.num_players = (int) players_.size();
    thor_source_metadata_.tournaments = tournaments_.data();
    thor_source_metadata_.num_tournaments = (int) tournaments_.size();
    thor_source_metadata_.selected_blacks = selected_blacks_.data();
    thor_source_metadata_.selected_whites = selected_whites_.data();
    thor_source_metadata_.selected_tournaments = selected_tournaments_.data();
    thor_source_metadata_.is_saved_games_folder = source.GetType() == SOURCE_TYPE_SAVED_GAMES;
  }

  ThorSourceMetadata* GetThorSourceMetadata() { return &thor_source_metadata_; }

 private:
  ThorSourceMetadata thor_source_metadata_;
  std::string name_;
  std::string folder_;
  std::vector<const char*> players_;
  std::vector<const char*> tournaments_;
  std::vector<int> selected_blacks_;
  std::vector<int> selected_whites_;
  std::vector<int> selected_tournaments_;
};

bool IncludeAllSources(ThorMetadata thor_metadata);

using namespace std::chrono_literals;

class Engine {
 public:
  Engine(
      const std::string& evals_filepath,
      const std::string& book_filepath,
      const std::string& thor_filepath,
      const std::string& saved_games_filepath,
      UpdateAnnotations update_annotations,
      SetThorMetadata set_thor_metadata,
      SendMessage send_message);

  void Start(EvaluationState* current_state,
             std::shared_ptr<EvaluationState>& first_state,
             const EvaluateParams& params, bool in_analysis);

  void StopBlocking() {
    Stop();
    current_future_.wait();
  }

  void RunUpdateAnnotations() {
    StopBlocking();
    update_annotations_(current_thread_, true, kNoMove);
  }

  void Stop();

  void StartSetFileSources(const std::vector<std::string>& sources) {
    current_future_ = std::async(
        std::launch::async, &Engine::RunSetFileSources, this, sources, current_future_).share();
  }

  bool ReloadSource(const std::string& file) {
    Stop();
    current_future_.wait();
    bool success = thor_->ReloadSource(file);
    BuildThorSourceMetadata();
    return success;
  }

  uint32_t CurrentThread() { return current_thread_.load(); }

 private:
  static constexpr int kNumEvaluators = 60;

  UpdateAnnotations update_annotations_;
  SetThorMetadata set_thor_metadata_;
  [[maybe_unused]] SendMessage send_message_;

  std::unique_ptr<EvalType> evals_;
  HashMap<kBitHashMap> hash_map_;
  TreeNodeSupplier tree_node_supplier_;
  std::array<std::unique_ptr<BoardToEvaluate>, kNumEvaluators> boards_to_evaluate_;
  int num_boards_to_evaluate_;

  std::atomic_uint32_t current_thread_;
  std::shared_future<void> current_future_;

  std::unique_ptr<Book<kBookVersion>> book_;

  std::vector<std::unique_ptr<ThorSourceMetadataExtended>> thor_sources_metadata_extended_;
  std::vector<ThorSourceMetadata*> thor_sources_metadata_;
  ThorMetadata thor_metadata_;

  std::unique_ptr<Thor<GameGetterOnDisk>> thor_;

  std::shared_ptr<EvaluationState> last_first_state_;
  EvaluationState* last_state_;

  void RunSetFileSources(const std::vector<std::string>& sources, std::shared_future<void> last_future) {
    last_future.get();
    thor_->SetFileSources(sources);
    BuildThorSourceMetadata();
  }

  void BuildThorSourceMetadata();

  void EvaluateThor(const EvaluateParams& params, EvaluationState& state);

  void Initialize(
      const std::string& evals_filepath,
      const std::string& book_filepath,
      const std::string& thor_filepath,
      const std::string& saved_games_filepath);

  void BuildEvals(const std::string& filepath) {
    evals_ = std::make_unique<EvalType>(LoadEvals(filepath));
  }
  void BuildBook(const std::string& filepath) {
    book_ = std::make_unique<Book<kBookVersion>>(filepath);
  }
  void CreateBoardsToEvaluate();
  void BuildThor(const std::string& filepath, const std::string& saved_games_filepath);

  void UpdateBoardsToEvaluate(EvaluationState& state, const EvaluateParams& params, bool in_analysis);

  BoardToEvaluate* NextBoardToEvaluate(double delta) {
    double highestPriority = -DBL_MAX;
    BoardToEvaluate* result = nullptr;

    for (int i = 0; i < num_boards_to_evaluate_; ++i) {
      BoardToEvaluate& b = *boards_to_evaluate_[i];
      double priority = b.Priority(delta);
      if (priority > highestPriority) {
        highestPriority = priority;
        result = &b;
      }
    }
    return result;
  }

  // NOTE: Pass variables by value, to avoid concurrent modifications.
  void Run(
      int current_thread, std::shared_future<void> last_future,
      EvaluationState* current_state, std::shared_ptr<EvaluationState> first_state,
      EvaluateParams params, bool in_analysis);

  void AnalyzePosition(
      int current_thread, EvaluationState* current_state,
      const std::shared_ptr<EvaluationState>& first_state, const EvaluateParams& params,
      bool in_analysis);
};

#endif // OTHELLO_SENSEI_ENGINE_H