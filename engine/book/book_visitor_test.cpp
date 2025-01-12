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

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>

#include "book_visitor.h"
#include "book.h"
#include "../board/board.h"
#include "test_utils.h"

using ::testing::UnorderedElementsAre;

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

VisitedNode GetVisitedNode(const Book<kBookVersion>& book, const std::string& pre_sequence, const std::string& sequence, NodeType node_type) {
  return VisitedNode {*book.Get(Board(pre_sequence + sequence)), node_type, Sequence(sequence)};
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
    nodes_.push_back({node, LEAF, sequence_});
  }
  void PreVisitInternalNode(Node& node) override {
    nodes_.push_back({node, FIRST_VISIT, sequence_});
  }
  void PostVisitInternalNode(Node& node) override {
    nodes_.push_back({node, LAST_VISIT, sequence_});
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
  void PreVisitInternalNode(Node& node) override {
    nodes_.push_back({node, FIRST_VISIT, sequence_});
  }
  void PostVisitInternalNode(Node& node) override {
    nodes_.push_back({node, LAST_VISIT, sequence_});
  }

 private:
  std::vector<VisitedNode> nodes_;
};

TEST(BookVisitor, IteratorBasic) {
  std::vector<std::string> lines = {"e6f4", "e6f4c3"};
  Book book = BookWithPositions(lines, /*evals=*/{}, /*skips=*/{}, /*visited=*/{});
  book.Commit();

  BookVisitorToVectorNoTransposition visitor(book);
  visitor.VisitString("e6f4");
  EXPECT_THAT(visitor.Get(), UnorderedElementsAre(
      GetVisitedNode(book, "", "e6f4", FIRST_VISIT),
      GetVisitedNode(book, "", "e6f4c3", FIRST_VISIT),
      GetVisitedNode(book, "", "e6f4d3", LEAF),
      GetVisitedNode(book, "", "e6f4e3", LEAF),
      GetVisitedNode(book, "", "e6f4f3", LEAF),
      GetVisitedNode(book, "", "e6f4g3", LEAF),
      GetVisitedNode(book, "", "e6f4c3c4", LEAF),
      GetVisitedNode(book, "", "e6f4c3c6", LEAF),
      GetVisitedNode(book, "", "e6f4c3d6", LEAF),
      GetVisitedNode(book, "", "e6f4c3e7", LEAF),
      GetVisitedNode(book, "", "e6f4c3", LAST_VISIT),
      GetVisitedNode(book, "", "e6f4", LAST_VISIT)
  ));
  CheckVectorHasRightOrder(visitor.Get());
}

TEST(BookVisitor, DoesNotRepeatFirstMove) {
  std::vector<std::string> lines = {""};
  Book book = BookWithPositions(lines, /*evals=*/{}, /*skips=*/{}, /*visited=*/{});
  book.Commit();

  BookVisitorToVectorNoTransposition visitor(book);
  visitor.VisitString("");
  EXPECT_THAT(visitor.Get(), UnorderedElementsAre(
      GetVisitedNode(book, "", "", FIRST_VISIT),
      GetVisitedNode(book, "", "e6", LEAF),
      GetVisitedNode(book, "", "", LAST_VISIT)
  ));
}

TEST(BookVisitor, DoesNotRepeatDiagonal) {
  std::vector<std::string> lines = {"e6f6f5"};
  Book book = BookWithPositions(lines, /*evals=*/{}, /*skips=*/{}, /*visited=*/{});
  book.Commit();

  BookVisitorToVectorNoTransposition visitor(book);
  visitor.VisitString("e6f6f5");
  EXPECT_THAT(visitor.Get(), UnorderedElementsAre(
      GetVisitedNode(book, "", "e6f6f5", FIRST_VISIT),
      GetVisitedNode(book, "", "e6f6f5d6", LEAF),
      GetVisitedNode(book, "", "e6f6f5", LAST_VISIT)
  ));
}

