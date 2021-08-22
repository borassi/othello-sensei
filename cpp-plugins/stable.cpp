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

#include "constants.h"

#if PDEP_PEXT
#include <immintrin.h>
#endif

#include "bitpattern.h"
#include "stable.h"


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

