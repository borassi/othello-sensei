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

#include <gtest/gtest.h>
#include <random>

#include "evaluator_alpha_beta_optimized.h"
#include "../board/board.h"
#include "../board/get_moves.h"
#include "../evaluatedepthone/pattern_evaluator.h"
#include "../evaluatedepthone/test_evaluator_depth_one.h"

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

TEST(EvaluatorAlphaBetaOptimizedTest, LastMove) {
  Board b("e6f4c3c4d3d6e3c2b3d2c5f5f3f6e1d1e2f1g4g3g5h5f2h4c7g6e7b4h3a4b1g1b5h2f7d8e8f8d7c8c6b8a3a2g7h6h8h7b2a1h1c1g8b6a5g2a7b7a8");
  EXPECT_EQ(Evaluate(b.Player(), b.Opponent()), 10);
}

TEST(EvaluatorAlphaBetaOptimizedTest, LastMovePass) {
  Board b("e6f4c3c4d3d6e3c2b3d2c5f5f3f6e1d1e2f1g4g3g5h5f2h4c7g6e7b4h3a4b1g1b5h2f7d8e8f8d7c8c6b8a3a2g7h6h8h7b2a1h1c1g8b6a5g2a7a6a8");
  EXPECT_EQ(Evaluate(b.Opponent(), b.Player()), 8);
}

TEST(EvaluatorAlphaBetaOptimizedTest, LastMoveAfterPass) {
  Board b("e6f4c3c4d3d6e3c2b3d2c5f5f3f6e1d1e2f1g4g3g5h5f2h4c7g6e7b4h3a4b1g1b5h2f7d8e8f8d7c8c6b8a3a2g7h6h8h7b2a1h1c1g8b6a5g2a7a6a8");
  EXPECT_EQ(Evaluate(b.Player(), b.Opponent()), -8);
}

TEST(EvaluatorAlphaBetaOptimizedTest, Last2Moves) {
  Board b("e6f4c3c4d3d6e3c2b3d2c5f5f3f6e1d1e2f1g4g3g5h5f2h4c7g6e7b4h3a4b1g1b5h2f7d8e8f8d7c8c6b8a3a2g7h6h8h7b2a1h1c1g8b6a5g2a7b7");
  EXPECT_EQ(Evaluate(b.Player(), b.Opponent()), -10);
}

TEST(EvaluatorAlphaBetaOptimizedTest, Last2MovesPass) {
  Board b("e6f4c3c4d3d6e3c2b3d2c5f5f3f6e1d1e2f1g4g3g5h5f2h4c7g6e7b4h3a4b1g1b5h2f7d8e8f8d7c8c6b8a3a2g7h6h8h7b2a1h1c1g8b6a5g2a7a6");
  EXPECT_EQ(Evaluate(b.Player(), b.Opponent()), -8);
}

TEST(EvaluatorAlphaBetaOptimizedTest, Last3Moves) {
  Board b("e6f4c3c4d3d6e3c2b3d2c5f5f3f6e1d1e2f1g4g3g5h5f2h4c7g6e7b4h3a4b1g1b5h2f7d8e8f8d7c8c6b8a3a2g7h6h8h7b2a1h1c1g8b6a5g2");
  EXPECT_EQ(Evaluate(b.Player(), b.Opponent()), -10);
}

TEST(EvaluatorAlphaBetaOptimizedTest, DoublePassLastMove) {
  Board b("e6f4c3c4d3d6e3c2b3d2c5f5f3f6e1d1e2f1g4g3g5h5f2h4c7g6e7b4h3a4b1g1b5h2f7d8e8f8d7c8c6b8a3a2g7c1h1g2h6g8h7a5h8b6a7a6b7a8");
  EXPECT_EQ(Evaluate(b.Player(), b.Opponent()), 2);
}

TEST(EvaluatorAlphaBetaOptimizedTest, DoublePassLast2Moves) {
  Board b("e6f4c3c4d3d6e3c2b3d2c5f5f3f6e1d1e2f1g4g3g5h5f2h4c7g6e7b4h3a4b1g1b5h2f7d8e8f8d7c8c6b8a3a2g7c1h1g2h6g8h7a5h8b6a7a6b7");
  EXPECT_EQ(Evaluate(b.Player(), b.Opponent()), -2);
}

TEST(EvaluatorAlphaBetaOptimizedTest, Endgame) {
  for (int i = 0; i < 1000; i++) {
    double perc_player = (double) rand() / RAND_MAX * 0.9;
    double perc_opponent = 0.9 - perc_player;
    Board b = RandomBoard(perc_player, perc_opponent);

    int n_visited = 0;
    int actual;
    BitPattern empties = b.Empties();
    int nEmpties = b.NEmpties();

    if (nEmpties > 8) {
      i--;
      continue;
    }
    actual = Evaluate(b.Player(), b.Opponent());
    int expected = SolveBasic(b.Player(), b.Opponent(), false);

    ASSERT_EQ(actual, expected) << b;
  }
}

