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
#include "../board/bitpattern.h"
#include "../board/board.h"
#include "../board/get_moves.h"
#include "pattern_evaluator.h"

using ::testing::ContainerEq;

class PatternEvaluatorTest : public testing::Test {
 protected:
  EvalType evals_ = LoadEvals();
};

TEST(PatternEvaluatorTest, BitPatternsAsExpected) {
  EXPECT_EQ(kFeatures.patterns[0].ToBitPattern(), kCorner);
  EXPECT_EQ(kFeatures.patterns[1].ToBitPattern(), kLastRowSmall);
  EXPECT_EQ(kFeatures.patterns[2].ToBitPattern(), k2LastRowSmall);
  EXPECT_EQ(kFeatures.patterns[20].ToBitPattern(), kDiagonalImproved);
}

TEST_F(PatternEvaluatorTest, Patterns) {
  PatternEvaluator evaluator(evals_.data());
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
            << Board(player, opponent) << "\n"
            <<*(visited_patterns[key]);
        ASSERT_EQ(key, kFeatures.patterns[i].GetValue(player, opponent));
        visited_patterns[key] = std::unique_ptr<Board>(new Board(player, opponent));
      }
    }
  }
}

TEST_F(PatternEvaluatorTest, Features) {
  PatternEvaluator evaluator(evals_.data());
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
              << Board(player, opponent) << "\n"
              << *(visited_patterns[key]);
          visited_patterns[key] = std::unique_ptr<Board>(new Board(player, opponent));
        }
      }
    }
  }
}

TEST(PatternEvaluatorTest, CanonicalRotation) {
  EXPECT_EQ(
    std::set<int>(std::begin(kFeatures.canonical_rotation),
                  std::end(kFeatures.canonical_rotation)).size(),
    kNumBaseRotations
  );
  for (int i = 0; i < kNumFeatures; ++i) {
    EXPECT_LT(kFeatures.canonical_rotation[i], kNumBaseRotations);
    for (int j = i+1; j < kNumFeatures; ++j) {
      if (kFeatures.canonical_rotation[i] == kFeatures.canonical_rotation[j]) {
        EXPECT_EQ(kFeatures.max_feature_value[i],
                  kFeatures.max_feature_value[j]);
      }
    }
  }
}

TEST(PatternEvaluatorTest, EquivalentTransposition) {
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

TEST(PatternEvaluatorTest, EquivalentFeatures) {
  EXPECT_THAT(kFeatures.equivalent_features[4][0], testing::ElementsAre(
      Pattern({0, 1, 8, 9, 18, 27, 36, 45, 54, 55, 62, 63}),
      Pattern({}),
      Pattern({}),
      Pattern({}),
      Pattern({})
  ));
  EXPECT_THAT(kFeatures.equivalent_features[4][1], testing::ElementsAre(
      Pattern({63, 62, 55, 54, 45, 36, 27, 18, 9, 8, 1, 0}),
      Pattern({}),
      Pattern({}),
      Pattern({}),
      Pattern({})
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

void AssertEvalsEQ(const PatternEvaluator& eval1, const PatternEvaluator& eval2) {
  for (int i = 0; i < kNumPatterns; ++i) {
    ASSERT_EQ(eval1.GetPatterns()[i], eval2.GetPatterns()[i]);
  }
  for (int i = 0; i < kNumFeatures; ++i) {
    ASSERT_EQ(eval1.GetFeature(i), eval2.GetFeature(i));
  }
  ASSERT_EQ(eval1.Empties(), eval2.Empties());
  ASSERT_EQ(eval1.Evaluate(), eval2.Evaluate());
  ASSERT_EQ(eval1.EvaluateBase<false>(), eval2.Evaluate());
}

TEST_F(PatternEvaluatorTest, UpdateAndUndo) {
  PatternEvaluator eval(evals_.data());;
  PatternEvaluator test(evals_.data());;
  for (int i = 0; i < 10000; ++i) {
    Board b = RandomBoard();
    std::vector<BitPattern> moves = GetAllMoves(b.Player(), b.Opponent());
    if (moves.empty()) {
      continue;
    }
    eval.Setup(b.Player(), b.Opponent());
    eval.Invert();
    ASSERT_EQ(eval.Empties(), b.NEmpties());
    for (BitPattern flip : moves) {
      BitPattern square = SquareFromFlip(flip, b.Player(), b.Opponent());
      Board after(b.Player(), b.Opponent());
      after.PlayMove(flip);
      if (flip != 0) {
        eval.Update(square, flip);
      }
      test.Setup(after.Player(), after.Opponent());
      AssertEvalsEQ(eval, test);
      if (flip != 0) {
        eval.UndoUpdate(square, flip);
      }
    }
    eval.Invert();
    test.Setup(b.Player(), b.Opponent());
    AssertEvalsEQ(eval, test);
  }
}