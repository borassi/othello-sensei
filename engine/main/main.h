/*
 * Copyright 2023-2026 Michele Borassi
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
#include <optional>

#include "bindings.h"
#include "engine.h"
#include "state.h"

#include "../board/board.h"
#include "../board/get_flip.h"
#include "../board/get_moves.h"
#include "../book/book.h"
#include "../evaluatederivative/evaluator_derivative.h"
#include "../evaluatederivative/tree_node.h"
#include "../evaluatedepthone/pattern_evaluator.h"
#include "../savegame/game_to_save.h"
#include "../thor/thor.h"
#include "../utils/misc.h"
#include "../xot/xot.h"

using namespace std::chrono_literals;

class Main {
 public:
  Main(
      const std::string& evals_filepath,
      const std::string& book_filepath,
      const std::string& thor_filepath,
      const std::string& saved_games_filepath,
      const std::string& xot_small_filepath,
      const std::string& xot_large_filepath,
      SetBoard set_board,
      UpdateAnnotations update_annotations,
      SetThorMetadata set_thor_metadata,
      UpdateTimers update_timers,
      SendMessage send_message);

  ~Main() {
    is_being_destroyed_ = true;
    update_timers_future_.get();
  }

  void NewGame() {
    first_state_ = std::make_shared<EvaluationState>(kStartingPositionMove, Board(), true, 0, 0, false);
    [[maybe_unused]] bool new_state = ToState(first_state_.get(), false);
    assert(new_state);
    ResetGameMetadata();
  }

  bool PlayMove(Square square, bool automatic) {
    if (automatic != current_state_->MustPlay(evaluate_params_.sensei_action)) {
      return false;
    }
    bool success = ToState(current_state_->NextState(square), true);
    if (success && evaluate_params_.sensei_action != SENSEI_EVALUATES) {
      current_state_->SetPrimaryLine();
    }
    return success;
  }

  bool SetCurrentMove(Square depth) {
    bool valid = ToState(first_state_->ToDepth(depth, evaluate_params_.sensei_action), false);
    current_state_->SetPlayed();
    return valid;
  }

  bool Redo() {
    return ToState(current_state_->NextLandable(evaluate_params_.sensei_action), false);
  }

  bool Undo() {
    return ToState(current_state_->PreviousLandable(evaluate_params_.sensei_action), false);
  }

  bool ToLastImportantNode() {
    EvaluationState* goal = current_state_->LastImportantNode(IsXot(), first_state_->InAnalysisLine(), evaluate_params_.sensei_action);
    if (current_state_ == goal) {
      return false;
    }
    return ToState(goal, false);
  }

  void Stop();

  char* GetSequence() {
    std::optional<Sequence> sequence_opt = current_state_->GetSequence();
    Sequence sequence = sequence_opt ? *sequence_opt : Sequence();
    std::string sequence_str = sequence.ToString();
    char* result = (char*) malloc(sequence_str.size() * sizeof(char) + 1);
    strncpy(result, sequence_str.c_str(), sequence_str.size() + 1);
    return result;
  }

  void SetSequence(const Sequence& moves, bool eval = false) {
    NewGame();
    for (int i = 0; i < moves.Size(); ++i) {
      if (eval) {
        Evaluate();
      }
      ToState(current_state_->NextState(moves.Move(i)), false);
    }
  }

  bool SetSequence(const std::string& sequence) {
    Sequence moves = Sequence::ParseFromString(sequence);
    if (moves.Size() == 0 && sequence != "") {
      return false;
    }
    SetSequence(moves);
    return true;
  }

  bool PasteBoard(const std::string& board) {
    std::optional<std::pair<Board, bool>> parsed = Board::FromString(board);
    if (!parsed) {
      return false;
    }
    auto& [result_board, result_turn] = *parsed;
    NewGame();
    ToState(current_state_->SetBoard(result_board.Player(), result_board.Opponent(), result_turn), false);
    return true;
  }

  void SetEvaluateParams(const EvaluateParams& params);

  void Evaluate();

  void Analyze() {
    last_params_ = evaluate_params_;
    current_state_->SetPrimaryLine();
    analyzing_ = 1;
    engine_.Start(current_state_, first_state_, evaluate_params_, analyzing_);
  }

  void ResetAnalyzedGame() {
    first_state_->ResetPrimaryLine();
    ResetGameMetadata();
  }

  // We need a separate call to get the annotations in the UI thread, to prevent
  // concurrent modification.
  Annotations* GetCurrentAnnotations(int current_thread) {
    if (engine_.CurrentThread() != current_thread) {
      return nullptr;
    }
    return current_state_->GetAnnotations();
  }

  // We need a separate call to get the annotations in the UI thread, to prevent
  // concurrent modification.
  Annotations* GetStartAnnotations(int current_thread) {
    if (engine_.CurrentThread() != current_thread) {
      return nullptr;
    }
    last_state_flutter_ = first_state_;
    return last_state_flutter_->GetAnnotations();
  }

  void RandomXOT(bool large) {
    const XOT& xot = large ? xot_small_ : xot_large_;
    Sequence sequence = xot.RandomSequence();
    SetSequence(sequence, true);
  }

  void SetXOTState(XOTState xot_state) {
    xot_state_ = xot_state;
    RunSetBoard(false);
  }
  XOTState GetXOTState() { return xot_state_; }

  bool IsXot() {
    switch (xot_state_) {
      case XOT_STATE_ALWAYS:
        return true;
      case XOT_STATE_NEVER:
        return false;
      case XOT_STATE_AUTOMATIC: {
        if (!first_state_) {
          return false;
        }
        auto state_in_xot = first_state_->ToDepth(8, SENSEI_EVALUATES);
        if (!state_in_xot) {
          return false;
        }
        auto sequence = state_in_xot->GetSequence();
        if (!(sequence && xot_large_.IsInListPrefix(*sequence))) {
          return false;
        }
        auto [error_black, error_white] = state_in_xot->TotalError();
        return std::max(error_black, error_white) > 5;
      }
      default:
        assert(false);
        return false;
    }
  }

  void SetSquare(int square, int value) {
    if (square < 0 || square > 63) {
      return;
    }
    ToStateNoStop(current_state_->SetSquare(square, value), false);
  }

  void InvertTurn() {
    ToStateNoStop(current_state_->InvertTurn(), false);
  }

  void RunUpdateTimers() {
    std::lock_guard<std::mutex> guard(time_on_this_position_mutex_);
    if (!current_state_) {
      return;
    }
    auto [seconds_black, seconds_white] = current_state_->SecondsParents();
    if (time_on_this_position_) {
      if (current_state_->GetAnnotations()->black_turn) {
        seconds_black += time_on_this_position_->Get();
      } else {
        seconds_white += time_on_this_position_->Get();
      }
    }
    update_timers_(seconds_black, seconds_white);
  }

  void RunUpdateTimersThread() {
    while (!is_being_destroyed_) {
      RunUpdateTimers();
      std::this_thread::sleep_for(100ms);
    }
  }

  void SetCountingTime(bool value) {
    std::lock_guard<std::mutex> guard(time_on_this_position_mutex_);
    SetCountingTimeNoLock(value);
  }
  void SetCountingTimeNoLock(bool value);

  GameMetadata* MutableGameMetadata() { return &game_metadata_; }

  SaveGameOutput* GetGameToSave() {
    auto sequence_opt = current_state_->GetSequence();
    auto save_game_output = (SaveGameOutput*) malloc(sizeof(SaveGameOutput));
    if (!sequence_opt) {
      save_game_output->success = false;
      static const std::string kErrorNoSequence = "Cannot save games with manual board setup (yet).";
      strncpy(save_game_output->error, kErrorNoSequence.c_str(), kErrorNoSequence.size() + 1);
      return save_game_output;
    }
    GameToSave game(
        *sequence_opt,
        std::string(game_metadata_.black),
        std::string(game_metadata_.white),
        std::string(game_metadata_.tournament),
        std::string(game_metadata_.notes),
        static_cast<short>(game_metadata_.year),
        static_cast<Eval>(game_metadata_.black_disks),
        game_metadata_.round);
    std::string output = game.ToString();
    if (output.size() >= 2000) {
      save_game_output->success = false;
      static const std::string kErrorLargeOutput = "Output too large (probably a bug). Please notify michele.borassi@gmail.com.";
      strncpy(save_game_output->error, kErrorLargeOutput.c_str(), kErrorLargeOutput.size() + 1);
      return save_game_output;
    }
    save_game_output->success = true;
    strncpy(save_game_output->game, output.c_str(), output.size() + 1);
    strncpy(save_game_output->error, "", 1);
    return save_game_output;
  }

  void Open(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
      throw std::runtime_error("Could not open file: " + path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    GameToSave game = GameToSave::FromString(content);
    ThorGame thor_game;
    // The game does not get rotated because the sequence is empty.
    GameToThorGame(game.ToGame(/*canonicalize=*/false), thor_game, Sequence());
    OpenThorGame(thor_game);
    strncpy(game_metadata_.notes, game.Notes().c_str(), game.Notes().size() + 1);
    strncpy(game_metadata_.round, game.Round().c_str(), game.Round().size() + 1);
  }

  void PlayOneMove(const ThorGame& game) {
    PlayMove(game.moves[current_state_->DepthNoPass()], false);
  }

  void OpenThorGame(const ThorGame& game) {
    SetSequence(Sequence(game.moves, 60));
    strncpy(game_metadata_.black, game.black, 20);
    strncpy(game_metadata_.white, game.white, 20);
    strncpy(game_metadata_.tournament, game.tournament, 26);
    game_metadata_.year = game.year;
    game_metadata_.black_disks = game.score;
    current_state_->SetPrimaryLine();
  }

  void SetFileSources(const std::vector<std::string>& sources) {
    engine_.StartSetFileSources(sources);
  }

  bool ReloadSource(const std::string& source) {
    return engine_.ReloadSource(source);
  }

 private:
  static constexpr int kNumEvaluators = 60;

  SetBoard set_board_;
  UpdateTimers update_timers_;

  // We keep the last state used by Flutter, so that it does not get deleted
  // when Flutter is still reading it.
  std::shared_ptr<EvaluationState> last_state_flutter_;
  std::shared_ptr<EvaluationState> first_state_;
  EvaluationState* current_state_;
  XOT xot_small_;
  XOT xot_large_;
  XOTState xot_state_;

  Engine engine_;
  // 0 = not in analysis; 1 = started analysis (skip undo), 2 = in analysis.
  int analyzing_;

  EvaluateParams evaluate_params_;
  EvaluateParams last_params_;

  std::optional<ElapsedTime> time_on_this_position_;
  std::mutex time_on_this_position_mutex_;

  bool is_being_destroyed_;
  std::future<void> update_timers_future_;

  GameMetadata game_metadata_;

  bool ToState(EvaluationState* new_state, bool handle_game_over);

  void ToStateNoStop(EvaluationState* new_state, bool handle_game_over);

  void RunSetBoard(bool handle_game_over) {
    Board board = current_state_->ToBoard();
    set_board_({
      board.Player(),
      board.Opponent(),
      current_state_->BlackTurn(),
      current_state_->LastMove(),
      handle_game_over && IsGameOver(board),
      first_state_->GetAnnotations()
    });
  }

  void ResetGameMetadata() {
    memset(game_metadata_.black, 0, 20);
    memset(game_metadata_.white, 0, 20);
    memset(game_metadata_.tournament, 0, 26);
    game_metadata_.year = GetCurrentYear();
    game_metadata_.black_disks = 64;
    memset(game_metadata_.round, 0, 10);
    memset(game_metadata_.notes, 0, 300);
  }
};

#endif // OTHELLO_SENSEI_MAIN_H