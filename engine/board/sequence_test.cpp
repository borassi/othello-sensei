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

using ::testing::ElementsAre;
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

TEST(Sequence, FromThor) {
  Square sequence[60] = {65, 46, 33, 43, 34};
  EXPECT_EQ(Sequence::FromThor(sequence), Sequence("e6f4c3c4d3"));
}

TEST(Sequence, Subsequence) {
  Sequence sequence("e6f4c3c4d3");
  EXPECT_EQ(sequence.Subsequence(4), Sequence("e6f4c3c4"));
  EXPECT_EQ(sequence.Subsequence(5), Sequence("e6f4c3c4d3"));
}

TEST(Sequence, ReplaceStart) {
  Sequence sequence("e6f4c3c4d3d6");
  Sequence replace("e6f4d3c4c3");
  sequence.ReplaceStart(replace);
  EXPECT_EQ(sequence, Sequence("e6f4d3c4c3d6"));
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
