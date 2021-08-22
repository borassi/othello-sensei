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
#include <assert.h>
#include <immintrin.h>
#include <iostream>
#include <regex>
#include <string>
#include "bitpattern.h"

int GetEvaluationGameOver(BitPattern player, BitPattern opponent) {
  int playerDisks = __builtin_popcountll(player);
  int opponentDisks = __builtin_popcountll(opponent);
  int diff = playerDisks - opponentDisks;
  return (diff + (int) ((diff > 0) - (diff < 0)) * (64 - playerDisks - opponentDisks));
}

std::string PatternToString(BitPattern pattern) {
  std::string result = "";
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

LastRow RowToLastRow(BitPattern pattern, BitPattern row, int row_num) {
  assert ((kLastRowPattern << row_num) == row);
  return (pattern & row) >> row_num;
}
LastRow ColumnToLastRow(BitPattern pattern, BitPattern column, int col_num) {
  assert ((kLastColumnPattern << col_num) == column);
  return (((pattern & column) >> col_num) * kMainDiag9Pattern) >> 56;
}
LastRow DiagonalToLastRow(BitPattern pattern, BitPattern diagonal) {
  return ((pattern & diagonal) * kLastColumnPattern) >> 56;
}

BitPattern LastRowToRow(LastRow last_row, int row_num) {
  return ((BitPattern) last_row) << row_num;
}
BitPattern LastRowToColumn(LastRow last_row, int col_num) {
  return ((last_row * kMainDiag9Pattern) & kFirstColumnPattern) >> (7-col_num);
}
BitPattern LastRowToDiagonal(LastRow last_row, BitPattern diagonal) {
  return (last_row * kLastColumnPattern) & diagonal;
}

BitPattern UniqueSet(BitPattern b) {
  return __builtin_popcountll(b) == 1 ? b : 0;
}

BitPattern FirstLastSet(BitPattern b) {
  return (1ULL << __builtin_ctzll(b)) | (1ULL << (63 - __builtin_clzll(b)));
}

BitPattern UniqueInEdges(BitPattern empties) {
  return UniqueSet(empties & kLastRowPattern)
          | UniqueSet(empties & kFirstRowPattern)
          | UniqueSet(empties & kFirstColumnPattern)
          | UniqueSet(empties & kLastColumnPattern);
}

BitPattern FirstLastInEdges(BitPattern empties) {
  return (FirstLastSet(empties & kLastRowPattern)
          | FirstLastSet(empties & kFirstRowPattern)
          | FirstLastSet(empties & kFirstColumnPattern)
          | FirstLastSet(empties & kLastColumnPattern)) & ~kCornerPattern;
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