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

#include "evaluator_alpha_beta.h"
#include "evaluator_last_moves.h"
#include "test_evaluator.h"
#include "../board/board.h"
#include "../board/get_moves.h"
#include "../evaluatedepthone/pattern_evaluator.h"
#include "../evaluatedepthone/test_evaluator_depth_one.h"
#include "../hashmap/hash_map.h"

Eval SolveBasic(long player, long opponent, bool passed) {
  long empties = ~(player | opponent);
  int eval = kMinEvalLarge;

  for (Square m = 0; m < 64; ++m) {
    if (((1L << m) & empties) == 0) {
      continue;
    }
    long flip = GetFlip(m, player, opponent);
    if (flip == 0) {
      continue;
    }
    eval = std::max(eval, -SolveBasic(opponent & ~flip, player | flip, false));
  }
  if (eval == kMinEvalLarge) {
    if (passed) {
      return GetEvaluationGameOver(player, opponent);
    }
    return -SolveBasic(opponent, player, true);
  }
  return eval;
}

class EvaluatorAlphaBetaEndgameTest : public testing::Test {
 protected:
  EvalType evals_ = LoadEvals();
};

TEST(EvaluatorAlphaBetaTest, InitialBoard) {
  HashMap<kBitHashMap> hash_map;
  EvaluatorAlphaBeta eval(&hash_map, TestEvaluatorDepthOne::Factory());
  Board board;

  EXPECT_EQ(eval.Evaluate(board.Player(), board.Opponent(), 1),
            8 * (/*depth 1*/(3-2) * kWeightDepthOne + /*depth 0*/ (0+2) * kWeightDepthZero)
            / (kWeightDepthOne + kWeightDepthZero));
  EXPECT_EQ(eval.GetNVisited(), 5);
  EXPECT_EQ(eval.Evaluate(board.Player(), board.Opponent(), 2),
            8 * (/*depth 2*/(0+2) * kWeightDepthOne + /*depth 1*/ (3-2) * kWeightDepthZero)
            / (kWeightDepthOne + kWeightDepthZero));
  EXPECT_EQ(eval.GetNVisited(), 1 + 4 + 6  /*3 for first move, 1 for other 3 */);
}

TEST(EvaluatorAlphaBetaTest, Pass) {
  HashMap<kBitHashMap> hash_map;
  EvaluatorAlphaBeta eval(&hash_map, TestEvaluatorDepthOne::Factory());
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

TEST(EvaluatorAlphaBetaTest, UsesHashMap) {
  HashMap<kBitHashMap> hash_map;
  EvaluatorAlphaBeta evaluator(&hash_map, TestEvaluatorDepthOne::Factory());
  Board b = RandomBoard(0.45, 0.45);

  while (b.NEmpties() > 10 || b.NEmpties() < 6) {
    b = RandomBoard(0.45, 0.45);
  }

  int n_visited = 0;
  int actual = evaluator.Evaluate(b.Player(), b.Opponent(), 64, kMinEvalLarge, kMaxEvalLarge);
  EXPECT_GT(n_visited, 1);

  if (b.NEmpties() > kMinEmptiesForHashMap) {
    n_visited = 0;
    actual = evaluator.Evaluate(b.Player(), b.Opponent(), 64, kMinEvalLarge, kMaxEvalLarge);
    EXPECT_EQ(n_visited, 1);
  }
}

TEST(EvaluatorAlphaBetaTest, CompareWithTest) {
  HashMap<kBitHashMap> hash_map;
  EvaluatorAlphaBeta eval(&hash_map, TestEvaluatorDepthOne::Factory());
  TestEvaluator test_eval(TestEvaluatorDepthOne::Factory());

  for (int i = 0; i < 1000; ++i) {
    Board board = RandomBoard();
    for (int d = 1; d <= 4; ++d) {
      if (d >= board.NEmpties()) {
        break;
      }
      ASSERT_EQ(
          test_eval.Evaluate(board.Player(), board.Opponent(), d),
          eval.Evaluate(board.Player(), board.Opponent(), d)) << board << " " << d;
    }
  }
}

TEST_F(EvaluatorAlphaBetaEndgameTest, Endgame) {
  HashMap<kBitHashMap> hash_map;
  EvaluatorAlphaBeta evaluator(&hash_map, PatternEvaluator::Factory(evals_.data()));
  for (int i = 0; i < 1000; i++) {
    double perc_player = (double) rand() / RAND_MAX * 0.9;
    double perc_opponent = 0.9 - perc_player;
    Board b = RandomBoard(perc_player, perc_opponent);

    int n_visited = 0;
    int actual;
    BitPattern empties = b.Empties();
    int nEmpties = b.NEmpties();

    if (nEmpties < 6 || nEmpties > 8) {
      i--;
      continue;
    }
    actual = evaluator.Evaluate(b.Player(), b.Opponent(), 64);
    int expected = SolveBasic(b.Player(), b.Opponent(), false) * 8;

    ASSERT_EQ(actual, expected) << b;
  }
}

