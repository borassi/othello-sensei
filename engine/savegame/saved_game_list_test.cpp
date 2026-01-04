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
#include <thread>     // For std::this_thread::sleep_for

#include "../board/sequence.h"
#include "../utils/files.h"
#include "saved_game_list.h"

using ::testing::Pair;
using ::testing::UnorderedElementsAre;
using ::testing::ElementsAre;

const std::string kTestFolder = "tmp/testdata/saved_games_test";

class SavedGamesListTest : public testing::Test {
 public:

  void SetUp() override {
    fs::remove_all(kTestFolder);
    fs::create_directories(kTestFolder);
  }

  void TearDown() override {
    fs::remove_all(kTestFolder);
  }

  void SaveGame(const GameToSave& game, const std::string& subpath) {
    std::ofstream file(kTestFolder + "/" + subpath);
    file << game.ToString();
    file.close();
  }
};

TEST_F(SavedGamesListTest, Basic) {
  GameToSave game(
      Sequence("e6f4c3c4d3d6e3c2b3d2c5f5f3f6e1d1e2f1g4g3g5h5f2h4c7g6e7b4h3a4b1g1b5h2f7d8e8f8d7c8c6b8a3a2g7h6h8h7b2a1h1c1g8b6a5g2a7b7a8a6"),
      "Black 1",
      "White 1",
      "Tournament 1",
      "Notes 1",
      2022,
      34,
      "Round 1");
  SaveGame(game, "game.sensei.txt");

  SavedGameList source(kTestFolder);
  EXPECT_THAT(source.Tournaments(), UnorderedElementsAre("Tournament 1"));
  EXPECT_THAT(source.Players(), UnorderedElementsAre("Black 1", "White 1"));
  auto games = source.GetGames(Sequence("e6f4c3c4d3"), 10);
  EXPECT_EQ(source.GetFolder(), kTestFolder);
  EXPECT_EQ(source.NumGames(), 1);
  EXPECT_EQ(games.max_games, 10);
  EXPECT_EQ(games.num_games, 1);
  EXPECT_THAT(games.examples, UnorderedElementsAre(game.ToGame(true)));
  EXPECT_THAT(games.next_moves, UnorderedElementsAre(Pair(20, 1)));
}

TEST_F(SavedGamesListTest, Invalid) {
  SavedGameList source(kTestFolder + "/IAmNotADirectory");
  EXPECT_THAT(source.Tournaments(), ::testing::IsEmpty());
  EXPECT_THAT(source.Players(), ::testing::IsEmpty());
  EXPECT_EQ(source.GetFolder(), kTestFolder + "/IAmNotADirectory");
  EXPECT_EQ(source.NumGames(), 0);
  auto games = source.GetGames(Sequence("e6f4c3c4d3"), 10);
  EXPECT_EQ(games.max_games, 10);
  EXPECT_EQ(games.num_games, 0);
  EXPECT_THAT(games.examples, ::testing::IsEmpty());
  EXPECT_THAT(games.next_moves, ::testing::IsEmpty());
}

TEST_F(SavedGamesListTest, LongerThanGame) {
  GameToSave game(Sequence("e6f4c3c4d3"), "B", "W", "T", "N", 2022, 34,"R");
  SaveGame(game, "game.sensei.txt");
  SavedGameList source(kTestFolder);
  auto games = source.GetGames(Sequence("e6f4c3c4d3d6"), 10);
  EXPECT_EQ(games.num_games, 0);
}

TEST_F(SavedGamesListTest, AsLongAsGame) {
  GameToSave game(Sequence("e6f4c3c4d3"), "B", "W", "T", "N", 2022, 34,"R");
  SaveGame(game, "game.sensei.txt");
  SavedGameList source(kTestFolder);
  auto games = source.GetGames(Sequence("e6f4c3c4d3"), 10);
  EXPECT_EQ(games.num_games, 1);
  EXPECT_THAT(games.examples, UnorderedElementsAre(game.ToGame(true)));
  EXPECT_THAT(games.next_moves, UnorderedElementsAre(Pair(kNoSquare, 1)));
}

TEST_F(SavedGamesListTest, Canonical) {
  GameToSave game(Sequence("f5d6c3d3c4"), "B", "W", "T", "N", 2022, 34,"R");
  SaveGame(game, "game.sensei.txt");
  SavedGameList source(kTestFolder);
  auto games = source.GetGames(Sequence("e6f4c3c4d3"), 10);
  EXPECT_EQ(games.num_games, 1);
  EXPECT_THAT(games.examples, UnorderedElementsAre(game.ToGame(true)));
  EXPECT_THAT(games.next_moves, UnorderedElementsAre(Pair(kNoSquare, 1)));
}

