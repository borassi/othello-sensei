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

#ifndef STABLE_H
#define STABLE_H

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
  LastRow arr[65536];

  constexpr static int Hash(BitPattern player, BitPattern opponent) {
    return (int) (player | (opponent << 8));
  }
  
  static constexpr LastRow GetFlipOneDirection(Square x, LastRow player, LastRow opponent) {
    return kFlip[(x << 8) | (kOutflank[(x << 8) | opponent] & player)];
  }

  constexpr void FillStableDisks(BitPattern player, BitPattern opponent) {
    BitPattern empties = ~(player | opponent);
    BitPattern stable = ~empties;

    if (empties == 0) {
      arr[Hash(player, opponent)] = (LastRow) stable;
    }

    for (int x = 0; x < 8; ++x) {
      Square move = 1 << x;
      if ((empties & move) != 0) {
        auto flip = (LastRow) (GetFlipOneDirection(x, (LastRow) player, (LastRow) opponent) | move);
        stable = stable & ~flip & arr[Hash(opponent & ~flip, player | flip)];
        flip = (LastRow) (GetFlipBasic(x, opponent, player) | move);
        stable = stable & ~flip & arr[Hash(player & ~flip, opponent | flip)];
      }
    }
    arr[Hash(player, opponent)] = (LastRow) stable;
  }

  constexpr StableDisksEdge() : arr() {
    for (int empties = 0; empties < 256; empties++) {
      for (int player = 0; player < 256; player++) {
        FillStableDisks(player, 255 & ~(empties | player));
      }
    }
  }
};

const StableDisksEdge kStableDisksEdge;

forceinline(BitPattern GetStableDisksEdges(BitPattern player, BitPattern opponent));

BitPattern GetStableDisksEdges(BitPattern player, BitPattern opponent) {
  BitPattern stable = kStableDisksEdge.arr[(player & kBottomEdgePattern) | ((opponent & kBottomEdgePattern) << 8)];
#if __BMI2__
  stable |= _pdep_u64(kStableDisksEdge.arr[_pext_u64(player, kTopEdgePattern) | (_pext_u64(opponent, kTopEdgePattern) << 8)], kTopEdgePattern);
  stable |= _pdep_u64(kStableDisksEdge.arr[_pext_u64(player, kRightEdgePattern) | (_pext_u64(opponent, kRightEdgePattern) << 8)], kRightEdgePattern);
  stable |= _pdep_u64(kStableDisksEdge.arr[_pext_u64(player, kLeftEdgePattern) | (_pext_u64(opponent, kLeftEdgePattern) << 8)], kLeftEdgePattern);
#else
  stable |= LastRowToRow(kStableDisksEdge.arr[RowToLastRow(player, kTopEdgePattern, 56) | (RowToLastRow(opponent, kTopEdgePattern, 56) << 8)], 56);
  stable |= LastRowToColumn(kStableDisksEdge.arr[ColumnToLastRow(player, kRightEdgePattern, 0) | (ColumnToLastRow(opponent, kRightEdgePattern, 0) << 8)], 0);
  stable |= LastRowToColumn(kStableDisksEdge.arr[ColumnToLastRow(player, kLeftEdgePattern, 7) | (ColumnToLastRow(opponent, kLeftEdgePattern, 7) << 8)], 7);
#endif
  return stable;
}

forceinline(BitPattern GetFullDiags7(BitPattern empty));
inline BitPattern GetFullDiags7(BitPattern empty) {
  BitPattern emptyL = empty | ((empty >> 7) & kAllMinusLastColumnPattern);
  emptyL = emptyL | ((emptyL >> 14) & kAllMinusLast2ColumnsPattern);
  emptyL = emptyL | ((emptyL >> 28) & kAllMinusLast4ColumnsPattern);

  BitPattern emptyR = empty | ((empty << 7) & kAllMinusFirstColumnPattern);
  emptyR = emptyR | ((emptyR << 14) & kAllMinusFirst2ColumnsPattern);
  emptyR = emptyR | ((emptyR << 28) & kAllMinusFirst4ColumnsPattern);
    
  return ~(emptyL | emptyR);
}

