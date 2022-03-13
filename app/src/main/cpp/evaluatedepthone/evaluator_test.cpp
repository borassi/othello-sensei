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
#include "evaluator.h"

using ::testing::ContainerEq;

TEST(PatternEvaluator, BitPatternsAsExpected) {
  EXPECT_EQ(kFeatures.patterns[0].ToBitPattern(), kCorner);
  EXPECT_EQ(kFeatures.patterns[1].ToBitPattern(), kLastRowSmall);
  EXPECT_EQ(kFeatures.patterns[2].ToBitPattern(), k2LastRowSmall);
  EXPECT_EQ(kFeatures.patterns[20].ToBitPattern(), kDiagonalImproved);
}

TEST(PatternEvaluator, Patterns) {
  Evaluator evaluator;
  for (int i = 0; i < kNumPatterns; ++i) {
    std::map<FeatureValue, std::unique_ptr<Board>> visited_patterns;
    const BitPattern pattern = kFeatures.patterns[i].ToBitPattern();
    if (__builtin_popcountll(pattern) >= 10) {
      continue;
    }
    for (BitPattern player : AllSubBitPatterns(pattern)) {
      for (BitPattern opponent : AllSubBitPatterns(pattern & ~player)) {
        evaluator.Setup(player, opponent);
        FeatureValue key = evaluator.GetPatterns()[i];
        ASSERT_LE(key, kFeatures.max_pattern_value[i]);
        // We do it like this because the testing::Contains is too slow.
        ASSERT_TRUE(visited_patterns.find(key) == visited_patterns.end())
            << "Found the pattern " << key << " twice:\n"
            << Board(player, opponent).ToString() << "\n"
            << visited_patterns[key]->ToString();
        ASSERT_EQ(key, kFeatures.patterns[i].GetValue(player, opponent));
        visited_patterns[key] = std::unique_ptr<Board>(new Board(player, opponent));
      }
    }
  }
}

TEST(PatternEvaluator, Features) {
  Evaluator evaluator;
  for (int i = 0; i < kNumFeatures; ++i) {
    std::map<int, std::unique_ptr<Board>> visited_patterns;
    for (int j = 0; kFeatures.features_to_patterns[i][j+1] != -1; ++j) {
      const BitPattern cur_pattern = kFeatures.patterns[kFeatures.features_to_patterns[i][j]].ToBitPattern();
      const BitPattern next_pattern = kFeatures.patterns[kFeatures.features_to_patterns[i][j + 1]].ToBitPattern();
      const BitPattern pattern = cur_pattern | next_pattern;

      for (BitPattern player : AllSubBitPatterns(pattern)) {
        for (BitPattern opponent : AllSubBitPatterns(pattern & ~player)) {
          if (((player | opponent) & cur_pattern) == 0) {
            continue;
          }
          evaluator.Setup(player, opponent);
          FeatureValue key = evaluator.GetFeature(i);
          ASSERT_EQ(evaluator.GetFeature(i),
                    evaluator.GetFeature(evaluator.PatternsForFeature(i), player, opponent));
          ASSERT_LE(key, kFeatures.max_feature_value[i]);
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
}

TEST(PatternEvaluator, CanonicalRotation) {
  EXPECT_EQ(
    std::set<int>(std::begin(kFeatures.canonical_rotation),
                  std::end(kFeatures.canonical_rotation)).size(),
    kNumEquivalentFeatures
  );
  for (int i = 0; i < kNumFeatures; ++i) {
    EXPECT_LT(kFeatures.canonical_rotation[i], kNumEquivalentFeatures);
    for (int j = i+1; j < kNumFeatures; ++j) {
      if (kFeatures.canonical_rotation[i] == kFeatures.canonical_rotation[j]) {
        EXPECT_EQ(kFeatures.max_feature_value[i],
                  kFeatures.max_feature_value[j]);
      }
    }
  }
}

TEST(PatternEvaluator, EquivalentTransposition) {
  BitPattern p = 1;
  int equivalent[kMaxPatternTranspositions];
  std::array<BitPattern, kMaxFeatureSize> feature = {1, 0, 0, 0, 0};
  Features::EquivalentTransposition(feature, equivalent);
  EXPECT_THAT(equivalent, testing::ElementsAre(0, 1, 2, 3, 1, 2, 3, 0));

  feature[1] = 2;
  Features::EquivalentTransposition(feature, equivalent);
  EXPECT_THAT(equivalent, testing::ElementsAre(0, 1, 2, 3, 4, 5, 6, 7));

  feature = {1 | (1ULL << 7) | (1ULL << 63) | (1ULL << 56)};
  Features::EquivalentTransposition(feature, equivalent);
  EXPECT_THAT(equivalent, testing::ElementsAre(0, 0, 0, 0, 0, 0, 0, 0));
}

TEST(PatternEvaluator, EquivalentFeatures) {
  EXPECT_THAT(kFeatures.equivalent_features[4][0], testing::ElementsAre(
      Pattern(12, (Square[]) {0, 1, 8, 9, 18, 27, 36, 45, 54, 55, 62, 63}),
      Pattern(0, {}),
      Pattern(0, {}),
      Pattern(0, {}),
      Pattern(0, {})
  ));
  EXPECT_THAT(kFeatures.equivalent_features[4][1], testing::ElementsAre(
      Pattern(12, (Square[]) {63, 62, 55, 54, 45, 36, 27, 18, 9, 8, 1, 0}),
      Pattern(0, {}),
      Pattern(0, {}),
      Pattern(0, {}),
      Pattern(0, {})
  ));
  EXPECT_GT(kFeatures.equivalent_features[0][1][0].Size(), 0);
  EXPECT_EQ(kFeatures.equivalent_features[0][2][0].Size(), 0);
  EXPECT_GT(kFeatures.equivalent_features[4][3][0].Size(), 0);
  EXPECT_EQ(kFeatures.equivalent_features[4][4][0].Size(), 0);
  for (int i = 0; i < kNumFeatures; ++i) {
    const auto& feature = kFeatures.features_to_patterns[i];
    for (int j = 0; j < kMaxPatternTranspositions; ++j) {
      const auto& equivalent_feature = kFeatures.equivalent_features[i][j];
      if (equivalent_feature[0] == Pattern(0, {})) {
        continue;
      }
      for (int k = 0; feature[k] != -1; ++k) {
        ASSERT_EQ(kFeatures.patterns[feature[k]].ToBitPattern(),
                  equivalent_feature[k].ToBitPattern())
            << i << " " << j << " " << k << "\n"
            << PatternToString(kFeatures.patterns[feature[k]].ToBitPattern())
            << "\n"
            << PatternToString(equivalent_feature[k].ToBitPattern());
      }
    }
  }
}

TEST(TestEvaluator, UpdateAndUndo) {
  Evaluator eval;
  Evaluator test;
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
    for (int i = 0; i < kNumPatterns; ++i) {
      ASSERT_EQ(eval.GetPatterns()[i], test.GetPatterns()[i]);
    }
    eval.UndoUpdate(1ULL << move, flip);
    eval.Invert();
    test.Setup(b.GetPlayer(), b.GetOpponent());
    for (int i = 0; i < kNumPatterns; ++i) {
      ASSERT_EQ(eval.GetPatterns()[i], test.GetPatterns()[i]);
    }
  }
}