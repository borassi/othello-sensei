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

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "../constants.h"
#include "../utils/random.h"

typedef int8_t Eval;
typedef int EvalLarge;
typedef unsigned long long BitPattern;
typedef u_int8_t Square;
typedef u_int8_t DepthValue;
typedef u_int16_t MoveShift;
typedef u_int8_t LastRow;
typedef unsigned long long NVisited;

constexpr Eval kLessThenMinEval = -66;
constexpr Square kNoSquare = 255;
constexpr Eval kMinEval = -64;
constexpr Eval kMaxEval = 64;
constexpr Square kNumSquares = 64;

constexpr EvalLarge EvalToEvalLarge(Eval eval) { return eval * 8; }

constexpr EvalLarge kLessThenMinEvalLarge = EvalToEvalLarge(kLessThenMinEval);
constexpr EvalLarge kMinEvalLarge = EvalToEvalLarge(kMinEval);
constexpr EvalLarge kMaxEvalLarge = EvalToEvalLarge(kMaxEval);

constexpr BitPattern ParsePattern(const char* pattern, char letter) {
  BitPattern result = 0;
  int parsed = 0;
  for (int i = 0; parsed < 64; i++) {
      result = result << 1L;
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
Eval GetEvaluationGameOver(BitPattern player, BitPattern opponent);

#define FOR_EACH_SET_BIT(pattern, name) \
  for (BitPattern name = pattern; name != 0; name = name & (name - 1))

constexpr BitPattern GetRow(Square move) {
  return kLastRowPattern << (move & 56);
}
constexpr BitPattern GetColumn(Square move) {
  return kLastColumnPattern << (move & 7);
}
constexpr BitPattern GetDiag7(Square move) {
  int shift = (int) ((move & 56) + ((move & 7) << 3)) - 56;
  return shift > 0 ?
         kMainDiag7Pattern << shift :
         kMainDiag7Pattern >> -shift;
}
constexpr BitPattern GetDiag9(Square move) {
  int shift = (int) (move & 56) - ((move & 7) << 3);
  return shift > 0 ?
         kMainDiag9Pattern << shift :
         kMainDiag9Pattern >> -shift;
}

struct HashValues {
  int hash_player[8][256];
  int hash_opponent[8][256];

  constexpr HashValues() : hash_player(), hash_opponent() {
      Random random;
      int hash_size = 1 << kBitHashMap;
      for (int row = 0; row < 8; row++) {
        for (int i = 0; i < 256; i++) {
          hash_player[row][i] = random.next() % hash_size;
          hash_opponent[row][i] = random.next() % hash_size;
        }
      }
    }
};

constexpr HashValues kHashValues;

constexpr int Hash(BitPattern player, BitPattern opponent) {
  return
      kHashValues.hash_player[0][player & kLastRowPattern] ^
      kHashValues.hash_player[1][(player >> 8) & kLastRowPattern] ^
      kHashValues.hash_player[2][(player >> 16) & kLastRowPattern] ^
      kHashValues.hash_player[3][(player >> 24) & kLastRowPattern] ^
      kHashValues.hash_player[4][(player >> 32) & kLastRowPattern] ^
      kHashValues.hash_player[5][(player >> 40) & kLastRowPattern] ^
      kHashValues.hash_player[6][(player >> 48) & kLastRowPattern] ^
      kHashValues.hash_player[7][(player >> 56)] ^
      kHashValues.hash_opponent[0][opponent & kLastRowPattern] ^
      kHashValues.hash_opponent[1][(opponent >> 8) & kLastRowPattern] ^
      kHashValues.hash_opponent[2][(opponent >> 16) & kLastRowPattern] ^
      kHashValues.hash_opponent[3][(opponent >> 24) & kLastRowPattern] ^
      kHashValues.hash_opponent[4][(opponent >> 32) & kLastRowPattern] ^
      kHashValues.hash_opponent[5][(opponent >> 40) & kLastRowPattern] ^
      kHashValues.hash_opponent[6][(opponent >> 48) & kLastRowPattern] ^
      kHashValues.hash_opponent[7][(opponent >> 56)];
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

inline LastRow RowToLastRow(BitPattern pattern, BitPattern row, int row_num) {
  assert ((kLastRowPattern << row_num) == row);
  return (pattern & row) >> row_num;
}
inline LastRow ColumnToLastRow(BitPattern pattern, BitPattern column, int col_num) {
  assert ((kLastColumnPattern << col_num) == column);
  return (((pattern & column) >> col_num) * kMainDiag9Pattern) >> 56;
}
inline LastRow DiagonalToLastRow(BitPattern pattern, BitPattern diagonal) {
  return ((pattern & diagonal) * kLastColumnPattern) >> 56;
}

inline BitPattern LastRowToRow(LastRow last_row, int row_num) {
  return ((BitPattern) last_row) << row_num;
}
inline BitPattern LastRowToColumn(LastRow last_row, int col_num) {
  return ((last_row * kMainDiag9Pattern) & kFirstColumnPattern) >> (7-col_num);
}
inline BitPattern LastRowToDiagonal(LastRow last_row, BitPattern diagonal) {
  return (last_row * kLastColumnPattern) & diagonal;
}

inline BitPattern UniqueSet(BitPattern b) {
  return __builtin_popcountll(b) == 1 ? b : 0;
}

inline BitPattern FirstLastSet(BitPattern b) {
  return b == 0 ? 0 : (1ULL << __builtin_ctzll(b)) | (1ULL << (63 - __builtin_clzll(b)));
}

inline BitPattern UniqueInEdges(BitPattern empties) {
  return UniqueSet(empties & kLastRowPattern)
         | UniqueSet(empties & kFirstRowPattern)
         | UniqueSet(empties & kFirstColumnPattern)
         | UniqueSet(empties & kLastColumnPattern);
}

inline BitPattern FirstLastInEdges(BitPattern empties) {
  return (FirstLastSet(empties & kLastRowPattern)
          | FirstLastSet(empties & kFirstRowPattern)
          | FirstLastSet(empties & kFirstColumnPattern)
          | FirstLastSet(empties & kLastColumnPattern)) & ~kCornerPattern;
}

inline bool IsFull(BitPattern pattern, Square move) {
  return ((1ULL << move) & pattern) != 0;
}

/**
 * Returns all the possible bitpatterns that are empty outside the bitPattern
 * provided.
 * @param p the bit pattern.
 * @return a vector with all the possible patterns.
 */
std::vector<BitPattern> AllSubBitPatterns(BitPattern p);

BitPattern RandomPattern(double percentage);
BitPattern RandomPattern();

#endif /* BITPATTERN_H */

