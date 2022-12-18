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

    while (true) {
      CompressedFlip father = (u_int8_t) serialized[i] | ((u_int8_t) serialized[i+1] << 8) | ((u_int8_t) serialized[i+2] << 16);
      i += 3;
      if (father == 0) {
        assert(fathers_.empty());
        break;
      }
      fathers_.insert(father >> 2);
      if ((father & 1) == 0) {
        break;
      }
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
    result.insert(result.end(), {(char) lower_, (char) upper_, (char) last_1, (char) first_0, (char) is_leaf_});

    if (fathers_.size() == 0) {
      result.insert(result.end(), {0, 0, 0});
    }
    int i = 0;
    for (CompressedFlip father : fathers_) {
      assert(father < (1 << 22));  // Max 22 bits
      father = father << 2 | (++i == fathers_.size() ? 0 : 1);
      result.insert(result.end(), {(char) (father % 256), (char) ((father >> 8) % 256), (char) ((father >> 16) % 256)});
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
      if (fathers_ != other.fathers_ || is_leaf_ != other.is_leaf_) {
        return false;
      }
    }
    return true;
  }

  bool operator==(const BookTreeNode& other) const = default;

  void Merge(const BookTreeNode& other) {
    assert(ToBoard() == other.ToBoard());
    float total_descendants = descendants_ + other.descendants_;
    std::set<CompressedFlip> fathers = fathers_;
    fathers.insert(other.fathers_.begin(), other.fathers_.end());
    if (other.descendants_ > descendants_ ||
        (other.descendants_ == descendants_ && this < &other)) {
      SetupFromOther(other);
    }
    descendants_ = total_descendants;
    fathers_ = fathers;
  }

  bool UpdateFather(const std::vector<BookTreeNode*>& children) {
    return BaseTreeNode<BookTreeNode>::UpdateFather(
        children.begin(), children.end());
  }

  void AddFather(CompressedFlip father) {
    fathers_.insert(father);
  }

  void SetChildren(const std::vector<BookTreeNode*>& children) {
    assert(AreChildrenCorrect(children));
    is_leaf_ = false;
    auto child_to_move = GetUniqueNextBoardsWithPass(ToBoard());
    for (BookTreeNode* child : children) {
      std::pair<Square, BitPattern> move = child_to_move.at(child->ToBoard());
      child->AddFather(SerializeFlip(move.first, move.second));
    }
    UpdateFather(children);
  }

  std::vector<Board> Fathers() const {
    std::vector<Board> result;
    for (CompressedFlip compressed_flip : fathers_) {
      std::pair move_and_flip = DeserializeFlip(compressed_flip);
      Square move = move_and_flip.first;
      BitPattern flip = move_and_flip.second;
      result.push_back(Board(opponent_ & ~flip, (player_ | flip) & ~(1ULL << move)).Unique());
    }
    return result;
  }
 private:
  bool is_leaf_;
  std::set<CompressedFlip> fathers_;

  bool AreChildrenCorrect(const std::vector<BookTreeNode*> children) {
    auto expected_children = GetUniqueNextBoardsWithPass(ToBoard());
    if (children.size() < expected_children.size()) {
      std::cout << "Wrong children size " << children.size()
          << "(should be at least " << expected_children.size() << ").\nBoard:\n"
          << ToBoard();
      return false;
    }
    for (auto expected_child : expected_children) {
      Board unique = expected_child.first;
      bool found = false;
      for (auto child : children) {
        if (child->ToBoard() == unique) {
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
