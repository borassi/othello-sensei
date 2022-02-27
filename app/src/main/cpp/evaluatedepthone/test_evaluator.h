/*
 * Copyright 2022 Google LLC
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

#include <utility>
#include "../board/board.h"

/**
 * Test-only evaluator that returns the disk difference plus 2.
 */

class TestEvaluator {
 private:
  long player_;
  long opponent_;

 public:
  int Eval() {
    return 100 * (__builtin_popcountll(player_) - __builtin_popcountll(opponent_) + 2);
  }

  void Setup(BitPattern player, BitPattern opponent) {
    player_ = player;
    opponent_ = opponent;
  }

  void Update(BitPattern square, BitPattern flip) {
    assert(__builtin_popcountll(square) == 1);
    BitPattern tmp = player_;
    player_ = NewPlayer(flip, opponent_);
    opponent_ = NewOpponent(flip, tmp);
  }

  void UndoUpdate(BitPattern square, BitPattern flip) {
    assert(__builtin_popcountll(square) == 1);
    BitPattern tmp = player_;
    player_ = (opponent_ & ~flip) & ~(1L << square);
    opponent_ = (tmp | flip) & ~(1L << square);
  }

  void Invert() {}
};