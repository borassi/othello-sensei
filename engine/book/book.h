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
#ifndef OTHELLOSENSEI_POSITION_TO_DATA_H
#define OTHELLOSENSEI_POSITION_TO_DATA_H

#include <algorithm>
#include <atomic>
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
#include "../utils/misc.h"
#include "../utils/constants.h"

constexpr int kMinFileSize = kMinValueFileSize;
constexpr int kMaxFileSize = 524288; // 2^19
constexpr int kNumDoubling = 12;
constexpr char kBookFilepath[] = "book";
typedef uint64_t HashMapIndex;

constexpr HashMapIndex kInitialHashMapSize = 8;
constexpr HashMapIndex kNumElementsOffset = sizeof(HashMapIndex) / sizeof(char);
constexpr HashMapIndex kOffset = 2 * sizeof(HashMapIndex) / sizeof(char);

enum NodeType {
  LEAF = 0,
  FIRST_VISIT = 1,
  LAST_VISIT = 2,
};


#ifdef _MSC_VER
__pragma(pack(push, 1));
#endif
class HashMapNode {
 public:
  HashMapNode() : size_byte_(0), offset_(0) {}
  HashMapNode(int size, BookFileOffset offset) : offset_(offset) {
    size_byte_ = SizeToByte(size);
  }

  BookFileOffset Offset() const { return offset_; }
  int Size() const { return ByteToSize(size_byte_); }
  bool IsEmpty() const { return size_byte_ == 0; }
  bool operator==(const HashMapNode& other) const {
    return size_byte_ == other.size_byte_ && offset_ == other.offset_;
  }
  bool operator!=(const HashMapNode& other) const { return !operator==(other); }

  int GetValueFilePosition() const { return SizeByteToPosition(size_byte_); }
  static int SizeToPosition(int size) { return SizeByteToPosition(SizeToByte(size)); }
  static int SizeByteToPosition(int size_byte) { return size_byte - 1; }
  static uint8_t SizeToByte(int size);
  static int ByteToSize(uint8_t b);
 private:
  BookFileOffset offset_;
  uint8_t size_byte_;
}
#ifdef _MSC_VER
__pragma(pack(pop));
#else
__attribute__((__packed__));
#endif

std::ostream& operator<<(std::ostream& stream, const HashMapNode& n);

constexpr int kBookVersion = 1;

template<int version = kBookVersion>
class Book {
 public:
  typedef BookTreeNode<Book, version> BookNode;

  Book(const std::string& folder);

  // TODO: Remove code duplication between Get and Mutable.
  std::unique_ptr<Node> Get(const Board& b) const;

  std::unique_ptr<Node> Get(BitPattern player, BitPattern opponent) const {
    return Get(Board(player, opponent));
  }

  BookNode* Mutable(const Board& b) {
//    ReloadSizes();
    Board unique = b.Unique();
    auto iterator = modified_nodes_.find(unique);
    if (iterator != modified_nodes_.end()) {
      return iterator->second.get();
    }
    HashMapNode node = Find(unique.Player(), unique.Opponent()).second;
    if (!node.IsEmpty()) {
      std::vector<char> serialized = GetValueFile(node).Get(node.Offset());
      std::unique_ptr<BookNode> node = BookNode::Deserialize(this, serialized);
      iterator = modified_nodes_.insert(std::make_pair(unique, std::move(node))).first;
      return iterator->second.get();
    }
    return nullptr;
  }

  BookNode* Add(const TreeNode& node);

  void AddChildren(const Board& father_board, const std::vector<Node>& children);

  void Commit(bool verbose = false);

  bool IsSizeOK();

  void Print(int start = 0, int end = -1);

  void Clean();

  HashMapIndex Size() { return book_size_; }

  const std::unordered_set<Board>& Roots() const {
    return roots_;
  }

  Book RemoveDescendants(const std::string& filepath) {
    Book book_no_descendants(filepath);
    assert(book_no_descendants.Size() == 0);

    book_no_descendants.Merge(*this, [](Node* node) { node->ResetDescendants(); }, [](Node* node) { node->ResetDescendants(); });
    return book_no_descendants;
  }

  class Iterator {
   public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = std::pair<Node, NodeType>;
    using pointer           = value_type*;
    using reference         = value_type;

