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

#include "pattern_evaluator.h"
#include "../utils/files.h"

void PatternEvaluator::Setup(BitPattern player, BitPattern opponent) {
  memset(patterns_, 0, sizeof(patterns_));
  empties_ = (int) __builtin_popcountll(~(player | opponent));
  FOR_EACH_SET_BIT(~player, remaining) {
    BitPattern square = remaining & (0 - remaining);
    const UpdatePatterns& update_patterns =
        kFeatures.square_to_update_patterns[__builtin_ctzll(square)];
    for (int i = 0; i < update_patterns.num_patterns; ++i) {
      const UpdatePattern& feature = update_patterns.pattern[i];
      patterns_[feature.pattern_number] +=
          ((square & opponent) == 0) ? feature.delta : feature.delta_double;
    }
  }
}

void PatternEvaluator::Invert() {
  for (int i = 0; i < kNumPatterns; ++i) {
    patterns_[i] = kFeatures.max_pattern_value[i] - patterns_[i];
  }
}

FeatureValue PatternEvaluator::GetFeature(
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

std::array<Pattern, kMaxFeatureSize> PatternEvaluator::PatternsForFeature(int i) {
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

FeatureValue PatternEvaluator::GetFeature(int i) const {
  std::array<FeatureValue, kMaxFeatureSize> pattern_values = {};
  const auto& feature_to_patterns = kFeatures.features_to_patterns[i];
  for (int j = 0; j < kMaxFeatureSize; ++j) {
    int pattern = feature_to_patterns[j];
    if (pattern == -1) {
      break;
    }
    pattern_values[j] = patterns_[pattern];
  }
  return GetFeature(pattern_values, PatternsForFeature(i));
}

FeatureValue PatternEvaluator::GetFeature(
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

FeatureValue PatternEvaluator::GetFeature(int i, BitPattern player, BitPattern opponent) {
  const auto& feature = kFeatures.equivalent_features[i][0];
  std::array<Pattern, kMaxFeatureSize> patterns;
  std::copy(std::begin(feature), std::end(feature), std::begin(patterns));
  return GetFeature(patterns, player, opponent);
}

FeatureValue PatternEvaluator::GetCanonicalFeature(int i, BitPattern player, BitPattern opponent) {
  FeatureValue result = kFeatures.max_feature_value[i] + 1;
  for (const auto& feature : kFeatures.equivalent_features[i]) {
    if (feature[0].Size() == 0) { break; }
    std::array<Pattern, kMaxFeatureSize> patterns;
    std::copy(std::begin(feature), std::end(feature), std::begin(patterns));
    result = std::min(result, GetFeature(patterns, player, opponent));
  }
  return result;
}

EvalLarge PatternEvaluator::Evaluate() const {
  int split = kFeatures.splits[empties_];
  const int8_t* const base_evals =
      evals_ + kFeatures.start_feature[kNumBaseRotations] * split;
  int eval = 0;

  if (patterns_[0] != 40) {
    eval += base_evals[patterns_[0] + patterns_[1] * 81];
  } else if (patterns_[1] != 364) {
    eval += base_evals[59049 + patterns_[1] + patterns_[2] * 729];
  } else if (patterns_[2] != 40) {
    eval += base_evals[118098 + patterns_[2] + patterns_[3] * 81];
  } else {
    eval += base_evals[124659 + patterns_[3] + patterns_[4] * 81];
  }

  if (patterns_[5] != 40) {
    eval += base_evals[patterns_[5] + patterns_[6] * 81];
  } else if (patterns_[6] != 364) {
    eval += base_evals[59049 + patterns_[6] + patterns_[7] * 729];
  } else if (patterns_[7] != 40) {
    eval += base_evals[118098 + patterns_[7] + patterns_[8] * 81];
  } else {
    eval += base_evals[124659 + patterns_[8] + patterns_[9] * 81];
  }

  if (patterns_[10] != 40) {
    eval += base_evals[patterns_[10] + patterns_[11] * 81];
  } else if (patterns_[11] != 364) {
    eval += base_evals[59049 + patterns_[11] + patterns_[12] * 729];
  } else if (patterns_[12] != 40) {
    eval += base_evals[118098 + patterns_[12] + patterns_[13] * 81];
  } else {
    eval += base_evals[124659 + patterns_[13] + patterns_[14] * 81];
  }

  if (patterns_[15] != 40) {
    eval += base_evals[patterns_[15] + patterns_[16] * 81];
  } else if (patterns_[16] != 364) {
    eval += base_evals[59049 + patterns_[16] + patterns_[17] * 729];
  } else if (patterns_[17] != 40) {
    eval += base_evals[118098 + patterns_[17] + patterns_[18] * 81];
  } else {
    eval += base_evals[124659 + patterns_[18] + patterns_[19] * 81];
  }
  eval += base_evals[131220 + patterns_[20]] + base_evals[131220 + patterns_[21]] +
      base_evals[662661 + patterns_[22]] + base_evals[662661 + patterns_[23]] +
      base_evals[662661 + patterns_[24]] + base_evals[662661 + patterns_[25]] +
      base_evals[2256984 + patterns_[26]] + base_evals[2256984 + patterns_[27]] +
      base_evals[2256984 + patterns_[28]] + base_evals[2256984 + patterns_[29]] +
      base_evals[2263545 + patterns_[30]] + base_evals[2263545 + patterns_[31]] +
      base_evals[2263545 + patterns_[32]] + base_evals[2263545 + patterns_[33]] +
      base_evals[2270106 + patterns_[34]] + base_evals[2270106 + patterns_[35]] +
      base_evals[2270106 + patterns_[36]] + base_evals[2270106 + patterns_[37]] +
      base_evals[2272293 + patterns_[38]] + base_evals[2272293 + patterns_[39]] +
      base_evals[2272293 + patterns_[40]] + base_evals[2272293 + patterns_[41]];
  return std::max(kMinEvalLarge, std::min(kMaxEvalLarge, eval));//getEvalAndSetupLastError(eval);
}


EvalType LoadEvals(std::string filepath) {
  return ReadFile<int8_t>(filepath);
}
