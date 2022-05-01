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
#include "evaluator_derivative.h"
#include "../hashmap/hash_map.h"
#include "../evaluatealphabeta/evaluator_alpha_beta.h"
#include "../evaluatedepthone/pattern_evaluator.h"

TEST(EvaluatorDerivativeTest, Base) {
  EvalType evals = LoadEvals();
  HashMap hash_map;
  EvaluatorAlphaBeta evaluator_alpha_beta(&hash_map, PatternEvaluator::Factory(evals.data()));
  EvaluatorDerivative evaluator_derivative(&evaluator_alpha_beta);
  Board initial_board("e6");
  evaluator_derivative.Evaluate(initial_board.GetPlayer(), initial_board.GetOpponent(), kMinEval + 1, kMaxEval - 1, 100000, 1);
  evaluator_derivative.Evaluate(initial_board.GetPlayer(), initial_board.GetOpponent(), kMinEval + 1, kMaxEval - 1, 100000, 1);
//  std::cout << evaluator_derivative.Get(initial_board)->GetEvaluation(1).ProbGreaterEqual();

//  std::cout << evaluator_derivative.Get(initial_board)->GetEvaluation(1).ProbGreaterEqual() << "\n";
}

TEST(EvaluatorDerivativeTest, Endgame) {
  EvalType evals = LoadEvals();
  HashMap hash_map;
  EvaluatorAlphaBeta evaluator_alpha_beta(&hash_map, PatternEvaluator::Factory(evals.data()));
  EvaluatorDerivative evaluator_derivative(&evaluator_alpha_beta);
  Board initial_board("e6f4c3c4d3d6e3c2b3c5b4f3d2c1d7c6f5c7f6e8b5e7b6g6g5h6g4h5g3h2h4h3f7f8g7f2e1d1g8e2b1b2");
  evaluator_derivative.Evaluate(initial_board.GetPlayer(), initial_board.GetOpponent(), kMinEval + 1, kMaxEval - 1, 1000000, 1);

  std::cout << evaluator_derivative.Get(initial_board)->GetEval() << "\n";

//  std::cout << evaluator_derivative.Get(initial_board)->GetEvaluation(1).ProbGreaterEqual() << "\n";
}