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

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>

#include "book_visitor_test_utils.h"
#include "merger.h"
#include "../book/book.h"
#include "../board/board.h"
#include "../book/test_utils.h"

using ::testing::UnorderedElementsAre;

TEST(BookVisitor, MergeDisjoint) {
  std::vector<std::string> lines = {"e6f4"};
  std::vector<std::string> lines2 = {"e6f6"};
  Book<> book = BookWithPositions(lines, /*evals=*/{}, /*skip=*/{}, /*visited=*/{}, kTempDir + "/book");
  Book<> book2 = BookWithPositions(lines2, /*evals=*/{}, /*skip=*/{}, /*visited=*/{}, kTempDir + "/book2");
  Node old_e6f4 = *book.Get(Board("e6f4"));
  Node old_e6f4c3 = *book.Get(Board("e6f4c3"));
  book.Commit();
  book2.Commit();

  BookVisitorMerge(book2, book).VisitAll();

  EXPECT_EQ(book.Size(), 11);
  EXPECT_EQ(*book.Get(Board("e6f4c3")), old_e6f4c3);
  EXPECT_EQ(*book.Get(Board("e6f4")), old_e6f4);
  EXPECT_EQ(*book.Get(Board("e6f6")), *book2.Get(Board("e6f6")));
  EXPECT_EQ(*book.Get(Board("e6f6f5")), *book2.Get(Board("e6f6f5")));
  EXPECT_THAT(
      book.Roots(),
      UnorderedElementsAre(Board("e6f4").Unique(), Board("e6f6").Unique()));
}

TEST(BookVisitorMerge, MergeSameLine) {
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

  BookVisitorMerge(book2, book).VisitAll();

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

TEST(BookVisitorMerge, MergeDifferentStarts) {
  Book<> book = BookWithPositions({"e6"}, {}, {}, {});
  Book<> book2 = BookWithPositions({"", "e6"}, {}, {}, {});
  BookVisitorMerge(book2, book).VisitAll();
  EXPECT_EQ(book.Size(), 5);
  EXPECT_EQ(book.Get(Board("e6f4"))->GetNVisited(), 2);
  EXPECT_EQ(book.Get(Board("e6"))->GetNVisited(), 8);
  // new board + 4 children + 3 grandchildren
  EXPECT_EQ(book.Get(Board(""))->GetNVisited(), 8);
  EXPECT_TRUE(book.Mutable(Board(""))->AreChildrenCorrect());
  EXPECT_TRUE(book.Mutable(Board("e6"))->AreChildrenCorrect());
  EXPECT_THAT(book.Roots(), UnorderedElementsAre(Board("").Unique()));
}

TEST(BookVisitorMerge, MergeTranspositions) {
  Book<> book = BookWithPositions({"e6"});
  Book<> book2 = BookWithPositions({"e6f4", "e6f4c3", "e6f4d3", "e6f4c3c4", "e6f4d3c4", "e6f4c3c4d3"});
  BookVisitorMerge(book2, book).VisitAll();

  EXPECT_THAT(book.Roots(), UnorderedElementsAre(Board("e6").Unique()));
  EXPECT_EQ(*book.Get(Board("e6f4c3c4d3")), *book2.Get(Board("e6f4c3c4d3")));
  EXPECT_EQ(*book.Get(Board("e6f4c3c4d3d6")), *book2.Get(Board("e6f4c3c4d3d6")));
}

TEST(BookVisitorMerge, ResetDescendants) {
  Book(kTempDir + "child1").Clean();
  Book(kTempDir).Clean();
  Book(kTempDir + "copy").Clean();
  Book book = BookWithPositions({"e6", "e6f4", "e6f4c3", "e6f4d3", "e6f4c3c4", "e6f4d3c4", "e6f4c3c4d3"});
  Book book_copy(kTempDir + "copy");
  book_copy.Clean();
  BookVisitorMerge(book, book_copy).VisitAll();

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
    BookVisitorMerge(book_child, book).VisitAll();

    BookVisitorToVectorNoTransposition nodes_book(book);
    BookVisitorToVectorNoTransposition nodes_copy(book_copy);
    ASSERT_THAT(nodes_book.Get(), ::testing::UnorderedElementsAreArray(nodes_copy.Get()));

    book_child.Clean();
    book_child = RemoveDescendants(book, kTempDir + "child1");
  }
}

TEST(BookVisitorMerge, ResetDescendantsDouble) {
  Book(kTempDir + "child1").Clean();
  Book(kTempDir + "child2").Clean();
  Book(kTempDir).Clean();
  Book(kTempDir + "copy").Clean();
  Book book = BookWithPositions({"e6", "e6f4", "e6f6"});
  Book book_copy(kTempDir + "copy");
  BookVisitorMerge(book, book_copy).VisitAll();

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
    BookVisitorMerge(book_child1, book).VisitAll();
    BookVisitorMerge(book_child2, book).VisitAll();

    BookVisitorToVectorNoTransposition nodes_book(book);
    BookVisitorToVectorNoTransposition nodes_copy(book_copy);
    ASSERT_THAT(nodes_book.Get(), ::testing::UnorderedElementsAreArray(nodes_copy.Get()));

    book_child1.Clean();
    book_child1 = RemoveDescendants(book, kTempDir + "child1");
    book_child2.Clean();
    book_child2 = RemoveDescendants(book, kTempDir + "child2");
  }
}