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

#ifndef OTHELLO_SENSEI_ENGINE_H
#define OTHELLO_SENSEI_ENGINE_H

#include <list>

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

constexpr Square kStartingPositionMove = 64;

enum BoardToEvaluateState {
  STATE_NOT_STARTED = 0,
  STATE_STARTED = 1,
  STATE_FINISHED = 2,
};

class BoardToEvaluate {
 public:
  BoardToEvaluate(
      Book<>* book,
      Thor* thor,
      TreeNodeSupplier* tree_node_supplier,
      HashMap<kBitHashMap>* hash_map,
      EvaluatorFactory evaluator_depth_one_factory,
      u_int8_t index) :
      book_(book),
      evaluator_(tree_node_supplier, hash_map, evaluator_depth_one_factory, index) {}

  void Reset(Board unique) {
    unique_ = unique;
    annotations_.clear();
    state_ = STATE_NOT_STARTED;
  }

  bool HasMove(Square move) const {
    for (auto* annotation : annotations_) {
      if (annotation->move == move) {
        return true;
      }
    }
    return false;
  }

  void AddAnnotation(Annotations* annotation) {
    annotations_.push_back(annotation);
  }

  void UpdateWithThor(std::unordered_map<Square, int> next_moves) {
    int num_thor_games = 0;
    for (Annotations* annotation : annotations_) {
      num_thor_games += next_moves[annotation->move];
    }
    for (Annotations* annotation : annotations_) {
      annotation->num_thor_games = num_thor_games;
    }
  }

  void EvaluateBook();

  void Evaluate(EvaluateParams params);

  void EvaluateFirst(EvaluateParams params);

  void FinalizeEvaluation();

  void Stop() {
    if (stoppable_.load()) {
      evaluator_.Stop();
    }
  }

  BoardToEvaluateState State() const { return state_; }

  const Board& Unique() const { return unique_; }

  double Priority(double delta) const {
    if (State() == STATE_FINISHED) {
      return -std::numeric_limits<double>::infinity();
    } else if (State() == STATE_NOT_STARTED) {
      return std::numeric_limits<double>::infinity();
    }
    auto first_position = evaluator_.GetFirstPosition();
    assert(first_position);
    return pow(2, -first_position->GetEval() / delta) / first_position->GetNVisited();
  }

 private:
  std::atomic_bool stoppable_;
  Board unique_;
  EvaluatorDerivative evaluator_;
  std::vector<Annotations*> annotations_;
  BoardToEvaluateState state_;
  Book<>* book_;

  bool AllAnnotationsFinished() const {
    for (Annotations* annotation : annotations_) {
      if (!annotation->finished) {
        return false;
      }
    }
    return true;
  }
};

class ThorSourceMetadataExtended {
 public:
  ThorSourceMetadataExtended(const ThorSourceMetadataExtended&) = delete;
  ThorSourceMetadataExtended(const std::string& name, const std::vector<std::string>& players, const std::vector<std::string>& tournaments) : name_(name) {
    players_.reserve(players.size());
    for (const std::string& player : players) {
      players_.push_back(player.c_str());
      selected_blacks_.push_back(-1);
      selected_whites_.push_back(-1);
    }
    tournaments_.reserve(tournaments.size());
    for (const std::string& tournament : tournaments) {
      tournaments_.push_back(tournament.c_str());
      selected_tournaments_.push_back(-1);
    }
    thor_source_metadata_.active = true;
    thor_source_metadata_.name = name_.c_str();
    thor_source_metadata_.players = players_.data();
    thor_source_metadata_.num_players = players_.size();
    thor_source_metadata_.tournaments = tournaments_.data();
    thor_source_metadata_.num_tournaments = tournaments_.size();
    thor_source_metadata_.selected_blacks = selected_blacks_.data();
    thor_source_metadata_.selected_whites = selected_whites_.data();
    thor_source_metadata_.selected_tournaments = selected_tournaments_.data();
  }

  ThorSourceMetadata* GetThorSourceMetadata() { return &thor_source_metadata_; }

 private:
  ThorSourceMetadata thor_source_metadata_;
  std::string name_;
  std::vector<const char*> players_;
  std::vector<const char*> tournaments_;
  std::vector<int> selected_blacks_;
  std::vector<int> selected_whites_;
  std::vector<int> selected_tournaments_;
};

class Engine {
 public:
  Engine(
      const std::string& evals_filepath,
      const std::string& book_filepath,
      const std::string& thor_filepath,
      UpdateAnnotations update_annotations);

  void Start(State* current_state, std::shared_ptr<State>& first_state,
             const EvaluateParams& params) {
    current_future_ = std::make_shared<std::future<void>>(std::async(
        std::launch::async, &Engine::Run, this, current_thread_.load(),
        current_future_, current_state, first_state, params));
  }

  void Stop();

  void StartAnalysis(std::shared_ptr<State>& first_state, const EvaluateParams& params) {
    current_future_ = std::make_shared<std::future<void>>(std::async(
        std::launch::async, &Engine::RunAnalysis, this, current_thread_.load(),
        current_future_, first_state, params));
  }

  ThorMetadata* GetThorMetadata() {
    Stop();
    current_future_->wait();
    return &thor_metadata_;
  }

  uint32_t CurrentThread() { return current_thread_.load(); }

 private:
  static constexpr int kNumEvaluators = 60;

  UpdateAnnotations update_annotations_;

  std::unique_ptr<EvalType> evals_;
  HashMap<kBitHashMap> hash_map_;
  TreeNodeSupplier tree_node_supplier_;
  std::array<std::unique_ptr<BoardToEvaluate>, kNumEvaluators> boards_to_evaluate_;
  int num_boards_to_evaluate_;

  ElapsedTime time_;

  std::atomic_uint32_t current_thread_;
  std::shared_ptr<std::future<void>> current_future_;

  std::unique_ptr<Book<kBookVersion>> book_;

  std::vector<std::unique_ptr<ThorSourceMetadataExtended>> thor_sources_metadata_extended_;
  std::vector<ThorSourceMetadata*> thor_sources_metadata_;
  ThorMetadata thor_metadata_;

  std::unique_ptr<Thor> thor_;

  std::shared_ptr<State> last_first_state_;
  State* last_state_;

  void EvaluateThor(const EvaluateParams& params, State& state);

  void Initialize(
      const std::string& evals_filepath,
      const std::string& book_filepath,
      const std::string& thor_filepath);

  void UpdateBoardsToEvaluate(const State& state, bool in_analysis);

  BoardToEvaluate* NextBoardToEvaluate(double delta) {
    double highestPriority = -std::numeric_limits<double>::infinity();
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
      int current_thread, std::shared_ptr<std::future<void>> last_future,
      State* current_state, std::shared_ptr<State> first_state,
      EvaluateParams params);

  // NOTE: Pass variables by value, to avoid concurrent modifications.
  void RunAnalysis(
      int current_thread, std::shared_ptr<std::future<void>> last_future,
      std::shared_ptr<State> first_state, EvaluateParams params);

  void RunUpdateAnnotations(int current_thread, int current_state, bool finished);

  void AnalyzePosition(
      int current_thread, State* current_state,
      const std::shared_ptr<State>& first_state, const EvaluateParams& params,
      bool in_analysis);
};

#endif // OTHELLO_SENSEI_ENGINE_H