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

#include "game_to_save.h"
#include "../utils/files.h"

TEST(GameToSave, Base) {
  GameToSave game(
      Sequence("e6f4c3"),
      "Black player",
      "White player",
      "Tournament",
      "Some notes",
      2010,
      64,
      "3");
  std::string result = game.ToString();
  EXPECT_EQ(GameToSave::FromString(result), game);
}

TEST(GameToSave, MovesInPlayerName) {
  std::string game = R"txt(
      1. e6 2. f4 3. c3
      Black: c4d3
      White: A player
      Year: 2011
      Black disks: 32
  )txt";
  EXPECT_EQ(GameToSave::FromString(game), GameToSave(
      Sequence("e6f4c3"),
      "c4d3",
      "A player",
      "",
      "",
      2011,
      32,
      ""));
}

TEST(GameToSave, AutoBlackDisksNoGameOver) {
  std::string game = R"txt(
      e6f4c3
      Black: B
      White: W
      Year: 2011
  )txt";
  EXPECT_EQ(GameToSave::FromString(game), GameToSave(
      Sequence("e6f4c3"),
      "B",
      "W",
      "",
      "",
      2011,
      64,
      ""));
}

TEST(GameToSave, AutoBlackDisksGameOver) {
  std::string game = R"txt(
      e6f4c3c4d3d6e3c2b3d2c5f5f3f6e1d1e2f1g4g3g5h5f2h4c7g6e7b4h3a4b1g1b5h2f7d8e8f8d7c8c6b8a3a2g7h6h8h7b2a1h1c1g8b6a5g2a7b7a8a6
      Black: B
      White: W
      Year: 2011
  )txt";
  EXPECT_EQ(GameToSave::FromString(game), GameToSave(
      Sequence("e6f4c3c4d3d6e3c2b3d2c5f5f3f6e1d1e2f1g4g3g5h5f2h4c7g6e7b4h3a4b1g1b5h2f7d8e8f8d7c8c6b8a3a2g7h6h8h7b2a1h1c1g8b6a5g2a7b7a8a6"),
      "B",
      "W",
      "",
      "",
      2011,
      27,
      ""));
}

TEST(GameToSave, FromToGame) {
  GameToSave game(
      Sequence("e6f4c3"),
      "Black player",
      "White player",
      "Tournament",
      "",
      2010,
      64,
      "");
  EXPECT_EQ(GameToSave::FromGame(game.ToGame(/*canonicalize=*/false)), game);
  EXPECT_EQ(GameToSave::FromGame(game.ToGame(/*canonicalize=*/true)), game);
}

TEST(GameToSave, FromToGameDoesNotCanonicalize) {
GameToSave game(
    Sequence("d3c3c4"),
    "Black player",
    "White player",
    "Tournament",
    "",
    2010,
    64,
    "");
EXPECT_EQ(GameToSave::FromGame(game.ToGame(/*canonicalize=*/false)), game);
}