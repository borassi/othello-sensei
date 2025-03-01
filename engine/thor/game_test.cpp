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
#include "game.h"
#include "thor_test_utils.h"

TEST(Game, ThorSquareToSquareTest) {
  EXPECT_EQ(kThorSquareToSquare.square[0], kNoSquare);
  EXPECT_EQ(kThorSquareToSquare.square[88], 0);
  EXPECT_EQ(kThorSquareToSquare.square[87], 1);
  EXPECT_EQ(kThorSquareToSquare.square[85], 3);
  EXPECT_EQ(kThorSquareToSquare.square[81], 7);
  EXPECT_EQ(kThorSquareToSquare.square[78], 8);
  EXPECT_EQ(kThorSquareToSquare.square[76], 10);
  EXPECT_EQ(kThorSquareToSquare.square[18], 56);
  EXPECT_EQ(kThorSquareToSquare.square[11], 63);
}


TEST(Game, Load) {
  std::vector<std::string> players = {"player0", "player1", "player2"};
  std::vector<std::string> tournaments = {"tournament0", "tournament1"};
  std::string moves = "e6f4c3c4d3d6e3c2b3d2c5f5f3f6e1d1e2f1g4g3g5b5b4c6c1b1f2a5a4g1h3g6a6a3a2b6g2h1h2h4e7c7a7b7a8d8d7b2f8f7g8c8h5h7h6e8b8g7h8a1";
  std::vector<char> stored_game = StoredGame(1, 2, 1, 2, 2, moves);

  Game game(stored_game.data(), 0, 2023, players, tournaments, 0.5);

  EXPECT_EQ(game.Year(), 2023);
  EXPECT_EQ(game.Black(), "player2");
  EXPECT_EQ(game.White(), "player1");
  EXPECT_EQ(game.Tournament(), "tournament1");
  EXPECT_EQ(game.Score(), 2);
  EXPECT_EQ(game.Moves(), Sequence(moves));
  EXPECT_FLOAT_EQ(game.Priority(), 0.5);
}

