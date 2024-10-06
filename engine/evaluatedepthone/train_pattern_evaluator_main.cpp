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

#include <unordered_map>

#include "pattern_evaluator.h"
#include "train_pattern_evaluator.h"
#include "../utils/load_training_set.h"

int main() {
//  auto start = std::chrono::system_clock::now();
  std::vector<EvaluatedBoard> full_train_board = load_hard_set(184);
  std::vector<EvaluatedBoard> train_board = load_train_set();
  full_train_board.insert(full_train_board.end(), train_board.begin(), train_board.end());
//  std::vector<EvaluatedBoard> full_train_board = load_set(1987, 1991);
//  std::vector<EvaluatedBoard> train_board = full_train_board;
  std::vector<EvaluatedBoard> test_board = load_test_set();
  int num_splits = 10;

  CategoricalRegressions trainer(1, test_board, (int) (full_train_board.size() + test_board.size()));

  trainer.Train(full_train_board, 0, {0.005F, 0.002F, 0.001F, 0.0005F});

  trainer.Split(num_splits);

//  trainer.Train(full_train_board, 2, {0.001});
  trainer.Train(train_board, 4, {0.001F});
  trainer.Train(train_board, 2, {0.001F});
  trainer.Train(train_board, 1, {0.001F});
  trainer.Train(train_board, 0, {0.002F, 0.001F, 0.0005F, 0.0002F, 0.0001F, 0.0001F});
  trainer.Round();
  trainer.Save("app/src/main/assets/coefficients/pattern_evaluator_cpp_new"
               ".dat");

  return 0;
}