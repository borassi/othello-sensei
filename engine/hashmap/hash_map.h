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
#include <cstring>
#include <memory>
#include <optional>
#include "../board/bitpattern.h"
#include "../utils/constants.h"


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

// We don't use a member or inheritance to save memory.
struct HashMapEntryInternal {
  BitPattern player;
  BitPattern opponent;
  EvalLarge lower;
  EvalLarge upper;
  DepthValue depth;
  Square best_move;
  Square second_best_move;
  std::atomic_bool busy;
};

std::ostream& operator<<(std::ostream& stream, const HashMapEntry& e);

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
    auto& entry = hash_map_[Hash(player, opponent)];
    bool expected = false;
    if (!entry.busy.compare_exchange_strong(expected, true)) {
      return;
    }
    entry.player = player;
    entry.opponent = opponent;
    entry.lower = eval > lower ? eval : kMinEvalLarge;
    entry.upper = eval < upper ? eval : kMaxEvalLarge;
    entry.depth = depth;
    entry.best_move = best_move;
    entry.second_best_move = second_best_move;
    entry.busy = false;
  }

  void Reset() {
    for (HashMapEntryInternal& entry : hash_map_) {
      entry.player = 0;
      entry.opponent = 0;
      entry.busy = false;
    }
  }

  // NOTE: We cannot just return a reference because another thread might
  // invalidate it. We need to copy it.
  std::unique_ptr<HashMapEntry> Get(BitPattern player, BitPattern opponent) {
    auto& entry_internal = hash_map_[Hash(player, opponent)];
    bool expected = false;
    if (!entry_internal.busy.compare_exchange_strong(expected, true)) {
      return nullptr;
    }
    if (entry_internal.player != player || entry_internal.opponent != opponent) {
      entry_internal.busy = false;
      return nullptr;
    }
    std::unique_ptr<HashMapEntry> entry(new HashMapEntry());
    memcpy(entry.get(), &entry_internal, sizeof(HashMapEntry));
    entry_internal.busy = false;
    return entry;
  }

  bool IsAllFree() {
    for (const HashMapEntryInternal& entry : hash_map_) {
      if (entry.busy) {
        return false;
      }
    }
    return true;
  }

 private:
  std::vector<HashMapEntryInternal> hash_map_;
};
#endif  // HASH_MAP_H