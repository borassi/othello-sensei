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
#include "../board/bitpattern.h"
#include "hash_map.h"
#include "../board/board.h"

void ExpectEntryEq(
  const HashMapEntry& entry, BitPattern player, BitPattern opponent,
  Eval lower, Eval upper, DepthValue depth,
  Square best_move, Square second_best_move) {
  EXPECT_EQ(entry.Player(), player);
  EXPECT_EQ(entry.Opponent(), opponent);
  EXPECT_EQ(entry.Lower(), lower);
  EXPECT_EQ(entry.Upper(), upper);
  EXPECT_EQ(entry.Depth(), depth);
  EXPECT_EQ(entry.BestMove(), best_move);
  EXPECT_EQ(entry.SecondBestMove(), second_best_move);
}

//            Epoch changes     Player-opp changes        Same board
// Update            Always        If better depth   If better depth
// Delete               Yes                    Yes                No
TEST(HashMapEntry, Update) {
  HashMapEntry entry;

  // BASE
  {
    // Update both
    ExpectEntryEq(entry, 0, 0, kMinEval, kMaxEval, 0, kNoSquare, kNoSquare);

    entry.Update(1, 2, 0, 4, 1, -2, 2, 34, 35);
    ExpectEntryEq(entry, 1, 2, 1, 1, 4, 34, 35);
    // Update only lower
    entry.Update(1, 2, 0, 6, 4, -2, 2, 36, 37);
    ExpectEntryEq(entry, 1, 2, 4, kMaxEval, 6, 36, 37);
    // Update only upper
    entry.Update(1, 2, 0, 8, -4, -2, 2, 38, 39);
    ExpectEntryEq(entry, 1, 2, kMinEval, -4, 8, 38, 39);
  }

  // NEW EPOCH
  {
    entry.Update(1, 2, 1, 6, 1, -2, 2, 34, 35);
    // Update in any case.
    entry.Update(1, 4, 2, 4, 0, -2, 2, 36, 37);
    ExpectEntryEq(entry, 1, 4, 0, 0, 4, 36, 37);

    entry.Update(1, 4, 3, 2, 0, -2, 2, 38, 39);
    ExpectEntryEq(entry, 1, 4, 0, 0, 2, 38, 39);
  }

  // SAME EPOCH, DIFFERENT BOARD
  {
    entry.Update(1, 2, 1, 4, 1, -2, 2, 34, 35);

    // Smaller or equal depth: do not update.
    entry.Update(1, 4, 1, 2, 0, -4, 4, 36, 37);
    ExpectEntryEq(entry, 1, 2, 1, 1, 4, 34, 35);
    entry.Update(1, 4, 1, 4, 0, -4, 4, 36, 37);
    ExpectEntryEq(entry, 1, 2, 1, 1, 4, 34, 35);

    // Higher depth: update.
    entry.Update(1, 4, 1, 6, -6, -4, 4, 36, 37);
    ExpectEntryEq(entry, 1, 4, kMinEval, -6, 6, 36, 37);

    entry.Update(1, 2, 2, 4, 1, -2, 2, 34, 35);
    ExpectEntryEq(entry, 1, 2, 1, 1, 4, 34, 35);
    entry.Update(1, 4, 2, 6, 6, -4, 4, 36, 37);
    ExpectEntryEq(entry, 1, 4, 6, kMaxEval, 6, 36, 37);
  }

  // SAME EPOCH, SAME BOARD
  {
    entry.Update(1, 2, 0, 4, 1, -2, 2, 34, 35);

    // Smaller or equal depth: do not update.
    entry.Update(1, 2, 0, 2, 0, -4, 4, 36, 37);
    ExpectEntryEq(entry, 1, 2, 1, 1, 4, 34, 35);
    entry.Update(1, 2, 0, 4, 0, -4, 4, 36, 37);
    ExpectEntryEq(entry, 1, 2, 1, 1, 4, 34, 35);

    // Higher depth: update.
    entry.Update(1, 2, 1, 6, -6, -4, 4, 36, 37);
    ExpectEntryEq(entry, 1, 2, kMinEval, -6, 6, 36, 37);

    entry.Update(1, 2, 2, 4, 1, -2, 2, 34, 35);
    ExpectEntryEq(entry, 1, 2, 1, 1, 4, 34, 35);
    entry.Update(1, 4, 2, 6, 6, -4, 4, 36, 37);
    ExpectEntryEq(entry, 1, 4, 6, kMaxEval, 6, 36, 37);
  }
}

TEST(HashMapEntry, GetLower) {
  HashMapEntry entry;
  const auto no_result = std::make_pair(kMinEval, kMaxEval);
  entry.Update(1, 4, 2, 6, 2, -4, 4, 36, 37);
  EXPECT_EQ(entry.GetLowerUpper(1, 4, 6), std::make_pair((Eval) 2, (Eval) 2));
  EXPECT_EQ(entry.GetLowerUpper(1, 4, 4), std::make_pair((Eval) 2, (Eval) 2));
  EXPECT_EQ(entry.GetLowerUpper(1, 4, 8), no_result);
  EXPECT_EQ(entry.GetLowerUpper(1, 2, 6), no_result);
  EXPECT_EQ(entry.GetLowerUpper(0, 4, 6), no_result);

  entry.Update(1, 4, 3, 4, -8, -4, 4, 36, 37);
  // Know: <= -8.
  EXPECT_EQ(entry.GetLowerUpper(1, 4, 4), std::make_pair(kMinEval, (Eval) -8));

  entry.Update(1, 4, 4, 4, 8, -4, 4, 36, 37);
  // Know: >= 8.
  EXPECT_EQ(entry.GetLowerUpper(1, 4, 4), std::make_pair((Eval) 8, kMaxEval));
}

TEST(HashMap, Base) {
  HashMap hash_map;
  hash_map.Update(1, 4, 2, 6, 2, -64, 64, 36, 37);
  EXPECT_EQ(hash_map.GetLowerUpper(1, 4, 4), std::make_pair((Eval) 2, (Eval) 2));
}