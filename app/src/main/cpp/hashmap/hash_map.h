/*
 * Copyright 2023 Michele Borassi
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

#include <array>
#include <atomic>
#include <memory>
#include <optional>
#include "../board/bitpattern.h"
#include "../constants.h"


#ifndef HASH_MAP_H
#define HASH_MAP_H

struct HashMapEntry {
  BitPattern player;
  BitPattern opponent;
  EvalLarge lower;
  EvalLarge upper;
  DepthValue depth;
  Square best_move;
  Square second_best_move;
};

struct HashMapEntryWithBusy {
  HashMapEntry entry;
  std::atomic_bool busy;
};

std::ostream& operator<<(std::ostream& stream, const HashMapEntry& e) {
  stream << "board: " << e.player << " " << e.opponent
         << "  eval: [" << (int) e.lower << " " << (int) e.upper << "] @ "
         << e.depth << "  best_moves: " << e.best_move << ", "
         << e.second_best_move;
  return stream;
}

template<int bits, int size = 1 << bits>
class HashMap {
 public:
  HashMap() : hash_map_(size) {
    Reset();
  }

  int Hash(BitPattern player, BitPattern opponent) {
    return ::Hash<bits>(player, opponent);
  }

  void Update(
      BitPattern player, BitPattern opponent, DepthValue depth,
      EvalLarge eval, EvalLarge lower, EvalLarge upper, Square best_move,
      Square second_best_move) {
    auto& entry_with_busy = hash_map_[Hash(player, opponent)];
    bool expected = false;
    if (!entry_with_busy.busy.compare_exchange_strong(expected, true)) {
      return;
    }
    auto& entry = entry_with_busy.entry;
    entry.player = player;
    entry.opponent = opponent;
    entry.lower = eval > lower ? eval : kMinEvalLarge;
    entry.upper = eval < upper ? eval : kMaxEvalLarge;
    entry.depth = depth;
    entry.best_move = best_move;
    entry.second_best_move = second_best_move;
    entry_with_busy.busy = false;
  }

  void Reset() {
    HashMapEntry empty;
    empty.player = 0;
    empty.opponent = 0;
    for (int i = 0; i < size; ++i) {
      hash_map_[i].entry = empty;
      hash_map_[i].busy = false;
    }
  }

  // NOTE: We cannot just return a reference because another thread might
  // invalidate it. We need to copy it.
  std::unique_ptr<HashMapEntry> Get(BitPattern player, BitPattern opponent) {
    auto& entry_with_busy = hash_map_[Hash(player, opponent)];
    bool expected = false;
    if (!entry_with_busy.busy.compare_exchange_strong(expected, true)) {
      return nullptr;
    }
    if (entry_with_busy.entry.player != player || entry_with_busy.entry.opponent != opponent) {
      entry_with_busy.busy = false;
      return nullptr;
    }
    HashMapEntry entry = entry_with_busy.entry;
    entry_with_busy.busy = false;
    return std::make_unique<HashMapEntry>(entry);
  }

  bool IsAllFree() {
    for (const HashMapEntryWithBusy& entry : hash_map_) {
      if (entry.busy) {
        return false;
      }
    }
    return true;
  }

 private:
  std::vector<HashMapEntryWithBusy> hash_map_;
};
#endif  // HASH_MAP_H