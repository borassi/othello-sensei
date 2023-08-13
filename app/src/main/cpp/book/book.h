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
#include <atomic>
#include <fstream>
#include <iterator>
#include <memory>
#include <optional>
#include <signal.h>
#include <stdio.h>
#include <string>
#include <unordered_map>

#include "book_tree_node.h"
#include "value_file.h"
#include "../board/bitpattern.h"
#include "../board/board.h"
#include "../utils/file.h"
#include "../utils/misc.h"

constexpr int kMinFileSize = kMinValueFileSize;
constexpr int kMaxFileSize = 524288; // 2^19
constexpr int kNumDoubling = 12;
constexpr char kBookFilepath[] = "book";
typedef u_int64_t HashMapIndex;

constexpr HashMapIndex kInitialHashMapSize = 8;
constexpr HashMapIndex kNumElementsOffset = sizeof(HashMapIndex) / sizeof(char);
constexpr HashMapIndex kOffset = 2 * sizeof(HashMapIndex) / sizeof(char);

class HashMapNode {
 public:
  HashMapNode() : size_byte_(0), offset_(0) {}
  HashMapNode(int size, BookFileOffset offset) : offset_(offset) {
    size_byte_ = SizeToByte(size);
  }

  BookFileOffset Offset() const { return offset_; }
  int Size() const { return ByteToSize(size_byte_); }
  bool IsEmpty() const { return size_byte_ == 0; }
  bool operator==(const HashMapNode& other) const = default;

  int GetValueFilePosition() const { return SizeByteToPosition(size_byte_); }
  static int SizeToPosition(int size) { return SizeByteToPosition(SizeToByte(size)); }
  static int SizeByteToPosition(int size_byte) { return size_byte - 1; }
  static u_int8_t SizeToByte(int size);
  static int ByteToSize(u_int8_t b);
 private:
  BookFileOffset offset_;
  u_int8_t size_byte_;
} __attribute__((packed));

std::ostream& operator<<(std::ostream& stream, const HashMapNode& n);

constexpr int kBookVersion = 1;

template<int version = kBookVersion>
class Book {
 public:
  typedef BookTreeNode<Book, version> BookNode;

  Book(const std::string& folder);

  // TODO: Add Mutable alongside Get, to avoid having to commit only for a read.
  // Difficulty: we cannot copy TreeNodes (what if Get returns a value in the
  // hash map?).
  std::optional<BookNode*> Get(const Board& b);

  std::optional<BookNode*> Get(BitPattern player, BitPattern opponent) {
    return Get(Board(player, opponent));
  }

  BookNode* Add(const TreeNode& node);

  void Commit(bool verbose = false);

  bool IsSizeOK();

  void Print(int start = 0, int end = -1);

  void Clean();

  HashMapIndex Size() { return book_size_; }

  std::vector<Board> Roots() {
    // We update the roots when we commit, so we cannot call this function if
    // we did not commit.
    // TODO: Remove this problem by updating roots_ when we add a new node.
    assert(modified_nodes_.empty());
    std::vector<Board> result;
    for (const auto& [board, unused_offset] : roots_) {
      result.push_back(board);
    }
    return result;
  }

 private:
  std::string folder_;
  std::vector<ValueFile> value_files_;
  ValueFile roots_file_;
  std::unordered_map<Board, int> roots_;
  HashMapIndex hash_map_size_;
  HashMapIndex book_size_;
  // References are not invalidated:
  // https://stackoverflow.com/questions/39868640/stdunordered-map-pointers-reference-invalidation
  std::unordered_map<Board, BookNode> modified_nodes_;
  // If we receive a signal when we are updating the book, we store it here
  // instead of letting it kill the program. We re-raise this after we updated
  // everything, to avoid inconsistencies. We use an atomic in case multiple
  // signals come at the same time (not sure if it's really needed).
  static std::atomic_int received_signal_;

  void Commit(BookNode& node);

  static void HandleSignal(int signal) { received_signal_ = signal; }

  BookNode GetBookNode(HashMapNode node);

  int GetValueFileOffset(int size);

  ValueFile& GetValueFile(const HashMapNode& node);

