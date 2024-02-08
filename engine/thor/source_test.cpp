/*
 * Copyright 2024 Michele Borassi
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

#include "../board/sequence.h"
#include "../utils/files.h"
#include "source.h"
#include "thor_test_utils.h"

using ::testing::IsSubsetOf;
using ::testing::Pair;
using ::testing::UnorderedElementsAre;

const std::string kFolder = kThorTestData + "/my_source";

class ThorSourceTest : public testing::Test {
  void SetUp() override {
    fs::create_directories(kFolder);
    std::ofstream thor_games_2022(kFolder + "/WTH_2022.wtb", std::ios::binary);
    std::ofstream thor_games_2023(kFolder + "/WTH_2023.wtb", std::ios::binary);
    std::ofstream thor_players(kFolder + "/WTH.JOU", std::ios::binary);
    std::ofstream thor_tournaments(kFolder + "/WTH.TRN", std::ios::binary);

    WriteHeader(thor_games_2022, 2022);
    WriteHeader(thor_games_2023, 2023);
    WriteHeader(thor_players, 2023);
    WriteHeader(thor_tournaments, 2023);

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

    std::vector<std::string> games_2022 = {
      "e6f6"
    };
    std::vector<std::string> games_2023 = {
      "e6f4c3c4d3d6e3c2b3d2c5f5f3f6e1d1e2f1g4g3g5b5b4c6c1b1f2a5a4g1h3g6a6a3a2b6g2h1h2h4e7c7a7b7a8d8d7b2f8f7g8c8h5h7h6e8b8g7h8a1",
      "e6f4d3c4c3d6e3c2b3d2c5f5f3f6e1d1e2f1g4g3g5b5b4c6c1b1f2a5a4g1h3g6a6a3a2b6g2h1h2h4e7c7a7b7a8d8d7b2f8f7g8c8h5h7h6e8b8g7h8a1",
      "d3c3c4c5d6f4f3e3",
      "d3c3c4e3d2",
      "e6f4c3c4c5c6c7",
      "e6d6c5f6f5e3",
      ""
    };
    std::vector<Sequence> sequences;

    for (int year = 2023; year >= 2022; year--) {
      auto& thor_games = year == 2022 ? thor_games_2022 : thor_games_2023;
      auto games = year == 2022 ? games_2022 : games_2023;
      for (int i = 0; i < games.size(); ++i) {
        sequences.push_back(Sequence(games[i]));
        std::vector<char> game = StoredGame(i % tournaments.size(), i % players.size(), (i+1) % players.size(), 12, -4, games[i]);
        thor_games.write(game.data(), game.size());
      }
    }
  }

  void TearDown() override {
    fs::remove_all(kFolder);
  }

 protected:
  std::vector<Game> games_;
};

TEST_F(ThorSourceTest, Basic) {
  Source source(kFolder);
  EXPECT_THAT(source.Tournaments(), UnorderedElementsAre("tournament0", "tournament1"));
  EXPECT_THAT(source.Players(), UnorderedElementsAre("player0", "player1", "player_with_20_chars"));
  auto games = source.GetGames(Sequence("e6f4c3c4c5"));
  EXPECT_EQ(games.num_games, 1);
  EXPECT_THAT(games.examples, UnorderedElementsAre(&source.AllGames()[5]));
  EXPECT_THAT(games.next_moves, UnorderedElementsAre(Pair(21, 1)));
}

TEST_F(ThorSourceTest, LongerThanGame) {
  Source source(kFolder);
  auto games = source.GetGames(Sequence("e6f4c3c4c5c6c7d3"));
  EXPECT_EQ(games.num_games, 0);
}

TEST_F(ThorSourceTest, BeforeTranspositions) {
  Source source(kFolder);
  auto games = source.GetGames(Sequence("e6"));
  EXPECT_EQ(games.num_games, 7);
  EXPECT_THAT(games.examples, UnorderedElementsAre(
      &source.AllGames()[0], &source.AllGames()[1], &source.AllGames()[2],
      &source.AllGames()[3], &source.AllGames()[4], &source.AllGames()[5],
      &source.AllGames()[6]));
  EXPECT_THAT(games.next_moves, UnorderedElementsAre(
      Pair(34, 3), Pair(18, 3), Pair(20, 1)
  ));
}

TEST_F(ThorSourceTest, ByBlack) {
  Source source(kFolder);
  auto games = source.GetGames(Sequence("e6"), 100, {"player0"});
  EXPECT_EQ(games.num_games, 3);
  EXPECT_THAT(games.examples, UnorderedElementsAre(&source.AllGames()[0], &source.AllGames()[1], &source.AllGames()[4]));
  EXPECT_THAT(games.next_moves, UnorderedElementsAre(Pair(34, 1), Pair(18, 2)));
}

TEST_F(ThorSourceTest, ByWhite) {
  Source source(kFolder);
  auto games = source.GetGames(Sequence("e6"), 100, {}, {"player_with_20_chars"});
  EXPECT_EQ(games.num_games, 2);
  EXPECT_THAT(games.examples, UnorderedElementsAre(&source.AllGames()[2], &source.AllGames()[5]));
  EXPECT_THAT(games.next_moves, UnorderedElementsAre(Pair(34, 2)));
}

TEST_F(ThorSourceTest, ByTournament) {
  Source source(kFolder);
  auto games = source.GetGames(Sequence("e6"), 100, {}, {}, {"tournament0"});
  EXPECT_EQ(games.num_games, 4);
  EXPECT_THAT(games.examples, UnorderedElementsAre(&source.AllGames()[0], &source.AllGames()[1], &source.AllGames()[3], &source.AllGames()[5]));
  EXPECT_THAT(games.next_moves, UnorderedElementsAre(Pair(34, 2), Pair(18, 2)));
}

TEST_F(ThorSourceTest, ByYear) {
  Source source(kFolder);
  auto games = source.GetGames(Sequence("e6"), 100, {}, {}, {}, 2023, 2023);
  EXPECT_EQ(games.num_games, 6);
  EXPECT_THAT(games.examples, UnorderedElementsAre(
      &source.AllGames()[1], &source.AllGames()[2], &source.AllGames()[3],
      &source.AllGames()[4], &source.AllGames()[5], &source.AllGames()[6]));
  EXPECT_THAT(games.next_moves, UnorderedElementsAre(
      Pair(34, 3), Pair(18, 2), Pair(20, 1)
  ));
}

TEST_F(ThorSourceTest, Limited) {
  Source source(kFolder);
  auto games = source.GetGames(Sequence("e6"), 3);
  EXPECT_EQ(games.examples.size(), 3);
  EXPECT_EQ(games.num_games, 7);
  EXPECT_THAT(games.examples, IsSubsetOf({
      &source.AllGames()[1], &source.AllGames()[2], &source.AllGames()[3],
      &source.AllGames()[4], &source.AllGames()[5], &source.AllGames()[6]}));
  EXPECT_THAT(games.next_moves, UnorderedElementsAre(
      Pair(34, 3), Pair(18, 3), Pair(20, 1)
  ));
}

TEST_F(ThorSourceTest, ByBlackLimited) {
  Source source(kFolder);
  auto games = source.GetGames(Sequence("e6"), 2, {"player0"});
  EXPECT_EQ(games.num_games, 3);
  EXPECT_THAT(games.examples, UnorderedElementsAre(&source.AllGames()[1], &source.AllGames()[4]))
      << source.AllGames()[1] << "\n" << source.AllGames()[4] << "\n";
  EXPECT_THAT(games.next_moves, UnorderedElementsAre(Pair(34, 1), Pair(18, 2)));
}

TEST_F(ThorSourceTest, ByBlackLimitedSubset) {
  Source source(kFolder);
  auto games = source.GetGames(Sequence("e6"), 1, {"player0"});
  EXPECT_EQ(games.num_games, 3);
  EXPECT_EQ(games.examples.size(), 1);
  EXPECT_THAT(games.examples, IsSubsetOf({&source.AllGames()[1], &source.AllGames()[4]}));
  EXPECT_THAT(games.next_moves, UnorderedElementsAre(Pair(34, 1), Pair(18, 2)));
}

TEST_F(ThorSourceTest, NextMoves) {
  Source source(kFolder);
  auto games = source.GetGames(Sequence("e6f6"), 100, {}, {}, {}, 2022, 2022);
  EXPECT_THAT(games.next_moves, UnorderedElementsAre(Pair(kNoSquare, 1)));
}

TEST_F(ThorSourceTest, Rotate) {
  Source source(kFolder);
  auto games = source.GetGames(Sequence("e6f4c3c4c5c6"));
  games.Rotate(Sequence("c4e3f6e6d6c6"));
  EXPECT_EQ(games.num_games, 1);
  EXPECT_THAT(games.examples, UnorderedElementsAre(&source.AllGames()[5]));
  EXPECT_THAT(games.next_moves, UnorderedElementsAre(Pair(22, 1)));
}
