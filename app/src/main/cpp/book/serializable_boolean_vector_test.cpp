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

TEST(SerializableBooleanVector, Serialize) {
  for (int i = 0; i < 1000; ++i) {
    int size = rand() % 100;
    std::vector<bool> expected;
    SerializableBooleanVector actual;
    for (int j = 0; j < size; ++j) {
      bool b = rand() % 2;
      expected.push_back(b);
      actual.PushBack(b);
    }
    SerializableBooleanVector deserialized(actual.Serialize(), actual.Size());
    ASSERT_EQ(expected.size(), actual.Size());
    ASSERT_EQ(expected.size(), deserialized.Size());
    for (int j = 0; j < size; ++j) {
      ASSERT_EQ(expected[j], actual.Get(j)) << i << " " << j << " " << expected[j] << " " << actual.Get(j);
      ASSERT_EQ(expected[j], deserialized.Get(j));
    }
  }
}