    Iterator(const Book& book, int start_root) : book_(book), roots_(book_.Roots().begin(), book_.Roots().end()), next_root_(start_root), stack_() {
      assert (next_root_ == 0 || next_root_ == roots_.size());
      ToNextRoot();
    }
    reference operator*() const {
      const Node& node = stack_.back();
      if (node.IsLeaf()) {
        return {node, LEAF};
      } else if (visited_.find(node.ToBoard()) != visited_.end()) {
        return {node, LAST_VISIT};
      } else {
        return {node, FIRST_VISIT};
      }
    }
//    pointer operator->() { return &operator*(); }

    // Prefix increment
    Iterator& operator++() {
      ToNext();
      return *this;
    }

    // Postfix increment
    Iterator operator++(int) {
      Iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    bool operator==(const Iterator& other) const {
      return &book_ == &other.book_ && roots_ == other.roots_ && stack_ == other.stack_ && next_root_ == other.next_root_;
    }
    bool operator!=(const Iterator& other) const {
      return !(*this == other);
    }

   private:
    const Book& book_;
    const std::vector<Board> roots_;
    std::vector<Node> stack_;
    int next_root_;
    std::unordered_set<Board> visited_;

    void ToNextRoot() {
      assert(stack_.empty());
      if (next_root_ == roots_.size()) {
        // Finished iteration.
        return;
      }
      auto next = book_.Get(roots_[next_root_++]);
      assert(next);
      stack_.push_back(*next);
    }

    void ToNext() {
      Node node = stack_.back();
      Board b = node.ToBoard();
      bool visited = visited_.find(b) != visited_.end();
      visited_.insert(b);
      if (visited || node.IsLeaf()) {
        stack_.pop_back();
        if (stack_.empty()) {
          ToNextRoot();
        }
      } else {
        for (auto child_board : GetUniqueNextBoardsWithPass(b)) {
          if (visited_.find(child_board.first) == visited_.end()) {
            auto child_in_book = book_.Get(child_board.first);
            assert(child_in_book);
            stack_.push_back(*child_in_book);
          }
        }
      }
    }
  };

  Iterator begin() const { return Iterator(*this, 0); }
  Iterator end() const { return Iterator(*this, (int) roots_.size()); }

  template<int other_version>
  void Merge(const Book<other_version>& other_book, void (*leaf_func)(Node*) = nullptr, void (*internal_func)(Node*) = nullptr) {
    // This avoids adding a lot of roots and removing them afterwards (to avoid
    // memory problems).
    for (const auto& root : other_book.Roots()) {
      Board unique = root.Unique();
      if (!Get(unique)) {
        roots_.insert(unique);
      }
    }
    for (auto other_node_and_type : other_book) {
      auto other_node = other_node_and_type.first;
      auto other_node_type = other_node_and_type.second;
      BookNode* my_node = Mutable(other_node.ToBoard());
      if (my_node) {
        if (other_node_type == LEAF || other_node_type == FIRST_VISIT) {
          my_node->AddDescendants(other_node.GetNVisited());
          my_node->lower_ = MaxEval(my_node->lower_, other_node.Lower());
          my_node->upper_ = MinEval(my_node->upper_, other_node.Upper());
        } else {
          if (my_node->IsLeaf()) {
            AddChildren(my_node->ToBoard(), {});
          } else {
            my_node->UpdateFather();
          }
        }
      } else {
        assert(other_node_type == LEAF || other_node_type == FIRST_VISIT);
        if (other_node_type == LEAF && leaf_func) {
          leaf_func(&other_node);
        }
        if (other_node_type == FIRST_VISIT && internal_func) {
          internal_func(&other_node);
        }
        AddNoRootsUpdate(other_node);
      }
    }
    Commit();
  }

  void ReloadSizes() {
    auto index_file = IndexFile();
    index_file.read((char*) &hash_map_size_, sizeof(hash_map_size_));
    index_file.read((char*) &book_size_, sizeof(book_size_));
  }

 private:
  std::string folder_;
  std::vector<ValueFile> value_files_;
  ValueFile roots_file_;
  std::unordered_set<Board> roots_;
  HashMapIndex hash_map_size_;
  HashMapIndex book_size_;
  // References are not invalidated:
  // https://stackoverflow.com/questions/39868640/stdunordered-map-pointers-reference-invalidation
  std::unordered_map<Board, std::unique_ptr<BookNode>> modified_nodes_;
  // If we receive a signal when we are updating the book, we store it here
  // instead of letting it kill the program. We re-raise this after we updated
  // everything, to avoid inconsistencies. We use an atomic in case multiple
  // signals come at the same time (not sure if it's really needed).
  static std::atomic_int received_signal_;

