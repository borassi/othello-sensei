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
#include "feature.h"

BitPattern RandomFeaturePattern() {
  while (true) {
    BitPattern pattern = RandomPattern(0.1);
    if (__builtin_popcountll(pattern) < kMaxFeatureSize) {
      return pattern;
    }
  }
}

TEST(FeatureTest, FromToBitPattern) {
  for (int i = 0; i < 10000; ++i) {
    BitPattern pattern = RandomFeaturePattern();
    Feature f = Feature(pattern);
    EXPECT_EQ(pattern, f.ToBitPattern());
  }
}

TEST(FeatureTest, HorizMirrorFeature) {
  EXPECT_EQ(Feature(4, (Square[]) {7, 4, 21, 63}).HorizMirror(),
    Feature(4, (Square[]) {0, 3, 18, 56}));
  EXPECT_EQ(Feature(3, (Square[]) {56, 50, 0}).HorizMirror(),
    Feature(3, (Square[]) {63, 53, 7}));

  for (int i = 0; i < 10000; ++i) {
    Feature f = Feature(RandomFeaturePattern());
    Feature new_f = f;
    for (int j = 0; j < 2; ++j) {
      new_f = new_f.HorizMirror();
    }
    EXPECT_EQ(f, new_f);
  }
}

TEST(FeatureTest, RotateFeature) {
  EXPECT_EQ(Feature(4, (Square[]) {0, 7, 63, 56}).Rotate(),
    Feature(4, (Square[]) {7, 63, 56, 0}));
  EXPECT_EQ(Feature(3, (Square[]) {1, 6, 61}).Rotate(),
    Feature(3, (Square[]) {15, 55, 40}));
  EXPECT_EQ(Feature(2, (Square[]) {18, 19}).Rotate(),
    Feature(2, (Square[]) {21, 29}));

  for (int i = 0; i < 10000; ++i) {
    Feature f = Feature(RandomFeaturePattern());
    Feature new_f = f;
    for (int j = 0; j < 4; ++j) {
      new_f = new_f.Rotate();
    }
    EXPECT_EQ(f, new_f);
  }
}

TEST(FeatureTest, AllFeatures) {
  Feature features[kMaxFeatureTranspositions];
  Feature::AllFeatures(1, features);
  EXPECT_THAT(features, testing::ElementsAre(
    Feature(1), Feature(1ULL << 7), Feature(1ULL << 63), Feature(1ULL << 56),
    Feature(0), Feature(0), Feature(0), Feature(0)));

  Feature::AllFeatures(3, features);
  EXPECT_THAT(features, testing::UnorderedElementsAre(
      Feature(2, (Square[]) {0, 1}),
      Feature(2, (Square[]) {7, 15}),
      Feature(2, (Square[]) {63, 62}),
      Feature(2, (Square[]) {56, 48}),
      Feature(2, (Square[]) {7, 6}),
      Feature(2, (Square[]) {63, 55}),
      Feature(2, (Square[]) {56, 57}),
      Feature(2, (Square[]) {0, 8})));

  BitPattern center = ParsePattern(
    "--------"
    "--------"
    "--------"
    "---XX---"
    "---XX---"
    "--------"
    "--------"
    "--------");
  Feature::AllFeatures(center, features);
  EXPECT_THAT(features, testing::ElementsAre(
    Feature(4, (Square[]) {27, 28, 35, 36}), Feature(0), Feature(0),
    Feature(0), Feature(0), Feature(0), Feature(0), Feature(0)));

  BitPattern diag = ParsePattern(
    "--------"
    "--------"
    "--------"
    "---X----"
    "----X---"
    "--------"
    "--------"
    "--------");
  Feature::AllFeatures(diag, features);
  EXPECT_THAT(features, testing::ElementsAre(
    Feature(2, (Square[]) {27, 36}),
    Feature(2, (Square[]) {28, 35}),
    Feature(0), Feature(0), Feature(0), Feature(0), Feature(0), Feature(0)));
}