/*
 * Copyright 2022-2024 Michele Borassi
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

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "../utils/files.h"
#include "value_file.h"

const std::string kTempFile = "app/testdata/tmp/value_file_test/value_file_test.val";

TEST(ValueFile, AddOnly) {
  ValueFile value_file(kTempFile, 5);
  value_file.Clean();
  EXPECT_EQ(value_file.Elements(), 1);

  std::vector<char> values1 = {0, 1, 2, 3, 4};
  BookFileOffset offset1 = value_file.Add(values1);
  EXPECT_EQ(value_file.Get(offset1), values1);
  EXPECT_EQ(value_file.Elements(), 2);

  std::vector<char> values2 = {1, 1, 2, 3, 4};
  BookFileOffset offset2 = value_file.Add(values2);
  EXPECT_EQ(value_file.Get(offset2), values2);
  EXPECT_EQ(value_file.Elements(), 3);

  std::vector<char> values3 = {1, 1, 2, 3, 3};
  BookFileOffset offset3 = value_file.Add(values3);
  EXPECT_EQ(value_file.Get(offset3), values3);
  EXPECT_EQ(value_file.Elements(), 4);
  value_file.Remove();
}

TEST(ValueFile, Reopen) {
  ValueFile value_file(kTempFile, 5);
  EXPECT_EQ(value_file.Elements(), 1);

  std::vector<char> values1 = {0, 1, 2, 3, 4};
  BookFileOffset offset1 = value_file.Add(values1);

  ValueFile value_file_copy(kTempFile, 5);

  EXPECT_EQ(value_file.Get(offset1), values1);
  EXPECT_EQ(value_file.Elements(), 2);
  value_file.Remove();
}

TEST(ValueFile, NotExistingDir) {
  fs::remove_all(fs::path(kTempFile).remove_filename());
  ValueFile value_file(kTempFile, 5);
  EXPECT_EQ(value_file.Elements(), 1);
  value_file.Remove();
}

TEST(ValueFile, AddAndRemove) {
  ValueFile value_file(kTempFile, 4);

  std::vector<char> values1 = {0, 1, 2, 3};
  BookFileOffset offset1 = value_file.Add(values1);

  auto values1_copy = value_file.Remove(offset1);
  EXPECT_EQ(values1_copy, values1);
  EXPECT_EQ(value_file.Elements(), 2);
  EXPECT_EQ(value_file.Get(offset1), std::vector<char>({0, 0, 0, 0}));

  offset1 = value_file.Add(values1);
  EXPECT_EQ(value_file.Elements(), 2);
  EXPECT_EQ(value_file.Get(offset1), values1);

  std::vector<char> values2 = {1, 1, 2, 3};
  BookFileOffset offset2 = value_file.Add(values2);

  std::vector<char> values3 = {1, 1, 2, 4};
  BookFileOffset offset3 = value_file.Add(values3);

  auto values2_copy = value_file.Remove(offset2);
  EXPECT_EQ(values2_copy, values2);
  EXPECT_EQ(value_file.Elements(), 4);
  EXPECT_EQ(value_file.Get(offset2), std::vector<char>({0, 0, 0, 0}));

  std::vector<char> values4 = {1, 1, 2, 6};
  BookFileOffset offset4 = value_file.Add(values4);

  EXPECT_EQ(value_file.Elements(), 4);
  EXPECT_EQ(value_file.Get(offset4), values4);

  value_file.Remove(offset1);
  value_file.Remove(offset3);
  EXPECT_EQ(value_file.Elements(), 4);
  EXPECT_EQ(value_file.Get(offset4), values4);

  std::vector<char> values5 = {1, 1, 2, 4};
  BookFileOffset offset5 = value_file.Add(values5);
  EXPECT_EQ(value_file.Get(offset5), values5);
  EXPECT_EQ(value_file.Elements(), 4);

  std::vector<char> values6 = {1, 1, 2, 3};
  BookFileOffset offset6 = value_file.Add(values6);
  EXPECT_EQ(value_file.Get(offset6), values6);
  EXPECT_EQ(value_file.Elements(), 4);
  value_file.Remove();
}

TEST(ValueFile, Iterator) {
  ValueFile value_file(kTempFile, 5);
  EXPECT_EQ(value_file.Elements(), 1);

  std::vector<char> values1 = {0, 1, 2, 3, 4};
  value_file.Add(values1);

  std::vector<char> values2 = {1, 1, 2, 3, 4};
  ValueFileSize offset2 = value_file.Add(values2);

  std::vector<char> values3 = {1, 1, 2, 3, 3};
  value_file.Add(values3);

  std::vector<std::pair<int, std::vector<char>>> all_elements(value_file.begin(), value_file.end());

  EXPECT_THAT(all_elements,
              ::testing::UnorderedElementsAre(
                  ::testing::Pair(1, values1),
                  ::testing::Pair(2, values2),
                  ::testing::Pair(3, values3)
              ));

  value_file.Remove(offset2);

  std::vector<std::pair<int, std::vector<char>>> all_elements2(value_file.begin(), value_file.end());

  EXPECT_THAT(all_elements2,
              ::testing::UnorderedElementsAre(
                  ::testing::Pair(1, values1),
                  ::testing::Pair(3, values3)
              ));  value_file.Clean();
}

TEST(ValueFile, IteratorRemoveLast) {
  ValueFile value_file(kTempFile, 9);
  EXPECT_EQ(value_file.Elements(), 1);

  std::vector<char> values1 = {0, 1, 2, 3, 4};
  value_file.Add(values1);

  std::vector<char> values2 = {1, 1, 2, 3, 4};
  value_file.Add(values2);

  std::vector<char> values3 = {1, 1, 2, 3, 3};
  ValueFileSize offset3 = value_file.Add(values3);

  value_file.Remove(offset3);

  std::vector<std::pair<int, std::vector<char>>> all_elements(value_file.begin(), value_file.end());

  EXPECT_THAT(all_elements,
              ::testing::UnorderedElementsAre(
                  ::testing::Pair(1, std::vector<char>{0, 1, 2, 3, 4, 0, 0, 0, 0}),
                  ::testing::Pair(2, std::vector<char>{1, 1, 2, 3, 4, 0, 0, 0, 0})
              ));
  value_file.Clean();
}