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

#include <array>
#include <memory>
#include <mutex>
#include "../board/bitpattern.h"
#include "../constants.h"


#ifndef HASH_MAP_H
#define HASH_MAP_H

class HashMapEntry {
 public:
  HashMapEntry() :
    player_(0),
    opponent_(0),
    lower_(kMinEvalLarge),
    upper_(kMaxEvalLarge),
    depth_(0),
    best_move_(kNoSquare),
    second_best_move_(kNoSquare) {}

  HashMapEntry(const HashMapEntry& e) :
    player_(e.player_),
    opponent_(e.opponent_),
    lower_(e.lower_),
    upper_(e.upper_),
    depth_(e.depth_),
    best_move_(e.best_move_),
    second_best_move_(e.second_best_move_) {}

  void Update(
    BitPattern player, BitPattern opponent, DepthValue depth, EvalLarge eval,
    EvalLarge lower, EvalLarge upper, Square best_move, Square second_best_move);

  std::pair<EvalLarge, EvalLarge> GetLowerUpper(
    BitPattern player, BitPattern opponent, DepthValue min_depth) const;

  BitPattern Player() const {
    return player_;
  }
  BitPattern Opponent() const {
    return opponent_;
  }
  EvalLarge Lower() const {
    return lower_;
  }
  EvalLarge Upper() const {
    return upper_;
  }
  DepthValue Depth() const {
    return depth_;
  }
  Square BestMove() const {
    return best_move_;
  }
  Square SecondBestMove() const {
    return second_best_move_;
  }

 private:
  BitPattern player_;
  BitPattern opponent_;
  EvalLarge lower_;
  EvalLarge upper_;
  DepthValue depth_;
  Square best_move_;
  Square second_best_move_;
};

class HashMap {
 public:
  ~HashMap() {
    delete[] hash_map_;
  }

  void Update(
    BitPattern player, BitPattern opponent, DepthValue depth,
    EvalLarge eval, EvalLarge lower, EvalLarge upper, Square best_move,
    Square second_best_move) {
    {
      const std::lock_guard<std::mutex> guard(mutex_);
      hash_map_[Hash(player, opponent)].Update(
          player, opponent, depth, eval, lower, upper, best_move,
          second_best_move);
    }
  }

  void Reset() {
    for (int i = 0; i < kHashMapSize; ++i) {
      hash_map_[i].Update(0, 0, 0, 0, kMinEvalLarge, kMaxEvalLarge, kNoSquare, kNoSquare);
    }
  }

  std::unique_ptr<HashMapEntry> Get(BitPattern player, BitPattern opponent) const {
    {
      const std::lock_guard<std::mutex> guard(mutex_);
      HashMapEntry result = hash_map_[Hash(player, opponent)];
      if (result.Player() == player && result.Opponent() == opponent) {
        return std::make_unique<HashMapEntry>(result);
      }
    }
    return nullptr;
  }
 private:
  HashMapEntry* hash_map_ = new HashMapEntry[kHashMapSize]();
  mutable std::mutex mutex_;
};
#endif  // HASH_MAP_H