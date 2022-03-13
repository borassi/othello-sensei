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
#include "train_pattern_evaluator.h"


TrainingBoard::TrainingBoard(
    const EvaluatedBoard& b,
    const std::vector <std::vector<
        FeatureValue>>& feature_value_to_canonical) {
  Evaluator p;
  eval_ = b.GetEval();
  p.Setup(b.GetPlayer(), b.GetOpponent());
  for (int i = 0; i < kNumFeatures; ++i) {
    features_.push_back(
        feature_value_to_canonical[kFeatures.canonical_rotation[i]][p
            .GetFeature(i)]);
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
  std::size_t result = std::hash<float>{}(f.Eval());
  for (const FeatureValue& feature : f.Features()) {
    result = result ^ std::hash<Pattern>{}(feature);
  }
  return result;
}

void TrainingFeature::UpdateValue(float error, float learning_rate, float lambda) {
  assert (isfinite(error));
  float update = error - lambda * value_;
  m_bias_ *= beta_1;
  m_t_ = beta_1 * m_t_ + (1 - beta_1) * update;
  value_ += 2 * learning_rate * m_t_ / (1 - m_bias_);
  value_ = std::max(-12.0F, std::min(value_, 12.0F));
}

void TrainingFeature::Round() {
  value_ = round(value_ * 8) / 8;
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
  assert (isfinite(b.Eval()));
  return b.Eval() - Eval(b);
}

void CategoricalRegression::Train(
    std::vector<const TrainingBoard*> training_set_,
    float learning_rate, float lambda) {
  auto rng = std::default_random_engine{};
  std::shuffle(std::begin(training_set_), std::end(training_set_), rng);
  for (int i = 0; i < training_set_.size(); ++i) {
    const TrainingBoard& b = *training_set_[i];
    const std::vector<FeatureValue>& features = b.Features();
    float error = Error(b);
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
  return (float) sqrt(total_error / test_set.size());
}

void CategoricalRegression::Round() {
  for (auto& feature : features_) {
    for (auto& value : feature) {
      value.Round();
    }
  }
}