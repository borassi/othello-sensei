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
#include "../board/bitpattern.h"
#include "../board/board.h"
#include "evaluator_last_moves.h"

Board THREE_EMPTIES(
        "-XXXOOOO"
        "XXXXOOOO"
        "XXXXOOOO"
        "XXXXOOOO"
        "X-XXOOOO"
        "XXXXOOOO"
        "XXXXOOOO"
        "XXXXOOO-", true);


int SolveBasic(long player, long opponent, bool passed) {
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
    eval = std::max(eval, -SolveBasic(opponent & ~flip, player | flip, false));
  }
  if (eval == -66) {
    if (passed) {
      return GetEvaluationGameOver(player, opponent);
    }
    return -SolveBasic(opponent, player, true);
  }
  return eval;
}

TEST(EvaluatorLastMoves, DoublePass) {
  int visited = 0;
  Board b("--OOOOOOX-OOOOO-XOOOOOXXXOOXOXXOXOOOXXOOXOOOOOOOXOOOOO--XXXXXXXX", true);
  EXPECT_EQ(SolveBasic(b.Player(), b.Opponent(), false), 0);
  b = Board("--OOOOOOX-OOOOO-XOOOOOXXXOOXOXXOXOOOXXOOXOOOOOOOXOOOOO--XXXXXXOX", true);
  EXPECT_EQ(SolveBasic(b.Player(), b.Opponent(), false), -4);
}

TEST(EvaluatorLastMoves, ManyPass) {
  Board b("XXXXXXXXXXXX-XXXXXXXXXXXXXOXXXXX-XXXXXX-XXXX-XX-XX-XXXXXXXX-XXXX", true);
  EXPECT_EQ(SolveBasic(b.Player(), b.Opponent(), false), 24);
}

TEST(EvaluatorLastMoves, E2E) {
  HashMap<kBitHashMap> hash_map;
//  EvaluatorLastMoves evaluator(&hash_map);
  for (int i = 0; i < 10000; i++) {
    double perc_player = (double) rand() / RAND_MAX * 0.9;
    double perc_opponent = 0.9 - perc_player;
    Board b = RandomBoard(perc_player, perc_opponent);

    int n_visited = 0;
    int actual;
    BitPattern empties = b.Empties();
    int nEmpties = b.NEmpties();
    Square x[4];
    int xCounter = 0;
    
    if (nEmpties > 5 || nEmpties == 0) {
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
    if (nEmpties == 5) {
      actual = EvalFiveEmpties(b.Player(), b.Opponent(), -64, 64, 0, 0, &n_visited);
    } else if (nEmpties == 4) {
      actual = EvalFourEmpties(x[0], x[1], x[2], x[3], b.Player(), b.Opponent(), -64, 64, rand() % 2, 0, 0, &n_visited);
    } else if (nEmpties == 3) {
      actual = EvalThreeEmpties(x[0], x[1], x[2], b.Player(), b.Opponent(), -64, 64, &n_visited);
    } else if (nEmpties == 2) {
      actual = EvalTwoEmpties(x[0], x[1], b.Player(), b.Opponent(), -64, 64, &n_visited);
    } else if (nEmpties == 1) {
      actual = EvalOneEmpty(x[0], b.Player(), b.Opponent());
    }
    int expected = SolveBasic(b.Player(), b.Opponent(), false);

    EXPECT_EQ(actual, expected);
  }
}

