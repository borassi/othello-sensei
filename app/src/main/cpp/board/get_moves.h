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

#ifndef BOARD_GET_MOVES_H
#define BOARD_GET_MOVES_H

#include "get_flip.h"

constexpr BitPattern kNonVerticalFlipMask = ParsePattern(
  "-XXXXXX-"
  "-XXXXXX-"
  "-XXXXXX-"
  "-XXXXXX-"
  "-XXXXXX-"
  "-XXXXXX-"
  "-XXXXXX-"
  "-XXXXXX-");



inline BitPattern GetDisksLeftOfCells6(BitPattern disks, BitPattern cells, int dir) noexcept __attribute__((always_inline));
inline BitPattern GetDisksLeftOfCells6(BitPattern disks, BitPattern cells, int dir) noexcept {
  int dir2 = dir * 2;

  BitPattern result = disks & (cells << dir);
  result |= disks & (result << dir);
  disks = disks & (disks << dir);
  result |= disks & (result << dir2);
  result |= disks & (result << dir2);
  return result;
}

inline BitPattern GetDisksRightOfCells6(BitPattern disks, BitPattern cells, int dir) noexcept __attribute__((always_inline));
inline BitPattern GetDisksRightOfCells6(BitPattern disks, BitPattern cells, int dir) noexcept {
  int dir2 = dir * 2;

  BitPattern result = disks & (cells >> dir);
  result |= disks & (result >> dir);
  disks = disks & (disks >> dir);
  result |= disks & (result >> dir2);
  result |= disks & (result >> dir2);
  return result;
}

inline BitPattern GetMoves(BitPattern player, BitPattern opponent) noexcept __attribute__((always_inline));
inline BitPattern GetMoves(BitPattern player, BitPattern opponent) noexcept {
  BitPattern opponentFlippable = opponent & kNonVerticalFlipMask;
  BitPattern opponentNearPlayer1 = GetDisksLeftOfCells6(opponentFlippable, player, 1) | GetDisksRightOfCells6(opponentFlippable, player, 1);
  BitPattern opponentNearPlayer8 = GetDisksLeftOfCells6(opponent, player, 8) | GetDisksRightOfCells6(opponent, player, 8);
  BitPattern opponentNearPlayer7 = GetDisksLeftOfCells6(opponentFlippable, player, 7) | GetDisksRightOfCells6(opponentFlippable, player, 7);
  BitPattern opponentNearPlayer9 = GetDisksLeftOfCells6(opponentFlippable, player, 9) | GetDisksRightOfCells6(opponentFlippable, player, 9);

  return ~(player | opponent) & (
      (opponentNearPlayer1 << 1) |
      (opponentNearPlayer8 << 8) |
      (opponentNearPlayer7 << 7) |
      (opponentNearPlayer9 << 9) |
      (opponentNearPlayer1 >> 1) |
      (opponentNearPlayer8 >> 8) |
      (opponentNearPlayer7 >> 7) |
      (opponentNearPlayer9 >> 9));
}

inline int GetNMovesApprox(BitPattern player, BitPattern opponent) noexcept __attribute__((always_inline));
inline int GetNMovesApprox(BitPattern empties, BitPattern opponent) noexcept {
  return __builtin_popcountll(Neighbors(opponent) & empties);
}

inline int GetNMoves(BitPattern player, BitPattern opponent) noexcept __attribute__((always_inline));
inline int GetNMoves(BitPattern player, BitPattern opponent) noexcept {
  return __builtin_popcountll(GetMoves(player, opponent));
}

bool HaveToPass(BitPattern player, BitPattern opponent);

BitPattern GetMovesBasic(BitPattern player, BitPattern opponent);

std::vector<BitPattern> GetAllMoves(BitPattern player, BitPattern opponent);

std::vector<BitPattern> GetAllMovesWithPass(BitPattern player, BitPattern opponent);

inline BitPattern SquareFromFlip(BitPattern flip, BitPattern player, BitPattern opponent) noexcept __attribute__((always_inline));
inline BitPattern SquareFromFlip(BitPattern flip, BitPattern player, BitPattern opponent) noexcept {
  return flip & ~(player | opponent);
}

#endif  // BOARD_GET_MOVES_H