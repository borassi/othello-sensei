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
#include "../evaluatedepthone/test_evaluator_depth_one.h"
#include "test_evaluator.h"


TEST(TestEvaluator, InitialBoard) {
  TestEvaluator eval(TestEvaluatorDepthOne::Factory());
  Board board;

  EXPECT_EQ(eval.Evaluate(board.Player(), board.Opponent(), 1),
            8 * (/*depth 1*/(3-2) * kWeightDepthOne + /*depth 0*/ (0+2) * kWeightDepthZero)
            / (kWeightDepthOne + kWeightDepthZero));
  EXPECT_EQ(eval.GetNVisited(), 5);
  EXPECT_EQ(eval.Evaluate(board.Player(), board.Opponent(), 2),
            8 * (/*depth 2*/(0+2) * kWeightDepthOne + /*depth 1*/ (3-2) * kWeightDepthZero)
            / (kWeightDepthOne + kWeightDepthZero));
  EXPECT_EQ(eval.GetNVisited(), 1 + 4 + 12);
}

TEST(TestEvaluator, Pass) {
  TestEvaluator eval(TestEvaluatorDepthOne::Factory());
  Board board(
      "XXXXO---"
      "--------"
      "--------"
      "--------"
      "--------"
      "------OO"
      "--------"
      "------OX", true);

  EXPECT_EQ(
      eval.Evaluate(board.Player(), board.Opponent(), 1),
      8 * (/*depth 1*/(4-2) * kWeightDepthOne + /*depth 0*/ (1+2) * kWeightDepthZero)
      / (kWeightDepthOne + kWeightDepthZero));

  EXPECT_EQ(eval.Evaluate(board.Player(), board.Opponent(), 2),
      8 * (/*depth 1*/(7-2) * kWeightDepthOne + /*depth 0*/ (4+2) * kWeightDepthZero)
      / (kWeightDepthOne + kWeightDepthZero));
  EXPECT_EQ(eval.Evaluate(board.Player(), board.Opponent(), 3), 8 * 60);
}
