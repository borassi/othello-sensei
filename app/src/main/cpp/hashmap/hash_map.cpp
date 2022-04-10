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

#include "hash_map.h"

void HashMapEntry::Update(
  BitPattern player, BitPattern opponent, EpochValue epoch, DepthValue depth, EvalLarge eval,
  EvalLarge lower, EvalLarge upper, Square best_move, Square second_best_move) {
  const std::lock_guard<std::mutex> lock(mutex_);
  if (epoch != epoch_ || depth > depth_) {
    player_ = player;
    opponent_ = opponent;
    epoch_ = epoch;
    lower_ = eval > lower ? eval : kMinEvalLarge;
    upper_ = eval < upper ? eval : kMaxEvalLarge;
    depth_ = depth;
    best_move_ = best_move;
    second_best_move_ = second_best_move;
  }
}

std::pair<EvalLarge, EvalLarge> HashMapEntry::GetLowerUpper(
    BitPattern player, BitPattern opponent, DepthValue min_depth) const {
  const std::lock_guard<std::mutex> lock(mutex_);
  if (player != player_ || opponent != opponent_ || depth_ < min_depth) {
    return std::make_pair(kMinEvalLarge, kMaxEvalLarge);
  }
  return std::make_pair(lower_, upper_);
}