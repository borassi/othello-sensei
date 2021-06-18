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

#ifndef STABLE_H
#define STABLE_H

#include <immintrin.h>
#include "bitpattern.h"
#include "get_flip.h"

constexpr BitPattern kTopEdgePattern = ParsePattern(
          "XXXXXXXX"
          "--------"
          "--------"
          "--------"
          "--------"
          "--------"
          "--------"
          "--------");
constexpr BitPattern kLeftEdgePattern = ParsePattern(
          "X-------"
          "X-------"
          "X-------"
          "X-------"
          "X-------"
          "X-------"
          "X-------"
          "X-------");
constexpr BitPattern kRightEdgePattern = ParsePattern(
          "-------X"
          "-------X"
          "-------X"
          "-------X"
          "-------X"
          "-------X"
          "-------X"
          "-------X");
constexpr BitPattern kBottomEdgePattern = ParsePattern(
          "--------"
          "--------"
          "--------"
          "--------"
          "--------"
          "--------"
          "--------"
          "XXXXXXXX");
constexpr BitPattern kAllMinusFirstColumnPattern = ~kLeftEdgePattern;
constexpr BitPattern kAllMinusFirst2ColumnsPattern =
        ~(kLeftEdgePattern | (kLeftEdgePattern >> 1));
constexpr BitPattern kAllMinusFirst4ColumnsPattern =
        ~(kLeftEdgePattern | (kLeftEdgePattern >> 1) | (kLeftEdgePattern >> 2) | (kLeftEdgePattern >> 3));
constexpr BitPattern kAllMinusLastColumnPattern = ~kRightEdgePattern;
constexpr BitPattern kAllMinusLast2ColumnsPattern =
        ~(kRightEdgePattern | (kRightEdgePattern << 1));
constexpr BitPattern kAllMinusLast4ColumnsPattern = ~kAllMinusFirst4ColumnsPattern;
constexpr BitPattern kEdgesPattern = kTopEdgePattern | kBottomEdgePattern | kLeftEdgePattern | kRightEdgePattern;
constexpr BitPattern kNonEdgePattern = ~kEdgesPattern;

struct StableDisksEdge {
  uint8_t arr[65536];

  constexpr static int Hash(BitPattern player, BitPattern opponent) {
    return player | (opponent << 8);
  }
  
  constexpr uint8_t GetFlipOneDirection(Move x, uint8_t player, uint8_t opponent) {
    return kFlip[(x << 8) | kOutflank[(x << 8) | opponent] & player];
  }

  constexpr void FillStableDisks(BitPattern player, BitPattern opponent) {
    BitPattern empties = ~(player | opponent);
    BitPattern stable = ~empties;

    if (empties == 0) {
      arr[Hash(player, opponent)] = stable;
    }

    for (int x = 0; x < 8; ++x) {
      Move move = 1 << x;
      if ((empties & move) != 0) {
        uint8_t flip = GetFlipOneDirection(x, player, opponent) | move;
        stable = stable & ~flip & arr[Hash(opponent & ~flip, player | flip)];
        flip = GetFlipBasic(x, opponent, player) | move;
        stable = stable & ~flip & arr[Hash(player & ~flip, opponent | flip)];
      }
    }
    arr[Hash(player, opponent)] = stable;
  }

  constexpr StableDisksEdge():arr() {
    for (int empties = 0; empties < 256; empties++) {
      for (int player = 0; player < 256; player++) {
        FillStableDisks(player, 255 & ~(empties | player));
      }
    }
  }
};

constexpr StableDisksEdge kStableDisksEdge;

BitPattern GetStableDisksEdges(BitPattern player, BitPattern opponent);
BitPattern GetFullDiags7(BitPattern empty);
BitPattern GetFullDiags9(BitPattern empty);
BitPattern GetFullColumns(BitPattern empty);
BitPattern GetFullRows(BitPattern empty);
BitPattern GetStableDisks(BitPattern player, BitPattern opponent, BitPattern stable = 0);
Eval GetLowerBound(BitPattern player, BitPattern opponent);
Eval GetUpperBoundFromStable(long stable, long opponent);
Eval GetStableDisksUpperBound(BitPattern player, BitPattern opponent);


#endif /* STABLE_H */

