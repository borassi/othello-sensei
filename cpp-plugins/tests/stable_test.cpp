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

#include "stable_test.h"
#include "../bitpattern.h"
#include "../board.h"
#include "../stable.h"


CPPUNIT_TEST_SUITE_REGISTRATION(stable);

void stable::testSimple() {
  Board testBoard("--------"
                  "--------" 
                  "--------"
                  "--OOO---"
                  "--------"
                  "--------"
                  "--------"
                  "-------X");
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
  CPPUNIT_ASSERT(expected == stable);
}

void stable::testLeft() {
  Board testBoard("X-------"
                  "X-------" 
                  "X-------"
                  "--------"
                  "X-------"
                  "X-------"
                  "X-------"
                  "X-------");
  BitPattern stable = GetStableDisks(testBoard.GetPlayer(), testBoard.GetOpponent());
  CPPUNIT_ASSERT(testBoard.GetPlayer() == stable);
}

void stable::testCenter() {
  Board b("----O---"
          "X---O---" 
          "-O--O--O"
          "--OOO-O-"
          "---OOO--"
          "XXXXXXXX"
          "---XXX--"
          "--X-X-X-");
  BitPattern stable = GetStableDisks(b.GetPlayer(), b.GetOpponent());
  BitPattern expected = ParsePattern(
          "--------"
          "--------"
          "--------" 
          "--------" 
          "--------" 
          "----X---" 
          "--------"
          "--------");
  std::cout << PatternToString(stable);
  CPPUNIT_ASSERT(expected == stable);
}

void stable::testUpdate() {
  Board b("XXXXOOO-"
          "XXXXO---" 
          "XOOOO--O"
          "XOOOO-O-"
          "--------"
          "-------O"
          "-----XXO"
          "---OOOOO");
  BitPattern stable = GetStableDisks(b.GetPlayer(), b.GetOpponent());
  BitPattern expected = ParsePattern("XXXX----"
                                     "XXX-----" 
                                     "X-------"
                                     "X-------"
                                     "--------"
                                     "-------X"
                                     "-------X"
                                     "---XXXXX");
  CPPUNIT_ASSERT(expected == stable);
}

void stable::testGetFullRowsColumns() {
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
  std::cout << PatternToString(output);
  CPPUNIT_ASSERT(output == expected);
  std::cout << PatternToString(GetFullColumns(~Diag9Symmetry(p)));
  CPPUNIT_ASSERT(GetFullColumns(~Diag9Symmetry(p)) == Diag9Symmetry(expected));
}
//  
//  @Test
//  public void testGetFullDiags() {
//    long input = BitPattern.parsePattern(
//          "---X----\n"
//        + "-----XXX\n"
//        + "----XXXX\n"
//        + "X--XXX--\n"
//        + "--XXXX--\n"
//        + "-XXXX--X\n"
//        + "-XXX--X-\n"
//        + "XXX--XXX\n");
//    long output = FindStableDisks.getFullDiags7(~input);
//    long expected = BitPattern.parsePattern(
//          "--------\n"
//        + "-------X\n"
//        + "------X-\n"
//        + "-----X--\n"
//        + "----X---\n"
//        + "---X---X\n"
//        + "--X---X-\n"
//        + "-X---X-X\n");
//    assertEquals(expected, output);
//    input = BitPattern.horizontalMirror(input);
//    expected = BitPattern.horizontalMirror(expected);
//    assertEquals(expected, FindStableDisks.getFullDiags9(~input));
//  }
//  
void stable::testAll() {
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

      if ((stablePlayer & opponent) != 0) {
        std::cout << "Original\n" << b.ToString();
        std::cout << "Stable\n" << Board(stablePlayer, stableOpponent).ToString();
        std::cout << "After moves\n" << Board(player, opponent).ToString();
        CPPUNIT_ASSERT(false);
      }
      CPPUNIT_ASSERT((stableOpponent & player) == 0);
    }
  }
}


