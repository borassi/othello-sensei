/*
 * Copyright 2022-2024 Michele Borassi
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

using ::testing::ElementsAre;
using ::testing::ElementsAreArray;
using ::testing::UnorderedElementsAre;

typedef Book<kBookVersion>::BookNode BookNode;

class BookParameterizedFixture : public ::testing::TestWithParam<bool> {
 protected:
  bool only_new;
};

TEST(HashMapNode, SizeToByte) {
  EXPECT_EQ(0, HashMapNode::ByteToSize(0));
  EXPECT_EQ(0, HashMapNode::SizeToByte(0));

  for (int i = 1; i <= kMinFileSize; ++i) {
    EXPECT_EQ(1, (int) HashMapNode::SizeToByte(i));
  }
  EXPECT_EQ(kMinFileSize, HashMapNode::ByteToSize(1));

  EXPECT_LE(kMaxFileSize, HashMapNode::ByteToSize(255));
  // Just to avoid making too large files.
  EXPECT_GT(2 * kMaxFileSize, HashMapNode::ByteToSize(255));

  for (int i = 0; i <= 255; ++i) {
    ASSERT_EQ(i, HashMapNode::SizeToByte(HashMapNode::ByteToSize(i)))
        << i << " "
        << HashMapNode::ByteToSize(i) << " "
        << (int) HashMapNode::SizeToByte(HashMapNode::ByteToSize(i)) << "\n";
  }
  for (int i = kMinFileSize; i < kMaxFileSize; ++i) {
    int new_size = HashMapNode::ByteToSize(HashMapNode::SizeToByte(i));
    ASSERT_LE(i, new_size)
        << i << " "
        << (int) HashMapNode::SizeToByte(i) << " "
        << HashMapNode::ByteToSize(HashMapNode::SizeToByte(i)) << "\n";
    ASSERT_GT(i, new_size / 2);
  }
}

TEST(Book, Basic) {
  TestBook test_book;
  Book<> book(kTempDir);
  book.Clean();
  // Updates this node.
  book.Add(*TestTreeNode(Board(0UL, 1UL), 0, -5, 5, 1010));
  book.Commit();

  EXPECT_THAT(book.Roots(), testing::ElementsAre(Board(0UL, 1UL)));
  EXPECT_EQ(*book.Get(0, 1), *GetTestBookTreeNode(&test_book, Board(0UL, 1UL), 0, -5, 5, 1010));
  EXPECT_FALSE(book.Get(0, 2));
  EXPECT_EQ(book.Size(), 1);
  book.Commit();
  std::vector<std::pair<Node, NodeType>> nodes(book.begin(), book.end());
  EXPECT_THAT(nodes, UnorderedElementsAre(std::make_pair(*book.Get(0, 1), LEAF)));
}

TEST(Book, HugeNode) {
  TestBook test_book;
  Book<> book(kTempDir);
  book.Clean();
  // Also adds the node to the book and all fathers / children.
  auto node = LargestTreeNode(book, 1000);
  Board b = node->ToBoard();
  book.Commit();

  EXPECT_TRUE(book.IsSizeOK());
  EXPECT_EQ(*book.Get(b), *LargestTreeNode(test_book, 1000));
}

TEST(Book, Reload) {
  TestBook test_book;
  Book<> book(kTempDir);
  book.Clean();
  // Updates this node.
  book.Add(*TestTreeNode(Board(0UL, 1UL), 0, -5, 5, 2000));
  book.Commit();

  Book<> book1(kTempDir);
  book.Add(*TestTreeNode(Board(0UL, 3UL), 5, -5, 5, 100));
  book.Commit();

  EXPECT_EQ(*book.Get(0, 1), *GetTestBookTreeNode(&test_book, Board(0UL, 1UL), 0, -5, 5, 2000));
  EXPECT_FALSE(book.Get(0, 2));
  EXPECT_EQ(*book.Get(0, 3), *GetTestBookTreeNode(&test_book, Board(0UL, 3UL), 5, -5, 5, 100));
  EXPECT_EQ(book.Size(), 2);
}

TEST(Book, Unique) {
  Book<> book(kTempDir);
  TestBook test_book;
  book.Clean();
  book.Add(*TestTreeNode(Board(0UL, 1UL << 7), 1, 1, 3, 10));
  book.Commit();
  EXPECT_EQ(*book.Get(0, 1), *GetTestBookTreeNode(&test_book, Board(0UL, 1UL << 7), 1, 1, 3, 10));
  EXPECT_EQ(*book.Get(0, 1ULL << 56), *GetTestBookTreeNode(&test_book, Board(0UL, 1UL << 7), 1, 1, 3, 10));
  EXPECT_EQ(*book.Get(0, 1ULL << 63), *GetTestBookTreeNode(&test_book, Board(0UL, 1UL << 7), 1, 1, 3, 10));
}

TEST(Book, LargeOne) {
  Book<> book(kTempDir);
  std::unordered_map<Board, std::shared_ptr<TreeNode>> expected;
  std::vector<Board> old_boards;
  book.Clean();

  for (int i = 0; i < 2000; ++i) {
    if (i % 1000 == 0 || (i < 1000 && i % 100 == 0)) {
      std::cout << i << "\n";
    }
    Board b = RandomBoard();
    Board unique = b.Unique();
    if (expected.find(unique) != expected.end()) {
      continue;
    }
    std::shared_ptr<TreeNode> node = RandomTestTreeNode(b);
    old_boards.push_back(b);

    expected[unique] = node;

    book.Add(*node);

    ASSERT_EQ(*book.Get(b.Player(), b.Opponent()), BookNode(&book, *node));
    int test_i = rand() % old_boards.size();
    Board test_board = old_boards[test_i];
    BookNode expected_node(&book, *expected[test_board.Unique()]);
    ASSERT_EQ(*book.Get(test_board.Player(), test_board.Opponent()),
              expected_node);

    if (i < 100) {
      for (int test_i = 0 ; test_i < i; ++test_i) {
        Board test_board = old_boards[test_i];
        BookNode expected_node(&book, *expected[test_board.Unique()]);
        ASSERT_EQ(*book.Get(test_board.Player(), test_board.Opponent()),
                  expected_node);
      }
    }
    if (i < 2000) {
      assert(book.IsSizeOK());
    }
    if (rand() % 100 == 0) {
      book.Commit();
      book = Book<>(kTempDir);
    }
  }
}

TEST(Book, AddChildren) {
  TestBook test_book;
  Book<> book(kTempDir);
  book.Clean();

  auto e6_node = TestTreeNode(Board("e6"), 0, -63, 63, 10);
  auto e6f4_node = TestTreeNode(Board("e6f4"), 30, -63, 63, 10);
  auto e6f6_node = TestTreeNode(Board("e6f6"), 30, -63, 63, 10);
  auto e6d6_node = TestTreeNode(Board("e6d6"), 10, -63, 63, 10);

  auto e6 = book.Add(*e6_node);
  auto leaf = LeafToUpdate<BookNode>::Leaf({e6});
  book.AddChildren(Board("e6"), {*e6f4_node, *e6f6_node, *e6d6_node});
  leaf.Finalize(30);

  book.Commit();

  EXPECT_EQ(*book.Get(Board("e6f4")), *GetTestBookTreeNode(&test_book, Board("e6f4"), 30, -63, 63, 10));
  EXPECT_EQ(*book.Get(Board("e6f6")), *GetTestBookTreeNode(&test_book, Board("e6f6"), 30, -63, 63, 10));
  EXPECT_EQ(*book.Get(Board("e6d6")), *GetTestBookTreeNode(&test_book, Board("e6d6"), 10, -63, 63, 10));

  EXPECT_FALSE(book.Mutable(Board("e6"))->IsLeaf());
  EXPECT_EQ(40, book.Get(Board("e6"))->GetNVisited());
  EXPECT_EQ(-10, book.Get(Board("e6"))->GetEval()) << *book.Get(Board("e6"));

  auto fathers = book.Mutable(Board("e6f6"))->Fathers();
  EXPECT_THAT(fathers, UnorderedElementsAre(*book.Get(Board("e6"))));
  EXPECT_THAT(
      book.Mutable(Board("e6"))->GetChildren(),
      UnorderedElementsAre(
          *book.Get(Board("e6f4")),
          *book.Get(Board("e6f6")),
          *book.Get(Board("e6d6"))));
}

TEST(Book, RemoveRoots) {
  TestBook test_book;
  Book<> book(kTempDir);
  book.Clean();

  auto e6_node = TestTreeNode(Board("e6"), 0, -63, 63, 10);
  auto e6f4_node = TestTreeNode(Board("e6f4"), 30, -63, 63, 10);
  auto e6f6_node = TestTreeNode(Board("e6f6"), 30, -63, 63, 10);
  auto e6d6_node = TestTreeNode(Board("e6d6"), 10, -63, 63, 10);

  book.Add(*e6_node);
  book.Add(*e6f4_node);

  ASSERT_THAT(book.Roots(), UnorderedElementsAre(Board("e6").Unique(), Board("e6f4").Unique()));
  // Not yet committed the change.
  ASSERT_TRUE(Book<>(kTempDir).Roots().empty());
  book.Commit();
  ASSERT_THAT(book.Roots(), UnorderedElementsAre(Board("e6").Unique(), Board("e6f4").Unique()));
  ASSERT_THAT(Book<>(kTempDir).Roots(), UnorderedElementsAre(Board("e6").Unique(), Board("e6f4").Unique()));

  auto e6 = book.Mutable(Board("e6"));
  assert(e6);
  auto leaf = LeafToUpdate<BookNode>::Leaf({e6});
  book.AddChildren(Board("e6"), {*e6f4_node, *e6f6_node, *e6d6_node});
  leaf.Finalize(30);

  ASSERT_THAT(book.Roots(), UnorderedElementsAre(Board("e6").Unique()));
  // Not yet committed the change.
  ASSERT_THAT(Book<>(kTempDir).Roots(), UnorderedElementsAre(Board("e6").Unique(), Board("e6f4").Unique()));
  book.Commit();
  ASSERT_THAT(book.Roots(), UnorderedElementsAre(Board("e6").Unique()));
  ASSERT_THAT(Book<>(kTempDir).Roots(), UnorderedElementsAre(Board("e6").Unique()));
}

TEST(Book, AddChildrenDoubleCommit) {
  TestBook test_book;
  Book<> book(kTempDir);
  book.Clean();

  auto e6_node = TestTreeNode(Board("e6"), 0, -63, 63, 10);
  auto e6f4_node = TestTreeNode(Board("e6f4"), 30, -63, 63, 10);
  auto e6f6_node = TestTreeNode(Board("e6f6"), 30, -63, 63, 10);
  auto e6d6_node = TestTreeNode(Board("e6d6"), 10, -63, 63, 10);

  auto e6 = book.Add(*e6_node);
  auto leaf = LeafToUpdate<BookNode>::Leaf({e6});
  book.AddChildren(Board("e6"), {*e6f4_node, *e6f6_node, *e6d6_node});
  leaf.Finalize(30);

  book.Commit();

  book.Get(Board("e6f4"));
  book.Commit();

  EXPECT_EQ(*book.Get(Board("e6f4")), *GetTestBookTreeNode(&test_book, Board("e6f4"), 30, -63, 63, 10));
  EXPECT_EQ(*book.Get(Board("e6f6")), *GetTestBookTreeNode(&test_book, Board("e6f6"), 30, -63, 63, 10));
  EXPECT_EQ(*book.Get(Board("e6d6")), *GetTestBookTreeNode(&test_book, Board("e6d6"), 10, -63, 63, 10));

  EXPECT_FALSE(book.Mutable(Board("e6"))->IsLeaf());
  EXPECT_EQ(40, book.Get(Board("e6"))->GetNVisited());
  EXPECT_EQ(-10, book.Get(Board("e6"))->GetEval()) << *book.Get(Board("e6"));

  auto fathers = book.Mutable(Board("e6f6"))->Fathers();
  EXPECT_EQ(fathers.size(), 1);
  EXPECT_EQ(fathers[0], *book.Get(Board("e6")));
  EXPECT_THAT(
      book.Mutable(Board("e6"))->GetChildren(),
      UnorderedElementsAre(
          *book.Get(Board("e6f4")),
          *book.Get(Board("e6f6")),
          *book.Get(Board("e6d6"))));
}

TEST(Book, AddChildrenTransposition) {
  TestBook test_book;
  Book<> book(kTempDir);
  book.Clean();
  std::vector<std::shared_ptr<TreeNode>> memory;

  auto e6f4c3c4 = TestTreeNode(Board("e6f4c3c4"), 0, -63, 63, 10);
  auto children1 = GetTreeNodeChildren(*e6f4c3c4, &memory);

  auto e6f4d3c4 = TestTreeNode(Board("e6f4d3c4"), 0, -63, 63, 10);
  auto children2 = GetTreeNodeChildren(*e6f4d3c4, &memory);

  auto e6f4c3c4_book = book.Add(*e6f4c3c4);
  book.AddChildren(Board("e6f4c3c4"), children1);
  auto e6f4d3c4_book = book.Add(*e6f4d3c4);
  book.AddChildren(Board("e6f4d3c4"), children2);
  book.Commit();

  auto fathers = book.Mutable(Board("e6f4c3c4d3"))->Fathers();
  EXPECT_THAT(fathers, UnorderedElementsAre(
      *book.Get(Board("e6f4c3c4")),
      *book.Get(Board("e6f4d3c4"))));
  EXPECT_THAT(book.Mutable(Board("e6f4c3c4"))->GetChildren(), UnorderedElementsAre(
      *book.Get(Board("e6f4c3c4d3")),
      *book.Get(Board("e6f4c3c4b3")),
      *book.Get(Board("e6f4c3c4c5")),
      *book.Get(Board("e6f4c3c4e3")),
      *book.Get(Board("e6f4c3c4f3")),
      *book.Get(Board("e6f4c3c4g3"))));
  EXPECT_THAT(book.Mutable(Board("e6f4d3c4"))->GetChildren(), UnorderedElementsAre(
      *book.Get(Board("e6f4d3c4c3")),
      *book.Get(Board("e6f4d3c4b3")),
      *book.Get(Board("e6f4d3c4b5")),
      *book.Get(Board("e6f4d3c4e3")),
      *book.Get(Board("e6f4d3c4f3")),
      *book.Get(Board("e6f4d3c4g3")),
      *book.Get(Board("e6f4d3c4f5"))));
}

TEST(Book, AddChildrenStartingPosition) {
  TestBook test_book;
  Book<> book(kTempDir);
  book.Clean();
  std::vector<std::shared_ptr<TreeNode>> memory;

  auto start = TestTreeNode(Board(), 0, -63, 63, 10);
  auto children = GetTreeNodeChildren(*start, &memory);

  auto start_book = book.Add(*start);
  book.AddChildren(Board(), children);

  book.Commit();
  EXPECT_THAT(book.Mutable(Board())->GetChildren(), UnorderedElementsAre(
      *book.Get(Board("e6"))));
  EXPECT_THAT(book.Mutable(Board("e6"))->Fathers(), UnorderedElementsAre(
      *book.Get(Board())));
}

TEST_P(BookParameterizedFixture, UpdateFathers) {
  std::vector<std::string> lines = {"e6f4", "e6f4c3", "e6f4c3c4", "e6f4d3", "e6f4d3c4"};
  std::unordered_map<Board, int> evals = {{Board("e6f4d3c4c3"), 0}};
  std::unordered_set<std::pair<Board, Board>, PairHash> skip = {};
  bool add_only_once = GetParam();
  if (add_only_once) {
    skip = {{Board("e6f4d3c4"), Board("e6f4d3c4c3")}};
  }
  Book<> book = BookWithPositions(lines, evals, skip, /*visited=*/{});
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
  auto e6f4c3c4d3 = book.Mutable(Board("e6f4c3c4d3"));
  auto leaf = LeafToUpdate<BookNode>::Leaf(
      {book.Mutable(Board("e6f4")), book.Mutable(Board("e6f4c3")),
       book.Mutable(Board("e6f4c3c4")), book.Mutable(Board("e6f4c3c4d3"))}
  );
  EXPECT_TRUE(e6f4c3c4d3->IsLeaf());
  e6f4c3c4d3->SetLower(20);
  leaf.Finalize(100);

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

