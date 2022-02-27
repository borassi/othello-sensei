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
#include <math.h>
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
    Square move = rand() % 64;
    BitPattern row = GetRow(move);
    LastRow result = RowToLastRow(p, row, move & 56);
    // To silence a warning, in case we change the type of LastRow.
    EXPECT_TRUE((int) result < 256);
    EXPECT_EQ((p & row), LastRowToRow(result, move & 56));
  }
}

TEST(BitPattern, ColumnToFirstRow) {
  for (int i = 0; i < 1000; ++i) {
    BitPattern p = RandomPattern();
    Square move = rand() % 64;
    BitPattern column = GetColumn(move);
    LastRow result = ColumnToLastRow(p, column, move & 7);
    EXPECT_TRUE((int) result < 256);
    EXPECT_EQ((p & column), LastRowToColumn(result, move & 7));
  }
}

TEST(BitPattern, DiagonalToFirstRow) {
  for (int i = 0; i < 1000; ++i) {
    BitPattern p = RandomPattern();
    Square move = rand() % 64;
    BitPattern diagonal = rand() % 2 ? GetDiag7(move) : GetDiag9(move);
    LastRow result = DiagonalToLastRow(p, diagonal);
    EXPECT_TRUE((int) result < 256);
    EXPECT_EQ((p & diagonal), LastRowToDiagonal(result, diagonal));
  }
}

TEST(BitPattern, Hash) {
  BitPattern player = RandomPattern();
  BitPattern opponent = RandomPattern() & ~player;
  int n = 1 << 20;
  std::vector<int> hashes(n, 0);

  for (int i = 0; i < n; ++i) {
    if (rand() % 2 == 0) {
      player = player ^ (1L << (rand() % 64));
      opponent = opponent & ~player;
    } else {
      opponent = opponent ^ (1L << (rand() % 64));
      player = player & ~opponent;
    }
    int hash = Hash(player, opponent);
    EXPECT_GE(hash, 0);
    EXPECT_LT(hash, 1L << kBitHashMap);
    hashes[hash % n]++;
  }
  int full = 0;
  for (int i = 0; i < n; ++i) {
    full += hashes[i] > 0 ? 1 : 0;
    EXPECT_LT(hashes[i], log(n));
  }
  EXPECT_GT(full, 0.9 * n / exp(1));
}

TEST(BitPattern, AllSubBitPatterns) {
  for (int i = 0; i < 100; ++i) {
    BitPattern pattern = RandomPattern(0.1);
    if (__builtin_popcountll(pattern) > 12) {
      continue;
    }
    std::vector<BitPattern> sub_patterns = AllSubBitPatterns(pattern);
    EXPECT_EQ((int) pow(2, __builtin_popcountll(pattern)), sub_patterns.size());
    EXPECT_EQ((int) pow(2, __builtin_popcountll(pattern)),
              std::set<BitPattern>(sub_patterns.begin(), sub_patterns.end()).size());
    for (BitPattern sub_pattern : sub_patterns) {
      EXPECT_EQ(0, sub_pattern & ~pattern);
    }
  }
}