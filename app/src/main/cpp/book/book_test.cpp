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
#include <utility>

#include "book.h"
#include "book_tree_node.h"
#include "test_utils.h"
#include "../board/bitpattern.h"
#include "../board/board.h"
#include "../evaluatederivative/tree_node.h"
#include "../utils/misc.h"

using ::testing::UnorderedElementsAre;
using ::testing::ElementsAreArray;

class BookParameterizedFixture : public ::testing::TestWithParam<bool> {
 protected:
  bool only_new;
};

TEST(Book, Basic) {
  TestBook test_book;
  Book book(kTempDir);
  book.Clean();
  // Updates this node.
  book.Get(0, 1)->Update(*TestTreeNode(Board(0ULL, 1ULL), 0, -5, 5, 1010));
  book.Get(0, 1)->AddDescendants(1010);
  // No update --> ignore
  book.Get(0, 2);
  book.Commit();

  EXPECT_EQ(*book.Get(0, 1), *GetTestBookTreeNode(&test_book, Board(0ULL, 1ULL), 0, -5, 5, 1010));
  EXPECT_FALSE(book.Get(0, 2)->IsValid());
  EXPECT_FALSE(book.Get(0, 3)->IsValid());
  EXPECT_EQ(book.Size(), 1);
}

TEST(Book, Reload) {
  TestBook test_book;
  Book book(kTempDir);
  book.Clean();
  // Updates this node.
  book.Get(0, 1)->Update(*TestTreeNode(Board(0ULL, 1ULL), 0, -5, 5, 2000));
  book.Get(0, 1)->AddDescendants(2000);
  // No update --> ignore
  book.Get(0, 2);
  book.Commit();
  Book book1(kTempDir);
  book.Get(0, 3)->Update(*TestTreeNode(Board(0ULL, 3ULL), 5, -5, 5, 100));
  book.Get(0, 3)->AddDescendants(100);
  book.Commit();

  EXPECT_EQ(*book.Get(0, 1), *GetTestBookTreeNode(&test_book, Board(0ULL, 1ULL), 0, -5, 5, 2000));
  EXPECT_FALSE(book.Get(0, 2)->IsValid());
  EXPECT_EQ(*book.Get(0, 3), *GetTestBookTreeNode(&test_book, Board(0ULL, 3ULL), 5, -5, 5, 100));
  EXPECT_EQ(book.Size(), 2);
}

TEST(Book, Overwrite) {
  Book book(kTempDir);
  TestBook test_book;
  book.Clean();
  book.Get(0, 1)->Update(*TestTreeNode(Board(0ULL, 1ULL), 0, -5, 5, 100));
  book.Get(0, 1)->AddDescendants(100);
  book.Get(0, 1)->Update(*TestTreeNode(Board(0ULL, 1ULL), 0, -7, 5, 100));
  book.Commit();
  EXPECT_EQ(*book.Get(0, 1), *GetTestBookTreeNode(&test_book, Board(0ULL, 1ULL), 0, -7, 5, 100));
}

TEST(Book, Unique) {
  Book book(kTempDir);
  TestBook test_book;
  book.Clean();
  book.Get(0, 1ULL << 7)->Update(*TestTreeNode(Board(0ULL, 1ULL << 7), 1, 1, 3, 10));
  book.Get(0, 1ULL << 7)->AddDescendants(10);
  book.Commit();
  EXPECT_EQ(*book.Get(0, 1), *GetTestBookTreeNode(&test_book, Board(0ULL, 1ULL << 7), 1, 1, 3, 10));
  EXPECT_EQ(*book.Get(0, 1ULL << 56), *GetTestBookTreeNode(&test_book, Board(0ULL, 1ULL << 7), 1, 1, 3, 10));
  EXPECT_EQ(*book.Get(0, 1ULL << 63), *GetTestBookTreeNode(&test_book, Board(0ULL, 1ULL << 7), 1, 1, 3, 10));
}

