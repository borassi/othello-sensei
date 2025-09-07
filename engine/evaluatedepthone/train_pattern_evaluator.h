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

#include <algorithm>
#include <cassert>
#include <fstream>
#include <memory>
#include <random>
#include <set>
#include <unordered_map>
#include "pattern_evaluator.h"
#include "../utils/load_training_set.h"
#include "../utils/misc.h"

class TrainingBoard {
 public:
  TrainingBoard() {}
  TrainingBoard(std::vector<FeatureValue> features, int eval) :
      features_(features), eval_(eval) {}

  TrainingBoard(
      const EvaluatedBoard& b,
      const std::vector<std::vector<FeatureValue>>& feature_value_to_canonical);

  int Eval() const { return eval_; }

  const std::vector<FeatureValue>& Features() const { return features_; }

  bool operator==(const TrainingBoard& other) const;

 private:
  std::vector<FeatureValue> features_;
  int eval_;
};

template<>
struct std::hash<TrainingBoard> {
  std::size_t operator()(const TrainingBoard& f) const noexcept;
};

class TrainingFeature {
 public:
  static constexpr float beta_1 = 0.9F;
  TrainingFeature() : value_(0), m_t_(0) {}

  float GetValue() const { return value_; }

  void UpdateValue(float error, float learning_rate, float lambda);

  float RoundValue() const { return round(value_ * 8) / 8; }

  void Round() { value_ = RoundValue(); }

  int8_t GetCompressedValue() const {
    float value = RoundValue() * 8;
    assert (value < 127 && value > -127);
    return (int8_t) value;
  }

 private:
  float value_;
  float m_t_;
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

  const std::vector<std::vector<TrainingFeature>>& GetFeatures() const {
    return features_;
  }

 private:
  std::vector<FeatureValue> max_feature_value_;
  std::vector<int> canonical_rotation_;
  std::vector<std::vector<TrainingFeature>> features_;
};

// TODO: Test this.
class CategoricalRegressions {
 public:
  CategoricalRegressions(
      int num_splits,
      std::vector<EvaluatedBoard> test_set,
      int max_num_boards);

  ~CategoricalRegressions();

  void Split(int new_num_splits);

  void Train(const std::vector<EvaluatedBoard>& boards, int distance,
             std::vector<float> learning_rates);

  void Round();

  void Save(const std::string& filepath) const;

 private:
  std::string Step(const std::vector<std::vector<const TrainingBoard*>>& train,
                   const std::vector<std::vector<const TrainingBoard*>>& test,
                   float learning_rate);

  std::vector<std::vector<const TrainingBoard*>> BuildTrainSet(
      const std::vector<EvaluatedBoard>& boards,
      int distance);

  static std::vector<FeatureValue> FeatureValueToCanonical(int i);

  static std::vector<std::vector<FeatureValue>> FeatureValueToCanonical();

  std::vector<CategoricalRegression> regressions_;
  std::unordered_map<EvaluatedBoard, int> board_to_index_;
  TrainingBoard* training_boards_;
  int max_num_boards;
  std::vector<std::vector<FeatureValue>> feature_value_to_canonical_;
  std::vector<EvaluatedBoard> test_set_;
  ElapsedTime elapsed_time_;

};