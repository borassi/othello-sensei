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

#include "visitor.h"
#include "book_visitor_test_utils.h"
#include "../book/test_utils.h"

using ::testing::UnorderedElementsAre;

TEST(BookVisitor, IteratorBasic) {
  std::vector<std::string> lines = {"e6f4", "e6f4c3"};
  Book book = BookWithPositions(lines, {
      {Board("e6f4c3"), 2},
      {Board("e6f4c3c4"), -2},
      {Board("e6f4c3c6"), -2},
      {Board("e6f4c3d6"), -2},
      {Board("e6f4c3e7"), 2},
    },
    {}, {}, kTempDir, true);
  book.Commit();

  BookVisitorToVectorNoTransposition visitor(book);
  visitor.VisitString("e6f4");
  EXPECT_THAT(visitor.Get(), UnorderedElementsAre(
      GetVisitedNode(book, "", "e6f4", FIRST_VISIT),
      GetVisitedNode(book, "", "e6f4c3", FIRST_VISIT, 2),
      GetVisitedNode(book, "", "e6f4d3", LEAF),
      GetVisitedNode(book, "", "e6f4e3", LEAF),
      GetVisitedNode(book, "", "e6f4f3", LEAF),
      GetVisitedNode(book, "", "e6f4g3", LEAF),
      GetVisitedNode(book, "", "e6f4c3c4", LEAF, 2),
      GetVisitedNode(book, "", "e6f4c3c6", LEAF, 2),
      GetVisitedNode(book, "", "e6f4c3d6", LEAF, 2),
      GetVisitedNode(book, "", "e6f4c3e7", LEAF, 2, 4),
      GetVisitedNode(book, "", "e6f4c3", LAST_VISIT, 2),
      GetVisitedNode(book, "", "e6f4", LAST_VISIT)
  ));
  CheckVectorHasRightOrder(visitor.Get());
}

TEST(BookVisitor, DoesNotRepeatFirstMove) {
  std::vector<std::string> lines = {""};
  Book book = BookWithPositions(lines, /*evals=*/{}, /*skips=*/{}, /*visited=*/{}, kTempDir, true);
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
  Book book = BookWithPositions(lines, /*evals=*/{}, /*skips=*/{}, /*visited=*/{}, kTempDir, true);
  book.Commit();

  BookVisitorToVectorNoTransposition visitor(book);
  visitor.VisitString("e6f6f5");
  EXPECT_THAT(visitor.Get(), UnorderedElementsAre(
      GetVisitedNode(book, "", "e6f6f5", FIRST_VISIT),
      GetVisitedNode(book, "", "e6f6f5d6", LEAF),
      GetVisitedNode(book, "", "e6f6f5", LAST_VISIT)
  ));
}

TEST(BookVisitor, IteratorTranspositions) {
  Book<> book(kTempDir);
  book.Clean();
  book.Add(*TestTreeNode(Board("e6f4c3c4"), 0, -63, 63, 10));
  book.Add(*TestTreeNode(Board("e6f4d3c4"), 0, -63, 63, 10));

  std::vector<Node> children1;
  for (Board child : GetNextBoardsWithPass(Board("e6f4c3c4"))) {
    children1.push_back(*TestTreeNode(child, 0, -63, 63, 10, 0));
  }
  std::vector<Node> children2;
  for (Board child : GetNextBoardsWithPass(Board("e6f4d3c4"))) {
    children2.push_back(*TestTreeNode(child, 0, -63, 63, 10, 0));
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

TEST(BookVisitor, IteratorTranspositionsChild) {
  Book<> book(kTempDir);
  book.Clean();
  book.Add(*TestTreeNode(Board("e6f4c3c4"), 0, -63, 63, 10, 0));
  book.Add(*TestTreeNode(Board("e6f4d3c4"), 0, -63, 63, 10, 0));

  std::vector<Node> children1;
  for (Board child : GetNextBoardsWithPass(Board("e6f4c3c4"))) {
    children1.push_back(*TestTreeNode(child, 0, -63, 63, 10, 0));
  }
  std::vector<Node> children2;
  for (Board child : GetNextBoardsWithPass(Board("e6f4d3c4"))) {
    children2.push_back(*TestTreeNode(child, 0, -63, 63, 10, 0));
  }
  std::vector<Node> children3;
  for (Board child : GetNextBoardsWithPass(Board("e6f4c3c4d3"))) {
    children3.push_back(*TestTreeNode(child, 0, -63, 63, 10, 0));
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

TEST(BookVisitor, WorksWithPass) {
  Book<> book(kTempDir);
  book.Clean();
  std::string sequence = "e6f6g6g7g8h8c4f8f5h6h7g5h5h4f7";
  Board start(sequence);
  book.Add(*TestTreeNode(start, 0, -63, 63, 10, 0));

  std::vector<Node> children;
  for (Board child : GetNextBoardsWithPass(start)) {
    children.push_back(*TestTreeNode(child, 0, -63, 63, 10, 0));
  }
  book.AddChildren(start, children);

  Board before_pass = start.Next(GetFlip(MoveToSquare("e8"), start.Player(), start.Opponent()));
  Board after_pass = before_pass.Next(0);
  book.AddChildren(before_pass, {*TestTreeNode(after_pass, 0, -63, 63, 10)});

  BookVisitorToVectorNoTransposition visitor(book);
  visitor.VisitSequence(Sequence(sequence));
  EXPECT_THAT(visitor.Get(), ::testing::IsSupersetOf({
      VisitedNode{*book.Get(start), FIRST_VISIT, Sequence(sequence)},
      VisitedNode{*book.Get(before_pass), FIRST_VISIT, Sequence(sequence + "e8")},
      VisitedNode{*book.Get(after_pass), LEAF, Sequence(sequence + "e8")},
      VisitedNode{*book.Get(before_pass), LAST_VISIT, Sequence(sequence + "e8")},
      VisitedNode{*book.Get(start), LAST_VISIT, Sequence(sequence)}
  }));
  CheckVectorHasRightOrder(visitor.Get());
}

TEST(BookVisitor, VisitAll) {
  Book<> book(kTempDir);
  book.Clean();
  book.Add(*TestTreeNode(Board("e6f4"), 0, -63, 63, 10, 0));
  book.Add(*TestTreeNode(Board("e6f6"), 0, -63, 63, 10));

  std::vector<Node> children;
  for (Board child : GetNextBoardsWithPass(Board("e6f4"))) {
    children.push_back(*TestTreeNode(child, 0, -63, 63, 10, 0));
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