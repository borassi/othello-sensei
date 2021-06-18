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

#include "bitpattern.h"
#include "stable.h"

BitPattern GetStableDisksEdges(BitPattern player, BitPattern opponent) {
  BitPattern stable = kStableDisksEdge.arr[(player & kBottomEdgePattern) | ((opponent & kBottomEdgePattern) << 8)];
  stable |= _pdep_u64(kStableDisksEdge.arr[_pext_u64(player, kTopEdgePattern) | (_pext_u64(opponent, kTopEdgePattern) << 8)], kTopEdgePattern);
  stable |= _pdep_u64(kStableDisksEdge.arr[_pext_u64(player, kRightEdgePattern) | (_pext_u64(opponent, kRightEdgePattern) << 8)], kRightEdgePattern);
  stable |= _pdep_u64(kStableDisksEdge.arr[_pext_u64(player, kLeftEdgePattern) | (_pext_u64(opponent, kLeftEdgePattern) << 8)], kLeftEdgePattern);

  return stable;
}

BitPattern GetFullDiags7(BitPattern empty) {
  BitPattern emptyL = empty | ((empty >> 7) & kAllMinusLastColumnPattern);
  emptyL = emptyL | ((emptyL >> 14) & kAllMinusLast2ColumnsPattern);
  emptyL = emptyL | ((emptyL >> 28) & kAllMinusLast4ColumnsPattern);

  BitPattern emptyR = empty | ((empty << 7) & kAllMinusFirstColumnPattern);
  emptyR = emptyR | ((emptyR << 14) & kAllMinusFirst2ColumnsPattern);
  emptyR = emptyR | ((emptyR << 28) & kAllMinusFirst4ColumnsPattern);
    
  return ~(emptyL | emptyR);
}

BitPattern GetFullDiags9(BitPattern empty) {
  BitPattern emptyL = empty | ((empty << 9) & kAllMinusLastColumnPattern);
  emptyL = emptyL | ((emptyL << 18) & kAllMinusLast2ColumnsPattern);
  emptyL = emptyL | ((emptyL << 36) & kAllMinusLast4ColumnsPattern);

  BitPattern emptyR = empty | ((empty >> 9) & kAllMinusFirstColumnPattern);
  emptyR = emptyR | ((emptyR >> 18) & kAllMinusFirst2ColumnsPattern);
  emptyR = emptyR | ((emptyR >> 36) & kAllMinusFirst4ColumnsPattern);
    
  return ~(emptyL | emptyR);
}

BitPattern GetFullColumns(BitPattern empty) {
  BitPattern emptyL = empty | (empty >> 8);
  emptyL = emptyL | (emptyL >> 16);
  emptyL = emptyL | (emptyL >> 32);

  BitPattern emptyR = empty | (empty << 8);
  emptyR = emptyR | (emptyR << 16);
  emptyR = emptyR | (emptyR << 32);

  return ~(emptyL | emptyR);
}

BitPattern GetFullRows(BitPattern empty) {
  BitPattern emptyL = empty | ((empty << 1) & kAllMinusLastColumnPattern);
  emptyL = emptyL | ((emptyL << 2) & kAllMinusLast2ColumnsPattern);
  emptyL = emptyL | ((emptyL << 4) & kAllMinusLast4ColumnsPattern);

  BitPattern emptyR = empty | ((empty >> 1) & kAllMinusFirstColumnPattern);
  emptyR = emptyR | ((emptyR >> 2) & kAllMinusFirst2ColumnsPattern);
  emptyR = emptyR | ((emptyR >> 4) & kAllMinusFirst4ColumnsPattern);
    
  return ~(emptyL | emptyR);
}

BitPattern GetStableDisks(BitPattern player, BitPattern opponent, BitPattern stable) {
  stable |= GetStableDisksEdges(player, opponent);
  long empties = ~(player | opponent);
  long full_rows = GetFullRows(empties);
  long full_columns = GetFullColumns(empties);
  long full_diags9 = GetFullDiags9(empties);
  long full_diags7 = GetFullDiags7(empties);
  stable = stable | (full_rows & full_columns & full_diags9 & full_diags7);
  long stablePlayer = stable & player;
  long newStable = stablePlayer;
  while (newStable != 0) {
    newStable = ((stablePlayer << 1) | (stablePlayer >> 1) | full_rows);
    newStable &= ((stablePlayer << 8 | (stablePlayer >> 8) | full_columns));
    newStable &= ((stablePlayer << 7 | (stablePlayer >> 7) | full_diags7));
    newStable &= ((stablePlayer << 9 | (stablePlayer >> 9) | full_diags9));
    newStable = newStable & player & kNonEdgePattern & ~stablePlayer;
    stablePlayer |= newStable;
  }

  return stable | stablePlayer;
}
  
Eval GetLowerBound(BitPattern player, BitPattern opponent) {
  return 2 * __builtin_popcountll(GetStableDisks(player, opponent) & player) - 64;
}
  
Eval GetUpperBoundFromStable(long stable, long opponent) {
  return 64 - 2 * __builtin_popcountll(stable & opponent);
}
  
Eval GetStableDisksUpperBound(BitPattern player, BitPattern opponent) {
  return GetUpperBoundFromStable(GetStableDisks(opponent, player), opponent);
}