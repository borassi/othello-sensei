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

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>
#include "book_tree_node.h"
#include "../board/board.h"
#include "../board/get_moves.h"
#include "../evaluatederivative/tree_node.h"

using testing::ElementsAre;

std::shared_ptr<TreeNode> TestTreeNode(Board b, Eval weak_lower, Eval weak_upper, Eval eval, NVisited n_visited) {
  std::shared_ptr<TreeNode> node(new TreeNode());
  node->Reset(b, 0, 1);
  node->SetLeaf(weak_lower, weak_upper, EvalToEvalLarge(eval), 4, n_visited);
  return node;
}

BookTreeNode RandomBookTreeNode(Board b) {
  int weak_lower = -(int) ((rand() % 32) * 2 + 1);
  int weak_upper = weak_lower + ((rand() % 32) * 2);
  int eval = 2 * (rand() % 60) - 60;
  int n_visited = rand() % 2000 + 1;
  return BookTreeNode(*TestTreeNode(b, weak_lower, weak_upper, eval, n_visited));
}

class BookTreeNodeTestFixture : public ::testing::TestWithParam<bool> {
 protected:
  bool serialize;
};

TEST(BookTreeNodeTestFixture, Basic) {
  TreeNode tree_node;
  tree_node.Reset(Board("e6"), 0, 1);
  tree_node.SetLeaf(-5, 1, EvalToEvalLarge(1), 4, 40);

  BookTreeNode node(tree_node);

  EXPECT_EQ(node.GetNVisited(), 40);
  EXPECT_EQ(node.Lower(), -65);
  EXPECT_EQ(node.Upper(), 65);
}

TEST(BookTreeNodeTestFixture, Serialize) {
  for (int i = 0; i < 10000; ++i) {
    BookTreeNode n = RandomBookTreeNode(RandomBoard());
    for (int j = 0; j < rand() % 4; ++j) {
      n.AddFather(rand() % (1 << 22));
    }
    ASSERT_TRUE(n.Equals(BookTreeNode(n.Serialize()), true, true)) << "Got: " << BookTreeNode(n.Serialize()) << "\nExpected: " << n;
  }
}

TEST(BookTreeNodeTestFixture, Merge) {
  auto book_tree_node1 = BookTreeNode(*TestTreeNode(Board("e6f4c3c4d3"), -5, 1, 1, 10));
  book_tree_node1.AddFather(12);

  BookTreeNode book_tree_node2(*TestTreeNode(Board("e6f4d3c4c3"), -7, 3, 3, 12));
  book_tree_node2.AddFather(13);

  book_tree_node1.Merge(book_tree_node2);
  EXPECT_EQ(book_tree_node1.GetNVisited(), 22);
  EXPECT_NEAR(book_tree_node1.GetEvaluation(3).ProbGreaterEqual(), 0.5, 0.01);
  EXPECT_EQ(book_tree_node1.Fathers().size(), 2);
}

TEST(BookTreeNodeTestFixture, MergeCommutative) {
  for (int i = 0; i < 1000; ++i) {
    Board b = RandomBoard();
    BookTreeNode tree_node1 = RandomBookTreeNode(b);
    BookTreeNode tree_node2 = RandomBookTreeNode(b);

    BookTreeNode merged = tree_node1;
    merged.Merge(tree_node2);
    BookTreeNode merged_reversed = tree_node2;
    merged_reversed.Merge(tree_node1);

    if (tree_node2.GetNVisited() != tree_node1.GetNVisited()) {  // TODO.
      ASSERT_EQ(merged, merged_reversed);
    }
  }
}

TEST(BookTreeNodeTestFixture, CopyAndMove) {
  TreeNode tree_node;
  tree_node.Reset(Board("e6"), 0, 1);
  tree_node.SetLeaf(-5, 1, EvalToEvalLarge(1), 4, 40);
  BookTreeNode expected(tree_node);
  expected.AddFather(123);

  // Copy from TreeNode.
  BookTreeNode n1 = tree_node;
  n1.AddFather(123);

  // Copy from BookTreeNode.
  BookTreeNode n2 = expected;
  BookTreeNode n3(expected);

  // Move from BookTreeNode.
  BookTreeNode tmp = expected;
  BookTreeNode n4 = std::move(tmp);

  EXPECT_EQ(expected, n1);
  EXPECT_EQ(expected, n2);
  EXPECT_EQ(expected, n3);
  EXPECT_EQ(expected, n4);
}

TEST_P(BookTreeNodeTestFixture, UpdateChildren) {
  TreeNode e6_tree;
  e6_tree.Reset(Board("e6"), 0, 1);
  e6_tree.SetLeaf(-63, 63, EvalToEvalLarge(1), 4, 40);
  BookTreeNode e6(e6_tree);

  TreeNode e6f4_tree;
  e6f4_tree.Reset(Board("e6f4"), 0, 1);
  e6f4_tree.SetLeaf(-63, 63, EvalToEvalLarge(10), 4, 40);
  BookTreeNode e6f4(e6f4_tree);

  TreeNode e6f6_tree;
  e6f6_tree.Reset(Board("e6f6"), 0, 1);
  e6f6_tree.SetLeaf(-63, 63, EvalToEvalLarge(30), 4, 40);
  BookTreeNode e6f6(e6f6_tree);

  TreeNode e6d6_tree;
  e6d6_tree.Reset(Board("e6d6"), 0, 1);
  e6d6_tree.SetLeaf(-63, 63, EvalToEvalLarge(30), 4, 40);
  BookTreeNode e6d6(e6d6_tree);

  e6.SetChildren({&e6f4, &e6f6, &e6d6});

  if (GetParam()) {
    e6 = BookTreeNode(e6.Serialize());
    e6f4 = BookTreeNode(e6f4.Serialize());
    e6f6 = BookTreeNode(e6f6.Serialize());
    e6d6 = BookTreeNode(e6d6.Serialize());
  }

  EXPECT_THAT(e6f4.Fathers(), ElementsAre(Board("e6").Unique()));
  EXPECT_THAT(e6f6.Fathers(), ElementsAre(Board("e6").Unique()));
  EXPECT_THAT(e6d6.Fathers(), ElementsAre(Board("e6").Unique()));
  EXPECT_TRUE(e6f4.IsLeaf());
  EXPECT_TRUE(e6f6.IsLeaf());
  EXPECT_TRUE(e6d6.IsLeaf());
  EXPECT_FALSE(e6.IsLeaf());
  EXPECT_EQ(e6.GetEval(), -10);
}

INSTANTIATE_TEST_SUITE_P(
    BookTreeNodeTest,
    BookTreeNodeTestFixture,
    ::testing::Values(false, true));