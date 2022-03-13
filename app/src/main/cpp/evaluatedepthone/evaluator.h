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

#ifndef PATTERN_EVALUATOR_H
#define PATTERN_EVALUATOR_H

#include <array>
#include <cassert>
#include <math.h>
#include <sstream>
#include <string.h>
#include "pattern.h"
#include "../board/bitpattern.h"

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

constexpr int kMaxFeatureSize = 5;
constexpr int kNumFeatures = 26;
constexpr int kNumPatterns = 42;

constexpr std::array<BitPattern, kMaxFeatureSize> kFeatureDefinition[] = {
  {kCorner, kLastRowSmall, k2LastRowSmall, k3LastRowSmall, k4LastRowSmall},
  {kCorner4x4},
  {kDiagonalImproved},
  {kLastRowPattern << 16},
  {kLastRowPattern << 24},
  {kMainDiag7Pattern << 8},
  {kMainDiag7Pattern << 16},
};

constexpr int kNumEquivalentFeatures =
    std::end(kFeatureDefinition) - std::begin(kFeatureDefinition);

typedef int8_t PatternNumber ;
constexpr PatternNumber kMaxPatternsPerSquare = 8;

struct UpdatePattern {
  PatternNumber pattern_number;
  FeatureValue delta;
  FeatureValue delta_double;

  constexpr UpdatePattern() : pattern_number(0), delta(0), delta_double(0) {}
};

struct UpdatePatterns {
  PatternNumber num_patterns;
  UpdatePattern pattern[kMaxPatternsPerSquare];
  constexpr UpdatePatterns() : num_patterns(0), pattern() {}
};

// TODO: Cleanup.
struct Features {
  UpdatePatterns square_to_update_patterns[kNumSquares];
  FeatureValue max_pattern_value[kNumPatterns];
  int canonical_rotation[kNumFeatures];
  FeatureValue max_feature_value[kNumFeatures];
  int features_to_patterns[kNumFeatures][kMaxFeatureSize + 1];
  Pattern patterns[kNumPatterns];
  Pattern equivalent_features[kNumFeatures][kMaxPatternTranspositions][
      kMaxFeatureSize] = {};

  static constexpr void EquivalentTransposition(
      const std::array<BitPattern, kMaxFeatureSize>& feature,
      int result[kMaxPatternTranspositions]) {
    Pattern transpositions[kMaxPatternTranspositions];
    for (int t = 0; t < kMaxPatternTranspositions; ++t) {
      result[t] = t;
      for (int u = 0; u < t; ++u) {
        bool different = false;
        for (int i = 0; i < kMaxFeatureSize; ++i) {
          if (feature[i] == 0) {
            break;
          }
          Pattern::AllTranspositions(feature[i], transpositions);
          different = different || transpositions[t].ToBitPattern() !=
              transpositions[u].ToBitPattern();
          if (different) {
            break;
          }
        }
        if (!different) {
          result[t] = u;
          break;
        }
      }
    }
  }

  constexpr Features() :
      square_to_update_patterns(),
      max_pattern_value(),
      canonical_rotation(),
      max_feature_value(),
      features_to_patterns(),
      patterns(),
      equivalent_features() {
    int num_features = 0;
    int num_patterns = 0;
    int num_canonical_rotations = 0;
    Pattern transpositions[kMaxPatternTranspositions];
    // Equivalent = same squares in different order.
    int equivalent_transposition[kMaxPatternTranspositions] = {};

    for (const auto &feature : kFeatureDefinition) {
      EquivalentTransposition(feature, equivalent_transposition);
      for (int t = 0; t < kMaxPatternTranspositions; ++t) {
        if (equivalent_transposition[t] != t) { continue; }
        int num_equivalent_features = 0;
        FeatureValue feature_size = 0;
        for (int i = 0; i < feature.size(); ++i) {
          if (feature[i] == 0) {
            features_to_patterns[num_features][i] = -1;
            break;
          }
          Pattern::AllTranspositions(feature[i], transpositions);
          Pattern pattern = transpositions[t];
          max_pattern_value[num_patterns] = pattern.MaxValue();
          features_to_patterns[num_features][i] = num_patterns;
          patterns[num_patterns] = pattern;
          if (i > 0) {
            feature_size += (max_pattern_value[num_patterns] + 1) *
                (max_pattern_value[num_patterns - 1] + 1);
          }
          for (int j = 0; j < pattern.Size(); ++j) {
            Square square = pattern.Squares()[j];
            UpdatePatterns
                *update_patterns = &square_to_update_patterns[square];
            UpdatePattern *update_pattern = &(
                update_patterns->pattern[update_patterns->num_patterns]);
            update_patterns->num_patterns++;
            update_pattern->pattern_number = num_patterns;
            update_pattern->delta = pattern.GetWeight(j);
            update_pattern->delta_double = 2 * update_pattern->delta;
          }
          ++num_patterns;
        }

        if (feature[1] == 0) {
          max_feature_value[num_features] = max_pattern_value[num_patterns - 1];
        } else {
          max_feature_value[num_features] = feature_size - 1;
        }
        canonical_rotation[num_features] = num_canonical_rotations;
        features_to_patterns[num_features][feature.size()] = -1;

        for (int u = t; u < kMaxPatternTranspositions; ++u) {
          if (equivalent_transposition[u] != t) { continue; }
          for (int i = 0; i < feature.size(); ++i) {
            Pattern::AllTranspositions(feature[i], transpositions);
            equivalent_features[num_features][num_equivalent_features][i] =
                transpositions[u];
          }
          num_equivalent_features++;
        }

        for (; num_equivalent_features < kMaxPatternTranspositions;
             ++num_equivalent_features) {
          equivalent_features[num_features][num_equivalent_features][0] =
              Pattern(0, {});
        }

        ++num_features;
      }
      num_canonical_rotations++;
    }
    assert (kNumEquivalentFeatures == num_canonical_rotations);
    assert (kNumPatterns == num_patterns);
    assert (kNumFeatures == num_features);
  }
};

constexpr Features kFeatures;

#define FOR_EACH_SET_BIT(pattern, name) \
  for (BitPattern name = pattern; name != 0; name = name & (name - 1))

class Evaluator {

 public:
  Evaluator() : empties_(0) {}

  void Setup(BitPattern player, BitPattern opponent);

  void Update(BitPattern square, BitPattern flip);

  void UndoUpdate(BitPattern square, BitPattern flip);

  void Invert();

  const FeatureValue* const GetPatterns() const { return patterns_; }

  int Empties() const { return empties_; }

  static FeatureValue GetFeature(
      std::array<FeatureValue, kMaxFeatureSize> pattern_values,
      std::array<Pattern, kMaxFeatureSize> patterns);

  static std::array<Pattern, kMaxFeatureSize> PatternsForFeature(int i);

  FeatureValue GetFeature(int i);

  static FeatureValue GetFeature(
      std::array<Pattern, kMaxFeatureSize> patterns,
      BitPattern player, BitPattern opponent);

  static FeatureValue GetFeature(int i, BitPattern player, BitPattern opponent);

  static FeatureValue GetCanonicalFeature(int i, BitPattern player, BitPattern opponent);

 private:
  FeatureValue patterns_[kNumPatterns];
  int empties_;
};

#endif /* PATTERN_EVALUATOR_H */