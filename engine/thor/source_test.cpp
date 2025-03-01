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
    fs::remove_all(kFolder);
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
  auto games = source.GetGames(Sequence("e6f4c3c4c5"), 10);
  EXPECT_EQ(games.num_games, 1);
  EXPECT_THAT(games.examples, UnorderedElementsAre(source.AllGames()[5]));
  EXPECT_THAT(games.next_moves, UnorderedElementsAre(Pair(21, 1)));
}

TEST_F(ThorSourceTest, LongerThanGame) {
  Source source(kFolder);
  auto games = source.GetGames(Sequence("e6f4c3c4c5c6c7d3"), 10);
  EXPECT_EQ(games.num_games, 0);
}

TEST_F(ThorSourceTest, BeforeTranspositions) {
  Source source(kFolder);
  auto games = source.GetGames(Sequence("e6"), 10);
  EXPECT_EQ(games.num_games, 7);
  EXPECT_THAT(games.examples, UnorderedElementsAre(
      source.AllGames()[0], source.AllGames()[1], source.AllGames()[2],
      source.AllGames()[3], source.AllGames()[4], source.AllGames()[5],
      source.AllGames()[6]));
  EXPECT_THAT(games.next_moves, UnorderedElementsAre(
      Pair(34, 3), Pair(18, 3), Pair(20, 1)
  ));
}

TEST_F(ThorSourceTest, ByBlack) {
  Source source(kFolder);
  auto games = source.GetGames(Sequence("e6"), 100, {"player0"});
  EXPECT_EQ(games.num_games, 3);
  EXPECT_THAT(games.examples, UnorderedElementsAre(source.AllGames()[0], source.AllGames()[1], source.AllGames()[4]));
  EXPECT_THAT(games.next_moves, UnorderedElementsAre(Pair(34, 1), Pair(18, 2)));
}

TEST_F(ThorSourceTest, ByWhite) {
  Source source(kFolder);
  auto games = source.GetGames(Sequence("e6"), 100, {}, {"player_with_20_chars"});
  EXPECT_EQ(games.num_games, 2);
  EXPECT_THAT(games.examples, UnorderedElementsAre(source.AllGames()[2], source.AllGames()[5]));
  EXPECT_THAT(games.next_moves, UnorderedElementsAre(Pair(34, 2)));
}

TEST_F(ThorSourceTest, ByTournament) {
  Source source(kFolder);
  auto games = source.GetGames(Sequence("e6"), 100, {}, {}, {"tournament0"});
  EXPECT_EQ(games.num_games, 4);
  EXPECT_THAT(games.examples, UnorderedElementsAre(source.AllGames()[0], source.AllGames()[1], source.AllGames()[3], source.AllGames()[5]));
  EXPECT_THAT(games.next_moves, UnorderedElementsAre(Pair(34, 2), Pair(18, 2)));
}

TEST_F(ThorSourceTest, ByYear) {
  Source source(kFolder);
  auto games = source.GetGames(Sequence("e6"), 100, {}, {}, {}, 2023, 2023);
  EXPECT_EQ(games.num_games, 6);
  EXPECT_THAT(games.examples, UnorderedElementsAre(
      source.AllGames()[1], source.AllGames()[2], source.AllGames()[3],
      source.AllGames()[4], source.AllGames()[5], source.AllGames()[6]));
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
      source.AllGames()[1], source.AllGames()[2], source.AllGames()[3],
      source.AllGames()[4], source.AllGames()[5], source.AllGames()[6]}));
  EXPECT_THAT(games.next_moves, UnorderedElementsAre(
      Pair(34, 3), Pair(18, 3), Pair(20, 1)
  ));
}

TEST_F(ThorSourceTest, ByBlackLimited) {
  Source source(kFolder);
  auto games = source.GetGames(Sequence("e6"), 2, {"player0"});
  EXPECT_EQ(games.num_games, 3);
  EXPECT_THAT(games.examples, UnorderedElementsAre(source.AllGames()[1], source.AllGames()[4]))
      << source.AllGames()[1] << "\n" << source.AllGames()[4] << "\n";
  EXPECT_THAT(games.next_moves, UnorderedElementsAre(Pair(34, 1), Pair(18, 2)));
}

