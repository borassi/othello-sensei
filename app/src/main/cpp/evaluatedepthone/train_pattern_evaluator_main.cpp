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

#include <chrono>
#include <ctime>
#include <unordered_map>

#include "evaluator.h"
#include "train_pattern_evaluator.h"
#include "../utils/load_training_set.h"

// TODO: Split into train/test. There is a bug when the test set is split
// differently from the number of regressions.
std::string Step(std::vector<CategoricalRegression>& regressions,
                 const std::vector<std::vector<const TrainingBoard*>>& train,
                 const std::vector<std::vector<const TrainingBoard*>>& test,
                 float learning_rate) {
  double total_error = 0;
  double total_examples = 0;
  std::stringstream result;
  result << "  Errors: ";
  for (int i = 0; i < regressions.size(); ++i) {
    regressions[i].Train(train[i], learning_rate, 0.01);
    float error = regressions[i].Test(test[i]);
    total_error += (error * error) * (float) test[i].size();
    total_examples += test[i].size();
    result << error << " ";
  }
  result << "\n  Total: " << sqrt(total_error / total_examples);
  return result.str();
}

double ElapsedTime(std::chrono::time_point<std::chrono::system_clock> start) {
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> diff = end - start;
  return diff.count();
}

// TODO: Handle this better.
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

std::vector<std::vector<const TrainingBoard *>> BuildTrainSet(
    const std::vector<EvaluatedBoard>& boards,
    const std::unordered_map<EvaluatedBoard, int>& boards_to_index,
    const TrainingBoard* training_boards,
    int num_splits,
    int distance) {
  std::vector<std::vector<const TrainingBoard *>> result(num_splits);
  for (const EvaluatedBoard& b : boards) {
    int current_board = boards_to_index.at(b);
    int exact_value = (b.Empties() - 1) * num_splits / 60;
    for (int i = std::max(0, exact_value - distance);
         i <= std::min((int) result.size() - 1, exact_value + distance); ++i) {
      result[i].push_back(training_boards + current_board);
    }
  }
  return result;
}

int main() {
  auto start = std::chrono::system_clock::now();
  std::vector<EvaluatedBoard> full_train_board = load_hard_set(184);
  std::vector<EvaluatedBoard> train_board = load_train_set();
  full_train_board.insert(full_train_board.end(), train_board.begin(), train_board.end());
  std::vector<EvaluatedBoard> test_board = load_test_set();
  std::vector<FeatureValue> max_feature_value(
      std::begin(kFeatures.max_feature_value),
      std::end(kFeatures.max_feature_value));
  std::vector<int> canonical_rotation(
      std::begin(kFeatures.canonical_rotation),
      std::end(kFeatures.canonical_rotation));
  const auto feature_value_to_canonical = FeatureValueToCanonical();

  std::cout << ElapsedTime(start) << ": loaded training / test set.\n";

  int num_distinct_boards = 0;
  std::unordered_map<EvaluatedBoard, int> train_board_set;
  train_board_set.reserve(full_train_board.size());
  TrainingBoard* train_board_converted = new TrainingBoard[full_train_board.size()];

  for (const auto& boards : {full_train_board, test_board}) {
    for (const EvaluatedBoard& b : boards) {
      auto value = train_board_set.insert(std::make_pair(b, num_distinct_boards));
      int current_board = value.first->second;
      assert((current_board == num_distinct_boards) == value.second);
      if (current_board == num_distinct_boards) {
        train_board_converted[current_board] =
            TrainingBoard(b, feature_value_to_canonical);
        num_distinct_boards++;
      }
    }
  }

  int num_splits = 10;

  const auto train0 = BuildTrainSet(
      full_train_board, train_board_set, train_board_converted, 1, 0);
  const auto train1 = BuildTrainSet(
      full_train_board, train_board_set, train_board_converted, num_splits, 2);
  const auto train2 = BuildTrainSet(
      train_board, train_board_set, train_board_converted, num_splits, 0);
  const auto test = BuildTrainSet(
      test_board, train_board_set, train_board_converted, num_splits, 0);

  std::cout << full_train_board.size() << " " << num_distinct_boards << " "
            << train_board_set.size() << "\n";
  std::cout << ElapsedTime(start) << ": converted train / test set.\n";

  std::vector<CategoricalRegression> base_regression(1, CategoricalRegression(max_feature_value,canonical_rotation));
  std::string result;
  result = Step(base_regression, train0, test, 0.01F);
  std::cout << ElapsedTime(start) << ": step 1\n" << result << "\n";
  result = Step(base_regression, train0, test, 0.005F);
  std::cout << ElapsedTime(start) << ": step 2\n" << result << "\n";
  result = Step(base_regression, train0, test, 0.002F);
  std::cout << ElapsedTime(start) << ": step 3\n" << result << "\n";
  result = Step(base_regression, train0, test, 0.001F);
  std::cout << ElapsedTime(start) << ": step 4\n" << result << "\n";

  std::vector<CategoricalRegression> regressions(num_splits, base_regression[0]);

//  result = Step(regressions, train1, test, 0.01F);
//  std::cout << ElapsedTime(start) << ": step 1\n" << result << "\n";
//  result = Step(regressions, train1, test, 0.005F);
//  std::cout << ElapsedTime(start) << ": step 2\n" << result << "\n";
  result = Step(regressions, train1, test, 0.002F);
  std::cout << ElapsedTime(start) << ": step 3\n" << result << "\n";
  result = Step(regressions, train1, test, 0.001F);
  std::cout << ElapsedTime(start) << ": step 4\n" << result << "\n";

  result = Step(regressions, train2, test, 0.002F);
  std::cout << ElapsedTime(start) << ": step 2.1\n" << result << "\n";
  result = Step(regressions, train2, test, 0.001F);
  std::cout << ElapsedTime(start) << ": step 2.2\n" << result << "\n";
  result = Step(regressions, train2, test, 0.0005F);
  std::cout << ElapsedTime(start) << ": step 2.3\n" << result << "\n";
  result = Step(regressions, train2, test, 0.0002F);
  std::cout << ElapsedTime(start) << ": step 2.4\n" << result << "\n";
  result = Step(regressions, train2, test, 0.0001F);
  std::cout << ElapsedTime(start) << ": step 2.5\n" << result << "\n";
  result = Step(regressions, train2, test, 0.0001F);
  std::cout << ElapsedTime(start) << ": step 2.5bis\n" << result << "\n";
  for (CategoricalRegression& r : regressions) {
    r.Round();
  }
  result = Step(regressions, train2, test, 0);
  std::cout << ElapsedTime(start) << ": final\n" << result << "\n";
  delete[] train_board_converted;
}