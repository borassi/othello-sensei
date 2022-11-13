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

#include "value_file.h"

#include "../utils/file.h"

FileOffset ValueFile::Add(const std::vector<char>& value) {
  assert(value.size() == size_);
  std::fstream file = OpenFile(Filename());
  FileOffset offset;
  file.read((char*) &offset, sizeof(offset));
  if (offset == 0) {
    offset = Elements();
  } else {
    Seek(offset, &file);
    FileOffset new_offset;
    file.read((char*) &new_offset, sizeof(new_offset));
    SetAsEmpty(0, new_offset, &file);
  }
  Seek(offset, &file);
  file.write(&value[0], value.size() * sizeof(char));
  return offset;
}

void ValueFile::Remove(FileOffset offset) {
  std::fstream file = OpenFile(Filename());
  Seek(0, &file);
  FileOffset next_free;
  file.read((char*) &next_free, sizeof(next_free));
  SetAsEmpty(offset, next_free, &file);
  SetAsEmpty(0, offset, &file);
}

std::vector<char> ValueFile::Get(FileOffset offset) {
  std::vector<char> result(size_);
  std::fstream file = OpenFile(Filename());
  Seek(offset, &file);
  file.read(&result[0], size_ * sizeof(char));
  return result;
}

void ValueFile::Print() {
  auto file = OpenFile(Filename());
  auto size = FileSize(file);
  std::vector<char> content(size);
  file.read(&content[0], size * sizeof(char));
  std::cout << "Value file with " << Elements() << " elements:";
  for (int i = 0; i < content.size(); ++i) {
    if (i % 5 == 0) {
      std::cout << "\n ";
    }
    std::cout << " " << (int) content[i];
  }
  std::cout << "\n";
}

void ValueFile::Seek(FileOffset offset, std::fstream* file) {
  file->seekg(offset * size_);
  file->seekp(file->tellg(), std::ios::beg);
}

void ValueFile::SetAsEmpty(FileOffset offset, FileOffset next_empty, std::fstream* file) {
  Seek(offset, file);
  std::vector<char> to_write(size_ - sizeof(FileOffset) / sizeof(char), 0);
  file->write((char*) &next_empty, sizeof(FileOffset));
  file->write(&to_write[0], to_write.size() * sizeof(char));
}