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

#include <array>
#include <math.h>
#include <sstream>
#include <string.h>
#include "feature.h"
#include "../board/bitpattern.h"

typedef int FeatureValue;

constexpr BitPattern kCorner4x4 = ParsePattern("--------"
                                     "--------"
                                     "--------"
                                     "--------"
                                     "------XX"
                                     "-----XXX"
                                     "----XXXX"
                                     "----XXXX");

constexpr BitPattern kCornerSided = ParsePattern("--------"
                                       "--------"
                                       "--------"
                                       "-------X"
                                       "-------X"
                                       "-------X"
                                       "------XX"
                                       "---XXXXX");

constexpr BitPattern kDiagonalImproved = ParsePattern("XX------"
                                             "XX------"
                                             "--X-----"
                                             "---X----"
                                             "----X---"
                                             "-----X--"
                                             "------XX"
                                             "------XX");

constexpr BitPattern kCorner = ParsePattern("--------"
                                  "--------"
                                  "--------"
                                  "--------"
                                  "--------"
                                  "--------"
                                  "-X----X-"
                                  "X------X");

constexpr BitPattern kLastRowSmall = ParsePattern("--------"
                                  "--------"
                                  "--------"
                                  "--------"
                                  "--------"
                                  "--------"
                                  "--------"
                                  "-XXXXXX-");

constexpr BitPattern k2LastRowSmall = ParsePattern("--------"
                                        "--------"
                                        "--------"
                                        "--------"
                                        "--------"
                                        "--------"
                                        "--XXXX--"
                                        "--------");

constexpr BitPattern k3LastRowSmall = ParsePattern("--------"
                                         "--------"
                                         "--------"
                                         "--------"
                                         "--------"
                                         "--XXXX--"
                                         "--------"
                                         "--------");

constexpr BitPattern k4LastRowSmall = ParsePattern("--------"
                                         "--------"
                                         "--------"
                                         "--------"
                                         "--XXXX--"
                                         "--------"
                                         "--------"
                                         "--------");

constexpr BitPattern kCorner5x2 = ParsePattern("--------"
                                         "--------"
                                         "--------"
                                         "--------"
                                         "--------"
                                         "--------"
                                         "---XXXXX"
                                         "---XXXXX");

constexpr int kTotalBaseFeatures = 42;
constexpr int kMaxFeatureGroupSize = 5;

constexpr std::array<BitPattern, kMaxFeatureGroupSize> kFeatureGroups[] = {
  {kCorner, kLastRowSmall, k2LastRowSmall, k3LastRowSmall, k4LastRowSmall},
  {kDiagonalImproved},
  {kCorner4x4},
  {kLastRowPattern << 16},
  {kLastRowPattern << 24},
  {kMainDiag7Pattern << 8},
  {kMainDiag7Pattern << 16},
};

typedef int8_t FeatureNumber ;
constexpr FeatureNumber kMaxFeaturesPerSquare = 8;

struct UpdateFeature {
  FeatureNumber feature_number;
  FeatureValue delta;
  FeatureValue delta_double;

  constexpr UpdateFeature() : feature_number(0), delta(0), delta_double(0) {}
};

struct UpdateFeatures {
  FeatureNumber num_features;
  UpdateFeature feature[kMaxFeaturesPerSquare];
  constexpr UpdateFeatures() : num_features(0), feature() {}
};

constexpr int pow(FeatureValue x, int y) {
  int result = 1;
  for (; y > 0; y--) {
    result *= x;
  }
  return result;
}

constexpr int kNumBaseFeatures = 42;

struct SquareToFeatures {
  UpdateFeatures square_to_update_features[kNumSquares];
  int max_feature_size[kNumBaseFeatures];
  BitPattern base_patterns[kNumBaseFeatures];

