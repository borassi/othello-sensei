/*
 * Copyright 2021 Google LLC
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
#include <regex>
#include "board.h"

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

TEST(Board, Unique) {
  for (int i = 0; i < 1000; ++i) {
    Board b = RandomBoard();
    for (Board t : b.AllTranspositions()) {
      EXPECT_EQ(b.Unique(), t.Unique());
    }
  }
}

TEST(SerializedBoard, Serialize) {
  for (int i = 0; i < 1000; ++i) {
    Board b = RandomBoard();
    const auto serialized = b.Serialize();
    EXPECT_EQ(b, Board::Deserialize(serialized.begin()));
  }
}