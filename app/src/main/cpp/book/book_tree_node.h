/*
 * Copyright 2022 Google LLC
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

#ifndef OTHELLOSENSEI_BOOK_BOOK_TREE_NODE_H
#define OTHELLOSENSEI_BOOK_BOOK_TREE_NODE_H

#include <iterator>
#include <optional>
#include <unordered_set>
#include <vector>

#include "../board/board.h"
#include "../evaluatederivative/tree_node.h"
#include "../estimators/endgame_time.h"
#include "serializable_boolean_vector.h"
#include "value_file.h"

// Max fathers = 140656 (see test).
constexpr int kMaxNodeSize = 13 /*board*/ + 3 * 140656 /*fathers*/ + 4 /*descendants*/ + 5 /*bounds*/ + 7 * 64 /*evals*/;

template<class Book>
class BookTreeNode : public TreeNode {
 public:
  BookTreeNode(Book* book, const TreeNode& node) : TreeNode(), book_(book), is_leaf_(true) {
    Board b = node.ToBoard().Unique();
    player_ = b.Player();
    opponent_ = b.Opponent();
    n_empties_ = b.NEmpties();

    min_evaluation_ = -63;
    weak_lower_ = -63;
    weak_upper_ = 63;
    lower_ = node.Lower();
    upper_ = node.Upper();
    leaf_eval_ = node.LeafEval();
    EnlargeEvaluationsInternal();
    descendants_ = node.GetNVisited();

    auto proof_eval = node.GetEvaluation(node.WeakLower());
    auto disproof_eval = node.GetEvaluation(node.WeakUpper());
    for (int i = kMinEval + 1; i <= kMaxEval - 1; i += 2) {
      if (i < lower_) {
        MutableEvaluation(i)->SetProved();
      } else if (i > upper_) {
        MutableEvaluation(i)->SetDisproved();
      } else if (i < node.WeakLower()) {
        MutableEvaluation(i)->SetProving(proof_eval, node.WeakLower() - i);
      } else if (i > node.WeakUpper()) {
        MutableEvaluation(i)->SetDisproving(disproof_eval, i - node.WeakUpper());
      } else {
        *MutableEvaluation(i) = Evaluation(node.GetEvaluation(i));
      }
    }
  }

