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
#include <unordered_map>
#include "evaluator.h"
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
  static constexpr double beta_1 = 0.9;
  TrainingFeature() : value_(0), update_(0), m_t_(0), m_bias_(1) {}

  float GetValue() { return value_; }

  void UpdateValue(float error, float learning_rate, float lambda);

  void Round();

 private:
  float m_bias_;
  float value_;
  float m_t_;
  float update_;
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

// TODO: Test this.
class CategoricalRegressions {
 public:
  CategoricalRegressions(
      int num_splits,
      std::vector<EvaluatedBoard> test_set) {
    std::vector<FeatureValue> max_feature_value(
        std::begin(kFeatures.max_feature_value),
        std::end(kFeatures.max_feature_value));
    std::vector<int> canonical_rotation(
        std::begin(kFeatures.canonical_rotation),
        std::end(kFeatures.canonical_rotation));
    feature_value_to_canonical_ = FeatureValueToCanonical();
    test_set_ = test_set;
    regressions_ =
        std::vector<CategoricalRegression>(num_splits, CategoricalRegression(max_feature_value, canonical_rotation));
  }

  void Split(int new_num_splits) {
    assert (regressions_.size() == 1);
    regressions_ = std::vector<CategoricalRegression>(new_num_splits, regressions_[0]);
  }

  void Train(const std::vector<EvaluatedBoard>& boards, int distance,
             std::vector<double> learning_rates) {
    std::vector<std::vector<const TrainingBoard*>> training = BuildTrainSet(boards, distance);
    std::vector<std::vector<const TrainingBoard*>> test = BuildTrainSet(test_set_, distance);
    std::cout << elapsed_time_.Get() << ": Prepared for training on "
              << boards.size() << " examples, distance " << distance << "\n";
    std::string result;
    for (int i = 0; i < learning_rates.size(); ++i) {
      result = Step(training, test, learning_rates[i]);
      std::cout << "  " << elapsed_time_.Get() << ": step " << i + 1
                << " (learning rate: " << learning_rates[i] << ")\n" << result
                << "\n";
    }
  }

 private:
  std::string Step(const std::vector<std::vector<const TrainingBoard*>>& train,
                   const std::vector<std::vector<const TrainingBoard*>>& test,
                   float learning_rate) {
    double total_error = 0;
    double total_examples = 0;
    std::stringstream result;
    result << "    Errors: ";
    for (int i = 0; i < regressions_.size(); ++i) {
      regressions_[i].Train(train[i], learning_rate, 0.01);
      float error = regressions_[i].Test(test[i]);
      total_error += (error * error) * (float) test[i].size();
      total_examples += test[i].size();
      result << error << " ";
    }
    result << "\n    Total: " << sqrt(total_error / total_examples);
    return result.str();
  }

  std::vector<std::vector<const TrainingBoard*>> BuildTrainSet(
      const std::vector<EvaluatedBoard>& boards,
      int distance) {
    int num_splits = regressions_.size();
    std::vector<std::vector<int>> board_to_index(num_splits);
    std::vector<std::vector<const TrainingBoard*>> result(num_splits);

    for (const EvaluatedBoard& b : boards) {
      int size = board_to_index_.size();
      auto value = board_to_index_.insert(std::make_pair(b, size));
      int current_board = value.first->second;
      assert((current_board == size) == value.second);
      if (current_board == size) {
        assert(training_boards_.size() == size);
        training_boards_
            .push_back(TrainingBoard(b, feature_value_to_canonical_));
      }
      int exact_value = (b.Empties() - 1) * num_splits / 60;
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

  std::vector<FeatureValue> FeatureValueToCanonical(int i) {
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
          if (((player | opponent) & pattern) == 0) {
            continue;
          }
          result[Evaluator::GetFeature(i, player, opponent)] =
              Evaluator::GetCanonicalFeature(i, player, opponent);
        }
      }
    }
    return result;
  }

  std::vector<std::vector<FeatureValue>> FeatureValueToCanonical() {
    std::vector<std::vector<FeatureValue>> result;
    int old_canonical_rotation = -1;
    for (int i = 0; i < kNumFeatures; ++i) {
      if (old_canonical_rotation == kFeatures.canonical_rotation[i]) {
        continue;
      }
      old_canonical_rotation = kFeatures.canonical_rotation[i];
      result.push_back(std::move(FeatureValueToCanonical(i)));
    }
    return result;
  }

  int num_features_;
  std::vector<CategoricalRegression> regressions_;
  std::unordered_map<EvaluatedBoard, int> board_to_index_;
  std::vector<TrainingBoard> training_boards_;
  int num_distinct_boards_;
  std::vector<std::vector<FeatureValue>> feature_value_to_canonical_;
  std::vector<EvaluatedBoard> test_set_;
  ElapsedTime elapsed_time_;

};