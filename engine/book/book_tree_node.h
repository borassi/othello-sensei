/*
 * Copyright 2022-2024 Michele Borassi
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
#include "../utils/serializable_boolean_vector.h"
#include "value_file.h"

// Max fathers = 140656 (see test).
constexpr int kMaxNodeSize = 13 /*board*/ + 3 * 140656 /*fathers*/ + 4 /*descendants*/ + 5 /*bounds*/ + 7 * 64 /*evals*/;

template<int version>
class Book;
class TestBook;

template<class Book, int version>
class BookTreeNode : public TreeNode {
 public:
  BookTreeNode(Book* book, const Node& node) : TreeNode(node), book_(book) {
    assert(node.WeakUpper() >= node.Lower() - 1);
    assert(node.WeakLower() <= node.Upper() + 1);
    Board b = node.ToBoard().Unique();
    player_ = b.Player();
    opponent_ = b.Opponent();
    n_empties_ = b.NEmpties();
    depth_ = kNoSquare;

    CopyAndEnlargeToAllEvals(node);
  }

  static std::unique_ptr<BookTreeNode> Deserialize(Book* book, const std::vector<char>& serialized) {
    std::vector<CompressedFlip> father_flips;
    Node n = Node::Deserialize(serialized, version, &father_flips);
    auto result = std::make_unique<BookTreeNode>(book, n);
    result->GetFathersFromBook(father_flips);
    return result;
  }

  std::vector<char> Serialize() const {
    // 0 - 12: Board
    assert(NThreadsWorking() == 0);
    std::vector<char> result;
    SerializedBoard board = ToBoard().Serialize();
    result.insert(result.end(), board.begin(), board.end());

    // 13 - 13+max(1, 3*n_fathers).
    if (n_fathers_ == 0) {
      result.insert(result.end(), {0, 0, 0});
    } else {
      for (unsigned int i = 0; i < n_fathers_; ++i) {
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

    int last_1 = lower_ - 1;
    int first_0 = upper_ + 1;

    for (int i = WeakLower(); i <= WeakUpper(); i += 2) {
      if (GetEvaluation(i).ProbGreaterEqual() == 1) {
        assert(GetEvaluation(i).MaxLogDerivative() == kLogDerivativeMinusInf);
        assert(GetEvaluation(i).DisproofNumber() == std::numeric_limits<float>::max());
        last_1 = i;
      }
      if (GetEvaluation(i).ProbGreaterEqual() == 0) {
        assert(GetEvaluation(i).MaxLogDerivative() == kLogDerivativeMinusInf);
        assert(GetEvaluation(i).ProofNumber() == std::numeric_limits<float>::max());
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

    SerializableBooleanVector serialized_bool;
    if (version >= 1) {
      // Max: 64 * 8 - 64 * 8 = 128 * 8 = 1024 => 11 bits.
      uint32_t leaf_eval_small = leaf_eval_ - kMinEvalLarge;
      serialized_bool.PushBack(leaf_eval_small, 11);
    }

    if (lower_ + 1 <= last_1) {
      PN last = GetEvaluation(lower_ + 1).ProofNumberSmall();
      for (int i = lower_ + 3; i <= last_1; i += 2) {
        PN current = (char) GetEvaluation(i).ProofNumberSmall();
        while (last < current) {
          serialized_bool.PushBack(false);
          last++;
        }
        serialized_bool.PushBack(true);
      }
    }
    if (upper_ - 1 >= first_0) {
      PN last = GetEvaluation(upper_ - 1).DisproofNumberSmall();
      for (int i = upper_ - 3; i >= first_0; i -= 2) {
        PN current = (char) GetEvaluation(i).DisproofNumberSmall();
        while (last < current) {
          serialized_bool.PushBack(false);
          last++;
        }
        serialized_bool.PushBack(true);
      }
    }
    const std::vector<char>& serialized_bool_converted = serialized_bool.Serialize();
    result.insert(result.end(), serialized_bool_converted.begin(), serialized_bool_converted.end());
    assert(result.size() <= kMaxNodeSize);
    return result;
  }

  bool Equals(const BookTreeNode<Book, version>& other, bool approx) const {
    return TreeNode::Equals((const TreeNode&) other, approx);
  }

  std::vector<Node> Fathers() override {
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

  void SetLeafEval(EvalLarge leaf_eval, Square depth) override {
    throw std::logic_error("Cannot update leaf eval in book node.");
  }

  template<ChildValueType type>
  BookTreeNode* BestChild(int eval_goal, float n_thread_multiplier) {
    return (BookTreeNode*) TreeNode::BestChild(eval_goal, n_thread_multiplier);
  }
  BookTreeNode* BestChild(int eval_goal, float n_thread_multiplier) {
    return (BookTreeNode*) TreeNode::BestChild(eval_goal, n_thread_multiplier);
  }

  int NFathers() const {
    return n_fathers_;
  }

  bool HasFathers() const {
    return NFathers() > 0;
  }

  bool IsLeafNoLock() const final {
    return this->Node::IsLeaf();
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
      for (auto& next_board_move : next_boards) {
        if (child->ToBoard() == next_board_move.first) {
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

  virtual bool IsLockableLeaf(Eval solved_lower, Eval solved_upper) override {
    return IsLeafNoLock();
  }

 protected:
  std::optional<std::lock_guard<std::mutex>> GetGuard() const override {
    return std::nullopt;
  }

 private:
  Book* book_;

  friend class TestBook;
  friend class ::Book<version>;

  void GetFathersFromBook(const std::vector<CompressedFlip>& father_flips) {
    assert(n_fathers_ == 0);
    for (CompressedFlip father_flip : father_flips) {
      auto move_flip = DeserializeFlip(father_flip);
      auto move = move_flip.first;
      auto flip = move_flip.second;
      BookTreeNode<Book, version>* father =
          book_->Mutable(Board(opponent_ & ~flip, (player_ | flip) & ~(1ULL << move)));
      assert(father);
      father->GetChildrenFromBook();
    }
  }

  void GetChildrenFromBook() {
    assert(!IsLeafNoLock());
    if (n_children_ > 0) {
      return;
    }
    // This is a trick to prevent GetChildrenFromBook calling Mutable calling
    // GetFathersFromBook calling GetChildrenFromBook.
    n_children_ = 255;
    std::vector<TreeNode*> children;
    for (const auto& child_board_move : GetUniqueNextBoardsWithPass(ToBoard())) {
      auto child = book_->Mutable(child_board_move.first);
      assert(child);
      children.push_back((TreeNode*) child);
    }
    SetChildrenNoLock(children);
  }
};

#endif  // OTHELLOSENSEI_BOOK_BOOK_TREE_NODE_H
