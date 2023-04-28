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

#include <gtest/gtest.h>
#include <random>

#include "../board/board.h"
#include "../board/get_moves.h"
#include "test_evaluator_depth_one.h"


TEST(TestEvaluator, UpdateAndUndo) {
  TestEvaluatorDepthOne eval;
  TestEvaluatorDepthOne test;
  for (int i = 0; i < 10000; ++i) {
    Board b = RandomBoard();
    std::vector<BitPattern> moves = GetAllMovesWithPass(b.Player(), b.Opponent());
    if (moves.empty()) {
      continue;
    }

    eval.Setup(b.Player(), b.Opponent());
    eval.Invert();
    for (BitPattern flip : moves) {
      BitPattern square = SquareFromFlip(flip, b.Player(), b.Opponent());
      Board after(b.Player(), b.Opponent());
      after.PlayMove(flip);
      if (flip != 0) {
        eval.Update(square, flip);
      }
      test.Setup(after.Player(), after.Opponent());
      ASSERT_EQ(eval.Evaluate(), test.Evaluate());
      if (flip != 0) {
        eval.UndoUpdate(square, flip);
      }
    }
    eval.Invert();
    test.Setup(b.Player(), b.Opponent());
    ASSERT_EQ(eval.Evaluate(), test.Evaluate());
  }
}
