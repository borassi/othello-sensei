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
#include "../evaluatederivative/tree_node.h"


TEST(BookTreeNodeTest, Serialize) {
  TreeNode tree_node;
  tree_node.Reset(Board("e6"), 0, 1);
  tree_node.SetLeaf(-5, 1, EvalToEvalLarge(1), 4, 40);

  BookTreeNode original(tree_node);
  std::vector<char> serialized = original.Serialize();
  BookTreeNode deserialized = BookTreeNode(serialized);
  EXPECT_EQ(original, deserialized);
}

TEST(BookTreeNodeTest, Serialize2) {
  TreeNode tree_node;
  tree_node.Reset(0, 2, 4, 0);
  tree_node.SetLeaf(-1, 9, 0, 4, 100);

  BookTreeNode original(tree_node);
  std::vector<char> serialized = original.Serialize();
  BookTreeNode deserialized = BookTreeNode(serialized);
  EXPECT_EQ(original, deserialized);
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