TEST(Book, LargeOne) {
  Book book(kTempDir);
  std::unordered_map<Board, std::shared_ptr<TreeNode>> expected;
  std::vector<Board> old_boards;
  book.Clean();

  for (int i = 0; i < 10000; ++i) {
    if (i % 1000 == 0 || (i < 1000 && i % 100 == 0)) {
      std::cout << i << "\n";
    }
    Board b = RandomBoard();
    Board unique = b.Unique();
    Eval lower = (i % 64) * 2 - 63;
    Eval upper = std::min(lower + (i % 32) * 2, 63);
    EvalLarge eval = ((i + 24) % 126) - 63;
    NVisited n_visited = rand() % 1000000;
    std::shared_ptr<TreeNode> node = TestTreeNode(b, eval, lower, upper, n_visited);
    old_boards.push_back(b);

    expected[unique] = TestTreeNode(b, eval, lower, upper, n_visited);

    book.Get(b)->Update(*node);
    book.Commit();

    int test_i = rand() % (i+1);
    Board test_board = old_boards[test_i];
    BookNode expected_node(&book, test_board);
    expected_node.Update(*expected[test_board.Unique()]);
    ASSERT_EQ(*book.Get(test_board.Player(), test_board.Opponent()),
                expected_node);

    if (i < 100) {
      for (int test_i = 0 ; test_i < i; ++test_i) {
        Board test_board = old_boards[test_i];
        BookNode expected_node(&book, test_board);
        expected_node.Update(*expected[test_board.Unique()]);
        ASSERT_EQ(*book.Get(test_board.Player(), test_board.Opponent()),
                    expected_node);
      }
    }
    if (i < 2000) {
      assert(book.IsSizeOK());
    }
    if (rand() % 100 == 0) {
      book = Book(kTempDir);
    }
  }
}

TEST(Book, AddChildren) {
  TestBook test_book;
  Book book(kTempDir);
  book.Clean();

  auto e6_node = TestTreeNode(Board("e6"), 0, -63, 63, 10);
  auto e6f4_node = TestTreeNode(Board("e6f4"), 30, -63, 63, 10);
  auto e6f6_node = TestTreeNode(Board("e6f6"), 30, -63, 63, 10);
  auto e6d6_node = TestTreeNode(Board("e6d6"), 10, -63, 63, 10);

  auto e6 = book.Get(Board("e6"));
  e6->Update(*e6_node);
  LeafToUpdate<BookNode>::Leaf({e6}).Finalize(10);
  e6->AddChildrenToBook<std::shared_ptr<TreeNode>>({e6f4_node, e6f6_node, e6d6_node});
  LeafToUpdate<BookNode>::Leaf({e6}).Finalize(30);

  book.Commit();

  EXPECT_EQ(*book.Get(Board("e6f4")), *GetTestBookTreeNode(&test_book, Board("e6f4"), 30, -63, 63, 10));
  EXPECT_EQ(*book.Get(Board("e6f6")), *GetTestBookTreeNode(&test_book, Board("e6f6"), 30, -63, 63, 10));
  EXPECT_EQ(*book.Get(Board("e6d6")), *GetTestBookTreeNode(&test_book, Board("e6d6"), 10, -63, 63, 10));

  EXPECT_FALSE(book.Get(Board("e6"))->IsLeaf());
  EXPECT_EQ(40, book.Get(Board("e6"))->GetNVisited());
  EXPECT_EQ(-10, book.Get(Board("e6"))->GetEval()) << *book.Get(Board("e6"));

  auto fathers = book.Get(Board("e6f6"))->Fathers();
  EXPECT_EQ(fathers.size(), 1);
  auto father = fathers[0];
  EXPECT_EQ(father, book.Get(Board("e6")));
  EXPECT_THAT(father->GetChildren(), UnorderedElementsAre(
      book.Get(Board("e6f4")),
      book.Get(Board("e6f6")),
      book.Get(Board("e6d6"))));
}

TEST(Book, AddChildrenDoubleCommit) {
  TestBook test_book;
  Book book(kTempDir);
  book.Clean();

  auto e6_node = TestTreeNode(Board("e6"), 0, -63, 63, 10);
  auto e6f4_node = TestTreeNode(Board("e6f4"), 30, -63, 63, 10);
  auto e6f6_node = TestTreeNode(Board("e6f6"), 30, -63, 63, 10);
  auto e6d6_node = TestTreeNode(Board("e6d6"), 10, -63, 63, 10);

  auto e6 = book.Get(Board("e6"));
  e6->Update(*e6_node);
  LeafToUpdate<BookNode>::Leaf({e6}).Finalize(10);
  e6->AddChildrenToBook<std::shared_ptr<TreeNode>>({e6f4_node, e6f6_node, e6d6_node});
  LeafToUpdate<BookNode>::Leaf({e6}).Finalize(30);

  book.Commit();

  book.Get(Board("e6f4"));
  book.Commit();

  EXPECT_EQ(*book.Get(Board("e6f4")), *GetTestBookTreeNode(&test_book, Board("e6f4"), 30, -63, 63, 10));
  EXPECT_EQ(*book.Get(Board("e6f6")), *GetTestBookTreeNode(&test_book, Board("e6f6"), 30, -63, 63, 10));
  EXPECT_EQ(*book.Get(Board("e6d6")), *GetTestBookTreeNode(&test_book, Board("e6d6"), 10, -63, 63, 10));

  EXPECT_FALSE(book.Get(Board("e6"))->IsLeaf());
  EXPECT_EQ(40, book.Get(Board("e6"))->GetNVisited());
  EXPECT_EQ(-10, book.Get(Board("e6"))->GetEval()) << *book.Get(Board("e6"));

  auto fathers = book.Get(Board("e6f6"))->Fathers();
  EXPECT_EQ(fathers.size(), 1);
  auto father = fathers[0];
  EXPECT_EQ(father, book.Get(Board("e6")));
  EXPECT_THAT(father->GetChildren(), UnorderedElementsAre(
      book.Get(Board("e6f4")),
      book.Get(Board("e6f6")),
      book.Get(Board("e6d6"))));
}

