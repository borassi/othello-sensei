/*
 * Copyright 2023 Michele Borassi
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

class TestBook;

typedef BookTreeNode<TestBook, kBookVersion> TestBookTreeNode;

class TestBook {
 public:

  void AddChildren(const Board& father_board, const std::vector<Node>& children) {
    auto& father = nodes_.find(father_board.Unique())->second;
    father.is_leaf_ = false;
    std::vector<TreeNode*> children_in_board;
    for (const auto child : children) {
      auto iter = nodes_.find(child.ToBoard().Unique());
      if (iter == nodes_.end()) {
        children_in_board.push_back((TreeNode*) Add(child));
      } else {
        children_in_board.push_back((TreeNode*) &iter->second);
        iter->second.AddDescendants(child.GetNVisited());
      }
    }
    father.SetChildren(children_in_board);
  }

  TestBookTreeNode* Add(const TreeNode& node) {
    Board unique = node.ToBoard().Unique();
    auto& it = (*(
        nodes_.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(unique),
            std::forward_as_tuple(this, node)).first)).second;
    it.is_leaf_ = true;
    return &it;
  }
  std::unique_ptr<Node> Get(const Board& b) {
    auto it = nodes_.find(b.Unique());
    if (it == nodes_.end()) {
      return nullptr;
    }
    return std::make_unique<Node>(it->second);
  }
  TestBookTreeNode* Mutable(const Board& b) {
    auto it = nodes_.find(b.Unique());
    if (it == nodes_.end()) {
      return nullptr;
    }
    return &(it->second);
  }
  void Commit() {}

 private:
  // References are not invalidated:
  // https://stackoverflow.com/questions/39868640/stdunordered-map-pointers-reference-invalidation
  std::unordered_map<Board, TestBookTreeNode> nodes_;
};

std::shared_ptr<TreeNode> TestTreeNode(Board b, Eval leaf_eval, Eval weak_lower, Eval weak_upper, NVisited n_visited, Eval fixed_eval = kLessThenMinEval) {
  std::shared_ptr<TreeNode> t(new TreeNode());
  t->Reset(b.Player(), b.Opponent(), 4, 0);
  t->SetLeafEval(EvalToEvalLarge(leaf_eval), 4);
  t->UpdateLeafWeakLowerUpper(weak_lower, weak_upper);
  if (fixed_eval != kLessThenMinEval) {
    assert(fixed_eval % 2 == 0);
    t->SetLower(fixed_eval);
    t->SetUpper(fixed_eval);
  }
  t->AddDescendants(n_visited);
  return t;
}

std::shared_ptr<TreeNode> RandomTestTreeNode(Board b) {
  std::shared_ptr<TreeNode> t(new TreeNode());
  int lower = -(int) ((rand() % 32) * 2);
  int upper = lower + ((rand() % 20) * 2);
  int weak_lower = std::min(upper - 1, -(int) ((rand() % 32) * 2 + 1));
  int weak_upper = std::max(lower + 1, weak_lower + ((rand() % 20) * 2));
  int eval = EvalToEvalLarge(lower) + rand() % (EvalToEvalLarge(upper) - EvalToEvalLarge(lower) + 1);
  int n_visited = rand() % 100 + 1;
  t->Reset(b.Player(), b.Opponent(), /*depth=*/ 4, /*evaluator=*/rand() % 20);
  t->SetLeafEval(eval, 4);
  t->UpdateLeafWeakLowerUpper(weak_lower, weak_upper);

  // To test serializing / deserializing cases where the log derivative needs
  // > 2 bytes.
  for (int i = std::max(lower + 1, weak_lower); i < std::min(upper - 1, weak_upper); i += 2) {
    Evaluation* eval = t->MutableEvaluation(i);
    if (eval->ProbGreaterEqual() > 0 && eval->ProbGreaterEqual() < 1) {
      eval->Set(
          eval->ProbGreaterEqualSmall(),
          eval->ProofNumberSmall(),
          eval->DisproofNumberSmall(),
          eval->MaxLogDerivative() - (rand() % 100) * 3000
      );
    }
  }
  t->SetLower(lower);
  t->SetUpper(upper);
  t->AddDescendants(n_visited);
  return t;
}

