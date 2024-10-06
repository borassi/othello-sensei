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

#include <cmath>
#include "../utils/misc.h"
#include "pattern_evaluator.h"
#include "train_pattern_evaluator.h"


TrainingBoard::TrainingBoard(
    const EvaluatedBoard& b,
    const std::vector <std::vector<
        FeatureValue>>& feature_value_to_canonical) {
  PatternEvaluator p(nullptr);
  eval_ = b.GetEval();
  p.Setup(b.GetPlayer(), b.GetOpponent());
  for (int i = 0; i < kNumFeatures; ++i) {
    features_.push_back(
        feature_value_to_canonical[kFeatures.canonical_rotation[i]]
                                  [p.GetFeature(i)]);
  }
}

bool TrainingBoard::operator==(const TrainingBoard& other) const {
  if (Eval() != other.Eval()) {
    return false;
  }
  const auto& features = Features();
  const auto& other_features = other.Features();
  if (features.size() != other_features.size()) {
    return false;
  }
  for (int i = 0; i < features.size(); ++i) {
    if (features[i] != other_features[i]) {
      return false;
    }
  }
  return true;
}

std::size_t std::hash<TrainingBoard>::operator()(
    const TrainingBoard& f) const noexcept {
  std::size_t result = std::hash<int>{}(f.Eval());
  for (const FeatureValue& feature : f.Features()) {
    result = result ^ std::hash<Pattern>{}(feature);
  }
  return result;
}

void TrainingFeature::UpdateValue(float error, float learning_rate, float lambda) {
  assert (isfinite(error));
//  n_appearences_ = n_appearences_ == 100 ? 100 : n_appearences_ + 1;
  float update = error - lambda * value_;
  m_t_ = beta_1 * m_t_ + (1.0F - beta_1) * update;// * 120 / (20 + n_appearences_);
  value_ += 2 * learning_rate * m_t_;
  value_ = std::max(-15.0F, std::min(value_, 15.0F));
}

CategoricalRegression::CategoricalRegression(
    std::vector<FeatureValue> max_feature_value,
    std::vector<int> canonical_rotation) :
    max_feature_value_(std::move(max_feature_value)),
    canonical_rotation_(std::move(canonical_rotation)) {
  for (int i = 0; i < max_feature_value_.size(); ++i) {
    if (i != 0 && canonical_rotation_[i] == canonical_rotation_[i - 1]) {
      continue;
    }
    std::vector<TrainingFeature> cur_features(max_feature_value_[i] + 1);
    features_.push_back(cur_features);
  }
}

float CategoricalRegression::Eval(const TrainingBoard& b) {
  const std::vector<FeatureValue>& features = b.Features();
  float result = 0;
  for (int i = 0; i < features.size(); ++i) {
    assert (features[i] <= max_feature_value_[i]);
    result += features_[canonical_rotation_[i]][features[i]].GetValue();
  }
  assert (isfinite(result));
  return result;
}

float CategoricalRegression::Error(const TrainingBoard& b) {
  return (float) b.Eval() - Eval(b);
}

void CategoricalRegression::Train(
    std::vector<const TrainingBoard*> training_set_,
    float learning_rate, float lambda) {
  auto rng = std::default_random_engine{};
  std::shuffle(std::begin(training_set_), std::end(training_set_), rng);
  for (const TrainingBoard* const b : training_set_) {
    const std::vector<FeatureValue>& features = b->Features();
    float error = Error(*b);
    for (int f = 0; f < features.size(); ++f) {
      features_[canonical_rotation_[f]][features[f]]
          .UpdateValue(error, learning_rate, lambda);
    }
  }
}

float CategoricalRegression::Test(const std::vector<const TrainingBoard*>& test_set) {
  double total_error = 0;

  for (const TrainingBoard* b : test_set) {
    float error = Error(*b);
    total_error += error * error;
  }
  return (float) sqrt(total_error / (double) test_set.size());
}

void CategoricalRegression::Round() {
  for (auto& feature : features_) {
    for (auto& value : feature) {
      value.Round();
    }
  }
}

CategoricalRegressions::CategoricalRegressions(
    int num_splits,
    std::vector<EvaluatedBoard> test_set,
    int max_num_boards) :
    max_num_boards(max_num_boards) {
  std::vector<FeatureValue> max_feature_value(
      std::begin(kFeatures.max_feature_value),
      std::end(kFeatures.max_feature_value));
  std::vector<int> canonical_rotation(
      std::begin(kFeatures.canonical_rotation),
      std::end(kFeatures.canonical_rotation));
  feature_value_to_canonical_ = FeatureValueToCanonical();
  test_set_ = std::move(test_set);
  training_boards_ =
      (TrainingBoard*) malloc(max_num_boards * sizeof(TrainingBoard));
  regressions_ =
      std::vector<CategoricalRegression>(num_splits, CategoricalRegression(
          max_feature_value, canonical_rotation));
}

CategoricalRegressions::~CategoricalRegressions() {
  free(training_boards_);
}

void CategoricalRegressions::Split(int new_num_splits) {
  assert (regressions_.size() == 1);
  regressions_ =
      std::vector<CategoricalRegression>(new_num_splits, regressions_[0]);
}