TEST(Book, AddChildrenTransposition) {
  TestBook test_book;
  Book book(kTempDir);
  book.Clean();
  std::vector<std::shared_ptr<TreeNode>> memory;

  auto e6f4c3c4 = TestTreeNode(Board("e6f4c3c4"), 0, -63, 63, 10);
  auto children1 = GetTreeNodeChildren(*e6f4c3c4, &memory);

  auto e6f4d3c4 = TestTreeNode(Board("e6f4d3c4"), 0, -63, 63, 10);
  auto children2 = GetTreeNodeChildren(*e6f4d3c4, &memory);

  auto e6f4c3c4_book = book.Get(e6f4c3c4->ToBoard());
  e6f4c3c4_book->Update(*e6f4c3c4);
  e6f4c3c4_book->AddChildrenToBook(children1);
  auto e6f4d3c4_book = book.Get(e6f4d3c4->ToBoard());
  e6f4d3c4_book->Update(*e6f4d3c4);
  e6f4d3c4_book->AddChildrenToBook(children2);
  book.Commit();

  auto fathers = book.Get(Board("e6f4c3c4d3"))->Fathers();
  EXPECT_THAT(fathers, UnorderedElementsAre(
      book.Get(Board("e6f4c3c4")),
      book.Get(Board("e6f4d3c4"))));
  EXPECT_THAT(book.Get(Board("e6f4c3c4"))->GetChildren(), UnorderedElementsAre(
      book.Get(Board("e6f4c3c4d3")),
      book.Get(Board("e6f4c3c4b3")),
      book.Get(Board("e6f4c3c4c5")),
      book.Get(Board("e6f4c3c4e3")),
      book.Get(Board("e6f4c3c4f3")),
      book.Get(Board("e6f4c3c4g3"))));
  EXPECT_THAT(book.Get(Board("e6f4d3c4"))->GetChildren(), UnorderedElementsAre(
      book.Get(Board("e6f4d3c4c3")),
      book.Get(Board("e6f4d3c4b3")),
      book.Get(Board("e6f4d3c4b5")),
      book.Get(Board("e6f4d3c4e3")),
      book.Get(Board("e6f4d3c4f3")),
      book.Get(Board("e6f4d3c4g3")),
      book.Get(Board("e6f4d3c4f5"))));
}

TEST(Book, AddChildrenStartingPosition) {
  TestBook test_book;
  Book book(kTempDir);
  book.Clean();
  std::vector<std::shared_ptr<TreeNode>> memory;

  auto start = TestTreeNode(Board(), 0, -63, 63, 10);
  auto children = GetTreeNodeChildren(*start, &memory);

  auto start_book = book.Get(Board());
  start_book->Update(*start);
  start_book->AddChildrenToBook(children);

  book.Commit();
  EXPECT_THAT(book.Get(Board())->GetChildren(), UnorderedElementsAre(
      book.Get(Board("e6"))));
  EXPECT_THAT(book.Get(Board("e6"))->Fathers(), UnorderedElementsAre(
      book.Get(Board())));
}

