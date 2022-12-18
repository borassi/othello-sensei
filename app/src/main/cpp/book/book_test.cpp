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

#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <unordered_map>

#include "book.h"
#include "book_tree_node.h"
#include "../board/bitpattern.h"
#include "../board/board.h"
#include "../evaluatederivative/tree_node.h"

const std::string kTempDir = "app/testdata/tmp/book_test";

using ::testing::UnorderedElementsAre;
using ::testing::ElementsAreArray;

std::shared_ptr<TreeNode> TestTreeNode(Board b, Eval leaf_eval, Eval weak_lower, Eval weak_upper, NVisited n_visited) {
  std::shared_ptr<TreeNode> t(new TreeNode());
  t->Reset(b.Player(), b.Opponent(), 4, 0);
//  Eval weak_lower, Eval weak_upper, EvalLarge leaf_eval, Square depth, NVisited n_visited)
  t->SetLeaf(weak_lower, weak_upper, EvalToEvalLarge(leaf_eval), 4, n_visited);
  return t;
}
BookTreeNode TestBookTreeNode(BitPattern player, BitPattern opponent, Eval leaf_eval, Eval weak_lower, Eval weak_upper, NVisited n_visited) {
  auto node = TestTreeNode(Board(player, opponent), leaf_eval, weak_lower, weak_upper, n_visited);
  return BookTreeNode(*node);
}

TEST(Book, Basic) {
  Book position_to_data(kTempDir);
  position_to_data.Clean();
  position_to_data.Put(TestBookTreeNode(0, 1, 0, -5, 5, 1000000000012));
//  position_to_data.Print();
  EXPECT_TRUE(position_to_data.Get(0, 1)->Equals(TestBookTreeNode(0, 1, 0, -5, 5, 1000000000012), true, true));
  EXPECT_EQ(position_to_data.Get(0, 2), std::nullopt);
  EXPECT_EQ(position_to_data.Size(), 1);
}

TEST(Book, Overwrite) {
  Book book(kTempDir);
  book.Clean();
  book.Put(TestBookTreeNode(0, 2, 0, -1, 9, 10));
  book.Put(TestBookTreeNode(0, 2, 0, 1, 9, 11));
  EXPECT_EQ(book.Get(0, 2), TestBookTreeNode(0, 2, 0, 1, 9, 21));
}

TEST(Book, DoNotOverwrite) {
  Book book(kTempDir);
  book.Clean();
  book.Put(TestBookTreeNode(0, 2, 0, -1, 9, 10));
  book.Put(TestBookTreeNode(0, 2, 0, 1, 9, 9));
  EXPECT_EQ(book.Get(0, 2), TestBookTreeNode(0, 2, 0, -1, 9, 19));
}

TEST(Book, ReloadBook) {
  Book book(kTempDir);
  book.Clean();
  book.Put(TestBookTreeNode(0, 1, 0, -5, 5, 10));
  book.Put(TestBookTreeNode(0, 2, 2, 1, 9, 20));

  Book book1(kTempDir);
  EXPECT_EQ(book1.Get(0, 1), TestBookTreeNode(0, 1, 0, -5, 5, 10));
  EXPECT_EQ(book1.Get(0, 2), TestBookTreeNode(0, 2, 2, 1, 9, 20));
}

TEST(Book, Unique) {
  Book book(kTempDir);
  book.Clean();
  book.Put(TestBookTreeNode(0, 1ULL << 7, 1, 1, 3, 10));
  EXPECT_EQ(book.Get(0, 1), TestBookTreeNode(0, 1ULL << 7, 1, 1, 3, 10));
  EXPECT_EQ(book.Get(0, 1ULL << 56), TestBookTreeNode(0, 1ULL << 7, 1, 1, 3, 10));
  EXPECT_EQ(book.Get(0, 1ULL << 63), TestBookTreeNode(0, 1ULL << 7, 1, 1, 3, 10));
}

