/*
 * Copyright 2023-2025 Michele Borassi
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

using ::testing::ElementsAre;
using ::testing::Pair;
using ::testing::UnorderedElementsAre;

TEST(Sequence, MoveMirror) {
  for (int i = 0; i < 64; ++i) {
    ASSERT_EQ(Diag7MirrorMove(i), VerticalMirrorMove(Diag9MirrorMove(VerticalMirrorMove(i)))) << i;
    ASSERT_EQ(Rotate180Move(i), Diag9MirrorMove(VerticalMirrorMove(Diag9MirrorMove(VerticalMirrorMove(i))))) << i;
  }
}

TEST(Sequence, Comparisons) {
  ASSERT_LT(Sequence("e6f4d3c4"), Sequence("e6f4c3c4"));
  ASSERT_LE(Sequence("e6f4d3c4"), Sequence("e6f4c3c4"));
  ASSERT_GT(Sequence("e6f4"), Sequence("e6f4d3"));
  ASSERT_LE(Sequence("e6f4"), Sequence("e6f4"));
  ASSERT_GE(Sequence("e6f4"), Sequence("e6f4"));
  ASSERT_FALSE(Sequence("") < Sequence(""));
  ASSERT_FALSE(Sequence("") > Sequence(""));
}

TEST(Sequence, AddMove) {
  Sequence sequence;
  sequence.AddMove(19);
  EXPECT_EQ(sequence, Sequence("e6"));

  sequence.AddMove(34);
  EXPECT_EQ(sequence, Sequence("e6f4"));

  sequence.RemoveLastMove();
  EXPECT_EQ(sequence, Sequence("e6"));

  sequence.RemoveLastMove();
  EXPECT_EQ(sequence, Sequence());

  sequence.AddMove(19);
  EXPECT_EQ(sequence, Sequence("e6"));
}

TEST(Sequence, Sequence) {
  Sequence sequence1("e6f4c3c4d3");
  Sequence sequence2("f5d6c3d3c4");
  Sequence sequence3("e6f4c3c4d3");
  Sequence sequence4("e6f4c3c4");

  EXPECT_GT(sequence4, sequence1);
  EXPECT_LT(sequence1, sequence2);
  EXPECT_GT(sequence2, sequence1);
  EXPECT_EQ(sequence1, sequence3);
}

TEST(Sequence, AllTranspositionsEmpty) {
  EXPECT_THAT(Sequence("").AllTranspositions(), UnorderedElementsAre(
      Sequence(""),
      Sequence(""),
      Sequence(""),
      Sequence(""),
      Sequence(""),
      Sequence(""),
      Sequence(""),
      Sequence("")
  ));
}

TEST(Sequence, AllTranspositions) {
  EXPECT_THAT(Sequence("e6f4").AllTranspositions(), UnorderedElementsAre(
      Sequence("e6f4"),
      Sequence("d6c4"),
      Sequence("c5e6"),
      Sequence("c4e3"),
      Sequence("d3c5"),
      Sequence("e3f5"),
      Sequence("f4d3"),
      Sequence("f5d6")
  ));
}

TEST(Sequence, AllTranspositionsBoard) {
  Sequence sequence("e6f4c3c4");
  Board b = sequence.ToBoard();
  auto sequence_transpositions = sequence.AllTranspositions();
  auto board_transpositions = b.AllTranspositions();

  int invalid_sequences = 0;
  for (int i = 0; i < sequence_transpositions.size(); ++i) {
    try {
      auto actual = sequence_transpositions[i].ToBoard();
      EXPECT_EQ(actual, board_transpositions[i]);
    } catch(InvalidSequenceException) {
      ++invalid_sequences;
    }
  }
  EXPECT_EQ(invalid_sequences, 4);
}

TEST(Sequence, Transpose) {
  Sequence s("e6f4");
  auto transpositions = s.AllTranspositions();
  for (int i = 0; i < 8; ++i) {
    EXPECT_EQ(s.Transpose(i), transpositions[i]);
    try {
      auto actual = transpositions[i].ToBoard();
      EXPECT_EQ(s.GetTransposition(transpositions[i]), i);
    } catch(InvalidSequenceException) {}
  }
}

TEST(Sequence, ToCanonicalFirstMoveEmpty) {
  EXPECT_EQ(Sequence("").ToCanonicalFirstMove(), Sequence(""));
}

TEST(Sequence, ToCanonicalFirstMove) {
  Sequence sequence1("e6f4c3c4d3");
  Sequence sequence2("f5d6c3d3c4");
  Sequence sequence3("d3c5f6f5e6");
  Sequence sequence4("c4e3f6e6f5");

  for (const Sequence& sequence : {sequence1, sequence2, sequence3, sequence4}) {
    EXPECT_EQ(sequence.ToCanonicalFirstMove(), Sequence("e6f4c3c4d3"));
  }
}

TEST(Sequence, ToCanonicalDiagonal) {
  EXPECT_EQ(Sequence("e6f6f5f4d3").ToCanonicalDiagonal(), Sequence("e6f6f5d6c4"));
  EXPECT_EQ(Sequence("e6f6f5f4").ToCanonicalDiagonal(), Sequence("e6f6f5d6"));
  EXPECT_EQ(Sequence("e6f6f5").ToCanonicalDiagonal(), Sequence("e6f6f5"));
  EXPECT_EQ(Sequence("e6f6f5d6").ToCanonicalDiagonal(), Sequence("e6f6f5d6"));
  EXPECT_EQ(Sequence("e6f4c3c4d3d6").ToCanonicalDiagonal(), Sequence("e6f4c3c4d3d6"));
}

TEST(Sequence, ToCanonicalGame) {
  Sequence sequence1("c4c3d3e3c2");
  Sequence sequence2("c4c3d3c5b3");
  Sequence sequence3("d3c3c4e3c2");
  Sequence sequence4("d3c3c4c5b3");
  Sequence sequence5("f5f6e6d6f7");
  Sequence sequence6("f5f6e6f4g6");
  Sequence sequence7("e6f6f5d6f7");
  Sequence sequence8("e6f6f5f4g6");

  for (const Sequence& sequence : {sequence1, sequence2, sequence3, sequence4, sequence5, sequence6, sequence7, sequence8}) {
    EXPECT_EQ(sequence.ToCanonicalGame(), Sequence("e6f6f5d6f7"));
  }
}

TEST(Sequence, ParseFromString) {
  EXPECT_EQ(Sequence::ParseFromString("e6f4"), Sequence("e6f4"));
  EXPECT_EQ(Sequence::ParseFromString("0. abcdt 78"), Sequence(""));
  EXPECT_EQ(Sequence::ParseFromString("1. e6 2. f4"), Sequence("e6f4"));
  EXPECT_EQ(Sequence::ParseFromString("1. e6 2. f4\n3.c3"), Sequence("e6f4c3"));
  EXPECT_EQ(Sequence::ParseFromString("e6 f4 a1 \n3.c3"), Sequence("e6f4c3"));
  EXPECT_EQ(Sequence::ParseFromString("e6 f4 e6 \n3.c3"), Sequence("e6f4c3"));
  EXPECT_EQ(Sequence::ParseFromString("E6 F4 e6 \n3.c3"), Sequence("e6f4c3"));
}

TEST(Sequence, StartsWith) {
  Sequence sequence("e6f4c3c4d3");
  EXPECT_TRUE(sequence.StartsWith(Sequence("")));
  EXPECT_TRUE(sequence.StartsWith(Sequence("e6f4")));
  EXPECT_TRUE(sequence.StartsWith(Sequence("e6f4c3c4d3")));
  EXPECT_FALSE(sequence.StartsWith(Sequence("e6f4c3c4d3d6")));
  EXPECT_FALSE(sequence.StartsWith(Sequence("e6f6")));
  EXPECT_FALSE(sequence.StartsWith(Sequence("f5")));
}

TEST(Sequence, FromThor) {
  Square sequence[60] = {65, 46, 33, 43, 34};
  EXPECT_EQ(Sequence::FromThor(sequence), Sequence("e6f4c3c4d3"));
}

TEST(Sequence, FromToThor) {
  for (int i = 0; i < 1000; ++i) {
    Sequence sequence = Sequence::RandomSequence(rand() % 60);
    std::vector<Square> thor = sequence.ToThor();
    EXPECT_EQ(Sequence::FromThor(thor.data()), sequence);
  }
}

TEST(Sequence, Subsequence) {
  Sequence sequence("e6f4c3c4d3");
  EXPECT_EQ(sequence.Subsequence(0), Sequence());
  EXPECT_EQ(sequence.Subsequence(4), Sequence("e6f4c3c4"));
  EXPECT_EQ(sequence.Subsequence(5), Sequence("e6f4c3c4d3"));
}

TEST(Sequence, ReplaceStart) {
  Sequence sequence("e6f4c3c4d3d6");
  Sequence replace("e6f4d3c4c3");
  sequence.ReplaceStart(replace);
  EXPECT_EQ(sequence, Sequence("e6f4d3c4c3d6"));
}

TEST(Sequence, RandomSequence) {
  std::unordered_set<Sequence> sequences;
  for (int i = 0; i < 100; ++i) {
    for (int length = 0; length <= 60; ++length) {
      Sequence sequence = Sequence::RandomSequence(length);
      ASSERT_EQ(sequence.Size(), length);
      // To check that the sequence is valid.
      sequence.ToBoards();
      if (length > 15) {
        EXPECT_TRUE(sequences.find(sequence) == sequences.end()) << "Repeated sequence " << sequence;
        sequences.insert(sequence);
      }
    }
  }
  EXPECT_EQ(sequences.size(), 100 * 45);
}

TEST(Sequence, ToBoards) {
  Sequence sequence("e6f4");
  EXPECT_THAT(
      sequence.ToBoards(), ElementsAre(Board(), Board("e6"), Board("e6f4"))
  );
  EXPECT_EQ(sequence.ToBoard(0), Board());
  EXPECT_EQ(sequence.ToBoard(1), Board("e6"));
  EXPECT_EQ(sequence.ToBoard(2), Board("e6f4"));
  EXPECT_EQ(sequence.ToBoard(-1), Board("e6f4"));
  EXPECT_EQ(sequence.ToBoard(-2), Board("e6"));
  EXPECT_EQ(sequence.ToBoard(-3), Board());
}

TEST(Sequence, ToBoardsAndBlackTurns) {
  EXPECT_THAT(
      Sequence("e6f6g6g7f5f4h8h7g4d6g5h4h6g8f8h5h3").ToBoardsAndBlackTurns(),
      ElementsAre(Pair(Board(), true),
                  Pair(Board("e6"), false),
                  Pair(Board("e6f6"), true),
                  Pair(Board("e6f6g6"), false),
                  Pair(Board("e6f6g6g7"), true),
                  Pair(Board("e6f6g6g7f5"), false),
                  Pair(Board("e6f6g6g7f5f4"), true),
                  Pair(Board("e6f6g6g7f5f4h8"), false),
                  Pair(Board("e6f6g6g7f5f4h8h7"), true),
                  Pair(Board("e6f6g6g7f5f4h8h7g4"), false),
                  Pair(Board("e6f6g6g7f5f4h8h7g4d6"), true),
                  Pair(Board("e6f6g6g7f5f4h8h7g4d6g5"), false),
                  Pair(Board("e6f6g6g7f5f4h8h7g4d6g5h4"), true),
                  Pair(Board("e6f6g6g7f5f4h8h7g4d6g5h4h6"), false),
                  Pair(Board("e6f6g6g7f5f4h8h7g4d6g5h4h6g8"), true),
                  Pair(Board("e6f6g6g7f5f4h8h7g4d6g5h4h6g8f8"), true),
                  Pair(Board("e6f6g6g7f5f4h8h7g4d6g5h4h6g8f8h5"), true),
                  Pair(Board("e6f6g6g7f5f4h8h7g4d6g5h4h6g8f8h5h3"), false)
  ));
}

TEST(Sequence, ToBoardAndBlackTurn) {
  EXPECT_EQ(Sequence("e6f4").ToBoardAndBlackTurn(0), std::pair(Board(), true));
  EXPECT_EQ(Sequence("").ToBoardAndBlackTurn(), std::pair(Board(), true));
  EXPECT_EQ(Sequence("e6f4").ToBoardAndBlackTurn(1), std::pair(Board("e6"), false));
  EXPECT_EQ(Sequence("e6f4").ToBoardAndBlackTurn(), std::pair(Board("e6f4"), true));
  // Before pass.
  EXPECT_EQ(
      Sequence("e6f6g6g7g8h8c4e7e8f8g5d8").ToBoardAndBlackTurn(),
      std::pair(Board("e6f6g6g7g8h8c4e7e8f8g5d8"), false));
  // After pass.
  EXPECT_EQ(
      Sequence("e6f6g6g7g8h8c4e7e8f8g5d8h4").ToBoardAndBlackTurn(),
      std::pair(Board("e6f6g6g7g8h8c4e7e8f8g5d8h4"), true));
}

TEST(Sequence, IsValid) {
  EXPECT_TRUE(Sequence("").IsValid());
  EXPECT_TRUE(Sequence("e6f4").IsValid());
  EXPECT_TRUE(Sequence("e6f4c3c4d3d6e3c2b3d2c5f5f3f6e1d1e2f1g4g3g5h5f2h4c7g6e7b4h3a4b1g1b5h2f7d8e8f8d7c8c6b8a3a2g7h6h8h7b2a1h1c1g8b6a5g2a7b7a8a6").IsValid());
  EXPECT_FALSE(Sequence("e6e7").IsValid());
  EXPECT_FALSE(Sequence("e6f4c3c4d3d6e3c2b3d2c5f5f3f6e1d1e2f1g4g3g5h5f2h4c7g6e7b4h3a4b1g1b5h2f7d8e8f8d7c8c6b8a3a2g7h6h8h7b2a1h1c1g8b6a5g2a7b7a8a4").IsValid());
}

TEST(Sequence, Unique) {
  EXPECT_EQ(Sequence("e6").Unique(), Sequence("e6"));
  EXPECT_EQ(Sequence("c4").Unique(), Sequence("e6"));
  EXPECT_EQ(Sequence("d3c4c3").Unique(), Sequence("d3c4c3"));
  EXPECT_EQ(Sequence("d6f4c3").Unique(), Sequence("d6f4c3"));
  EXPECT_EQ(Sequence("").Unique(), Sequence(""));
}


TEST(Sequence, SequenceCanonicalizer) {
  std::vector<Sequence> sequences = {
    Sequence("e6f4d3c4c3"),
    Sequence("e6f4c3c4d3")
  };
  SequenceCanonicalizer canonicalizer(sequences);
  EXPECT_THAT(
      canonicalizer.AllEquivalentSequences(Sequence("e6f4c3c4d3d6")),
      UnorderedElementsAre(Sequence("e6f4d3c4c3d6"), Sequence("e6f4c3c4d3d6"))
  );
}

TEST(Sequence, SequenceCanonicalizerDouble) {
  std::vector<Sequence> sequences = {
    Sequence("e6f4c3c4d3e2d2d6e3"),
    Sequence("e6f4"),
    Sequence("e6f4d3c4c3d6e3e2d2"),
  };
  // e6f4c3c4d3 becomes e6f4d3c4c3
  // e2d2d6e3 becomes d6e3e2d2
  // Canonical is e6f4d3c4c3d6e3e2d2
  SequenceCanonicalizer canonicalizer(sequences);
  EXPECT_THAT(
      canonicalizer.AllEquivalentSequences(Sequence("e6f4c3c4d3d6")),
      UnorderedElementsAre(Sequence("e6f4d3c4c3d6"), Sequence("e6f4c3c4d3d6"))
  );
  EXPECT_THAT(
      canonicalizer.AllEquivalentSequences(Sequence("e6f4c3c4d3e2d2d6e3")),
      UnorderedElementsAre(
          Sequence("e6f4c3c4d3e2d2d6e3"),
          Sequence("e6f4d3c4c3d6e3e2d2")
      )
  );
  EXPECT_THAT(
      canonicalizer.AllEquivalentSequences(Sequence("e6f4c3c4d3e2d2d6e3c2")),
      UnorderedElementsAre(
          Sequence("e6f4c3c4d3e2d2d6e3c2"),
          Sequence("e6f4d3c4c3d6e3e2d2c2")
      )
  );
}

TEST(Sequence, SequenceCanonicalizerA2B1A1B2) {
  // Same as before, but with inverted sequences.
  std::vector<Sequence> sequences = {
    Sequence("e6f4c3c4d3d6e3e2d2"),
    Sequence("e6f4d3c4c3e2d2d6e3"),
  };
  // e6f4c3c4d3 becomes e6f4d3c4c3
  // e2d2d6e3 becomes d6e3e2d2
  // Canonical is e6f4d3c4c3d6e3e2d2
  SequenceCanonicalizer canonicalizer(sequences);
  EXPECT_THAT(
      canonicalizer.AllEquivalentSequences(Sequence("e6f4c3c4d3d6")),
      UnorderedElementsAre(Sequence("e6f4d3c4c3d6"), Sequence("e6f4c3c4d3d6"))
  );
  EXPECT_THAT(
      canonicalizer.AllEquivalentSequences(Sequence("e6f4c3c4d3e2d2d6e3")),
      UnorderedElementsAre(
          Sequence("e6f4c3c4d3d6e3e2d2"),
          Sequence("e6f4d3c4c3e2d2d6e3")
      )
  );
  EXPECT_THAT(
      canonicalizer.AllEquivalentSequences(Sequence("e6f4c3c4d3e2d2d6e3c2")),
      UnorderedElementsAre(
          Sequence("e6f4c3c4d3d6e3e2d2c2"),
          Sequence("e6f4d3c4c3e2d2d6e3c2")
      )
  );
}

TEST(Sequence, SequenceCanonicalizerTransposition) {
  std::vector<Sequence> sequences = {
    Sequence("e6f6f5d6c5"),
    Sequence("e6f6f5f4"),
  };
  SequenceCanonicalizer canonicalizer(sequences);
  EXPECT_THAT(
      canonicalizer.AllEquivalentSequences(Sequence("e6f6f5d6")),
      UnorderedElementsAre(Sequence("e6f6f5d6"), Sequence("e6f6f5f4"))
  );
  EXPECT_THAT(
      canonicalizer.AllEquivalentSequences(Sequence("e6f6f5f4")),
      UnorderedElementsAre(Sequence("e6f6f5d6"), Sequence("e6f6f5f4"))
  );
  EXPECT_THAT(
      canonicalizer.AllEquivalentSequences(Sequence("e6f6f5f4g6")),
      UnorderedElementsAre(Sequence("e6f6f5f4g6"), Sequence("e6f6f5d6f7"))
  );
}

TEST(Sequence, SequenceCanonicalizerTranspositionDouble) {
  std::vector<Sequence> sequences = {
    Sequence("e6f6f5f4e3d6c6c5"),
    Sequence("e6f6f5d6c5e3f3f4"),
  };
  SequenceCanonicalizer canonicalizer(sequences);
  EXPECT_THAT(
      canonicalizer.AllEquivalentSequences(Sequence("e6f6f5f4e3c5c6d6")),
      UnorderedElementsAre(
          Sequence("e6f6f5f4e3d6c6c5"),
          Sequence("e6f6f5d6c5e3f3f4")
      )
  );
  EXPECT_THAT(
      canonicalizer.AllEquivalentSequences(Sequence("e6f6f5f4e3c5c6d6e7")),
      UnorderedElementsAre(
          Sequence("e6f6f5f4e3d6c6c5e7"),
          Sequence("e6f6f5d6c5e3f3f4g5")
      )
  );
}

TEST(Sequence, SequenceCanonicalizerSameLastMove) {
  // We keep the sequences even if they have the same last move.
  std::vector<Sequence> sequences = {
    Sequence("e6f4e3f6f5d6g4f3"),
    Sequence("e6f6f5f4e3d6g4f3"),
  };
  SequenceCanonicalizer canonicalizer(sequences);
  EXPECT_THAT(
      canonicalizer.AllEquivalentSequences(Sequence("e6f6f5f4e3d6g4f3")),
      UnorderedElementsAre(
          Sequence("e6f6f5f4e3d6g4f3"),
          Sequence("e6f4e3f6f5d6g4f3")
      )
  );
}

TEST(Sequence, SequenceCanonicalizerIgnoresFullGame) {
  std::vector<Sequence> sequences = {
    Sequence("e6f6f5d6f7f4f3g5h6f8c4h4e7d3e3g4g3d2h3h2h5h7g6f2c7b4c5b5e2c6d7c8c3b3d8e8d1f1e1c1c2b1b6b7a6a4a5a7a2b2a1a3a8g1g2b8h1g8g7h8"),
    Sequence("e6f6f5d6c5e3d3g5d7c6e7c4g4f7b4f4h5f3g6b5e8c7e2d8a5a3b6a4a6a7c8f2d2c3b3c1d1e1c2h4h3b1g3a2g1g8f8b8b2a1b7a8f1g2h7h6h2g7h1h8"),
  };
  SequenceCanonicalizer canonicalizer(sequences);
  EXPECT_THAT(
      canonicalizer.AllEquivalentSequences(Sequence("e6f6f5d6c5e3d3g5d7c6e7c4g4f7b4f4h5f3g6b5e8c7e2d8a5a3b6a4a6a7c8f2d2c3b3c1d1e1c2h4h3b1g3a2g1g8f8b8b2a1b7a8f1g2h7h6h2g7h1h8")),
      UnorderedElementsAre(
          Sequence("e6f6f5d6c5e3d3g5d7c6e7c4g4f7b4f4h5f3g6b5e8c7e2d8a5a3b6a4a6a7c8f2d2c3b3c1d1e1c2h4h3b1g3a2g1g8f8b8b2a1b7a8f1g2h7h6h2g7h1h8")
      )
  );
}

TEST(Sequence, SequenceCanonicalizerSerialize) {
  std::vector<Sequence> sequences = {
    Sequence("e6f6f5f4e3d6c6c5"),
    Sequence("e6f6f5d6c5e3f3f4"),
  };

  SequenceCanonicalizer canonicalizer(sequences);
  SequenceCanonicalizer copy;
  copy.Load(canonicalizer.Serialize());

  EXPECT_THAT(
      canonicalizer.AllEquivalentSequences(Sequence("e6f6f5f4e3c5c6d6")),
      UnorderedElementsAre(
          Sequence("e6f6f5f4e3d6c6c5"),
          Sequence("e6f6f5d6c5e3f3f4")
      )
  );
  EXPECT_THAT(
      canonicalizer.AllEquivalentSequences(Sequence("e6f6f5f4e3c5c6d6e7")),
      UnorderedElementsAre(
          Sequence("e6f6f5f4e3d6c6c5e7"),
          Sequence("e6f6f5d6c5e3f3f4g5")
      )
  );
}

TEST(Sequence, ExtendStandard) {
  Sequence s("e6");
  EXPECT_TRUE(s.Extend(Sequence("f4")));
  EXPECT_EQ(s, Sequence("e6f4"));
}

TEST(Sequence, ExtendBarelyIn) {
  Sequence s("e6f4c3c4d3d6e3c2b3d2c5f5f3f6e1d1e2f1g4g3g5h5f2h4c7g6e7b4h3");
  EXPECT_TRUE(s.Extend(Sequence("a4b1g1b5h2f7d8e8f8d7c8c6b8a3a2g7h6h8h7b2a1h1c1g8b6a5g2a7b7a8a6")));
  EXPECT_EQ(s, Sequence("e6f4c3c4d3d6e3c2b3d2c5f5f3f6e1d1e2f1g4g3g5h5f2h4c7g6e7b4h3a4b1g1b5h2f7d8e8f8d7c8c6b8a3a2g7h6h8h7b2a1h1c1g8b6a5g2a7b7a8a6"));
}

TEST(Sequence, ExtendEmpty) {
  Sequence s("e6f4c3c4d3");
  EXPECT_TRUE(s.Extend(Sequence("")));
  EXPECT_EQ(s, Sequence("e6f4c3c4d3"));
}

TEST(Sequence, ExtendTooLong) {
  Sequence s("e6f4c3c4d3d6e3c2b3d2c5f5f3f6e1d1e2f1g4g3g5h5f2h4c7g6e7b4h3");
  EXPECT_FALSE(s.Extend(Sequence("a4b1g1b5h2f7d8e8f8d7c8c6b8a3a2g7h6h8h7b2a1h1c1g8b6a5g2a7b7a8a6a6")));
  EXPECT_EQ(s, Sequence("e6f4c3c4d3d6e3c2b3d2c5f5f3f6e1d1e2f1g4g3g5h5f2h4c7g6e7b4h3"));
}