forceinline(BitPattern GetFullDiags9(BitPattern empty));
inline BitPattern GetFullDiags9(BitPattern empty) {
  BitPattern emptyL = empty | ((empty << 9) & kAllMinusLastColumnPattern);
  emptyL = emptyL | ((emptyL << 18) & kAllMinusLast2ColumnsPattern);
  emptyL = emptyL | ((emptyL << 36) & kAllMinusLast4ColumnsPattern);

  BitPattern emptyR = empty | ((empty >> 9) & kAllMinusFirstColumnPattern);
  emptyR = emptyR | ((emptyR >> 18) & kAllMinusFirst2ColumnsPattern);
  emptyR = emptyR | ((emptyR >> 36) & kAllMinusFirst4ColumnsPattern);
    
  return ~(emptyL | emptyR);
}

forceinline(BitPattern GetFullColumns(BitPattern empty));
inline BitPattern GetFullColumns(BitPattern empty) {
  BitPattern emptyL = empty | (empty >> 8);
  emptyL = emptyL | (emptyL >> 16);
  emptyL = emptyL | (emptyL >> 32);

  BitPattern emptyR = empty | (empty << 8);
  emptyR = emptyR | (emptyR << 16);
  emptyR = emptyR | (emptyR << 32);

  return ~(emptyL | emptyR);
}

forceinline(BitPattern GetFullRows(BitPattern empty));
inline BitPattern GetFullRows(BitPattern empty) {
  BitPattern emptyL = empty | ((empty << 1) & kAllMinusLastColumnPattern);
  emptyL = emptyL | ((emptyL << 2) & kAllMinusLast2ColumnsPattern);
  emptyL = emptyL | ((emptyL << 4) & kAllMinusLast4ColumnsPattern);

  BitPattern emptyR = empty | ((empty >> 1) & kAllMinusFirstColumnPattern);
  emptyR = emptyR | ((emptyR >> 2) & kAllMinusFirst2ColumnsPattern);
  emptyR = emptyR | ((emptyR >> 4) & kAllMinusFirst4ColumnsPattern);
    
  return ~(emptyL | emptyR);
}

forceinline(BitPattern GetStableDisks(BitPattern player, BitPattern opponent, BitPattern stable = 0));
inline BitPattern GetStableDisks(BitPattern player, BitPattern opponent, BitPattern stable) {
  stable |= GetStableDisksEdges(player, opponent);
  BitPattern empties = ~(player | opponent);
  BitPattern full_rows = GetFullRows(empties);
  BitPattern full_columns = GetFullColumns(empties);
  BitPattern full_diags9 = GetFullDiags9(empties);
  BitPattern full_diags7 = GetFullDiags7(empties);
  stable = stable | (full_rows & full_columns & full_diags9 & full_diags7);
  BitPattern stablePlayer = stable & player;
  BitPattern newStable = stablePlayer;

  while (newStable != 0) {
    newStable = ((stablePlayer << 1) | (stablePlayer >> 1) | full_rows);
    newStable &= ((stablePlayer << 8) | (stablePlayer >> 8) | full_columns);
    newStable &= ((stablePlayer << 7) | (stablePlayer >> 7) | full_diags7);
    newStable &= ((stablePlayer << 9) | (stablePlayer >> 9) | full_diags9);
    newStable = newStable & player & kNonEdgePattern & ~stablePlayer;
    stablePlayer |= newStable;
  }

  return stable | stablePlayer;
}

forceinline(int GetUpperBoundFromStable(BitPattern stable, BitPattern opponent));

inline int GetUpperBoundFromStable(BitPattern stable, BitPattern opponent) {
  return (64 - 2 * (int) __builtin_popcountll(stable & opponent));
}

inline int GetStableDisksUpperBound(BitPattern player, BitPattern opponent) {
  return GetUpperBoundFromStable(GetStableDisks(opponent, player), opponent);
}

#endif /* STABLE_H */

