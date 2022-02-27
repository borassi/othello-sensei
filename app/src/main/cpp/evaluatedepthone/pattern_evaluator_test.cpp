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

#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include "../board/bitpattern.h"
#include "../board/board.h"
#include "../board/get_moves.h"
#include "pattern_evaluator.h"

using ::testing::ContainerEq;

TEST(PatternEvaluator, BitPatternsAsExpected) {
  EXPECT_EQ(kSquareToFeatures.base_patterns[0], kCorner);
  EXPECT_EQ(kSquareToFeatures.base_patterns[4], kLastRowSmall);
  EXPECT_EQ(kSquareToFeatures.base_patterns[20], kDiagonalImproved);
}

TEST(PatternEvaluator, Base) {
  PatternEvaluator evaluator;
  for (int i = 0; i < kNumBaseFeatures; ++i) {
    std::map<int, std::unique_ptr<Board>> visited_patterns;
    const BitPattern pattern = kSquareToFeatures.base_patterns[i];
    if (__builtin_popcountll(pattern) >= 10) {
      continue;
    }
    for (BitPattern player : AllSubBitPatterns(pattern)) {
      for (BitPattern opponent : AllSubBitPatterns(pattern & ~player)) {
        evaluator.Setup(player, opponent);
        FeatureValue key = evaluator.GetBaseFeatures()[i];
        ASSERT_LE(key, kSquareToFeatures.max_feature_size[i]);
        // We do it like this because the testing::Contains is too slow.
        ASSERT_TRUE(visited_patterns.find(key) == visited_patterns.end())
            << "Found the pattern " << key << " twice:\n"
            << Board(player, opponent).ToString() << "\n"
            << visited_patterns[key]->ToString();
        visited_patterns[key] = std::unique_ptr<Board>(new Board(player, opponent));
      }
    }
  }
}

TEST(TestEvaluator, UpdateAndUndo) {
  PatternEvaluator eval;
  PatternEvaluator test;
  for (int i = 0; i < 10000; ++i) {
    Board b = RandomBoard();
    std::vector<BitPattern> moves = GetAllMoves(b.GetPlayer(), b.GetOpponent());
    if (moves.empty()) {
      continue;
    }
    BitPattern flip = moves[rand() % moves.size()];
    int move = SquareFromFlip(flip, b.GetPlayer(), b.GetOpponent());
    eval.Setup(b.GetPlayer(), b.GetOpponent());
    if (flip == 0) {
      continue;
    }
    eval.Invert();
    eval.Update(1ULL << move, flip);
    Board after(b.GetPlayer(), b.GetOpponent());
    after.PlayMove(flip);
    test.Setup(after.GetPlayer(), after.GetOpponent());
    for (int i = 0; i < kNumBaseFeatures; ++i) {
      ASSERT_EQ(eval.GetBaseFeatures()[i], test.GetBaseFeatures()[i]);
    }
    eval.UndoUpdate(1ULL << move, flip);
    eval.Invert();
    test.Setup(b.GetPlayer(), b.GetOpponent());
    for (int i = 0; i < kNumBaseFeatures; ++i) {
      ASSERT_EQ(eval.GetBaseFeatures()[i], test.GetBaseFeatures()[i]);
    }
  }
}