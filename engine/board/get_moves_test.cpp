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

#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <vector>

#include "bitpattern.h"
#include "board.h"
#include "get_flip.h"
#include "get_moves.h"

using testing::UnorderedElementsAre;
using testing::Pair;

TEST(GetMoves, CompareWithBasic) {
  int n = 10000;
  for (int i = 0; i < n; i++) {
    Board b = RandomBoard();
    BitPattern moves_basic = GetMovesBasic(b.Player(), b.Opponent());
    BitPattern moves = GetMoves(b.Player(), b.Opponent());
    ASSERT_EQ(moves, moves_basic);
  }
}

TEST(GetMoves, GetAllMoves) {
  int n = 10000;
  for (int i = 0; i < n; i++) {
    Board b = RandomBoard();
    std::vector<BitPattern> moves = GetAllMoves(b.Player(), b.Opponent());
    std::set<BitPattern> moves_set(moves.begin(), moves.end());
    ASSERT_TRUE(moves.size() == moves_set.size());
    int n_moves = 0;

    for (Square x = 0; x < 64; ++x) {
      if ((1ULL << x) & (b.Player() | b.Opponent())) {
        continue;
      }
      long flip = GetFlipBasic(x, b.Player(), b.Opponent());
      if (flip == 0) {
        continue;
      }
      ASSERT_THAT(moves_set, ::testing::Contains(flip));
      n_moves++;
    }
    if (n_moves == 0) {
      ASSERT_TRUE(HaveToPass(b.Player(), b.Opponent()));
    }
    ASSERT_EQ(moves.size(), n_moves);
  }
}

TEST(GetMoves, GetAllMovesWithPass) {
  Board b;
  ASSERT_THAT(
      GetAllMovesWithPass(b.Player(), b.Opponent()),
      ::testing::UnorderedElementsAre(
          ParsePattern(
              "--------"
              "--------"
              "--------"
              "--------"
              "----X---"
              "----X---"
              "--------"
              "--------"),
          ParsePattern(
              "--------"
              "--------"
              "--------"
              "--------"
              "----XX--"
              "--------"
              "--------"
              "--------"),
          ParsePattern(
              "--------"
              "--------"
              "--------"
              "--XX----"
              "--------"
              "--------"
              "--------"
              "--------"),
          ParsePattern(
              "--------"
              "--------"
              "---X----"
              "---X----"
              "--------"
              "--------"
              "--------"
              "--------"))
  );
}

TEST(GetMoves, GetNextBoardsWithPass) {
  Board b;
  ASSERT_THAT(
      GetNextBoardsWithPass(b),
      ::testing::UnorderedElementsAre(
          Board(
              "--------"
              "--------"
              "--------"
              "---XO---"
              "---OO---"
              "----O---"
              "--------"
              "--------", true),
          Board(
              "--------"
              "--------"
              "--------"
              "---XO---"
              "---OOO--"
              "--------"
              "--------"
              "--------", true),
          Board(
              "--------"
              "--------"
              "--------"
              "--OOO---"
              "---OX---"
              "--------"
              "--------"
              "--------", true),
          Board(
              "--------"
              "--------"
              "---O----"
              "---OO---"
              "---OX---"
              "--------"
              "--------"
              "--------", true))
  );
}

TEST(GetMoves, SquareFromFlip) {
  int n = 10000;
  for (int i = 0; i < n; i++) {
    Board b = RandomBoard();
    for (Square x = 0; x < 64; ++x) {
      if ((1ULL << x) & (b.Player() | b.Opponent())) {
        continue;
      }
      long flip = GetFlipBasic(x, b.Player(), b.Opponent());
      if (flip != 0) {
        ASSERT_EQ(SquareFromFlip(flip, b.Player(), b.Opponent()), 1ULL << x)
        << b;
      }
    }
  }
}

TEST(GetMoves, SerializedRow) {
  for (int start = 0; start < 8; ++start) {
    for (int end = start; end < 8; ++end) {
      LastRow flip = 0;
      for (int i = start; i <= end; ++i) {
        flip |= 1 << i;
      }
      for (int square = start; square <= end; ++square) {
        // If I flip the first / last element in the row, then it must be where
        // I play.
        if ((end == 7 && square != 7) || (start == 0 && square != 0)) {
          continue;
        }
        ASSERT_LE(SerializeRow(square, flip), 15); // 4 bits
        ASSERT_EQ(flip, DeserializeRow(square, SerializeRow(square, flip)));
      }
    }
  }
}

