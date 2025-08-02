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

#include "visitor.h"
#include "../book/book.h"
#include "../estimators/win_probability.h"
#include "../thor/thor.h"
#include "../utils/misc.h"
#include "../utils/parse_flags.h"

class BookVisitorStats : public BookVisitorWithProgress<kBookVersion> {
 public:
  typedef BookVisitorWithProgress<kBookVersion> BookVisitorWithProgress;
  using typename BookVisitorWithProgress::Book;
  using typename BookVisitorWithProgress::BookNode;
  using BookVisitorWithProgress::book_;

  BookVisitorStats(const Book& book, const Thor<GameGetterInMemory>& archive, const std::string& output_path) :
      BookVisitorWithProgress(book),
      archive_(archive) {
    // 8754564 / 286170000
    std::cout << "Archive games: "
              << archive_.GetGamesFromAllSources(Sequence(""), 1).num_games << "\n";
    output_.open(output_path);
    output_ << "sequence,games,empties,depth,error_black,error_white,uncertainty";
    for (int i = 1; i < 40; ++i) {
      output_ << ",error" << i;
    }
    output_ << "\n";
  }

 protected:
  using BookVisitor::sequence_;
  using BookVisitor::depth_;

  int VisitNode(Node& node) {
    int num_thor_games = archive_.GetGames<false>("OthelloQuest", sequence_).num_games;

    auto uncertainty = node.Uncertainty();
    if (num_thor_games > 0) {
      auto [error_black, error_white] = GetErrors();
      output_
          << sequence_ << ","
          << num_thor_games << ","
          << (int) node.NEmpties() << ","
          << depth_ << ","
          << error_black << ","
          << error_white << ","
          << uncertainty;
      for (int i = 1; i <= depth_; ++i) {
        output_ << "," << GetErrorAtDepth(i);
      }
      output_ << "\n";
    }
    return num_thor_games;
  }

  void VisitLeaf(Node& node) override {
    VisitNode(node);
  }

  bool PreVisitInternalNode(Node& node) override {
    return VisitNode(node) > 0;
  }

 private:
  const Thor<GameGetterInMemory>& archive_;
  std::ofstream output_;
};

int main(int argc, char* argv[]) {
  ParseFlags parse_flags(argc, argv);
  std::string book_path = parse_flags.GetFlag("book_path");
  std::string archive_path = parse_flags.GetFlag("archive_path");
  std::string output_path = parse_flags.GetFlag("output_path");

  const Book book(book_path);
  Thor<GameGetterInMemory> archive(archive_path, output_path + "/saved_games_filepath.txt");

  BookVisitorStats visitor(book, archive, output_path);
  visitor.VisitString("");
}