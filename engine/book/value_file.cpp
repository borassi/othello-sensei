/*
 * Copyright 2022-2025 Michele Borassi
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

#include <cerrno>   // Required for errno
#include <sstream>
#include <system_error> // Required for std::system_category

#include "value_file.h"

#include "../utils/files.h"

BookFileOffset ValueFile::Add(const std::vector<char>& value) {
  assert(value.size() <= size_);
  assert(value.size() > size_ / 2);
  auto file = GetFile();
  BookFileOffset offset;
  file.read((char*) &offset, sizeof(offset));
  if (offset == 0) {
    offset = Elements();
  } else {
    Seek(offset, file);
    BookFileOffset new_offset;
    file.read((char*) &new_offset, sizeof(new_offset));
    SetAsEmpty(0, new_offset, file);
  }
  Seek(offset, file);
  file.write(&value[0], value.size() * sizeof(char));
  if (size_ != value.size()) {
    std::vector<char> zeros(size_ - value.size(), {0});
    file.write(&zeros[0], zeros.size() * sizeof(char));
  }
  return offset;
}

std::vector<char> ValueFile::Remove(BookFileOffset offset) {
  auto file = GetFile();
  std::vector<char> result = Get(offset, file);
  Seek(0, file);
  BookFileOffset next_free;
  file.read((char*) &next_free, sizeof(next_free));
  SetAsEmpty(offset, next_free, file);
  SetAsEmpty(0, offset, file);
  return result;
}

std::vector<char> ValueFile::Get(BookFileOffset offset) const {
  std::shared_ptr<std::fstream> file = GetFileCached();
  return Get(offset, *file);
}

std::vector<char> ValueFile::Get(BookFileOffset offset, std::fstream& file) const {
  std::vector<char> result(size_);
  Seek(offset, file);
  file.read(&result[0], size_ * sizeof(char));
  return result;
}

void ValueFile::Print() const {
  std::shared_ptr<std::fstream> file = GetFileCached();
  auto elements = Elements();
  std::vector<char> content(elements * Size());
  file->read(&content[0], elements * Size() * sizeof(char));
  std::cout << "Value file with " << elements << " elements:";
  for (int i = 0; i < content.size(); ++i) {
    if (i % Size() == 0) {
      std::cout << "\n" << i / Size() << ": ";
    }
    std::cout << " " << (int) content[i];
  }
  std::cout << "\n";
}

std::fstream ValueFile::GetFile() const {
  cached_file_ = nullptr;
  return std::fstream(filename_, std::ios::binary | std::ios::out | std::ios::in);
}

std::shared_ptr<std::fstream> ValueFile::GetFileCached() const {
  if (cached_file_ != nullptr) {
    return cached_file_;
  }
  // We can't use GetFile() because the fstream must be on the stack and not on the heap.
  auto file = std::make_shared<std::fstream>(filename_, std::ios::binary | std::ios::in);
  if (!file->is_open()) {
    std::stringstream error;
    error << "Error opening file: " << std::system_category().message(errno) << " (code: " << errno << ")";
    throw std::invalid_argument(error.str());
  }
  if (FileLength(*file) <= size_) {
    return file;
  }
  cached_file_ = std::move(file);
  return cached_file_;
}

void ValueFile::Seek(BookFileOffset offset, std::fstream& file) const {
  file.seekg(offset * size_);
  file.seekp(file.tellg());
}

void ValueFile::SetAsEmpty(BookFileOffset offset, BookFileOffset next_empty, std::fstream& file) {
  Seek(offset, file);
  std::vector<char> to_write(size_ - sizeof(BookFileOffset) / sizeof(char), 0);
  file.write((char*) &next_empty, sizeof(BookFileOffset));
  file.write(to_write.data(), to_write.size() * sizeof(char));
}