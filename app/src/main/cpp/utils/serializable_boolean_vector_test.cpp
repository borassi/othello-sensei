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
#include <gtest/gtest.h>
#include <variant>
#include "serializable_boolean_vector.h"

TEST(SerializableBooleanVector, Works) {
  SerializableBooleanVector v;
  v.PushBack(true);
  v.PushBack(true);
  v.PushBack(false);
  v.PushBack(false);
  v.PushBack(true);
  v.PushBack(false);
  v.PushBack(true);
  v.PushBack(true);
  v.PushBack(true);

  EXPECT_EQ(v.Get(0), true);
  EXPECT_EQ(v.Get(1), true);
  EXPECT_EQ(v.Get(2), false);
  EXPECT_EQ(v.Get(3), false);
  EXPECT_EQ(v.Get(4), true);
  EXPECT_EQ(v.Get(5), false);
  EXPECT_EQ(v.Get(6), true);
  EXPECT_EQ(v.Get(7), true);
  EXPECT_EQ(v.Get(8), true);
  EXPECT_EQ(v.Size(), 9);
}

TEST(SerializableBooleanVector, PushBackUnsignedInt) {
  SerializableBooleanVector v;
  v.PushBack(0b010111, 6);
  v.PushBack(UINT32_MAX, 32);
  v.PushBack(0, 4);

  EXPECT_EQ(v.Get(0), false);
  EXPECT_EQ(v.Get(1), true);
  EXPECT_EQ(v.Get(2), false);
  EXPECT_EQ(v.Get(3), true);
  EXPECT_EQ(v.Get(4), true);
  EXPECT_EQ(v.Get(5), true);
  for (int i = 6; i < 6 + 32; ++i) {
    EXPECT_EQ(v.Get(i), true);
  }
  for (int i = 38; i < 38 + 4; ++i) {
    EXPECT_EQ(v.Get(i), false);
  }
  EXPECT_EQ(v.Size(), 42);
}

TEST(SerializableBooleanVector, Randomized) {
  for (int i = 0; i < 5000; ++i) {
    int size = rand() % 500;
    std::vector<std::pair<uint32_t, int>> expected;
    SerializableBooleanVector actual;
    int total_size = 0;
    for (int j = 0; j < size; ++j) {
      if (rand() % 10 == 0) {
        int size = 1 + rand() % 32;
        uint32_t next = rand() % (1U << size);
        expected.push_back(std::make_pair(next, size));
        actual.PushBack(next, size);
        total_size += size;
      } else {
        bool next = rand() % 2;
        expected.push_back(std::make_pair(next, 1));
        actual.PushBack(next);
        ++total_size;
      }
    }
    auto serialized = actual.Serialize();
    SerializableBooleanVector deserialized(serialized.begin(), serialized.end(), actual.Size());
    ASSERT_EQ(total_size, actual.Size());
    ASSERT_EQ(total_size, deserialized.Size());
    int actual_index = 0;
    for (int j = 0; j < expected.size(); ++j) {
      uint32_t value = expected[j].first;
      int size = expected[j].second;
      ASSERT_EQ(value, actual.Get(actual_index, size)) << i << " " << j << " " << value << " " << size << " " << actual.Get(actual_index, size);
      ASSERT_EQ(value, deserialized.Get(actual_index, size));
      actual_index += size;
    }
  }
}