  constexpr SquareToFeatures() : square_to_update_features(), max_feature_size(), base_patterns() {
    int num_base_features = 0;
    Feature features[8];
    for (const auto& feature_group : kFeatureGroups) {
      for (int i = 0; i < feature_group.size(); ++i) {
        BitPattern pattern = feature_group[i];
        if (pattern == 0) {
          break;
        }
        Feature::AllFeatures(pattern, features);
        for (Feature feature : features) {
          if (feature.size() == 0) {
            break;
          }
          max_feature_size[num_base_features] = pow(3, (int) feature.size()) - 1;
          base_patterns[num_base_features] = feature.ToBitPattern();
          for (int i = 0; i < feature.size(); ++i) {
            Square square = feature.squares()[i];
            UpdateFeatures* update_features = &square_to_update_features[square];
            UpdateFeature* update_feature = &(
                update_features->feature[update_features->num_features]);
            update_features->num_features++;
            update_feature->feature_number = num_base_features;
            update_feature->delta = pow(3, i);
            update_feature->delta_double = 2 * pow(3, i);
          }
          num_base_features++;
        }
      }
    }
    assert(kNumBaseFeatures == num_base_features);
  }
};

constexpr SquareToFeatures kSquareToFeatures;

#define FOR_EACH_SET_BIT(pattern, name) \
  for (BitPattern name = pattern; name != 0; name = name & (name - 1))

class PatternEvaluator {

 public:
  void Setup(BitPattern player, BitPattern opponent) {
    memset(base_features_, 0, sizeof(base_features_));

    empties_ = __builtin_popcountll(~(player | opponent));
    FOR_EACH_SET_BIT(~player, remaining) {
      BitPattern square = remaining & (-remaining);
      const UpdateFeatures& update_features = kSquareToFeatures.square_to_update_features[__builtin_ctzll(square)];
      for (int i = 0; i < update_features.num_features; ++i) {
        const UpdateFeature& feature = update_features.feature[i];

        base_features_[feature.feature_number] +=
            ((square & opponent) == 0) ? feature.delta :  feature.delta_double;
      }
    }
  }

  void Update(BitPattern square, BitPattern flip) {
    assert(__builtin_popcountll(square) == 1);
    empties_--;
    const UpdateFeatures& update_features =
        kSquareToFeatures.square_to_update_features[__builtin_ctzll(square)];
    for (int i = 0; i < update_features.num_features; ++i) {
      const UpdateFeature& feature = update_features.feature[i];
      base_features_[feature.feature_number] += feature.delta;
    }
    FOR_EACH_SET_BIT(flip & ~square, remaining) {
      const UpdateFeatures& update_features =
        kSquareToFeatures.square_to_update_features[__builtin_ctzll(remaining)];
      for (int i = 0; i < update_features.num_features; ++i) {
        const UpdateFeature& feature = update_features.feature[i];
        base_features_[feature.feature_number] += feature.delta_double;
      }
    }
  }

  void UndoUpdate(BitPattern square, BitPattern flip) {
    assert(__builtin_popcountll(square) == 1);
    empties_--;
    const UpdateFeatures& update_features =
      kSquareToFeatures.square_to_update_features[__builtin_ctzll(square)];
    for (int i = 0; i < update_features.num_features; ++i) {
      const UpdateFeature& feature = update_features.feature[i];
      base_features_[feature.feature_number] -= feature.delta;
    }
    FOR_EACH_SET_BIT(flip & ~square, remaining) {
      const UpdateFeatures& update_features =
        kSquareToFeatures.square_to_update_features[__builtin_ctzll(remaining)];
      for (int i = 0; i < update_features.num_features; ++i) {
        const UpdateFeature& feature = update_features.feature[i];
        base_features_[feature.feature_number] -= feature.delta_double;
      }
    }
  }

  void Invert() {
    for (int i = 0; i < kNumBaseFeatures; ++i) {
      base_features_[i] = kSquareToFeatures.max_feature_size[i] - base_features_[i];
    }
  }

  const FeatureValue* const GetBaseFeatures() const {
    return base_features_;
  }

 private:
  FeatureValue base_features_[kNumBaseFeatures];
  int empties_;
};