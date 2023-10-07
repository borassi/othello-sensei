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

#include <iostream>

#include "book.h"
#include "../utils/parse_flags.h"
#include "../estimators/win_probability.h"

constexpr int kOldVersion = 1;
constexpr int kNewVersion = 1;

// TODO: This is just to document the conversion. Remove in a next commit.
constexpr double BaseRescaleProbOld(double x) {
  if (x == 0) {
    return 0;
  }
  return pow(-log(0.08 * x), -2);
}

constexpr double RescaleProbOld(double x) {
  return (BaseRescaleProbOld(x) - BaseRescaleProbOld(1-x)) / (BaseRescaleProbOld(1) - BaseRescaleProbOld(0)) / 2 + 0.5;
}

constexpr double InverseRescaleProbOld(double y) {
  return Inverse(RescaleProbOld, y, 0, 1);
}

constexpr Probability ProbabilityToByteExplicitOld(double probability) {
  assert(probability >= 0);
  assert(probability <= 1);
  return round(RescaleProbOld(probability) * kProbStep);
}

constexpr double ByteToProbabilityExplicitOld(Probability byte) {
  assert(byte >= 0);
  assert(byte <= kProbStep);
  return InverseRescaleProbOld(byte / (double) kProbStep);
}

void UpdateProbabilities(Node* node) {
  for (int i = -63; i <= 63; i += 2) {
    node->MutableEvaluation(i)->prob_greater_equal_ = ProbabilityToByteExplicit(ByteToProbabilityExplicitOld(node->MutableEvaluation(i)->prob_greater_equal_));
  }
}
// TODO: Remove until here.

int main(int argc, char* argv[]) {
  ParseFlags parse_flags(argc, argv);
  std::string old_book_path = parse_flags.GetFlag("old");
  std::string new_book_path = parse_flags.GetFlag("new");

  Book<kOldVersion> old_book(old_book_path);
  Book<kNewVersion> new_book(new_book_path);

  if (old_book.Size() == 0) {
    std::cout << "--old must point to a non-empty book";
    return 1;
  }
  std::cout
      << "Updating book " << old_book_path << " (version " << kOldVersion
      << ", size " << old_book.Size() << ") to version " << kNewVersion
      << " in path " << new_book_path << "\n";

  new_book.Merge(old_book);
}