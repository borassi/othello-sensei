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

#include <gtest/gtest.h>
#include "position_to_data.h"
#include "../board/board.h"

const std::string kTempDir = "app/testdata/tmp/book_test";

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
}

TEST(PositionToData, Unique) {
  PositionToData position_to_data(kTempDir);
  position_to_data.Clean();
  position_to_data.Put(0, 1ULL << 7, std::vector<char>({1, 2, 3, 4}));
  EXPECT_EQ(position_to_data.Get(0, 1), std::vector<char>({1, 2, 3, 4}));
  EXPECT_EQ(position_to_data.Get(0, 1ULL << 56), std::vector<char>({1, 2, 3, 4}));
  EXPECT_EQ(position_to_data.Get(0, 1ULL << 63), std::vector<char>({1, 2, 3, 4}));
}