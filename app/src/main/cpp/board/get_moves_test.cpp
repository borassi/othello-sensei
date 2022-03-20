/*
 * Copyright 2022 Google LLC
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

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <vector>

#include "bitpattern.h"
#include "board.h"
#include "get_flip.h"
#include "get_moves.h"

TEST(GetMoves, CompareWithBasic) {
  int n = 10000;
  for (int i = 0; i < n; i++) {
    Board b = RandomBoard();
    BitPattern moves_basic = GetMovesBasic(b.GetPlayer(), b.GetOpponent());
    BitPattern moves = GetMoves(b.GetPlayer(), b.GetOpponent());
    ASSERT_EQ(moves, moves_basic);
  }
}

TEST(GetMoves, GetAllMoves) {
  int n = 10000;
  for (int i = 0; i < n; i++) {
    Board b = RandomBoard();
    std::vector<BitPattern> moves = GetAllMoves(b.GetPlayer(), b.GetOpponent());
    std::set<BitPattern> moves_set(moves.begin(), moves.end());
    ASSERT_TRUE(moves.size() == moves_set.size());
    int n_moves = 0;

    for (Square x = 0; x < 64; ++x) {
      if ((1ULL << x) & (b.GetPlayer() | b.GetOpponent())) {
        continue;
      }
      long flip = GetFlipBasic(x, b.GetPlayer(), b.GetOpponent());
      if (flip == 0) {
        continue;
      }
      ASSERT_THAT(moves_set, ::testing::Contains(flip));
      n_moves++;
    }
    if (n_moves == 0) {
      ASSERT_TRUE(HaveToPass(b.GetPlayer(), b.GetOpponent()));
    }
    ASSERT_EQ(moves.size(), n_moves);
  }
}

TEST(GetMoves, SquareFromFlip) {
  int n = 10000;
  for (int i = 0; i < n; i++) {
    Board b = RandomBoard();
    for (Square x = 0; x < 64; ++x) {
      if ((1ULL << x) & (b.GetPlayer() | b.GetOpponent())) {
        continue;
      }
      long flip = GetFlipBasic(x, b.GetPlayer(), b.GetOpponent());
      if (flip != 0) {
        ASSERT_EQ(SquareFromFlip(flip, b.GetPlayer(), b.GetOpponent()), x);
      }
    }
  }
}