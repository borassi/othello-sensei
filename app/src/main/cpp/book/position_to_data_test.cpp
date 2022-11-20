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

#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include <unordered_map>
#include "position_to_data.h"
#include "../board/board.h"

const std::string kTempDir = "app/testdata/tmp/book_test";

using ::testing::ElementsAreArray;

TEST(PositionToData, Basic) {
  PositionToData position_to_data(kTempDir);
  position_to_data.Clean();
  position_to_data.Put(0, 1, std::vector<char>({1, 2, 3, 4}));
  position_to_data.Print();
  EXPECT_EQ(position_to_data.Get(0, 1), std::vector<char>({1, 2, 3, 4}));
  EXPECT_EQ(position_to_data.Get(0, 2), std::vector<char>());
  position_to_data.Put(0, 2, std::vector<char>({5, 6, 7, 8}));
  position_to_data.Print();
  EXPECT_EQ(position_to_data.Get(0, 2), std::vector<char>({5, 6, 7, 8}));
  position_to_data.Put(0, 2, std::vector<char>({5, 6, 7, 8, 9}));
  position_to_data.Print();
  EXPECT_EQ(position_to_data.Get(0, 1), std::vector<char>({1, 2, 3, 4}));
  EXPECT_EQ(position_to_data.Get(0, 2), std::vector<char>({5, 6, 7, 8, 9}));
  position_to_data.Put(0, 3, std::vector<char>({1, 1, 1, 1}));
  position_to_data.Print();
  PositionToData position_to_data1(kTempDir);
  EXPECT_EQ(position_to_data1.Get(0, 1), std::vector<char>({1, 2, 3, 4}));
}

TEST(PositionToData, LargeOne) {
  std::unordered_map<Board, std::vector<char>> expected;
  std::vector<Board> old_boards;
  PositionToData actual(kTempDir);
  actual.Clean();

  for (int i = 0; i < 100000; ++i) {
    if (i % 1000 == 0 || (i < 1000 && i % 100 == 0)) {
      std::cout << i << "\n";
    }
    Board b = RandomBoard();
    std::vector<char> value = {(char) (i % 256), (char) (i / 256)};
    old_boards.push_back(b);
    expected[b.Unique()] = value;
    actual.Put(b.Player(), b.Opponent(), value);

    int test_i = rand() % (i+1);
    Board test_board = old_boards[test_i];
    ASSERT_THAT(
        actual.Get(test_board.Player(), test_board.Opponent()),
        ElementsAreArray(expected[test_board.Unique()])) << i << " " << test_i;

    if (i < 500) {
      for (int test_i = 0 ; test_i < i; ++test_i) {
        Board test_board = old_boards[test_i];
        ASSERT_THAT(
            actual.Get(test_board.Player(), test_board.Opponent()),
            ElementsAreArray(expected[test_board.Unique()])) << i << " " << test_i;
      }
    }
    if (i < 10000) {
      assert(actual.IsSizeOK());
    }
    if (rand() % 100 == 0) {
      actual = PositionToData(kTempDir);
    }
  }
}

TEST(PositionToData, Unique) {
  PositionToData position_to_data(kTempDir);
  position_to_data.Clean();
  position_to_data.Put(0, 1ULL << 7, std::vector<char>({1, 2, 3, 4}));
  EXPECT_EQ(position_to_data.Get(0, 1), std::vector<char>({1, 2, 3, 4}));
  EXPECT_EQ(position_to_data.Get(0, 1ULL << 56), std::vector<char>({1, 2, 3, 4}));
  EXPECT_EQ(position_to_data.Get(0, 1ULL << 63), std::vector<char>({1, 2, 3, 4}));
}