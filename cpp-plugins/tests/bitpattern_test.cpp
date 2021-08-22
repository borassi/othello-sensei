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

#include "bitpattern_test.h"
#include "../bitpattern.h"


CPPUNIT_TEST_SUITE_REGISTRATION(bitpattern_test);

void bitpattern_test::testNeighbors() {
  BitPattern p = ParsePattern("X--X---X"
                              "--------" 
                              "--------"
                              "X-------"
                              "-------X"
                              "--------"
                              "-------X"
                              "XXX-----");
  BitPattern expected = ParsePattern("-XX-X-X-"
                                     "XXXXX-XX" 
                                     "XX------"
                                     "-X----XX"
                                     "XX----X-"
                                     "------XX"
                                     "XXXX--X-"
                                     "XXXX--XX");
  CPPUNIT_ASSERT(Neighbors(p) == expected);
}
void bitpattern_test::testUniqueInEdges() {
  BitPattern p1 = ParsePattern("XXXX-XXX"
                               "--------" 
                               "X------X"
                               "X-------"
                               "X------X"
                               "-------X"
                               "X------X"
                               "XXXXXXXX");
  BitPattern expected1 = ParsePattern("----X---"
                                      "--------"
                                      "--------"
                                      "--------"
                                      "--------"
                                      "--------"
                                      "--------"
                                      "--------");
  BitPattern expectedFirstLast = ParsePattern("----X---"
                                              "X------X"
                                              "--------"
                                              "-------X"
                                              "--------"
                                              "X-------"
                                              "--------"
                                              "--------");
  CPPUNIT_ASSERT(FirstLastInEdges(~p1) == expectedFirstLast);
  CPPUNIT_ASSERT(UniqueInEdges(~p1) == expected1);
  BitPattern p2 = ParsePattern("X-XXXXXX"
                               "X-------" 
                               "X------X"
                               "X------X"
                               "X------X"
                               "-------X"
                               "X------X"
                               "XXXXXXX-");
  BitPattern expected2 = ParsePattern("-X------"
                                      "--------"
                                      "--------"
                                      "--------"
                                      "--------"
                                      "X-------"
                                      "--------"
                                      "-------X");
  CPPUNIT_ASSERT(UniqueInEdges(~p2) == expected2);
}
void bitpattern_test::testRowToFirstRow() {
  for (int i = 0; i < 1000; ++i) {
    BitPattern p = RandomPattern();
    Move move = rand() % 64;
    BitPattern row = GetRow(move);
    LastRow result = RowToLastRow(p, row, move & 56);
    CPPUNIT_ASSERT(result < 256);
    CPPUNIT_ASSERT((p & row) == LastRowToRow(result, move & 56));
  }
}

void bitpattern_test::testColumnToFirstRow() {
  for (int i = 0; i < 1000; ++i) {
    BitPattern p = RandomPattern();
    Move move = rand() % 64;
    BitPattern column = GetColumn(move);
    LastRow result = ColumnToLastRow(p, column, move & 7);
    CPPUNIT_ASSERT(result < 256);
    CPPUNIT_ASSERT((p & column) == LastRowToColumn(result, move & 7));
  }
}

void bitpattern_test::testDiagonalToFirstRow() {
  for (int i = 0; i < 1000; ++i) {
    BitPattern p = RandomPattern();
    Move move = rand() % 64;
    BitPattern diagonal = rand() % 2 ? GetDiag7(move) : GetDiag9(move);
    LastRow result = DiagonalToLastRow(p, diagonal);
    CPPUNIT_ASSERT(result < 256);
    CPPUNIT_ASSERT((p & diagonal) == LastRowToDiagonal(result, diagonal));
  }  
}

