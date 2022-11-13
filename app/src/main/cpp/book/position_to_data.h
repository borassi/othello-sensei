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

#include <fstream>
#include <optional>
#include <string>

#include "value_file.h"
#include "../board/bitpattern.h"
#include "../board/board.h"
#include "../utils/file.h"

constexpr int kMinFileSize = kMinValueFileSize;
constexpr int kMaxFileSize = 100;

class Position {
 public:
  Position(BitPattern player, BitPattern opponent, FileOffset offset, u_int8_t size) :
      board_(SerializedBoard(player, opponent)), offset_(offset), size_(size), next_collision_position_(255) {}
  Position() : Position(0, 0, 0, 0) {}

  SerializedBoard Board() { return board_; }
  BitPattern Offset() { return offset_; }
  BitPattern Size() { return size_; }
  u_int8_t NextCollisionPosition() { return next_collision_position_; }

  u_int8_t CommonPart(SerializedBoard b) {
    if (board_ > b) {
      return 0;
    }
    return std::min(next_collision_position_, board_.FirstDifference(b));
  }

 private:
  SerializedBoard board_;
  u_int8_t next_collision_position_;
  // Offset of the next lexicographic position.
  FileOffset next_position_offset_;
  // Offset of the next position that differs from this in the "next" bit.
  FileOffset next_position_skip_offset_;
  FileOffset offset_;
  u_int8_t size_;
};

class PositionToData {

 public:
  PositionToData(std::string folder) : folder_(folder), value_files_() {
    for (int i = kMinFileSize; i <= kMaxFileSize; ++i) {
      value_files_.push_back(ValueFile(folder, i));
    }
  }

  std::vector<char> Get(BitPattern player, BitPattern opponent) {
    SerializedBoard board(player, opponent);

    std::fstream file = OpenFile(IndexFilename());
    Position position = LastSmallerPosition(board, &file);
    if (board != position.Board()) {
      return std::vector<char>();
    }
    return GetValueFile(position.Size()).Get(position.Offset());
  }

  void Put(BitPattern player, BitPattern opponent, std::vector<char> value) {
    assert(value.size() >= 4 && value.size() < 100);
    auto offset = GetValueFile(value.size()).Add(value);

    std::fstream file = OpenFile(IndexFilename());
    Position new_position(player, opponent, offset, value.size());
    Position old_position = LastSmallerPosition(new_position.Board(), &file);

    if (old_position.Board() != new_position.Board()) {
      file.seekp(0, std::ios::end);
    }
    file.write((char*) &new_position, sizeof(new_position));
    file.close();
  }

  void Print() {
    std::fstream index_file = OpenFile(IndexFilename());
    int size = FileSize(index_file);
    Position position;

    std::cout << "Book with " << size / sizeof(Position) << " elements:\n";
    while (index_file.tellg() != size) {
      index_file.read((char*) &position, sizeof(Position));
      Board b = position.Board().ToBoard();
      std::cout << "  " << b.Player() << " " << b.Opponent() << ":";
      for (char c : GetValueFile(position.Size()).Get(position.Offset())) {
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
  }

 private:
  std::string folder_;
  std::vector<ValueFile> value_files_;

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

  std::string IndexFilename() { return folder_ + "/index.sen"; }

  Position LastSmallerPosition(SerializedBoard b, std::fstream* file) {
    int size = FileSize(*file);
    FileOffset best_position_index = ~0;
    Position best_position;
    Position position;

    while (file->tellg() != size) {
      file->read((char*) &position, sizeof(Position));
      if (position.Board() == b || (position.CommonPart(b) > best_position.CommonPart(b))) {
        best_position = position;
        best_position_index = (unsigned long long) file->tellg() - sizeof(Position);
      }
    }
    file->seekg(best_position_index, std::ios_base::beg);
    file->seekp(best_position_index, std::ios_base::beg);
    return best_position;
  }
};



#endif //OTHELLOSENSEI_POSITION_TO_DATA_H
