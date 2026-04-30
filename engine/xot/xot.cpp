/*
 * Copyright 2025 Michele Borassi
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

#include <algorithm>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "xot.h"
#include "../board/sequence.h"
#include "../utils/files.h"


std::vector<Sequence> Xot::FindStart(const Sequence& suffix, int max) const {
  BitPattern full = 0;
  std::vector<Sequence> result;
  for (int i = 0; i < suffix.Size(); ++i) {
    Square move = suffix.Moves()[i];
    assert(move >= 0 && move < 64);
    full |= 1ULL << suffix.Moves()[i];
  }
  for (const auto& sequence_with_metadata : sequences_) {
    const auto& sequence_transpositions = sequence_with_metadata.sequence_transpositions;
    const auto& board_transpositions = sequence_with_metadata.board_transpositions;
    for (int i = 0; i < board_transpositions.size(); ++i) {
      auto& start = sequence_transpositions[i];
      Square first_move = start.Moves()[0];
      if (first_move != 19 && first_move != 26 && first_move != 37 && first_move != 44) {
        continue;
      }
      auto& board = board_transpositions[i];
      if ((full & (board.Player() | board.Opponent())) != 0) {
        continue;
      }
      Sequence full = start;
      if (!full.Extend(suffix)) {
        continue;
      }
      try {
        full.ToBoard();
      } catch (InvalidSequenceException e) {
        continue;
      }
      result.push_back(start);
      if (result.size() >= max) {
        return result;
      }
    }
  }
  return result;
}

XotHandler::XotHandler(const std::string& folder) {
  std::vector<std::string> files = GetAllFiles(folder, true, false);
  std::sort(files.begin(), files.end());

  std::unordered_map<std::string, int> name_counts;

  // Prevent reallocation to avoid invalidating internal pointers in Xot.
  xots_.reserve(files.size());

  for (const std::string& filepath : files) {
    std::string raw_name = Filename(filepath);

    // Remove file extension
    size_t dot_pos = raw_name.find_last_of('.');
    if (dot_pos != std::string::npos) {
      raw_name = raw_name.substr(0, dot_pos);
    }

    // Remove prefix "XXX - "
    std::string parsed_name = raw_name;
    size_t dash_pos = raw_name.find(" - ");
    if (dash_pos != std::string::npos) {
      parsed_name = raw_name.substr(dash_pos + 3);
    }

    // Handle duplicates
    name_counts[parsed_name]++;
    if (name_counts[parsed_name] > 1) {
      parsed_name += " (" + std::to_string(name_counts[parsed_name]) + ")";
    }

    xots_.push_back(std::make_unique<Xot>(LoadTextFile(filepath), parsed_name));
  }
}

int XotHandler::GetPrefixLength(const Sequence& sequence) const {
  for (const auto& xot : xots_) {
    if (xot->IsInListPrefix(sequence)) {
      return xot->SequenceSize();
    }
  }
  return -1;
}

bool XotHandler::IsInList(const Sequence& sequence) const {
  for (const auto& xot : xots_) {
    if (xot->IsInList(sequence)) {
      return true;
    }
  }
  return false;
}

Sequence XotHandler::RandomSequence(const std::string& filename) const {
  for (const auto& xot : xots_) {
    if (xot->Name() == filename) {
      return xot->RandomSequence();
    }
  }
  throw std::invalid_argument("Xot name not found: " + filename);
}

std::vector<std::string> XotHandler::SourceNames() const {
  std::vector<std::string> names;
  names.reserve(xots_.size());
  for (const auto& xot : xots_) {
    names.push_back(xot->Name());
  }
  return names;
}

int XotHandler::FirstFileSequenceSize() const {
  if (xots_.empty()) {
    return -1;
  }
  return xots_.front()->SequenceSize();
}