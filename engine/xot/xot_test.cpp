/*
 * Copyright 2025 Michele Borassi
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
#include <gmock/gmock.h>
#include <optional>

#include "xot.h"
#include "../board/board.h"
#include "../board/sequence.h"

using ::testing::DoubleNear;
using ::testing::IsEmpty;
using ::testing::Optional;
using ::testing::Pair;
using ::testing::UnorderedElementsAre;

TEST(XOT, IsInList) {
  XOT xot("f5d6c4d3c2b3b4b5\n"
          "f5f4g3g6f3g4e3e2");

  EXPECT_TRUE(xot.IsInList(Sequence("f5d6c4d3c2b3b4b5")));
  EXPECT_TRUE(xot.IsInList(Sequence("f5f4g3g6f3g4e3e2")));
  EXPECT_TRUE(xot.IsInList(Sequence("c4e3f5e6f7g6g5g4")));
  EXPECT_FALSE(xot.IsInList(Sequence("")));
  EXPECT_FALSE(xot.IsInList(Sequence("e6f4c3c4d3d6f6e7")));
}
TEST(XOT, IsInListPrefix) {
  XOT xot("f5d6c4d3c2b3b4b5\n"
          "f5f4g3g6f3g4e3e2");

  EXPECT_TRUE(xot.IsInListPrefix(Sequence("f5d6c4d3c2b3b4b5")));
  EXPECT_TRUE(xot.IsInListPrefix(Sequence("f5f4g3g6f3g4e3e2")));
  EXPECT_TRUE(xot.IsInListPrefix(Sequence("c4e3f5e6f7g6g5g4")));
  EXPECT_TRUE(xot.IsInListPrefix(Sequence("f5d6c4d3c2b3b4b5a4d2")));
  EXPECT_FALSE(xot.IsInListPrefix(Sequence("")));
  EXPECT_FALSE(xot.IsInListPrefix(Sequence("f5d6c4d3c2b3b4f4")));
  EXPECT_FALSE(xot.IsInListPrefix(Sequence("e6f4c3c4d3d6f6e7")));
}

TEST(XOT, RandomPosition) {
  srand(42);
  XOT xot("f5d6c4d3c2b3b4b5\n"
          "f5f4g3g6f3g4e3e2");

  std::unordered_map<Sequence, double> sequences;
  for (int i = 0; i < 10000; ++i) {
    Sequence sequence = xot.RandomSequence();
    sequences[sequence] = GetOrDefault(sequences, sequence, 0.0) + 1;
  }
  EXPECT_THAT(
      sequences,
      UnorderedElementsAre(
          Pair(Sequence("f5d6c4d3c2b3b4b5"), DoubleNear(5000, 100)),
          Pair(Sequence("f5f4g3g6f3g4e3e2"), DoubleNear(5000, 100))
      )
  );
}

TEST(XOT, Transposition) {
  srand(42);
  XOT xot("e6f4c3c4d3d6f6e7");

  EXPECT_FALSE(xot.IsInList(Sequence("e6f4d3c4c3d6f6e7")));
}

TEST(XOT, FindStart) {
  XOT xot("f5d6c4d3c2b3b4b5\n"
          "f5f4g3g6f3g4e3e2");

  EXPECT_THAT(
      xot.FindStart(Sequence("c5e2c3d2a3c6e3a4c1e1d1b1a5f3e6f4f6b6d7a6a7e7c7g4g6d8f8g5h5h4h3g3h2g7h8c8f7h6h7g2g1g8e8b2b7h1f1f2a1a2b8a8")),
      UnorderedElementsAre(Sequence("f5d6c4d3c2b3b4b5"))
  );
}

TEST(XOT, FindStartRotation) {
  XOT xot("f5d6c4d3c2b3b4b5\n"
          "f5f4g3g6f3g4e3e2");

  EXPECT_THAT(
      xot.FindStart(Sequence("f4d3f3g3f6f8")),
      UnorderedElementsAre(Sequence("c4e3f5e6f7g6g5g4"))
  );
}

TEST(XOT, FindStartInvalid) {
  XOT xot("f5d6c4d3c2b3b4b5\n"
          "f5f4g3g6f3g4e3e2");

  EXPECT_THAT(xot.FindStart(Sequence("a1")), IsEmpty());
}

TEST(XOT, FindStartMirroredBoard) {
  XOT xot("f5d6c4d3c2b3b4b5\n"
          "f5f4g3g6f3g4e3e2");

  EXPECT_THAT(
      xot.FindStart(Sequence("c4e7c6d7a6c3e6a5c8e8d8b8a4f6e3f5f3b3d2a3a2e2c2g5g3d1f1g4h4h5h6g6h7g2h1c1f2h3h2g7g8g1e1b7b2h8f8f7a8a7b1a1")),
      IsEmpty()
  );
}

TEST(XOT, FindStartMultiple) {
  XOT xot("f5d6c4d3c2b3b4b5\n"
          "f5f4g3g6f3g4e3e2");

  EXPECT_THAT(
      xot.FindStart(Sequence("c5")),
      UnorderedElementsAre(
          Sequence("f5d6c4d3c2b3b4b5"),
          Sequence("f5f4g3g6f3g4e3e2")
      )
  );
}

TEST(XOT, FindStartMultipleLimit) {
  XOT xot("f5d6c4d3c2b3b4b5\n"
          "f5f4g3g6f3g4e3e2");

  EXPECT_THAT(
      xot.FindStart(Sequence("c5"), 1),
      UnorderedElementsAre(Sequence("f5d6c4d3c2b3b4b5"))
  );
}