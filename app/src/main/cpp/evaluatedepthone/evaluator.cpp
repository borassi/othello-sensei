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

#include "evaluator.h"

void Evaluator::Setup(BitPattern player, BitPattern opponent) {
  memset(patterns_, 0, sizeof(patterns_));
  for (int i = 0; i < kNumPatterns; ++i) {
    patterns_[i] = kFeatures.patterns[i].GetValue(player, opponent);
  }
  empties_ = kNumSquares - __builtin_popcountll(player | opponent);
}

void Evaluator::Update(BitPattern square, BitPattern flip) {
  assert(__builtin_popcountll(square) == 1);
  empties_--;
  const UpdatePatterns& update_patterns =
      kFeatures.square_to_update_patterns[__builtin_ctzll(square)];
  for (int i = 0; i < update_patterns.num_patterns; ++i) {
    const UpdatePattern& pattern = update_patterns.pattern[i];
    patterns_[pattern.pattern_number] += pattern.delta;
  }
  FOR_EACH_SET_BIT(flip & ~square, remaining) {
    const UpdatePatterns& update_patterns =
        kFeatures.square_to_update_patterns[__builtin_ctzll(remaining)];
    for (int i = 0; i < update_patterns.num_patterns; ++i) {
      const UpdatePattern& pattern = update_patterns.pattern[i];
      patterns_[pattern.pattern_number] += pattern.delta_double;
    }
  }
}

void Evaluator::UndoUpdate(BitPattern square, BitPattern flip) {
  assert(__builtin_popcountll(square) == 1);
  empties_--;
  const UpdatePatterns& update_patterns =
      kFeatures.square_to_update_patterns[__builtin_ctzll(square)];
  for (int i = 0; i < update_patterns.num_patterns; ++i) {
    const UpdatePattern& pattern = update_patterns.pattern[i];
    patterns_[pattern.pattern_number] -= pattern.delta;
  }
  FOR_EACH_SET_BIT(flip & ~square, remaining) {
    const UpdatePatterns& update_patterns =
        kFeatures.square_to_update_patterns[__builtin_ctzll(remaining)];
    for (int i = 0; i < update_patterns.num_patterns; ++i) {
      const UpdatePattern& pattern = update_patterns.pattern[i];
      patterns_[pattern.pattern_number] -= pattern.delta_double;
    }
  }
}

void Evaluator::Invert() {
  for (int i = 0; i < kNumPatterns; ++i) {
    patterns_[i] = kFeatures.max_pattern_value[i] - patterns_[i];
  }
}

FeatureValue Evaluator::GetFeature(
    std::array<FeatureValue, kMaxFeatureSize> pattern_values,
    std::array<Pattern, kMaxFeatureSize> patterns) {
  FeatureValue result = 0;
  int i = 0;
  while (
      pattern_values[i] * 2 == patterns[i].MaxValue() &&
      patterns[i + 1].Size() != 0 && i + 2 < kMaxFeatureSize &&
      patterns[i + 2].Size() != 0) {
    result += (patterns[i].MaxValue() + 1) * (patterns[i + 1].MaxValue() + 1);
    ++i;
  }
  if (patterns[i + 1].Size() == 0) {
    result += pattern_values[i];
  } else {
    result += pattern_values[i] +
              (patterns[i].MaxValue() + 1) * pattern_values[i + 1];
  }
  return result;
}

std::array<Pattern, kMaxFeatureSize> Evaluator::PatternsForFeature(int i) {
  std::array<Pattern, kMaxFeatureSize> result;
  const auto& feature_to_patterns = kFeatures.features_to_patterns[i];
  for (int j = 0; j < kMaxFeatureSize; ++j) {
    if (feature_to_patterns[j] == -1) {
      result[j] = Pattern(0, {});
      break;
    }
    result[j] = kFeatures.patterns[feature_to_patterns[j]];
  }
  return result;
}

FeatureValue Evaluator::GetFeature(int i) const {
  std::array<FeatureValue, kMaxFeatureSize> pattern_values = {};
  const auto& feature_to_patterns = kFeatures.features_to_patterns[i];
  for (int j = 0; j < kMaxFeatureSize; ++j) {
    if (feature_to_patterns[j] == -1) {
      break;
    }
    pattern_values[j] = patterns_[feature_to_patterns[j]];
  }
  return GetFeature(pattern_values, PatternsForFeature(i));
}

FeatureValue Evaluator::GetFeature(
    std::array<Pattern, kMaxFeatureSize> patterns,
    BitPattern player, BitPattern opponent) {
  std::array<FeatureValue, kMaxFeatureSize> pattern_values = {};
  for (int j = 0; j < kMaxFeatureSize; ++j) {
    if (patterns[j].Size() == 0) {
      break;
    }
    pattern_values[j] = patterns[j].GetValue(player, opponent);
  }
  return GetFeature(pattern_values, patterns);
}

FeatureValue Evaluator::GetFeature(int i, BitPattern player, BitPattern opponent) {
  const auto& feature = kFeatures.equivalent_features[i][0];
  std::array<Pattern, kMaxFeatureSize> patterns;
  std::copy(std::begin(feature), std::end(feature), std::begin(patterns));
  return GetFeature(patterns, player, opponent);
}

FeatureValue Evaluator::GetCanonicalFeature(int i, BitPattern player, BitPattern opponent) {
  FeatureValue result = kFeatures.max_feature_value[i] + 1;
  for (const auto& feature : kFeatures.equivalent_features[i]) {
    if (feature[0].Size() == 0) { break; }
    std::array<Pattern, kMaxFeatureSize> patterns;
    std::copy(std::begin(feature), std::end(feature), std::begin(patterns));
    result = std::min(result, GetFeature(patterns, player, opponent));
  }
  return result;
}

int8_t* LoadEvals(const std::string& filepath) {
  std::ifstream file;
  file.open(filepath, std::ios_base::binary | std::ios_base::in);
  assert (file.is_open());
  int num_splits;
  file.read((char*) &num_splits, sizeof(num_splits));
  assert (num_splits == kSplits);
  std::vector<int8_t> result;
  for (int i = 0; i < num_splits; ++i) {
    int num_features;
    file.read((char*) &num_features, sizeof(num_features));
    assert (num_features == kNumBaseRotations);
    for (int j = 0; j < num_features; ++j) {
      int feature_size;
      file.read((char*) &feature_size, sizeof(feature_size));
      auto tmp = new int8_t[feature_size];
      file.read((char*) tmp, feature_size);
      result.reserve(result.size() + feature_size);
      std::copy(&tmp[0], &tmp[feature_size], std::back_inserter(result));
      delete[] tmp;
    }
  }

  auto result_array = (int8_t*) malloc(result.size() * sizeof(int8_t));
  memcpy(result_array, &result[0], result.size() * sizeof(int8_t));
  file.close();
  return result_array;
}

int8_t* Evaluator::evals_ = LoadEvals(
    "src/main/assets/coefficients/pattern_evaluator_cpp.dat");