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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "sequence.h"

TEST(Sequence, Sequence) {
  Sequence sequence1("e6f4c3c4d3");
  Sequence sequence2("f5d6c3d3c4");
  Sequence sequence3("e6f4c3c4d3");
  Sequence sequence4("e6f4c3c4");

  EXPECT_LT(sequence4, sequence1);
  EXPECT_LT(sequence1, sequence2);
  EXPECT_GT(sequence2, sequence1);
  EXPECT_EQ(sequence1, sequence3);
}

TEST(Sequence, ToCanonicalSequence1) {
  Sequence sequence1("e6f4c3c4d3");
  Sequence sequence2("f5d6c3d3c4");
  Sequence sequence3("d3c5f6f5e6");
  Sequence sequence4("c4e3f6e6f5");

  std::vector<Sequence> sequences = {sequence1, sequence2, sequence3, sequence4};
  for (Sequence& sequence : sequences) {
    sequence.ToCanonicalSequence();
    EXPECT_EQ(sequence, Sequence("e6f4c3c4d3"));
  }
}