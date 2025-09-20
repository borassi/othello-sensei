/*
 * Copyright 2022-2024 Michele Borassi
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
#include <cstring>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

#include "../utils/files.h"

typedef uint32_t BookFileOffset;
typedef int ValueFileSize;

constexpr ValueFileSize kMinValueFileSize = sizeof(BookFileOffset);

// A file that can store vector<char> with the same size (at least kMinSize=4).
// Supports the following operations:
// - Add(): Adds a new element.
// - Remove(): Removes the element. The space used by the element is reused in
//   next Add() calls.
class ValueFile {
 public:
  ValueFile(std::string filename, ValueFileSize size) : filename_(filename), size_(size), cached_file_(nullptr) {
    assert(size >= kMinValueFileSize);
    CreateFileIfNotExists(filename_);
    if (Elements() == 0) {
      Clean();
    }
  }

  ValueFileSize Size() const { return size_; }

  BookFileOffset Add(const std::vector<char>& value);

  std::vector<char> Remove(BookFileOffset offset);

  std::vector<char> Get(BookFileOffset offset) const;

  void Clean() {
    CreateEmptyFileWithDirectories(filename_);
    auto file = GetFile();
    SetAsEmpty(0, 0, file);
  }

  void Remove() {
    remove(filename_.c_str());
  }

  BookFileOffset Elements() const {
    std::shared_ptr<std::fstream> file = GetFileCached();
    return (BookFileOffset) (FileLength(*file) / size_);
  }

  void Print() const;

  class Iterator {
   public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = std::pair<BookFileOffset, std::vector<char>>;
    using pointer           = value_type*;  // or also value_type*
    using reference         = const value_type&;  // or also value_type&

    Iterator(const ValueFile* file, BookFileOffset offset) : current_(offset, std::vector<char>()), size_(file->Size()) {
      assert (offset == 0 || offset == file->Elements());

      if (offset == 0) {
        std::shared_ptr<std::fstream> fstream = file->GetFileCached();
        file->Seek(0, *fstream);
        values_.resize(FileLength(*fstream));
        fstream->read(values_.data(), values_.size());

        BookFileOffset empty = 0;
        empty = *((BookFileOffset*) values_.data() + empty * size_);
        empties_.insert(empty);
        while (empty != 0) {
          empty = *((BookFileOffset*) (values_.data() + empty * size_));
          empties_.insert(empty);
        }
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

    bool operator==(const Iterator& other) const {
      return current_ == other.current_ && size_ == other.size_;
    }
    bool operator!=(const Iterator& other) const { return !operator==(other); }

   private:
    std::pair<BookFileOffset, std::vector<char>> current_;
    std::unordered_set<BookFileOffset> empties_;
    int size_;
    std::vector<char> values_;

    void ToNextNonEmpty() {
      auto& offset = current_.first;
      auto& value = current_.second;
      while (empties_.find(offset) != empties_.end() && offset < values_.size()) {
        ++offset;
      }
      if (offset * size_ < values_.size()) {
        value.resize(size_);
        std::memcpy(value.data(), values_.data() + offset * size_, size_);
      } else {
        value.clear();
      }
    }
  };

  Iterator begin() const { return {this, 0}; }
  Iterator end() const { return {this, Elements()}; }

 private:
  std::string filename_;
  ValueFileSize size_;
  mutable std::shared_ptr<std::fstream> cached_file_;

  std::fstream GetFile() const;
  std::shared_ptr<std::fstream> GetFileCached() const;
  std::vector<char> Get(BookFileOffset offset, std::fstream& file) const;
  void Seek(BookFileOffset offset, std::fstream& file) const;
  void SetAsEmpty(BookFileOffset offset, BookFileOffset next_empty, std::fstream& file);
};

#endif //OTHELLOSENSEI_VALUEFILE_H
