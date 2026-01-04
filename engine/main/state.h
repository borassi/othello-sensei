/*
 * Copyright 2024-2026 Michele Borassi
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

#ifndef OTHELLO_SENSEI_STATE_H
#define OTHELLO_SENSEI_STATE_H

#include <memory>

#include "bindings.h"

#include "../board/board.h"
#include "../board/sequence.h"
#include "../evaluatederivative/tree_node.h"
#include "../playagainstsensei/move_selector.h"
#include "../thor/thor.h"

enum SetNextStateResult {
  INVALID_MOVE,
  SAME_STATE,
  DIFFERENT_STATE,
};

class EvaluationState : public TreeNode {
 public:
  EvaluationState(Square move, const Board& board, bool black_turn, int depth, int depth_no_pass, bool modified) :
      TreeNode() {
    annotations_.example_thor_games = (ThorGame*) malloc(sizeof(ThorGame));
    ResetState(move, board, black_turn, depth, depth_no_pass, modified);
    assert(weak_lower_ == -63 && weak_upper_ == 63);
  }
  void ResetState(Square move, const Board& board, bool black_turn, int depth, int depth_no_pass, bool modified) {
    TreeNode::Reset(board, depth, 0);
    lower_ = -64;
    upper_ = 64;
    weak_lower_ = -63;
    weak_upper_ = 63;
    EnlargeEvaluations();
    is_book_leaf_ = false;
    annotations_.move = move;
    annotations_.depth = depth_;
    annotations_.depth_no_pass = depth_no_pass;
    annotations_.black_turn = black_turn;
    annotations_.num_example_thor_games = 0;
    annotations_.num_thor_games = 0;
    annotations_.first_child = nullptr;
    annotations_.next_sibling = nullptr;
    annotations_.during_analysis = false;
    annotations_.modified = modified;
    SetNextStateSecondary(nullptr);
    SetNextStatePrimary(nullptr);
    InvalidateThis();
  }
  ~EvaluationState() { free(annotations_.example_thor_games); }
  EvaluationState(const EvaluationState&) = delete;

  Square Move() const { return annotations_.move; }
  Square LastMove() const { return Move() == kPassMove ? Father()->Move() : Move(); }
  Annotations* GetAnnotations() { return &annotations_; }
  const Annotations* GetAnnotations() const { return &annotations_; }
  bool IsValid() const { return annotations_.valid; }
  void SetDerived(bool new_value) { annotations_.derived = new_value; }
  double Seconds() const { return annotations_.seconds; }
  bool IsPass() const {
    return children_.size() == 1 && children_[0]->annotations_.move == kPassMove;
  }
  void SetDuringAnalysis(bool value) { annotations_.during_analysis = value; }
  bool AfterPass() const {
    return annotations_.move == kPassMove;
  }
  double SecondsOnThisNode() const { return seconds_on_this_node_; }
  void ResetSecondsOnThisNode() { seconds_on_this_node_ = 0; }
  void AddSecondsOnThisNode(double value) { seconds_on_this_node_ += value; }
  void ResetSecondsToEvaluateThisNode() { seconds_to_evaluate_this_node_ = 0; }
  void AddSecondsToEvaluateThisNode(double value) { seconds_to_evaluate_this_node_ += value; }
  double SecondsToEvaluateThisNode() const { return seconds_to_evaluate_this_node_; }
  int DepthNoPass() const { return annotations_.depth_no_pass; }
  bool InAnalysisLine() const {
    if (next_state_primary_ != nullptr) {
      return true;
    }
    EvaluationState* father = Father();
    if (father == nullptr) {
      return false;
    }
    return father->next_state_primary_ == this;
  }

  void SetLeafEval(EvalLarge leaf_eval, Square depth) override {
    throw std::logic_error("Cannot update leaf eval in evaluated node.");
  }

  void SetGameOver() {
    EnsureValid();
    Eval final_eval = GetEvaluationGameOver(player_, opponent_);
    int final_eval_large = EvalToEvalLarge(final_eval);
    SetSolvedNoUpdate(final_eval_large, final_eval_large);
    is_book_leaf_ = false;
    annotations_.provenance = EVALUATE;
    UpdateSavedAnnotations();
    assert(weak_lower_ == -63 && weak_upper_ == 63);
  }

  bool HasValidChildren() const {
    for (auto& child : children_) {
      if (!child->IsValid()) {
        return false;
      }
    }
    return !IsLeaf();
  }

  std::pair<double, double> TotalError() {
    double errorBlack = 0.0;
    double errorWhite = 0.0;

    std::vector<double> scores;
    for (auto* state = this; state != nullptr; state = state->Father()) {
      scores.push_back(state->IsValid() ? state->annotations_.eval_best_line : kLessThenMinEvalLarge);
    }
    for (int i = 0; i < scores.size() - 1; ++i) {
      if (scores[scores.size() - i - 1] == kLessThenMinEvalLarge ||
          scores[scores.size() - i - 2] == kLessThenMinEvalLarge) {
        continue;
      }
      if (i % 2 == 0) {
        errorBlack += scores[scores.size() - i - 1] + scores[scores.size() - i - 2];
      } else {
        errorWhite += scores[scores.size() - i - 1] + scores[scores.size() - i - 2];
      }
    }
    return {errorBlack, errorWhite};
  }

  void SetThor(const GamesList& games);
  bool BlackTurn() const { return annotations_.black_turn; }

  int NumChildrenWithDescendants() {
    int result = 0;
    for (const std::shared_ptr<EvaluationState>& child : children_) {
      result += child->HasValidChildren();
    }
    return result;
  }

  bool IsImportant(bool is_xot, bool primary_states_available, bool is_next_state_primary, SenseiAction action) {
    if (PreviousLandable(action) == nullptr) {
      return true;
    }
    if (is_xot && annotations_.depth > 8 && PreviousLandable(action)->annotations_.depth < 8) {
      return true;
    }
    if (primary_states_available) {
      return !is_next_state_primary && next_state_primary_ != nullptr;
    } else {
      return NumChildrenWithDescendants() > 1;
    }
  }

  EvaluationState* LastImportantNode(bool is_xot, bool primary_states_available, SenseiAction action) {
    for (EvaluationState* state = PreviousLandable(action); state != nullptr; state = state->PreviousLandable(action)) {
      if (state->IsImportant(is_xot, primary_states_available, InAnalysisLine(), action)) {
        return state;
      }
    }
    return this;
  }

  void SetNumThorGames(unsigned int new_value) { annotations_.num_thor_games = new_value; }

  bool MaybeSetGameOver() {
    if (!IsGameOver(ToBoard())) {
      return false;
    }
    SetGameOver();
    assert(weak_lower_ == -63 && weak_upper_ == 63);
    return true;
  }

  EvaluationState* Father() const {
    assert(n_fathers_ <= 1);
    if (n_fathers_ == 0) {
      return nullptr;
    }
    return dynamic_cast<EvaluationState*>(fathers_[0]);
  }

  void SetPlayed() {
    EvaluationState* father = Father();
    if (father == nullptr) {
      return;
    }
    father->SetNextStateSecondary(this);
    father->SetPlayed();
  }

  void ResetPrimaryLine() {
    EvaluationState* state;
    for (state = next_state_primary_; state != nullptr; state = state->next_state_primary_) {
      state->Father()->SetNextStatePrimary(nullptr);
    }
  }

  void SetPrimaryLine() {
    for (EvaluationState* state = this; state->Father() != nullptr; state = state->Father()) {
      EvaluationState* father = state->Father();
      if (father->next_state_primary_ == state) {
        return;
      } else if (father->next_state_primary_ != nullptr) {
        father->next_state_primary_->ResetPrimaryLine();
      }
      father->SetNextStatePrimary(state);
    }
  }

  EvaluationState* NextState(Square move) const;

  EvaluationState* NextState() const {
    return next_state_primary_ ? next_state_primary_ : next_state_secondary_;
  }

  void SetNextStates();

  std::vector<EvaluationState*> GetChildren() const {
    std::vector<EvaluationState*> result;
    result.reserve(children_.size());
    for (const std::shared_ptr<EvaluationState>& child : children_) {
      result.push_back(child.get());
    }
    return result;
  }

  EvaluationState* ToDepth(int new_depth, SenseiAction action) {
    EvaluationState* result = ThisOrNextLandable(action);
    for (;
         result != nullptr && result->annotations_.depth_no_pass < new_depth;
         result = result->NextLandable(action)) {}
    return result;
  }

  bool IsModified() const {
    for (const EvaluationState* state = this; state != nullptr; state = state->Father()) {
      if (state->annotations_.modified) {
        return true;
      }
    }
    return false;
  }

  bool IsBeforeModification() const {
    for (const auto& child : children_) {
      if (child->annotations_.modified) {
        assert(children_.size() == 1);
        return true;
      }
    }
    return false;
  }

  bool IsLandable(SenseiAction action) const {
    if (MustPlay(action)) {
      return false;
    }
    if (n_children_ != 1) {
      return true;
    }
    Square move = children_[0]->Move();
    return (move != kPassMove && move != kSetupBoardMove);
  }

  EvaluationState* PreviousLandable(SenseiAction action) const {
    for (EvaluationState* result = Father(); result != nullptr; result = result->Father()) {
      if (result->IsLandable(action)) {
        return result;
      }
    }
    return nullptr;
  }

  EvaluationState* ThisOrPreviousLandable(SenseiAction action) {
    if (IsLandable(action)) {
      return this;
    }
    return PreviousLandable(action);
  }

  EvaluationState* GetChildForNextLandable() {
    if (children_.size() == 1) {
      return children_[0].get();
    }
    return next_state_primary_;
  }

  EvaluationState* ThisOrNextLandable(SenseiAction action) {
    for (EvaluationState* result = this; result != nullptr; result = result->GetChildForNextLandable()) {
      if (result->IsLandable(action)) {
        return result;
      }
    }
    return nullptr;
  }

  EvaluationState* NextLandable(SenseiAction action) const {
    EvaluationState* next = NextState();
    if (!next) {
      return nullptr;
    }
    return next->ThisOrNextLandable(action);
  }

  std::optional<Sequence> GetSequence() const {
    std::vector<Square> moves_inverted;
    for (const EvaluationState* state = this; state->Father() != nullptr; state = state->Father()) {
      if (state->annotations_.move == kPassMove) {
        continue;
      } else if (state->annotations_.move == kSetupBoardMove) {
        return std::nullopt;
      }
      moves_inverted.push_back(state->annotations_.move);
    }
    return Sequence(moves_inverted.rbegin(), moves_inverted.rend());
  }

  bool MustPlay(SenseiAction action) const {
    switch(action) {
      case SENSEI_INVALID_ACTION:
        assert(false);
      case SENSEI_INACTIVE:
      case SENSEI_EVALUATES:
        return false;
      case SENSEI_PLAYS_BLACK:
        return annotations_.black_turn;
      case SENSEI_PLAYS_WHITE:
        return !annotations_.black_turn;
      case SENSEI_PLAYS_BOTH:
        return true;
      default:
        assert(false);
        return false;
    }
  }

  bool InBook() const {
    return annotations_.provenance == BOOK || annotations_.provenance == CHILD_BOOK;
  }

  bool BookMixed() const {
    return annotations_.provenance == CHILD_MIXED;
  }

  void SetAnnotations(const Node& node, bool book, double seconds) {
    CopyAndEnlargeToAllEvals(node);
    if (book) {
      is_book_leaf_ = node.IsLeaf();
      // descendants_book are just copied from the book; descendants are updated, instead.
      annotations_.descendants_book = descendants_;
      descendants_ = annotations_.descendants;
      annotations_.provenance = BOOK;
    } else {
      assert(!HasValidChildren() || IsBeforeModification());
      assert(descendants_ >= annotations_.descendants);
      is_book_leaf_ = false;
      annotations_.descendants_evaluating_this = descendants_;
      annotations_.descendants = descendants_;
      annotations_.seconds = seconds;
      annotations_.provenance = CHILD_EVALUATE;
    }
    UpdateFather();
    UpdateSavedAnnotations();
    assert(weak_lower_ == -63 && weak_upper_ == 63);
  }

  void DeleteChildren() {
    children_.clear();
    if (n_children_ != 0) {
      delete[] TreeNode::children_;
      n_children_ = 0;
      is_leaf_ = true;
      TreeNode::children_ = nullptr;
    }
    SetNextStateSecondary(nullptr);
    SetNextStatePrimary(nullptr);
  }

  void EnsureValid() {
    if (!IsValid()) {
      leaf_eval_ = 0;
      annotations_.valid = true;
    }
    assert(IsValid());
  }

  void UpdateEvalBestLine() {
    assert(CanUpdateFather());
    bool has_book_child = annotations_.descendants_book > 0;
    bool all_book_children = true;
    annotations_.eval_best_line = kLessThenMinEvalLarge;
    annotations_.median_eval_best_line = kLessThenMinEvalLarge;
    descendants_ = annotations_.descendants_evaluating_this;
    annotations_.seconds = 0;
    for (auto& child : children_) {
      descendants_ += child->descendants_;
      annotations_.seconds += child->annotations_.seconds;
      annotations_.eval_best_line = std::max(
          annotations_.eval_best_line, -child->annotations_.eval_best_line);
      annotations_.median_eval_best_line = std::max(
          annotations_.median_eval_best_line,
          -child->annotations_.median_eval_best_line);
      if (child->InBook()) {
        has_book_child = true;
      } else if (child->BookMixed()) {
        has_book_child = true;
        all_book_children = false;
      } else {
        all_book_children = false;
      }
    }
    if (all_book_children) {
      annotations_.provenance = CHILD_BOOK;
    } else if (has_book_child) {
      annotations_.provenance = CHILD_MIXED;
    } else {
      annotations_.provenance = CHILD_EVALUATE;
    }
  }

  bool CanUpdateFather() {
    return HasValidChildren() && !IsBeforeModification();
  }

  void UpdateFather() override {
    // Do not update if we should keep this as a leaf.
    if (!CanUpdateFather()) {
      return;
    }
    if (!is_book_leaf_ || annotations_.descendants_book < annotations_.descendants) {
      EnsureValid();
      TreeNode::UpdateFather();
    }
    assert(IsValid());
    UpdateEvalBestLine();
    UpdateSavedAnnotations();
    assert(annotations_.median_eval % 2 == 0);
    assert(annotations_.median_eval_best_line % 2 == 0);
    assert(weak_lower_ == -63 && weak_upper_ == 63);
  }

  int ChooseRandomMoveToPlay(double error, double max_error) {
    std::vector<const Node*> children;
    children.reserve(children_.size());
    for (auto& child : children_) {
      children.push_back((const Node*) child.get());
    }
    const Node* best_child = FindNextMoveTotalError(children, error, max_error);
    if (best_child == nullptr) {
      return kNoMove;
    }
    return ((EvaluationState*) best_child)->Move();
  }

  void UpdateSavedAnnotations() {
    annotations_.eval = GetEval();
    annotations_.median_eval = GetPercentileLower(0.5) - 1;
    annotations_.leaf_eval = LeafEval();
    annotations_.prob_lower_eval = SolveProbabilityLower(-65);
    annotations_.prob_upper_eval = SolveProbabilityUpper(65);
    annotations_.proof_number_lower = annotations_.median_eval == -64 ? 0 : RemainingWork(-63, (Eval) (annotations_.median_eval - 1));
    annotations_.disproof_number_upper = annotations_.median_eval == 64 ? 0 : RemainingWork((Eval) (annotations_.median_eval + 1), 63);
    annotations_.lower = Lower();
    annotations_.upper = Upper();
    annotations_.weak_lower = WeakLower();
    annotations_.weak_upper = WeakUpper();
    annotations_.descendants = GetNVisited();
    annotations_.missing = Node::IsSolved() ? 0 : RemainingWork(-63, 63);
    annotations_.valid = true;
    if (!CanUpdateFather()) {
      annotations_.eval_best_line = annotations_.eval;
      annotations_.median_eval_best_line = annotations_.median_eval;
    }
    assert(annotations_.median_eval % 2 == 0);
    assert(annotations_.median_eval_best_line % 2 == 0);
  }

  EvaluationState* SetSquare(int square, int value) {
    BitPattern square_pattern = 1ULL << square;
    BitPattern player = player_;
    BitPattern opponent = opponent_;

    if (!annotations_.black_turn) {
      value = -value;
    }
    if (value == -1) {
      player |= square_pattern;
      opponent &= ~square_pattern;
    } else if (value == 1) {
      opponent |= square_pattern;
      player &= ~square_pattern;
    } else {
      player &= ~square_pattern;
      opponent &= ~square_pattern;
    }
    bool new_black_turn = Board(player, opponent).NEmpties() % 2 == 0;
    if (new_black_turn != annotations_.black_turn) {
      std::swap(player, opponent);
    }
    return SetBoard(player, opponent, new_black_turn);
  }

  EvaluationState* InvertTurn() {
    return SetBoard(opponent_, player_, !annotations_.black_turn);
  }

  void InvalidateRecursive() {
    InvalidateThis();
    for (auto& child : children_) {
      child->InvalidateRecursive();
    }
  }

  std::pair<double, double> SecondsParents() const {
    double time_black = 0;
    double time_white = 0;
    for (const EvaluationState* node = this; node != nullptr; node = node->Father()) {
      if (node->annotations_.black_turn) {
        time_black += node->seconds_on_this_node_;
      } else {
        time_white += node->seconds_on_this_node_;
      }
    }
    return {time_black, time_white};
  }

  EvaluationState* SetBoard(BitPattern player, BitPattern opponent, bool black_turn) {
    if (HaveToPass(Board(player, opponent)) && !HaveToPass(Board(opponent, player))) {
      black_turn = !black_turn;
      std::swap(player, opponent);
    }
    if (!annotations_.modified) {
      DeleteChildren();
      std::vector<TreeNode*> children;
      std::shared_ptr<EvaluationState> state = std::make_shared<EvaluationState>(
          kSetupBoardMove, Board(player, opponent), black_turn, Depth() + 1, annotations_.depth_no_pass, true);
      children_.push_back(state);
      children.push_back(state.get());
      SetChildrenNoUpdate(children);
      UpdateAnnotationsTree();
      children_[0]->SetNextStates();
      return children_[0].get();
    } else {
      TreeNode* father = n_fathers_ > 0 ? fathers_[0] : nullptr;
      Annotations* next_sibling = annotations_.next_sibling;
      DeleteChildren();
      ResetState(annotations_.move, Board(player, opponent), black_turn, depth_, annotations_.depth_no_pass, true);
      if (father != nullptr) {
        AddFather(father);
      }
      annotations_.next_sibling = next_sibling;
      SetNextStates();
      return this;
    }
  }

 private:
  Annotations annotations_;
  double seconds_on_this_node_;
  double seconds_to_evaluate_this_node_;
  std::vector<std::shared_ptr<EvaluationState>> children_;
  EvaluationState* next_state_primary_;
  EvaluationState* next_state_secondary_;
  bool is_book_leaf_;

  void AddFather(TreeNode* father) override {
    TreeNode::AddFather(father);
    assert(n_fathers_ == 1);
    annotations_.father = &static_cast<EvaluationState*>(fathers_[0])->annotations_;
    assert(weak_lower_ == -63 && weak_upper_ == 63);
  }

  void InvalidateThis() {
    seconds_on_this_node_ = 0;
    seconds_to_evaluate_this_node_ = 0;
    annotations_.valid = false;
    annotations_.derived = false;
    annotations_.descendants = 0;
    annotations_.descendants_book = 0;
    annotations_.descendants_evaluating_this = 0;
    annotations_.seconds = 0;
  }

  void UpdateAnnotationsTree() {
    annotations_.first_child = &children_[0]->annotations_;
    for (int i = 1; i < children_.size(); ++i) {
      children_[i-1]->annotations_.next_sibling = &children_[i]->annotations_;
    }
  }

  void SetNextStateSecondary(EvaluationState* new_value) {
    next_state_secondary_ = new_value;
    annotations_.next_state_secondary = next_state_secondary_ ? &next_state_secondary_->annotations_ : nullptr;
  }

  void SetNextStatePrimary(EvaluationState* new_value) {
    next_state_primary_ = new_value;
    annotations_.next_state_primary = next_state_primary_ ? &next_state_primary_->annotations_ : nullptr;
  }
};

void GameToThorGame(const Game& game, ThorGame& thor_game, const Sequence& sequence);

#endif // OTHELLO_SENSEI_STATE_H
