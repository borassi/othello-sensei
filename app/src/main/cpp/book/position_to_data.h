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
#include <string>

#include "value_file.h"
#include "../board/bitpattern.h"
#include "../board/board.h"
#include "../utils/file.h"

constexpr int kMinFileSize = kMinValueFileSize;
constexpr int kMaxFileSize = 100;
typedef u_int64_t HashMapIndex;

constexpr HashMapIndex kInitialHashMapSize = 8;
constexpr HashMapIndex kNumElementsOffset = sizeof(HashMapIndex) / sizeof(char);
constexpr HashMapIndex kOffset = 2 * sizeof(HashMapIndex) / sizeof(char);

class HashMapNode {
 public:
  HashMapNode() : size_(0), offset_(0) {}
  HashMapNode(u_int8_t size, FileOffset offset) : size_(size), offset_(offset) {}

  FileOffset Offset() const { return offset_; }
  u_int8_t Size() const { return size_; }
  bool IsEmpty() const { return size_ == 0; }

 private:
  u_int8_t size_;
  FileOffset offset_;
};

class PositionToData {

 public:
  PositionToData(std::string folder) : folder_(folder), value_files_() {
    for (int i = kMinFileSize; i <= kMaxFileSize; ++i) {
      value_files_.push_back(ValueFile(folder, i));
    }
    if (!FileExists(IndexFilename())) {
      CreateEmptyFile(IndexFilename());
    }
    std::fstream index_file(IndexFilename());
    if (FileSize(index_file) < kOffset) {
      index_file.close();
      Clean();
    } else {
      index_file.read((char*) &hash_map_size_, sizeof(hash_map_size_));
      index_file.read((char*) &book_size_, sizeof(book_size_));
    }
  }

  std::vector<char> Get(BitPattern player, BitPattern opponent) {
    return Find(player, opponent).second;
  }

  u_int64_t OffsetToFilePosition(HashMapIndex offset) {
    return kOffset + offset * sizeof(HashMapNode) / sizeof(char);
  }

  std::pair<SerializedBoard, std::vector<char>> GetBoardAndValue(HashMapNode node) {
    std::vector<char> v = GetValueFile(node.Size()).Get(node.Offset());
    SerializedBoard board;
    std::copy_n(v.begin(), 13, board.begin());
    return std::make_pair(board, std::vector<char>(v.begin() + 13, v.end()));
  }

  HashMapIndex PowerAfterHashMapSize() {
    return 1ULL << (sizeof(HashMapIndex) * 8 - __builtin_clzll(hash_map_size_ - 1));
  }

  HashMapIndex PowerBeforeHashMapSize() {
    return PowerAfterHashMapSize() / 2;
  }

  HashMapIndex RepositionHash(HashMapIndex board_hash) {
    HashMapIndex hash = board_hash % PowerAfterHashMapSize();
    if (hash >= hash_map_size_) {
      return hash - PowerBeforeHashMapSize();
    }
    return hash;
  }

  std::pair<std::fstream, std::vector<char>>
      Find(BitPattern player, BitPattern opponent) {
    Board unique = Board(player, opponent).Unique();
    player = unique.Player();
    opponent = unique.Opponent();
    auto hash = RepositionHash(HashFull(player, opponent));
    std::fstream file = OpenFile(IndexFilename());
    file.seekg(OffsetToFilePosition(hash));
    HashMapNode node;
    SerializedBoard board = Board(player, opponent).Serialize();

    while (true) {
      file.read((char*) &node, sizeof(HashMapNode));
      if (node.IsEmpty()) {
        file.seekg((unsigned) file.tellg() - sizeof(HashMapNode), std::ios::beg);
          return std::make_pair(std::move(file), std::vector<char>());
      }
      auto board_and_value = GetBoardAndValue(node);
      if (board_and_value.first == board) {
        file.seekg((unsigned) file.tellg() - sizeof(HashMapNode), std::ios::beg);
        return std::make_pair(std::move(file), board_and_value.second);
      }

      if (file.tellg() == OffsetToFilePosition(hash_map_size_)) {
        return std::make_pair(std::move(file), std::vector<char>());
      }
    }
  }

  void Put(BitPattern player, BitPattern opponent, std::vector<char> value) {
    SerializedBoard board = Board(player, opponent).Serialize();
    std::vector<char> to_store(board.begin(), board.end());
    to_store.insert(to_store.end(), value.begin(), value.end());
    auto size = to_store.size();
    auto offset = GetValueFile(size).Add(to_store);
    auto file_value = Find(player, opponent);
    auto file = std::move(file_value.first);
    if (file_value.second.empty()) {
      ++book_size_;
    }
    file.seekp(file.tellg());
    HashMapNode to_be_stored(size, offset);
    if (file.tellg() < OffsetToFilePosition(hash_map_size_)) {
      file.write((char*) &to_be_stored, sizeof(HashMapNode));
      Resize(&file, {});
    } else {
      Resize(&file, {to_be_stored});
    }
    UpdateSizes(&file);
    file.close();
//    if (book_size_ > 28979) {
//      Print();
//    }
  }

