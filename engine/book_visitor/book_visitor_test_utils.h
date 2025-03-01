/*
 * Copyright 2025 Michele Borassi
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

#include "visitor.h"

enum NodeType {
  LEAF = 0,
  FIRST_VISIT = 1,
  LAST_VISIT = 2,
};

struct VisitedNode {
  Node node;
  NodeType node_type;
  Sequence sequence;
  double error_black;
  double error_white;

  bool operator==(const VisitedNode& other) const {
    return
        node == other.node &&
        node_type == other.node_type &&
        int(error_black * 100) == other.error_black * 100 &&
        int(error_white * 100) == other.error_white * 100 &&
        sequence == other.sequence;
  }
};

std::ostream& operator<<(std::ostream& stream, const VisitedNode& n) {
  stream << "\n[" << n.sequence << "] - type: " << n.node_type << " - errors: " << n.error_black << " " << n.error_white << "\n" << n.node << "\n";
  return stream;
}

void CheckVectorHasRightOrder(const std::vector<VisitedNode>& nodes) {
  std::unordered_set<Board> visited;
  for (int i = 0; i < nodes.size(); ++i) {
    auto& node = nodes[i].node;
    auto& node_type = nodes[i].node_type;
    visited.insert(node.ToBoard());
    if (node_type == FIRST_VISIT) {
      auto children = GetUniqueNextBoardsWithPass(node.ToBoard());
      for (const Board& b : visited) {
        children.erase(b);
      }
      for (int j = i+1; j <= nodes.size(); ++j) {
        ASSERT_LT(j, nodes.size());
        auto& child = nodes[j].node;
        auto& child_type = nodes[j].node_type;
        if (child.ToBoard() == node.ToBoard()) {
          ASSERT_EQ(child_type, LAST_VISIT);
          ASSERT_TRUE(children.empty());
          break;
        }
        ASSERT_EQ(visited.find(child.ToBoard()), visited.end());
        children.erase(child.ToBoard());
      }
    }
  }
}

VisitedNode GetVisitedNode(
    const Book<kBookVersion>& book,
    const std::string& pre_sequence,
    const std::string& sequence,
    NodeType node_type,
    double error_black = 0,
    double error_white = 0) {
  return VisitedNode {*book.Get(Board(pre_sequence + sequence)), node_type, Sequence(sequence), error_black, error_white};
}

class BookVisitorToVectorNoTransposition : public BookVisitorNoTranspositions<kBookVersion> {
 public:
  typedef BookVisitorNoTranspositions<kBookVersion> BookVisitor;
  using typename BookVisitor::Book;
  using typename BookVisitor::BookNode;
  using BookVisitor::sequence_;

  using BookVisitorNoTranspositions::BookVisitorNoTranspositions;

  const std::vector<VisitedNode>& Get() {
    return nodes_;
  }

 protected:

  void VisitLeaf(Node& node) override {
    auto [error_black, error_white] = GetErrors();
    nodes_.push_back({node, LEAF, sequence_, error_black, error_white});
  }
  bool PreVisitInternalNode(Node& node) override {
    auto [error_black, error_white] = GetErrors();
    nodes_.push_back({node, FIRST_VISIT, sequence_, error_black, error_white});
    return true;
  }
  void PostVisitInternalNode(Node& node) override {
    auto [error_black, error_white] = GetErrors();
    nodes_.push_back({node, LAST_VISIT, sequence_, error_black, error_white});
  }

 private:
  std::vector<VisitedNode> nodes_;
};

class BookVisitorToVectorTransposition : public BookVisitor<kBookVersion> {
 public:
  typedef BookVisitor<kBookVersion> BookVisitor;
  using typename BookVisitor::Book;
  using typename BookVisitor::BookNode;
  using BookVisitor::sequence_;

  using BookVisitor::BookVisitor;

  const std::vector<VisitedNode>& Get() {
    return nodes_;
  }

 protected:

  void VisitLeaf(Node& node) override {
    nodes_.push_back({node, LEAF, sequence_});
  }
  bool PreVisitInternalNode(Node& node) override {
    nodes_.push_back({node, FIRST_VISIT, sequence_});
    return true;
  }
  void PostVisitInternalNode(Node& node) override {
    nodes_.push_back({node, LAST_VISIT, sequence_});
  }

 private:
  std::vector<VisitedNode> nodes_;
};