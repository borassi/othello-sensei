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

#ifndef BOARD_GET_MOVES_H
#define BOARD_GET_MOVES_H

#include <unordered_map>

#include "bitpattern.h"
#include "board.h"
#include "get_flip.h"

typedef uint32_t CompressedFlip;

constexpr BitPattern kNonVerticalFlipMask = ParsePattern(
  "-XXXXXX-"
  "-XXXXXX-"
  "-XXXXXX-"
  "-XXXXXX-"
  "-XXXXXX-"
  "-XXXXXX-"
  "-XXXXXX-"
  "-XXXXXX-");



forceinline(BitPattern GetDisksLeftOfCells6(BitPattern disks, BitPattern cells, int dir) noexcept);
inline BitPattern GetDisksLeftOfCells6(BitPattern disks, BitPattern cells, int dir) noexcept {
  int dir2 = dir * 2;

  BitPattern result = disks & (cells << dir);
  result |= disks & (result << dir);
  disks = disks & (disks << dir);
  result |= disks & (result << dir2);
  result |= disks & (result << dir2);
  return result;
}

forceinline(BitPattern GetDisksRightOfCells6(BitPattern disks, BitPattern cells, int dir) noexcept);
inline BitPattern GetDisksRightOfCells6(BitPattern disks, BitPattern cells, int dir) noexcept {
  int dir2 = dir * 2;

  BitPattern result = disks & (cells >> dir);
  result |= disks & (result >> dir);
  disks = disks & (disks >> dir);
  result |= disks & (result >> dir2);
  result |= disks & (result >> dir2);
  return result;
}

forceinline(BitPattern GetMoves(BitPattern player, BitPattern opponent) noexcept);
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

forceinline(int GetNMovesApprox(BitPattern player, BitPattern opponent) noexcept);
inline int GetNMovesApprox(BitPattern empties, BitPattern opponent) noexcept {
  return (int) __builtin_popcountll(Neighbors(opponent) & empties);
}

forceinline(int GetNMoves(BitPattern player, BitPattern opponent) noexcept);
inline int GetNMoves(BitPattern player, BitPattern opponent) noexcept {
  return (int) __builtin_popcountll(GetMoves(player, opponent));
}

bool HaveToPass(BitPattern player, BitPattern opponent);

bool HaveToPass(const Board& board);

bool IsGameOver(const Board& board);

BitPattern GetMovesBasic(BitPattern player, BitPattern opponent);

std::vector<BitPattern> GetAllMoves(BitPattern player, BitPattern opponent);

std::vector<BitPattern> GetAllMovesWithPass(BitPattern player, BitPattern opponent);

std::vector<Board> GetNextBoardsWithPass(BitPattern player, BitPattern opponent);

std::vector<Board> GetNextBoardsWithPass(Board b);

std::unordered_map<Board, std::pair<Square, BitPattern>> GetUniqueNextBoardsWithPass(Board b);

forceinline(BitPattern SquareFromFlip(BitPattern flip, BitPattern player, BitPattern opponent) noexcept);
inline BitPattern SquareFromFlip(BitPattern flip, BitPattern player, BitPattern opponent) noexcept {
  return flip & ~(player | opponent);
}

LastRow SerializeRow(Square square, Square row);

Square DeserializeRow(Square square, LastRow serialized);

CompressedFlip SerializeFlip(Square square, BitPattern flip);

std::pair<Square, BitPattern> DeserializeFlip(CompressedFlip flip);

#endif  // BOARD_GET_MOVES_H