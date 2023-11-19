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

#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include <math.h>
#include <stdio.h>

#include "bitpattern.h"

TEST(BitPattern, EvalToEvalLarge) {
  for (Eval i = kMinEval; i <= kMaxEval; ++i) {
    EXPECT_EQ(i, EvalLargeToEvalRound(EvalToEvalLarge(i)));
    EXPECT_EQ(i, EvalLargeToEvalLower(EvalToEvalLarge(i)));
    EXPECT_EQ(i, EvalLargeToEvalUpper(EvalToEvalLarge(i)));
  }
  for (EvalLarge i = kMinEvalLarge; i <= kMaxEvalLarge; ++i) {
    EXPECT_NEAR(EvalToEvalLarge(EvalLargeToEvalLower(i)), i, 7);
    EXPECT_LE(EvalToEvalLarge(EvalLargeToEvalLower(i)), i);
    EXPECT_NEAR(EvalToEvalLarge(EvalLargeToEvalUpper(i)), i, 7);
    EXPECT_GE(EvalToEvalLarge(EvalLargeToEvalUpper(i)), i);
  }
}

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
  EXPECT_EQ(FirstLastInEdges(~p1), expectedFirstLast) <<
      PatternToString(FirstLastInEdges(~p1));
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
  int n = 1 << 10;
  std::vector<int> hashes(n, 0);

  for (int i = 0; i < n; ++i) {
    if (rand() % 2 == 0) {
      player = player ^ (1L << (rand() % 64));
      opponent = opponent & ~player;
    } else {
      opponent = opponent ^ (1L << (rand() % 64));
      player = player & ~opponent;
    }
    int hash = Hash<10>(player, opponent);
    EXPECT_GE(hash, 0);
    EXPECT_LT(hash, 1L << 10);
    hashes[hash % n]++;
  }
  int full = 0;
  for (int i = 0; i < n; ++i) {
    full += hashes[i] > 0 ? 1 : 0;
    EXPECT_LT(hashes[i], 2 * log(n));
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

TEST(BitPattern, Empties) {
  BitPattern player = ParsePattern("XXXXXXXX"
                               "X------X"
                               "X------X"
                               "X------X"
                               "X------X"
                               "X------X"
                               "X------X"
                               "XXXXXXXX");
  BitPattern opponent = ParsePattern("--------"
                                      "--XXXX--"
                                      "--XXXX--"
                                      "--------"
                                      "--------"
                                      "--------"
                                      "--------"
                                      "--------");
  EXPECT_EQ(28, NEmpties(player, opponent));
  EXPECT_EQ(36, NEmpties(player, 0));
  EXPECT_EQ(56, NEmpties(0, opponent));
  EXPECT_EQ(64, NEmpties(0, 0));
}

TEST(BitPattern, MoveToSquare) {
  EXPECT_EQ(MoveToSquare("h8"), 0);
  EXPECT_EQ(MoveToSquare("g8"), 1);
  EXPECT_EQ(MoveToSquare("h6"), 16);
  EXPECT_EQ(MoveToSquare("d4"), 36);
  EXPECT_EQ(MoveToSquare("a1"), 63);
}

TEST(BitPattern, SquareToMove) {
  for (int i = 0; i < 64; ++i) {
    ASSERT_EQ(i, MoveToSquare(SquareToMove(i)));
  }
}

TEST(BitPattern, VerticalMirror) {
  BitPattern p = ParsePattern("XX------"
                              "------XX"
                              "--------"
                              "--------"
                              "--------"
                              "--------"
                              "--------"
                              "--------");
  BitPattern q = ParsePattern("--------"
                              "--------"
                              "--------"
                              "--------"
                              "--------"
                              "--------"
                              "------XX"
                              "XX------");
  EXPECT_EQ(VerticalMirror(p), q);
  EXPECT_EQ(VerticalMirror(q), p);
}

TEST(BitPattern, Rotate) {
  BitPattern r1 = ParsePattern("XX------"
                               "--------"
                               "--------"
                               "--------"
                               "--------"
                               "--------"
                               "--------"
                               "--------");
  BitPattern r2 = ParsePattern("-------X"
                               "-------X"
                               "--------"
                               "--------"
                               "--------"
                               "--------"
                               "--------"
                               "--------");
  BitPattern r3 = ParsePattern("--------"
                               "--------"
                               "--------"
                               "--------"
                               "--------"
                               "--------"
                               "--------"
                               "------XX");
  BitPattern r4 = ParsePattern("--------"
                               "--------"
                               "--------"
                               "--------"
                               "--------"
                               "--------"
                               "X-------"
                               "X-------");
  EXPECT_EQ(Rotate(r1), r4);
  EXPECT_EQ(Rotate(r4), r3);
  EXPECT_EQ(Rotate(r3), r2);
  EXPECT_EQ(Rotate(r2), r1);
}

TEST(BitPattern, AllBitPatternTranspositions) {
  BitPattern r1 = ParsePattern("XX------"
                               "--------"
                               "--------"
                               "--------"
                               "--------"
                               "--------"
                               "--------"
                               "--------");
  BitPattern r2 = ParsePattern("-------X"
                               "-------X"
                               "--------"
                               "--------"
                               "--------"
                               "--------"
                               "--------"
                               "--------");
  BitPattern r3 = ParsePattern("--------"
                               "--------"
                               "--------"
                               "--------"
                               "--------"
                               "--------"
                               "--------"
                               "------XX");
  BitPattern r4 = ParsePattern("--------"
                               "--------"
                               "--------"
                               "--------"
                               "--------"
                               "--------"
                               "X-------"
                               "X-------");
  BitPattern r5 = ParsePattern("X-------"
                               "X-------"
                               "--------"
                               "--------"
                               "--------"
                               "--------"
                               "--------"
                               "--------");
  BitPattern r6 = ParsePattern("------XX"
                               "--------"
                               "--------"
                               "--------"
                               "--------"
                               "--------"
                               "--------"
                               "--------");
  BitPattern r7 = ParsePattern("--------"
                               "--------"
                               "--------"
                               "--------"
                               "--------"
                               "--------"
                               "-------X"
                               "-------X");
  BitPattern r8 = ParsePattern("--------"
                               "--------"
                               "--------"
                               "--------"
                               "--------"
                               "--------"
                               "--------"
                               "XX------");
  EXPECT_THAT(AllBitPatternTranspositions(r1),
              testing::UnorderedElementsAre(r1, r2, r3, r4, r5, r6, r7, r8));
}