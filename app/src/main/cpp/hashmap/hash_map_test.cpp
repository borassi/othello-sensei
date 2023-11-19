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

#include <future>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <random>
#include <thread>
#include "hash_map.h"
#include "../board/bitpattern.h"

void CheckCorrect(const HashMapEntry& entry, BitPattern player, BitPattern opponent) {
  int i = entry.player / 2;
  ASSERT_EQ(entry.player, player);
  ASSERT_EQ(entry.opponent, opponent);
  ASSERT_EQ(entry.depth, i % 4);
  ASSERT_EQ(entry.lower, i % 8 + (entry.best_move - 10) * 8);
  ASSERT_EQ(entry.upper, i % 8 + (entry.best_move - 10) * 8);
}

void FillAndTestHashMap(HashMap<5>& hash_map, int thread_num) {
  for (int i = 1; i < 10000; ++i) {
    hash_map.Update(i * 2, 1, i % 4, i % 8 + (thread_num - 10) * 8, kMinEvalLarge, kMaxEvalLarge, thread_num, 0);
    auto entry = hash_map.Get(i * 2, 1);
    if (entry) {
      CheckCorrect(*entry, i * 2, 1);
    }
    int j = rand() % i;
    for (int check : {i, rand() % i, rand() % 10000}) {
      auto entry = hash_map.Get(check * 2, 1);
      if (entry) {
        CheckCorrect(*entry, check * 2, 1);
      }
    }
  }
}

TEST(HashMapTest, Base) {
  HashMap<5> hash_map;
  FillAndTestHashMap(hash_map, 0);
}

TEST(HashMapTest, Parallel) {
  HashMap<5> hash_map;
  std::vector<std::future<void>> futures;
  for (int i = 0; i < 64; ++i) {
    futures.push_back(std::async(std::launch::async, &FillAndTestHashMap, std::ref(hash_map), i));
  }
  for (int i = 0; i < 64; ++i) {
    futures[i].get();
  }
  for (int i = 0; i < 5; ++i) {
    ASSERT_TRUE(hash_map.IsAllFree());
  }
}