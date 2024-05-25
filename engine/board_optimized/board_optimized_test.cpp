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
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>
#include <regex>
#include "../board/board.h"
#include "board_optimized.h"

TEST(BoardOptimized, ReadBoard) {
  short patterns[kNumPatterns];
  for (int i = 0; i < 2000; ++i) {
    Board b = RandomBoard();
    PatternsInit(patterns, b.Player(), b.Opponent());
    CheckPatternsOk(patterns);
    ASSERT_EQ(ToBoard(patterns), b) << b << "\n" << ToBoard(patterns);
  }
}

TEST(BoardOptimized, EmptyToPlayer) {
  short patterns[kNumPatterns];
  PatternsInit(patterns, 0, 0);
  for (int i = 0; i < 64; ++i) {
    kConstants.flip_function[i + /*new_disk=*/64](patterns);
    ASSERT_EQ(GetSquare(patterns, i), 1);
  }
}

TEST(BoardOptimized, OpponentToPlayer) {
  short patterns[kNumPatterns];
  PatternsInit(patterns, 0, -1);
  for (int i = 0; i < 64; ++i) {
    kConstants.flip_function[i](patterns);
    ASSERT_EQ(GetSquare(patterns, i), 1);
  }
}

TEST(BoardOptimized, CompressFlip) {
  for (int flip = 0; flip <= 255; ++flip) {
    if (flip & 129) {
      continue;
    }
    bool valid_flip = false;
    for (int i = 1; i < 7; ++i) {
      if (IsContiguous((1 << i) | flip)) {
        valid_flip = true;
        break;
      }
    }
    if (!valid_flip) {
      continue;
    }
    u_int8_t compressed = CompressFlip(flip);
    EXPECT_LT(compressed, kNumCompressedFlipValues);
    auto decompressed_flip = DecompressFlip(compressed);
    EXPECT_EQ(decompressed_flip, flip);
  }
}

TEST(BoardOptimized, PatternToSquares) {
  EXPECT_EQ(kConstants.pattern_to_square[0][0], 0);
  EXPECT_EQ(kConstants.pattern_to_square[0][1], 1);
  EXPECT_EQ(kConstants.pattern_to_square[0][7], 7);
  EXPECT_EQ(kConstants.pattern_to_square[1][0], 8);
  EXPECT_EQ(kConstants.pattern_to_square[7][7], 63);
  EXPECT_EQ(kConstants.pattern_to_square[8][0], 0);
  EXPECT_EQ(kConstants.pattern_to_square[8][1], 8);
  EXPECT_EQ(kConstants.pattern_to_square[9][0], 1);
  EXPECT_EQ(kConstants.pattern_to_square[16][0], 5);
  EXPECT_EQ(kConstants.pattern_to_square[16][1], 14);
  EXPECT_EQ(kConstants.pattern_to_square[21][0], 0);
  EXPECT_EQ(kConstants.pattern_to_square[21][7], 63);
  EXPECT_EQ(kConstants.pattern_to_square[27][0], 2);
  EXPECT_EQ(kConstants.pattern_to_square[27][1], 9);
  EXPECT_EQ(kConstants.pattern_to_square[27][2], 16);
  EXPECT_EQ(kConstants.pattern_to_square[27][3], 255);
  EXPECT_EQ(kConstants.pattern_to_square[32][0], 7);
  EXPECT_EQ(kConstants.pattern_to_square[32][1], 14);
}

TEST(BoardOptimized, IsContiguous) {
  EXPECT_TRUE(IsContiguous(0));
  EXPECT_TRUE(IsContiguous(2));
  EXPECT_TRUE(IsContiguous(6));
  EXPECT_TRUE(IsContiguous(126));
  EXPECT_TRUE(IsContiguous(255));
  EXPECT_TRUE(IsContiguous(64));
  EXPECT_FALSE(IsContiguous(5));
  EXPECT_FALSE(IsContiguous(10));
}

TEST(BoardOptimized, NumFlips) {
  EXPECT_EQ(GetFlip(0, 1 + 0*3 + 2*9), CompressFlip(2));
  EXPECT_EQ(kConstants.num_flips[NumFlipOffset(0, 1 + 0*3 + 2*9)], CompressFlip(2));
}

TEST(BoardOptimized, FlipHoriz) {
  short patterns[kNumPatterns];
  Board b("--------"
          "---OOOX-"
          "--------"
          "--------"
          "--------"
          "--------"
          "--------"
          "--------", true);
  Board expected("--------"
                 "--OOOOO-"
                 "--------"
                 "--------"
                 "--------"
                 "--------"
                 "--------"
                 "--------", true);

  PatternsInit(patterns, b.Player(), b.Opponent());
//  PlayMove<53>(patterns);
  kConstants.play_move_function[53](patterns);
  Invert(patterns);
  EXPECT_EQ(expected, ToBoard(patterns));
}

TEST(BoardOptimized, FlipVert) {
  short patterns[kNumPatterns];
  Board b("--------"
          "--------"
          "--------"
          "--------"
          "--------"
          "-----X--"
          "-----O--"
          "--------", true);
  Board expected("--------"
                 "--------"
                 "--------"
                 "--------"
                 "--------"
                 "-----O--"
                 "-----O--"
                 "-----O--", true);

  PatternsInit(patterns, b.Player(), b.Opponent());
  kConstants.play_move_function[2](patterns);
  Invert(patterns);
  EXPECT_EQ(expected, ToBoard(patterns));
}

TEST(BoardOptimized, FlipDiag9) {
  short patterns[kNumPatterns];
  Board b("--------"
          "--------"
          "--------"
          "--------"
          "--------"
          "-----X--"
          "------O-"
          "--------", true);
  Board expected("--------"
                 "--------"
                 "--------"
                 "--------"
                 "--------"
                 "-----O--"
                 "------O-"
                 "-------O", true);

  PatternsInit(patterns, b.Player(), b.Opponent());
  kConstants.play_move_function[0](patterns);
  Invert(patterns);
  EXPECT_EQ(expected, ToBoard(patterns));
}

TEST(BoardOptimized, FlipDiag7) {
  short patterns[kNumPatterns];
  Board b("--------"
          "--------"
          "--------"
          "--------"
          "--------"
          "-------X"
          "------O-"
          "--------", true);
  Board expected("--------"
                 "--------"
                 "--------"
                 "--------"
                 "--------"
                 "-------O"
                 "------O-"
                 "-----O--", true);

  PatternsInit(patterns, b.Player(), b.Opponent());
  kConstants.play_move_function[2](patterns);
  Invert(patterns);
  EXPECT_EQ(expected, ToBoard(patterns));
}