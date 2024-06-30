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
  auto file = GetFile();
  return Get(offset, file);
}

std::vector<char> ValueFile::Get(BookFileOffset offset, std::fstream& file) const {
  std::vector<char> result(size_);
  Seek(offset, file);
  file.read(&result[0], size_ * sizeof(char));
  return result;
}

void ValueFile::Print() {
  auto file = GetFile();
  auto elements = Elements();
  std::vector<char> content(elements);
  file.read(&content[0], elements * sizeof(char));
  std::cout << "Value file with " << elements << " elements:";
  for (int i = 0; i < content.size(); ++i) {
    if (i % 5 == 0) {
      std::cout << "\n ";
    }
    std::cout << " " << (int) content[i];
  }
  std::cout << "\n";
}

std::fstream ValueFile::GetFile() const {
  return std::fstream(filename_, std::ios::binary | std::ios::out | std::ios::in);
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