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

  bool operator==(const HashMapEntry& other) const = default;
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
    HashMapEntry entry {
      .player = player,
      .opponent = opponent,
      .lower = eval > lower ? eval : kMinEvalLarge,
      .upper = eval < upper ? eval : kMaxEvalLarge,
      .depth = depth,
      .best_move = best_move,
      .second_best_move = second_best_move
    };
    hash_map_[Hash(player, opponent)].store(entry, std::memory_order_relaxed);
  }

  void Reset() {
    HashMapEntry empty { .player = 0, .opponent = 0 };
    for (int i = 0; i < size; ++i) {
      std::atomic_init(&hash_map_[i], empty);
    }
  }

  // NOTE: We cannot just return a reference because another thread might
  // invalidate it. We need to copy it.
  std::optional<HashMapEntry> Get(BitPattern player, BitPattern opponent) {
    HashMapEntry result = hash_map_[Hash(player, opponent)].load(std::memory_order_relaxed);
    if (result.player == player && result.opponent == opponent) {
      return result;
    }
    return std::nullopt;
  }

 private:
  std::vector<std::atomic<HashMapEntry>> hash_map_;
};
#endif  // HASH_MAP_H