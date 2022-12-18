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
 #include "book.h"


Book::Book(const std::string& folder) : folder_(folder), value_files_() {
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

std::optional<BookTreeNode> Book::Get(BitPattern player, BitPattern opponent) {
  return std::get<2>(Find(player, opponent));
}

std::pair<std::fstream, std::optional<BookTreeNode>> Book::Remove(
    BitPattern player, BitPattern opponent) {
  auto triple = Find(player, opponent);
  auto file = std::move(std::get<0>(triple));
  auto hash_map_node = std::move(std::get<1>(triple));
  auto tree_node = std::move(std::get<2>(triple));
  if (tree_node) {
    HashMapNode empty;
    GetValueFile(hash_map_node.Size()).Remove(hash_map_node.Offset());
    file.write((char*) &empty, sizeof(HashMapNode));
    file.seekp((int64_t) file.tellp() - sizeof(HashMapNode));
    file.seekg(file.tellp());
    --book_size_;
  }
  return std::make_pair(std::move(file), tree_node);
}

void Book::Put(const BookTreeNode& node) {
  Put(node, false, true);
}

void Book::Put(const BookTreeNode& node, bool overwrite, bool update_fathers) {
  auto triple = Find(node.Player(), node.Opponent());
  auto file = std::move(std::get<0>(triple));
  auto hash_map_node = std::move(std::get<1>(triple));
  auto stored_node = std::move(std::get<2>(triple));
  BookTreeNode node_to_store(node);

  if (stored_node) {
    GetValueFile(hash_map_node.Size()).Remove(hash_map_node.Offset());
    if (!overwrite) {
      node_to_store.Merge(*stored_node);
    }
  } else {
    assert(!overwrite);
    ++book_size_;
  }

  std::vector<char> to_store = node_to_store.Serialize();
  auto size = to_store.size();
  auto offset = GetValueFile(size).Add(to_store);
  HashMapNode to_be_stored(size, offset);
  if (file.tellg() < OffsetToFilePosition(hash_map_size_)) {
    file.write((char*) &to_be_stored, sizeof(HashMapNode));
    Resize(&file, {});
  } else {
    Resize(&file, {to_be_stored});
  }
  UpdateSizes(&file);
  if (update_fathers) {
    UpdateFathers(node_to_store);
  }
}

bool Book::IsSizeOK() {
  std::fstream index_file = OpenFile(IndexFilename());
  if (OffsetToFilePosition(hash_map_size_) != FileSize(index_file)) {
    std::cout << "Wrong hash map size " << hash_map_size_ << ". Should be " << FileSize(index_file) << "\n";
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

void Book::Print(int start, int end) {
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
              << node.Offset() << " -> " << GetBookTreeNode(node) << "\n";
  }
}

void Book::Clean() {
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

int Book::GetValueFileOffset(int size) {
  assert(size >= kMinFileSize);
  assert(size <= kMaxFileSize);
  return size - kMinFileSize;
}

ValueFile& Book::GetValueFile(int size) {
  ValueFile& file = value_files_[GetValueFileOffset(size)];
  assert(file.Size() == size);
  return file;
}

void Book::UpdateSizes(std::fstream* file) {
  file->seekp(0);
  file->write((char*) &hash_map_size_, sizeof(hash_map_size_));
  file->write((char*) &book_size_, sizeof(book_size_));
}

BookTreeNode Book::GetBookTreeNode(HashMapNode node) {
  std::vector<char> v = GetValueFile(node.Size()).Get(node.Offset());
  return BookTreeNode(v);
}

HashMapIndex Book::RepositionHash(HashMapIndex board_hash) {
  HashMapIndex hash = board_hash % PowerAfterHashMapSize();
  if (hash >= hash_map_size_) {
    return hash - PowerBeforeHashMapSize();
  }
  return hash;
}

std::tuple<std::fstream, HashMapNode, std::optional<BookTreeNode>>
    Book::Find(BitPattern player, BitPattern opponent) {
  Board unique = Board(player, opponent).Unique();
  player = unique.Player();
  opponent = unique.Opponent();
  auto hash = RepositionHash(HashFull(player, opponent));
  std::fstream file = OpenFile(IndexFilename());
  file.seekg(OffsetToFilePosition(hash));
  HashMapNode node;

  while (true) {
    file.read((char*) &node, sizeof(HashMapNode));
    if (node.IsEmpty()) {
      file.seekg((unsigned) file.tellg() - sizeof(HashMapNode), std::ios::beg);
      file.seekp(file.tellg());
      return std::make_tuple(std::move(file), node, std::nullopt);
    }
    auto book_tree_node = GetBookTreeNode(node);
    Board b = book_tree_node.ToBoard();
    if (b.Player() == player && b.Opponent() == opponent) {
      file.seekg((unsigned) file.tellg() - sizeof(HashMapNode), std::ios::beg);
      file.seekp(file.tellg());
      return std::make_tuple(std::move(file), node, book_tree_node);
    }

    if (file.tellg() == OffsetToFilePosition(hash_map_size_)) {
      file.seekp(file.tellg());
      return std::make_tuple(std::move(file), HashMapNode(), std::nullopt);
    }
  }
}

void Book::Resize(std::fstream* file, std::vector<HashMapNode> add_elements) {
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
    BookTreeNode node = GetBookTreeNode(node_to_move);
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

bool Book::IsEmpty(std::fstream& file, HashMapIndex hash) {
  auto old_position = file.tellg();
  file.seekg(OffsetToFilePosition(hash));
  HashMapNode node;
  file.read((char*) &node, sizeof(node));
  file.seekg(old_position);
  return node.IsEmpty();
}
