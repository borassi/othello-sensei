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

#ifndef BOOK_VISITOR_VISITOR_H
#define BOOK_VISITOR_VISITOR_H

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

#endif  // BOOK_VISITOR_VISITOR_H