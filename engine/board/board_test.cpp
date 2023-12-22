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
#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>
#include <regex>
#include "board.h"
#include "get_moves.h"

TEST(Board, Basic) {
  std::string board_string = "--------"
                             "--------"
                             "--------"
                             "--------"
                             "--------"
                             "--------"
                             "--------"
                             "------OX";
  Board b(board_string.c_str(), true);
  EXPECT_EQ(b, Board(1, 2));
  EXPECT_EQ(b.Player(), 1);
  EXPECT_EQ(b.Opponent(), 2);

  EXPECT_FALSE(b.IsEmpty(0));
  EXPECT_FALSE(b.IsEmpty(1));
  EXPECT_TRUE(b.IsEmpty(2));

  EXPECT_TRUE(b.IsPlayer(0));
  EXPECT_FALSE(b.IsPlayer(1));
  EXPECT_FALSE(b.IsPlayer(2));

  EXPECT_FALSE(b.IsOpponent(0));
  EXPECT_TRUE(b.IsOpponent(1));
  EXPECT_FALSE(b.IsOpponent(2));

  std::ostringstream ss;
  ss << b;
  EXPECT_EQ(std::regex_replace(ss.str(), std::regex("\\n"), ""), board_string);
}

TEST(Board, Sequence) {
  Board b("e6f4c3c4d3");
  Board b_copy("--------"
               "--------"
               "--XX----"
               "--OXOO--"
               "---XX---"
               "----X---"
               "--------"
               "--------", false);
  EXPECT_EQ(b, b_copy);
}

TEST(Board, BoardsFromSequenceEmpty) {
  std::vector<Board> previous;
  Board b("", &previous);
  EXPECT_THAT(previous, ::testing::ElementsAre());
}

TEST(Board, BoardsFromSequenceBase) {
  std::vector<Board> previous;
  Board b("e6f4c3c4d3", &previous);
  EXPECT_THAT(
      previous,
      ::testing::ElementsAre(
          Board(), Board("e6"), Board("e6f4"), Board("e6f4c3"),
          Board("e6f4c3c4"))
  );
}

TEST(Board, BoardsFromSequencePass) {
  std::vector<Board> previous;
  Board b("e6f6g6f4f5g7f7h7h8d6h6g8f8", &previous);
  EXPECT_THAT(
      previous,
      ::testing::ElementsAre(
          Board(""),
          Board("e6"),
          Board("e6f6"),
          Board("e6f6g6"),
          Board("e6f6g6f4"),
          Board("e6f6g6f4f5"),
          Board("e6f6g6f4f5g7"),
          Board("e6f6g6f4f5g7f7"),
          Board("e6f6g6f4f5g7f7h7"),
          Board("e6f6g6f4f5g7f7h7h8"),
          Board("e6f6g6f4f5g7f7h7h8d6"),
          Board("e6f6g6f4f5g7f7h7h8d6h6"),
          Board("e6f6g6f4f5g7f7h7h8d6h6g8"),
          Board(b.Opponent(), b.Player())
      )
  );
}

TEST(Board, BoardsFromSequencePass1) {
  std::vector<Board> previous;
  Board b("e6f6g6f4f5g7f7h7h8d6h6g8f8g5", &previous);
  Board result_after_pass = Board("e6f6g6f4f5g7f7h7h8d6h6g8f8");
  EXPECT_THAT(
      previous,
      ::testing::ElementsAre(
          Board(""),
          Board("e6"),
          Board("e6f6"),
          Board("e6f6g6"),
          Board("e6f6g6f4"),
          Board("e6f6g6f4f5"),
          Board("e6f6g6f4f5g7"),
          Board("e6f6g6f4f5g7f7"),
          Board("e6f6g6f4f5g7f7h7"),
          Board("e6f6g6f4f5g7f7h7h8"),
          Board("e6f6g6f4f5g7f7h7h8d6"),
          Board("e6f6g6f4f5g7f7h7h8d6h6"),
          Board("e6f6g6f4f5g7f7h7h8d6h6g8"),
          Board(result_after_pass.Opponent(), result_after_pass.Player()),
          Board("e6f6g6f4f5g7f7h7h8d6h6g8f8")
      )
  );
}

TEST(Board, Unique) {
  for (int i = 0; i < 1000; ++i) {
    Board b = RandomBoard();
    for (Board t : b.AllTranspositions()) {
      EXPECT_EQ(b.Unique(), t.Unique());
    }
  }
}

TEST(Board, UndoMove) {
  for (int i = 0; i < 1000; ++i) {
    const Board b = RandomBoard();
    const auto flips = GetAllMoves(b.Player(), b.Opponent());
    if (flips.size() == 0) {
      continue;
    }
    BitPattern flip = flips[rand() % flips.size()];
    Board b_copy = b.Next(flip);
    b_copy.UndoMove(flip, b.MoveFromFlip(flip));
    ASSERT_EQ(b, b_copy);
  }
}

TEST(SerializedBoard, Serialize) {
  for (int i = 0; i < 1000; ++i) {
    Board b = RandomBoard();
    const auto serialized = b.Serialize();
    EXPECT_EQ(b, Board::Deserialize(serialized.begin()));
  }
}