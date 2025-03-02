/*
 * Copyright 2024 Michele Borassi
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

#ifndef SENSEI_BOARD_OPTIMIZED_BOARD_OPTIMIZED_H
#define SENSEI_BOARD_OPTIMIZED_BOARD_OPTIMIZED_H

#include "../board/bitpattern.h"
#include "../board/board.h"
#if __BMI2__
#include <immintrin.h>
#endif
#include <tuple>

typedef uint64_t BoardPattern;
typedef uint16_t SingleRowPattern;
typedef uint8_t SingleRowPatternSmall;

// Bottom 4
BoardPattern top_rows;
BoardPattern bottom_rows;
BoardPattern left_columns;
BoardPattern right_columns;
BoardPattern top_diag7s;
BoardPattern bottom_diag7s;
BoardPattern top_diag9s;
BoardPattern bottom_diag9s;

bool flipped;
Square depth;
int8_t turn;
int8_t action_stack_count;
int8_t disk_difference;

void NoFlip() {}

struct ActionStack {
  void (*value[128][5])();
  ActionStack() : value() {
    for (int i = 0; i < 128; ++i) {
      for (int j = 0; j < 5; ++j) {
        value[i][j] = &NoFlip;
      }
    }
  }
};

ActionStack action_stack;

SingleRowPattern* const row_a1_h1 = ((SingleRowPattern*) &top_rows) + 3;
SingleRowPattern* const row_a2_h2 = ((SingleRowPattern*) &top_rows) + 2;
SingleRowPattern* const row_a3_h3 = ((SingleRowPattern*) &top_rows) + 1;
SingleRowPattern* const row_a4_h4 = ((SingleRowPattern*) &top_rows);
SingleRowPattern* const row_a5_h5 = ((SingleRowPattern*) &bottom_rows) + 3;
SingleRowPattern* const row_a6_h6 = ((SingleRowPattern*) &bottom_rows) + 2;
SingleRowPattern* const row_a7_h7 = ((SingleRowPattern*) &bottom_rows) + 1;
SingleRowPattern* const row_a8_h8 = ((SingleRowPattern*) &bottom_rows);

SingleRowPattern* const column_a1_a8 = ((SingleRowPattern*) &left_columns) + 3;
SingleRowPattern* const column_b1_b8 = ((SingleRowPattern*) &left_columns) + 2;
SingleRowPattern* const column_c1_c8 = ((SingleRowPattern*) &left_columns) + 1;
SingleRowPattern* const column_d1_d8 = ((SingleRowPattern*) &left_columns);
SingleRowPattern* const column_e1_e8 = ((SingleRowPattern*) &right_columns) + 3;
SingleRowPattern* const column_f1_f8 = ((SingleRowPattern*) &right_columns) + 2;
SingleRowPattern* const column_g1_g8 = ((SingleRowPattern*) &right_columns) + 1;
SingleRowPattern* const column_h1_h8 = ((SingleRowPattern*) &right_columns);

SingleRowPatternSmall* const diag7_c1_a3 = ((SingleRowPatternSmall*) &top_diag7s) + 7;
SingleRowPatternSmall* const diag7_d1_a4 = ((SingleRowPatternSmall*) &top_diag7s) + 6;
SingleRowPatternSmall* const diag7_e1_a5 = ((SingleRowPatternSmall*) &top_diag7s) + 5;
SingleRowPatternSmall* const diag7_h6_f8 = ((SingleRowPatternSmall*) &top_diag7s) + 4;
SingleRowPattern* const diag7_f1_a6 = ((SingleRowPattern*) &top_diag7s) + 1;
SingleRowPattern* const diag7_g1_a7 = ((SingleRowPattern*) &top_diag7s);
SingleRowPattern* const diag7_h1_a8 = ((SingleRowPattern*) &bottom_diag7s) + 3;
SingleRowPattern* const diag7_h2_b8 = ((SingleRowPattern*) &bottom_diag7s) + 2;
SingleRowPattern* const diag7_h3_c8 = ((SingleRowPattern*) &bottom_diag7s) + 1;
SingleRowPatternSmall* const diag7_h4_d8 = ((SingleRowPatternSmall*) &bottom_diag7s) + 1;
SingleRowPatternSmall* const diag7_h5_e8 = ((SingleRowPatternSmall*) &bottom_diag7s);

SingleRowPatternSmall* const diag9_f1_h3 = ((SingleRowPatternSmall*) &top_diag9s) + 7;
SingleRowPatternSmall* const diag9_e1_h4 = ((SingleRowPatternSmall*) &top_diag9s) + 6;
SingleRowPatternSmall* const diag9_d1_h5 = ((SingleRowPatternSmall*) &top_diag9s) + 5;
SingleRowPatternSmall* const diag9_a6_c8 = ((SingleRowPatternSmall*) &top_diag9s) + 4;
SingleRowPattern* const diag9_c1_h6 = ((SingleRowPattern*) &top_diag9s) + 1;
SingleRowPattern* const diag9_b1_h7 = ((SingleRowPattern*) &top_diag9s);
SingleRowPattern* const diag9_a1_h8 = ((SingleRowPattern*) &bottom_diag9s) + 3;
SingleRowPattern* const diag9_a2_g8 = ((SingleRowPattern*) &bottom_diag9s) + 2;
SingleRowPattern* const diag9_a3_f8 = ((SingleRowPattern*) &bottom_diag9s) + 1;
SingleRowPatternSmall* const diag9_a4_e8 = ((SingleRowPatternSmall*) &bottom_diag9s) + 1;
SingleRowPatternSmall* const diag9_a5_d8 = ((SingleRowPatternSmall*) &bottom_diag9s);


constexpr SingleRowPattern kPowersOf3[] = {1, 3, 9, 27, 81, 243, 729, 2187, 6561};

constexpr SingleRowPattern LastRowToPatternExplicit(BitPattern player, BitPattern opponent) {
  assert((player & opponent) == 0);
  assert(player < 256);
  assert(opponent < 256);
  short result = 0;

  for (int i = 0; i < 8; ++i) {
    if (player & (1 << i)) {
      result += 2 * kPowersOf3[i];
    } else if (!(opponent & (1 << i))) {
      result += kPowersOf3[i];
    }
  }
  assert(result < kPowersOf3[8]);
  return result;
}

struct LastRowToPattern {
  SingleRowPattern value[256][256];

  constexpr LastRowToPattern() : value() {
    for (int i = 0; i < 256; ++i) {
      for (int j = 0; j < 256; ++j) {
        if ((i & j) == 0) {
          value[i][j] = LastRowToPatternExplicit(i, j);
        } else {
          value[i][j] = 0;
        }
      }
    }
  }
};

constexpr LastRowToPattern kLastRowToPattern;

void NoMove();

void BoardToPatterns(BitPattern player, BitPattern opponent) {
  *row_a1_h1 = kLastRowToPattern.value[_pext_u64(player, kLastRowPattern << 56)][_pext_u64(opponent, kLastRowPattern << 56)];
  *row_a2_h2 = kLastRowToPattern.value[_pext_u64(player, kLastRowPattern << 48)][_pext_u64(opponent, kLastRowPattern << 48)];
  *row_a3_h3 = kLastRowToPattern.value[_pext_u64(player, kLastRowPattern << 40)][_pext_u64(opponent, kLastRowPattern << 40)];
  *row_a4_h4 = kLastRowToPattern.value[_pext_u64(player, kLastRowPattern << 32)][_pext_u64(opponent, kLastRowPattern << 32)];
  *row_a5_h5 = kLastRowToPattern.value[_pext_u64(player, kLastRowPattern << 24)][_pext_u64(opponent, kLastRowPattern << 24)];
  *row_a6_h6 = kLastRowToPattern.value[_pext_u64(player, kLastRowPattern << 16)][_pext_u64(opponent, kLastRowPattern << 16)];
  *row_a7_h7 = kLastRowToPattern.value[_pext_u64(player, kLastRowPattern << 8)][_pext_u64(opponent, kLastRowPattern << 8)];
  *row_a8_h8 = kLastRowToPattern.value[_pext_u64(player, kLastRowPattern << 0)][_pext_u64(opponent, kLastRowPattern << 0)];

  *column_a1_a8 = kLastRowToPattern.value[_pext_u64(player, kLastColumnPattern << 7)][_pext_u64(opponent, kLastColumnPattern << 7)];
  *column_b1_b8 = kLastRowToPattern.value[_pext_u64(player, kLastColumnPattern << 6)][_pext_u64(opponent, kLastColumnPattern << 6)];
  *column_c1_c8 = kLastRowToPattern.value[_pext_u64(player, kLastColumnPattern << 5)][_pext_u64(opponent, kLastColumnPattern << 5)];
  *column_d1_d8 = kLastRowToPattern.value[_pext_u64(player, kLastColumnPattern << 4)][_pext_u64(opponent, kLastColumnPattern << 4)];
  *column_e1_e8 = kLastRowToPattern.value[_pext_u64(player, kLastColumnPattern << 3)][_pext_u64(opponent, kLastColumnPattern << 3)];
  *column_f1_f8 = kLastRowToPattern.value[_pext_u64(player, kLastColumnPattern << 2)][_pext_u64(opponent, kLastColumnPattern << 2)];
  *column_g1_g8 = kLastRowToPattern.value[_pext_u64(player, kLastColumnPattern << 1)][_pext_u64(opponent, kLastColumnPattern << 1)];
  *column_h1_h8 = kLastRowToPattern.value[_pext_u64(player, kLastColumnPattern << 0)][_pext_u64(opponent, kLastColumnPattern << 0)];

  *diag7_c1_a3 = kLastRowToPattern.value[_pext_u64(player, kMainDiag7Pattern << 40)][_pext_u64(opponent, kMainDiag7Pattern << 40) | 248];
  *diag7_d1_a4 = kLastRowToPattern.value[_pext_u64(player, kMainDiag7Pattern << 32)][_pext_u64(opponent, kMainDiag7Pattern << 32) | 240];
  *diag7_e1_a5 = kLastRowToPattern.value[_pext_u64(player, kMainDiag7Pattern << 24)][_pext_u64(opponent, kMainDiag7Pattern << 24) | 224];
  *diag7_h6_f8 = kLastRowToPattern.value[_pext_u64(player, kMainDiag7Pattern >> 40)][_pext_u64(opponent, kMainDiag7Pattern >> 40) | 248];
  *diag7_f1_a6 = kLastRowToPattern.value[_pext_u64(player, kMainDiag7Pattern << 16)][_pext_u64(opponent, kMainDiag7Pattern << 16) | 192];
  *diag7_g1_a7 = kLastRowToPattern.value[_pext_u64(player, kMainDiag7Pattern << 8)][_pext_u64(opponent, kMainDiag7Pattern << 8) | 128];
  *diag7_h1_a8 = kLastRowToPattern.value[_pext_u64(player, kMainDiag7Pattern << 0)][_pext_u64(opponent, kMainDiag7Pattern << 0)];
  *diag7_h2_b8 = kLastRowToPattern.value[_pext_u64(player, kMainDiag7Pattern >> 8)][_pext_u64(opponent, kMainDiag7Pattern >> 8) | 128];
  *diag7_h3_c8 = kLastRowToPattern.value[_pext_u64(player, kMainDiag7Pattern >> 16)][_pext_u64(opponent, kMainDiag7Pattern >> 16) | 192];
  *diag7_h4_d8 = kLastRowToPattern.value[_pext_u64(player, kMainDiag7Pattern >> 24)][_pext_u64(opponent, kMainDiag7Pattern >> 24) | 224];
  *diag7_h5_e8 = kLastRowToPattern.value[_pext_u64(player, kMainDiag7Pattern >> 32)][_pext_u64(opponent, kMainDiag7Pattern >> 32) | 240];

  *diag9_f1_h3 = kLastRowToPattern.value[_pext_u64(player, kMainDiag9Pattern << 40)][_pext_u64(opponent, kMainDiag9Pattern << 40) | 248];
  *diag9_e1_h4 = kLastRowToPattern.value[_pext_u64(player, kMainDiag9Pattern << 32)][_pext_u64(opponent, kMainDiag9Pattern << 32) | 240];
  *diag9_d1_h5 = kLastRowToPattern.value[_pext_u64(player, kMainDiag9Pattern << 24)][_pext_u64(opponent, kMainDiag9Pattern << 24) | 224];
  *diag9_a6_c8 = kLastRowToPattern.value[_pext_u64(player, kMainDiag9Pattern >> 40)][_pext_u64(opponent, kMainDiag9Pattern >> 40) | 248];
  *diag9_c1_h6 = kLastRowToPattern.value[_pext_u64(player, kMainDiag9Pattern << 16)][_pext_u64(opponent, kMainDiag9Pattern << 16) | 192];
  *diag9_b1_h7 = kLastRowToPattern.value[_pext_u64(player, kMainDiag9Pattern << 8)][_pext_u64(opponent, kMainDiag9Pattern << 8) | 128];
  *diag9_a1_h8 = kLastRowToPattern.value[_pext_u64(player, kMainDiag9Pattern << 0)][_pext_u64(opponent, kMainDiag9Pattern << 0)];
  *diag9_a2_g8 = kLastRowToPattern.value[_pext_u64(player, kMainDiag9Pattern >> 8)][_pext_u64(opponent, kMainDiag9Pattern >> 8) | 128];
  *diag9_a3_f8 = kLastRowToPattern.value[_pext_u64(player, kMainDiag9Pattern >> 16)][_pext_u64(opponent, kMainDiag9Pattern >> 16) | 192];
  *diag9_a4_e8 = kLastRowToPattern.value[_pext_u64(player, kMainDiag9Pattern >> 24)][_pext_u64(opponent, kMainDiag9Pattern >> 24) | 224];
  *diag9_a5_d8 = kLastRowToPattern.value[_pext_u64(player, kMainDiag9Pattern >> 32)][_pext_u64(opponent, kMainDiag9Pattern >> 32) | 240];

  depth = 0;
  flipped = false;
  turn = 1;
  disk_difference = __builtin_popcountll(player) - __builtin_popcountll(opponent);
}

constexpr int GetFromPattern(SingleRowPattern pattern, int position) {
  return (pattern % kPowersOf3[position+1]) / kPowersOf3[position] - 1;
}

void CheckPatternsOk() {
  assert(GetFromPattern(*row_a1_h1, 0) == GetFromPattern(*column_h1_h8, 7));
  assert(GetFromPattern(*row_a1_h1, 0) == GetFromPattern(*diag7_h1_a8, 7));
  assert(GetFromPattern(*row_a1_h1, 1) == GetFromPattern(*column_g1_g8, 7));
  assert(GetFromPattern(*row_a1_h1, 1) == GetFromPattern(*diag7_g1_a7, 6));
  assert(GetFromPattern(*row_a1_h1, 2) == GetFromPattern(*column_f1_f8, 7));
  assert(GetFromPattern(*row_a1_h1, 2) == GetFromPattern(*diag7_f1_a6, 5));
  assert(GetFromPattern(*row_a1_h1, 2) == GetFromPattern(*diag9_f1_h3, 2));
  assert(GetFromPattern(*row_a1_h1, 3) == GetFromPattern(*column_e1_e8, 7));
  assert(GetFromPattern(*row_a1_h1, 3) == GetFromPattern(*diag7_e1_a5, 4));
  assert(GetFromPattern(*row_a1_h1, 3) == GetFromPattern(*diag9_e1_h4, 3));
  assert(GetFromPattern(*row_a1_h1, 4) == GetFromPattern(*column_d1_d8, 7));
  assert(GetFromPattern(*row_a1_h1, 4) == GetFromPattern(*diag7_d1_a4, 3));
  assert(GetFromPattern(*row_a1_h1, 4) == GetFromPattern(*diag9_d1_h5, 4));
  assert(GetFromPattern(*row_a1_h1, 5) == GetFromPattern(*column_c1_c8, 7));
  assert(GetFromPattern(*row_a1_h1, 5) == GetFromPattern(*diag7_c1_a3, 2));
  assert(GetFromPattern(*row_a1_h1, 5) == GetFromPattern(*diag9_c1_h6, 5));
  assert(GetFromPattern(*row_a1_h1, 6) == GetFromPattern(*column_b1_b8, 7));
  assert(GetFromPattern(*row_a1_h1, 6) == GetFromPattern(*diag9_b1_h7, 6));
  assert(GetFromPattern(*row_a1_h1, 7) == GetFromPattern(*column_a1_a8, 7));
  assert(GetFromPattern(*row_a1_h1, 7) == GetFromPattern(*diag9_a1_h8, 7));
  assert(GetFromPattern(*row_a2_h2, 0) == GetFromPattern(*column_h1_h8, 6));
  assert(GetFromPattern(*row_a2_h2, 0) == GetFromPattern(*diag7_h2_b8, 6));
  assert(GetFromPattern(*row_a2_h2, 1) == GetFromPattern(*column_g1_g8, 6));
  assert(GetFromPattern(*row_a2_h2, 1) == GetFromPattern(*diag7_h1_a8, 6));
  assert(GetFromPattern(*row_a2_h2, 1) == GetFromPattern(*diag9_f1_h3, 1));
  assert(GetFromPattern(*row_a2_h2, 2) == GetFromPattern(*column_f1_f8, 6));
  assert(GetFromPattern(*row_a2_h2, 2) == GetFromPattern(*diag7_g1_a7, 5));
  assert(GetFromPattern(*row_a2_h2, 2) == GetFromPattern(*diag9_e1_h4, 2));
  assert(GetFromPattern(*row_a2_h2, 3) == GetFromPattern(*column_e1_e8, 6));
  assert(GetFromPattern(*row_a2_h2, 3) == GetFromPattern(*diag7_f1_a6, 4));
  assert(GetFromPattern(*row_a2_h2, 3) == GetFromPattern(*diag9_d1_h5, 3));
  assert(GetFromPattern(*row_a2_h2, 4) == GetFromPattern(*column_d1_d8, 6));
  assert(GetFromPattern(*row_a2_h2, 4) == GetFromPattern(*diag7_e1_a5, 3));
  assert(GetFromPattern(*row_a2_h2, 4) == GetFromPattern(*diag9_c1_h6, 4));
  assert(GetFromPattern(*row_a2_h2, 5) == GetFromPattern(*column_c1_c8, 6));
  assert(GetFromPattern(*row_a2_h2, 5) == GetFromPattern(*diag7_d1_a4, 2));
  assert(GetFromPattern(*row_a2_h2, 5) == GetFromPattern(*diag9_b1_h7, 5));
  assert(GetFromPattern(*row_a2_h2, 6) == GetFromPattern(*column_b1_b8, 6));
  assert(GetFromPattern(*row_a2_h2, 6) == GetFromPattern(*diag7_c1_a3, 1));
  assert(GetFromPattern(*row_a2_h2, 6) == GetFromPattern(*diag9_a1_h8, 6));
  assert(GetFromPattern(*row_a2_h2, 7) == GetFromPattern(*column_a1_a8, 6));
  assert(GetFromPattern(*row_a2_h2, 7) == GetFromPattern(*diag9_a2_g8, 6));
  assert(GetFromPattern(*row_a3_h3, 0) == GetFromPattern(*column_h1_h8, 5));
  assert(GetFromPattern(*row_a3_h3, 0) == GetFromPattern(*diag7_h3_c8, 5));
  assert(GetFromPattern(*row_a3_h3, 0) == GetFromPattern(*diag9_f1_h3, 0));
  assert(GetFromPattern(*row_a3_h3, 1) == GetFromPattern(*column_g1_g8, 5));
  assert(GetFromPattern(*row_a3_h3, 1) == GetFromPattern(*diag7_h2_b8, 5));
  assert(GetFromPattern(*row_a3_h3, 1) == GetFromPattern(*diag9_e1_h4, 1));
  assert(GetFromPattern(*row_a3_h3, 2) == GetFromPattern(*column_f1_f8, 5));
  assert(GetFromPattern(*row_a3_h3, 2) == GetFromPattern(*diag7_h1_a8, 5));
  assert(GetFromPattern(*row_a3_h3, 2) == GetFromPattern(*diag9_d1_h5, 2));
  assert(GetFromPattern(*row_a3_h3, 3) == GetFromPattern(*column_e1_e8, 5));
  assert(GetFromPattern(*row_a3_h3, 3) == GetFromPattern(*diag7_g1_a7, 4));
  assert(GetFromPattern(*row_a3_h3, 3) == GetFromPattern(*diag9_c1_h6, 3));
  assert(GetFromPattern(*row_a3_h3, 4) == GetFromPattern(*column_d1_d8, 5));
  assert(GetFromPattern(*row_a3_h3, 4) == GetFromPattern(*diag7_f1_a6, 3));
  assert(GetFromPattern(*row_a3_h3, 4) == GetFromPattern(*diag9_b1_h7, 4));
  assert(GetFromPattern(*row_a3_h3, 5) == GetFromPattern(*column_c1_c8, 5));
  assert(GetFromPattern(*row_a3_h3, 5) == GetFromPattern(*diag7_e1_a5, 2));
  assert(GetFromPattern(*row_a3_h3, 5) == GetFromPattern(*diag9_a1_h8, 5));
  assert(GetFromPattern(*row_a3_h3, 6) == GetFromPattern(*column_b1_b8, 5));
  assert(GetFromPattern(*row_a3_h3, 6) == GetFromPattern(*diag7_d1_a4, 1));
  assert(GetFromPattern(*row_a3_h3, 6) == GetFromPattern(*diag9_a2_g8, 5));
  assert(GetFromPattern(*row_a3_h3, 7) == GetFromPattern(*column_a1_a8, 5));
  assert(GetFromPattern(*row_a3_h3, 7) == GetFromPattern(*diag7_c1_a3, 0));
  assert(GetFromPattern(*row_a3_h3, 7) == GetFromPattern(*diag9_a3_f8, 5));
  assert(GetFromPattern(*row_a4_h4, 0) == GetFromPattern(*column_h1_h8, 4));
  assert(GetFromPattern(*row_a4_h4, 0) == GetFromPattern(*diag7_h4_d8, 4));
  assert(GetFromPattern(*row_a4_h4, 0) == GetFromPattern(*diag9_e1_h4, 0));
  assert(GetFromPattern(*row_a4_h4, 1) == GetFromPattern(*column_g1_g8, 4));
  assert(GetFromPattern(*row_a4_h4, 1) == GetFromPattern(*diag7_h3_c8, 4));
  assert(GetFromPattern(*row_a4_h4, 1) == GetFromPattern(*diag9_d1_h5, 1));
  assert(GetFromPattern(*row_a4_h4, 2) == GetFromPattern(*column_f1_f8, 4));
  assert(GetFromPattern(*row_a4_h4, 2) == GetFromPattern(*diag7_h2_b8, 4));
  assert(GetFromPattern(*row_a4_h4, 2) == GetFromPattern(*diag9_c1_h6, 2));
  assert(GetFromPattern(*row_a4_h4, 3) == GetFromPattern(*column_e1_e8, 4));
  assert(GetFromPattern(*row_a4_h4, 3) == GetFromPattern(*diag7_h1_a8, 4));
  assert(GetFromPattern(*row_a4_h4, 3) == GetFromPattern(*diag9_b1_h7, 3));
  assert(GetFromPattern(*row_a4_h4, 4) == GetFromPattern(*column_d1_d8, 4));
  assert(GetFromPattern(*row_a4_h4, 4) == GetFromPattern(*diag7_g1_a7, 3));
  assert(GetFromPattern(*row_a4_h4, 4) == GetFromPattern(*diag9_a1_h8, 4));
  assert(GetFromPattern(*row_a4_h4, 5) == GetFromPattern(*column_c1_c8, 4));
  assert(GetFromPattern(*row_a4_h4, 5) == GetFromPattern(*diag7_f1_a6, 2));
  assert(GetFromPattern(*row_a4_h4, 5) == GetFromPattern(*diag9_a2_g8, 4));
  assert(GetFromPattern(*row_a4_h4, 6) == GetFromPattern(*column_b1_b8, 4));
  assert(GetFromPattern(*row_a4_h4, 6) == GetFromPattern(*diag7_e1_a5, 1));
  assert(GetFromPattern(*row_a4_h4, 6) == GetFromPattern(*diag9_a3_f8, 4));
  assert(GetFromPattern(*row_a4_h4, 7) == GetFromPattern(*column_a1_a8, 4));
  assert(GetFromPattern(*row_a4_h4, 7) == GetFromPattern(*diag7_d1_a4, 0));
  assert(GetFromPattern(*row_a4_h4, 7) == GetFromPattern(*diag9_a4_e8, 4));
  assert(GetFromPattern(*row_a5_h5, 0) == GetFromPattern(*column_h1_h8, 3));
  assert(GetFromPattern(*row_a5_h5, 0) == GetFromPattern(*diag7_h5_e8, 3));
  assert(GetFromPattern(*row_a5_h5, 0) == GetFromPattern(*diag9_d1_h5, 0));
  assert(GetFromPattern(*row_a5_h5, 1) == GetFromPattern(*column_g1_g8, 3));
  assert(GetFromPattern(*row_a5_h5, 1) == GetFromPattern(*diag7_h4_d8, 3));
  assert(GetFromPattern(*row_a5_h5, 1) == GetFromPattern(*diag9_c1_h6, 1));
  assert(GetFromPattern(*row_a5_h5, 2) == GetFromPattern(*column_f1_f8, 3));
  assert(GetFromPattern(*row_a5_h5, 2) == GetFromPattern(*diag7_h3_c8, 3));
  assert(GetFromPattern(*row_a5_h5, 2) == GetFromPattern(*diag9_b1_h7, 2));
  assert(GetFromPattern(*row_a5_h5, 3) == GetFromPattern(*column_e1_e8, 3));
  assert(GetFromPattern(*row_a5_h5, 3) == GetFromPattern(*diag7_h2_b8, 3));
  assert(GetFromPattern(*row_a5_h5, 3) == GetFromPattern(*diag9_a1_h8, 3));
  assert(GetFromPattern(*row_a5_h5, 4) == GetFromPattern(*column_d1_d8, 3));
  assert(GetFromPattern(*row_a5_h5, 4) == GetFromPattern(*diag7_h1_a8, 3));
  assert(GetFromPattern(*row_a5_h5, 4) == GetFromPattern(*diag9_a2_g8, 3));
  assert(GetFromPattern(*row_a5_h5, 5) == GetFromPattern(*column_c1_c8, 3));
  assert(GetFromPattern(*row_a5_h5, 5) == GetFromPattern(*diag7_g1_a7, 2));
  assert(GetFromPattern(*row_a5_h5, 5) == GetFromPattern(*diag9_a3_f8, 3));
  assert(GetFromPattern(*row_a5_h5, 6) == GetFromPattern(*column_b1_b8, 3));
  assert(GetFromPattern(*row_a5_h5, 6) == GetFromPattern(*diag7_f1_a6, 1));
  assert(GetFromPattern(*row_a5_h5, 6) == GetFromPattern(*diag9_a4_e8, 3));
  assert(GetFromPattern(*row_a5_h5, 7) == GetFromPattern(*column_a1_a8, 3));
  assert(GetFromPattern(*row_a5_h5, 7) == GetFromPattern(*diag7_e1_a5, 0));
  assert(GetFromPattern(*row_a5_h5, 7) == GetFromPattern(*diag9_a5_d8, 3));
  assert(GetFromPattern(*row_a6_h6, 0) == GetFromPattern(*column_h1_h8, 2));
  assert(GetFromPattern(*row_a6_h6, 0) == GetFromPattern(*diag7_h6_f8, 2));
  assert(GetFromPattern(*row_a6_h6, 0) == GetFromPattern(*diag9_c1_h6, 0));
  assert(GetFromPattern(*row_a6_h6, 1) == GetFromPattern(*column_g1_g8, 2));
  assert(GetFromPattern(*row_a6_h6, 1) == GetFromPattern(*diag7_h5_e8, 2));
  assert(GetFromPattern(*row_a6_h6, 1) == GetFromPattern(*diag9_b1_h7, 1));
  assert(GetFromPattern(*row_a6_h6, 2) == GetFromPattern(*column_f1_f8, 2));
  assert(GetFromPattern(*row_a6_h6, 2) == GetFromPattern(*diag7_h4_d8, 2));
  assert(GetFromPattern(*row_a6_h6, 2) == GetFromPattern(*diag9_a1_h8, 2));
  assert(GetFromPattern(*row_a6_h6, 3) == GetFromPattern(*column_e1_e8, 2));
  assert(GetFromPattern(*row_a6_h6, 3) == GetFromPattern(*diag7_h3_c8, 2));
  assert(GetFromPattern(*row_a6_h6, 3) == GetFromPattern(*diag9_a2_g8, 2));
  assert(GetFromPattern(*row_a6_h6, 4) == GetFromPattern(*column_d1_d8, 2));
  assert(GetFromPattern(*row_a6_h6, 4) == GetFromPattern(*diag7_h2_b8, 2));
  assert(GetFromPattern(*row_a6_h6, 4) == GetFromPattern(*diag9_a3_f8, 2));
  assert(GetFromPattern(*row_a6_h6, 5) == GetFromPattern(*column_c1_c8, 2));
  assert(GetFromPattern(*row_a6_h6, 5) == GetFromPattern(*diag7_h1_a8, 2));
  assert(GetFromPattern(*row_a6_h6, 5) == GetFromPattern(*diag9_a4_e8, 2));
  assert(GetFromPattern(*row_a6_h6, 6) == GetFromPattern(*column_b1_b8, 2));
  assert(GetFromPattern(*row_a6_h6, 6) == GetFromPattern(*diag7_g1_a7, 1));
  assert(GetFromPattern(*row_a6_h6, 6) == GetFromPattern(*diag9_a5_d8, 2));
  assert(GetFromPattern(*row_a6_h6, 7) == GetFromPattern(*column_a1_a8, 2));
  assert(GetFromPattern(*row_a6_h6, 7) == GetFromPattern(*diag7_f1_a6, 0));
  assert(GetFromPattern(*row_a6_h6, 7) == GetFromPattern(*diag9_a6_c8, 2));
  assert(GetFromPattern(*row_a7_h7, 0) == GetFromPattern(*column_h1_h8, 1));
  assert(GetFromPattern(*row_a7_h7, 0) == GetFromPattern(*diag9_b1_h7, 0));
  assert(GetFromPattern(*row_a7_h7, 1) == GetFromPattern(*column_g1_g8, 1));
  assert(GetFromPattern(*row_a7_h7, 1) == GetFromPattern(*diag7_h6_f8, 1));
  assert(GetFromPattern(*row_a7_h7, 1) == GetFromPattern(*diag9_a1_h8, 1));
  assert(GetFromPattern(*row_a7_h7, 2) == GetFromPattern(*column_f1_f8, 1));
  assert(GetFromPattern(*row_a7_h7, 2) == GetFromPattern(*diag7_h5_e8, 1));
  assert(GetFromPattern(*row_a7_h7, 2) == GetFromPattern(*diag9_a2_g8, 1));
  assert(GetFromPattern(*row_a7_h7, 3) == GetFromPattern(*column_e1_e8, 1));
  assert(GetFromPattern(*row_a7_h7, 3) == GetFromPattern(*diag7_h4_d8, 1));
  assert(GetFromPattern(*row_a7_h7, 3) == GetFromPattern(*diag9_a3_f8, 1));
  assert(GetFromPattern(*row_a7_h7, 4) == GetFromPattern(*column_d1_d8, 1));
  assert(GetFromPattern(*row_a7_h7, 4) == GetFromPattern(*diag7_h3_c8, 1));
  assert(GetFromPattern(*row_a7_h7, 4) == GetFromPattern(*diag9_a4_e8, 1));
  assert(GetFromPattern(*row_a7_h7, 5) == GetFromPattern(*column_c1_c8, 1));
  assert(GetFromPattern(*row_a7_h7, 5) == GetFromPattern(*diag7_h2_b8, 1));
  assert(GetFromPattern(*row_a7_h7, 5) == GetFromPattern(*diag9_a5_d8, 1));
  assert(GetFromPattern(*row_a7_h7, 6) == GetFromPattern(*column_b1_b8, 1));
  assert(GetFromPattern(*row_a7_h7, 6) == GetFromPattern(*diag7_h1_a8, 1));
  assert(GetFromPattern(*row_a7_h7, 6) == GetFromPattern(*diag9_a6_c8, 1));
  assert(GetFromPattern(*row_a7_h7, 7) == GetFromPattern(*column_a1_a8, 1));
  assert(GetFromPattern(*row_a7_h7, 7) == GetFromPattern(*diag7_g1_a7, 0));
  assert(GetFromPattern(*row_a8_h8, 0) == GetFromPattern(*column_h1_h8, 0));
  assert(GetFromPattern(*row_a8_h8, 0) == GetFromPattern(*diag9_a1_h8, 0));
  assert(GetFromPattern(*row_a8_h8, 1) == GetFromPattern(*column_g1_g8, 0));
  assert(GetFromPattern(*row_a8_h8, 1) == GetFromPattern(*diag9_a2_g8, 0));
  assert(GetFromPattern(*row_a8_h8, 2) == GetFromPattern(*column_f1_f8, 0));
  assert(GetFromPattern(*row_a8_h8, 2) == GetFromPattern(*diag7_h6_f8, 0));
  assert(GetFromPattern(*row_a8_h8, 2) == GetFromPattern(*diag9_a3_f8, 0));
  assert(GetFromPattern(*row_a8_h8, 3) == GetFromPattern(*column_e1_e8, 0));
  assert(GetFromPattern(*row_a8_h8, 3) == GetFromPattern(*diag7_h5_e8, 0));
  assert(GetFromPattern(*row_a8_h8, 3) == GetFromPattern(*diag9_a4_e8, 0));
  assert(GetFromPattern(*row_a8_h8, 4) == GetFromPattern(*column_d1_d8, 0));
  assert(GetFromPattern(*row_a8_h8, 4) == GetFromPattern(*diag7_h4_d8, 0));
  assert(GetFromPattern(*row_a8_h8, 4) == GetFromPattern(*diag9_a5_d8, 0));
  assert(GetFromPattern(*row_a8_h8, 5) == GetFromPattern(*column_c1_c8, 0));
  assert(GetFromPattern(*row_a8_h8, 5) == GetFromPattern(*diag7_h3_c8, 0));
  assert(GetFromPattern(*row_a8_h8, 5) == GetFromPattern(*diag9_a6_c8, 0));
  assert(GetFromPattern(*row_a8_h8, 6) == GetFromPattern(*column_b1_b8, 0));
  assert(GetFromPattern(*row_a8_h8, 6) == GetFromPattern(*diag7_h2_b8, 0));
  assert(GetFromPattern(*row_a8_h8, 7) == GetFromPattern(*column_a1_a8, 0));
  assert(GetFromPattern(*row_a8_h8, 7) == GetFromPattern(*diag7_h1_a8, 0));
}

void AddToPattern(SingleRowPattern pattern, int position, Square square, BitPattern& player, BitPattern& opponent) {
  switch (GetFromPattern(pattern, position) * turn) {
    case 1:
      player |= 1ULL << square;
      return;
    case -1:
      opponent |= 1ULL << square;
      return;
    case 0:
      return;
    default:
      assert(false);
  }
}

Board PatternsToBoard() {
  BitPattern player = 0;
  BitPattern opponent = 0;
  AddToPattern(*row_a1_h1, 0, 56, player, opponent);
  AddToPattern(*row_a1_h1, 1, 57, player, opponent);
  AddToPattern(*row_a1_h1, 2, 58, player, opponent);
  AddToPattern(*row_a1_h1, 3, 59, player, opponent);
  AddToPattern(*row_a1_h1, 4, 60, player, opponent);
  AddToPattern(*row_a1_h1, 5, 61, player, opponent);
  AddToPattern(*row_a1_h1, 6, 62, player, opponent);
  AddToPattern(*row_a1_h1, 7, 63, player, opponent);
  AddToPattern(*row_a2_h2, 0, 48, player, opponent);
  AddToPattern(*row_a2_h2, 1, 49, player, opponent);
  AddToPattern(*row_a2_h2, 2, 50, player, opponent);
  AddToPattern(*row_a2_h2, 3, 51, player, opponent);
  AddToPattern(*row_a2_h2, 4, 52, player, opponent);
  AddToPattern(*row_a2_h2, 5, 53, player, opponent);
  AddToPattern(*row_a2_h2, 6, 54, player, opponent);
  AddToPattern(*row_a2_h2, 7, 55, player, opponent);
  AddToPattern(*row_a3_h3, 0, 40, player, opponent);
  AddToPattern(*row_a3_h3, 1, 41, player, opponent);
  AddToPattern(*row_a3_h3, 2, 42, player, opponent);
  AddToPattern(*row_a3_h3, 3, 43, player, opponent);
  AddToPattern(*row_a3_h3, 4, 44, player, opponent);
  AddToPattern(*row_a3_h3, 5, 45, player, opponent);
  AddToPattern(*row_a3_h3, 6, 46, player, opponent);
  AddToPattern(*row_a3_h3, 7, 47, player, opponent);
  AddToPattern(*row_a4_h4, 0, 32, player, opponent);
  AddToPattern(*row_a4_h4, 1, 33, player, opponent);
  AddToPattern(*row_a4_h4, 2, 34, player, opponent);
  AddToPattern(*row_a4_h4, 3, 35, player, opponent);
  AddToPattern(*row_a4_h4, 4, 36, player, opponent);
  AddToPattern(*row_a4_h4, 5, 37, player, opponent);
  AddToPattern(*row_a4_h4, 6, 38, player, opponent);
  AddToPattern(*row_a4_h4, 7, 39, player, opponent);
  AddToPattern(*row_a5_h5, 0, 24, player, opponent);
  AddToPattern(*row_a5_h5, 1, 25, player, opponent);
  AddToPattern(*row_a5_h5, 2, 26, player, opponent);
  AddToPattern(*row_a5_h5, 3, 27, player, opponent);
  AddToPattern(*row_a5_h5, 4, 28, player, opponent);
  AddToPattern(*row_a5_h5, 5, 29, player, opponent);
  AddToPattern(*row_a5_h5, 6, 30, player, opponent);
  AddToPattern(*row_a5_h5, 7, 31, player, opponent);
  AddToPattern(*row_a6_h6, 0, 16, player, opponent);
  AddToPattern(*row_a6_h6, 1, 17, player, opponent);
  AddToPattern(*row_a6_h6, 2, 18, player, opponent);
  AddToPattern(*row_a6_h6, 3, 19, player, opponent);
  AddToPattern(*row_a6_h6, 4, 20, player, opponent);
  AddToPattern(*row_a6_h6, 5, 21, player, opponent);
  AddToPattern(*row_a6_h6, 6, 22, player, opponent);
  AddToPattern(*row_a6_h6, 7, 23, player, opponent);
  AddToPattern(*row_a7_h7, 0, 8, player, opponent);
  AddToPattern(*row_a7_h7, 1, 9, player, opponent);
  AddToPattern(*row_a7_h7, 2, 10, player, opponent);
  AddToPattern(*row_a7_h7, 3, 11, player, opponent);
  AddToPattern(*row_a7_h7, 4, 12, player, opponent);
  AddToPattern(*row_a7_h7, 5, 13, player, opponent);
  AddToPattern(*row_a7_h7, 6, 14, player, opponent);
  AddToPattern(*row_a7_h7, 7, 15, player, opponent);
  AddToPattern(*row_a8_h8, 0, 0, player, opponent);
  AddToPattern(*row_a8_h8, 1, 1, player, opponent);
  AddToPattern(*row_a8_h8, 2, 2, player, opponent);
  AddToPattern(*row_a8_h8, 3, 3, player, opponent);
  AddToPattern(*row_a8_h8, 4, 4, player, opponent);
  AddToPattern(*row_a8_h8, 5, 5, player, opponent);
  AddToPattern(*row_a8_h8, 6, 6, player, opponent);
  AddToPattern(*row_a8_h8, 7, 7, player, opponent);
  return Board(player, opponent);
}

#endif  // SENSEI_BOARD_OPTIMIZED_BOARD_OPTIMIZED_H