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

// Copyright 2019 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <gtest/gtest.h>
#include <random>

#include "../board/board.h"
#include "../board/get_moves.h"
#include "test_evaluator.h"


TEST(TestEvaluator, UpdateAndUndo) {
  TestEvaluator eval;
  TestEvaluator test;
  for (int i = 0; i < 10000; ++i) {
    Board b = RandomBoard();
    std::vector<BitPattern> moves = GetAllMoves(b.GetPlayer(), b.GetOpponent());
    if (moves.empty()) {
      continue;
    }
    BitPattern flip = moves[rand() % moves.size()];
    int move = SquareFromFlip(flip, b.GetPlayer(), b.GetOpponent());
    eval.Setup(b.GetPlayer(), b.GetOpponent());
    if (flip == 0) {
      continue;
    }
    eval.Invert();
    eval.Update(move, flip);
    Board after(b.GetPlayer(), b.GetOpponent());
    after.PlayMove(flip);
    test.Setup(after.GetPlayer(), after.GetOpponent());
    EXPECT_EQ(eval.Eval(), test.Eval());

    eval.UndoUpdate(move, flip);
    eval.Invert();
    test.Setup(b.GetPlayer(), b.GetOpponent());
    EXPECT_EQ(eval.Eval(), test.Eval());
  }
}