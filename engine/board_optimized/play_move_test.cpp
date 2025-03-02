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
#include "play_move.h"


class PlayMove : public testing::Test {
 protected:

  void SetUp() override {
    action_stack = ActionStack();
  }
};

TEST_F(PlayMove, PlayHoriz) {
  Board b("-OX-----"
          "--------"
          "--------"
          "--------"
          "--------"
          "--------"
          "--------"
          "--------", true);
  Board expected("OOO-----"
                 "--------"
                 "--------"
                 "--------"
                 "--------"
                 "--------"
                 "--------"
                 "--------", true);

  BoardToPatterns(b.Player(), b.Opponent());
  CheckPatternsOk();
  PlayA1();
  EXPECT_EQ(expected, PatternsToBoard());
  CheckPatternsOk();
  EXPECT_EQ(disk_difference, 3);
}

TEST_F(PlayMove, PlayVert) {
  Board b("--------"
          "O-------"
          "O-------"
          "X-------"
          "--------"
          "--------"
          "--------"
          "--------", true);
  Board expected("O-------"
                 "O-------"
                 "O-------"
                 "O-------"
                 "--------"
                 "--------"
                 "--------"
                 "--------", true);

  BoardToPatterns(b.Player(), b.Opponent());
  PlayA1();
  EXPECT_EQ(expected, PatternsToBoard());
  CheckPatternsOk();
  EXPECT_EQ(disk_difference, 4);
}

TEST_F(PlayMove, PlayDiag9) {
  Board b("--------"
          "-O------"
          "--O-----"
          "---O----"
          "----O---"
          "-----X--"
          "--------"
          "--------", true);
  Board expected("O-------"
                 "-O------"
                 "--O-----"
                 "---O----"
                 "----O---"
                 "-----O--"
                 "--------"
                 "--------", true);

  BoardToPatterns(b.Player(), b.Opponent());
  PlayA1();
  EXPECT_EQ(expected, PatternsToBoard());
  CheckPatternsOk();
  EXPECT_EQ(disk_difference, 6);
}

TEST_F(PlayMove, PlayDiag7) {
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


  BoardToPatterns(b.Player(), b.Opponent());
  PlayF8();
  EXPECT_EQ(expected, PatternsToBoard());
  CheckPatternsOk();
  EXPECT_EQ(disk_difference, 3);
}

TEST_F(PlayMove, PlayMultiDir) {
  Board b("--------"
          "--------"
          "--------"
          "--------"
          "--------"
          "--------"
          "--------"
          "XOO-OOOX", true);
  Board expected("--------"
                 "--------"
                 "--------"
                 "--------"
                 "--------"
                 "--------"
                 "--------"
                 "OOOOOOOO", true);


  BoardToPatterns(b.Player(), b.Opponent());
  PlayD8();
  EXPECT_EQ(expected, PatternsToBoard());
  CheckPatternsOk();
  EXPECT_EQ(disk_difference, 8);
}

TEST_F(PlayMove, PlayStrangeDiagonal) {
  Board b("--------"
          "--------"
          "--------"
          "--------"
          "-------X"
          "--------"
          "-----X--"
          "----O---", true);
  Board expected("--------"
                 "--------"
                 "--------"
                 "--------"
                 "-------X"
                 "------O-"
                 "-----O--"
                 "----O---", true);

  BoardToPatterns(b.Player(), b.Opponent());
  turn = -turn;
  PlayG6();
  EXPECT_EQ(expected, PatternsToBoard());
}


TEST_F(PlayMove, PlayInvertedTurn) {
  Board b("--------"
          "--------"
          "--------"
          "---O----"
          "---X----"
          "-O-X-O--"
          "--XXX---"
          "OXX-XXXO", true);
  Board expected("--------"
                 "--------"
                 "--------"
                 "---O----"
                 "---O----"
                 "-O-O-O--"
                 "--OOO---"
                 "OOOOOOOO", true);

  BoardToPatterns(b.Player(), b.Opponent());
  turn = -turn;
  PlayD8();
  EXPECT_EQ(expected, PatternsToBoard());
  CheckPatternsOk();
  EXPECT_EQ(depth, 1);
  EXPECT_EQ(disk_difference, -16);
}

TEST_F(PlayMove, PlayMultipleMoves) {
  Board b;
  BoardToPatterns(b.Player(), b.Opponent());
  PlayE6();
  PlayF4();
  PlayC3();
  EXPECT_EQ(Board("e6f4c3"), PatternsToBoard());
  CheckPatternsOk();
  EXPECT_EQ(depth, 3);
  EXPECT_EQ(disk_difference, 3);
}

TEST_F(PlayMove, Undo) {
  Board b;
  BoardToPatterns(b.Player(), b.Opponent());
  PlayE6();
  Undo();
  EXPECT_EQ(Board(), PatternsToBoard());
  CheckPatternsOk();
  EXPECT_EQ(depth, 0);
  EXPECT_EQ(disk_difference, 0);
}

TEST_F(PlayMove, MultipleUndo) {
  Board b;
  BoardToPatterns(b.Player(), b.Opponent());
  PlayE6();
  PlayF4();
  PlayC3();
  Undo();
  Undo();
  EXPECT_EQ(Board("e6"), PatternsToBoard());
  CheckPatternsOk();
  EXPECT_EQ(depth, 1);
  EXPECT_EQ(disk_difference, 3);
}

TEST_F(PlayMove, PlayMovePointers) {
  Board b;
  BoardToPatterns(b.Player(), b.Opponent());
  play_move[19]();
  play_move[34]();
  EXPECT_EQ(Board("e6f4"), PatternsToBoard());
  EXPECT_EQ(disk_difference, 0);
}