TEST_F(SavedGamesListTest, ByBlack) {
  GameToSave game1(Sequence("e6f4c3c4d3"), "B1", "W1", "T1", "N", 2021, 34, "R");
  SaveGame(game1, "game1.sensei.txt");
  GameToSave game2(Sequence("e6f4c3c4d3"), "B2", "W2", "T2", "N", 2022, 34, "R");
  SaveGame(game2, "game2.sensei.txt");
  SavedGameList source(kTestFolder);
  auto games = source.GetGames(Sequence("e6"), 10, {"B1"});
  EXPECT_EQ(games.num_games, 1);
  EXPECT_THAT(games.examples, UnorderedElementsAre(game1.ToGame(true)));
}

TEST_F(SavedGamesListTest, ByWhite) {
  GameToSave game1(Sequence("e6f4c3c4d3"), "B1", "W1", "T1", "N", 2021, 34, "R");
  SaveGame(game1, "game1.sensei.txt");
  GameToSave game2(Sequence("e6f4c3c4d3"), "B2", "W2", "T2", "N", 2022, 34, "R");
  SaveGame(game2, "game2.sensei.txt");
  SavedGameList source(kTestFolder);
  auto games = source.GetGames(Sequence("e6"), 10, {}, {"W2"});
  EXPECT_EQ(games.num_games, 1);
  EXPECT_THAT(games.examples, UnorderedElementsAre(game2.ToGame(true)));
}

TEST_F(SavedGamesListTest, ByTournament) {
  GameToSave game1(Sequence("e6f4c3c4d3"), "B1", "W1", "T1", "N", 2021, 34, "R");
  SaveGame(game1, "game1.sensei.txt");
  GameToSave game2(Sequence("e6f4c3c4d3"), "B2", "W2", "T2", "N", 2022, 34, "R");
  SaveGame(game2, "game2.sensei.txt");
  SavedGameList source(kTestFolder);
  auto games = source.GetGames(Sequence("e6"), 10, {}, {}, {"T1"});
  EXPECT_EQ(games.num_games, 1);
  EXPECT_THAT(games.examples, UnorderedElementsAre(game1.ToGame(true)));
}

TEST_F(SavedGamesListTest, ByYear) {
  GameToSave game1(Sequence("e6f4c3c4d3"), "B1", "W1", "T1", "N", 2021, 34, "R");
  SaveGame(game1, "game1.sensei.txt");
  GameToSave game2(Sequence("e6f4c3c4d3"), "B2", "W2", "T2", "N", 2022, 34, "R");
  SaveGame(game2, "game2.sensei.txt");
  GameToSave game3(Sequence("e6f4c3c4d3"), "B2", "W2", "T2", "N", 2023, 34, "R");
  SaveGame(game3, "game3.sensei.txt");
  SavedGameList source(kTestFolder);
  auto games = source.GetGames(Sequence("e6"), 10, {}, {}, {}, 2022, 2022);
  EXPECT_EQ(games.num_games, 1);
  EXPECT_THAT(games.examples, UnorderedElementsAre(game2.ToGame(true)));
}

TEST_F(SavedGamesListTest, SameExampleOrder) {
  GameToSave game1(Sequence("e6f4c3c4d3"), "B1", "W1", "T1", "N", 2021, 34, "R");
  SaveGame(game1, "game_c_1.sensei.txt");
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  GameToSave game2(Sequence("e6f4c3c4d3"), "B2", "W2", "T2", "N", 2022, 34, "R");
  SaveGame(game2, "game_a_2.sensei.txt");
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  GameToSave game3(Sequence("e6f4c3c4d3"), "B3", "W3", "T2", "N", 2023, 34, "R");
  SaveGame(game3, "game_b_3.sensei.txt");
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  GameToSave game4(Sequence("e6f4c3c4d3"), "B4", "W4", "T2", "N", 2023, 34, "R");
  SaveGame(game4, "game_d_4.sensei.txt");
  SavedGameList source(kTestFolder);
  auto games = source.GetGames(Sequence("e6"), 10);
  EXPECT_EQ(games.num_games, 4);
  EXPECT_THAT(games.examples, ElementsAre(game4.ToGame(true), game3.ToGame(true), game2.ToGame(true), game1.ToGame(true)));
}