TEST(GetMoves, SerializeFlip) {
  int n = 10000;
  for (int i = 0; i < n; i++) {
    Board b = RandomBoard();
    auto flips = GetAllMoves(b.Player(), b.Opponent());
    for (BitPattern flip : flips) {
      Board next = b;
      next.PlayMove(flip);
      Square square = __builtin_ctzll(
          SquareFromFlip(flip, b.Player(), b.Opponent()));
      auto serialized = SerializeFlip(square, flip);
      ASSERT_LT(serialized, 1 << 22);  // Less than 22 bits
      auto deserialized = DeserializeFlip(serialized);
      ASSERT_EQ(deserialized.first, square)
          << "Original\n" << b << "After flip\n" << next << "Flip\n" <<
          PatternToString(flip) << "Got" << PatternToString(deserialized
          .second);
      ASSERT_EQ(deserialized.second, flip)
          << "Original\n" << b << "After flip\n" << next << "Flip\n" <<
          PatternToString(flip) << "Got" << PatternToString(deserialized
          .second);
    }
  }
}

TEST(GetUniqueNextBoardsWithPass, Base) {
  Board b("d3");
  auto unique_next_boards = GetUniqueNextBoardsWithPass(b);
  EXPECT_THAT(
      unique_next_boards,
      UnorderedElementsAre(
          Pair(Board("e6f4"),
               std::make_pair(34, Board("e6f4").Opponent() & ~Board("e6").Player())),
          Pair(Board("e6f6"),
               std::make_pair(18, Board("e6f6").Opponent() & ~Board("e6").Player())),
          Pair(Board("f5f4"),
               std::make_pair(34, Board("f5f4").Opponent() & ~Board("f5")
               .Player()))));
}

TEST(GetUniqueNextBoardsWithPass, ManyNextPositions) {
  Board b;
  auto unique_next_boards = GetUniqueNextBoardsWithPass(b);
  EXPECT_EQ(unique_next_boards.size(), 1);
  EXPECT_THAT(unique_next_boards, UnorderedElementsAre(
      Pair(Board("e6").Unique(),
           std::make_pair(37, Board("e6").Unique().Opponent() & ~b.Player()))));
}

TEST(GetUniqueNextBoardsWithPass, Pass) {
  Board b(
      "--------"
      "--------"
      "--------"
      "--------"
      "--------"
      "--------"
      "--------"
      "--XXXOOX", true);
  Board next(
      "--------"
      "--------"
      "--------"
      "--------"
      "--------"
      "--------"
      "--------"
      "--OOOXXO", true);
  auto unique_next_boards = GetUniqueNextBoardsWithPass(b);
  EXPECT_THAT(unique_next_boards, UnorderedElementsAre(
      Pair(next, std::make_pair(-1, 0))));
}

TEST(GetUniqueNextBoardsWithPass, DoublePass) {
  Board b(
      "--------"
      "--------"
      "--------"
      "--------"
      "--------"
      "--------"
      "--------"
      "XXXXXOOX", true);
  auto unique_next_boards = GetUniqueNextBoardsWithPass(b);
  EXPECT_TRUE(unique_next_boards.empty());
}

TEST(GetUniqueNextBoardsWithPass, ManyWaysToGetPosition) {
  Board b(
      "--------"
      "--------"
      "--------"
      "--------"
      "--------"
      "--------"
      "--------"
      "XXXO-XXX", true);
  Board next(
      "--------"
      "--------"
      "--------"
      "--------"
      "--------"
      "--------"
      "--------"
      "OOOOOOOO", true);
  auto unique_next_boards = GetUniqueNextBoardsWithPass(b);
  EXPECT_EQ(unique_next_boards.size(), 1);
  EXPECT_THAT(unique_next_boards,
              UnorderedElementsAre(
                  Pair(next, std::make_pair(3, next.Opponent() & ~b.Player())
                  )));
}