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

#include "merger.h"
#include "../book/book.h"
#include "../utils/parse_flags.h"
#include "../estimators/win_probability.h"

constexpr int kOldVersion = 1;
constexpr int kNewVersion = 1;

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

  BookVisitorMerge<kOldVersion, kNewVersion>(old_book, new_book).VisitAll();
}