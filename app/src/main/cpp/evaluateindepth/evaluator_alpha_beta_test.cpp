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

#include <gtest/gtest.h>
#include <random>

#include "evaluator_alpha_beta.h"
#include "evaluator_last_moves.h"
#include "test_evaluator.h"
#include "../board/board.h"
#include "../board/get_moves.h"
#include "../evaluatedepthone/pattern_evaluator.h"
#include "../evaluatedepthone/test_evaluator_depth_one.h"
#include "../hashmap/hash_map.h"

int EvalBasic(long player, long opponent, bool passed) {
  long empties = ~(player | opponent);
  int eval = -66;

  for (Square m = 0; m < 64; ++m) {
    if (((1L << m) & empties) == 0) {
      continue;
    }
    long flip = GetFlip(m, player, opponent);
    if (flip == 0) {
      continue;
    }
    eval = std::max(eval, -EvalBasic(opponent & ~flip, player | flip, false));
  }
  if (eval == -66) {
    if (passed) {
      return GetEvaluationGameOver(player, opponent);
    }
    return -EvalBasic(opponent, player, true);
  }
  return eval;
}

//TEST(EvaluatorAlphaBeta, InitialBoard) {
//  HashMap hash_map;
//  EvaluatorAlphaBeta eval(&hash_map, TestEvaluatorDepthOne::Create);
//  Board board;
//
//  EXPECT_EQ(eval.Evaluate(board.GetPlayer(), board.GetOpponent(), 1),
//            8 * (/*depth 1*/(3-2) * kWeightDepthOne + /*depth 0*/ (0+2) * kWeightDepthZero)
//            / (kWeightDepthOne + kWeightDepthZero));
//  EXPECT_EQ(eval.GetNVisited(), 5);
//  EXPECT_EQ(eval.Evaluate(board.GetPlayer(), board.GetOpponent(), 2),
//            8 * (/*depth 2*/(0+2) * kWeightDepthOne + /*depth 1*/ (3-2) * kWeightDepthZero)
//            / (kWeightDepthOne + kWeightDepthZero));
//  EXPECT_EQ(eval.GetNVisited(), 1 + 4 + 12);
//}
//
//TEST(EvaluatorAlphaBeta, CompareWithTest) {
//  HashMap hash_map;
//  EvaluatorAlphaBeta eval(&hash_map, TestEvaluatorDepthOne::Create);
//  TestEvaluator test_eval(TestEvaluatorDepthOne::Create);
//
//  for (int i = 0; i < 10; ++i) {
//    Board board = RandomBoard();
//    for (int d = 1; d < 4; ++d) {
//      EXPECT_EQ(
//          test_eval.Evaluate(board.GetPlayer(), board.GetOpponent(), d),
//          eval.Evaluate(board.GetPlayer(), board.GetOpponent(), d));
//    }
//  }
//}

TEST(EvaluatorLastMoves, Endgame) {
  HashMap hash_map;
  EvaluatorAlphaBeta evaluator(&hash_map, &PatternEvaluator::Create);
//  EvaluatorLastMoves evaluator(&hash_map);
  for (int i = 0; i < 10000; i++) {
    double perc_player = (double) rand() / RAND_MAX * 0.9;
    double perc_opponent = 0.9 - perc_player;
    Board b = RandomBoard(perc_player, perc_opponent);

    int n_visited = 0;
    int actual;
    BitPattern empties = b.GetEmpties();
    int nEmpties = b.NEmpties();

    if (nEmpties < 6 || nEmpties > 8) {
      i--;
      continue;
    }
    actual = evaluator.Evaluate(b.GetPlayer(), b.GetOpponent(), 64);
    int expected = EvalBasic(b.GetPlayer(), b.GetOpponent(), false) * 8;

    EXPECT_EQ(actual, expected);
  }
}