  void Commit(const BookNode& node);

  static void HandleSignal(int signal) { received_signal_ = signal; }

  BookNode GetBookNode(HashMapNode node);

  BookNode* AddNoRootsUpdate(const TreeNode& node);

  int GetValueFileOffset(int size);

  const ValueFile& GetValueFile(const HashMapNode& node) const;

  ValueFile& MutableValueFile(const HashMapNode& node) {
    return const_cast<ValueFile&>(GetValueFile(node));
  }

  void UpdateSizes(std::fstream& file);

  std::vector<BookNode> MissingChildren(const Board& b, const std::vector<BookNode*>& children);

  void UpdateFathers(const BookNode& b);

  uint64_t OffsetToFilePosition(HashMapIndex offset) const {
    return kOffset + offset * sizeof(HashMapNode) / sizeof(char);
  }
  std::string IndexFilename() const { return folder_ + "/index.sen"; }
  std::fstream IndexFile() const {
    return std::fstream(IndexFilename(), std::ios::binary | std::ios::out | std::ios::in);
  }

  HashMapIndex PowerAfterHashMapSize() const {
    return 1ULL << (sizeof(HashMapIndex) * 8 - __builtin_clzll(hash_map_size_ - 1));
  }

  HashMapIndex PowerBeforeHashMapSize() const {
    return PowerAfterHashMapSize() / 2;
  }

  HashMapIndex RepositionHash(HashMapIndex board_hash) const;

  std::pair<std::fstream, HashMapNode> Find(BitPattern player, BitPattern opponent) const;