//TEST(Book, LargeOne) {
//  std::unordered_map<Board, std::optional<BookTreeNode>> expected;
//  std::vector<Board> old_boards;
//  Book actual(kTempDir);
//  actual.Clean();
//
//  for (int i = 0; i < 10000; ++i) {
//    if (i % 1000 == 0 || (i < 1000 && i % 100 == 0)) {
//      std::cout << i << "\n";
//    }
//    Board b = RandomBoard();
//    Board unique = b.Unique();
//    Eval lower = (i % 64) * 2 - 63;
//    Eval upper = std::min(lower + (i % 32) * 2, 63);
//    EvalLarge eval = ((i + 24) % 126) - 63;
//    NVisited n_visited = rand();
//    BookTreeNode node = TestBookTreeNode(b.Player(), b.Opponent(), eval, lower, upper, n_visited);
//    old_boards.push_back(b);
//    if (expected[unique]) {
//      expected[unique]->Merge(node);
//    } else {
//      expected[unique] = node;
//    }
//    actual.Put(node);
//
//    int test_i = rand() % (i+1);
//    Board test_board = old_boards[test_i];
//    ASSERT_TRUE(actual.Get(test_board.Player(), test_board.Opponent())->Equals(
//                *expected[test_board.Unique()], true, true));
//
//    if (i < 100) {
//      for (int test_i = 0 ; test_i < i; ++test_i) {
//        Board test_board = old_boards[test_i];
//        ASSERT_TRUE(actual.Get(test_board.Player(), test_board.Opponent())->Equals(
//                    *expected[test_board.Unique()], true, true));
//      }
//    }
//    if (i < 2000) {
//      assert(actual.IsSizeOK());
//    }
//    if (rand() % 100 == 0) {
//      actual = Book(kTempDir);
//    }
//  }
//}

TEST(Book, AddChildren) {
  Book book(kTempDir);
  book.Clean();

  auto e6_node = TestTreeNode(Board("e6"), 0, -63, 63, 10);
  BookTreeNode e6(*e6_node);
  auto e6f4_node = TestTreeNode(Board("e6f4"), -30, -63, 63, 10);
  BookTreeNode e6f4(*e6f4_node);
  auto e6f6_node = TestTreeNode(Board("e6f6"), -30, -63, 63, 10);
  BookTreeNode e6f6(*e6f6_node);
  auto e6d6_node = TestTreeNode(Board("e6d6"), -10, -63, 63, 10);
  BookTreeNode e6d6(*e6d6_node);

  book.Put(e6);
  book.AddChildren(e6.ToBoard(), {&e6f4, &e6f6, &e6d6});
  e6_node->SetChildren({e6f4_node.get(), e6f6_node.get(), e6d6_node.get()});

  EXPECT_EQ(book.Get(Board("e6f4")), e6f4);
  EXPECT_EQ(book.Get(Board("e6f6")), e6f6);
  EXPECT_EQ(book.Get(Board("e6d6")), e6d6);

  EXPECT_TRUE(book.Get(Board("e6"))->Equals(BookTreeNode(*e6_node), true, false));
  EXPECT_FALSE(book.Get(Board("e6"))->IsLeaf());
  auto fathers = book.Get(Board("e6f4"))->Fathers();
  EXPECT_EQ(fathers.size(), 1);
  auto father = book.Get(*fathers.begin());
  EXPECT_TRUE(father->Equals(BookTreeNode(*e6_node), true, false));
}

