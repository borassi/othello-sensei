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

#ifndef BITPATTERN_H
#define BITPATTERN_H

#include <iostream>
#include <string>

typedef int Eval;
typedef unsigned long long BitPattern;
typedef u_int8_t Move;
typedef u_int16_t MoveShift;
typedef u_int8_t LastRow;

constexpr BitPattern ParsePattern(const char* pattern, char letter) {
  long result = 0;
  int parsed = 0;
  for (int i = 0; parsed < 64; i++) {
    result = result << 1;
    if (pattern[i] == '\n' || pattern[i] == ' ') {
      continue;
    }
    if (pattern[i] == letter) {
      result = result | 1;
    }
    ++parsed;
  }
  return result;
}

constexpr BitPattern ParsePattern(const char* pattern) {
  return ParsePattern(pattern, 'X');
}

constexpr BitPattern kLastRowPattern = ParsePattern(
        "--------"
        "--------"
        "--------"
        "--------"
        "--------"
        "--------"
        "--------"
        "XXXXXXXX");
constexpr BitPattern kFirstRowPattern = kLastRowPattern << 56;

constexpr BitPattern kLastColumnPattern = ParsePattern(
        "-------X"
        "-------X"
        "-------X"
        "-------X"
        "-------X"
        "-------X"
        "-------X"
        "-------X");

constexpr BitPattern kFirstColumnPattern = kLastColumnPattern << 7;

constexpr BitPattern kMainDiag7Pattern = ParsePattern(
        "-------X"
        "------X-"
        "-----X--"
        "----X---"
        "---X----"
        "--X-----"
        "-X------"
        "X-------");

constexpr BitPattern kMainDiag9Pattern = ParsePattern(
        "X-------"
        "-X------"
        "--X-----"
        "---X----"
        "----X---"
        "-----X--"
        "------X-"
        "-------X");

constexpr BitPattern kCornerPattern = ParsePattern(
        "X------X"
        "--------"
        "--------"
        "--------"
        "--------"
        "--------"
        "--------"
        "X------X");
int GetEvaluationGameOver(BitPattern player, BitPattern opponent);

constexpr BitPattern GetRow(Move move) {
  return kLastRowPattern << (move & 56);
}
constexpr BitPattern GetColumn(Move move) {
  return kLastColumnPattern << (move & 7);
}
constexpr BitPattern GetDiag7(Move move) {
  int shift = (int) ((move & 56) + ((move & 7) << 3)) - 56;
  return shift > 0 ? 
      kMainDiag7Pattern << shift : 
      kMainDiag7Pattern >> -shift;
}
constexpr BitPattern GetDiag9(Move move) {
  int shift = (int) (move & 56) - ((move & 7) << 3);
  return shift > 0 ? 
      kMainDiag9Pattern << shift : 
      kMainDiag9Pattern >> -shift;
}

std::string PatternToString(BitPattern pattern);

constexpr BitPattern Diag9Symmetry(BitPattern b) {
  BitPattern t = (b ^ (b >> 7)) & 0x00aa00aa00aa00aaL;
  b = b ^ t ^ (t << 7);
  t = (b ^ (b >> 14)) & 0x0000cccc0000ccccL;
  b = b ^ t ^ (t << 14);
  t = (b ^ (b >> 28)) & 0x00000000f0f0f0f0L;
  b = b ^ t ^ (t << 28);

  return b;
}

constexpr BitPattern Neighbors(BitPattern b) {
  return (((b << 1) | (b << 9) | (b >> 7)) & ~kLastColumnPattern)
          | (((b >> 1) | (b >> 9) | (b << 7)) & ~kFirstColumnPattern)
          | (b >> 8) | (b << 8);
}

LastRow RowToLastRow(BitPattern pattern, BitPattern row, int row_num);
LastRow ColumnToLastRow(BitPattern pattern, BitPattern column, int col_num);
LastRow DiagonalToLastRow(BitPattern pattern, BitPattern diagonal);

BitPattern LastRowToRow(LastRow last_row, int row_num);
BitPattern LastRowToColumn(LastRow last_row, int col_num);
BitPattern LastRowToDiagonal(LastRow last_row, BitPattern diagonal);

BitPattern UniqueInEdges(BitPattern empties);
BitPattern FirstLastInEdges(BitPattern empties);

BitPattern RandomPattern(double percentage);
BitPattern RandomPattern();

#endif /* BITPATTERN_H */

