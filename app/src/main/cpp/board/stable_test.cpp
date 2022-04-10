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

#include <gtest/gtest.h>
#include "bitpattern.h"
#include "board.h"
#include "stable.h"

TEST(Stable, Simple) {
  Board testBoard("--------"
                  "--------" 
                  "--------"
                  "--OOO---"
                  "--------"
                  "--------"
                  "--------"
                  "-------X", true);
  BitPattern stable = GetStableDisks(testBoard.GetPlayer(), testBoard.GetOpponent());
  BitPattern expected = ParsePattern(
          "--------"
          "--------"
          "--------" 
          "--------" 
          "--------" 
          "--------" 
          "--------"
          "-------X");
  EXPECT_EQ(expected, stable);
}

TEST(Stable, Left) {
  Board testBoard("X-------"
                  "X-------" 
                  "X-------"
                  "--------"
                  "X-------"
                  "X-------"
                  "X-------"
                  "X-------", true);
  BitPattern stable = GetStableDisks(testBoard.GetPlayer(), testBoard.GetOpponent());
  EXPECT_EQ(testBoard.GetPlayer(), stable);
}

TEST(Stable, Center) {
  Board b("----O---"
          "X---O---" 
          "-O--O--O"
          "--OOO-O-"
          "---OOO--"
          "XXXXXXXX"
          "---XXX--"
          "--X-X-X-", true);
  BitPattern stable = GetStableDisks(b.GetPlayer(), b.GetOpponent());
  BitPattern expected = ParsePattern(
          "--------"
          "--------"
          "--------" 
          "--------" 
          "--------" 
          "----X---" 
          "--------"
          "--------", true);
  std::cout << PatternToString(stable);
  EXPECT_EQ(expected, stable);
}

TEST(Stable, Update) {
  Board b("XXXXOOO-"
          "XXXXO---" 
          "XOOOO--O"
          "XOOOO-O-"
          "--------"
          "-------O"
          "-----XXO"
          "---OOOOO", true);
  BitPattern stable = GetStableDisks(b.GetPlayer(), b.GetOpponent());
  BitPattern expected = ParsePattern("XXXX----"
                                     "XXX-----" 
                                     "X-------"
                                     "X-------"
                                     "--------"
                                     "-------X"
                                     "-------X"
                                     "---XXXXX");
  EXPECT_EQ(expected, stable);
}

TEST(Stable, FullRowsColumns) {
  BitPattern p = ParsePattern("--------"
                              "XXXXXXXX" 
                              "XXXX-XXX"
                              "XXXXXXX-"
                              "-XXXXXXX"
                              "XXX---XX"
                              "--------"
                              "--------");
  BitPattern output = GetFullRows(~p);
  BitPattern expected = ParsePattern("--------"
                                     "XXXXXXXX" 
                                     "--------"
                                     "--------"
                                     "--------"
                                     "--------"
                                     "--------"
                                     "--------");
//  std::cout << PatternToString(output);
  EXPECT_EQ(output, expected);
//  std::cout << PatternToString(GetFullColumns(~Diag9Symmetry(p)));
  EXPECT_EQ(GetFullColumns(~Diag9Symmetry(p)), Diag9Symmetry(expected));
}

TEST(Stable, All) {
  for (int i = 0; i < 100000; i++) {
    Board b = RandomBoard();
    BitPattern stable = GetStableDisks(b.GetPlayer(), b.GetOpponent());
    BitPattern stablePlayer = stable & b.GetPlayer();
    BitPattern stableOpponent = stable & b.GetOpponent();
    BitPattern player = b.GetPlayer();
    BitPattern opponent = b.GetOpponent();
    for (int d = 0; d < 5; ++d) {
      BitPattern empties = ~(player | opponent);
      BitPattern flip = 0;
      for (int move = __builtin_ctzll(empties); empties != 0 && flip == 0; move = __builtin_ctzll(empties)) {
        empties = empties & ~(1L << move);
        flip = GetFlip(move, player, opponent);
      }
      if (flip == 0) {
        break;
      }
      BitPattern tmp = opponent & ~flip;
      opponent = player | flip;
      player = tmp;
      tmp = stableOpponent;
      stableOpponent = stablePlayer;
      stablePlayer = tmp;

      EXPECT_EQ(stablePlayer & opponent, 0)
          << "Original\n" << b << "Stable\n"
          << Board(stablePlayer, stableOpponent)
          << "After moves\n" << Board(player, opponent);
      EXPECT_EQ(stableOpponent & player, 0)
          << "Original\n" << b << "Stable\n"
          << Board(stablePlayer, stableOpponent)
          << "After moves\n" << Board(player, opponent);    }
  }
}


