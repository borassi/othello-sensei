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
#include <vector>

#include "../board/board.h"
#include "../evaluatederivative/tree_node.h"
#include "../evaluatederivative/endgame_time_estimator.h"
#include "value_file.h"

class HashMapNode {
 public:
  HashMapNode() : size_(0), offset_(0) {}
  HashMapNode(u_int8_t size, BookFileOffset offset) : size_(size), offset_(offset) {}

  BookFileOffset Offset() const { return offset_; }
  u_int8_t Size() const { return size_; }
  bool IsEmpty() const { return size_ == 0; }
  bool operator==(const HashMapNode& other) const = default;

 private:
  BookFileOffset offset_;
  u_int8_t size_;
} __attribute__((packed));

std::ostream& operator<<(std::ostream& stream, const HashMapNode& n);

class BookTreeNode : public BaseTreeNode<BookTreeNode> {
 public:
  BookTreeNode() = delete;
  BookTreeNode(const TreeNode& other) {
    SetupFromOther(other);
    is_leaf_ = true;
  }
  BookTreeNode(BookTreeNode&& other) {
    fathers_ = other.fathers_;
    SetupFromOther(other);
  }
  BookTreeNode(const BookTreeNode& other) {
    fathers_ = other.fathers_;
    SetupFromOther(other);
  }

  BookTreeNode(const std::vector<char>& serialized) {
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
    int n_fathers = serialized[i++];

    for (int j = 0; j < n_fathers; ++j) {
      HashMapNode* node = (HashMapNode*) &serialized[i];
      fathers_.push_back(*node);
      i += sizeof(HashMapNode);
    }
    EnlargeEvaluations();

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

  template<class T>
  void SetupFromOther(const T& other) {
    min_evaluation_ = -63;
    weak_lower_ = -63;
    weak_upper_ = 63;
    is_leaf_ = other.IsLeaf();
    Copy(other);
    Board unique = other.ToBoard().Unique();
    player_ = unique.Player();
    opponent_ = unique.Opponent();
  }

  BookTreeNode& operator=(BookTreeNode&& other) {
    SetupFromOther(other);
    return *this;
  }

  std::vector<char> Serialize() {
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
    result.insert(result.end(), {(char) lower_, (char) upper_, (char) last_1, (char) first_0});
    result.insert(result.end(), {is_leaf_, (char) fathers_.size()});

    for (int i = 0; i < fathers_.size(); ++i) {
      char* tmp = (char*) &fathers_[i];
      result.insert(result.end(), (char*) &fathers_[i], ((char*) &fathers_[i]) + sizeof(HashMapNode));
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
    return result;
  }

  bool IsLeaf() const override { return is_leaf_; }

  bool EqualsExceptDescendants(const BookTreeNode& other) const {
    if (Board(player_, opponent_).Unique() != Board(other.player_, other.opponent_).Unique() ||
        lower_ != other.lower_ ||
        upper_ != other.upper_) {
      return false;
    }

    for (int i = WeakLower(); i <= WeakUpper(); i += 2) {
      if (GetEvaluation(i) != other.GetEvaluation(i)) {
        return false;
      }
    }
    return true;
  }

  bool Equals(const BookTreeNode& other, bool approx, bool fathers) const {
    if (Board(player_, opponent_).Unique() != Board(other.player_, other.opponent_).Unique() ||
        lower_ != other.lower_ ||
        upper_ != other.upper_) {
      return false;
    }

    for (int i = WeakLower(); i <= WeakUpper(); i += 2) {
      if (GetEvaluation(i) != other.GetEvaluation(i)) {
        return false;
      }
    }
    if (approx) {
      if (abs((double) GetNVisited() / other.GetNVisited() - 1) > 1E-6) {
        return false;
      }
    } else {
      if (GetNVisited() != other.GetNVisited()) {
        return false;
      }
    }
    if (fathers) {
      if (fathers_.size() != other.fathers_.size() || is_leaf_ != other.is_leaf_) {
        return false;
      }
      for (int i = 0; i < fathers_.size(); ++i) {
        if (fathers_[i] != other.fathers_[i]) {
          return false;
        }
      }
    }
    return true;
  }

  bool operator==(const BookTreeNode& other) const = default;

  void Merge(const BookTreeNode& other) {
    assert(ToBoard() == other.ToBoard());
    float total_descendants = descendants_ + other.descendants_;
    std::vector<HashMapNode> fathers(fathers_);
    fathers.insert(fathers.end(), other.fathers_.begin(), other.fathers_.end());
    if (other.descendants_ > descendants_ ||
        (other.descendants_ == descendants_ && this < &other)) {
      SetupFromOther(other);
    }
    descendants_ = total_descendants;
    fathers_ = fathers;
  }

  bool UpdateFather(const std::vector<const BookTreeNode*>& children) {
    return BaseTreeNode<BookTreeNode>::UpdateFather(
        children.begin(), children.end());
  }

  void AddFather(const HashMapNode& father) {
    assert(!father.IsEmpty());
    fathers_.push_back(father);
  }

  void SetChildren(const std::vector<const BookTreeNode*>& children) {
    assert(AreChildrenCorrect(children));
    is_leaf_ = false;
    UpdateFather(children);
  }

  std::vector<HashMapNode> Fathers() const {
    return fathers_;
  }
 private:
  bool is_leaf_;
  std::vector<HashMapNode> fathers_;

  bool AreChildrenCorrect(const std::vector<const BookTreeNode*> children) {
    std::vector<Board> expected_children = GetNextBoardsWithPass(ToBoard());
    if (children.size() != expected_children.size()) {
      std::cout << "Wrong children size " << children.size()
          << "(should be " << expected_children.size() << ").\nBoard:\n"
          << ToBoard();
      return false;
    }
    for (auto expected_child : expected_children) {
      Board unique = expected_child.Unique();
      bool found = false;
      for (auto child : children) {
        if (child->ToBoard() == unique) {
          assert(!found);
          found = true;
        }
      }
      if (!found) {
        std::cout << "Missing child\n" << unique << "Available children\n";
        for (auto child : children) {
          std::cout << child->ToBoard() << "\n";
        }
        return false;
      }
    }
    return true;
  }
};

std::ostream& operator<<(std::ostream& stream, const BookTreeNode& b);


#endif  // OTHELLOSENSEI_BOOK_BOOK_TREE_NODE_H
