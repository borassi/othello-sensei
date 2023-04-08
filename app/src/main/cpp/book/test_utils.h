/*
 * Copyright 2023 Google LLC
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

#ifndef OTHELLOSENSEI_BOOK_TEST_UTILS_H
#define OTHELLOSENSEI_BOOK_TEST_UTILS_H

#include "book_tree_node.h"
#include "book.h"
#include "../board/get_moves.h"
#include "../evaluatederivative/tree_node.h"

const std::string kTempDir = "app/testdata/tmp/book_test";

class TestBook {
 public:
  BookTreeNode<TestBook>* Get(const Board& b, bool father = false) {
    Board unique = b.Unique();
    return &(*(nodes_.try_emplace(unique, this, unique).first)).second;
  }
  void Commit() {}
 private:
  // References are not invalidated:
  // https://stackoverflow.com/questions/39868640/stdunordered-map-pointers-reference-invalidation
  std::unordered_map<Board, BookTreeNode<TestBook>> nodes_;
};

typedef BookTreeNode<TestBook> TestBookTreeNode;

std::shared_ptr<TreeNode> TestTreeNode(Board b, Eval leaf_eval, Eval weak_lower, Eval weak_upper, NVisited n_visited) {
  std::shared_ptr<TreeNode> t(new TreeNode());
  t->Reset(b.Player(), b.Opponent(), 4, 0, weak_lower, weak_upper);
//  Eval weak_lower, Eval weak_upper, EvalLarge leaf_eval, Square depth, NVisited n_visited)
  t->SetLeaf(EvalToEvalLarge(leaf_eval), 4);
  t->AddDescendants(n_visited);
  return t;
}

std::vector<TreeNode*> GetTreeNodeChildren(
    const TreeNode& node, std::vector<std::shared_ptr<TreeNode>>* memory,
    int default_eval = 0, int default_desc = 10) {
  auto children_board = GetNextBoardsWithPass(node.ToBoard());
  std::vector<TreeNode*> children;
  children.reserve(children_board.size());
  for (const Board& child : children_board) {
    memory->push_back(TestTreeNode(child, default_eval, -63, 63, default_desc));
    children.push_back((*memory)[memory->size() - 1].get());
  }
  return children;
}


template<class Book>
BookTreeNode<Book>* GetTestBookTreeNode(Book* book, Board b, Eval leaf_eval, Eval weak_lower, Eval weak_upper, NVisited n_visited) {
  auto node = TestTreeNode(b, leaf_eval, weak_lower, weak_upper, n_visited);
  auto result = book->Get(b);
  result->Reset(b.Unique(), 60, 0, weak_lower, weak_upper);
  result->Update(*node);
  result->AddDescendants(node->GetNVisited());
  return result;
}

template<class Book>
BookTreeNode<Book>* GetTestBookTreeNode(Book* book, BitPattern player, BitPattern opponent, Eval leaf_eval, Eval weak_lower, Eval weak_upper, NVisited n_visited) {
  return GetTestBookTreeNode(book, Board(player, opponent), leaf_eval, weak_lower, weak_upper, n_visited);
}

template<class Book>
BookTreeNode<Book>* RandomBookTreeNode(Book* book, Board b) {
  int weak_lower = -(int) ((rand() % 32) * 2 + 1);
  int weak_upper = weak_lower + ((rand() % 32) * 2);
  int eval = 2 * (rand() % 60) - 60;
  int n_visited = rand() % 2000 + 1;
  return GetTestBookTreeNode(book, b, eval, weak_lower, weak_upper, n_visited);
}

Book BookWithPositions(
    const std::vector<std::string>& lines,
    const std::unordered_map<Board, int>& evals,
    const std::unordered_set<std::pair<Board, Board>, PairHash>& skip,
    const std::unordered_map<Board, int>& visited) {
  Book book(kTempDir);
  book.Clean();

  auto first_line = lines[0];
  auto start = book.Get(Board(first_line));
  start->Update(*TestTreeNode(Board(first_line), 0, -63, 63, 1));
  LeafToUpdate<BookNode>::Leaf({start}).Finalize(GetOrDefault(visited, Board(first_line), 1));

  for (const auto& line : lines) {
    NVisited total_visited = 0;
    std::vector<BookNode*> parents;
    for (int i = first_line.length(); i <= line.length(); i += 2) {
      parents.push_back(book.Get(line.substr(0, i)));
    }
    std::vector<std::shared_ptr<TreeNode>> children;
    Board father = Board(line);
    for (Board child : GetNextBoardsWithPass(father)) {
      if (skip.contains(std::make_pair(father, child))) {
        continue;
      }
      int eval = GetOrDefault(evals, child, 41);
      int n_visited = GetOrDefault(visited, child, 1);
      children.push_back(TestTreeNode(child, eval, -63, 63, n_visited));
      total_visited += n_visited;
    }
    book.Get(father)->AddChildrenToBook(children);
    LeafToUpdate<BookNode>::Leaf(parents).Finalize(total_visited);
  }
  return book;
}
#endif //OTHELLOSENSEI_BOOK_TEST_UTILS_H
