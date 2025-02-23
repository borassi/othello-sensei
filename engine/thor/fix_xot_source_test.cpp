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


#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>

#include "fix_xot_source.h"
#include "thor_test_utils.h"

using ::testing::IsSubsetOf;
using ::testing::Pair;
using ::testing::UnorderedElementsAre;

const std::string kFolder = kThorTestData + "/fix_xot_source_test";
const std::string kInputFolder = kThorTestData + "/input";
const std::string kOutputFolder = kThorTestData + "/output";

class FixXotSourceTest : public testing::Test {
  void SetUp() override {
    fs::remove_all(kThorTestData);
    fs::create_directories(kInputFolder);
    std::ofstream thor_games(kInputFolder + "/WTH_2023.wtb", std::ios::binary);
    std::ofstream thor_players(kInputFolder + "/WTH.JOU", std::ios::binary);
    std::ofstream thor_tournaments(kInputFolder + "/WTH.TRN", std::ios::binary);

    WriteHeader(thor_games, 2023);
    WriteHeader(thor_players, 2023);
    WriteHeader(thor_tournaments, 2023);

    std::vector<std::string> players = {"player0", "player1", "player2"};
    std::vector<std::string> tournaments = {"tournament0", "tournament1"};

    for (const std::string& s : players) {
      std::string padded = PadString(s, 20);
      thor_players.write(padded.c_str(), 20);
    }
    for (const std::string& s : tournaments) {
      std::string padded = PadString(s, 26);
      thor_tournaments.write(padded.c_str(), 26);
    }

    std::vector<std::string> games = {
        // Standard game player0 vs player1, tournament0.
        "e6f4c3c4d3d6e3c2b3d2c5f5f3f6e1d1e2f1g4g3g5b5b4c6c1b1f2a5a4g1h3g6a6a3a2b6g2h1h2h4e7c7a7b7a8d8d7b2f8f7g8c8h5h7h6e8b8g7h8a1",
        // Short game player1 vs player2, tournament1.
        "e6f4c3c4d3d6e3c2b3d2c5f5f3f6e1d1e2f1g4g3g5b5b4c6c1b1f2a5a4g1h3g6a6a3a2b6g2h1h2h4e7c7a7b7a8d8d7",
        // XOT with start f5d6c4d3c2b3b4b5 player2 vs player0, tournament0.
        "c5e2c3d2a3c6e3a4c1e1d1b1a5f3e6f4f6b6d7a6a7e7c7g4g6d8f8g5h5h4h3g3h2g7h8c8f7h6h7g2g1g8e8b2b7h1f1f2a1a2b8a8",
        // XOT with two starts f5f4g3g6f3g4e3e2 player0 vs player1, tournament1.
        "c5",
        // Empty game player1 vs player2, tournament0.
        "",
        // Invalid.
        "a8",
    };
    std::vector<Sequence> sequences;

    for (int i = 0; i < games.size(); ++i) {
      sequences.push_back(Sequence(games[i]));
      std::vector<char> game = StoredGame(i % tournaments.size(), i % players.size(), (i+1) % players.size(), 12, -4, games[i]);
      thor_games.write(game.data(), game.size());
    }
  }

  void TearDown() override {
    fs::remove_all(kFolder);
  }

 protected:
  std::vector<Game> games_;
};


TEST_F(FixXotSourceTest, Basic) {
  std::string sequences = "f5d6c4d3c2b3b4b5\nf5f4g3g6f3g4e3e2";
  auto results = FixXOTSource(sequences, kInputFolder, kOutputFolder);
  EXPECT_EQ(results.ok, 3);
  EXPECT_EQ(results.fixed, 1);
  EXPECT_EQ(results.not_found, 1);
  EXPECT_EQ(results.multiple_found, 1);
  Source source(kOutputFolder);
  EXPECT_THAT(source.Tournaments(), UnorderedElementsAre("tournament0", "tournament1"));
  EXPECT_THAT(source.Players(), UnorderedElementsAre("player0", "player1", "player2"));
  EXPECT_THAT(source.AllGames(), UnorderedElementsAre(
      Game(
          Sequence("e6f4c3c4d3d6e3c2b3d2c5f5f3f6e1d1e2f1g4g3g5b5b4c6c1b1f2a5a4g1h3g6a6a3a2b6g2h1h2h4e7c7a7b7a8d8d7b2f8f7g8c8h5h7h6e8b8g7h8a1"),
          &source.Players()[0],
          &source.Players()[1],
          &source.Tournaments()[0],
          2023,
          12,
          0.1
      ),
      Game(
          Sequence("e6f4c3c4d3d6e3c2b3d2c5f5f3f6e1d1e2f1g4g3g5b5b4c6c1b1f2a5a4g1h3g6a6a3a2b6g2h1h2h4e7c7a7b7a8d8d7"),
          &source.Players()[1],
          &source.Players()[2],
          &source.Tournaments()[1],
          2023,
          12,
          0.1
      ),
      Game(
          Sequence("e6f4d3c4b3c2d2e2e3b5c3b4c1f3c5d1a3a5a4a2e1c6f5d6f6f2g4f1g1g5g3d7f7h4h6e7e8d8c8c7b8g7h8h3g6f8g8b7a7h7h5b2g2a8a6b6a1b1h2h1"),
          &source.Players()[2],
          &source.Players()[0],
          &source.Tournaments()[0],
          2023,
          12,
          0.1
      ),
      Game(
          Sequence(""),
          &source.Players()[1],
          &source.Players()[2],
          &source.Tournaments()[0],
          2023,
          12,
          0.1
      )
  ));
}