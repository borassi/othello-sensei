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

using ::testing::Pair;
using ::testing::UnorderedElementsAre;

class ThorTest : public testing::Test {
  void SetUp() override {
    // In case one test failed and did not clean up.
    fs::remove_all(kThorTestData);
    fs::create_directories(kThorTestData + "/Thor");
    fs::create_directories(kThorTestData + "/PlayOK");
    std::ofstream thor_games_2022_file(kThorTestData + "/Thor/WTH_2022.wtb", std::ios::binary);
    std::ofstream thor_games_2023_file(kThorTestData + "/Thor/WTH_2023.wtb", std::ios::binary);
    std::ofstream thor_players_file(kThorTestData + "/Thor/WTH.JOU", std::ios::binary);
    std::ofstream thor_tournaments_file(kThorTestData + "/Thor/WTH.TRN", std::ios::binary);

    std::ofstream playok_games_file(kThorTestData + "/PlayOK/PLAYOK_2023.wtb", std::ios::binary);
    std::ofstream playok_players_file(kThorTestData + "/PlayOK/WTH.JOU", std::ios::binary);
    std::ofstream playok_tournaments_file(kThorTestData + "/PlayOK/WTH.TRN", std::ios::binary);

    WriteHeader(thor_games_2022_file, 2022);
    for (auto file : {&thor_games_2023_file, &thor_players_file,
                      &thor_tournaments_file, &playok_games_file,
                      &playok_players_file, &playok_tournaments_file}) {
      WriteHeader(*file, 2023);
    }

    std::vector<std::string> thor_players = {"player0", "player1", "player_with_20_chars"};
    std::vector<std::string> thor_tournaments = {"tournament0", "tournament1"};

    std::vector<std::string> playok_players = {"vader", "luke"};
    std::vector<std::string> playok_tournaments = {"bespin_open", "endor_championships"};

    for (auto [file, values, size] : {
        std::tuple<std::ofstream*, std::vector<std::string>, int>{&thor_players_file, thor_players, 20},
        std::tuple<std::ofstream*, std::vector<std::string>, int>{&thor_tournaments_file, thor_tournaments, 26},
        std::tuple<std::ofstream*, std::vector<std::string>, int>{&playok_players_file, playok_players, 20},
        std::tuple<std::ofstream*, std::vector<std::string>, int>{&playok_tournaments_file, playok_tournaments, 26}}) {
      for (const std::string& s : values) {
        std::string padded = PadString(s, size);
        file->write(padded.c_str(), size);
      }
    }

    std::vector<std::string> games_2022 = {
      "e6f6"
    };
    std::vector<std::string> games_2023 = {
      "e6f4c3c4d3d6e3c2b3d2c5f5f3f6e1d1e2f1g4g3g5b5b4c6c1b1f2a5a4g1h3g6a6a3a2b6g2h1h2h4e7c7a7b7a8d8d7b2f8f7g8c8h5h7h6e8b8g7h8a1",
      "e6f4d3c4c3d6e3c2b3d2c5f5f3f6e1d1e2f1g4g3g5b5b4c6c1b1f2a5a4g1h3g6a6a3a2b6g2h1h2h4e7c7a7b7a8d8d7b2f8f7g8c8h5h7h6e8b8g7h8a1",
      "d3c3c4c5d6f4f3e3",
      "d3c3c4e3d2",
      "e6f4c3c4c5c6",
      "e6d6c5f6f5e3"
    };

    for (int year = 2023; year >= 2022; year--) {
      auto& games = year == 2022 ? games_2022 : games_2023;
      auto& file = year == 2022 ? thor_games_2022_file : thor_games_2023_file;
      for (int i = 0; i < games.size(); ++i) {
        std::vector<char> game = StoredGame(i % thor_tournaments.size(), i % thor_players.size(), (i+1) % thor_players.size(), 12, -4, games[i]);
        file.write(game.data(), game.size());
      }
    }
    for (int i = 0; i < games_2023.size(); ++i) {
      std::vector<char> game = StoredGame(i % playok_tournaments.size(), i % playok_players.size(), (i+1) % playok_players.size(), 12, -4, games_2023[i]);
      playok_games_file.write(game.data(), game.size());
    }
  }

  void TearDown() override {
    fs::remove_all(kThorTestData);
  }

 protected:
  std::vector<Game> games_;
};

TEST_F(ThorTest, Metadata) {
  Thor thor(kThorTestData);
  EXPECT_THAT(thor.Sources(), UnorderedElementsAre("Thor", "PlayOK"));
  EXPECT_THAT(thor.Tournaments(), UnorderedElementsAre(
      Pair("Thor", std::vector<std::string>{"tournament0", "tournament1"}),
      Pair("PlayOK", std::vector<std::string>{"bespin_open", "endor_championships"})));
  EXPECT_THAT(thor.Players(), UnorderedElementsAre(
      Pair("Thor", UnorderedElementsAre("player0", "player1", "player_with_20_chars")),
      Pair("PlayOK", UnorderedElementsAre("luke", "vader"))));
}

TEST_F(ThorTest, SingleSource) {
  fs::remove_all(kThorTestData + "/PlayOK");
  Thor thor(kThorTestData);
  auto games = thor.GetGames("Thor", Sequence("e6"));
  EXPECT_EQ(games.num_games, 7);
  EXPECT_THAT(games.next_moves, UnorderedElementsAre(
      Pair(34, 3), Pair(18, 3), Pair(20, 1)
  ));
}

TEST_F(ThorTest, Rotations) {
  fs::remove_all(kThorTestData + "/PlayOK");
  Thor thor(kThorTestData);
  auto games = thor.GetGames("Thor", Sequence("d3c3c4e3"));
  EXPECT_EQ(games.num_games, 2);
  EXPECT_THAT(games.next_moves, UnorderedElementsAre(Pair(52, 1), Pair(34, 1)));
}

TEST_F(ThorTest, ManyFiles) {
  Thor thor(kThorTestData);
  auto games = thor.GetGames("Thor", Sequence("d3c3c4e3"));
  EXPECT_EQ(games.num_games, 2);
  EXPECT_THAT(games.next_moves, UnorderedElementsAre(Pair(52, 1), Pair(34, 1)));
  games = thor.GetGames("PlayOK", Sequence("d3c3c4e3"));
  EXPECT_EQ(games.num_games, 2);
  EXPECT_THAT(games.next_moves, UnorderedElementsAre(Pair(52, 1), Pair(34, 1)));
}

TEST_F(ThorTest, Save) {
  Thor thor1(kThorTestData);
  thor1.SaveAll();
  // TODO: Verify that we actually don't recompute anything.
  Thor thor(kThorTestData);
  auto games = thor.GetGames("Thor", Sequence("d3c3c4e3"));
  EXPECT_EQ(games.num_games, 2);
  EXPECT_THAT(games.next_moves, UnorderedElementsAre(Pair(52, 1), Pair(34, 1)));
  games = thor.GetGames("PlayOK", Sequence("d3c3c4e3"));
  EXPECT_EQ(games.num_games, 2);
  EXPECT_THAT(games.next_moves, UnorderedElementsAre(Pair(52, 1), Pair(34, 1)));
}