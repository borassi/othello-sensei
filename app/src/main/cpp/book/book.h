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
#include "../utils/misc.h"

constexpr int kMinFileSize = kMinValueFileSize;
constexpr int kMaxFileSize = 300;
constexpr char kBookFilepath[] = "book";
typedef u_int64_t HashMapIndex;

constexpr HashMapIndex kInitialHashMapSize = 8;
constexpr HashMapIndex kNumElementsOffset = sizeof(HashMapIndex) / sizeof(char);
constexpr HashMapIndex kOffset = 2 * sizeof(HashMapIndex) / sizeof(char);

class Book {

 public:
  Book(const std::string& folder);

  std::optional<BookTreeNode> Get(BitPattern player, BitPattern opponent);
  std::optional<BookTreeNode> Get(Board b) {
    return Get(b.Player(), b.Opponent());
  }

  void AddChildren(const Board& b, const std::vector<BookTreeNode*>& children, const std::vector<Board> ancestors) {
    BookTreeNode father = Get(b.Player(), b.Opponent()).value();
    assert(father.IsLeaf());
    father.SetChildren(children);
    NVisited visited = 0;
    for (const auto& child : children) {
      Put(*child, false, false);
      visited += child->GetNVisited();
    }
    father.AddDescendants(visited);
    Put(father, true, true);
    UpdateVisited(father, ancestors, visited);
  }

  BookTreeNode GetBookTreeNode(HashMapNode node);

  void Put(const BookTreeNode& node, const std::vector<Board> ancestors = {});

  bool IsSizeOK();

  void Print(int start = 0, int end = -1);

  void Clean();

  void UpdateFathers(const BookTreeNode& b) {
    for (Board father_board : b.Fathers()) {
      auto father = Get(father_board.Player(), father_board.Opponent()).value();

      auto children_board = GetUniqueNextBoardsWithPass(father_board);
      std::vector<BookTreeNode> children_memory;
      std::vector<BookTreeNode*> children_pointer;
      children_memory.reserve(children_board.size());
      children_pointer.reserve(children_board.size());

      for (auto child_board : children_board) {
        children_memory.push_back(Get(child_board.first).value());
        children_pointer.push_back(&children_memory[children_memory.size() - 1]);
      }

      assert(!father.IsLeaf());
      father.UpdateFather(children_pointer);
      Put(father, true, true);
    }
  }

  HashMapIndex Size() { return book_size_; }

 private:
  std::string folder_;
  std::vector<ValueFile> value_files_;
  HashMapIndex hash_map_size_;
  HashMapIndex book_size_;

  int GetValueFileOffset(int size);

  ValueFile& GetValueFile(int size);

  void UpdateSizes(std::fstream* file);

  void Put(const BookTreeNode& node, bool overwrite, bool update_fathers);

  void UpdateVisited(const BookTreeNode& leaf,
                     const std::vector<Board>& ancestors,
                     NVisited visited) {
    BookTreeNode ancestor = leaf;
    for (auto it = ancestors.rbegin(); it != ancestors.rend(); ++it) {
      Board ancestor_board = *it;
      assert(Contains(ancestor.Fathers(), ancestor_board.Unique()));
      ancestor = Get(ancestor_board).value();
      ancestor.AddDescendants(visited);
      Put(ancestor, true, false);
    }
  }

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

  std::pair<std::fstream, std::optional<BookTreeNode>> Remove(BitPattern player, BitPattern opponent);

  std::tuple<std::fstream, HashMapNode, std::optional<BookTreeNode>>
      Find(BitPattern player, BitPattern opponent);

  void Resize(std::fstream* file, std::vector<HashMapNode> add_elements);

  bool IsEmpty(std::fstream& file, HashMapIndex hash);
};



#endif //OTHELLOSENSEI_POSITION_TO_DATA_H
