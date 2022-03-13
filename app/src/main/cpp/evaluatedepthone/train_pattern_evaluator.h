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

#include <algorithm>
#include <cassert>
#include <memory>
#include <random>
#include <set>
#include "evaluator.h"
#include "../utils/load_training_set.h"

class TrainingBoard {
 public:
  TrainingBoard() {}
  TrainingBoard(std::vector<FeatureValue> features, float eval) :
      features_(features), eval_(eval) {}

  TrainingBoard(
      const EvaluatedBoard& b,
      const std::vector<std::vector<FeatureValue>>& feature_value_to_canonical);

  float Eval() const { return eval_; }

  const std::vector<FeatureValue>& Features() const { return features_; }

  bool operator==(const TrainingBoard& other) const;

 private:
  std::vector<FeatureValue> features_;
  float eval_;
};

template<>
struct std::hash<TrainingBoard> {
  std::size_t operator()(const TrainingBoard& f) const noexcept;
};

class TrainingFeature {
 public:
  static constexpr float beta_1 = 0.9;
  static constexpr float beta_2 = 0.999;
  static constexpr float eps = 1E-2;
  TrainingFeature() : value_(0), update_(0), m_t_(0), v_t_(0), visited_(0), m_bias_(1), v_bias_(1) {}

  float GetValue() { return value_; }

  void UpdateValue(float error, float learning_rate, float lambda);

  void Round();

 private:
  float v_bias_;
  float m_bias_;
  float value_;
  float m_t_;
  float v_t_;
  float update_;
  int visited_;
};

class CategoricalRegression {
 public:
  CategoricalRegression(std::vector<FeatureValue> max_feature_value,
                        std::vector<int> canonical_rotation);

  float Eval(const TrainingBoard& b);

  float Error(const TrainingBoard& b);

  void Train(std::vector<const TrainingBoard*> training_set_, float learning_rate, float lambda);

  float Test(const std::vector<const TrainingBoard*>& test_set);

  void Round();

 private:
  std::vector<FeatureValue> max_feature_value_;
  std::vector<int> canonical_rotation_;
  std::vector<std::vector<TrainingFeature>> features_;
};