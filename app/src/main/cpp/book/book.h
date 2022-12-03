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
#ifndef OTHELLOSENSEI_POSITION_TO_DATA_H
#define OTHELLOSENSEI_POSITION_TO_DATA_H

#include <algorithm>
#include <fstream>
#include <optional>
#include <stdio.h>
#include <string>

#include "book_tree_node.h"
#include "value_file.h"
#include "../board/bitpattern.h"
#include "../board/board.h"
#include "../utils/file.h"

constexpr int kMinFileSize = kMinValueFileSize;
constexpr int kMaxFileSize = 300;
constexpr char kBookFilepath[] = "book";
typedef u_int64_t HashMapIndex;

constexpr HashMapIndex kInitialHashMapSize = 8;
constexpr HashMapIndex kNumElementsOffset = sizeof(HashMapIndex) / sizeof(char);
constexpr HashMapIndex kOffset = 2 * sizeof(HashMapIndex) / sizeof(char);

class HashMapNode {
 public:
  HashMapNode() : size_(0), offset_(0) {}
  HashMapNode(u_int8_t size, BookFileOffset offset) : size_(size), offset_(offset) {}

  BookFileOffset Offset() const { return offset_; }
  u_int8_t Size() const { return size_; }
  bool IsEmpty() const { return size_ == 0; }

 private:
  BookFileOffset offset_;
  u_int8_t size_;
} __attribute__((packed));

class Book {

 public:
  Book(const std::string& folder);

  std::optional<BookTreeNode> Get(BitPattern player, BitPattern opponent);

  void Put(const BaseTreeNode& node);

  bool IsSizeOK();

  void Print(int start = 0, int end = -1);

  void Clean();

  HashMapIndex Size() { return book_size_; }

 private:
  std::string folder_;
  std::vector<ValueFile> value_files_;
  HashMapIndex hash_map_size_;
  HashMapIndex book_size_;

  int GetValueFileOffset(int size);

  ValueFile& GetValueFile(int size);

  void UpdateSizes(std::fstream* file);

  u_int64_t OffsetToFilePosition(HashMapIndex offset) {
    return kOffset + offset * sizeof(HashMapNode) / sizeof(char);
  }
  std::string IndexFilename() { return folder_ + "/index.sen"; }

  BookTreeNode GetBookTreeNode(HashMapNode node);

  HashMapIndex PowerAfterHashMapSize() {
    return 1ULL << (sizeof(HashMapIndex) * 8 - __builtin_clzll(hash_map_size_ - 1));
  }

  HashMapIndex PowerBeforeHashMapSize() {
    return PowerAfterHashMapSize() / 2;
  }

  HashMapIndex RepositionHash(HashMapIndex board_hash);

  std::tuple<std::fstream, HashMapNode, std::optional<BookTreeNode>>
      Find(BitPattern player, BitPattern opponent);

  void Resize(std::fstream* file, std::vector<HashMapNode> add_elements);

  bool IsEmpty(std::fstream& file, HashMapIndex hash);
};



#endif //OTHELLOSENSEI_POSITION_TO_DATA_H