TEST(BookVisitor, IteratorTraspositions) {
  Book<> book(kTempDir);
  book.Clean();
  book.Add(*TestTreeNode(Board("e6f4c3c4"), 0, -63, 63, 10));
  book.Add(*TestTreeNode(Board("e6f4d3c4"), 0, -63, 63, 10));

  std::vector<Node> children1;
  for (Board child : GetNextBoardsWithPass(Board("e6f4c3c4"))) {
    children1.push_back(*RandomTestTreeNode(child));
  }
  std::vector<Node> children2;
  for (Board child : GetNextBoardsWithPass(Board("e6f4d3c4"))) {
    children2.push_back(*RandomTestTreeNode(child));
  }
  book.AddChildren(Board("e6f4c3c4"), children1);
  book.AddChildren(Board("e6f4d3c4"), children2);

  BookVisitorToVectorNoTransposition visitor(book);
  visitor.VisitString("e6f4c3c4");
  visitor.VisitString("e6f4d3c4");
  EXPECT_THAT(visitor.Get(), UnorderedElementsAre(
      GetVisitedNode(book, "", "e6f4c3c4", FIRST_VISIT),
      GetVisitedNode(book, "", "e6f4c3c4", LAST_VISIT),
      GetVisitedNode(book, "", "e6f4d3c4", FIRST_VISIT),
      GetVisitedNode(book, "", "e6f4d3c4", LAST_VISIT),
      GetVisitedNode(book, "", "e6f4c3c4b3", LEAF),
      GetVisitedNode(book, "", "e6f4c3c4c5", LEAF),
      GetVisitedNode(book, "", "e6f4c3c4d3", LEAF),
      GetVisitedNode(book, "", "e6f4c3c4e3", LEAF),
      GetVisitedNode(book, "", "e6f4c3c4f3", LEAF),
      GetVisitedNode(book, "", "e6f4c3c4g3", LEAF),
      GetVisitedNode(book, "", "e6f4d3c4b3", LEAF),
      GetVisitedNode(book, "", "e6f4d3c4b5", LEAF),
      GetVisitedNode(book, "", "e6f4d3c4e3", LEAF),
      GetVisitedNode(book, "", "e6f4d3c4f3", LEAF),
      GetVisitedNode(book, "", "e6f4d3c4f5", LEAF),
      GetVisitedNode(book, "", "e6f4d3c4g3", LEAF)
  ));
  CheckVectorHasRightOrder(visitor.Get());
}

TEST(BookVisitor, IteratorTraspositionsChild) {
  Book<> book(kTempDir);
  book.Clean();
  book.Add(*TestTreeNode(Board("e6f4c3c4"), 0, -63, 63, 10));
  book.Add(*TestTreeNode(Board("e6f4d3c4"), 0, -63, 63, 10));

  std::vector<Node> children1;
  for (Board child : GetNextBoardsWithPass(Board("e6f4c3c4"))) {
    children1.push_back(*TestTreeNode(child, 0, -63, 63, 10));
  }
  std::vector<Node> children2;
  for (Board child : GetNextBoardsWithPass(Board("e6f4d3c4"))) {
    children2.push_back(*TestTreeNode(child, 0, -63, 63, 10));
  }
  std::vector<Node> children3;
  for (Board child : GetNextBoardsWithPass(Board("e6f4c3c4d3"))) {
    children3.push_back(*RandomTestTreeNode(child));
  }
  book.AddChildren(Board("e6f4c3c4"), children1);
  book.AddChildren(Board("e6f4d3c4"), children2);
  book.AddChildren(Board("e6f4c3c4d3"), children3);

  BookVisitorToVectorNoTransposition visitor(book);
  visitor.VisitString("e6f4c3c4");
  visitor.VisitString("e6f4d3c4");
  EXPECT_THAT(visitor.Get(), UnorderedElementsAre(
      GetVisitedNode(book, "", "e6f4c3c4", FIRST_VISIT),
      GetVisitedNode(book, "", "e6f4c3c4", LAST_VISIT),
      GetVisitedNode(book, "", "e6f4d3c4", FIRST_VISIT),
      GetVisitedNode(book, "", "e6f4d3c4", LAST_VISIT),
      GetVisitedNode(book, "", "e6f4c3c4d3", FIRST_VISIT),
      GetVisitedNode(book, "", "e6f4c3c4d3", LAST_VISIT),
      GetVisitedNode(book, "", "e6f4c3c4b3", LEAF),
      GetVisitedNode(book, "", "e6f4c3c4c5", LEAF),
      GetVisitedNode(book, "", "e6f4c3c4e3", LEAF),
      GetVisitedNode(book, "", "e6f4c3c4f3", LEAF),
      GetVisitedNode(book, "", "e6f4c3c4g3", LEAF),
      GetVisitedNode(book, "", "e6f4d3c4b3", LEAF),
      GetVisitedNode(book, "", "e6f4d3c4b5", LEAF),
      GetVisitedNode(book, "", "e6f4d3c4e3", LEAF),
      GetVisitedNode(book, "", "e6f4d3c4f3", LEAF),
      GetVisitedNode(book, "", "e6f4d3c4f5", LEAF),
      GetVisitedNode(book, "", "e6f4d3c4g3", LEAF),
      GetVisitedNode(book, "", "e6f4c3c4d3c2", LEAF),
      GetVisitedNode(book, "", "e6f4c3c4d3c6", LEAF),
      GetVisitedNode(book, "", "e6f4c3c4d3d6", LEAF),
      GetVisitedNode(book, "", "e6f4c3c4d3e2", LEAF),
      GetVisitedNode(book, "", "e6f4c3c4d3e7", LEAF),
      GetVisitedNode(book, "", "e6f4c3c4d3f7", LEAF)
  ));
  CheckVectorHasRightOrder(visitor.Get());
}