TEST_P(BookParameterizedFixture, UpdateFathers) {
  std::vector<std::string> lines = {"e6f4", "e6f4c3", "e6f4c3c4", "e6f4d3", "e6f4d3c4"};
  std::unordered_map<Board, int> evals = {{Board("e6f4d3c4c3"), 0}};
  std::unordered_set<std::pair<Board, Board>, PairHash> skip = {};
  bool add_only_once = GetParam();
  if (add_only_once) {
    skip = {{Board("e6f4d3c4"), Board("e6f4d3c4c3")}};
  }
  Book book = BookWithPositions(lines, evals, skip, /*visited=*/{});
  book.Commit();

  // Has two "descendants" with evaluation 0.
  EXPECT_NEAR(book.Get(Board("e6f4"))->GetEval(), 1, 1);

  EXPECT_EQ(book.Get(Board("e6f4c3c4d3"))->GetNVisited(), add_only_once ? 1 : 2);
  // 6 moves + 1 for itself
  EXPECT_EQ(book.Get(Board("e6f4c3c4"))->GetNVisited(), 7);
  // 7 moves + 1 for itself
  EXPECT_EQ(book.Get(Board("e6f4d3c4"))->GetNVisited(), add_only_once ? 7 : 8);
  // 7 from c4 + 3 other moves + 1 from itself
  EXPECT_EQ(book.Get(Board("e6f4c3"))->GetNVisited(), 11);
  // 8 from c4 + 4 other moves + 1 from itself
  EXPECT_EQ(book.Get(Board("e6f4d3"))->GetNVisited(), add_only_once ? 12 : 13);
  // 11 from c3 + 13 from d3 + 3 other moves + 1 from itself
  EXPECT_EQ(book.Get(Board("e6f4"))->GetNVisited(), add_only_once ? 27 : 28);

  // Overwrite: more nodes.
  auto e6f4c3c4d3 = book.Get(Board("e6f4c3c4d3"));
  EXPECT_TRUE(book.Get(Board("e6f4c3c4d3"))->IsLeaf());
  e6f4c3c4d3->Update(*TestTreeNode(Board("e6f4c3c4d3"), 20, -63, 63, 100));
  LeafToUpdate<BookNode>::Leaf(
      {book.Get(Board("e6f4")), book.Get(Board("e6f4c3")),
       book.Get(Board("e6f4c3c4")), book.Get(Board("e6f4c3c4d3"))}
  ).Finalize(100);

  EXPECT_NEAR(book.Get(Board("e6f4c3c4d3"))->GetEval(), 20, 1E-4);
  EXPECT_NEAR(book.Get(Board("e6f4c3c4"))->GetEval(), -20, 1E-4);
  EXPECT_NEAR(book.Get(Board("e6f4d3c4"))->GetEval(), -20, 1E-4);
  EXPECT_NEAR(book.Get(Board("e6f4c3"))->GetEval(), 20, 1E-4);
  EXPECT_NEAR(book.Get(Board("e6f4d3"))->GetEval(), 20, 1E-4);
  // Has two "descendants" with evaluation 0.
  EXPECT_NEAR(book.Get(Board("e6f4"))->GetEval(), -19, 1);

  EXPECT_EQ(book.Get(Board("e6f4c3c4d3"))->GetNVisited(), add_only_once ? 101 : 102);
  // 6 moves + 1 for itself
  EXPECT_EQ(book.Get(Board("e6f4c3c4"))->GetNVisited(), 107);
  // 7 moves + 1 for itself
  EXPECT_EQ(book.Get(Board("e6f4d3c4"))->GetNVisited(), add_only_once ? 7 : 8);
  // 7 from c4 + 3 other moves + 1 from itself
  EXPECT_EQ(book.Get(Board("e6f4c3"))->GetNVisited(), 111);
  // 8 from c4 + 4 other moves + 1 from itself
  EXPECT_EQ(book.Get(Board("e6f4d3"))->GetNVisited(), add_only_once ? 12 : 13);
  // 11 from c3 + 13 from d3 + 3 other moves + 1 from itself
  EXPECT_EQ(book.Get(Board("e6f4"))->GetNVisited(), add_only_once ? 127 : 128);
}

TEST(Book, BestDescendant) {
  std::vector<std::string> lines = {"e6f4", "e6f4c3", "e6f4c3c4", "e6f4d3", "e6f4d3c4"};
  std::unordered_map<Board, int> evals = {{Board("e6f4d3c4c3"), 0}, {Board("e6f4e3"), 2}};
  Book book = BookWithPositions(lines, evals, /*skip=*/{}, /*visited=*/{});
  BookNode* start = book.Get(Board("e6f4"));
  auto result = LeafToUpdate<BookNode>::BestDescendant(start, 0);
  EXPECT_TRUE(result);
  EXPECT_EQ(result->Leaf()->ToBoard().Unique(), Board("e6f4d3c4c3").Unique());
}

TEST(Book, BestDescendants) {
  std::vector<std::string> lines = {"e6f4", "e6f4c3", "e6f4c3c4", "e6f4d3", "e6f4d3c4"};
  std::unordered_map<Board, int> evals = {{Board("e6f4d3c4c3"), 0}, {Board("e6f4e3"), 2}};
  Book book = BookWithPositions(lines, evals, /*skip=*/{}, /*visited=*/{});
  BookNode* start = book.Get(Board("e6f4"));
  auto result = LeafToUpdate<BookNode>::BestDescendants(start);
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result[0].Leaf()->ToBoard().Unique(), Board("e6f4d3c4c3").Unique());
  EXPECT_EQ(result[1].Leaf()->ToBoard().Unique(), Board("e6f4e3").Unique());
}

INSTANTIATE_TEST_SUITE_P(
    BookParameterized,
    BookParameterizedFixture,
    ::testing::Values(false, true));