  BookTreeNode(Book* book, const std::vector<char>& serialized, bool fathers = true) : book_(book) {
    Board board = Board::Deserialize(serialized.begin());
    player_ = board.Player();
    opponent_ = board.Opponent();
    n_empties_ = board.NEmpties();
    int i = 13;

    while (true) {
      CompressedFlip compressed_flip = 0;
      compressed_flip |= (u_int8_t) serialized[i] | ((u_int8_t) serialized[i+1] << 8) | ((u_int8_t) serialized[i+2] << 16);
      i += 3;
      if (compressed_flip == 0) {
        break;
      }
      father_flips_.push_back(compressed_flip >> 2);
      if ((compressed_flip & 1) == 0) {
        break;
      }
    }

    descendants_ = (u_int64_t) *((float*) &(serialized[i]));
    i += sizeof(float);
    lower_ = (Eval) serialized[i++];
    upper_ = (Eval) serialized[i++];
    weak_lower_ = -63;
    weak_upper_ = 63;
    min_evaluation_ = -63;
    Eval last_1 = (Eval) serialized[i++];
    Eval first_0 = (Eval) serialized[i++];
    is_leaf_ = (bool) serialized[i++];
    // Mark as valid.
    leaf_eval_ = 0;
    n_threads_working_ = 0;

    EnlargeEvaluationsInternal();

    for (int eval = weak_lower_; eval <= lower_; eval += 2) {
      MutableEvaluation(eval)->SetProved();
    }
    for (int eval = weak_upper_; eval >= std::max(upper_, weak_lower_); eval -= 2) {
      MutableEvaluation(eval)->SetDisproved();
    }
    for (int eval = last_1 + 2; eval <= first_0 - 2; eval += 2) {
      Probability prob = serialized[i++];
      PN proof_number = serialized[i++];
      PN disproof_number = serialized[i++];
      u_int32_t max_log_derivative = 0;
      u_int8_t first_byte = serialized[i];
      if (first_byte & (1U << 7)) {
        max_log_derivative |= (first_byte & ~(1U << 7)) << 16;
        i++;
      }
      max_log_derivative |= (u_int8_t) serialized[i++] << 8;
      max_log_derivative |= (u_int8_t) serialized[i++];
      MutableEvaluation(eval)->Set(prob, proof_number, disproof_number, -max_log_derivative);
    }
    PN proof, disproof;
    if (lower_ + 1 <= last_1) {
      proof = serialized[i++];
    }
    if (upper_ - 1 >= first_0) {
      disproof = serialized[i++];
    }
    SerializableBooleanVector proof_disproof_numbers(serialized.begin() + i, serialized.end(), 8 * (serialized.size() - i));
    int j = 0;
    if (lower_ + 1 <= last_1) {
      MutableEvaluation(lower_ + 1)->SetProving(proof);

      for (int eval = lower_ + 3; eval <= last_1; eval += 2) {
        while (proof_disproof_numbers.Get(j++) == 0) {
          proof++;
        }
        MutableEvaluation(eval)->SetProving(proof);
      }
    }
    if (upper_ - 1 >= first_0) {
      MutableEvaluation(upper_ - 1)->SetDisproving(disproof);

      for (int eval = upper_ - 3; eval >= first_0; eval -= 2) {
        while (proof_disproof_numbers.Get(j++) == 0) {
          disproof++;
        }
        MutableEvaluation(eval)->SetDisproving(disproof);
      }
    }
  }