void CategoricalRegressions::Train(const std::vector<EvaluatedBoard>& boards,
                                   int distance,
                                   std::vector<float> learning_rates) {
  std::vector<std::vector<const TrainingBoard*>>
      training = BuildTrainSet(boards, distance);
  std::vector<std::vector<const TrainingBoard*>>
      test = BuildTrainSet(test_set_, distance);
  if (!boards.empty()) {
    std::cout << elapsed_time_.Get() << ": Prepared for training on "
              << boards.size() << " examples, distance " << distance << "\n";
  }
  std::string result;
  for (int i = 0; i < learning_rates.size(); ++i) {
    result = Step(training, test, learning_rates[i]);
    std::cout << "  " << elapsed_time_.Get() << ": step " << i + 1
              << " (learning rate: " << learning_rates[i] << ")\n" << result
              << "\n";
  }
}

void CategoricalRegressions::Round() {
  for (auto& regression : regressions_) {
    regression.Round();
  }
  std::cout << "Rounded.\n";
  Train({}, 0, {0});
}

void CategoricalRegressions::Save(const std::string& filepath) const {
  std::ofstream file;
  file.open(filepath, std::ios_base::binary | std::ios::out);
  assert (file.is_open());

  for (const CategoricalRegression& r : regressions_) {
    const std::vector<std::vector<TrainingFeature>>& features = r.GetFeatures();
    for (int i = 0; i < features.size(); ++i) {
      const std::vector<TrainingFeature>& feature = features[i];
      EvalType converted_feature;
      converted_feature.reserve(feature.size());
      for (int j = 0; j < feature.size(); ++j) {
        converted_feature.push_back(
            feature[feature_value_to_canonical_[i][j]].GetCompressedValue());
      }
      file.write((char*) &converted_feature[0],
                 (int) (sizeof(int8_t) * feature.size()));
    }
  }
  file.close();
}

std::string CategoricalRegressions::Step(
    const std::vector<std::vector<const TrainingBoard*>>& train,
    const std::vector<std::vector<const TrainingBoard*>>& test,
    float learning_rate) {
  double total_error = 0;
  double total_examples = 0;
  std::stringstream result;
  result << "    Errors: ";
  for (int i = 0; i < regressions_.size(); ++i) {
    regressions_[i].Train(train[i], learning_rate, 0);
    float error = regressions_[i].Test(test[i]);
    total_error += (error * error) * (float) test[i].size();
    total_examples += (double) test[i].size();
    result << error << " ";
  }
  result << "\n    Total: " << sqrt(total_error / total_examples);
  return result.str();
}

std::vector<std::vector<const TrainingBoard*>>
    CategoricalRegressions::BuildTrainSet(
        const std::vector<EvaluatedBoard>& boards,
        int distance) {
  int num_splits = (int) regressions_.size();
  std::vector<std::vector<int>> board_to_index(num_splits);
  std::vector<std::vector<const TrainingBoard*>> result(num_splits);

  for (const EvaluatedBoard& b : boards) {
    int size = (int) board_to_index_.size();
    auto value = board_to_index_.insert(std::make_pair(b, size));
    int current_board = value.first->second;
    assert((current_board == size) == value.second);
    if (current_board == size) {
      assert(current_board < max_num_boards);
      training_boards_[current_board] =
          TrainingBoard(b, feature_value_to_canonical_);
    }
    int exact_value = GetSplit(b.Empties(), num_splits);
    for (int i = std::max(0, exact_value - distance);
         i <= std::min((int) board_to_index.size() - 1, exact_value + distance);
         ++i) {
      board_to_index[i].push_back(current_board);
    }
  }
  for (int i = 0; i < board_to_index.size(); ++i) {
    result[i].reserve(board_to_index[i].size());
    for (int j = 0; j < board_to_index[i].size(); ++j) {
      result[i].push_back(&(training_boards_[board_to_index[i][j]]));
    }
  }
  return result;
}

std::vector<FeatureValue> CategoricalRegressions::FeatureValueToCanonical(int i) {
  std::vector<FeatureValue> result(kFeatures.max_feature_value[i] + 1);
  for (int j = 0; j == 0 ||
                  (kFeatures.features_to_patterns[i][j] != -1
                   && j + 1 < kMaxFeatureSize
                   && kFeatures.features_to_patterns[i][j + 1] != -1); ++j) {
    const BitPattern cur_pattern =
        kFeatures.patterns[kFeatures.features_to_patterns[i][j]]
            .ToBitPattern();
    BitPattern next_pattern = 0;
    if (kFeatures.features_to_patterns[i][j + 1] != -1) {
      next_pattern =
          kFeatures.patterns[kFeatures.features_to_patterns[i][j + 1]]
              .ToBitPattern();
    }
    const BitPattern pattern = cur_pattern | next_pattern;

    for (BitPattern player : AllSubBitPatterns(pattern)) {
      for (BitPattern opponent : AllSubBitPatterns(pattern & ~player)) {
        result[PatternEvaluator::GetFeature(i, player, opponent)] =
            PatternEvaluator::GetCanonicalFeature(i, player, opponent);
      }
    }
  }
  return result;
}

std::vector<std::vector<FeatureValue>> CategoricalRegressions::FeatureValueToCanonical() {
  std::vector<std::vector<FeatureValue>> result;
  int old_canonical_rotation = -1;
  for (int i = 0; i < kNumFeatures; ++i) {
    if (old_canonical_rotation == kFeatures.canonical_rotation[i]) {
      continue;
    }
    old_canonical_rotation = kFeatures.canonical_rotation[i];
    result.push_back(FeatureValueToCanonical(i));
  }
  return result;
}