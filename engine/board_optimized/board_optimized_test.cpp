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

TEST(BoardOptimized, ReadBoardGeneric) {
  for (int i = 0; i < 2000; ++i) {
    Board b = RandomBoard();
    BoardToPatterns(b.Player(), b.Opponent());
    ASSERT_EQ(PatternsToBoard(), b) << b << "\n" << PatternsToBoard();
    CheckPatternsOk();
    ASSERT_EQ(depth, 0);
    ASSERT_FALSE(flipped);
  }
}

TEST(BoardOptimized, ReadBoard) {
  Board b("-OXX----"
          "--------"
          "--------"
          "--------"
          "--------"
          "--------"
          "--------"
          "--------", true);
  BoardToPatterns(b.Player(), b.Opponent());
  ASSERT_EQ(PatternsToBoard(), b) << b << "\n" << PatternsToBoard();
  CheckPatternsOk();
  ASSERT_EQ(depth, 0);
  ASSERT_FALSE(flipped);
  ASSERT_EQ(disk_difference, 1);
}