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

#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include "bitpattern.h"
#include "board.h"
#include "get_flip.h"

TEST(GetFlip, Basic) {
  Board horiz("--------"
              "------OX"
              "--------"
              "-OOOOOOX"
              "XOOOOOO-"
              "-----XOO"
              "--------"
              "OOX-----", true);
  BitPattern flip1 = ParsePattern("--------"
                                  "-----XX-"
                                  "--------"
                                  "--------"
                                  "--------"
                                  "--------"
                                  "--------"
                                  "--------");
  BitPattern flip2 = ParsePattern("--------"
                                  "--------"
                                  "--------"
                                  "XXXXXXX-"
                                  "--------"
                                  "--------"
                                  "--------"
                                  "--------");
  BitPattern flip3 = ParsePattern("--------"
                                  "--------"
                                  "--------"
                                  "--------"
                                  "-XXXXXXX"
                                  "--------"
                                  "--------"
                                  "--------");
  
  if (GetFlipBasic(50, horiz.Player(), horiz.Opponent()) != flip1) {
    std::cout << PatternToString(flip1) << "\n";
    std::cout << PatternToString(GetFlipBasic(50, horiz.Player(), horiz.Opponent()));
  }
  if (GetFlipBasic(39, horiz.Player(), horiz.Opponent()) != flip2) {
    std::cout << PatternToString(flip2) << "\n";
    std::cout << PatternToString(GetFlipBasic(39, horiz.Player(), horiz.Opponent()));
  }
  if (GetFlipBasic(24, horiz.Player(), horiz.Opponent()) != flip3) {
    std::cout << PatternToString(flip2) << "\n";
    std::cout << PatternToString(GetFlipBasic(24, horiz.Player(), horiz.Opponent()));
  }
  if (GetFlipBasic(24, horiz.Player(), horiz.Opponent()) != flip3) {
    std::cout << PatternToString(flip2) << "\n";
    std::cout << PatternToString(GetFlipBasic(24, horiz.Player(), horiz.Opponent()));
  }

  EXPECT_EQ(GetFlipBasic(8, horiz.Player(), horiz.Opponent()), 0);
  EXPECT_EQ(GetFlipBasic(15, horiz.Player(), horiz.Opponent()), 0);
}

TEST(GetFlip, Outflank) {
  int flip = 0b00111010;
  EXPECT_EQ(kOutflank[0 << 8 | flip], 0b00000100);
  EXPECT_EQ(kOutflank[1 << 8 | flip], 0);
  EXPECT_EQ(kOutflank[2 << 8 | flip], 0b01000001);
  EXPECT_EQ(kOutflank[4 << 8 | flip], 0);
  EXPECT_EQ(kOutflank[6 << 8 | flip], 0b00000100);

  EXPECT_EQ(kFlip[0 << 8 | 0b00001000], 0b00000111);
  EXPECT_EQ(kFlip[1 << 8 | 0b00001000], 0b00000110);
  EXPECT_EQ(kFlip[2 << 8 | 0b00001000], 0);
  EXPECT_EQ(kFlip[3 << 8 | 0b00001000], 0);
  EXPECT_EQ(kFlip[4 << 8 | 0b00001000], 0);
  EXPECT_EQ(kFlip[5 << 8 | 0b00001000], 0b00110000);
  EXPECT_EQ(kFlip[6 << 8 | 0b00001000], 0b01110000);
  EXPECT_EQ(kFlip[7 << 8 | 0b00001000], 0b11110000);

  EXPECT_EQ(kFlip[0 << 8 | 0b01000010], 0);
  EXPECT_EQ(kFlip[1 << 8 | 0b01000010], 0);
  EXPECT_EQ(kFlip[2 << 8 | 0b01000010], 0);
  EXPECT_EQ(kFlip[3 << 8 | 0b01000010], 0b00111100);
  EXPECT_EQ(kFlip[4 << 8 | 0b01000010], 0b00111100);
  EXPECT_EQ(kFlip[5 << 8 | 0b01000010], 0);
  EXPECT_EQ(kFlip[6 << 8 | 0b01000010], 0);
  EXPECT_EQ(kFlip[7 << 8 | 0b01000010], 0);
}

TEST(GetFlip, Random) {
  for (int i = 0; i < 1000000; ++i) {
    Board b = RandomBoard();
    Square move = rand() % 64;
    if (b.IsEmpty(move)) {
      BitPattern flip_basic = GetFlipBasic(move, b.Player(), b.Opponent());
      BitPattern flip = GetFlip(move, b.Player(), b.Opponent());
      EXPECT_EQ(flip_basic, flip) << b << " " << move << "\n"  << PatternToString(flip_basic) << "\n" << PatternToString(flip) << "\n";
    }
  }
  PrintMoveMetadata();
}