TEST(BookVisitor, VisitAll) {
  Book<> book(kTempDir);
  book.Clean();
  book.Add(*TestTreeNode(Board("e6f4"), 0, -63, 63, 10));
  book.Add(*TestTreeNode(Board("e6f6"), 0, -63, 63, 10));

  std::vector<Node> children;
  for (Board child : GetNextBoardsWithPass(Board("e6f4"))) {
    children.push_back(*TestTreeNode(child, 0, -63, 63, 10));
  }
  book.AddChildren(Board("e6f4"), children);

  BookVisitorToVectorNoTransposition visitor(book);
  visitor.VisitAll();
  EXPECT_THAT(visitor.Get(), UnorderedElementsAre(
      GetVisitedNode(book, "e6f4", "", FIRST_VISIT),
      GetVisitedNode(book, "e6f4", "c3", LEAF),
      GetVisitedNode(book, "e6f4", "d3", LEAF),
      GetVisitedNode(book, "e6f4", "e3", LEAF),
      GetVisitedNode(book, "e6f4", "f3", LEAF),
      GetVisitedNode(book, "e6f4", "g3", LEAF),
      GetVisitedNode(book, "e6f4", "", LAST_VISIT),
      GetVisitedNode(book, "e6f6", "", LEAF)
  ));
}

TEST(BookVisitor, MergeDisjoint) {
  std::vector<std::string> lines = {"e6f4"};
  std::vector<std::string> lines2 = {"e6f6"};
  Book<> book = BookWithPositions(lines, /*evals=*/{}, /*skip=*/{}, /*visited=*/{}, kTempDir + "/book");
  Book<> book2 = BookWithPositions(lines2, /*evals=*/{}, /*skip=*/{}, /*visited=*/{}, kTempDir + "/book2");
  Node old_e6f4 = *book.Get(Board("e6f4"));
  Node old_e6f4c3 = *book.Get(Board("e6f4c3"));
  book.Commit();
  book2.Commit();

  BookMerge(book2, book).VisitAll();

  EXPECT_EQ(book.Size(), 11);
  EXPECT_EQ(*book.Get(Board("e6f4c3")), old_e6f4c3);
  EXPECT_EQ(*book.Get(Board("e6f4")), old_e6f4);
  EXPECT_EQ(*book.Get(Board("e6f6")), *book2.Get(Board("e6f6")));
  EXPECT_EQ(*book.Get(Board("e6f6f5")), *book2.Get(Board("e6f6f5")));
  EXPECT_THAT(
      book.Roots(),
      UnorderedElementsAre(Board("e6f4").Unique(), Board("e6f6").Unique()));
}

