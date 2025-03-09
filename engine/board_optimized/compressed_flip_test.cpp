/*
 * Copyright 2025 Michele Borassi
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
#include "compressed_flip.h"

TEST(BoardOptimized, IsValidFlip) {
  EXPECT_TRUE(IsValidFlip(ParsePattern("--------", 'X', 8)));
  EXPECT_TRUE(IsValidFlip(ParsePattern("-XXX----", 'X', 8)));
  EXPECT_TRUE(IsValidFlip(ParsePattern("-X-X----", 'X', 8)));
  EXPECT_TRUE(IsValidFlip(ParsePattern("-X-XXXX-", 'X', 8)));
  EXPECT_TRUE(IsValidFlip(ParsePattern("-XXXXXX-", 'X', 8)));
  EXPECT_FALSE(IsValidFlip(ParsePattern("X-------", 'X', 8)));
  EXPECT_FALSE(IsValidFlip(ParsePattern("XXX-----", 'X', 8)));
  EXPECT_FALSE(IsValidFlip(ParsePattern("-----XXX", 'X', 8)));
  EXPECT_FALSE(IsValidFlip(ParsePattern("-XX-X-X-", 'X', 8)));
  EXPECT_FALSE(IsValidFlip(ParsePattern("-XX--XX-", 'X', 8)));
}

TEST(BoardOptimized, CompressFlip) {
  for (int flip = 0; flip <= 255; ++flip) {
    if (!IsValidFlip(flip)) {
      continue;
    }
    u_int8_t compressed = CompressFlip(flip);
    EXPECT_LT(compressed, kNumCompressedFlipValues);
    auto decompressed_flip = DecompressFlip(compressed);
    EXPECT_EQ(decompressed_flip, flip);
  }
}

TEST(BoardOptimized, SquareCompressedPatternToFlipRight) {
  std::string pattern = "-XO-XOO-";
  auto player = ParsePattern(pattern.c_str(), 'X', 8);
  auto opponent = ParsePattern(pattern.c_str(), 'O', 8);
  auto row = kLastRowToPattern.value[player][opponent];
  EXPECT_EQ(DecompressFlip(kSquareCompressedPatternToFlip.value_8[4][row]), ParsePattern("--X-----", 'X', 8));
  EXPECT_EQ(DecompressFlip(kSquareCompressedPatternToFlip.value_8[0][row]), ParsePattern("-----XX-", 'X', 8));
  EXPECT_EQ(kSquareCompressedPatternToFlip.value_8[7][row], 0);
  EXPECT_EQ(kSquareCompressedPatternToFlip.value_8[6][row], 0);
  EXPECT_EQ(kSquareCompressedPatternToFlip.value_8[5][row], 0);
}

TEST(BoardOptimized, SquareCompressedPatternToFlipLeft) {
  std::string pattern = "-OX-----";
  auto player = ParsePattern(pattern.c_str(), 'X', 8);
  auto opponent = ParsePattern(pattern.c_str(), 'O', 8);
  auto row = kLastRowToPattern.value[player][opponent];
  EXPECT_EQ(DecompressFlip(kSquareCompressedPatternToFlip.value_8[7][row]), ParsePattern("-X------", 'X', 8));
}

TEST(BoardOptimized, SquareCompressedPatternToFlipNoOutflank) {
  std::string pattern = "-XO--OO-";
  auto player = ParsePattern(pattern.c_str(), 'X', 8);
  auto opponent = ParsePattern(pattern.c_str(), 'O', 8);
  auto row = kLastRowToPattern.value[player][opponent];
  EXPECT_EQ(kSquareCompressedPatternToFlip.value_8[3][row], 0);
  EXPECT_EQ(kSquareCompressedPatternToFlip.value_8[0][row], 0);
}

TEST(BoardOptimized, SquareCompressedPatternToFlipTwoDirections) {
  std::string pattern = "-XO-OOOX";
  auto player = ParsePattern(pattern.c_str(), 'X', 8);
  auto opponent = ParsePattern(pattern.c_str(), 'O', 8);
  auto row = kLastRowToPattern.value[player][opponent];
  EXPECT_EQ(DecompressFlip(kSquareCompressedPatternToFlip.value_8[4][row]), ParsePattern("--X-XXX-", 'X', 8));
}

TEST(BoardOptimized, SmallerValues) {
  std::string pattern = "OOO-XOO-";
  auto player = ParsePattern(pattern.c_str(), 'X', 8);
  auto opponent = ParsePattern(pattern.c_str(), 'O', 8);
  auto row = kLastRowToPattern.value[player][opponent];
  EXPECT_EQ(DecompressFlip(kSquareCompressedPatternToFlip.value_5[0][row]), ParsePattern("-----XX-", 'X', 8));
}

TEST(BoardOptimized, SmallerValuesNotFlipTurn0) {
  std::string pattern = "-----OO-";
  auto player = ParsePattern(pattern.c_str(), 'X', 3);
  auto opponent = ParsePattern(pattern.c_str(), 'O', 3);
  auto row = kLastRowToPattern.value[player][opponent | (8 + 16 + 32 + 64 + 128)];
  EXPECT_EQ(kSquareCompressedPatternToFlip.value_3[0][row], 0);
}

TEST(BoardOptimized, SmallerValuesNotFlipTurn1) {
  std::string pattern = "-----XX-";
  auto player = ParsePattern(pattern.c_str(), 'X', 3);
  auto opponent = ParsePattern(pattern.c_str(), 'O', 3);
  auto row = kLastRowToPattern.value[player][opponent | (8 + 16 + 32 + 64 + 128)];
  EXPECT_EQ(kSquareCompressedPatternToFlip.value_3[0][row], 0);
}