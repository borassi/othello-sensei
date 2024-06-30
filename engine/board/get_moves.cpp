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

#include <unordered_map>
#include <vector>

#include "bitpattern.h"
#include "board.h"
#include "get_flip.h"
#include "get_moves.h"

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
  BitPattern empties = ~(player | opponent);
  FOR_EACH_SET_BIT(empties, square) {
    BitPattern flip = GetFlip((Square) __builtin_ctzll(square), player, opponent);
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
  if (GetAllMoves(opponent, player).empty()) {
    // Game over.
    return {};
  }
  return {0};
}

bool HaveToPass(BitPattern player, BitPattern opponent) {
  return GetMoves(player, opponent) == 0;
}

bool HaveToPass(const Board& board) {
  return HaveToPass(board.Player(), board.Opponent());
}

bool IsGameOver(const Board& board) {
  return HaveToPass(board.Player(), board.Opponent()) && HaveToPass(board.Opponent(), board.Player());
}

std::vector<Board> GetNextBoardsWithPass(BitPattern player,
                                         BitPattern opponent) {
  std::vector<Board> result;
  std::vector<BitPattern> flips = GetAllMovesWithPass(player, opponent);
  for (BitPattern flip : flips) {
    result.push_back(Board(NewPlayer(flip, opponent),
                           NewOpponent(flip, player)));
  }
  return result;
}

std::vector<Board> GetNextBoardsWithPass(Board b) {
  return GetNextBoardsWithPass(b.Player(), b.Opponent());
}

LastRow SerializeRow(Square square, Square row) {
  assert((row & (1 << square)) != 0);
  int left_flip = 7 - (__builtin_clz(row) - 24) - square;
  int right_flip = square - __builtin_ctz(row);
  assert(left_flip + right_flip + 1 == __builtin_popcount(row));
  if (square <= 1) {
    assert(left_flip <= 15 && left_flip >= 0);  // 4 bits
    return left_flip;
  } else if (square == 2) {
    assert(left_flip <= 4);
    assert(right_flip <= 1);
    return left_flip | (right_flip << 3);
  } else if (square <= 4) {
    assert(left_flip <= 3);
    assert(right_flip <= 3);
    return left_flip | (right_flip << 2);
  } else if (square == 5) {
    assert(left_flip <= 1);
    assert(right_flip <= 4);
    return left_flip | (right_flip << 1);
  } else {
    assert(square <= 7);
    return right_flip;
  }
}

LastRow DeserializeRow(Square square, LastRow serialized) {
  int left_flip;
  int right_flip;
  if (square <= 1) {
    left_flip = serialized;
    right_flip = 0;
  } else if (square == 2) {
    left_flip = serialized & 7;
    right_flip = serialized >> 3;
  } else if (square <= 4) {
    left_flip = serialized & 3;
    right_flip = serialized >> 2;
  } else if (square == 5) {
    left_flip = serialized & 1;
    right_flip = serialized >> 1;
  } else {
    left_flip = 0;
    right_flip = serialized;
  }
  LastRow result = 0;
  for (int i = square - right_flip; i <= square + left_flip; ++i) {
    result |= 1 << i;
  }
  return result;
}

CompressedFlip SerializeFlip(Square square, BitPattern flip) {
  assert(square <= 63);
  if (flip == 0) {
    return 0;
  }
  CompressedFlip result = square;
  uint8_t square_y = square / 8;
  uint8_t square_x = square % 8;
  // Horizontal
  LastRow row = RowToLastRow(flip, GetRow(square), square_y * 8);
  result |= SerializeRow(square_x, row) << 6;
  // Vertical
  LastRow column = ColumnToLastRow(flip, GetColumn(square), square_x);
  result |= SerializeRow(7 - square_y, column) << 10;
  // Diag7
  LastRow diag7 = DiagonalToLastRow(flip, GetDiag7(square));
  result |= SerializeRow(square_x, diag7) << 14;
  // Diag9
  LastRow diag9 = DiagonalToLastRow(flip, GetDiag9(square));
  result |= SerializeRow(square_x, diag9) << 18;

  assert(result != 0);
  return result;
}

std::pair<Square, BitPattern> DeserializeFlip(CompressedFlip flip) {
  if (flip == 0) {
    return std::make_pair(0, 0);
  }
  Square square = flip & 63;
  uint8_t square_y = square / 8;
  uint8_t square_x = square % 8;

  auto row = DeserializeRow(square_x,  (flip >> 6 & 15));
  auto column = DeserializeRow(7 - square_y, (flip >> 10) & 15);
  auto diag7 = DeserializeRow(square_x, (flip >> 14) & 15);
  auto diag9 = DeserializeRow(square_x, (flip >> 18) & 15);

  BitPattern full_flip = LastRowToRow(row, square_y * 8);
  full_flip |= LastRowToColumn(column, square_x);
  full_flip |= LastRowToDiagonal(diag7, GetDiag7(square));
  full_flip |= LastRowToDiagonal(diag9, GetDiag9(square));

  return std::make_pair(square, full_flip);
}

// Returns a map from the possible children (modulo transposition) to the flip.
//
// Example: b is
// OX
// XO
//
// Result is {
// OX
// XXX: (26, XX)
std::unordered_map<Board, std::pair<Square, BitPattern>> GetUniqueNextBoardsWithPass(Board b) {
  std::unordered_map<Board, std::pair<Square, BitPattern>> result;
  std::vector<Board> b_transp = b.AllTranspositions();
  for (auto base_flip : GetAllMovesWithPass(b.Player(), b.Opponent())) {
    std::vector<BitPattern> flip_transp = AllBitPatternTranspositions(base_flip);
    assert(b_transp.size() == flip_transp.size());
    Board best_child;
    BitPattern best_flip = 0;
    Square best_square = 65;
    for (int i = 0; i < b_transp.size(); ++i) {
      BitPattern flip = flip_transp[i];
      Board flipped_father = b_transp[i];
      Board child = flipped_father.Next(flip);
      assert(child.Unique() == b_transp[0].Next(flip_transp[0]).Unique());
      if (best_square == 65 ||
          (child.Player() < best_child.Player() ||
           (child.Player() == best_child.Player() &&
            child.Opponent() < best_child.Opponent()))) {
        best_child = child;
        best_flip = flip;
        best_square = flip == 0 ? -1 : (int) __builtin_ctzll(
            SquareFromFlip(flip, flipped_father.Player(), flipped_father.Opponent()));
      }
    }
    assert(best_square != 65);
    assert(best_child == b.Next(base_flip).Unique());
    // NOTE: We cannot assert that best_square and best_flip are the same as
    // result.find(best_child) if available, as there can be two different flips
    // that yield symmetric positions.
    result[best_child] = {best_square, best_flip};
  }
  return result;
}