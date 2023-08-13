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
#include "test_utils.h"
#include "../board/board.h"
#include "../board/get_moves.h"
#include "../evaluatederivative/tree_node.h"

using testing::ElementsAre;

class BookTreeNodeTestFixture : public ::testing::TestWithParam<bool> {
 protected:
  bool serialize;
};

TEST(BookTreeNodeTestFixture, Basic) {
  TestBook book;
  auto tree_node = TestTreeNode(Board("e6"), 0, -5, 1, 40);

  TestBookTreeNode* node = book.Add(*tree_node);

  EXPECT_EQ(node->GetNVisited(), 40);
  EXPECT_EQ(node->GetEvaluation(-1), tree_node->GetEvaluation(-1));
  EXPECT_EQ(node->Lower(), tree_node->Lower());
  EXPECT_EQ(node->Upper(), tree_node->Upper());
}

TEST(BookTreeNodeTestFixture, Serialize) {
  TestBook book;
  for (int i = 0; i < 10000; ++i) {
    TestBookTreeNode* n = RandomBookTreeNode(&book, RandomBoard());
    TestBookTreeNode actual = TestBookTreeNode(&book, n->Serialize());
    ASSERT_EQ(*n, actual);
  }
}

//TEST(BookTreeNodeTestFixture, SerializeLargestTreeNode) {
//  TestBook book;
//  TestBookTreeNode* n = LargestTreeNode<TestBook>(book, 10000);
//  auto serialized = n->Serialize();
//  std::cout << n->Fathers().size() << " " << serialized.size() << " " << kMaxNodeSize << "\n";
//  TestBookTreeNode actual = TestBookTreeNode(&book, serialized);
//  ASSERT_EQ(*n, actual);
//}
//TEST(BookTreeNodeTestFixture, Merge) {
//  TestBook book;
//  auto book_tree_node1 = GetTestBookTreeNode(&book, Board("e6f4c3c4d3"), 1, -5, 1, 10);
//  auto tree_node2 = TestTreeNode(Board("e6f4d3c4c3"), 3, -7, 3, 12);
//
//  book_tree_node1->Update(*tree_node2);
//  LeafToUpdate<TestBookTreeNode>::Leaf({book_tree_node1}).Finalize(12);
//  EXPECT_EQ(book_tree_node1->GetNVisited(), 22);
//  EXPECT_EQ(book_tree_node1->GetEvaluation(3), tree_node2->GetEvaluation(3));
//}

TEST(BookTreeNodeTestFixture, UpdateChildrenDepth1) {
  TestBook book;
  auto e6_node = TestTreeNode(Board("e6"), 0, -63, 63, 40);
  auto e6f4_node = TestTreeNode(Board("e6f4"), 10, -63, 63, 40);
  auto e6f6_node = TestTreeNode(Board("e6f6"), 30, -63, 63, 40);
  auto e6d6_node = TestTreeNode(Board("e6d6"), 30, -63, 63, 40);

  e6_node->SetChildren({e6f4_node.get(), e6f6_node.get(), e6d6_node.get()});

  auto e6 = book.Add(*e6_node);
  book.AddChildren(Board("e6"), e6_node->GetChildren());
  LeafToUpdate<TestBookTreeNode>::Leaf({e6}).Finalize(120);

  EXPECT_FALSE(book.Get(Board("e6")).value()->IsLeaf());
  EXPECT_TRUE(book.Get(Board("e6f4")).value()->IsLeaf());
  EXPECT_TRUE(book.Get(Board("e6f6")).value()->IsLeaf());
  EXPECT_TRUE(book.Get(Board("e6d6")).value()->IsLeaf());
  EXPECT_EQ(book.Get(Board("e6")).value()->GetEval(), -10);
}

