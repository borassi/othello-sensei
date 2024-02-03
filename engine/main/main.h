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

#ifndef OTHELLO_SENSEI_MAIN_H
#define OTHELLO_SENSEI_MAIN_H

#include <list>

#include "bindings.h"

#include "../board/board.h"
#include "../board/get_flip.h"
#include "../board/get_moves.h"
#include "../book/book.h"
#include "../evaluatederivative/evaluator_derivative.h"
#include "../evaluatederivative/tree_node.h"
#include "../evaluatedepthone/pattern_evaluator.h"
#include "../thor/thor.h"
#include "../utils/misc.h"

class State {
 public:
  State(Square move, Board board, bool black_turn) : move_(move), board_(board), black_turn_(black_turn) {}
  State() : State(kNoSquare, Board(), true) {}

  const Board& GetBoard() const { return board_; }
  bool GetBlackTurn() const { return black_turn_; }
  Square Move() const { return move_; }

  std::optional<State> ToNextState(int square) const {
    if (!board_.IsEmpty(square)) {
      return std::nullopt;
    }
    BitPattern flip = GetFlip(square, board_.Player(), board_.Opponent());
    if (!flip) {
      return std::nullopt;
    }

    Board new_board = board_.Next(flip);
    bool new_black_turn;

    if (HaveToPass(new_board.Player(), new_board.Opponent())) {
      new_board.PlayMove(0);
      new_black_turn = black_turn_;
    } else {
      new_black_turn = !black_turn_;
    }

    return State(square, new_board, new_black_turn);
  }

 private:
  Square move_;
  Board board_;
  bool black_turn_;
};

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

  bool HasMove(Square s) const {
    for (auto* annotation : annotations_) {
      if (annotation->square == s) {
        return true;
      }
    }
    return false;
  }

  void AddAnnotation(MoveAnnotations* annotation) {
    annotations_.push_back(annotation);
  }

  void UpdateWithThor(std::unordered_map<Square, int> next_moves) {
    int num_thor_games = 0;
    for (MoveAnnotations* annotation : annotations_) {
      num_thor_games += next_moves[annotation->square];
    }
    for (MoveAnnotations* annotation : annotations_) {
      annotation->num_thor_games = num_thor_games;
    }
  }

  void EvaluateBook();

  void Evaluate(EvaluateParams params, int steps);

  void Stop() { evaluator_.Stop(); }

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
  Board unique_;
  EvaluatorDerivative evaluator_;
  std::vector<MoveAnnotations*> annotations_;
  BoardToEvaluateState state_;
  Book<>* book_;
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

class Main {
 public:
  Main(
      const std::string& evals_filepath,
      const std::string& book_filepath,
      const std::string& thor_filepath,
      SetBoard set_board,
      UpdateAnnotations update_annotations);

  ~Main() { free(annotations_.example_thor_games); }

  void NewGame() {
    Stop();
    states_ = {State()};
    current_state_ = 0;
    BoardChanged();
  }

  void PlayMove(Square square) {
    const State& state = states_[current_state_];
    auto next_state = state.ToNextState(square);
    if (!next_state) {
      return;
    }
    Stop();
    ++current_state_;
    states_.erase(states_.begin() + current_state_, states_.end());
    states_.push_back(next_state.value());
    BoardChanged();
  }

  void Redo() {
    if (current_state_ == states_.size() - 1) {
      return;
    }
    Stop();
    ++current_state_;
    BoardChanged();
  }

  void Undo() {
    if (current_state_ == 0) {
      return;
    }
    Stop();
    --current_state_;
    BoardChanged();
  }

  void Stop();

  void Evaluate() {
    Stop();
    current_future_ = std::make_shared<std::future<void>>(std::async(
        std::launch::async, &Main::EvaluateThread, this,
        current_thread_.load(), current_future_));
  }

  const ThorMetadata& GetThorMetadata() const {
    current_future_->get();
    return thor_metadata_;
  }

  EvaluateParams& GetEvaluateParams() { return evaluate_params_; }

 private:
  static constexpr int kNumEvaluators = 60;

  SetBoard set_board_;
  UpdateAnnotations update_annotations_;

  std::vector<State> states_;
  int current_state_;

  std::unique_ptr<EvalType> evals_;
  HashMap<kBitHashMap> hash_map_;
  TreeNodeSupplier tree_node_supplier_;
  std::array<std::unique_ptr<BoardToEvaluate>, kNumEvaluators> boards_to_evaluate_;
  int num_boards_to_evaluate_;

  Annotations annotations_;
  ElapsedTime time_;

  std::atomic_uint32_t current_thread_;
  std::shared_ptr<std::future<void>> current_future_;
  std::atomic_bool reset_;

  std::unique_ptr<Book<kBookVersion>> book_;

  std::vector<std::unique_ptr<ThorSourceMetadataExtended>> thor_sources_metadata_extended_;
  std::vector<ThorSourceMetadata*> thor_sources_metadata_;
  ThorMetadata thor_metadata_;

  std::unique_ptr<Thor> thor_;

  EvaluateParams evaluate_params_;

  void BoardChanged();

  void UpdateBoardsToEvaluate();

  BoardToEvaluate* NextBoardToEvaluate(double delta) {
    double highestPriority = -std::numeric_limits<double>::infinity();
    BoardToEvaluate* result = nullptr;

    for (int i = 0; i < num_boards_to_evaluate_; ++i) {
      BoardToEvaluate* b = boards_to_evaluate_[i].get();
      double priority = b->Priority(delta);
      if (priority > highestPriority) {
        highestPriority = priority;
        result = b;
      }
    }
    return result;
  }

  void InitializeEvaluator(const std::string& evals_filepath, const std::string& book_filepath, const std::string& thor_filepath) {
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

    annotations_.example_thor_games = (ThorGame*) malloc(sizeof(ThorGame));
  }

  void EvaluateThread(
      int current_thread, std::shared_ptr<std::future<void>> last_future);

  void RunUpdateAnnotations(int current_thread, bool finished);

  Sequence GetSequence() {
    if (states_[0].GetBoard() != Board()) {
      return Sequence();
    }
    std::vector<Square> moves;
    for (int i = 1; i <= current_state_; ++i) {
      moves.push_back(states_[i].Move());
    }
    return Sequence(moves);
  }
};

#endif // OTHELLO_SENSEI_MAIN_H