/*
 * Copyright 2025 Michele Borassi
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

#include<string.h>

#include "../book/book.h"
#include "../board/get_moves.h"
#include "../board/sequence.h"

enum NodeType {
  LEAF = 0,
  FIRST_VISIT = 1,
  LAST_VISIT = 2,
};

template<int version = kBookVersion>
class BookVisitor {
 public:
  typedef Book<version> Book;
  typedef typename Book::BookNode BookNode;

  BookVisitor(const Book& book) : book_(book), depth_(0) {}

  virtual void VisitAll() {
    for (const Board& root : book_.Roots()) {
      sequence_ = Sequence();
      Visit(root);
    }
  }

  virtual void VisitString(const std::string& sequence) {
    VisitSequence(Sequence(sequence));
  }

  virtual void VisitSequence(const Sequence& sequence) {
    sequence_ = sequence;
    Visit(sequence.ToBoard());
  }

  virtual void Visit(const Board& board) {
    std::unique_ptr<Node> node = book_.Get(board);
    assert(node);
    if (node->IsLeaf()) {
      VisitLeaf(*node);
      return;
    }
    if (!PreVisitInternalNode(*node)) {
      return;
    }
    auto flips = GetAllMovesWithPass(board.Player(), board.Opponent());
    std::unordered_set<Board> seen_boards;
    seen_boards.reserve(flips.size());
    for (const BitPattern flip : flips) {
      Board child = board.Next(flip);
      if (!seen_boards.insert(child.Unique()).second) {
        continue;
      }
      Square move = __builtin_ctzll(SquareFromFlip(flip, board.Player(), board.Opponent()));
      if (flip != 0) {
        sequence_.AddMove(move);
      }
      ++depth_;
      Visit(child);
      --depth_;
      if (flip != 0) {
        sequence_.RemoveLastMove();
      }
    }
    PostVisitInternalNode(*node);
  }

 protected:
  const Book& book_;
  Sequence sequence_;
  int depth_;

  virtual void VisitLeaf(Node& node) {};
  virtual bool PreVisitInternalNode(Node& node) { return true; };
  virtual void PostVisitInternalNode(Node& node) {};
};

template<int version = kBookVersion>
class BookVisitorNoTranspositions : public BookVisitor<version> {
 public:
  typedef BookVisitor<version> BookVisitor;
  using typename BookVisitor::Book;
  using typename BookVisitor::BookNode;

  BookVisitorNoTranspositions(const Book& book) : BookVisitor(book) {}

  void Visit(const Board& board) override {
    if (!visited_.insert(board.Unique()).second) {
      return;
    }
    BookVisitor::Visit(board);
  }

 private:
  std::unordered_set<Board> visited_;
};

template<int source_version = kBookVersion, int target_version = kBookVersion>
class BookMerge : public BookVisitorNoTranspositions<source_version> {
 public:
  typedef BookVisitor<source_version> BookVisitor;
  typedef Book<source_version> BookSource;
  typedef Book<target_version> BookTarget;
  typedef typename BookSource::BookNode BookNodeSource;
  typedef typename BookTarget::BookNode BookNodeTarget;
  using BookVisitor::book_;

  BookMerge(
      const BookSource& source,
      BookTarget& destination,
      void (*leaf_func)(Node*) = nullptr,
      void (*internal_func)(Node*) = nullptr) :
      BookVisitorNoTranspositions<source_version>(source),
      destination_(destination),
      leaf_func_(leaf_func),
      internal_func_(internal_func) {
    // This avoids adding a lot of roots and removing them afterwards (to avoid
    // memory problems).
    for (const auto& root: book_.Roots()) {
      Board unique = root.Unique();
      if (!destination_.Get(unique)) {
        destination_.roots_.insert(unique);
      }
    }
  }

  virtual void VisitAll() override {
    BookVisitor::VisitAll();
    destination_.Commit();
  }

 protected:
  BookTarget& destination_;
  void (*leaf_func_)(Node*);
  void (*internal_func_)(Node*);

  void FirstVisit(Node& source, void (*function)(Node*)) {
    auto* destination_node = destination_.Mutable(source.ToBoard());
    if (destination_node) {
      destination_node->AddDescendants(source.GetNVisited());
      // The min / max is computed inside these functions.
      destination_node->SetLower(source.Lower());
      destination_node->SetUpper(source.Upper());
    } else {
      if (function) {
        function(&source);
      }
      destination_.AddNoRootsUpdate(source);
    }
  }

  void VisitLeaf(Node& node) override {
    FirstVisit(node, leaf_func_);
  }

  bool PreVisitInternalNode(Node& node) override {
    FirstVisit(node, internal_func_);
    return true;
  }

  void PostVisitInternalNode(Node& node) override {
    // Node is the source node
    auto* destination_node = destination_.Mutable(node.ToBoard());
    assert(destination_node);
    if (destination_node->IsLeaf()) {
      destination_.AddChildren(destination_node->ToBoard(), {});
    } else {
      destination_node->UpdateFather();
    }
  }
};

template<int version = kBookVersion>
Book<version> RemoveDescendants(const Book<version>& book, const std::string& filepath) {
  Book<version> book_no_descendants(filepath);
  assert(book_no_descendants.Size() == 0);

  BookMerge<version, version>(
      book,
      book_no_descendants,
      [](Node* node) { node->ResetDescendants(); },
      [](Node* node) { node->ResetDescendants(); }).VisitAll();
  return book_no_descendants;
}