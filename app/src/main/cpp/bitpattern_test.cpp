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
#include <stdio.h>
#include "bitpattern.h"

TEST(BitPattern, Neighbors) {
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
  EXPECT_EQ(Neighbors(p), expected);
}

TEST(BitPattern, UniqueInEdges) {
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
  EXPECT_EQ(FirstLastInEdges(~p1), expectedFirstLast);
  EXPECT_EQ(UniqueInEdges(~p1), expected1);
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
  EXPECT_EQ(UniqueInEdges(~p2), expected2);
}

TEST(BitPattern, RowToFirstRow) {
  for (int i = 0; i < 1000; ++i) {
    BitPattern p = RandomPattern();
    Move move = rand() % 64;
    BitPattern row = GetRow(move);
    LastRow result = RowToLastRow(p, row, move & 56);
    EXPECT_TRUE(result < 256);
    EXPECT_EQ((p & row), LastRowToRow(result, move & 56));
  }
}

TEST(BitPattern, ColumnToFirstRow) {
  for (int i = 0; i < 1000; ++i) {
    BitPattern p = RandomPattern();
    Move move = rand() % 64;
    BitPattern column = GetColumn(move);
    LastRow result = ColumnToLastRow(p, column, move & 7);
    EXPECT_TRUE(result < 256);
    EXPECT_EQ((p & column), LastRowToColumn(result, move & 7));
  }
}

TEST(BitPattern, DiagonalToFirstRow) {
  for (int i = 0; i < 1000; ++i) {
    BitPattern p = RandomPattern();
    Move move = rand() % 64;
    BitPattern diagonal = rand() % 2 ? GetDiag7(move) : GetDiag9(move);
    LastRow result = DiagonalToLastRow(p, diagonal);
    EXPECT_TRUE(result < 256);
    EXPECT_EQ((p & diagonal), LastRowToDiagonal(result, diagonal));
  }
}