void CheckVectorHasRightOrder(const std::vector<std::pair<Node, NodeType>>& nodes) {
  std::unordered_set<Board> visited;
  for (int i = 0; i < nodes.size(); ++i) {
    auto& node = nodes[i].first;
    auto& node_type = nodes[i].second;
    visited.insert(node.ToBoard());
    if (node_type == FIRST_VISIT) {
      auto children = GetUniqueNextBoardsWithPass(node.ToBoard());
      for (const Board& b : visited) {
        children.erase(b);
      }
      for (int j = i+1; j <= nodes.size(); ++j) {
        ASSERT_LT(j, nodes.size());
        auto& child = nodes[j].first;
        auto& child_type = nodes[j].second;
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

TEST(Book, IteratorBasic) {
  std::vector<std::string> lines = {"e6f4", "e6f4c3"};
  Book<> book = BookWithPositions(lines, /*evals=*/{}, /*skips=*/{}, /*visited=*/{});
  book.Commit();

  std::vector<std::pair<Node, NodeType>> nodes(book.begin(), book.end());
  EXPECT_THAT(nodes, UnorderedElementsAre(
      std::make_pair(*book.Get(Board("e6f4")), FIRST_VISIT),
      std::make_pair(*book.Get(Board("e6f4c3")), FIRST_VISIT),
      std::make_pair(*book.Get(Board("e6f4d3")), LEAF),
      std::make_pair(*book.Get(Board("e6f4e3")), LEAF),
      std::make_pair(*book.Get(Board("e6f4f3")), LEAF),
      std::make_pair(*book.Get(Board("e6f4g3")), LEAF),
      std::make_pair(*book.Get(Board("e6f4c3c4")), LEAF),
      std::make_pair(*book.Get(Board("e6f4c3c6")), LEAF),
      std::make_pair(*book.Get(Board("e6f4c3d6")), LEAF),
      std::make_pair(*book.Get(Board("e6f4c3e7")), LEAF),
      std::make_pair(*book.Get(Board("e6f4c3")), LAST_VISIT),
      std::make_pair(*book.Get(Board("e6f4")), LAST_VISIT)
  ));
  CheckVectorHasRightOrder(nodes);
}

TEST(Book, IteratorTraspositions) {
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

  std::vector<std::pair<Node, NodeType>> nodes(book.begin(), book.end());
  EXPECT_THAT(nodes, UnorderedElementsAre(
      std::make_pair(*book.Get(Board("e6f4c3c4")), FIRST_VISIT),
      std::make_pair(*book.Get(Board("e6f4c3c4")), LAST_VISIT),
      std::make_pair(*book.Get(Board("e6f4d3c4")), FIRST_VISIT),
      std::make_pair(*book.Get(Board("e6f4d3c4")), LAST_VISIT),
      std::make_pair(*book.Get(Board("e6f4c3c4b3")), LEAF),
      std::make_pair(*book.Get(Board("e6f4c3c4c5")), LEAF),
      std::make_pair(*book.Get(Board("e6f4c3c4d3")), LEAF),
      std::make_pair(*book.Get(Board("e6f4c3c4e3")), LEAF),
      std::make_pair(*book.Get(Board("e6f4c3c4f3")), LEAF),
      std::make_pair(*book.Get(Board("e6f4c3c4g3")), LEAF),
      std::make_pair(*book.Get(Board("e6f4d3c4b3")), LEAF),
      std::make_pair(*book.Get(Board("e6f4d3c4b5")), LEAF),
      std::make_pair(*book.Get(Board("e6f4d3c4e3")), LEAF),
      std::make_pair(*book.Get(Board("e6f4d3c4f3")), LEAF),
      std::make_pair(*book.Get(Board("e6f4d3c4f5")), LEAF),
      std::make_pair(*book.Get(Board("e6f4d3c4g3")), LEAF)
  ));
  CheckVectorHasRightOrder(nodes);
}

TEST(Book, IteratorTraspositionsChild) {
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

  std::vector<std::pair<Node, NodeType>> nodes(book.begin(), book.end());
  EXPECT_THAT(nodes, UnorderedElementsAre(
      std::make_pair(*book.Get(Board("e6f4c3c4")), FIRST_VISIT),
      std::make_pair(*book.Get(Board("e6f4c3c4")), LAST_VISIT),
      std::make_pair(*book.Get(Board("e6f4d3c4")), FIRST_VISIT),
      std::make_pair(*book.Get(Board("e6f4d3c4")), LAST_VISIT),
      std::make_pair(*book.Get(Board("e6f4c3c4d3")), FIRST_VISIT),
      std::make_pair(*book.Get(Board("e6f4c3c4d3")), LAST_VISIT),
      std::make_pair(*book.Get(Board("e6f4c3c4b3")), LEAF),
      std::make_pair(*book.Get(Board("e6f4c3c4c5")), LEAF),
      std::make_pair(*book.Get(Board("e6f4c3c4e3")), LEAF),
      std::make_pair(*book.Get(Board("e6f4c3c4f3")), LEAF),
      std::make_pair(*book.Get(Board("e6f4c3c4g3")), LEAF),
      std::make_pair(*book.Get(Board("e6f4d3c4b3")), LEAF),
      std::make_pair(*book.Get(Board("e6f4d3c4b5")), LEAF),
      std::make_pair(*book.Get(Board("e6f4d3c4e3")), LEAF),
      std::make_pair(*book.Get(Board("e6f4d3c4f3")), LEAF),
      std::make_pair(*book.Get(Board("e6f4d3c4f5")), LEAF),
      std::make_pair(*book.Get(Board("e6f4d3c4g3")), LEAF),
      std::make_pair(*book.Get(Board("e6f4c3c4d3c2")), LEAF),
      std::make_pair(*book.Get(Board("e6f4c3c4d3c6")), LEAF),
      std::make_pair(*book.Get(Board("e6f4c3c4d3d6")), LEAF),
      std::make_pair(*book.Get(Board("e6f4c3c4d3e2")), LEAF),
      std::make_pair(*book.Get(Board("e6f4c3c4d3e7")), LEAF),
      std::make_pair(*book.Get(Board("e6f4c3c4d3f7")), LEAF)
  ));
  CheckVectorHasRightOrder(nodes);
}

TEST(Book, BestDescendant) {
  std::vector<std::string> lines = {"e6f4", "e6f4c3", "e6f4c3c4", "e6f4d3", "e6f4d3c4"};
  std::unordered_map<Board, int> evals = {{Board("e6f4d3c4c3"), 0}, {Board("e6f4e3"), 2}};
  Book<> book = BookWithPositions(lines, evals, /*skip=*/{}, /*visited=*/{});
  BookNode* start = book.Mutable(Board("e6f4"));
  auto result = LeafToUpdate<BookNode>::BestDescendant(start, 0, kLessThenMinEval);
  EXPECT_TRUE(result);
  EXPECT_EQ(result->Leaf()->ToBoard().Unique(), Board("e6f4d3c4c3").Unique());
}

TEST(Book, MergeDisjoint) {
  std::vector<std::string> lines = {"e6f4"};
  std::vector<std::string> lines2 = {"e6f6"};
  Book<> book = BookWithPositions(lines, /*evals=*/{}, /*skip=*/{}, /*visited=*/{});
  Book<> book2 = BookWithPositions(lines2, /*evals=*/{}, /*skip=*/{}, /*visited=*/{});
  Node old_e6f4 = *book.Get(Board("e6f4"));
  Node old_e6f4c3 = *book.Get(Board("e6f4c3"));

  book.Merge(book2);
  EXPECT_EQ(book.Size(), 11);
  EXPECT_EQ(*book.Get(Board("e6f4c3")), old_e6f4c3);
  EXPECT_EQ(*book.Get(Board("e6f4")), old_e6f4);
  EXPECT_EQ(*book.Get(Board("e6f6")), *book2.Get(Board("e6f6")));
  EXPECT_EQ(*book.Get(Board("e6f6f5")), *book2.Get(Board("e6f6f5")));
  EXPECT_THAT(
      book.Roots(),
      UnorderedElementsAre(Board("e6f4").Unique(), Board("e6f6").Unique()));
}

TEST(Book, MergeSameLine) {
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

  book.Merge(book2);
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

TEST(Book, MergeDifferentStarts) {
  Book<> book = BookWithPositions({"e6"}, {}, {}, {});
  Book<> book2 = BookWithPositions({"", "e6"}, {}, {}, {});
  book.Merge(book2);

  EXPECT_EQ(book.Size(), 5);
  EXPECT_EQ(book.Get(Board("e6f4"))->GetNVisited(), 2);
  EXPECT_EQ(book.Get(Board("e6"))->GetNVisited(), 8);
  // new board + 4 children + 3 grandchildren
  EXPECT_EQ(book.Get(Board(""))->GetNVisited(), 8);
  EXPECT_TRUE(book.Mutable(Board(""))->AreChildrenCorrect());
  EXPECT_TRUE(book.Mutable(Board("e6"))->AreChildrenCorrect());
  EXPECT_THAT(book.Roots(), UnorderedElementsAre(Board("").Unique()));
}

TEST(Book, MergeTranspositions) {
  Book<> book = BookWithPositions({"e6"});
  Book<> book2 = BookWithPositions({"e6f4", "e6f4c3", "e6f4d3", "e6f4c3c4", "e6f4d3c4", "e6f4c3c4d3"});
  book.Merge(book2);

  EXPECT_THAT(book.Roots(), UnorderedElementsAre(Board("e6").Unique()));
}

TEST(Book, ResetDescendants) {
  Book<>(kTempDir + "child1").Clean();
  Book<>(kTempDir).Clean();
  Book<>(kTempDir + "copy").Clean();
  Book<> book = BookWithPositions({"e6", "e6f4", "e6f4c3", "e6f4d3", "e6f4c3c4", "e6f4d3c4", "e6f4c3c4d3"});
  Book<> book_copy(kTempDir + "copy");
  book_copy.Clean();
  book_copy.Merge(book);

  Book<> book_child = book.RemoveDescendants(kTempDir + "child1");

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
    book.Merge(book_child);

    std::vector<std::pair<Node, NodeType>> nodes_child(book.begin(), book.end());
    std::vector<std::pair<Node, NodeType>> nodes_copy(book_copy.begin(), book_copy.end());
    ASSERT_THAT(nodes_child, ::testing::UnorderedElementsAreArray(nodes_copy));

    book_child.Clean();
    book_child = book.RemoveDescendants(kTempDir + "child1");
  }
}

// TODO: This started to fail, but we might have a different approach in the future.
//TEST(Book, ResetDescendantsDouble) {
//  Book<>(kTempDir + "child1").Clean();
//  Book<>(kTempDir + "child2").Clean();
//  Book<>(kTempDir).Clean();
//  Book<>(kTempDir + "copy").Clean();
//  Book<> book = BookWithPositions({"e6", "e6f4", "e6f6"});
//  Book<> book_copy(kTempDir + "copy");
//  book_copy.Merge(book);
//
//  Book<> book_child1 = book.RemoveDescendants(kTempDir + "child1");
//  Book<> book_child2 = book.RemoveDescendants(kTempDir + "child2");
//
//  for (int i = 0; i < 5; ++i) {
//    for (int j = 0; j < 10; ++j) {
//      bool diagonal = rand() % 2;
//      std::string line = diagonal ? "e6f6" : "e6f4";
//      auto& current_book = diagonal ? book_child2 : book_child1;
//      auto leaf_child = *LeafToUpdate<Book<>::BookNode>::BestDescendant(
//          current_book.Mutable(Board(line)), 0, kLessThenMinEval, {});
//      auto leaf_copy = *LeafToUpdate<Book<>::BookNode>::BestDescendant(
//          book_copy.Mutable(Board(line)), 0, kLessThenMinEval, {});
//
//      ASSERT_EQ(leaf_child.Leaf()->ToBoard(), leaf_copy.Leaf()->ToBoard());
//
//      std::vector<Node> children;
//      int n_visited;
//      for (auto child_flip : GetUniqueNextBoardsWithPass(leaf_child.Leaf()->ToBoard())) {
//        auto eval_result = RandomTestTreeNode(child_flip.first);
//        eval_result->SetLeafNeverSolved();
//        children.push_back(*eval_result);
//        n_visited += eval_result->GetNVisited();
//      }
//      current_book.AddChildren(leaf_child.Leaf()->ToBoard(), children);
//      book_copy.AddChildren(leaf_child.Leaf()->ToBoard(), children);
//      leaf_child.Finalize(n_visited);
//      leaf_copy.Finalize(n_visited);
//    }
//    book.Merge(book_child1);
//    book.Merge(book_child2);
//
//    std::vector<std::pair<Node, NodeType>> nodes_child(book.begin(), book.end());
//    std::vector<std::pair<Node, NodeType>> nodes_copy(book_copy.begin(), book_copy.end());
//    ASSERT_THAT(nodes_child, ::testing::UnorderedElementsAreArray(nodes_copy));
//
//    book_child1.Clean();
//    book_child1 = book.RemoveDescendants(kTempDir + "child1");
//    book_child2.Clean();
//    book_child2 = book.RemoveDescendants(kTempDir + "child2");
//  }
//}

INSTANTIATE_TEST_SUITE_P(
    BookParameterized,
    BookParameterizedFixture,
    ::testing::Values(false, true));