TEST(Book, AddChildrenTransposition) {
  Book book(kTempDir);
  book.Clean();

  BookTreeNode e6f4c3c4(*TestTreeNode(Board("e6f4c3c4"), 0, -63, 63, 10));
  book.Put(e6f4c3c4);
  std::vector<BookTreeNode> memory;
  memory.reserve(15);
  std::vector<BookTreeNode*> e6f4c3c4_successors;
  for (Board b : GetNextBoardsWithPass(e6f4c3c4.ToBoard())) {
    memory.push_back(*TestTreeNode(b, 0, -63, 63, 10));
    e6f4c3c4_successors.push_back(&memory[memory.size() - 1]);
  }

  BookTreeNode e6f4d3c4(*TestTreeNode(Board("e6f4d3c4"), 0, -63, 63, 10));
  book.Put(e6f4d3c4);
  std::vector<BookTreeNode*> e6f4d3c4_successors;
  for (Board b : GetNextBoardsWithPass(e6f4d3c4.ToBoard())) {
    memory.push_back(*TestTreeNode(b, 0, -63, 63, 10));
    e6f4d3c4_successors.push_back(&memory[memory.size() - 1]);
  }

  book.AddChildren(Board("e6f4c3c4"), e6f4c3c4_successors);
  book.AddChildren(Board("e6f4d3c4"), e6f4d3c4_successors);

  auto fathers = book.Get(Board("e6f4c3c4d3"))->Fathers();
  EXPECT_THAT(fathers, UnorderedElementsAre(Board("e6f4c3c4").Unique(), Board("e6f4d3c4").Unique()));
}

TEST(Book, AddChildrenStartingPosition) {
  Book book(kTempDir);
  book.Clean();

  BookTreeNode start(*TestTreeNode(Board(), 0, -63, 63, 10));
  book.Put(start);
  std::vector<BookTreeNode> memory;
  memory.reserve(4);
  std::vector<BookTreeNode*> successors;
  for (Board b : GetNextBoardsWithPass(Board())) {
    memory.push_back(*TestTreeNode(b, 0, -63, 63, 10));
    successors.push_back(&memory[memory.size() - 1]);
  }

  book.AddChildren(Board(), successors);

  auto fathers = book.Get(Board("e6"))->Fathers();
  EXPECT_THAT(fathers, UnorderedElementsAre(Board()));
}

TEST(Book, UpdateFathers) {
  Book book(kTempDir);
  book.Clean();

  std::vector<BookTreeNode> memory;
  memory.reserve(40);

  BookTreeNode e6f4(*TestTreeNode(Board("e6f4"), -10, -63, 63, 10));
  book.Put(e6f4);

  for (auto moves : {"e6f4", "e6f4c3", "e6f4c3c4", "e6f4d3", "e6f4d3c4"}) {
    std::vector<BookTreeNode*> successors;
    for (Board b : GetNextBoardsWithPass(Board(moves))) {
      int eval = (b == Board("e6f4c3c4d3")) ? 0 : 41;
      memory.push_back(*TestTreeNode(b, eval, -63, 63, 10));
      successors.push_back(&memory[memory.size() - 1]);
    }
    book.AddChildren(Board(moves), successors);
  }
  // Has two "descendants" with evaluation 0.
  EXPECT_NEAR(book.Get(Board("e6f4"))->GetEval(), 1, 1);

  // Overwrite: more nodes.
  BookTreeNode e6f4c3c4d3(*TestTreeNode(Board("e6f4c3c4d3"), 20, -63, 63, 100));
  book.Put(e6f4c3c4d3);

  EXPECT_NEAR(book.Get(Board("e6f4c3c4d3"))->GetEval(), 20, 1E-4);
  EXPECT_NEAR(book.Get(Board("e6f4c3c4"))->GetEval(), -20, 1E-4);
  EXPECT_NEAR(book.Get(Board("e6f4d3c4"))->GetEval(), -20, 1E-4);
  EXPECT_NEAR(book.Get(Board("e6f4c3"))->GetEval(), 20, 1E-4);
  EXPECT_NEAR(book.Get(Board("e6f4d3"))->GetEval(), 20, 1E-4);
  // Has two "descendants" with evaluation 0.
  EXPECT_NEAR(book.Get(Board("e6f4"))->GetEval(), -19, 1);
}