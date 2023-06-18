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
#include "value_file.h"

template<class Book>
class BookTreeNode : public TreeNode {
 public:
  BookTreeNode(Book* book, const TreeNode& node) : TreeNode(), book_(book), is_leaf_(true) {
    Board b = node.ToBoard().Unique();
    player_ = b.Player();
    opponent_ = b.Opponent();

    min_evaluation_ = -63;
    weak_lower_ = -63;
    weak_upper_ = 63;
    lower_ = node.Lower();
    upper_ = node.Upper();
    leaf_eval_ = 0;
    EnlargeEvaluationsInternal();
    descendants_ = node.GetNVisited();

    auto proof_number = node.GetEvaluation(node.WeakLower()).ProofNumber();
    auto disproof_number = node.GetEvaluation(node.WeakUpper()).DisproofNumber();
    for (int i = kMinEval + 1; i <= kMaxEval - 1; i += 2) {
      if (i < node.WeakLower()) {
        MutableEvaluation(i)->SetProving(ProofNumberToByte(ConvertProofNumber(proof_number, node.WeakLower(), i)));
      } else if (i > node.WeakUpper()) {
        MutableEvaluation(i)->SetDisproving(ProofNumberToByte(ConvertDisproofNumber(disproof_number, node.WeakUpper(), i)));
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
    EnlargeEvaluationsInternal();

    for (int eval = WeakLower(); eval <= WeakUpper(); eval += 2) {
      if (eval <= last_1) {
        MutableEvaluation(eval)->SetProving(serialized[i++]);
      } else if (eval >= first_0) {
        MutableEvaluation(eval)->SetDisproving(serialized[i++]);
      } else {
        Probability prob = serialized[i++];
        PN proof_number = serialized[i++];
        PN disproof_number = serialized[i++];
        int tmp = *((int*) &serialized[i]);
        i += 4;
        MutableEvaluation(eval)->Set(prob, proof_number, disproof_number, tmp);
      }
    }
  }

  void SetUpper(Eval upper) {
    upper_ = std::min(upper, upper_);
    for (int i = weak_upper_; i >= upper_; i -= 2) {
      MutableEvaluation(i)->SetDisproved();
    }
  }

  void SetLower(Eval lower) {
    lower_ = std::max(lower, lower_);
    for (int i = weak_lower_; i <= lower_; i += 2) {
      MutableEvaluation(i)->SetProved();
    }
  }

  std::vector<char> Serialize() const {
    assert(NThreadsWorking() == 0);
    std::vector<char> result;
    SerializedBoard board = ToBoard().Serialize();
    int last_1 = kMinEval - 1;
    int first_0 = kMaxEval + 1;

    for (int i = WeakLower(); i <= WeakUpper(); i += 2) {
      if (GetEvaluation(i).ProbGreaterEqual() == 1) {
        last_1 = i;
      }
      if (GetEvaluation(i).ProbGreaterEqual() == 0) {
        first_0 = i;
        break;
      }
    }
    result.insert(result.end(), board.begin(), board.end());
    float descendants = (float) descendants_;
    result.insert(result.end(), (char*) &descendants, (char*) &descendants + sizeof(float));
    result.insert(result.end(), {(char) lower_, (char) upper_, (char) last_1, (char) first_0, (char) is_leaf_});

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

    for (int i = WeakLower(); i <= WeakUpper(); i += 2) {
      Evaluation evaluation = GetEvaluation(i);
      if (i <= last_1) {
        result.push_back((char) evaluation.ProofNumberSmall());
      } else if (i >= first_0) {
        result.push_back((char) evaluation.DisproofNumberSmall());
      } else {
        result.push_back((char) evaluation.ProbGreaterEqualSmall());
        result.push_back((char) evaluation.ProofNumberSmall());
        result.push_back((char) evaluation.DisproofNumberSmall());
        int tmp = evaluation.MaxLogDerivative();
        for (int k = 0; k < 4; ++k) {
          result.push_back(*(((char*) &tmp) + k));
        }
      }
    }
    assert(result.size() < 256);
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