  std::vector<char> Serialize() const {
    // 0 - 12: Board
    assert(NThreadsWorking() == 0);
    std::vector<char> result;
    SerializedBoard board = ToBoard().Serialize();
    result.insert(result.end(), board.begin(), board.end());

    // 13 - 13+max(1, 3*n_fathers).
    if (n_fathers_ == 0) {
      if (father_flips_.empty()) {
        result.insert(result.end(), {0, 0, 0});
      } else {
        for (int i = 0; i < father_flips_.size(); ++i) {
          auto father_flip = father_flips_[i];
          father_flip = father_flip << 2 | (i == father_flips_.size() - 1 ? 0 : 1);
          result.insert(result.end(), {(char) (father_flip % 256), (char) ((father_flip >> 8) % 256), (char) ((father_flip >> 16) % 256)});
        }
      }
    } else {
      for (int i = 0; i < n_fathers_; ++i) {
        TreeNode* father = fathers_[i];
        auto child_to_move = GetUniqueNextBoardsWithPass(father->ToBoard());
        std::pair<Square, BitPattern> move = child_to_move.at(ToBoard());
        auto flip_serialized = SerializeFlip(move.first, move.second);
        assert(flip_serialized < (1 << 22));  // Max 22 bits
        flip_serialized = flip_serialized << 2 | (i == n_fathers_ - 1 ? 0 : 1);
        result.insert(result.end(), {(char) (flip_serialized % 256), (char) ((flip_serialized >> 8) % 256), (char) ((flip_serialized >> 16) % 256)});
      }
    }

    float descendants = (float) descendants_;
    result.insert(result.end(), (char*) &descendants, (char*) &descendants + sizeof(float));

    int last_1 = kMinEval - 1;
    int first_0 = kMaxEval + 1;

    for (int i = WeakLower(); i <= WeakUpper(); i += 2) {
      if (GetEvaluation(i).ProbGreaterEqual() == 1) {
        assert(GetEvaluation(i).MaxLogDerivative() == kLogDerivativeMinusInf);
        assert(GetEvaluation(i).DisproofNumber() == std::numeric_limits<float>::infinity());
        last_1 = i;
      }
      if (GetEvaluation(i).ProbGreaterEqual() == 0) {
        assert(GetEvaluation(i).MaxLogDerivative() == kLogDerivativeMinusInf);
        assert(GetEvaluation(i).ProofNumber() == std::numeric_limits<float>::infinity());
        first_0 = i;
        break;
      }
    }
    result.insert(result.end(), {(char) lower_, (char) upper_, (char) last_1, (char) first_0, (char) is_leaf_});

    for (int i = last_1 + 2; i <= first_0 - 2; i += 2) {
      const Evaluation& evaluation = GetEvaluation(i);
      assert(evaluation.MaxLogDerivative() != kLogDerivativeMinusInf);
      result.push_back((char) evaluation.ProbGreaterEqualSmall());
      result.push_back((char) evaluation.ProofNumberSmall());
      result.push_back((char) evaluation.DisproofNumberSmall());
      uint32_t max_log_derivative = -evaluation.MaxLogDerivative();
      assert(max_log_derivative > 0);
      assert(max_log_derivative < -kLogDerivativeMinusInf);
      assert(max_log_derivative < 8388607 /*2^23-1*/);
      if (max_log_derivative <= 32767 /*2^15-1*/) {
        result.push_back((char) (max_log_derivative >> 8));
        result.push_back((char) (max_log_derivative & 255));
      } else {
        max_log_derivative |= (1U << 23);
        result.push_back((char) ((max_log_derivative >> 16) & 255));
        result.push_back((char) ((max_log_derivative >> 8) & 255));
        result.push_back((char) (max_log_derivative & 255));
      }
    }
    if (lower_ + 1 <= last_1) {
      result.push_back((char) GetEvaluation(lower_ + 1).ProofNumberSmall());
    }
    if (upper_ - 1 >= first_0) {
      result.push_back((char) GetEvaluation(upper_ - 1).DisproofNumberSmall());
    }

    SerializableBooleanVector proof_disproof_numbers;
    if (lower_ + 1 <= last_1) {
      PN last = GetEvaluation(lower_ + 1).ProofNumberSmall();
      for (int i = lower_ + 3; i <= last_1; i += 2) {
        PN current = (char) GetEvaluation(i).ProofNumberSmall();
        while (last < current) {
          proof_disproof_numbers.PushBack(false);
          last++;
        }
        proof_disproof_numbers.PushBack(true);
      }
    }
    if (upper_ - 1 >= first_0) {
      PN last = GetEvaluation(upper_ - 1).DisproofNumberSmall();
      for (int i = upper_ - 3; i >= first_0; i -= 2) {
        PN current = (char) GetEvaluation(i).DisproofNumberSmall();
        while (last < current) {
          proof_disproof_numbers.PushBack(false);
          last++;
        }
        proof_disproof_numbers.PushBack(true);
      }
    }
    const std::vector<char>& serialized_proof_disproof_numbers = proof_disproof_numbers.Serialize();
    result.insert(result.end(), serialized_proof_disproof_numbers.begin(), serialized_proof_disproof_numbers.end());
    assert(result.size() <= kMaxNodeSize);
    return result;
  }

  bool IsLeaf() const override { return is_leaf_; }

  bool Equals(const BookTreeNode<Book>& other, bool approx) const {
    return TreeNode::Equals((const TreeNode&) other, approx);
  }

  template<class ChildPointer>
  void AddChildrenToBook(const std::vector<ChildPointer>& children) {
    assert(IsLeaf());
    is_leaf_ = false;
    std::vector<TreeNode*> children_in_book;

    for (const auto& [board, unused_move] : GetUniqueNextBoardsWithPass(ToBoard())) {
      BookTreeNode* child_in_book = nullptr;
      ChildPointer new_child = nullptr;
      auto child_in_book_opt = book_->Get(board);
      for (ChildPointer child : children) {
        if (child->ToBoard().Unique() == board) {
          new_child = child;
        }
      }
      if (child_in_book_opt) {
        child_in_book = child_in_book_opt.value();
        if (new_child != nullptr) {
          child_in_book->AddDescendants(new_child->GetNVisited());
        }
      } else {
        assert(new_child != nullptr);
        child_in_book = book_->Add(*new_child);
      }
      assert(child_in_book != nullptr);
      children_in_book.push_back(child_in_book);
    }
    SetChildren(children_in_book);
    assert(AreChildrenCorrect());
  }

