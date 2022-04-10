/*
 * Copyright 2021 Google LLC
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
#include <cassert>

#if PDEP_PEXT
#include <immintrin.h>
#endif

#include <iostream>
#include <regex>
#include <string>
#include "bitpattern.h"

Eval GetEvaluationGameOver(BitPattern player, BitPattern opponent) {
  int player_disks = __builtin_popcountll(player);
  int opponent_disks = __builtin_popcountll(opponent);
  int diff = player_disks - opponent_disks;
  return static_cast<Eval>(diff + (int) ((diff > 0) - (diff < 0)) * (64 - player_disks - opponent_disks));
}

std::string PatternToString(BitPattern pattern) {
  std::string result;
  for (int i = 63; i >= 0; --i) {
    BitPattern mask = (1L << i);
    if ((pattern & mask) != 0) {
      result += 'X';
    } else {
      result += '-';
    }
    if (i % 8 == 0) {
      result += '\n';
    }
  }
  return result;
}

BitPattern RandomPattern(double percentage) {
  BitPattern result = 0;
  for (int j = 0; j < 64; j++) {
    if ((double) rand() / RAND_MAX < percentage) {
      result |= 1;
    }
    result = result << 1;
  }
  return result;
}

BitPattern RandomPattern() {
  return RandomPattern((double) rand() / RAND_MAX);
}

std::vector<BitPattern> AllSubBitPatterns(BitPattern p) {
  if (p == 0) {
    return std::vector<BitPattern>({0});
  }

  int last_full_square = __builtin_ctzll(p);
  BitPattern last_full_square_pattern = 1ULL << last_full_square;
  std::vector<BitPattern> patterns = AllSubBitPatterns(
      p & ~last_full_square_pattern);
  auto n_patterns_no_last_square = patterns.size();
  for (int i = 0; i < n_patterns_no_last_square; ++i) {
    patterns.push_back(last_full_square_pattern | patterns[i]);
  }
  return patterns;
}