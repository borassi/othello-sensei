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
#include <string>

#include "../utils/files.h"
#include "thor.h"
#include "thor_test_utils.h"

const std::string kTempFolder = "app/testdata/tmp/thor";

using ::testing::UnorderedElementsAre;

void WriteHeader(std::ofstream& file) {
  std::vector<char> to_write(10, 0);
  short year = 2023;
  file.write(to_write.data(), 10);
  file.write((char*) &year, 2);
  file.write(to_write.data(), 4);
}

class ThorTest : public testing::Test {
  void SetUp() override {
    fs::create_directories(kTempFolder);
    std::ofstream thor_games(kTempFolder + "/WTH_2023.wtb", std::ios::binary);
    std::ofstream thor_players(kTempFolder + "/WTH.JOU", std::ios::binary);
    std::ofstream thor_tournaments(kTempFolder + "/WTH.TRN", std::ios::binary);

    WriteHeader(thor_games);
    WriteHeader(thor_players);
    WriteHeader(thor_tournaments);

    std::vector<std::string> players = {"player0", "player1", "player_with_20_chars"};
    std::vector<std::string> tournaments = {"tournament0", "tournament1"};

    for (const std::string& s : players) {
      std::string padded = PadString(s, 20);
      thor_players.write(padded.c_str(), 20);
    }
    for (const std::string& s : tournaments) {
      std::string padded = PadString(s, 26);
      thor_tournaments.write(padded.c_str(), 26);
    }

    std::string moves = "e6f4c3c4d3d6e3c2b3d2c5f5f3f6e1d1e2f1g4g3g5b5b4c6c1b1f2a5a4g1h3g6a6a3a2b6g2h1h2h4e7c7a7b7a8d8d7b2f8f7g8c8h5h7h6e8b8g7h8a1";
    std::vector<char> stored_game = StoredGame(1, 2, 1, 2, 2, moves);

    thor_games.write(stored_game.data(), stored_game.size());
  }

  void TearDown() override {
    fs::remove_all(kTempFolder);
  }
};

TEST_F(ThorTest, Basic) {
  Thor thor(kTempFolder);
  thor.LookupPositions({"player_with_20_chars"}, {"player1"}, {});
  EXPECT_THAT(thor.Tournaments(), UnorderedElementsAre("tournament1"));
  EXPECT_THAT(thor.Players(), UnorderedElementsAre("player1", "player_with_20_chars"));
  EXPECT_EQ(thor.GetNumGames(Board("e6f4c3c4d3")), 1);
  std::vector<Game> games = thor.GetGames(Board("e6f4c3c4d3"));
  EXPECT_EQ(games.size(), 1);
  Game game = games[0];
  EXPECT_EQ(game.Year(), 2023);
  EXPECT_EQ(game.Moves(), "e6f4c3c4d3d6e3c2b3d2c5f5f3f6e1d1e2f1g4g3g5b5b4c6c1b1f2a5a4g1h3g6a6a3a2b6g2h1h2h4e7c7a7b7a8d8d7b2f8f7g8c8h5h7h6e8b8g7h8a1");
}

TEST(Thor, Thor) {
  Thor thor("app/src/main/assets/thor");
}