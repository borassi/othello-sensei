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
#include <vector>

#include "../board/board.h"
#include "../evaluatederivative/tree_node.h"
#include "../evaluatederivative/endgame_time_estimator.h"

class BookTreeNode : public BaseTreeNode {
 public:
  BookTreeNode() = delete;
  BookTreeNode(const BaseTreeNode& other) { Copy(other); }
  BookTreeNode(BookTreeNode&& other) { Copy(other); }
  BookTreeNode(const BookTreeNode& other) { Copy(other); }

  BookTreeNode(const std::vector<char>& serialized) {
    Board board = Board::Deserialize(serialized.begin());
    player_ = board.Player();
    opponent_ = board.Opponent();
    n_empties_ = board.NEmpties();
    int i = 13;
    descendants_ = *((float*) &(serialized[i]));
    i += sizeof(float);
    lower_ = (Eval) serialized[i++];
    upper_ = (Eval) serialized[i++];
    Eval last_1 = (Eval) serialized[i++];
    Eval first_0 = (Eval) serialized[i++];
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

  BookTreeNode& operator=(BookTreeNode&& other) {
    Copy(other);
    return *this;
  }

  void Copy(const BaseTreeNode& other) {
    Reset(other.Player(), other.Opponent());
    n_empties_ = other.NEmpties();
    lower_ = other.Lower();
    upper_ = other.Upper();
    descendants_ = other.GetNVisited();
    EnlargeEvaluations();

    auto other_proof_number = other.GetEvaluation(other.WeakLower()).ProofNumber();
    auto other_disproof_number = other.GetEvaluation(other.WeakUpper()).DisproofNumber();
    for (int i = kMinEval + 1; i <= kMaxEval - 1; i += 2) {
      if (i < other.WeakLower()) {
        MutableEvaluation(i)->SetProving(ConvertProofNumber(other_proof_number, other.WeakLower(), i));
      } else if (i > other.WeakUpper()) {
        MutableEvaluation(i)->SetDisproving(ConvertDisproofNumber(other_disproof_number, other.WeakUpper(), i));
      } else {
        *MutableEvaluation(i) = Evaluation(other.GetEvaluation(i));
      }
    }
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
    char* tmp = (char*) &descendants_;
    result.insert(result.end(), tmp, tmp + sizeof(float));
    result.insert(result.end(), {(char) lower_, (char) upper_, (char) last_1, (char) first_0});

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

  Eval WeakLower() const override { return -63; }
  Eval WeakUpper() const override { return 63; }
  Eval MinEvaluation() const override { return -63; }

  NVisited GetNVisited() const override {
    return (NVisited) round(descendants_);
  }

  void AddDescendants(NVisited descendants) override {
    descendants_ += descendants;
  }

  bool operator==(const BookTreeNode& other) const {
    if (Board(player_, opponent_).Unique() != Board(other.player_, other.opponent_).Unique() ||
        lower_ != other.lower_ ||
        upper_ != other.upper_ ||
        GetNVisited() != other.GetNVisited()) {
      return false;
    }

    for (int i = WeakLower(); i <= WeakUpper(); i += 2) {
      if (GetEvaluation(i) != other.GetEvaluation(i)) {
        return false;
      }
    }
    return true;
  }

  void Merge(const BookTreeNode& other) {
    assert(ToBoard().Unique() == other.ToBoard().Unique());
    float total_descendants = descendants_ + other.descendants_;
    if (other.descendants_ > descendants_) {
      Copy(other);
    }
    descendants_ = total_descendants;
  }

 private:
  float descendants_;

  std::vector<BaseTreeNode*> GetChildren() const override {
    return std::vector<BaseTreeNode*>();
  }

  std::vector<BaseTreeNode*> Fathers() const override {
    return std::vector<BaseTreeNode*>();
  }
};

std::ostream& operator<<(std::ostream& stream, const BookTreeNode& b);


#endif  // OTHELLOSENSEI_BOOK_BOOK_TREE_NODE_H