  void UpdateSizes(std::fstream* file);

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

template<int version>
std::atomic_int Book<version>::received_signal_ = NSIG;

template<int version>
Book<version>::Book(const std::string& folder) : folder_(folder), value_files_(), roots_file_(folder + "/roots.val", kSerializedBoardSize), roots_() {
  for (int i = 1; i <= 255; ++i) {
    int size = HashMapNode::ByteToSize(i);
    std::string filename = folder + "/value_" + std::to_string(size) + ".val";
    value_files_.push_back(ValueFile(filename, size));
  }

  for (const auto& [offset, serialized] : roots_file_) {
    roots_[Board::Deserialize(serialized.begin())] = offset;
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

template<int version>
typename Book<version>::BookNode* Book<version>::Add(const TreeNode& node) {
  Board unique = node.ToBoard().Unique();
  assert(!Get(unique));
  auto [iterator, inserted] = modified_nodes_.try_emplace(unique, this, node);
  assert(inserted);
  return &iterator->second;
}

template<int version>
std::optional<typename Book<version>::BookNode*> Book<version>::Get(const Board& b) {
  Board unique = b.Unique();
  auto iterator = modified_nodes_.find(unique);
  if (iterator != modified_nodes_.end()) {
    return &(iterator->second);
  }
  HashMapNode node = Find(unique.Player(), unique.Opponent()).second;
  if (!node.IsEmpty()) {
    std::vector<char> serialized = GetValueFile(node).Get(node.Offset());
    iterator = modified_nodes_.try_emplace(unique, this, serialized).first;
    return &iterator->second;
  }
  return std::nullopt;
}

template<int version>
void Book<version>::Commit(bool verbose) {
  // Logic taken from https://stackoverflow.com/questions/76798937.
  signal(SIGINT, this->HandleSignal);   // CTRL-C
  signal(SIGHUP, this->HandleSignal);   // Close terminal
  signal(SIGQUIT, this->HandleSignal);  // CTRL-/
  if (verbose) {
    std::cout << "Committing " << modified_nodes_.size() << " nodes: " << std::flush;
  }
  for (const auto& [board, node] : modified_nodes_) {
    if (verbose) {
      std::cout << "." << std::flush;
    }
    Commit(node);
  }
  modified_nodes_.clear();
  auto file = OpenFile(IndexFilename());
  UpdateSizes(&file);
  assert(modified_nodes_.empty());
  file.close();
  if (verbose) {
    std::cout << " Done\n" << std::flush;
  }
  // Reset these actions to default behavior.
  signal(SIGINT, SIG_DFL);
  signal(SIGHUP, SIG_DFL);
  signal(SIGQUIT, SIG_DFL);
  if (received_signal_ != NSIG) {
    raise(received_signal_);
  }
}

template<int version>
void Book<version>::Commit(const BookNode& node) {
  auto [file, hash_map_node] = Find(node.Player(), node.Opponent());
  Board b = node.ToBoard();

  if (!hash_map_node.IsEmpty()) {
    std::vector<char> removed = GetValueFile(hash_map_node).Remove(hash_map_node.Offset());
    const BookNode removed_node(this, removed);
    assert(node.NFathers() >= removed_node.NFathers());
    assert(b == removed_node.ToBoard());
    if (removed_node.HasFathers()) {
      assert (!roots_.contains(b));
    } else if (node.HasFathers()) {
      assert (roots_.contains(b));
      roots_file_.Remove(roots_[b]);
      roots_.erase(b);
    }
  } else {
    ++book_size_;
    if (!node.HasFathers()) {
      roots_[b] = roots_file_.Add(b.Serialize());
    }
  }

  std::vector<char> to_store = node.Serialize();
  auto size = to_store.size();
  auto position = value_files_[HashMapNode::SizeToPosition(to_store.size())].Add(to_store);
  HashMapNode to_be_stored(size, position);
  if (file.tellg() < OffsetToFilePosition(hash_map_size_)) {
    file.write((char*) &to_be_stored, sizeof(HashMapNode));
    Resize(&file, {});
  } else {
    Resize(&file, {to_be_stored});
  }
  // So that the asserts below work.
  file.close();
  assert(Get(node.ToBoard()));
  assert(!Find(node.Player(), node.Opponent()).second.IsEmpty());
  assert(IsSizeOK());
}

template<int version>
bool Book<version>::IsSizeOK() {
  std::fstream index_file = OpenFile(IndexFilename());
  if (OffsetToFilePosition(hash_map_size_) != FileSize(index_file)) {
    std::cout << "Wrong hash map size " << OffsetToFilePosition(hash_map_size_) << ". Should be " << FileSize(index_file) << "\n";
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
    return false;
  }
  return true;
}

template<int version>
void Book<version>::Print(int start, int end) {
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
    std::cout << "  " << hash << ": size=" << (int) node.Size() << " off="
              << node.Offset() << " -> " << node << "\n";
  }
}

template<int version>
void Book<version>::Clean() {
  std::ofstream(IndexFilename(), std::ios::out).close();
  u_int32_t to_write = 0;
  for (ValueFile& value_file : value_files_) {
    value_file.Clean();
  }
  roots_file_.Clean();
  roots_.clear();
  auto file = OpenFile(IndexFilename());
  hash_map_size_ = kInitialHashMapSize;
  book_size_ = 0;
  UpdateSizes(&file);
  std::vector<HashMapNode> nodes(kInitialHashMapSize);
  file.write((char*) &nodes[0], kInitialHashMapSize * sizeof(HashMapNode));
  modified_nodes_.clear();
}

template<int version>
ValueFile& Book<version>::GetValueFile(const HashMapNode& node) {
  ValueFile& file = value_files_[node.GetValueFilePosition()];
  assert(file.Size() == node.Size());
  return file;
}

template<int version>
void Book<version>::UpdateSizes(std::fstream* file) {
  file->seekp(0);
  file->write((char*) &hash_map_size_, sizeof(hash_map_size_));
  file->write((char*) &book_size_, sizeof(book_size_));
}

template<int version>
HashMapIndex Book<version>::RepositionHash(HashMapIndex board_hash) {
  assert(board_hash >= 0);
  HashMapIndex hash = board_hash % PowerAfterHashMapSize();
  if (hash >= hash_map_size_) {
    return hash - PowerBeforeHashMapSize();
  }
  return hash;
}

template<int version>
std::pair<std::fstream, HashMapNode> Book<version>::Find(BitPattern player, BitPattern opponent) {
  Board unique = Board(player, opponent).Unique();
  player = unique.Player();
  opponent = unique.Opponent();
  auto hash = RepositionHash(HashFull(player, opponent));
  std::fstream file = OpenFile(IndexFilename());
  assert(file);
  file.seekg(OffsetToFilePosition(hash));
  HashMapNode node;

  while (true) {
    file.read((char*) &node, sizeof(HashMapNode));
    if (node.IsEmpty()) {
      file.seekg((unsigned long long) file.tellg() - sizeof(HashMapNode), std::ios::beg);
      file.seekp(file.tellg());
      return std::make_pair(std::move(file), node);
    }
    std::vector<char> v = GetValueFile(node).Get(node.Offset());
    BookNode book_tree_node(this, v, false);
    Board b = book_tree_node.ToBoard();
    assert(b == b.Unique());
    if (b.Player() == player && b.Opponent() == opponent) {
      file.seekg((unsigned) file.tellg() - sizeof(HashMapNode), std::ios::beg);
      file.seekp(file.tellg());
      return std::make_pair(std::move(file), node);
    }

    if (file.tellg() == OffsetToFilePosition(hash_map_size_)) {
      file.seekp(file.tellg());
      return std::make_pair(std::move(file), HashMapNode());
    }
  }
}

template<int version>
void Book<version>::Resize(std::fstream* file, std::vector<HashMapNode> add_elements) {
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
    std::vector<char> v = GetValueFile(node_to_move).Get(node_to_move.Offset());
    BookNode node(this, v, false);
    Board board = node.ToBoard();
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
}

#endif //OTHELLOSENSEI_POSITION_TO_DATA_H
