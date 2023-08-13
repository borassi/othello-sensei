/*
 * Copyright 2022 Google LLC
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

#ifndef OTHELLOSENSEI_VALUEFILE_H
#define OTHELLOSENSEI_VALUEFILE_H

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "../utils/file.h"

typedef u_int32_t BookFileOffset;
typedef int ValueFileSize;

constexpr ValueFileSize kMinValueFileSize = sizeof(BookFileOffset);

// A file that can store vector<char> with the same size (at least kMinSize=4).
// Supports the following operations:
// - Add(): Adds a new element.
// - Remove(): Removes the element. The space used by the element is reused in
//   next Add() calls.
class ValueFile {
 public:
  ValueFile(std::string filename, ValueFileSize size) : filename_(filename), size_(size) {
    assert(size >= kMinValueFileSize);
    auto path = std::filesystem::path(Filename());
    std::filesystem::create_directories(path.remove_filename());
    if (!std::filesystem::exists(Filename())) {
      Clean();
    }
  }

  ValueFileSize Size() { return size_; }

  BookFileOffset Add(const std::vector<char>& value);

  std::vector<char> Remove(BookFileOffset offset);

  std::vector<char> Get(BookFileOffset offset);

  void Clean() {
    std::ofstream(Filename(), std::ios::out).close();
    std::fstream file = OpenFile(Filename());
    SetAsEmpty(0, 0, &file);
  }

  void Remove() {
    remove(Filename().c_str());
  }

  BookFileOffset Elements() const {
    auto file = OpenFile(Filename());
    return FileSize(file) / size_;
  }

  void Print();

  class Iterator {
   public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = std::pair<int, std::vector<char>>;
    using pointer           = value_type*;  // or also value_type*
    using reference         = const value_type&;  // or also value_type&

    Iterator(ValueFile* file, int offset) : file_(file), current_(offset, std::vector<char>()), next_empty_(0), elements_(file->Elements()) {
      assert (offset == 0 || offset == elements_);
      if (offset == 0) {
        ToNextNonEmpty();
      }
    }
    reference operator*() const { return current_; }
    pointer operator->() { return &current_; }

    // Prefix increment
    Iterator& operator++() {
      ++current_.first;
      ToNextNonEmpty();
      return *this;
    }

    // Postfix increment
    Iterator operator++(int) {
      Iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    bool operator==(const Iterator& other) const = default;
    bool operator!=(const Iterator& other) const = default;

   private:

    void ToNextNonEmpty() {
      auto& [offset, value] = current_;
      while (offset == next_empty_ && offset < elements_) {
        std::fstream file = OpenFile(file_->Filename());
        file_->Seek(next_empty_, &file);
        file.read((char*) &next_empty_, sizeof(next_empty_));
        ++offset;
      }
      if (offset < elements_) {
        value = file_->Get(offset);
      } else {
        value.clear();
      }
    }

    ValueFileSize elements_;
    std::pair<int, std::vector<char>> current_;
    ValueFile* file_;
    ValueFileSize next_empty_;
  };

  Iterator begin() { return Iterator(this, 0); }
  Iterator end() { return Iterator(this, Elements()); }

 private:
  std::string filename_;
  ValueFileSize size_;

  std::string Filename() const {
    return filename_;
  }

  std::vector<char> Get(BookFileOffset offset, std::fstream* file);
  void Seek(BookFileOffset offset, std::fstream* file);
  void SetAsEmpty(BookFileOffset offset, BookFileOffset next_empty, std::fstream* file);
};

#endif //OTHELLOSENSEI_VALUEFILE_H