TEST(BookTreeNodeTestFixture, UpdateChildrenDepth2) {
  TestBook book;
  auto e6_node = TestTreeNode(Board("e6"), 0, -63, 63, 40);
  auto e6f4_node = TestTreeNode(Board("e6f4"), 10, -63, 63, 40);
  auto e6f6_node = TestTreeNode(Board("e6f6"), 30, -63, 63, 40);
  auto e6d6_node = TestTreeNode(Board("e6d6"), 30, -63, 63, 40);
  auto e6f4c3_node = TestTreeNode(Board("e6f4c3"), -9, -63, 63, 40);
  auto e6f4d3_node = TestTreeNode(Board("e6f4d3"), 20, -63, 63, 40);
  auto e6f4e3_node = TestTreeNode(Board("e6f4e3"), 20, -63, 63, 40);
  auto e6f4f3_node = TestTreeNode(Board("e6f4f3"), 20, -63, 63, 40);
  auto e6f4g3_node = TestTreeNode(Board("e6f4g3"), 20, -63, 63, 40);

  e6_node->SetChildren({e6f4_node.get(), e6f6_node.get(), e6d6_node.get()});

  TestBookTreeNode* e6 = book.Add(*e6_node);
  book.AddChildren(Board("e6"), e6_node->GetChildren());
  LeafToUpdate<TestBookTreeNode>::Leaf({e6}).Finalize(120);
  auto e6f4 = book.Get(Board("e6f4")).value();
  book.AddChildren<TreeNode*>(
      Board("e6f4"),
      {e6f4c3_node.get(), e6f4d3_node.get(), e6f4e3_node.get(), e6f4f3_node.get(), e6f4g3_node.get()});
  LeafToUpdate<TestBookTreeNode>::Leaf({book.Get(Board("e6")).value(), book.Get(Board("e6f4")).value()}).Finalize(120);

  EXPECT_FALSE(book.Get(Board("e6")).value()->IsLeaf());
  EXPECT_FALSE(book.Get(Board("e6f4")).value()->IsLeaf());
  EXPECT_TRUE(book.Get(Board("e6f6")).value()->IsLeaf());
  EXPECT_TRUE(book.Get(Board("e6d6")).value()->IsLeaf());
  EXPECT_TRUE(book.Get(Board("e6f4c3")).value()->IsLeaf());
  EXPECT_NEAR(book.Get(Board("e6f4c3")).value()->GetEval(), -9, 0.01);
  EXPECT_NEAR(book.Get(Board("e6f4")).value()->GetEval(), 9, 0.01);
  EXPECT_NEAR(book.Get(Board("e6")).value()->GetEval(), -9, 0.01);
}

TEST(BookTreeNodeTestFixture, UpdateChildrenExisting) {
  TestBook book;
  auto e6_node = TestTreeNode(Board("e6"), 0, -63, 63, 40);
  auto e6f4_node = TestTreeNode(Board("e6f4"), 10, -63, 63, 40);
  auto e6f6_node = TestTreeNode(Board("e6f6"), 30, -63, 63, 40);
  auto e6d6_node = TestTreeNode(Board("e6d6"), 30, -63, 63, 40);
  auto e6f4_node_bis = TestTreeNode(Board("e6f4"), 30, -63, 63, 40);

  auto e6 = book.Add(*e6_node);
  auto e6f4 = book.Add(*e6f4_node);
  book.AddChildren<std::shared_ptr<TreeNode>>(
      Board("e6"),
      {e6f4_node_bis, e6f6_node, e6d6_node});
  LeafToUpdate<TestBookTreeNode>::Leaf({e6}).Finalize(120);

  EXPECT_FALSE(book.Get(Board("e6")).value()->IsLeaf());
  EXPECT_TRUE(book.Get(Board("e6f4")).value()->IsLeaf());
  EXPECT_TRUE(book.Get(Board("e6f6")).value()->IsLeaf());
  EXPECT_TRUE(book.Get(Board("e6d6")).value()->IsLeaf());
  EXPECT_EQ(book.Get(Board("e6")).value()->GetEval(), -10);
  EXPECT_EQ(book.Get(Board("e6f4")).value()->GetNVisited(), 80);
}
