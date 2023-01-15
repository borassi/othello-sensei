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
#include <memory>
#include <optional>
#include <stdio.h>
#include <string>

#include "book_tree_node.h"
#include "value_file.h"
#include "../board/bitpattern.h"
#include "../board/board.h"
#include "../utils/file.h"
#include "../utils/misc.h"

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
  bool operator==(const HashMapNode& other) const = default;

 private:
  BookFileOffset offset_;
  u_int8_t size_;
} __attribute__((packed));

std::ostream& operator<<(std::ostream& stream, const HashMapNode& n);

class Book;

typedef BookTreeNode<Book> BookNode;

class Book {
 public:
  Book(const std::string& folder);

  BookNode* Get(const Board& b);

  BookNode* Get(BitPattern player, BitPattern opponent) {
    return Get(Board(player, opponent));
  }

  void Commit();

  bool IsSizeOK();

  void Print(int start = 0, int end = -1);

  void Clean();

  HashMapIndex Size() { return book_size_; }

 private:
  std::string folder_;
  std::vector<ValueFile> value_files_;
  HashMapIndex hash_map_size_;
  HashMapIndex book_size_;
  // References are not invalidated:
  // https://stackoverflow.com/questions/39868640/stdunordered-map-pointers-reference-invalidation
  std::unordered_map<Board, BookNode> modified_nodes_;

  void Commit(const BookNode& node);

  BookNode GetBookNode(HashMapNode node);

  int GetValueFileOffset(int size);

  ValueFile& GetValueFile(int size);

  void UpdateSizes(std::fstream* file);

  void Put(const BookNode& node, bool overwrite, bool update_fathers);

  std::vector<BookNode> MissingChildren(const Board& b, const std::vector<BookNode*>& children);

  void UpdateFathers(const BookNode& b);

  u_int64_t OffsetToFilePosition(HashMapIndex offset) {
    return kOffset + offset * sizeof(HashMapNode) / sizeof(char);
  }
  std::string IndexFilename() { return folder_ + "/index.sen"; }

  HashMapIndex PowerAfterHashMapSize() {
    return 1ULL << (sizeof(HashMapIndex) * 8 - __builtin_clzll(hash_map_size_ - 1));
  }

  HashMapIndex PowerBeforeHashMapSize() {
    return PowerAfterHashMapSize() / 2;
  }

  HashMapIndex RepositionHash(HashMapIndex board_hash);

  std::pair<std::fstream, HashMapNode> Find(BitPattern player, BitPattern opponent);

  void Resize(std::fstream* file, std::vector<HashMapNode> add_elements);
};



#endif //OTHELLOSENSEI_POSITION_TO_DATA_H
