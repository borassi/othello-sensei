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
#include "bitpattern.h"
#include "board.h"
#include "evaluator_last_moves.h"

Board THREE_EMPTIES(
        "-XXXOOOO"
        "XXXXOOOO"
        "XXXXOOOO"
        "XXXXOOOO"
        "X-XXOOOO"
        "XXXXOOOO"
        "XXXXOOOO"
        "XXXXOOO-");


int EvalBasic(long player, long opponent, bool passed) {
  long empties = ~(player | opponent);
  int eval = -66;

  for (Move m = 0; m < 64; ++m) {
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

TEST(EvaluatorLastMoves, DoublePass) {
  int visited = 0;
  Board b("--OOOOOOX-OOOOO-XOOOOOXXXOOXOXXOXOOOXXOOXOOOOOOOXOOOOO--XXXXXXXX");
  EXPECT_EQ(EvalBasic(b.GetPlayer(), b.GetOpponent(), false), 0);
  b = Board("--OOOOOOX-OOOOO-XOOOOOXXXOOXOXXOXOOOXXOOXOOOOOOOXOOOOO--XXXXXXOX");
  EXPECT_EQ(EvalBasic(b.GetPlayer(), b.GetOpponent(), false), -4);
}

TEST(EvaluatorLastMoves, ManyPass) {
  Board b("XXXXXXXXXXXX-XXXXXXXXXXXXXOXXXXX-XXXXXX-XXXX-XX-XX-XXXXXXXX-XXXX");
  EXPECT_EQ(EvalBasic(b.GetPlayer(), b.GetOpponent(), false), 24);
}

TEST(EvaluatorLastMoves, E2E) {
  for (int i = 0; i < 10000; i++) {
    double perc_player = (double) rand() / RAND_MAX * 0.9;
    double perc_opponent = 0.9 - perc_player;
    Board b = RandomBoard(perc_player, perc_opponent);

    int nVisited = 0;
    int actual;
    BitPattern empties = b.GetEmpties();
    int nEmpties = b.NEmpties();
    Move x[4];
    int xCounter = 0;
    
    if (nEmpties > 8 || nEmpties == 0) {
      i--;
      continue;
    }
    if (nEmpties <= 4) {
      while (empties != 0) {
        x[xCounter] = __builtin_ctzll(empties);
        empties = empties & ~(1ULL << x[xCounter]);
        xCounter++;
      }
    }
    if (nEmpties > 5) {
      actual = Evaluate(b.GetPlayer(), b.GetOpponent(), -64, 64, &nVisited);
    } else if (nEmpties == 5) {
      actual = EvalFiveEmpties(b.GetPlayer(), b.GetOpponent(), -64, 64, 0, 0, &nVisited);
    } else if (nEmpties == 4) {
      actual = EvalFourEmpties(x[0], x[1], x[2], x[3], b.GetPlayer(), b.GetOpponent(), -64, 64, rand() % 2, 0, 0, &nVisited);
    } else if (nEmpties == 3) {
      actual = EvalThreeEmpties(x[0], x[1], x[2], b.GetPlayer(), b.GetOpponent(), -64, 64, &nVisited);
    } else if (nEmpties == 2) {
      actual = EvalTwoEmpties(x[0], x[1], b.GetPlayer(), b.GetOpponent(), -64, 64, &nVisited);
    } else if (nEmpties == 1) {
      actual = EvalOneEmpty(x[0], b.GetPlayer(), b.GetOpponent());
    }
    int expected = EvalBasic(b.GetPlayer(), b.GetOpponent(), false);

    EXPECT_EQ(actual, expected);
  }
}

