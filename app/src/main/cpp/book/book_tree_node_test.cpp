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

std::shared_ptr<TreeNode> TestTreeNode(Board b, Eval leaf_eval, Eval weak_lower, Eval weak_upper, NVisited n_visited) {
  std::shared_ptr<TreeNode> t(new TreeNode());
  t->Reset(b.Player(), b.Opponent(), 4, 0);
//  Eval weak_lower, Eval weak_upper, EvalLarge leaf_eval, Square depth, NVisited n_visited)
  t->SetLeaf(weak_lower, weak_upper, EvalToEvalLarge(leaf_eval), 4, n_visited);
  return t;
}
BookTreeNode TestBookTreeNode(Board b, Eval leaf_eval, Eval weak_lower, Eval weak_upper, NVisited n_visited) {
  auto node = TestTreeNode(b, leaf_eval, weak_lower, weak_upper, n_visited);
  return BookTreeNode(*node);
}

BookTreeNode RandomBookTreeNode(Board b) {
  int weak_lower = -(int) ((rand() % 32) * 2 + 1);
  int weak_upper = weak_lower + ((rand() % 32) * 2);
  int eval = 2 * (rand() % 60) - 60;
  int n_visited = rand() % 2000 + 1;
  return TestBookTreeNode(b, eval, weak_lower, weak_upper, n_visited);
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
  auto book_tree_node1 = BookTreeNode(*TestTreeNode(Board("e6f4c3c4d3"), 1, -5, 1, 10));
  book_tree_node1.AddFather(12);

  BookTreeNode book_tree_node2(*TestTreeNode(Board("e6f4d3c4c3"), 3, -7, 3, 12));
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

  // Copy from BookTreeNode.
  TreeNode tree_node1;
  tree_node1.Reset(Board("e6f4"), 0, 2);
  tree_node1.SetLeaf(-7, 3, EvalToEvalLarge(-5), 4, 40);
  BookTreeNode n5(tree_node1);
  n5 = expected;

  EXPECT_EQ(expected, n1);
  EXPECT_EQ(expected, n2);
  EXPECT_EQ(expected, n3);
  EXPECT_EQ(expected, n4);
  EXPECT_EQ(expected, n5);
}

TEST_P(BookTreeNodeTestFixture, UpdateChildren) {
  BookTreeNode e6 = TestBookTreeNode(Board("e6"), 0, -63, 63, 40);
  BookTreeNode e6f4 = TestBookTreeNode(Board("e6f4"), 10, -63, 63, 40);
  BookTreeNode e6f6 = TestBookTreeNode(Board("e6f6"), 30, -63, 63, 40);
  BookTreeNode e6d6 = TestBookTreeNode(Board("e6d6"), 30, -63, 63, 40);

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

TEST(BookTreeNode, BestDescendants) {
  BookTreeNode e6 = TestBookTreeNode(Board("e6"), 0, -63, 63, 40);
  BookTreeNode e6f4 = TestBookTreeNode(Board("e6f4"), 10, -63, 63, 40);
  BookTreeNode e6f6 = TestBookTreeNode(Board("e6f6"), 11, -63, 63, 40);
  BookTreeNode e6d6 = TestBookTreeNode(Board("e6d6"), 30, -63, 63, 40);

  e6.SetChildren({&e6f4, &e6f6, &e6d6});

//  EXPECT_THAT(e6.BestDescendants(1), ElementsAre(
//      LeafToUpdate<BaseTreeNode<BookTreeNode>>(&e6f4, -9, -19, 1, -63, 63, 0)));
}

INSTANTIATE_TEST_SUITE_P(
    BookTreeNodeTest,
    BookTreeNodeTestFixture,
    ::testing::Values(false, true));