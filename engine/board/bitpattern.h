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

#ifndef BITPATTERN_H
#define BITPATTERN_H

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "../utils/constants.h"
#include "../utils/random.h"
#include "../utils/types.h"

constexpr Eval kLessThenMinEval = -66;
constexpr Square kNoSquare = 255;
constexpr Eval kMinEval = -64;
constexpr Eval kMaxEval = 64;
constexpr Square kNumSquares = 64;

constexpr EvalLarge EvalToEvalLarge(Eval eval) { return eval * 8; }
constexpr Eval EvalLargeToEvalRound(EvalLarge eval) { return (Eval) ((eval + 8004) / 8 - 1000); }
constexpr Eval EvalLargeToEvalLower(EvalLarge eval) { return (Eval) ((eval + 8000) / 8 - 1000); }
constexpr Eval EvalLargeToEvalUpper(EvalLarge eval) { return (Eval) ((eval + 8007) / 8 - 1000); }

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

constexpr BitPattern kXPattern = ParsePattern(
    "--------"
    "-X----X-"
    "--------"
    "--------"
    "--------"
    "--------"
    "-X----X-"
    "--------");

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
  uint32_t hash_player[33][8][256];
  uint32_t hash_opponent[33][8][256];

  constexpr HashValues() : hash_player(), hash_opponent() {
    Random random;
    for (int row = 0; row < 8; row++) {
      for (int i = 0; i < 256; i++) {
        auto player = random.next();
        auto opponent = random.next();
        for (int bits = 0; bits < 32; ++bits) {
          hash_player[bits][row][i] = player % (1 << bits);
          hash_opponent[bits][row][i] = opponent % (1 << bits);
        }
        hash_player[32][row][i] = player;
        hash_opponent[32][row][i] = opponent;
      }
    }
  }
};

constexpr HashValues kHashValues;

template<int bits>
inline uint32_t Hash(BitPattern player, BitPattern opponent) {
  auto hash_player = kHashValues.hash_player[bits];
  auto hash_opponent = kHashValues.hash_opponent[bits];
  return
      hash_player[0][player & kLastRowPattern] ^
      hash_player[1][(player >> 8) & kLastRowPattern] ^
      hash_player[2][(player >> 16) & kLastRowPattern] ^
      hash_player[3][(player >> 24) & kLastRowPattern] ^
      hash_player[4][(player >> 32) & kLastRowPattern] ^
      hash_player[5][(player >> 40) & kLastRowPattern] ^
      hash_player[6][(player >> 48) & kLastRowPattern] ^
      hash_player[7][(player >> 56)] ^
      hash_opponent[0][opponent & kLastRowPattern] ^
      hash_opponent[1][(opponent >> 8) & kLastRowPattern] ^
      hash_opponent[2][(opponent >> 16) & kLastRowPattern] ^
      hash_opponent[3][(opponent >> 24) & kLastRowPattern] ^
      hash_opponent[4][(opponent >> 32) & kLastRowPattern] ^
      hash_opponent[5][(opponent >> 40) & kLastRowPattern] ^
      hash_opponent[6][(opponent >> 48) & kLastRowPattern] ^
      hash_opponent[7][(opponent >> 56)];
}

inline uint32_t HashFull(BitPattern player, BitPattern opponent) {
  return Hash<32>(player, opponent);
}

std::string PatternToString(BitPattern pattern);

constexpr BitPattern Neighbors(BitPattern b) {
    return (((b << 1) | (b << 9) | (b >> 7)) & ~kLastColumnPattern)
           | (((b >> 1) | (b >> 9) | (b << 7)) & ~kFirstColumnPattern)
           | (b >> 8) | (b << 8);
}

inline LastRow RowToLastRow(BitPattern pattern, BitPattern row, int row_num) {
  assert ((kLastRowPattern << row_num) == row);
  return (LastRow) ((pattern & row) >> row_num);
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

inline Square NEmpties(BitPattern player, BitPattern opponent) {
  return (Square) (64 - __builtin_popcountll(player | opponent));
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

inline Square MoveToSquare(std::string move) {
  return ('h' - move[0]) + ('8' - move[1]) * 8;
}

inline std::string SquareToMove(Square move) {
  return std::string(1, 'h' - (move % 8)) + std::to_string(8 - move / 8);
}

constexpr BitPattern VerticalMirror(BitPattern p) {
	p = ((p >>  8) & 0x00FF00FF00FF00FFULL) | ((p <<  8) & 0xFF00FF00FF00FF00ULL);
	p = ((p >> 16) & 0x0000FFFF0000FFFFULL) | ((p << 16) & 0xFFFF0000FFFF0000ULL);
	return ((p >> 32) & 0x00000000FFFFFFFFULL) | ((p << 32) &
  0xFFFFFFFF00000000ULL);
}

constexpr BitPattern Diag9Mirror(BitPattern b) {
  BitPattern t = (b ^ (b >> 7)) & 0x00aa00aa00aa00aaL;
  b = b ^ t ^ (t << 7);
  t = (b ^ (b >> 14)) & 0x0000cccc0000ccccL;
  b = b ^ t ^ (t << 14);
  t = (b ^ (b >> 28)) & 0x00000000f0f0f0f0L;
  return b ^ t ^ (t << 28);
}

constexpr BitPattern Rotate(BitPattern p) {
  return VerticalMirror(Diag9Mirror(p));
}

std::vector<BitPattern> AllBitPatternTranspositions(BitPattern p);

#endif /* BITPATTERN_H */

