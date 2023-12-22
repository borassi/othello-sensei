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
#include "pattern.h"

BitPattern RandomBitPattern() {
  while (true) {
    BitPattern pattern = RandomPattern(0.1);
    if (__builtin_popcountll(pattern) < kMaxPatternSize) {
      return pattern;
    }
  }
}

TEST(PatternTest, FromToBitPattern) {
  for (int i = 0; i < 10000; ++i) {
    BitPattern pattern = RandomBitPattern();
    Pattern f = Pattern(pattern);
    EXPECT_EQ(pattern, f.ToBitPattern());
  }
}

TEST(PatternTest, HorizMirrorPattern) {
  EXPECT_EQ(Pattern({7, 4, 21, 63}).HorizMirror(),
            Pattern({0, 3, 18, 56}));
  EXPECT_EQ(Pattern({56, 50, 0}).HorizMirror(),
            Pattern({63, 53, 7}));

  for (int i = 0; i < 10000; ++i) {
    Pattern f = Pattern(RandomBitPattern());
    Pattern new_f = f;
    for (int j = 0; j < 2; ++j) {
      new_f = new_f.HorizMirror();
    }
    EXPECT_EQ(f, new_f);
  }
}

TEST(PatternTest, RotatePattern) {
  EXPECT_EQ(Pattern({0, 7, 63, 56}).Rotate(), Pattern({7, 63, 56, 0}));
  EXPECT_EQ(Pattern({1, 6, 61}).Rotate(), Pattern({15, 55, 40}));
  EXPECT_EQ(Pattern({18, 19}).Rotate(), Pattern({21, 29}));

  for (int i = 0; i < 10000; ++i) {
    Pattern f = Pattern(RandomBitPattern());
    Pattern new_f = f;
    for (int j = 0; j < 4; ++j) {
      new_f = new_f.Rotate();
    }
    EXPECT_EQ(f, new_f);
  }
}

TEST(PatternTest, AllTranspositions) {
  EXPECT_THAT(Pattern::AllTranspositions(1), testing::ElementsAre(
      Pattern(1), Pattern(1ULL << 7), Pattern(1ULL << 63), Pattern(1ULL << 56),
      Pattern(1ULL << 7), Pattern(1ULL << 63), Pattern(1ULL << 56), Pattern(1)));

  EXPECT_THAT(Pattern::AllTranspositions(3), testing::UnorderedElementsAre(
      Pattern({0, 1}),
      Pattern({7, 15}),
      Pattern({63, 62}),
      Pattern({56, 48}),
      Pattern({7, 6}),
      Pattern({63, 55}),
      Pattern({56, 57}),
      Pattern({0, 8})));

  BitPattern center = ParsePattern(
    "--------"
    "--------"
    "--------"
    "---XX---"
    "---XX---"
    "--------"
    "--------"
    "--------");

  EXPECT_THAT(Pattern::AllTranspositions(center), testing::UnorderedElementsAre(
    Pattern({27, 28, 35, 36}),
      Pattern({28, 27, 36, 35}),
      Pattern({35, 27, 36, 28}), // Rotate 90
      Pattern({27, 35, 28, 36}),
      Pattern({36, 35, 28, 27}), // Rotate 180
      Pattern({35, 36, 27, 28}),
      Pattern({28, 36, 27, 35}), // Rotate 270
      Pattern({36, 28, 35, 27})
  ));

  BitPattern diag = ParsePattern(
    "--------"
    "--------"
    "--------"
    "---X----"
    "----X---"
    "--------"
    "--------"
    "--------");
  EXPECT_THAT(Pattern::AllTranspositions(diag), testing::UnorderedElementsAre(
      Pattern({27, 36}),
      Pattern({36, 27}),
      Pattern({28, 35}),
      Pattern({35, 28}),
      Pattern({27, 36}),
      Pattern({36, 27}),
      Pattern({28, 35}),
      Pattern({35, 28})));
}

TEST(PatternTest, GetValue) {
  Pattern patterns[kMaxPatternTranspositions];

  for (int i = 0; i < 10; ++i) {
    std::map<FeatureValue, std::unique_ptr<Board>> visited_patterns;
    Pattern pattern = Pattern(RandomBitPattern());
    FeatureValue pattern_size = Pow(3, pattern.Size());
    for (BitPattern player : AllSubBitPatterns(pattern.ToBitPattern())) {
      for (BitPattern opponent : AllSubBitPatterns(pattern.ToBitPattern() & ~player)) {
        FeatureValue key = pattern.GetValue(player, opponent);
        ASSERT_LT(key, pattern_size);
        // We do it like this because the testing::Contains is too slow.
        ASSERT_TRUE(visited_patterns.find(key) == visited_patterns.end())
            << "Found the pattern " << key << " twice:\n"
            << Board(player, opponent) << "\n"
            << *(visited_patterns[key]);
        visited_patterns[key] = std::unique_ptr<Board>(new Board(player, opponent));
      }
    }
    EXPECT_EQ(pattern_size, visited_patterns.size());
  }
}