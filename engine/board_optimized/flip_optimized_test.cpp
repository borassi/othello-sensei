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
#include "flip_optimized.h"

TEST(FlipOptimized, Flip) {
  Board b("OOOOOOOO"
          "OOOOOOOO"
          "OOOOOOOO"
          "OOOOOOOO"
          "OOOOOOOO"
          "OOOOOOOO"
          "OOOOOOOO"
          "OOOOOOOO", true);
  Board expected("OXOOOOOO"
                  "OOOOOOOO"
                  "OOOOOOOO"
                  "OOOOOOOO"
                  "OOOOOOOO"
                  "OOOOOOOO"
                  "OOOOOOOO"
                  "OOOOOOOO", true);

  BoardToPatterns(b.Player(), b.Opponent());
  Flip_B1();
  CheckPatternsOk();
  EXPECT_EQ(expected, PatternsToBoard());
  EXPECT_TRUE(flipped);
  EXPECT_EQ(disk_difference, -62);
}

TEST(FlipOptimized, FlipDouble) {
  Board b("OOOOOOOO"
          "OOOOOOOO"
          "OOOOOOOO"
          "OOOOOOOO"
          "OOOOOOOO"
          "OOOOOOOO"
          "OOOOOOOO"
          "OOOOOOOO", true);
  Board expected("OXXOOOOO"
                 "OOOOOOOO"
                 "OOOOOOOO"
                 "OOOOOOOO"
                 "OOOOOOOO"
                 "OOOOOOOO"
                 "OOOOOOOO"
                 "OOOOOOOO", true);

  BoardToPatterns(b.Player(), b.Opponent());
  Flip_B1C1();
  CheckPatternsOk();
  EXPECT_EQ(expected, PatternsToBoard());
  EXPECT_TRUE(flipped);
  EXPECT_EQ(disk_difference, -60);
}

TEST(FlipOptimized, FlipTriple) {
  Board b("OOOOOOOO"
          "OOOOOOOO"
          "OOOOOOOO"
          "OOOOOOOO"
          "OOOOOOOO"
          "OOOOOOOO"
          "OOOOOOOO"
          "OOOOOOOO", true);
  Board expected("OXXOXXXO"
                 "OOOOOOOO"
                 "OOOOOOOO"
                 "OOOOOOOO"
                 "OOOOOOOO"
                 "OOOOOOOO"
                 "OOOOOOOO"
                 "OOOOOOOO", true);

  BoardToPatterns(b.Player(), b.Opponent());
  Flip_B1C1_E1G1();
  CheckPatternsOk();
  EXPECT_EQ(expected, PatternsToBoard());
  EXPECT_TRUE(flipped);
  EXPECT_EQ(disk_difference, -54);
}

TEST(FlipOptimized, FlipCol) {
  Board b("OOOOOOOO"
          "OOOOOOOO"
          "OOOOOOOO"
          "OOOOOOOO"
          "OOOOOOOO"
          "OOOOOOOO"
          "OOOOOOOO"
          "OOOOOOOO", true);
  Board expected("OOOOOOOO"
                 "OOOOOOOO"
                 "OOXOOOOO"
                 "OOOOOOOO"
                 "OOXOOOOO"
                 "OOXOOOOO"
                 "OOXOOOOO"
                 "OOOOOOOO", true);

  BoardToPatterns(b.Player(), b.Opponent());
  Flip_C3_C5C7();
  CheckPatternsOk();
  EXPECT_EQ(expected, PatternsToBoard());
  EXPECT_TRUE(flipped);
  EXPECT_EQ(disk_difference, -56);
}

TEST(FlipOptimized, FlipDiag7) {
  Board b("OOOOOOOO"
          "OOOOOOOO"
          "OOOOOOOO"
          "OOOOOOOO"
          "OOOOOOOO"
          "OOOOOOOO"
          "OOOOOOOO"
          "OOOOOOOO", true);
  Board expected("OOOOOOOO"
                 "OOOXOOOO"
                 "OOXOOOOO"
                 "OOOOOOOO"
                 "OOOOOOOO"
                 "OOOOOOOO"
                 "OOOOOOOO"
                 "OOOOOOOO", true);

  BoardToPatterns(b.Player(), b.Opponent());
  Flip_D2C3();
  EXPECT_EQ(expected, PatternsToBoard());
  CheckPatternsOk();
  EXPECT_TRUE(flipped);
  EXPECT_EQ(disk_difference, -60);
}

TEST(FlipOptimized, FlipDiag9) {
  Board b("OOOOOOOO"
          "OOOOOOOO"
          "OOOOOOOO"
          "OOOOOOOO"
          "OOOOOOOO"
          "OOOOOOOO"
          "OOOOOOOO"
          "OOOOOOOO", true);
  Board expected("OOOOOOOO"
                 "OXOOOOOO"
                 "OOXOOOOO"
                 "OOOXOOOO"
                 "OOOOXOOO"
                 "OOOOOOOO"
                 "OOOOOOOO"
                 "OOOOOOOO", true);

  BoardToPatterns(b.Player(), b.Opponent());
  Flip_B2E5();
  EXPECT_EQ(expected, PatternsToBoard());
  CheckPatternsOk();
  EXPECT_TRUE(flipped);
  EXPECT_EQ(disk_difference, -56);
}

TEST(FlipOptimized, Turn) {
  Board b("XXXXXXXX"
          "XXXXXXXX"
          "XXXXXXXX"
          "XXXXXXXX"
          "XXXXXXXX"
          "XXXXXXXX"
          "XXXXXXXX"
          "XXXXXXXX", true);
  Board expected("OOOOOOOO"
                 "OXOOOOOO"
                 "OOOOOOOO"
                 "OOOXOOOO"
                 "OOOOOOOO"
                 "OOOOOOOO"
                 "OOOOOOOO"
                 "OOOOOOOO", true);

  BoardToPatterns(b.Player(), b.Opponent());
  turn = -1;
  Flip_B2_D4();
  EXPECT_EQ(expected, PatternsToBoard());
  CheckPatternsOk();
  EXPECT_TRUE(flipped);
  EXPECT_EQ(disk_difference, 60);
}

TEST(FlipOptimized, Set) {
  Board b("--------"
          "--------"
          "--------"
          "--------"
          "--------"
          "--------"
          "--------"
          "--------", true);
  Board expected("--------"
          "--------"
          "--------"
          "--------"
          "--------"
          "-X------"
          "--------"
          "--------", true);

  BoardToPatterns(b.Player(), b.Opponent());
  Set_B6();
  EXPECT_EQ(expected, PatternsToBoard());
  CheckPatternsOk();
  EXPECT_EQ(disk_difference, 1);
}

TEST(FlipOptimized, SetOtherTurn) {
  Board b("--------"
          "--------"
          "--------"
          "--------"
          "--------"
          "--------"
          "--------"
          "--------", true);
  Board expected("--------"
          "--------"
          "--------"
          "--------"
          "--------"
          "-X------"
          "--------"
          "--------", true);

  BoardToPatterns(b.Player(), b.Opponent());
  turn = -1;
  Set_B6();
  EXPECT_EQ(expected, PatternsToBoard());
  CheckPatternsOk();
  EXPECT_EQ(disk_difference, -1);
}