TEST_F(ThorSourceTest, ByBlackLimitedSubset) {
  Source source(kFolder);
  auto games = source.GetGames(Sequence("e6"), 1, {"player0"});
  EXPECT_EQ(games.num_games, 3);
  EXPECT_EQ(games.examples.size(), 1);
  EXPECT_THAT(games.examples, IsSubsetOf({source.AllGames()[1], source.AllGames()[4]}));
  EXPECT_THAT(games.next_moves, UnorderedElementsAre(Pair(34, 1), Pair(18, 2)));
}

TEST_F(ThorSourceTest, NextMoves) {
  Source source(kFolder);
  auto games = source.GetGames(Sequence("e6f6"), 100, {}, {}, {}, 2022, 2022);
  EXPECT_THAT(games.next_moves, UnorderedElementsAre(Pair(kNoSquare, 1)));
}

TEST_F(ThorSourceTest, Rotate) {
  Source source(kFolder);
  auto games = source.GetGames(Sequence("e6f4c3c4c5c6"), 10);
  games.Rotate(Sequence("c4e3f6e6d6c6"));
  EXPECT_EQ(games.num_games, 1);
  EXPECT_THAT(games.examples, UnorderedElementsAre(source.AllGames()[5]));
  EXPECT_THAT(games.next_moves, UnorderedElementsAre(Pair(22, 1)));
}

TEST_F(ThorSourceTest, SameExampleOrderWhenFiltering) {
  Source source(kFolder);
  auto examples1 = source.GetGames(Sequence(""), 100, {}, {}, {"tournament1"}).examples;
  auto examples2 = source.GetGames(Sequence("e6"), 100, {}, {}, {"tournament1"}).examples;
  int i = -1;
  for (int j = 0; j < examples2.size(); ++j) {
    ++i;
    while (examples1[i] != examples2[j]) {
      ++i;
      ASSERT_LT(i, examples1.size());
    }
  }
}

TEST_F(ThorSourceTest, SameExampleOrderWhenNotFiltering) {
  Source source(kFolder);
  auto examples1 = source.GetGames(Sequence(""), 10).examples;
  auto examples2 = source.GetGames(Sequence("e6"), 10).examples;
  int i = -1;
  for (int j = 0; j < examples2.size(); ++j) {
    ++i;
    while (examples1[i] != examples2[j]) {
      ++i;
      ASSERT_LT(i, examples1.size());
    }
  }
}

TEST_F(ThorSourceTest, SameExampleOrderWhenFilteringAndNotFiltering) {
  Source source(kFolder);
  auto examples1 = source.GetGames(Sequence("e6"), 100).examples;
  auto examples2 = source.GetGames(Sequence("e6"), 100, {}, {}, {"tournament1"}).examples;
  int i = -1;
  for (int j = 0; j < examples2.size(); ++j) {
    ++i;
    while (examples1[i] != examples2[j]) {
      ++i;
      ASSERT_LT(i, examples1.size());
    }
  }
}

TEST_F(ThorSourceTest, KeepsFirstExamplesWhenFiltering) {
  Source source(kFolder);
  for (int t = 1; t < 10; ++t) {
    auto examples1 = source.GetGames(Sequence("e6"), 100, {}, {}, {"tournament1"}).examples;
    auto examples2 = source.GetGames(Sequence("e6"), t, {}, {}, {"tournament1"}).examples;
    assert(examples1.size() >= examples2.size());
    examples1.erase(examples1.begin() + examples2.size(), examples1.end());
    ASSERT_EQ(examples1, examples2);
  }
}

