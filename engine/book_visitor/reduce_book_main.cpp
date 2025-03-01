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
#include <queue>

#include "merger.h"
#include "visitor.h"
#include "../book/book.h"
#include "../estimators/win_probability.h"
#include "../thor/thor.h"
#include "../utils/misc.h"
#include "../utils/parse_flags.h"

class BookVisitorTopKValue : public BookVisitorWithProgress<kBookVersion> {
 public:
  typedef BookVisitorWithProgress<kBookVersion> BookVisitorWithProgress;
  using typename BookVisitorWithProgress::Book;
  using typename BookVisitorWithProgress::BookNode;
  using BookVisitorWithProgress::book_;

  BookVisitorTopKValue(const Book& book, int k, double kth_value_estimate) :
      BookVisitorWithProgress(book),
      k_(k),
      kth_value_estimate_(kth_value_estimate){}

  std::vector<double> Values() {
    std::vector<double> result;
    while (!lowest_k_values_.empty()) {
      result.push_back(lowest_k_values_.top());
      lowest_k_values_.pop();
    }
    return result;
  }

 protected:
  using BookVisitor::sequence_;
  using BookVisitor::depth_;

  double Value() {
    auto [error_black, error_white] = GetErrors();
    return error_black + error_white + depth_ / 2.0;
  }

  bool VisitNode() {
    double value = Value();
    if (value > kth_value_estimate_) {
      return false;
    }
    if (lowest_k_values_.size() >= k_ && value > lowest_k_values_.top()) {
      return false;
    }
    lowest_k_values_.push(value);
    if (lowest_k_values_.size() > k_) {
      lowest_k_values_.pop();
    }
    return true;
  }

  void VisitLeaf(Node& node) override {}

  bool PreVisitInternalNode(Node& node) override {
    return VisitNode();
  }

 private:
  int k_;
  double kth_value_estimate_;
  std::priority_queue<double> lowest_k_values_;
  std::vector<Board> output_;
};

template<int version = kBookVersion>
class BookVisitorCopy : public BookVisitorMerge<version, version> {
 public:
  BookVisitorCopy(const Book<version>& source, Book<version>& destination, double max_value) :
      max_value_(max_value), BookVisitorMerge<version, version>(source, destination) {}

 protected:
  double Value() {
    auto [error_black, error_white] = BookVisitor<version>::GetErrors();
    return error_black + error_white + BookVisitorMerge<version, version>::depth_ / 2.0;
  }

  bool VisitNode(Node& node) {
    BookVisitorMerge<version, version>::VisitNode(node);
    return Value() < max_value_;
  }

 private:
  double max_value_;
};

int main(int argc, char* argv[]) {
  ParseFlags parse_flags(argc, argv);
  std::string source_path = parse_flags.GetFlag("source_path");
  int desired_num_nodes = parse_flags.GetIntFlag("desired_num_nodes");
  double kth_value_estimate = parse_flags.GetDoubleFlag("kth_value_estimate");
  std::string target_path = parse_flags.GetFlag("target_path");

  Book source(source_path);
  Book target(target_path);

  BookVisitorTopKValue top_k(source, desired_num_nodes, kth_value_estimate);
  top_k.VisitString("");
  std::vector<double> top_k_values = top_k.Values();

  std::cout << "Got " << top_k_values.size() << " nodes:\n";
  for (int i = 10; i < top_k_values.size(); i *= 10) {
    std::cout << "  Value at " << i << ": " << top_k_values[top_k_values.size() - i - 1] << "\n";
  }
  std::cout << "  Value at " << top_k_values.size() << ": " << top_k_values[0] << "\n";

  // TODO: Optimize it by running in memory.
  // TODO: Ensure that, when we include a position, we also include its children.
  BookVisitorCopy copier(source, target, top_k_values[top_k_values.size() - 1]);
  copier.VisitAll();
  std::cout << "Done\n";
}