  void Resize(std::fstream& file, std::vector<HashMapNode> add_elements);
};

template<int version>
std::atomic_int Book<version>::received_signal_(NSIG);

template<int version>
Book<version>::Book(const std::string& folder) : folder_(folder), value_files_(), roots_file_(folder + "/roots.val", kSerializedBoardSize), roots_() {
  for (int i = 1; i <= 255; ++i) {
    int size = HashMapNode::ByteToSize(i);
    std::string filename = folder + "/value_" + std::to_string(size) + ".val";
    value_files_.push_back(ValueFile(filename, size));
  }

  for (const auto& offset_serialized : roots_file_) {
    const auto& serialized = offset_serialized.second;
    roots_.insert(Board::Deserialize(serialized.begin()));
  }
  CreateFileIfNotExists(IndexFilename());
  auto index_file = IndexFile();
  if (FileLength(index_file) < kOffset) {
    index_file.close();
    CreateEmptyFileWithDirectories(IndexFilename());
    Clean();
  } else {
    index_file.close();
    ReloadSizes();
  }
}

template<int version>
void Book<version>::AddChildren(const Board& father_board, const std::vector<Node>& children) {
  Book<version>::BookNode* father = Mutable(father_board);
  assert(father);
  assert(father->IsLeaf());
  father->is_leaf_ = false;
  std::vector<TreeNode*> children_in_book;

  for (const auto& board_move : GetUniqueNextBoardsWithPass(father->ToBoard())) {
    auto board = board_move.first;
    Book<version>::BookNode* child_in_book = Mutable(board);
    const Node* new_child = nullptr;
    for (const Node& child : children) {
      if (child.ToBoard().Unique() == board) {
        new_child = &child;
        break;
      }
    }
    if (child_in_book) {
      if (new_child != nullptr) {
        child_in_book->AddDescendants(new_child->GetNVisited());
      }
      if (!child_in_book->HasFathers()) {
        roots_.erase(board);
      }
    } else {
      assert(new_child != nullptr);
      child_in_book = AddNoRootsUpdate(*new_child);
    }
    assert(child_in_book != nullptr);
    children_in_book.push_back(child_in_book);
  }
  father->SetChildren(children_in_book);
  assert(father->AreChildrenCorrect());
}

template<int version>
typename Book<version>::BookNode* Book<version>::Add(const TreeNode& node) {
  Board unique = node.ToBoard().Unique();
  roots_.insert(unique);
  return AddNoRootsUpdate(node);
}

template<int version>
typename Book<version>::BookNode* Book<version>::AddNoRootsUpdate(const TreeNode& node) {
  Board unique = node.ToBoard().Unique();
  assert(!Get(unique));
  auto iterator_inserted = modified_nodes_.emplace(unique, std::make_unique<BookNode>(this, node));
  iterator_inserted.first->second->is_leaf_ = true;
  assert(iterator_inserted.second);
  return iterator_inserted.first->second.get();
}

template<int version>
std::unique_ptr<Node> Book<version>::Get(const Board& b) const {
  Board unique = b.Unique();
  auto iterator = modified_nodes_.find(unique);
  if (iterator != modified_nodes_.end()) {
    return std::make_unique<Node>(*iterator->second);
  }
  HashMapNode node = Find(unique.Player(), unique.Opponent()).second;
  if (!node.IsEmpty()) {
    std::vector<char> serialized = GetValueFile(node).Get(node.Offset());
    return std::make_unique<Node>(Node::Deserialize(serialized, version, nullptr));
  }
  return nullptr;
}

template<int version>
void Book<version>::Commit(bool verbose) {
  // Logic taken from https://stackoverflow.com/questions/76798937.

#if defined(__GNUC__) || defined(__GNUG__) || defined(clang)
  signal(SIGINT, this->HandleSignal);   // CTRL-C
  signal(SIGHUP, this->HandleSignal);   // Close terminal
  signal(SIGQUIT, this->HandleSignal);  // CTRL-/
#endif
  if (verbose) {
    std::cout << "Committing " << modified_nodes_.size() << " nodes: " << std::flush;
  }
  roots_file_.Clean();
  for (const Board& b : roots_) {
    assert(b == b.Unique());
    roots_file_.Add(b.Serialize());
  }
  for (const auto& board_node : modified_nodes_) {
    if (verbose) {
      std::cout << "." << std::flush;
    }
    Commit(*board_node.second);
  }
  modified_nodes_.clear();
  auto file = IndexFile();
  UpdateSizes(file);
  assert(modified_nodes_.empty());
  file.close();
  if (verbose) {
    std::cout << " Done\n" << std::flush;
  }
  // Reset these actions to default behavior.
#if defined(__GNUC__) || defined(__GNUG__) || defined(clang)
  signal(SIGINT, SIG_DFL);
  signal(SIGHUP, SIG_DFL);
  signal(SIGQUIT, SIG_DFL);
#endif
  if (received_signal_ != NSIG) {
    raise(received_signal_);
  }
}

template<int version>
void Book<version>::Commit(const BookNode& node) {
  auto file_hash_map_node = Find(node.Player(), node.Opponent());
  auto& file = file_hash_map_node.first;
  auto& hash_map_node = file_hash_map_node.second;
  Board b = node.ToBoard();

  if (!hash_map_node.IsEmpty()) {
    std::vector<char> removed = MutableValueFile(hash_map_node).Remove(hash_map_node.Offset());
    std::unique_ptr<BookNode> removed_node = BookNode::Deserialize(this, removed);
    assert(node.NFathers() >= removed_node->NFathers());
    assert(b == removed_node->ToBoard());
    assert ((roots_.find(b) != roots_.end()) == !node.HasFathers());
  } else {
    ++book_size_;
  }

  std::vector<char> to_store = node.Serialize();
  int size = (int) to_store.size();
  int position = value_files_[HashMapNode::SizeToPosition((int) to_store.size())].Add(to_store);
  HashMapNode to_be_stored(size, position);
  if (file.tellg() < (int64_t) OffsetToFilePosition(hash_map_size_)) {
    file.write((char*) &to_be_stored, sizeof(HashMapNode));
    Resize(file, {});
  } else {
    Resize(file, {to_be_stored});
  }
  // So that the asserts below work.
  file.close();
  assert(Get(node.ToBoard()));
  assert(!Find(node.Player(), node.Opponent()).second.IsEmpty());
  assert(IsSizeOK());
}

template<int version>
bool Book<version>::IsSizeOK() {
  auto index_file = IndexFile();
  if (OffsetToFilePosition(hash_map_size_) != FileLength(index_file)) {
    std::cout << "Wrong hash map size " << OffsetToFilePosition(hash_map_size_) << ". Should be " << FileLength(index_file) << "\n";
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
  auto index_file = IndexFile();
  HashMapNode node;
  if (end == -1) {
    end = hash_map_size_;
  }

  std::cout << "Book with " << book_size_ << " elements in " << hash_map_size_ << " buckets:\n";
  for (int hash = start; hash < end; ++hash) {
    index_file->Seek(OffsetToFilePosition(hash));
    index_file->Read((char*) &node, sizeof(HashMapNode));
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
  for (ValueFile& value_file : value_files_) {
    value_file.Clean();
  }
  roots_file_.Clean();
  roots_.clear();
  auto file = IndexFile();
  hash_map_size_ = kInitialHashMapSize;
  book_size_ = 0;
  UpdateSizes(file);
  std::vector<HashMapNode> nodes(kInitialHashMapSize);
  file.write((char*) &nodes[0], kInitialHashMapSize * sizeof(HashMapNode));
  modified_nodes_.clear();
}

template<int version>
const ValueFile& Book<version>::GetValueFile(const HashMapNode& node) const {
  const ValueFile& file = value_files_[node.GetValueFilePosition()];
  assert(file.Size() == node.Size());
  return file;
}

template<int version>
void Book<version>::UpdateSizes(std::fstream& file) {
  file.seekp(0);
  file.write((char*) &hash_map_size_, sizeof(hash_map_size_));
  file.write((char*) &book_size_, sizeof(book_size_));
}

template<int version>
HashMapIndex Book<version>::RepositionHash(HashMapIndex board_hash) const {
  assert(board_hash >= 0);
  HashMapIndex hash = board_hash % PowerAfterHashMapSize();
  if (hash >= hash_map_size_) {
    return hash - PowerBeforeHashMapSize();
  }
  return hash;
}

template<int version>
std::pair<std::fstream, HashMapNode> Book<version>::Find(BitPattern player, BitPattern opponent) const {
  Board unique = Board(player, opponent).Unique();
  player = unique.Player();
  opponent = unique.Opponent();
  auto hash = RepositionHash(HashFull(player, opponent));
  assert(std::fstream(IndexFilename(), std::ios::in).is_open());
  auto file = IndexFile();
  file.seekg(OffsetToFilePosition(hash));
  HashMapNode node;

  while (true) {
    file.read((char*) &node, sizeof(HashMapNode));
    if (node.IsEmpty()) {
      file.seekg((unsigned long long) file.tellg() - sizeof(HashMapNode));
      file.seekp(file.tellg());
      return std::make_pair(std::move(file), node);
    }
    std::vector<char> v = GetValueFile(node).Get(node.Offset());
    Board b = Board::Deserialize(v.begin());
    assert(b == b.Unique());
    if (b.Player() == player && b.Opponent() == opponent) {
      file.seekg((unsigned) file.tellg() - sizeof(HashMapNode));
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
void Book<version>::Resize(std::fstream& file, std::vector<HashMapNode> add_elements) {
  HashMapNode node_to_move;
  HashMapNode empty_node;

  int extra_buffer = (int) add_elements.size();
  while (book_size_ > 0.4 * hash_map_size_ || extra_buffer > 0) {
    int current_buffer = 0;
    for (HashMapIndex invalidated = RepositionHash(hash_map_size_); true; ++invalidated) {
      file.seekg(OffsetToFilePosition(invalidated));
      file.read((char*) &node_to_move, sizeof(node_to_move));
      file.seekp(OffsetToFilePosition(invalidated));
      file.write((char*) &empty_node, sizeof(empty_node));
      if (node_to_move.IsEmpty()) {
        break;
      }
      add_elements.push_back(node_to_move);
      ++current_buffer;
    }
    extra_buffer = std::max(current_buffer, extra_buffer);
    file.seekp(0, std::ios::end);
    file.write((char*) &empty_node, sizeof(empty_node));
    ++hash_map_size_;
    --extra_buffer;
  }

  for (const HashMapNode& node_to_move : add_elements) {
    std::vector<char> v = GetValueFile(node_to_move).Get(node_to_move.Offset());
    Board board = Board::Deserialize(v.begin());
    HashMapIndex hash = RepositionHash(HashFull(board.Player(), board.Opponent()));
    HashMapNode board_in_position;
    file.seekg(OffsetToFilePosition(hash));
    file.read((char*) &board_in_position, sizeof(board_in_position));
    while (!board_in_position.IsEmpty()) {
      file.read((char*) &board_in_position, sizeof(board_in_position));
    }
    file.seekp((uint64_t) file.tellg() - sizeof(HashMapNode));
    file.write((char*) &node_to_move, sizeof(node_to_move));
  }
}

#endif //OTHELLOSENSEI_POSITION_TO_DATA_H