TEST(BookMerge, MergeSameLine) {
  Book<> book = BookWithPositions(
      {"e6", "e6f4"},
      /*evals=*/{{Board("e6f4"), 1}},
      /*skip=*/{},
      /*visited=*/{{Board("e6"), 1}, {Board("e6f4"), 3}});
  Book<> book2 = BookWithPositions(
      {"e6", "e6f6"},
      /*evals=*/{{Board("e6f6"), 10}},
      /*skip=*/{},
      /*visited=*/{{Board("e6"), 3}, {Board("e6f6"), 2}});

  BookMerge(book2, book).VisitAll();

  EXPECT_EQ(book.Size(), 13); // e6, par, diag, perp, diag+4, perp+5
  EXPECT_EQ(book.Get(Board("e6f4"))->GetNVisited(), 9); // e6f4(3) + successors of e6f4(5) + element in other book(1)
//  EXPECT_EQ(book.Get(Board("e6f6"))->GetEval(), 1);
  EXPECT_EQ(book.Get(Board("e6f6"))->GetNVisited(), 7); // e6f6(2) + successors of e6f6(4) + element in other book(1)
  // First book: 1 from start, 8 from e6f4, 2 from other children.
  // Second book: 3 from start, 6 from e6f6, 2 from other children.
  EXPECT_EQ(book.Get(Board("e6"))->GetNVisited(), 22);
  EXPECT_TRUE(book.Mutable(Board("e6"))->AreChildrenCorrect());
  EXPECT_TRUE(book.Mutable(Board("e6f4"))->AreChildrenCorrect());
  EXPECT_TRUE(book.Mutable(Board("e6f6"))->AreChildrenCorrect());
  EXPECT_THAT(book.Roots(), UnorderedElementsAre(Board("e6").Unique()));
}

TEST(BookMerge, MergeDifferentStarts) {
  Book<> book = BookWithPositions({"e6"}, {}, {}, {});
  Book<> book2 = BookWithPositions({"", "e6"}, {}, {}, {});
  BookMerge(book2, book).VisitAll();
  EXPECT_EQ(book.Size(), 5);
  EXPECT_EQ(book.Get(Board("e6f4"))->GetNVisited(), 2);
  EXPECT_EQ(book.Get(Board("e6"))->GetNVisited(), 8);
  // new board + 4 children + 3 grandchildren
  EXPECT_EQ(book.Get(Board(""))->GetNVisited(), 8);
  EXPECT_TRUE(book.Mutable(Board(""))->AreChildrenCorrect());
  EXPECT_TRUE(book.Mutable(Board("e6"))->AreChildrenCorrect());
  EXPECT_THAT(book.Roots(), UnorderedElementsAre(Board("").Unique()));
}

TEST(BookMerge, MergeTranspositions) {
  Book<> book = BookWithPositions({"e6"});
  Book<> book2 = BookWithPositions({"e6f4", "e6f4c3", "e6f4d3", "e6f4c3c4", "e6f4d3c4", "e6f4c3c4d3"});
  BookMerge(book2, book).VisitAll();

  EXPECT_THAT(book.Roots(), UnorderedElementsAre(Board("e6").Unique()));
}

