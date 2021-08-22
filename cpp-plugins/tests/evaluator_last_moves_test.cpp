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

#include "evaluator_last_moves_test.h"
#include "../bitpattern.h"
#include "../board.h"
#include "../evaluator_last_moves.h"


CPPUNIT_TEST_SUITE_REGISTRATION(evaluator_last_moves_test);

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

void evaluator_last_moves_test::testDoublePass() {
  int visited = 0;
  Board b("--OOOOOOX-OOOOO-XOOOOOXXXOOXOXXOXOOOXXOOXOOOOOOOXOOOOO--XXXXXXXX");
  CPPUNIT_ASSERT(EvalBasic(b.GetPlayer(), b.GetOpponent(), false) == 0);
//  std::cout << "\n\n\n" << evaluate(b.getPlayer(), b.getOpponent(), -64, 64, &visited) << "\n\n\n";
//  CPPUNIT_ASSERT(evaluate(b.getPlayer(), b.getOpponent(), -64, 64, &visited) == 0);
  b = Board("--OOOOOOX-OOOOO-XOOOOOXXXOOXOXXOXOOOXXOOXOOOOOOOXOOOOO--XXXXXXOX");
  CPPUNIT_ASSERT(EvalBasic(b.GetPlayer(), b.GetOpponent(), false) == -4);
}

void evaluator_last_moves_test::testManyPass() {
  Board b("XXXXXXXXXXXX-XXXXXXXXXXXXXOXXXXX-XXXXXX-XXXX-XX-XX-XXXXXXXX-XXXX");
  CPPUNIT_ASSERT(EvalBasic(b.GetPlayer(), b.GetOpponent(), false) == 24);
//  CPPUNIT_ASSERT(evaluate(b.getPlayer(), b.getOpponent(), false) == 24);
}

void evaluator_last_moves_test::testE2E() {
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

//    std::cout << nEmpties << b.ToString() << "\n";
    CPPUNIT_ASSERT(actual == expected);
  }
}
//
//  @Test
//  public void testE2E() {
//    EvaluatorBasic eval = new EvaluatorBasic(new PatternEvaluatorImproved(), new PossibleMovesFinderImproved());
//    for (int i = 0; i < 10000; i++) {
//      double percBlack = Math.random() * 0.9;
//      double percWhite = 0.9 - percBlack;
//      Board b = Board.randomBoard(percBlack, percWhite);
//      if (b.getEmptySquares() > 6) {
//        i--;
//        continue;
//      }
//      if (i % 1000 == 0) {
//        System.out.println(i);
//      }
//      float result = eval.evaluate(b.getPlayer(), b.getOpponent(), b.getEmptySquares() + 3, -6400, 6400);
//      int alpha = (int) (result + (Math.random() - 0.5) * 1600);
//      int beta = alpha + 1 + (int) (Math.random() * 800);
//
//      int result1 = LAST_MOVE_EVALUATOR.evaluate(b, alpha, beta, 1L << (int) (Math.random() * 64));
//      if (result < alpha) {
//        if (result > result1 || result1 > alpha) {
//          System.out.println(alpha + " " + beta);
//          System.out.println(b.toString().replace("\n", ""));
//          assert(false);          
//        }
//        assert(result <= result1 && result1 <= alpha);
//      } else if (result > beta) {
//        if (beta > result1 || result1 > result) {
//          System.out.println("FAILED");
//          System.out.println(result);
//          System.out.println(result1);
//          System.out.println(alpha + " " + beta);
//          System.out.println(b.toString().replace("\n", ""));
//          assert(false);
//        }
//        assert(beta <= result1 && result1 <= result);
//      } else {
//        if (result != result1) {
//          System.out.println(b);
//          System.out.println(result);
//          System.out.println(result1);
//          assert(false);          
//        }
//      }
//    }
//  }

