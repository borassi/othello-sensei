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

#include <fstream>
#include <iostream>
#include "load_training_set.h"
#include "../board/bitpattern.h"
#include "../board/board.h"

using ::std::istream;
using ::std::ifstream;
using ::std::ios;

istream& operator>>(istream& is, EvaluatedBoard& board) {
  is.read((char*) &board.player_, sizeof(board.player_));
  is.read((char*) &board.opponent_, sizeof(board.opponent_));
  is.read((char*) &board.eval_, sizeof(board.eval_));
  return is;
}

std::vector<EvaluatedBoard> load_set(const std::vector<std::string>& files) {
  std::vector<EvaluatedBoard> result;
  for (const std::string& file : files) {
    ifstream input;
    input.open(file, ios::binary);
    while (input.peek() != EOF) {
      EvaluatedBoard b;
      input >> b;
      if (b.IsValid()) {
        result.push_back(b);
      }
    }
    input.close();
  }
  return result;
}

std::vector<EvaluatedBoard> load_set(int start_year, int end_year) {
  std::vector<std::string> files;
  for (int i = start_year; i <= end_year; ++i) {
    files.push_back(
      "testdata/training_set/TrainingSet" + std::to_string(i) + ".tmp");
  }
  return load_set(files);
}

std::vector<EvaluatedBoard> load_hard_set(int last_file_number) {
  std::vector<std::string> files;
  for (int i = 0; i <= last_file_number; ++i) {
    files.push_back(
      "testdata/training_set/weird_positions_result_" + std::to_string(i)
      + ".tmp");
  }
  return load_set(files);
}

std::vector<EvaluatedBoard> load_train_set() {
  return load_set(1977, 2014);
}

std::vector<EvaluatedBoard> load_test_set() {
  return load_set(2015, 2016);
}