TEST(BookMerge, ResetDescendants) {
  Book(kTempDir + "child1").Clean();
  Book(kTempDir).Clean();
  Book(kTempDir + "copy").Clean();
  Book book = BookWithPositions({"e6", "e6f4", "e6f4c3", "e6f4d3", "e6f4c3c4", "e6f4d3c4", "e6f4c3c4d3"});
  Book book_copy(kTempDir + "copy");
  book_copy.Clean();
  BookMerge(book, book_copy).VisitAll();

  Book book_child = RemoveDescendants(book, kTempDir + "child1");

  for (int i = 0; i < 5; ++i) {
    for (int j = 0; j < 10; ++j) {
      auto leaf_child = *LeafToUpdate<Book<>::BookNode>::BestDescendant(
          book_child.Mutable(Board("e6")), 0, kLessThenMinEval, {});
      auto leaf_copy = *LeafToUpdate<Book<>::BookNode>::BestDescendant(
          book_copy.Mutable(Board("e6")), 0, kLessThenMinEval, {});

      ASSERT_EQ(leaf_child.Leaf()->ToBoard(), leaf_copy.Leaf()->ToBoard());

      std::vector<Node> children;
      int n_visited;
      for (auto child_flip : GetUniqueNextBoardsWithPass(leaf_child.Leaf()->ToBoard())) {
        auto eval_result = RandomTestTreeNode(child_flip.first);
        eval_result->SetLeafNeverSolved();
        children.push_back(*eval_result);
        n_visited += eval_result->GetNVisited();
      }
      book_child.AddChildren(leaf_child.Leaf()->ToBoard(), children);
      book_copy.AddChildren(leaf_child.Leaf()->ToBoard(), children);
      leaf_child.Finalize(n_visited);
      leaf_copy.Finalize(n_visited);
    }
    BookMerge(book_child, book).VisitAll();

    BookVisitorToVectorNoTransposition nodes_book(book);
    BookVisitorToVectorNoTransposition nodes_copy(book_copy);
    ASSERT_THAT(nodes_book.Get(), ::testing::UnorderedElementsAreArray(nodes_copy.Get()));

    book_child.Clean();
    book_child = RemoveDescendants(book, kTempDir + "child1");
  }
}

TEST(BookMerge, ResetDescendantsDouble) {
  Book(kTempDir + "child1").Clean();
  Book(kTempDir + "child2").Clean();
  Book(kTempDir).Clean();
  Book(kTempDir + "copy").Clean();
  Book book = BookWithPositions({"e6", "e6f4", "e6f6"});
  Book book_copy(kTempDir + "copy");
  BookMerge(book, book_copy).VisitAll();

  Book<> book_child1 = RemoveDescendants(book, kTempDir + "child1");
  Book<> book_child2 = RemoveDescendants(book, kTempDir + "child2");

  for (int i = 0; i < 5; ++i) {
    for (int j = 0; j < 10; ++j) {
      bool diagonal = rand() % 2;
      std::string line = diagonal ? "e6f6" : "e6f4";
      auto& current_book = diagonal ? book_child2 : book_child1;
      auto leaf_child = *LeafToUpdate<Book<>::BookNode>::BestDescendant(
          current_book.Mutable(Board(line)), 0, kLessThenMinEval, {});
      auto leaf_copy = *LeafToUpdate<Book<>::BookNode>::BestDescendant(
          book_copy.Mutable(Board(line)), 0, kLessThenMinEval, {});

      ASSERT_EQ(leaf_child.Leaf()->ToBoard(), leaf_copy.Leaf()->ToBoard());

      std::vector<Node> children;
      int n_visited;
      for (auto child_flip : GetUniqueNextBoardsWithPass(leaf_child.Leaf()->ToBoard())) {
        auto eval_result = RandomTestTreeNode(child_flip.first);
        eval_result->SetLeafNeverSolved();
        children.push_back(*eval_result);
        n_visited += eval_result->GetNVisited();
      }
      current_book.AddChildren(leaf_child.Leaf()->ToBoard(), children);
      book_copy.AddChildren(leaf_child.Leaf()->ToBoard(), children);
      leaf_child.Finalize(n_visited);
      leaf_copy.Finalize(n_visited);
    }
    BookMerge(book_child1, book).VisitAll();
    BookMerge(book_child2, book).VisitAll();

    BookVisitorToVectorNoTransposition nodes_book(book);
    BookVisitorToVectorNoTransposition nodes_copy(book_copy);
    ASSERT_THAT(nodes_book.Get(), ::testing::UnorderedElementsAreArray(nodes_copy.Get()));

    book_child1.Clean();
    book_child1 = RemoveDescendants(book, kTempDir + "child1");
    book_child2.Clean();
    book_child2 = RemoveDescendants(book, kTempDir + "child2");
  }
}