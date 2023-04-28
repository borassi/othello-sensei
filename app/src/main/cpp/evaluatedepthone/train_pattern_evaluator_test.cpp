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

#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "train_pattern_evaluator.h"

using ::testing::ContainerEq;

std::vector<TrainingBoard> TrainingSet(
    int size,
    const std::vector<FeatureValue>& max_feature_value) {
  std::vector<TrainingBoard> result;
  for (int i = 0; i < size; ++i) {
    std::vector<FeatureValue> features(max_feature_value.size());
    int eval = 0;
    for (FeatureValue f = 0; f < max_feature_value.size(); ++f) {
      features[f] = rand() % (max_feature_value[f] + 1);
      eval += features[f];
    }
    result.push_back(TrainingBoard(features, eval));
  }
  return result;
}

TEST(TrainPatternEvaluator, Simple) {
  std::vector<FeatureValue> max_feature_value = {1, 5, 7};
  std::vector<FeatureValue> canonical_rotation = {0, 1, 2};
  std::vector<TrainingBoard> train_set_board = TrainingSet(1000, max_feature_value);
  std::vector<TrainingBoard> test_set = TrainingSet(1000, max_feature_value);

  std::vector<const TrainingBoard*> train_set;

  for (TrainingBoard& b : train_set_board) {
    train_set.push_back(&b);
  }

  CategoricalRegression regression(max_feature_value, canonical_rotation);
  regression.Train(train_set, 0.1, 0);
//  EXPECT_FLOAT_EQ(regression.Eval(TrainingBoard({0, 4, 2}, 6)), 6);
//  EXPECT_FLOAT_EQ(regression.Error(TrainingBoard({0, 4, 2}, 6)), 0);
//  EXPECT_FLOAT_EQ(regression.Error(TrainingBoard({0, 4, 2}, 7)), 1);
//  EXPECT_NEAR(regression.Test(test_set), 0, 1E-5);
}

TEST(TrainPatternEvaluator, Save) {
  std::vector<FeatureValue> max_feature_value = {1, 5, 7};
  std::vector<FeatureValue> canonical_rotation = {0, 1, 2};
  std::vector<TrainingBoard> train_set_board = TrainingSet(1000, max_feature_value);
  std::vector<TrainingBoard> test_set = TrainingSet(1000, max_feature_value);

  std::vector<const TrainingBoard*> train_set;

  for (TrainingBoard& b : train_set_board) {
    train_set.push_back(&b);
  }

  CategoricalRegression regression(max_feature_value, canonical_rotation);
  regression.Train(train_set, 0.1, 0);
//  EXPECT_FLOAT_EQ(regression.Eval(TrainingBoard({0, 4, 2}, 6)), 6);
//  EXPECT_FLOAT_EQ(regression.Error(TrainingBoard({0, 4, 2}, 6)), 0);
//  EXPECT_FLOAT_EQ(regression.Error(TrainingBoard({0, 4, 2}, 7)), 1);
//  EXPECT_NEAR(regression.Test(test_set), 0, 1E-5);
}