TEST_F(SavedGamesListTest, FiltersLastModified) {
  GameToSave game1(Sequence("e6f4c3c4d3"), "B1", "W1", "T1", "N", 2021, 34, "R");
  SaveGame(game1, "game_c_1.sensei.txt");
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  GameToSave game2(Sequence("e6f4c3c4d3"), "B2", "W2", "T2", "N", 2022, 34, "R");
  SaveGame(game2, "game_a_2.sensei.txt");
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  GameToSave game3(Sequence("e6f4c3c4d3"), "B3", "W3", "T2", "N", 2023, 34, "R");
  SaveGame(game3, "game_b_3.sensei.txt");
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  GameToSave game4(Sequence("e6f4c3c4d3"), "B4", "W4", "T2", "N", 2023, 34, "R");
  SaveGame(game4, "game_d_4.sensei.txt");
  SavedGameList source(kTestFolder);
  auto games = source.GetGames(Sequence("e6"), 2);
  EXPECT_EQ(games.num_games, 4);
  EXPECT_THAT(games.examples, ElementsAre(game4.ToGame(true), game3.ToGame(true)));
}

TEST_F(SavedGamesListTest, Recursive) {
  fs::create_directories(kTestFolder + "/subfolder1");
  fs::create_directories(kTestFolder + "/subfolder2");

  GameToSave game1(Sequence("e6f4c3c4d3"), "B1", "W", "T1", "N", 2022, 34,"R");
  GameToSave game2(Sequence("e6f4c3c4d3"), "B2", "W", "T2", "N", 2022, 34,"R");
  GameToSave game3(Sequence("e6f4c3c4d3"), "B3", "W", "T3", "N", 2022, 34,"R");
  SaveGame(game1, "game.sensei.txt");
  SaveGame(game2, "subfolder1/game.sensei.txt");
  SaveGame(game3, "subfolder2/game.sensei.txt");

  SavedGameList source(kTestFolder);
  EXPECT_THAT(source.Tournaments(), UnorderedElementsAre("T1", "T2", "T3"));
  EXPECT_THAT(source.Players(), UnorderedElementsAre("B1", "B2", "B3", "W"));
  auto games = source.GetGames(Sequence("e6f4c3c4d3"), 10);
  EXPECT_EQ(games.num_games, 3);
  EXPECT_THAT(games.examples, UnorderedElementsAre(game1.ToGame(true), game2.ToGame(true), game3.ToGame(true)));
}

TEST_F(SavedGamesListTest, MaxGames) {
  GameToSave game1(Sequence("e6f4c3c4d3"), "B1", "W", "T1", "N", 2022, 34,"R");
  GameToSave game2(Sequence("e6f4c3c4d3"), "B2", "W", "T2", "N", 2022, 34,"R");
  GameToSave game3(Sequence("e6f4c3c4d3"), "B3", "W", "T3", "N", 2022, 34,"R");
  SaveGame(game1, "game1.sensei.txt");
  SaveGame(game2, "game2.sensei.txt");
  SaveGame(game3, "game3.sensei.txt");

  SavedGameList source(kTestFolder, 2);
  EXPECT_EQ(source.Tournaments().size(), 2);
  EXPECT_THAT(source.Players().size(), 3);
  auto games = source.GetGames(Sequence("e6f4c3c4d3"), 10);
  EXPECT_EQ(games.num_games, 2);
}

TEST_F(SavedGamesListTest, MaxGamesWithOtherFiles) {
  GameToSave game1(Sequence("e6f4c3c4d3"), "B1", "W", "T1", "N", 2022, 34,"R");
  GameToSave game2(Sequence("e6f4c3c4d3"), "B2", "W", "T2", "N", 2022, 34,"R");
  GameToSave game3(Sequence("e6f4c3c4d3"), "B3", "W", "T3", "N", 2022, 34,"R");
  SaveGame(game1, "game1.sensei.txt");
  SaveGame(game2, "game2.sensei.txt");
  SaveGame(game3, "game3.sensei.txt");
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  fs::create_directories(kTestFolder + "/test_dir");
  SaveGame(game1, "game3_not_sensei.txt.ext");

  SavedGameList source(kTestFolder, 4);
  EXPECT_EQ(source.Tournaments().size(), 2);
  EXPECT_THAT(source.Players().size(), 3);
  auto games = source.GetGames(Sequence("e6f4c3c4d3"), 10);
  EXPECT_EQ(games.num_games, 2);
}