  std::vector<TreeNode*> Fathers() {
    GetFathersFromBook();
    return TreeNode::Fathers();
  }

  TreeNode** ChildrenStart() override {
    GetChildrenFromBook();
    return children_;
  }

  TreeNode** ChildrenEnd() override {
    GetChildrenFromBook();
    return children_ + n_children_;
  }

  void SetLeafNoLock(EvalLarge leaf_eval, Square depth, Eval weak_lower, Eval weak_upper) override {
    throw std::logic_error("Cannot update leaf eval in book node.");
    }

  template<ChildValueType type>
  BookTreeNode* BestChild(int eval_goal, float n_thread_multiplier) {
    return (BookTreeNode*) this->TreeNode::template BestChild<type>(eval_goal, n_thread_multiplier);
  }
  BookTreeNode* BestChild(int eval_goal, float n_thread_multiplier) {
    return (BookTreeNode*) this->TreeNode::BestChild(eval_goal, n_thread_multiplier);
  }

  void SetLeafNeverSolved() {
    assert(weak_lower_ = kMinEval + 1);
    assert(weak_upper_ = kMaxEval - 1);
    lower_ = -64;
    upper_ = 64;
    for (int i = weak_lower_; i <= weak_upper_; i += 2) {
      MutableEvaluation(i)->SetLeaf(player_, opponent_, EvalToEvalLarge(i), EvalToEvalLarge(i), 1, n_empties_);
      assert(GetEvaluation(i).ProbGreaterEqual() > 0 &&
             GetEvaluation(i).ProbGreaterEqual() < 1);
    }
    leaf_eval_ = 4;
  }

 private:
  bool is_leaf_;
  Book* book_;
  std::vector<CompressedFlip> father_flips_;

  void GetFathersFromBook() override {
    for (CompressedFlip father_flip : father_flips_) {
      auto move_and_flip = DeserializeFlip(father_flip);
      Square move = move_and_flip.first;
      BitPattern flip = move_and_flip.second;

      std::optional<BookTreeNode<Book>*> father = book_->Get(Board(opponent_ & ~flip, (player_ | flip) & ~(1ULL << move)));
      assert(father);
      (*father)->GetChildrenFromBook();
    }
  }

  void GetChildrenFromBook() {
    assert(!IsLeaf());
    if (n_children_ > 0) {
      return;
    }
    std::vector<TreeNode*> children;
    for (const auto& [child_board, unused_move] : GetUniqueNextBoardsWithPass(ToBoard())) {
      auto child = book_->Get(child_board);
      children.push_back((TreeNode*) child.value());
    }
    SetChildrenNoLock(children);
  }

  bool AreChildrenCorrect() override {
    GetChildrenFromBook();
    auto next_boards = GetUniqueNextBoardsWithPass(ToBoard());
    if (n_children_ != next_boards.size()) {
      throw ChildError("Wrong children size");
    }

    for (int i = 0; i < n_children_; ++i) {
      TreeNode* child = children_[i];
      bool found = false;
      for (auto& [next_board, unused_move] : next_boards) {
        if (child->ToBoard() == next_board) {
          assert(!found);  // If it's true, there is a duplicate in the flips.
          found = true;
        }
      }
      if (!found) {
        throw ChildError("Missing child");
      }
    }
    return true;
  }
};

#endif  // OTHELLOSENSEI_BOOK_BOOK_TREE_NODE_H