TEST_F(ThorSourceTest, KeepsFirstExamplesWhenNotFiltering) {
  Source source(kFolder);
  for (int t = 1; t < 10; ++t) {
    auto examples1 = source.GetGames(Sequence("e6"), 100).examples;
    auto examples2 = source.GetGames(Sequence("e6"), t).examples;
    assert(examples1.size() >= examples2.size());
    examples1.erase(examples1.begin() + examples2.size(), examples1.end());
    ASSERT_EQ(examples1, examples2);
  }
}

TEST_F(ThorSourceTest, SaveLoadGamesSmallHash) {
  Source source(kFolder);
  source.SaveGamesSmallHash();
  Source source1(kFolder);
  auto examples = source.GetGames(Sequence("e6"), 100).examples;
  auto examples1 = source.GetGames(Sequence("e6"), 100).examples;
  EXPECT_EQ(examples, examples1);
}

template <typename GameGetter>
class SourceTest : public ::testing::Test {
 public:
  typedef Source<GameGetter> SourceToTest;
};

using Types = ::testing::Types<GameGetterOnDisk, GameGetterInMemory>;
TYPED_TEST_SUITE(SourceTest, Types);

TYPED_TEST(SourceTest, EndToEnd) {
  fs::remove_all(kFolder);
  fs::create_directories(kFolder);
  int start_year = 1900 + rand() % 100;
  int end_year = start_year + rand() % 5;
  std::vector<Game> games;
  std::vector<std::string> players;
  std::vector<std::string> tournaments;

  std::ofstream players_file(kFolder + "/WTH.JOU", std::ios::binary);
  WriteHeader(players_file, end_year);
  for (int i = 0; i < 20; ++i) {
    std::string name = "player " + std::to_string(i);
    players.push_back(name);
    std::string padded = PadString(name, 20);
    players_file.write(padded.c_str(), 20);
  }
  players_file.close();

  std::ofstream tournaments_file(kFolder + "/WTH.TRN", std::ios::binary);
  WriteHeader(tournaments_file, end_year);
  for (int i = 0; i < 30; ++i) {
    std::string name = "tournament " + std::to_string(i);
    tournaments.push_back(name);
    std::string padded = PadString(name, 26);
    tournaments_file.write(padded.c_str(), 26);
  }
  tournaments_file.close();

  for (int year = start_year; year <= end_year; ++year) {
    std::ofstream games_file(kFolder + "/WTH_" + std::to_string(year) + ".wtb", std::ios::binary);
    WriteHeader(games_file, year);
    int num_games = rand() % 200;
    for (int game = 0; game < num_games; ++game) {
      int tournament = rand() % 30;
      int black = rand() % 20;
      int white = rand() % 20;
      int score = (rand() % 32) * 2;
      int theoretical = (rand() % 32) * 2;
      Sequence sequence = Sequence::RandomSequence(rand() % 60).ToCanonicalGame();

      std::vector<char> game_to_store = StoredGame(tournament, black, white, score, theoretical, sequence.ToString());
      games_file.write(game_to_store.data(), game_to_store.size());
      games.push_back(Game(sequence, &players[black], &players[white], &tournaments[tournament],
                           year, score, game / (float) (num_games - 1)));
    }
  }
  typename TestFixture::SourceToTest source(kFolder);

  for (int i = 0; i < 1000; ++i) {
    Sequence sequence = Sequence::RandomSequence(rand() % 10);
    std::vector<Game> expected;
    int max_games = 1 + rand() % 100;
    std::unordered_map<Square, int> next_moves;
    for (const Game& game : games) {
      if (game.Moves().Size() >= sequence.Size() &&
          game.Moves().Subsequence(sequence.Size()) == sequence) {
        expected.push_back(game);
      }
    }
    std::sort(expected.begin(), expected.end(), CompareGamesByHash);
    GamesList actual = source.GetGames(sequence, max_games);
    ASSERT_EQ(actual.num_games, expected.size());
    ASSERT_EQ(actual.max_games, max_games);
    ASSERT_EQ(actual.examples, std::vector<Game>(expected.begin(), std::min(expected.end(), expected.begin() + max_games)));
  }
  fs::remove_all(kFolder);
}
