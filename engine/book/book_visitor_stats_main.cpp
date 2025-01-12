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

#include <iostream>

#include "book.h"
#include "book_visitor.h"
#include "../estimators/win_probability.h"
#include "../thor/thor.h"
#include "../utils/misc.h"
#include "../utils/parse_flags.h"

class BookVisitorStats : public BookVisitor<kBookVersion> {
 public:
  typedef BookVisitor<kBookVersion> BookVisitor;
  using typename BookVisitor::Book;
  using typename BookVisitor::BookNode;
  using BookVisitor::book_;

  BookVisitorStats(const Book& book, const Thor<GameGetterInMemory>& archive, const std::string& output_path) :
      BookVisitor(book),
      actually_visited_(0),
      archive_(archive),
      thor_games_(0) {
    to_be_visited_ = book_.Get(Board())->GetNVisited();
    // 8754564 / 286170000
    std::cout << "Book size: " << book_.Size() << "\n";
    std::cout << "Archive games: "
              << archive_.GetGamesFromAllSources(Sequence(""), 1).num_games << "\n";
    output_.open(output_path);
    for (int i = 0; i < 120; ++i) {
      visited_at_depth_[i] = 0;
      visiting_at_depth_[i] = 0;
      evaluations_at_depth_[i] = 0;
    }
    output_ << "sequence, games, empties, depth, error_black, error_white, uncertainty\n";
  }

 protected:
  using BookVisitor::sequence_;
  using BookVisitor::depth_;

  int VisitNode(Node& node) {
    evaluations_at_depth_[depth_] = node.GetEval();
    ++actually_visited_;
    int num_thor_games = archive_.GetGames<false>("OthelloQuest", sequence_).num_games;
    if (actually_visited_ % 100000 == 0) {
      NVisited visited = GetVisited();
      double time = time_.Get();
      double total_time = time / visited * to_be_visited_;
      std::cout << "Done " << PrettyPrintDouble(visited) << " of " << PrettyPrintDouble(to_be_visited_) << "\n";
      std::cout << "  Sequence: " << sequence_ << "\n";
      std::cout << "  Total time: " << total_time << "\n";
      std::cout << "  Remaining time: " << total_time - time << "\n";
      std::cout << "  Depth: " << (int) depth_ << "\n";
    }
    if (depth_ < 5 || (depth_ < 10 && rand() % 10 == 0) || rand() % 100 == 0) {
      auto [error_black, error_white] = GetErrors(depth_);
      output_
          << sequence_ << ", "
          << num_thor_games << ", "
          << (int) node.NEmpties() << ", "
          << depth_ << ", "
          << error_black << ", "
          << error_white << ", "
          << node.Uncertainty() << ", "
          << "\n";
    }
    return num_thor_games;
  }

  NVisited GetVisited() {
    NVisited visited;
    for (int i = 0; i < 120; ++i) {
      visited += visited_at_depth_[i];
    }
    return visited;
  }

  std::pair<double, double> GetErrors(int depth) {
    double error_black = 0.0;
    double error_white = 0.0;

    for (int i = 1; i <= depth_; ++i) {
      double error = evaluations_at_depth_[i] + evaluations_at_depth_[i-1];
      if (i % 2 == 1) {
        error_black += error;
      } else {
        error_white += error;
      }
    }
    return {error_black, error_white};
  }

  void VisitLeaf(Node& node) override {
    VisitNode(node);
    visited_at_depth_[depth_] += node.GetNVisited();
  }

  bool PreVisitInternalNode(Node& node) override {
    return VisitNode(node) > 0;
  }

  void PostVisitInternalNode(Node& node) override {
    visited_at_depth_[depth_] += node.GetNVisited();
    visited_at_depth_[depth_ + 1] = 0;
    evaluations_at_depth_[depth_] = 0;
  }

 private:
  const Thor<GameGetterInMemory>& archive_;
  ElapsedTime time_;
  NVisited thor_games_;
  NVisited to_be_visited_;
  NVisited visiting_at_depth_[120];
  NVisited visited_at_depth_[120];
  double evaluations_at_depth_[120];
  NVisited actually_visited_;
  std::ofstream output_;
};

int main(int argc, char* argv[]) {
  ParseFlags parse_flags(argc, argv);
  std::string book_path = parse_flags.GetFlag("book_path");
  std::string archive_path = parse_flags.GetFlag("archive_path");
  std::string output_path = parse_flags.GetFlag("output_path");

  Book book(book_path);
  Thor<GameGetterInMemory> archive(archive_path);

  BookVisitorStats visitor(book, archive, output_path);
  visitor.VisitString("");
}