std::vector<Node> GetTreeNodeChildren(
    const TreeNode& node, std::vector<std::shared_ptr<TreeNode>>* memory,
    int default_eval = 0, int default_desc = 10) {
  auto children_board = GetNextBoardsWithPass(node.ToBoard());
  std::vector<Node> children;
  children.reserve(children_board.size());
  for (const Board& child : children_board) {
    children.push_back(*TestTreeNode(child, default_eval, -63, 63, default_desc));
  }
  return children;
}


template<class Book>
BookTreeNode<Book, kBookVersion>* GetTestBookTreeNode(Book* book, Board b, Eval leaf_eval, Eval weak_lower, Eval weak_upper, NVisited n_visited) {
  auto node = TestTreeNode(b, leaf_eval, weak_lower, weak_upper, n_visited);
  return book->Add(*node);
}

template<class Book>
BookTreeNode<Book, kBookVersion>* GetTestBookTreeNode(Book* book, BitPattern player, BitPattern opponent, Eval leaf_eval, Eval weak_lower, Eval weak_upper, NVisited n_visited) {
  return GetTestBookTreeNode(book, Board(player, opponent), leaf_eval, weak_lower, weak_upper, n_visited);
}

template<class Book>
BookTreeNode<Book, kBookVersion>* RandomBookTreeNode(Book* book, Board b) {
  return book->Add(*RandomTestTreeNode(b));
}

template<int version = kBookVersion>
Book<version> BookWithPositions(
    const std::vector<std::string>& lines,
    const std::unordered_map<Board, int>& evals = {},
    const std::unordered_set<std::pair<Board, Board>, PairHash>& skip = {},
    const std::unordered_map<Board, int>& visited = {},
    const std::string dir = kTempDir,
    bool are_evals_fixed = false) {
  Book<> book(dir);
  book.Clean();

  auto first_line = lines[0];
  auto eval_fixed = are_evals_fixed ? GetOrDefault(evals, Board(first_line), 0) : kLessThenMinEval;
  auto start = book.Add(*TestTreeNode(Board(first_line), 0, -63, 63, GetOrDefault(visited, Board(first_line), 1), eval_fixed));

  for (const auto& line : lines) {
    NVisited total_visited = 0;
    std::vector<typename Book<version>::BookNode*> parents;
    for (int i = first_line.length(); i <= line.length(); i += 2) {
      auto node = book.Mutable(line.substr(0, i));
      assert(node);
      parents.push_back(node);
    }
    std::vector<Node> children;
    Board father = Board(line);
    for (Board child : GetNextBoardsWithPass(father)) {
      if (skip.find(std::make_pair(father, child)) != skip.end()) {
        continue;
      }
      int eval = GetOrDefault(evals, child, 41);
      int n_visited = GetOrDefault(visited, child, 1);
      auto eval_fixed = are_evals_fixed ? GetOrDefault(evals, child, 0) : kLessThenMinEval;
      children.push_back(*TestTreeNode(child, eval, -63, 63, n_visited, eval_fixed));
      total_visited += n_visited;
    }
    auto leaf = LeafToUpdate<typename Book<version>::BookNode>::Leaf(parents);
    book.AddChildren(father, children);
    leaf.Finalize(total_visited);
  }
  return book;
}

