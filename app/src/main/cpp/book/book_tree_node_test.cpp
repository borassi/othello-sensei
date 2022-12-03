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
#include "book_tree_node.h"
#include "../board/board.h"
#include "../evaluatederivative/tree_node.h"

BookTreeNode RandomBookTreeNode(Board b) {
  TreeNode node;
  node.Reset(b, 0, 1);
  int lower = -(int) ((rand() % 32) * 2 + 1);
  int upper = lower + ((rand() % 32) * 2);
  int eval = (rand() % 512);
  int n_visited = rand();
  node.SetLeaf(lower, upper, eval, 4, n_visited);
  return BookTreeNode(node);
}


TEST(BookTreeNodeTest, Basic) {
  TreeNode tree_node;
  tree_node.Reset(Board("e6"), 0, 1);
  tree_node.SetLeaf(-5, 1, EvalToEvalLarge(1), 4, 40);

  BookTreeNode node(tree_node);
  EXPECT_EQ(node.GetNVisited(), 40);
  EXPECT_EQ(node.Lower(), -65);
  EXPECT_EQ(node.Upper(), 65);
}

TEST(BookTreeNodeTest, Serialize) {
  TreeNode tree_node;
  tree_node.Reset(Board("e6"), 0, 1);
  tree_node.SetLeaf(-5, 1, EvalToEvalLarge(1), 4, 1000000000000);

  BookTreeNode original(tree_node);
  std::vector<char> serialized = original.Serialize();
  BookTreeNode deserialized = BookTreeNode(serialized);
  EXPECT_EQ(original, deserialized);
}

TEST(BookTreeNodeTest, Merge) {
  TreeNode tree_node1;
  tree_node1.Reset(Board("e6"), 0, 1);
  tree_node1.SetLeaf(-5, 1, EvalToEvalLarge(1), 4, 10);
  BookTreeNode book_tree_node1(tree_node1);

  TreeNode tree_node2;
  tree_node2.Reset(Board("e6"), 0, 1);
  tree_node2.SetLeaf(-7, 3, EvalToEvalLarge(3), 2, 12);
  BookTreeNode book_tree_node2(tree_node2);

  book_tree_node1.Merge(book_tree_node2);
  EXPECT_EQ(book_tree_node1.GetNVisited(), 22);
  EXPECT_NEAR(book_tree_node1.GetEvaluation(3).ProbGreaterEqual(), 0.5, 0.01);
}

TEST(BookTreeNodeTest, MergeCommutative) {
  for (int i = 0; i < 1000; ++i) {
    Board b = RandomBoard();
    BookTreeNode tree_node1 = RandomBookTreeNode(b);
    BookTreeNode tree_node2 = RandomBookTreeNode(b);

    BookTreeNode merged = tree_node1;
    merged.Merge(tree_node2);
    BookTreeNode merged_reversed = tree_node2;
    merged_reversed.Merge(tree_node1);

    ASSERT_EQ(merged, merged_reversed);
  }
}

TEST(BookTreeNodeTest, CopyAndMove) {
  TreeNode tree_node;
  tree_node.Reset(Board("e6"), 0, 1);
  tree_node.SetLeaf(-5, 1, EvalToEvalLarge(1), 4, 40);

  TreeNode tree_node1;
  tree_node1.Reset(Board("e6"), 0, 1);
  tree_node1.SetLeaf(-5, 1, EvalToEvalLarge(3), 4, 40);

  BookTreeNode book_tree_node = tree_node;
  EXPECT_EQ(book_tree_node.GetEvaluation(-5).ProbGreaterEqual(),
            tree_node.GetEvaluation(-5).ProbGreaterEqual());
  book_tree_node = tree_node1;
  EXPECT_EQ(book_tree_node.GetEvaluation(-5).ProbGreaterEqual(),
            tree_node1.GetEvaluation(-5).ProbGreaterEqual());
}