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
#include <fstream>
#include <math.h>
#include <memory>
#include <sstream>
#include <string.h>
#include "pattern.h"
#include "evaluator_depth_one_base.h"
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
constexpr int kSplits = 10;

constexpr std::array<BitPattern, kMaxFeatureSize> kFeatureDefinition[] = {
  {kCorner, kLastRowSmall, k2LastRowSmall, k3LastRowSmall, k4LastRowSmall},
  {kDiagonalImproved},
  {kCorner4x4},
  {kLastRowPattern << 16},
  {kLastRowPattern << 24},
  {kMainDiag7Pattern << 8},
  {kMainDiag7Pattern << 16},
};

constexpr int kNumBaseRotations =
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

constexpr int GetSplit(int empties, int num_splits) {
  return (empties - 1) * num_splits / 60;
}

template<int num_splits>
constexpr int GetSplit(int empties) {
  return GetSplit(empties, num_splits);
}

// TODO: Cleanup.
struct Features {
  UpdatePatterns square_to_update_patterns[kNumSquares];
  FeatureValue max_pattern_value[kNumPatterns];
  int canonical_rotation[kNumFeatures];
  FeatureValue max_feature_value[kNumFeatures];
  FeatureValue start_feature[kNumBaseRotations + 1];
  int features_to_patterns[kNumFeatures][kMaxFeatureSize + 1];
  Pattern patterns[kNumPatterns];
  Pattern equivalent_features[kNumFeatures][kMaxPatternTranspositions][
      kMaxFeatureSize] = {};
  int splits[60];

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
      start_feature(),
      features_to_patterns(),
      patterns(),
      equivalent_features(),
      splits() {
    int num_features = 0;
    int num_patterns = 0;
    int num_canonical_rotations = 0;
    Pattern transpositions[kMaxPatternTranspositions];
    // Equivalent = same squares in different order.
    int equivalent_transposition[kMaxPatternTranspositions] = {};
    start_feature[0] = 0;

    for (int i = 0; i < 60; ++i) {
      splits[i] = GetSplit(i, kSplits);
    }

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
      start_feature[num_canonical_rotations + 1] =
          1 + max_feature_value[num_features - 1] +
          start_feature[num_canonical_rotations];
      num_canonical_rotations++;
    }
    assert (kNumBaseRotations == num_canonical_rotations);
    assert (kNumPatterns == num_patterns);
    assert (kNumFeatures == num_features);
  }
};

constexpr Features kFeatures;

class PatternEvaluator : public EvaluatorDepthOneBase {

 public:
  PatternEvaluator() : empties_(0) {}
  ~PatternEvaluator() {}

  static std::unique_ptr<EvaluatorDepthOneBase> Create() {
    return std::make_unique<PatternEvaluator>(PatternEvaluator());
  }

  void Setup(BitPattern player, BitPattern opponent);

  void Update(BitPattern square, BitPattern flip) {
    Update<1>(square, flip);
  }

  void UndoUpdate(BitPattern square, BitPattern flip) {
    Update<-1>(square, flip);
  }

  void Invert();

  const FeatureValue* const GetPatterns() const { return patterns_; }

  int Empties() const { return empties_; }

  template<bool verbose>
  EvalLarge EvaluateBase() const {
    int split = GetSplit<kSplits>(empties_);
    int8_t* base_evals =
        evals_ + kFeatures.start_feature[kNumBaseRotations] * split;
    EvalLarge result = 0;

    for (int i = 0; i < kNumFeatures; ++i) {
      int canonical_rotation = kFeatures.canonical_rotation[i];
      int8_t* evals = base_evals + kFeatures.start_feature[canonical_rotation];
      result += evals[GetFeature(i)];
      if (verbose) {
        std::cout << i << " " << GetFeature(i) << " "
                  << evals[GetFeature(i)] / 8.0 << "\n";
      }
    }
    return result;
  }

  EvalLarge Evaluate() const;

  static FeatureValue GetFeature(
      std::array<FeatureValue, kMaxFeatureSize> pattern_values,
      std::array<Pattern, kMaxFeatureSize> patterns);

  static std::array<Pattern, kMaxFeatureSize> PatternsForFeature(int i);

  FeatureValue GetFeature(int i) const;

  static FeatureValue GetFeature(
      std::array<Pattern, kMaxFeatureSize> patterns,
      BitPattern player, BitPattern opponent);

  static FeatureValue GetFeature(int i, BitPattern player, BitPattern opponent);

  static FeatureValue GetCanonicalFeature(int i, BitPattern player, BitPattern opponent);

 private:
  template<int multiplier>
  void Update(BitPattern square, BitPattern flip);

  static int8_t* evals_;
  FeatureValue patterns_[kNumPatterns];
  int empties_;
};

#endif /* PATTERN_EVALUATOR_H */