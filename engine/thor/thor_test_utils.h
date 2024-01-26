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
 
#ifndef OTHELLO_SENSEI_THOR_TEST_UTILS_H
#define OTHELLO_SENSEI_THOR_TEST_UTILS_H

#include <fstream>
#include <string>
const std::string kThorTestData = "tmp/testdata/thor";

char StringToThorSquare(std::string square) {
  char value = (square[0] - 'a' + 1) + 10 * (square[1] - '0');
  assert(value / 10 >= 1 && value / 10 <= 8);
  assert(value % 10 >= 1 && value % 10 <= 8);
  return value;
}

std::vector<char> StoredGame(int tournament, int black, int white, int score, int theoretical, std::string moves) {
  std::vector<char> result;
  result.insert(result.end(), {(char) tournament, (char) (tournament / 256)});
  result.insert(result.end(), {(char) black, (char) (black / 256)});
  result.insert(result.end(), {(char) white, (char) (white / 256)});
  result.insert(result.end(), {(char) score, (char) theoretical});

  for (int i = 0; i < 60; ++i) {
    if (moves.length() <= 2 * i) {
      result.push_back(0);
    } else {
      result.push_back(StringToThorSquare(moves.substr(2 * i, 2)));
    }
  }
  return result;
}

std::string PadString(const std::string& to_pad, int length) {
  std::string result = to_pad;
  while (result.length() < length) {
    result += '\0';
  }
  return result;
}

void WriteHeader(std::ofstream& file, short year) {
  std::vector<char> to_write(10, 0);
  file.write(to_write.data(), 10);
  file.write((char*) &year, 2);
  file.write(to_write.data(), 4);
}

#endif //OTHELLO_SENSEI_THOR_TEST_UTILS_H
