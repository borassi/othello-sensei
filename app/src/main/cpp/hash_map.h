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
#include "bitpattern.h"
#include "constants.h"


#ifndef HASH_MAP_H
#define HASH_MAP_H

typedef u_int8_t Epoch;

class HashMapEntry {
 public:
  HashMapEntry() :
    epoch_(0),
    player_(0),
    opponent_(0),
    lower_(kMinEval),
    upper_(kMaxEval),
    depth_(0),
    best_move_(kNoMove),
    second_best_move_(kNoMove) {}

  void Update(
    BitPattern player, BitPattern opponent, Epoch epoch, DepthValue depth, Eval eval,
    Eval lower, Eval upper, Move best_move, Move second_best_move);

  std::pair<Eval, Eval> GetLowerUpper(
    BitPattern player, BitPattern opponent, DepthValue min_depth) const;

  BitPattern Player() const {
    const std::lock_guard<std::mutex> lock(mutex_);
    return player_;
  }
  BitPattern Opponent() const {
    const std::lock_guard<std::mutex> lock(mutex_);
    return opponent_;
  }
  Eval Lower() const {
    const std::lock_guard<std::mutex> lock(mutex_);
    return lower_;
  }
  Eval Upper() const {
    const std::lock_guard<std::mutex> lock(mutex_);
    return upper_;
  }
  DepthValue Depth() const {
    const std::lock_guard<std::mutex> lock(mutex_);
    return depth_;
  }
  Move BestMove() const {
    const std::lock_guard<std::mutex> lock(mutex_);
    return best_move_;
  }
  Move SecondBestMove() const {
    const std::lock_guard<std::mutex> lock(mutex_);
    return second_best_move_;
  }

 private:
  BitPattern player_;
  BitPattern opponent_;
  Epoch epoch_;
  Eval lower_;
  Eval upper_;
  DepthValue depth_;
  Move best_move_;
  Move second_best_move_;
  mutable std::mutex mutex_;
};

class HashMap {
 public:
  ~HashMap() {
    delete[] hash_map_;
  }

  void Update(
      BitPattern player, BitPattern opponent, Epoch epoch, DepthValue depth,
      Eval eval, Eval lower, Eval upper, Move best_move,
      Move second_best_move) {
    hash_map_[Hash(player, opponent)].Update(
      player, opponent, epoch, depth, eval, lower, upper, best_move,
      second_best_move);
  }

  std::pair<Eval, Eval> GetLowerUpper (
      BitPattern player, BitPattern opponent, DepthValue depth) const {
    return hash_map_[Hash(player, opponent)].GetLowerUpper(player, opponent, depth);
  }
 private:
  HashMapEntry* hash_map_ = new HashMapEntry[kHashMapSize]();
};
#endif  // HASH_MAP_H