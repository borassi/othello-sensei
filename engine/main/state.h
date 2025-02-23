/*
 * Copyright 2024-2025 Michele Borassi
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
#include "../thor/thor.h"

enum SetNextStateResult {
  INVALID_MOVE,
  SAME_STATE,
  DIFFERENT_STATE,
};

class EvaluationState : public TreeNode {
 public:
  EvaluationState(Square move, const Board& board, bool black_turn, int depth) :
      TreeNode() {
    TreeNode::Reset(board, depth, 0);
    lower_ = -64;
    upper_ = 64;
    weak_lower_ = -63;
    weak_upper_ = 63;
    EnlargeEvaluations();
    annotations_.move = move;
    annotations_.depth = depth_;
    annotations_.black_turn = black_turn;
    annotations_.example_thor_games = (ThorGame*) malloc(sizeof(ThorGame));
    annotations_.num_example_thor_games = 0;
    annotations_.num_thor_games = 0;
    annotations_.first_child = nullptr;
    annotations_.next_sibling = nullptr;
    annotations_.during_analysis = false;
    SetNextStatePlayed(nullptr);
    SetNextStateInAnalysis(nullptr);
    InvalidateThis();
    assert(weak_lower_ == -63 && weak_upper_ == 63);
  }
  ~EvaluationState() { free(annotations_.example_thor_games); }
  EvaluationState(const EvaluationState&) = delete;

  Square Move() const { return annotations_.move; }
  Annotations* GetAnnotations() { return &annotations_; }
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
  bool InAnalysisLine() const {
    if (NextStateInAnalysis() != nullptr) {
      return true;
    }
    EvaluationState* father = Father();
    if (father == nullptr) {
      return false;
    }
    return father->NextStateInAnalysis() == this;
  }

  void SetLeafEval(EvalLarge leaf_eval, Square depth) override {
    throw std::logic_error("Cannot update leaf eval in evaluated node.");
  }

  void SetGameOver() {
    EnsureValid();
    Eval final_eval = GetEvaluationGameOver(player_, opponent_);
    int final_eval_large = EvalToEvalLarge(final_eval);
    SetSolvedNoUpdate(final_eval_large, final_eval_large);
    annotations_.median_eval = final_eval;
    annotations_.eval_best_line = final_eval;
    annotations_.median_eval_best_line = final_eval;
    annotations_.provenance = CHILD_EVALUATE;
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
      scores.push_back(state->annotations_.eval_best_line);
    }
    for (int i = 0; i < scores.size() - 1; ++i) {
      if (isnan(scores[scores.size() - i - 1]) || isnan(scores[scores.size() - i - 2])) {
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

  // Returns:
  // - If there is an analyzed game and this is outside the analysis, the first
  //   ancestor in the analysis.
  // - The first state otherwise.
  EvaluationState* LastAnalyzedState() {
    EvaluationState* state;
    for (state = this; state->Father() != nullptr; state = state->Father()) {
      if (state->next_state_in_analysis_ && !next_state_in_analysis_) {
        return state;
      }
    }
    return state;
  }

  int NumChildrenWithDescendants() {
    int result = 0;
    for (const std::shared_ptr<EvaluationState>& child : children_) {
      result += child->HasValidChildren();
    }
    return result;
  }

  // Returns the first ancestor with multiple visited children.
  EvaluationState* LastChoice() {
    EvaluationState* state;
    // Avoid a crash if this is the starting position.
    if (Father() == nullptr) {
      return this;
    }
    for (state = Father(); state->Father() != nullptr; state = state->Father()) {
      // NOTE: we cannot assert(state->NumChildrenWithDescendants() >= 1), if the position is not
      // evaluated, yet.
      if (state->NumChildrenWithDescendants() > 1) {
        return state;
      }
    }
    return state;
  }

  EvaluationState* NextStateInAnalysis() const {
    assert(
      (next_state_in_analysis_ == nullptr && annotations_.next_state_in_analysis == nullptr)
      || next_state_in_analysis_->GetAnnotations() == annotations_.next_state_in_analysis);
    return next_state_in_analysis_;
  }

  EvaluationState* NextStatePlayed() const {
    assert(
      (next_state_played_ == nullptr && annotations_.next_state_played == nullptr)
      || next_state_played_->GetAnnotations() == annotations_.next_state_played);
    return next_state_played_;
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
    return static_cast<EvaluationState*>(fathers_[0]);
  }

  void SetPlayed() {
    EvaluationState* father = Father();
    if (father == nullptr) {
      return;
    }
    father->SetNextStatePlayed(this);
    father->SetPlayed();
  }

  void SetNotAnalyzed() {
    assert(Father() == nullptr);
    assert(NextStateInAnalysis() != nullptr);
    EvaluationState* state;
    for (state = NextStateInAnalysis(); state != nullptr; state = state->NextStateInAnalysis()) {
      state->Father()->SetNextStateInAnalysis(nullptr);
    }
  }

  EvaluationState* SetAnalyzed() {
    assert(Father() == nullptr);
    EvaluationState* state;
    for (state = this; state->NextStatePlayed() != nullptr; state = state->NextStatePlayed()) {
      state->SetNextStateInAnalysis(state->NextStatePlayed());
    }
    return state;
  }

  EvaluationState* NextState(Square move) const;

  EvaluationState* NextState() const {
    return NextStateInAnalysis() ? NextStateInAnalysis() : NextStatePlayed();
  }

  void SetNextStates();

  std::vector<EvaluationState*> GetChildren() const {
    std::vector<EvaluationState*> result;
    for (const std::shared_ptr<EvaluationState>& child : children_) {
      result.push_back(child.get());
    }
    return result;
  }

  EvaluationState* ToDepth(int new_depth) {
    EvaluationState* result = this;
    assert(new_depth >= 60 - result->n_empties_);
    // We cannot use Depth() because of passes.
    for (;
         result != nullptr && new_depth > 60 - result->n_empties_;
         result = result->NextState()) {}
    return result;
  }

  Sequence GetLongestSequence() const {
    std::vector<Square> moves;
    for (const EvaluationState* state = this; state != nullptr; state = state->NextStatePlayed()) {
      if (state->annotations_.move != kPassMove) {
        moves.push_back(state->annotations_.move);
      }
    }
    return Sequence(moves.begin(), moves.end());
  }

  Sequence GetSequence() const {
    std::vector<Square> moves_inverted;
    for (const EvaluationState* state = this; state->Father() != nullptr; state = state->Father()) {
      if (state->annotations_.move != kPassMove) {
        moves_inverted.push_back(state->annotations_.move);
      }
    }
    return Sequence(moves_inverted.rbegin(), moves_inverted.rend());
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
      // descendants_book are just copied from the book; descendants are updated, instead.
      annotations_.descendants_book = descendants_;
      descendants_ = annotations_.descendants;
      annotations_.provenance = BOOK;
      if (HasValidChildren()) {
        UpdateFather();
        return;
      }
    } else {
      assert(!HasValidChildren());
      assert(descendants_ >= annotations_.descendants);
      annotations_.descendants = descendants_;
      annotations_.seconds = seconds;
      annotations_.provenance = CHILD_EVALUATE;
    }
    assert(!HasValidChildren());
    annotations_.eval_best_line = GetEval();
    annotations_.median_eval = GetPercentileLower(0.5) - 1;
    annotations_.median_eval_best_line = annotations_.median_eval;
    assert(weak_lower_ == -63 && weak_upper_ == 63);
    UpdateSavedAnnotations();
  }

  void InvalidateRecursive() {
    if (!IsValid()) {
      return;
    }
    InvalidateThis();
    for (auto& child : children_) {
      child->InvalidateRecursive();
    }
  }

  void EnsureValid() {
    if (!IsValid()) {
      leaf_eval_ = 0;
    }
  }

  EvaluationState* PreviousNonPass() {
    auto father = Father();
    return AfterPass() ? father->Father() : father;
  }

  void UpdateFather() override {
    // Handle the case where we have invalid nodes intertwined with valid nodes.
    if (!HasValidChildren()) {
      return;
    }
    EnsureValid();
    TreeNode::UpdateFather();
    bool has_book_child = annotations_.descendants_book > 0;
    bool all_book_children = true;
    annotations_.eval_best_line = kLessThenMinEvalLarge;
    annotations_.median_eval_best_line = kLessThenMinEvalLarge;
    descendants_ = 0;
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
    annotations_.descendants = descendants_;
    if (all_book_children) {
      annotations_.provenance = CHILD_BOOK;
    } else if (has_book_child) {
      annotations_.provenance = CHILD_MIXED;
    } else {
      annotations_.provenance = CHILD_EVALUATE;
    }
    annotations_.median_eval = GetPercentileLower(0.5) - 1;
    UpdateSavedAnnotations();
    assert(annotations_.median_eval % 2 == 0);
    assert(annotations_.median_eval_best_line % 2 == 0);
    assert(weak_lower_ == -63 && weak_upper_ == 63);
  }

  void UpdateSavedAnnotations() {
    annotations_.eval = GetEval();
    annotations_.leaf_eval = LeafEval();
    annotations_.prob_lower_eval = SolveProbabilityLower(-65);
    annotations_.prob_upper_eval = SolveProbabilityUpper(65);
    annotations_.proof_number_lower = annotations_.median_eval == -64 ? 0 : RemainingWork(-63, annotations_.median_eval - 1);
    annotations_.disproof_number_upper = annotations_.median_eval == 64 ? 0 : RemainingWork(annotations_.median_eval + 1, 63);
    annotations_.lower = Lower();
    annotations_.upper = Upper();
    annotations_.weak_lower = WeakLower();
    annotations_.weak_upper = WeakUpper();
    annotations_.descendants = GetNVisited();
    annotations_.missing = Node::IsSolved() ? 0 : RemainingWork(-63, 63);
    annotations_.valid = true;
    assert(annotations_.median_eval % 2 == 0);
    assert(annotations_.median_eval_best_line % 2 == 0);
  }

 private:
  Annotations annotations_;
  std::vector<std::shared_ptr<EvaluationState>> children_;
  EvaluationState* next_state_in_analysis_;
  EvaluationState* next_state_played_;

  void AddFather(TreeNode* father) override {
    TreeNode::AddFather(father);
    assert(n_fathers_ == 1);
    annotations_.father = &static_cast<EvaluationState*>(fathers_[0])->annotations_;
    assert(weak_lower_ == -63 && weak_upper_ == 63);
  }

  void InvalidateThis() {
    annotations_.valid = false;
    annotations_.derived = false;
    annotations_.descendants = 0;
    annotations_.descendants_book = 0;
    annotations_.seconds = 0;
  }

  void UpdateAnnotationsTree() {
    annotations_.first_child = &children_[0]->annotations_;
    for (int i = 1; i < children_.size(); ++i) {
      children_[i-1]->annotations_.next_sibling = &children_[i]->annotations_;
    }
  }

  void SetNextStatePlayed(EvaluationState* new_value) {
    next_state_played_ = new_value;
    annotations_.next_state_played = next_state_played_ ? &next_state_played_->annotations_ : nullptr;
  }

  void SetNextStateInAnalysis(EvaluationState* new_value) {
    next_state_in_analysis_ = new_value;
    annotations_.next_state_in_analysis = next_state_in_analysis_ ? &next_state_in_analysis_->annotations_ : nullptr;
  }
};

#endif // OTHELLO_SENSEI_STATE_H