// Example: last_row = ---OOOOO, dir = -0, square = 0.
// CanContinueFlipping(b, [0, 1, 2, 3], -1) = true.
// CanContinueFlipping(b, 4, -1) = false
//   because we cannot have flipped ---XXXXX (missing outflank).
bool CanContinueFlipping(Board b, int square, int dir) {
  assert(square >= 0 && square <= 63);
  if (square + dir < 0 || square + dir > 63) {
    return false;
  }
  if ((square + dir) % 8 == 0 && square % 8 == 7) {
    return false;
  }
  if ((square + dir) % 8 == 7 && square % 8 == 0) {
    return false;
  }
  return b.IsOpponent(square + dir);
}

std::vector<BitPattern> AllPossibleFlips(Board b, Square square, int dir) {
  std::vector<BitPattern> result;
  if (!b.IsOpponent(square)) {
    return result;
  }
  BitPattern p = 1ULL << square;
  result.push_back(p);
  if (!CanContinueFlipping(b, square, dir)) {
    return result;
  }
  for (int c = square + dir; CanContinueFlipping(b, c, dir); c += dir) {
    p |= 1ULL << c;
    result.push_back(p);
  }
  return result;
}

template<class Book>
BookTreeNode<Book, kBookVersion>* LargestTreeNode(Book& book, int max_fathers = 0) {
  auto result = RandomBookTreeNode(&book, Board(
      "OOOOOO-O"
      "OOOOOOOO"
      "OOOOOOOO"
      "OOOOOOOO"
      "OOOOOOOO"
      "OOOOOOOO"
      "OOOOOOOO"
      "OOOOOOOO", true));
  result->SetLeafNeverSolved();
  BitPattern flip;
  Board b = result->ToBoard();
  int tmp = 0;
  for (int i = 0; i < 64; ++i) {
    std::vector<Board> all_possible_fathers;
    for (BitPattern flip_minus9 : AllPossibleFlips(b, i, -9)) {
      for (BitPattern flip_minus8 : AllPossibleFlips(b, i, -8)) {
        for (BitPattern flip_minus7 : AllPossibleFlips(b, i, -7)) {
          for (BitPattern flip_minus1 : AllPossibleFlips(b, i, -1)) {
            for (BitPattern flip_plus1 : AllPossibleFlips(b, i, 1)) {
              for (BitPattern flip_plus7 : AllPossibleFlips(b, i, 7)) {
                for (BitPattern flip_plus8 : AllPossibleFlips(b, i, 8)) {
                  for (BitPattern flip_plus9 : AllPossibleFlips(b, i, 9)) {
                    BitPattern flip = flip_minus9 | flip_minus8 | flip_minus7 | flip_minus1 | flip_plus1 | flip_plus7 | flip_plus8 | flip_plus9;
                    if (flip == 1ULL << i) {
                      continue;
                    }
                    Board b = result->ToBoard();
                    b.UndoMove(flip, i);
                    // It could be that other disks are flipped, and in this
                    // case this is not a valid father.
                    if (GetFlip(i, b.Player(), b.Opponent()) == flip) {
                      all_possible_fathers.push_back(b);
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
    std::vector<BookTreeNode<Book, kBookVersion>*> fathers;
    for (const Board& father_board : all_possible_fathers) {
      // We might have already added a transposition of father.
      if (book.Get(father_board)) {
        continue;
      }
      BookTreeNode<Book, kBookVersion>* father = RandomBookTreeNode(&book, father_board);
      // To avoid that the father has incompatible lower/upper wrt children.
      father->SetLeafNeverSolved();
      std::vector<Node> children = {result};
      for (auto b_move : GetUniqueNextBoardsWithPass(father_board)) {
        auto b = b_move.first;
        if (b == result->ToBoard()) {
          continue;
        }
        auto child = book.Mutable(b);
        if (child) {
          children.push_back(*child);
        } else {
          auto new_child = RandomBookTreeNode(&book, b);
          children.push_back(new_child);
        }
      }
      book.AddChildren(father_board, children);
      if (max_fathers > 0 && result->NFathers() > max_fathers) {
        return result;
      }
    }
  }
  return result;
}

#endif //OTHELLOSENSEI_BOOK_TEST_UTILS_H