  void Resize(std::fstream* file, std::vector<HashMapNode> add_elements) {
    HashMapNode node_to_move;
    HashMapNode empty_node;

    int extra_buffer = add_elements.size();
    while (book_size_ > 0.4 * hash_map_size_ || extra_buffer > 0) {
      int current_buffer = 0;
      for (HashMapIndex invalidated = RepositionHash(hash_map_size_); true; ++invalidated) {
        file->seekg(OffsetToFilePosition(invalidated));
        file->read((char*) &node_to_move, sizeof(node_to_move));
        file->seekp(OffsetToFilePosition(invalidated));
        file->write((char*) &empty_node, sizeof(empty_node));
        if (node_to_move.IsEmpty()) {
          break;
        }
        add_elements.push_back(node_to_move);
        ++current_buffer;
      }
      extra_buffer = std::max(current_buffer, extra_buffer);
      file->seekp(0, std::ios::end);
      file->write((char*) &empty_node, sizeof(empty_node));
      ++hash_map_size_;
      --extra_buffer;
    }

    for (const HashMapNode& node_to_move : add_elements) {
      Board board = Board::Deserialize(GetBoardAndValue(node_to_move).first);
      HashMapIndex hash = RepositionHash(HashFull(board.Player(), board.Opponent()));
      HashMapNode board_in_position;
      file->seekg(OffsetToFilePosition(hash));
      file->read((char*) &board_in_position, sizeof(board_in_position));
      while (!board_in_position.IsEmpty()) {
        file->read((char*) &board_in_position, sizeof(board_in_position));
      }
      file->seekp((u_int64_t) file->tellg() - sizeof(HashMapNode));
      file->write((char*) &node_to_move, sizeof(node_to_move));
    }
//    file->close();
  }

  bool IsSizeOK() {
    std::fstream index_file = OpenFile(IndexFilename());
    if (OffsetToFilePosition(hash_map_size_) != FileSize(index_file)) {
      std::cout << "Wrong hash map size " << hash_map_size_ << ". Should be " << FileSize(index_file) << "\n";
//      Print();
      return false;
    }
    HashMapNode node;
    HashMapIndex num_elements = 0;
    index_file.seekg(kOffset);
    for (HashMapIndex hash = 0; hash < hash_map_size_; ++hash) {
      index_file.read((char*) &node, sizeof(HashMapNode));
      if (!node.IsEmpty()) {
        ++num_elements;
      }
    }
    if (num_elements != book_size_) {
      std::cout << "Wrong book size " << book_size_ << ". Should be " << num_elements << "\n";
//      Print();
      return false;
    }
    return true;
  }

  bool IsEmpty(std::fstream& file, HashMapIndex hash) {
    auto old_position = file.tellg();
    file.seekg(OffsetToFilePosition(hash));
    HashMapNode node;
    file.read((char*) &node, sizeof(node));
    file.seekg(old_position);
    return node.IsEmpty();
  }

  void Print(int start = 0, int end = -1) {
    std::fstream index_file = OpenFile(IndexFilename());
    int size = FileSize(index_file);
    HashMapNode node;
    if (end == -1) {
      end = hash_map_size_;
    }

    std::cout << "Book with " << book_size_ << " elements in " << hash_map_size_ << " buckets:\n";
    for (int hash = start; hash < end; ++hash) {
      index_file.seekg(OffsetToFilePosition(hash));
      index_file.read((char*) &node, sizeof(HashMapNode));
      if (node.IsEmpty()) {
        continue;
      }
      std::cout << "  " << hash << ": size=" << (int) node.Size() << " off=" << node.Offset() << " -> ";
      auto board_and_value = GetBoardAndValue(node);
      Board board = Board::Deserialize(board_and_value.first);
      std::cout << board.Player() << " " << board.Opponent() << ":";
      for (char c : board_and_value.second) {
        std::cout << " " << (int) c;
      }
      std::cout << "\n";
    }
  }

  void Clean() {
    std::ofstream(IndexFilename(), std::ios::out).close();
    u_int32_t to_write = 0;
    for (ValueFile& value_file : value_files_) {
      value_file.Clean();
    }
    auto file = OpenFile(IndexFilename());
    hash_map_size_ = kInitialHashMapSize;
    book_size_ = 0;
    UpdateSizes(&file);
    std::vector<HashMapNode> nodes(kInitialHashMapSize);
    file.write((char*) &nodes[0], kInitialHashMapSize * sizeof(HashMapNode));
  }

 private:
  std::string folder_;
  std::vector<ValueFile> value_files_;
  HashMapIndex hash_map_size_;
  HashMapIndex book_size_;

  int GetValueFileOffset(int size) {
    assert(size >= kMinFileSize);
    assert(size <= kMaxFileSize);
    return size - kMinFileSize;
  }

  ValueFile& GetValueFile(int size) {
    ValueFile& file = value_files_[GetValueFileOffset(size)];
    assert(file.Size() == size);
    return file;
  }

  void UpdateSizes(std::fstream* file) {
    file->seekp(0);
    file->write((char*) &hash_map_size_, sizeof(hash_map_size_));
    file->write((char*) &book_size_, sizeof(book_size_));
  }

  std::string IndexFilename() { return folder_ + "/index.sen"; }
};



#endif //OTHELLOSENSEI_POSITION_TO_DATA_H
