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

#include <iostream>
#include <string>
#include "../bitpattern.h"
#include "../board.h"
#include "../get_flip.h"
#include "get_flip_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION(get_flip_test);

void get_flip_test::testGetFlipBasic() {
  Board horiz("--------"
              "------OX"
              "--------"
              "-OOOOOOX"
              "XOOOOOO-"
              "-----XOO"
              "--------"
              "OOX-----");
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
  
  if (GetFlipBasic(50, horiz.GetPlayer(), horiz.GetOpponent()) != flip1) {
    std::cout << PatternToString(flip1) << "\n";
    std::cout << PatternToString(GetFlipBasic(50, horiz.GetPlayer(), horiz.GetOpponent()));
    CPPUNIT_ASSERT(false);
  }
  if (GetFlipBasic(39, horiz.GetPlayer(), horiz.GetOpponent()) != flip2) {
    std::cout << PatternToString(flip2) << "\n";
    std::cout << PatternToString(GetFlipBasic(39, horiz.GetPlayer(), horiz.GetOpponent()));
    CPPUNIT_ASSERT(false);    
  }
  if (GetFlipBasic(24, horiz.GetPlayer(), horiz.GetOpponent()) != flip3) {
    std::cout << PatternToString(flip2) << "\n";
    std::cout << PatternToString(GetFlipBasic(24, horiz.GetPlayer(), horiz.GetOpponent()));
    CPPUNIT_ASSERT(false);    
  }
  if (GetFlipBasic(24, horiz.GetPlayer(), horiz.GetOpponent()) != flip3) {
    std::cout << PatternToString(flip2) << "\n";
    std::cout << PatternToString(GetFlipBasic(24, horiz.GetPlayer(), horiz.GetOpponent()));
    CPPUNIT_ASSERT(false);    
  }
  CPPUNIT_ASSERT(GetFlipBasic(8, horiz.GetPlayer(), horiz.GetOpponent()) == 0);
  CPPUNIT_ASSERT(GetFlipBasic(15, horiz.GetPlayer(), horiz.GetOpponent()) == 0);
}

void get_flip_test::testFlipOutflank() {
  int flip = 0b00111010;
  CPPUNIT_ASSERT(kOutflank[0 << 8 | flip] == 0b00000100);
  CPPUNIT_ASSERT(kOutflank[1 << 8 | flip] == 0);
  CPPUNIT_ASSERT(kOutflank[2 << 8 | flip] == 0b01000001);
  CPPUNIT_ASSERT(kOutflank[4 << 8 | flip] == 0);
  CPPUNIT_ASSERT(kOutflank[6 << 8 | flip] == 0b00000100);

  CPPUNIT_ASSERT(kFlip[0 << 8 | 0b00001000] == 0b00000111);
  CPPUNIT_ASSERT(kFlip[1 << 8 | 0b00001000] == 0b00000110);
  CPPUNIT_ASSERT(kFlip[2 << 8 | 0b00001000] == 0);
  CPPUNIT_ASSERT(kFlip[3 << 8 | 0b00001000] == 0);
  CPPUNIT_ASSERT(kFlip[4 << 8 | 0b00001000] == 0);
  CPPUNIT_ASSERT(kFlip[5 << 8 | 0b00001000] == 0b00110000);
  CPPUNIT_ASSERT(kFlip[6 << 8 | 0b00001000] == 0b01110000);
  CPPUNIT_ASSERT(kFlip[7 << 8 | 0b00001000] == 0b11110000);

  CPPUNIT_ASSERT(kFlip[0 << 8 | 0b01000010] == 0);
  CPPUNIT_ASSERT(kFlip[1 << 8 | 0b01000010] == 0);
  CPPUNIT_ASSERT(kFlip[2 << 8 | 0b01000010] == 0);
  CPPUNIT_ASSERT(kFlip[3 << 8 | 0b01000010] == 0b00111100);
  CPPUNIT_ASSERT(kFlip[4 << 8 | 0b01000010] == 0b00111100);
  CPPUNIT_ASSERT(kFlip[5 << 8 | 0b01000010] == 0);
  CPPUNIT_ASSERT(kFlip[6 << 8 | 0b01000010] == 0);
  CPPUNIT_ASSERT(kFlip[7 << 8 | 0b01000010] == 0);
}

void get_flip_test::testGetFlipRandom() {
  for (int i = 0; i < 1000000; ++i) {
    Board b = RandomBoard();
    Move move = rand() % 64;
    if (b.IsEmpty(move)) {
      BitPattern flip_basic = GetFlipBasic(move, b.GetPlayer(), b.GetOpponent());
      BitPattern flip = GetFlip(move, b.GetPlayer(), b.GetOpponent());
      if (flip_basic != flip) {
        std::cout << "Error\n";
        std::cout << b.ToString() << " " << move << "\n";
        std::cout << PatternToString(flip_basic) << "\n";
        std::cout << PatternToString(flip) << "\n";
        
      }
      CPPUNIT_ASSERT(flip_basic == flip);
    }
  }
  PrintMoveMetadata();
}

