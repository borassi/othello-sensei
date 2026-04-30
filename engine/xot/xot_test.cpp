/*
 * Copyright 2025-2026 Michele Borassi
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
#include <filesystem>
#include <fstream>
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

TEST(Xot, Name) {
  Xot default_xot("f5d6c4d3c2b3b4b5");
  EXPECT_EQ(default_xot.Name(), "");

  Xot named_xot("f5f4g3g6f3g4e3e2", "file1.txt");
  EXPECT_EQ(named_xot.Name(), "file1.txt");
}

TEST(Xot, IsInList) {
  Xot xot("f5d6c4d3c2b3b4b5\n"
          "f5f4g3g6f3g4e3e2");

  EXPECT_TRUE(xot.IsInList(Sequence("f5d6c4d3c2b3b4b5")));
  EXPECT_TRUE(xot.IsInList(Sequence("f5f4g3g6f3g4e3e2")));
  EXPECT_TRUE(xot.IsInList(Sequence("c4e3f5e6f7g6g5g4")));
  EXPECT_FALSE(xot.IsInList(Sequence("")));
  EXPECT_FALSE(xot.IsInList(Sequence("e6f4c3c4d3d6f6e7")));
}

TEST(Xot, IsInListPrefix) {
  Xot xot("f5d6c4d3c2b3b4b5\n"
          "f5f4g3g6f3g4e3e2");

  EXPECT_TRUE(xot.IsInListPrefix(Sequence("f5d6c4d3c2b3b4b5")));
  EXPECT_TRUE(xot.IsInListPrefix(Sequence("f5f4g3g6f3g4e3e2")));
  EXPECT_TRUE(xot.IsInListPrefix(Sequence("c4e3f5e6f7g6g5g4")));
  EXPECT_TRUE(xot.IsInListPrefix(Sequence("f5d6c4d3c2b3b4b5a4d2")));
  EXPECT_FALSE(xot.IsInListPrefix(Sequence("")));
  EXPECT_FALSE(xot.IsInListPrefix(Sequence("f5d6c4d3c2b3b4f4")));
  EXPECT_FALSE(xot.IsInListPrefix(Sequence("e6f4c3c4d3d6f6e7")));
}

TEST(Xot, RandomPosition) {
  srand(42);
  Xot xot("f5d6c4d3c2b3b4b5\n"
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

TEST(Xot, Transposition) {
  srand(42);
  Xot xot("e6f4c3c4d3d6f6e7");

  EXPECT_FALSE(xot.IsInList(Sequence("e6f4d3c4c3d6f6e7")));
}

TEST(Xot, FindStart) {
  Xot xot("f5d6c4d3c2b3b4b5\n"
          "f5f4g3g6f3g4e3e2");

  EXPECT_THAT(
      xot.FindStart(Sequence("c5e2c3d2a3c6e3a4c1e1d1b1a5f3e6f4f6b6d7a6a7e7c7g4g6d8f8g5h5h4h3g3h2g7h8c8f7h6h7g2g1g8e8b2b7h1f1f2a1a2b8a8")),
      UnorderedElementsAre(Sequence("f5d6c4d3c2b3b4b5"))
  );
}

TEST(Xot, FindStartRotation) {
  Xot xot("f5d6c4d3c2b3b4b5\n"
          "f5f4g3g6f3g4e3e2");

  EXPECT_THAT(
      xot.FindStart(Sequence("f4d3f3g3f6f8")),
      UnorderedElementsAre(Sequence("c4e3f5e6f7g6g5g4"))
  );
}

TEST(Xot, FindStartInvalid) {
  Xot xot("f5d6c4d3c2b3b4b5\n"
          "f5f4g3g6f3g4e3e2");

  EXPECT_THAT(xot.FindStart(Sequence("a1")), IsEmpty());
}

TEST(Xot, FindStartMirroredBoard) {
  Xot xot("f5d6c4d3c2b3b4b5\n"
          "f5f4g3g6f3g4e3e2");

  EXPECT_THAT(
      xot.FindStart(Sequence("c4e7c6d7a6c3e6a5c8e8d8b8a4f6e3f5f3b3d2a3a2e2c2g5g3d1f1g4h4h5h6g6h7g2h1c1f2h3h2g7g8g1e1b7b2h8f8f7a8a7b1a1")),
      IsEmpty()
  );
}

TEST(Xot, FindStartMultiple) {
  Xot xot("f5d6c4d3c2b3b4b5\n"
          "f5f4g3g6f3g4e3e2");

  EXPECT_THAT(
      xot.FindStart(Sequence("c5")),
      UnorderedElementsAre(
          Sequence("f5d6c4d3c2b3b4b5"),
          Sequence("f5f4g3g6f3g4e3e2")
      )
  );
}

TEST(Xot, FindStartMultipleLimit) {
  Xot xot("f5d6c4d3c2b3b4b5\n"
          "f5f4g3g6f3g4e3e2");

  EXPECT_THAT(
      xot.FindStart(Sequence("c5"), 1),
      UnorderedElementsAre(Sequence("f5d6c4d3c2b3b4b5"))
  );
}

TEST(Xot, DifferentSize) {
  Xot xot("f5d6c4d3\n"
          "f5f4g3g6");
  EXPECT_EQ(xot.SequenceSize(), 4);
  EXPECT_TRUE(xot.IsInList(Sequence("f5d6c4d3")));
  EXPECT_TRUE(xot.IsInList(Sequence("f5f4g3g6")));
  EXPECT_TRUE(xot.IsInList(Sequence("c4e3f5e6")));
  EXPECT_FALSE(xot.IsInList(Sequence("")));
  EXPECT_FALSE(xot.IsInList(Sequence("e6f4d3c4c3")));

  EXPECT_TRUE(xot.IsInListPrefix(Sequence("f5d6c4d3")));
  EXPECT_TRUE(xot.IsInListPrefix(Sequence("f5f4g3g6")));
  EXPECT_TRUE(xot.IsInListPrefix(Sequence("c4e3f5e6")));
  EXPECT_TRUE(xot.IsInListPrefix(Sequence("f5d6c4d3c2")));
  EXPECT_FALSE(xot.IsInListPrefix(Sequence("")));
  EXPECT_FALSE(xot.IsInListPrefix(Sequence("f5d6c4b3")));
}

namespace fs = std::filesystem;
const std::string kTestDataFolder = "tmp/testdata/xot";

class XotHandlerTest : public testing::Test {
 protected:
  void SetUp() override {
    // In case one test failed and did not clean up.
    fs::remove_all(kTestDataFolder);
    fs::create_directories(kTestDataFolder);
  }

  void TearDown() override {
    fs::remove_all(kTestDataFolder);
  }

  void CreateXotFile(const std::string& filename, const std::string& content) {
    std::ofstream file(kTestDataFolder + "/" + filename);
    file << content;
  }
};

TEST_F(XotHandlerTest, Empty) {
  XotHandler xot_handler(kTestDataFolder);
  EXPECT_EQ(xot_handler.GetPrefixLength(Sequence()), -1);
  EXPECT_EQ(xot_handler.GetPrefixLength(Sequence("e6f4")), -1);
  EXPECT_FALSE(xot_handler.IsInList(Sequence("e6f4")));
  EXPECT_TRUE(xot_handler.SourceNames().empty());
}

TEST_F(XotHandlerTest, LoadsMultipleFiles) {
  CreateXotFile("file1.txt", "f5d6c4d3c2b3b4b5\n");
  CreateXotFile("file2.txt", "f5f4g3g6f3g4e3e2\n");

  XotHandler xot_handler(kTestDataFolder);

  EXPECT_THAT(xot_handler.SourceNames(), UnorderedElementsAre("file1", "file2"));
  EXPECT_TRUE(xot_handler.IsInList(Sequence("f5d6c4d3c2b3b4b5")));
  EXPECT_TRUE(xot_handler.IsInList(Sequence("f5f4g3g6f3g4e3e2")));
  EXPECT_FALSE(xot_handler.IsInList(Sequence("e6f4c3c4d3d6f6e7")));
}

TEST_F(XotHandlerTest, DuplicateNames) {
  // Files that resolve to the exact same base name.
  CreateXotFile("001 - match.txt", "e6f4\n");
  CreateXotFile("002 - match.txt", "e6f6\n");
  CreateXotFile("003 - match.xot", "e6d6\n");

  XotHandler xot_handler(kTestDataFolder);

  // First occurrence gets the base name, subsequent occurrences
  // get (2), (3), etc.
  EXPECT_THAT(
      xot_handler.SourceNames(),
      ::testing::ElementsAre("match", "match (2)", "match (3)")
  );
}

TEST_F(XotHandlerTest, MixedPrefixesAndAlphabeticalOrder) {
  // Lexicographical order of these raw filenames is:
  // 1. "02 - zebra.txt"
  // 2. "1 - apple.txt"
  // 3. "banana.txt" (no prefix)
  CreateXotFile("02 - zebra.txt", "e6f4\n");
  CreateXotFile("1 - apple.txt", "e6f6\n");
  CreateXotFile("banana.txt", "e6d6\n");

  XotHandler xot_handler(kTestDataFolder);

  // The resulting list follows the strict lexicographical order of the
  // original filenames, not the stripped names or integer prefix values.
  EXPECT_THAT(
      xot_handler.SourceNames(),
      ::testing::ElementsAre("zebra", "apple", "banana")
  );
}

TEST_F(XotHandlerTest, GetPrefixLength) {
  CreateXotFile("file1.txt", "f5d6c4d3c2b3b4b5\n");
  XotHandler xot_handler(kTestDataFolder);

  EXPECT_EQ(xot_handler.GetPrefixLength(Sequence("f5d6c4d3c2b3b4b5")), 8);
  EXPECT_EQ(xot_handler.GetPrefixLength(Sequence("f5d6c4d3c2b3b4b5a4d2")), 8);
  EXPECT_EQ(xot_handler.GetPrefixLength(Sequence("f5d6c4d3")), -1);
}

TEST_F(XotHandlerTest, RandomSequence) {
  CreateXotFile("file1.txt", "f5d6c4d3c2b3b4b5\n");
  XotHandler xot_handler(kTestDataFolder);

  EXPECT_EQ(xot_handler.RandomSequence("file1").ToString(), "f5d6c4d3c2b3b4b5");
}

TEST_F(XotHandlerTest, FilenameParsingAndStripping) {
  // Using different prefix lengths between test runs to ensure we don't assume the length,
  // but within this specific folder/run, they have the same length as per the assumption.
  CreateXotFile("000 - file2.txt", "f5d6c4d3\n");
  CreateXotFile("001 - file1.txt", "f5d6c4d3c2\n");
  CreateXotFile("002 - another_file.xot", "f5f4g3g6\n");

  XotHandler xot_handler(kTestDataFolder);

  // Checks that the prefix ("XXX - ") and the extensions (".txt", ".xot") are removed.
  EXPECT_THAT(
      xot_handler.SourceNames(),
      UnorderedElementsAre("file2", "file1", "another_file")
  );

  // Verify we can access the sequences using the parsed names
  EXPECT_EQ(xot_handler.RandomSequence("file1").ToString(), "f5d6c4d3c2");
}

TEST_F(XotHandlerTest, FirstFilePrioritized) {
  CreateXotFile("000 - file2.txt", "f5d6c4d3\n");      // 4 moves
  CreateXotFile("001 - file1.txt", "f5d6c4d3c2\n");    // 5 moves

  XotHandler xot_handler(kTestDataFolder);

  // The sequence f5d6c4d3c2b3 has 6 moves.
  // "000 - file2.txt" is loaded first, so it matches the 4-move prefix
  // and returns 4, ignoring the 5-move match in the second loaded file.
  EXPECT_EQ(xot_handler.GetPrefixLength(Sequence("f5d6c4d3c2b3")), 4);
}

TEST_F(XotHandlerTest, FallbackToShorterSequence) {
  CreateXotFile("00 - file1.txt", "f5d6c4d3c2\n"); // 5 moves
  CreateXotFile("01 - file2.txt", "f5d6c4d3\n");   // 4 moves

  XotHandler xot_handler(kTestDataFolder);

  // The sequence f5d6c4d3d7 has 5 moves.
  // The first loaded file ("000 - file1.txt") expects f5d6c4d3c2, so it fails.
  // The second loaded file ("001 - file2.txt") expects f5d6c4d3, which matches, returning 4.
  EXPECT_EQ(xot_handler.GetPrefixLength(Sequence("f5d6c4d3d7")), 4);
}

TEST_F(XotHandlerTest, FirstFileSequenceSize) {
  CreateXotFile("001 - file1.txt", "f5d6c4d3c2\n"); // 5 moves
  CreateXotFile("002 - file2.txt", "f5d6c4d3\n");   // 4 moves

  XotHandler xot_handler(kTestDataFolder);

  EXPECT_EQ(xot_handler.FirstFileSequenceSize(), 5);
}

TEST_F(XotHandlerTest, FirstFileSequenceSizeEmpty) {
  XotHandler xot_handler(kTestDataFolder);

  EXPECT_EQ(xot_handler.FirstFileSequenceSize(), -1);
}