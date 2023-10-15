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

#include "../utils/assets.h"

BookFileOffset ValueFile::Add(const std::vector<char>& value) {
  assert(value.size() <= size_);
  assert(value.size() > size_ / 2);
  auto asset = GetAsset();
  BookFileOffset offset;
  asset->Read((char*) &offset, sizeof(offset));
  if (offset == 0) {
    offset = Elements();
  } else {
    Seek(offset, asset.get());
    BookFileOffset new_offset;
    asset->Read((char*) &new_offset, sizeof(new_offset));
    SetAsEmpty(0, new_offset, asset.get());
  }
    Seek(offset, asset.get());
  asset->Write(&value[0], value.size() * sizeof(char));
  if (size_ != value.size()) {
    std::vector<char> zeros(size_ - value.size(), {0});
    asset->Write(&zeros[0], zeros.size() * sizeof(char));
  }
  return offset;
}

std::vector<char> ValueFile::Remove(BookFileOffset offset) {
  auto asset = GetAsset();
  std::vector<char> result = Get(offset, asset.get());
  Seek(0, asset.get());
  BookFileOffset next_free;
  asset->Read((char*) &next_free, sizeof(next_free));
  SetAsEmpty(offset, next_free, asset.get());
  SetAsEmpty(0, offset, asset.get());
  return result;
}

std::vector<char> ValueFile::Get(BookFileOffset offset) const {
  auto asset = GetAsset();
  return Get(offset, asset.get());
}

std::vector<char> ValueFile::Get(BookFileOffset offset, Asset* asset) const {
  std::vector<char> result(size_);
  Seek(offset, asset);
  asset->Read(&result[0], size_ * sizeof(char));
  return result;
}

void ValueFile::Print() {
  auto asset = GetAsset();
  auto elements = Elements();
  std::vector<char> content(elements);
  asset->Read(&content[0], elements * sizeof(char));
  std::cout << "Value file with " << elements << " elements:";
  for (int i = 0; i < content.size(); ++i) {
    if (i % 5 == 0) {
      std::cout << "\n ";
    }
    std::cout << " " << (int) content[i];
  }
  std::cout << "\n";
}

std::unique_ptr<Asset> ValueFile::GetAsset() const {
  return ::GetAsset(filename_);
}

void ValueFile::Seek(BookFileOffset offset, Asset* asset) const {
  asset->Seek(offset * size_);
}

void ValueFile::SetAsEmpty(BookFileOffset offset, BookFileOffset next_empty, Asset* asset) {
  Seek(offset, asset);
  std::vector<char> to_write(size_ - sizeof(BookFileOffset) / sizeof(char), 0);
  asset->Write((char*) &next_empty, sizeof(BookFileOffset));
  asset->Write(&to_write[0], to_write.size() * sizeof(char));
}