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
#include <limits>
#include <random>
#include "../board/board.h"
#include "endgame_time_estimator.h"

TEST(EndgameTimeEstimator, ConvertProofNumber) {
  for (int i = 0; i < 1000; ++i) {
    Board b = RandomBoard();
    Eval eval = (rand() % 64) /*0 1...63*/ * 2 /*0 2...126*/ - 63 /*-63 -61...63*/;
    Eval goal = (rand() % 64) /*0 1...63*/ * 2 /*0 2...126*/ - 63 /*-63 -61...63*/;
    Eval other_goal = std::max((int) kMinEval, goal - (rand() % 12) * 2);

    double proof_number = ProofNumber(b.Player(), b.Opponent(), EvalToEvalLarge(goal), EvalToEvalLarge(eval));
    double other_proof_number = ProofNumber(b.Player(), b.Opponent(), EvalToEvalLarge(other_goal), EvalToEvalLarge(eval));
    ASSERT_NEAR(ConvertProofNumber(proof_number, goal, other_goal),
                other_proof_number, 1E-8 * other_proof_number);
  }
}

TEST(EndgameTimeEstimator, ConvertDisproofNumber) {
  for (int i = 0; i < 1000; ++i) {
    Board b = RandomBoard();
    Eval eval = (rand() % 64) /*0 1...63*/ * 2 /*0 2...126*/ - 63 /*-63 -61...63*/;
    Eval goal = (rand() % 64) /*0 1...63*/ * 2 /*0 2...126*/ - 63 /*-63 -61...63*/;
    Eval other_goal = std::min((int) kMaxEval, goal + (rand() % 12) * 2);

    double disproof_number = DisproofNumber(b.Player(), b.Opponent(), EvalToEvalLarge(goal), EvalToEvalLarge(eval));
    double other_disproof_number = DisproofNumber(b.Player(), b.Opponent(), EvalToEvalLarge(other_goal), EvalToEvalLarge(eval));
    ASSERT_NEAR(ConvertDisproofNumber(disproof_number, goal, other_goal),
                other_disproof_number, 1E-8 * other_disproof_number);
  }
}