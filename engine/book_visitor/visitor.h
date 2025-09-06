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

template<int version = kBookVersion>
class BookVisitor {
 public:
  typedef Book<version> Book;
  typedef typename Book::BookNode BookNode;

  BookVisitor(const Book& book) : book_(book), depth_(0), evaluations_at_depth_() {
    for (int i = 0; i < 120; ++i) {
      evaluations_at_depth_[i] = 0;
    }
  }

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
    evaluations_at_depth_[depth_] = node->GetEval();
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
      Square move = (Square) __builtin_ctzll(SquareFromFlip(flip, board.Player(), board.Opponent()));
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
    evaluations_at_depth_[depth_] = 0;
  }

  double GetErrorAtDepth(int i) {
    return evaluations_at_depth_[i] + evaluations_at_depth_[i-1];
  }

  std::pair<double, double> GetErrors() {
    double error_black = 0.0;
    double error_white = 0.0;

    for (int i = 1; i <= depth_; ++i) {
      double error = GetErrorAtDepth(i);
      if (i % 2 == 1) {
        error_black += error;
      } else {
        error_white += error;
      }
    }
    return {error_black, error_white};
  }

 protected:
  const Book& book_;
  Sequence sequence_;
  int depth_;
  double evaluations_at_depth_[120];

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

  virtual void Visit(const Board& board) override {
    if (!visited_.insert(board.Unique()).second) {
      return;
    }
    BookVisitor::Visit(board);
  }

 private:
  std::unordered_set<Board> visited_;
};

template<int version = kBookVersion>
class BookVisitorWithProgress : public BookVisitor<kBookVersion> {
 public:
  typedef BookVisitor<kBookVersion> BookVisitor;
  using typename BookVisitor::Book;
  using typename BookVisitor::BookNode;
  using BookVisitor::book_;

  BookVisitorWithProgress(const Book& book) : BookVisitor(book), actually_visited_(0) {
    to_be_visited_ = book_.Get(Board())->GetNVisited();
    for (int i = 0; i < 120; ++i) {
      visited_at_depth_[i] = 0;
      visiting_at_depth_[i] = 0;
    }
    std::cout << "Book size: " << book_.Size() << "\n";
  }

  virtual void Visit(const Board& board) override {
    ++actually_visited_;
    if (actually_visited_ % 100000 == 0) {
      NVisited visited = GetVisited();
      double time = time_.Get();
      double total_time = time / visited * to_be_visited_;
      std::cout << "Done " << PrettyPrintDouble((double) visited) << " of " << PrettyPrintDouble((double) to_be_visited_) << "\n";
      std::cout << "  Sequence: " << sequence_ << "\n";
      std::cout << "  Total time: " << total_time << "\n";
      std::cout << "  Remaining time: " << total_time - time << "\n";
      std::cout << "  Depth: " << (int) depth_ << "\n";
    }
    BookVisitor::Visit(board);
    // TODO: Avoid this Get.
    visited_at_depth_[depth_] += book_.Get(board)->GetNVisited();
    visited_at_depth_[depth_ + 1] = 0;
    evaluations_at_depth_[depth_] = 0;
  }

  virtual void VisitSequence(const Sequence& sequence) override {
    to_be_visited_ = book_.Get(sequence.ToBoard())->GetNVisited();
    sequence_ = sequence;
    Visit(sequence.ToBoard());
  }

 protected:
  using BookVisitor::sequence_;
  using BookVisitor::depth_;

  NVisited GetVisited() {
    NVisited visited = 0;
    for (int i = 0; i < 120; ++i) {
      visited += visited_at_depth_[i];
    }
    return visited;
  }

 private:
  ElapsedTime time_;
  NVisited to_be_visited_;
  NVisited actually_visited_;
  NVisited visiting_at_depth_[120];
  NVisited visited_at_depth_[120];
};

#endif  // BOOK_VISITOR_VISITOR_H