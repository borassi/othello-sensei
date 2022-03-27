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

#include <vector>

#include "bitpattern.h"
#include "board.h"
#include "get_flip.h"
#include "get_moves.h"

namespace {
Square IJToSquare(int i, int j) {
  return 8 * i + j;
}
} // anonymous namespace

BitPattern GetMovesBasic(BitPattern player, BitPattern opponent) {
  BitPattern result = 0;

  for (Square x = 0; x < 64; x++) {
    if ((1ULL << x) & (player | opponent)) {
      continue;
    }
    if (GetFlipBasic(x, player, opponent)) {
      result |= 1ULL << x;
    }
  }
  return result;
}

std::vector<BitPattern> GetAllMoves(BitPattern player, BitPattern opponent) {
  std::vector<BitPattern> result;

  for (Square x = 0; x < 64; x++) {
    if ((1ULL << x) & (player | opponent)) {
      continue;
    }
    BitPattern flip = GetFlipBasic(x, player, opponent);
    if (flip) {
      result.push_back(flip);
    }
  }
  return result;
}

std::vector<BitPattern> GetAllMovesWithPass(BitPattern player, BitPattern opponent) {
  std::vector<BitPattern> result = GetAllMoves(player, opponent);
  if (!result.empty()) {
    return result;
  }
  if (!GetAllMoves(opponent, player).empty()) {
    // Game over.
    return {};
  }
  return {0};
}

bool HaveToPass(BitPattern player, BitPattern opponent) {
  return GetMoves(player, opponent) == 0;
}