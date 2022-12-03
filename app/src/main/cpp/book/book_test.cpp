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

using ::testing::ElementsAreArray;

BookTreeNode TestBookTreeNode(BitPattern player, BitPattern opponent, Eval leaf_eval, Eval weak_lower, Eval weak_upper, NVisited n_visited) {
  TreeNode t;
  t.Reset(player, opponent, 4, 0);
//  Eval weak_lower, Eval weak_upper, EvalLarge leaf_eval, Square depth, NVisited n_visited)
  t.SetLeaf(weak_lower, weak_upper, leaf_eval, 4, n_visited);
  return BookTreeNode(t);
}

TEST(Book, Basic) {
  Book position_to_data(kTempDir);
  position_to_data.Clean();
  position_to_data.Put(TestBookTreeNode(0, 1, 0, -5, 5, 1000000000012));
//  position_to_data.Print();
  EXPECT_EQ(position_to_data.Get(0, 1), TestBookTreeNode(0, 1, 0, -5, 5, 1000000000012));
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

TEST(Book, LargeOne) {
  std::unordered_map<Board, std::optional<BookTreeNode>> expected;
  std::vector<Board> old_boards;
  Book actual(kTempDir);
  actual.Clean();

  for (int i = 0; i < 100000; ++i) {
    if (i % 1000 == 0 || (i < 1000 && i % 100 == 0)) {
      std::cout << i << "\n";
    }
    Board b = RandomBoard();
    Board unique = b.Unique();
    Eval lower = (i % 64) * 2 - 63;
    Eval upper = std::min(lower + (i % 32) * 2, 63);
    EvalLarge eval = ((i + 24) % 511) - 255;
    NVisited n_visited = rand();
    BookTreeNode node = TestBookTreeNode(b.Player(), b.Opponent(), eval, lower, upper, n_visited);
    old_boards.push_back(b);
    if (expected[unique]) {
      expected[unique]->Merge(node);
    } else {
      expected[unique] = node;
    }
    actual.Put(node);

    int test_i = rand() % (i+1);
    Board test_board = old_boards[test_i];
    ASSERT_EQ(actual.Get(test_board.Player(), test_board.Opponent()),
              expected[test_board.Unique()]);

    if (i < 500) {
      for (int test_i = 0 ; test_i < i; ++test_i) {
        Board test_board = old_boards[test_i];
        ASSERT_EQ(actual.Get(test_board.Player(), test_board.Opponent()),
                  expected[test_board.Unique()]);
      }
    }
    if (i < 10000) {
      assert(actual.IsSizeOK());
    }
    if (rand() % 100 == 0) {
      actual = Book(kTempDir);
    }
  }
}
