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

#include <cassert>
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
  ValueFile(std::string folder, ValueFileSize size) : folder_(folder), size_(size) {
    assert(size >= kMinValueFileSize);
  }

  ValueFileSize Size() { return size_; }

  BookFileOffset Add(const std::vector<char>& value);

  void Remove(BookFileOffset offset);

  std::vector<char> Get(BookFileOffset offset);

  void Clean() {
    std::ofstream(Filename(), std::ios::out).close();
    std::fstream file = OpenFile(Filename());
    SetAsEmpty(0, 0, &file);
  }

  BookFileOffset Elements() const {
    auto file = OpenFile(Filename());
    return FileSize(file) / size_;
  }

  void Print();

 private:
  std::string folder_;
  ValueFileSize size_;

  std::string Filename() const {
    return folder_ + "/value_" + std::to_string(size_) + ".val";
  }

  void Seek(BookFileOffset offset, std::fstream* file);
  void SetAsEmpty(BookFileOffset offset, BookFileOffset next_empty, std::fstream* file);
};

#endif //OTHELLOSENSEI_VALUEFILE_H
